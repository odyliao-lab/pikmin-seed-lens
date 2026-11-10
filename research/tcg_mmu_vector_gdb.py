"""Explicit process-local SDK MMU vector-save research, never an APK dependency.

Only a newly launched, independently authorized/pinned Linux AVD may use
this controller. It registers guarded host wrappers in this child's MMU tables;
it does not patch SDK files, guest instructions, API, clocks or security policy.
"""
import hashlib
import json
import os
import sys
from pathlib import Path

import gdb

sys.path.insert(0,str(Path(__file__).resolve().parent))
from tcg_host_targets import select_target, validate_target_argv

EXE = Path('/opt/seedlens-lab/sdk/emulator/qemu/linux-x86_64/qemu-system-aarch64-headless')
SHA256 = 'e697effa1e3f5e0962dab1c8fdbca5787582e6081e2c22eabe74ff396fbd2436'
PROBE_END = 0xa905d5
HOST_AVX_FLAGS = 0x18de1cc
PROBE_BYTES = bytes.fromhex('803df0dbe40001')


def require(condition, message):
    if not condition:
        raise gdb.GdbError('VECTOR_SAVE_REFUSED: ' + message)


try:
    target=select_target(os.environ.get('SEEDLENS_TCG_VECTOR_SAVE_RESEARCH'))
except ValueError as error:
    raise gdb.GdbError('VECTOR_SAVE_REFUSED: '+str(error)) from None
require(Path(gdb.current_progspace().filename).resolve() == EXE, 'wrong executable')
require(hashlib.sha256(EXE.read_bytes()).hexdigest() == SHA256, 'SDK hash mismatch')
HELPER=Path('/opt/seedlens-lab/host-mmu-save-v1/libseed-tcg-mmu-save.so')
require(hashlib.sha256(HELPER.read_bytes()).hexdigest()=='823f6e1826a353ad38a91e7652ec46132c69c349bf6bcc28d1399c512323508c','helper hash mismatch')
require(hashlib.sha256(target.mapper.read_bytes()).hexdigest()==target.mapper_sha256,'mapper hash mismatch')
require(set(os.environ.get('LD_PRELOAD','').split(':'))=={str(HELPER),str(target.mapper)},'wrong preload scope')
gdb.execute('set pagination off')
gdb.execute('set confirm off')
gdb.execute('set debuginfod enabled off')
gdb.execute('handle SIGUSR1 nostop noprint pass')
gdb.execute('handle SIGPIPE nostop noprint pass')
gdb.execute('handle SIGSEGV stop print pass')
gdb.execute('handle SIGABRT stop print pass')
# Starts only this executable at its first instruction, not an external PID.
gdb.execute('starti')
inferior = gdb.selected_inferior()
proc = Path('/proc') / str(inferior.pid)
require(proc.joinpath('exe').resolve() == EXE, 'runtime executable changed')
argv = proc.joinpath('cmdline').read_bytes().rstrip(b'\0').split(b'\0')
try:
    validate_target_argv(argv,target)
except ValueError as error:
    raise gdb.GdbError('VECTOR_SAVE_REFUSED: '+str(error)) from None
maps = gdb.execute('info proc mappings', to_string=True)
matches = [line for line in maps.splitlines()
           if str(EXE) in line and line.split()[3] == '0x0']
require(len(matches) == 1, 'ambiguous ELF base')
base = int(matches[0].split()[0], 16)
require(inferior.read_memory(base + PROBE_END, len(PROBE_BYTES)).tobytes() == PROBE_BYTES,
        'probe instruction mismatch')
applied = False


class VectorSave(gdb.Breakpoint):
    def stop(self):
        global applied
        flags=inferior.read_memory(base+HOST_AVX_FLAGS,2).tobytes()
        require(flags==bytes([1,1]),'expected real host AVX1/AVX2 probe')
        offsets=[0x86ae20,0x86b510,0x86c510,0x86d510,0,0,0,0,0,0x86b890,0x86c880,0x86d8b0,0,0,0,0,0x86b1c0,0x86bc40,0x86cc20,0x86dc40,0,0,0,0,0,0x86c090,0x86d090,0x86e120,0,0,0,0]
        import struct
        expected=[base+x if x else 0 for x in offsets]
        table=base+0x16cdc30
        before=list(struct.unpack('<32Q',inferior.read_memory(table,256).tobytes()))
        require(before==expected,'unexpected SDK MMU function table')
        originals=int(gdb.parse_and_eval('(void*)&seed_mmu_originals'))
        wrappers_addr=int(gdb.parse_and_eval('(void*)&seed_mmu_wrappers'))
        require(inferior.read_memory(originals,256).tobytes()==bytes(256),'helper already initialized')
        wrappers=list(struct.unpack('<32Q',inferior.read_memory(wrappers_addr,256).tobytes()))
        mappings=gdb.execute('info proc mappings',to_string=True)
        ranges=[(int(row.split()[0],16),int(row.split()[1],16)) for row in mappings.splitlines() if str(HELPER) in row and 'r-x' in row]
        require(ranges and all(any(lo<=x<hi for lo,hi in ranges) for x in wrappers),'wrong wrapper addresses')
        inferior.write_memory(originals,struct.pack('<32Q',*before))
        require(inferior.read_memory(originals,256).tobytes()==struct.pack('<32Q',*before),'originals write failed')
        after=[wrappers[i] if x else 0 for i,x in enumerate(before)]
        inferior.write_memory(table,struct.pack('<32Q',*after))
        require(inferior.read_memory(table,256).tobytes()==struct.pack('<32Q',*after),'table registration failed')
        applied=True
        print('SEED_TCG_VECTOR_SAVE_REGISTERED',json.dumps({'wrappers':sum(bool(x) for x in before),'hostAvxFlagsUnchanged':list(flags),'sdkFileModified':False,'guestApiModified':False,'productQualification':False}),flush=True)
        return False


VectorSave('*' + hex(base + PROBE_END), internal=True, temporary=True)
exits = []
gdb.events.exited.connect(lambda event: exits.append(getattr(event, 'exit_code', None)))
for stops in range(10000):
    gdb.execute('continue')
    if gdb.selected_inferior().pid == 0:
        break
    # Keep host faults observable, then pass the actual signal. A stop alone
    # is not a fatal exit, and debugger exit0 is not Android qualification.
    print('SEED_VECTOR_HOST_STOP', gdb.execute('info program', to_string=True), flush=True)
    for inspect in ('x/i $pc', 'info registers', 'bt 12'):
        try:
            gdb.execute(inspect)
        except gdb.error as error:
            print('SEED_VECTOR_INSPECT_ERROR', str(error), flush=True)
else:
    print('SEED_VECTOR_STOP_LIMIT', flush=True)
    gdb.execute('kill')
print('SEED_TCG_VECTOR_SAVE_FINISHED', json.dumps({
    'applied': applied, 'inferiorExitEvents': exits,
    'debuggerExitIsInferiorExit': False, 'productQualification': False,
}), flush=True)
require(applied, 'TCG host policy was never selected')
