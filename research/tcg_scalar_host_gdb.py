"""Process-local Linux host experiment; never an Android/product dependency.

Run only through the authorized private launcher using gdb --args. This selects
the SDK's scalar TCG generator, retaining guest ARM SIMD emulation. It changes
two host data bytes after their real feature probe, not the SDK file, Android
API, guest instructions, clocks, Root, SELinux, or product admission rules.
"""
import hashlib
import json
import os
import sys
from pathlib import Path

import gdb

sys.path.insert(0, str(Path(__file__).resolve().parent))
from tcg_host_targets import select_scalar_target, validate_scalar_preload, validate_scalar_argv

EXE = Path('/opt/seedlens-lab/sdk/emulator/qemu/linux-x86_64/qemu-system-aarch64-headless')
SHA256 = 'e697effa1e3f5e0962dab1c8fdbca5787582e6081e2c22eabe74ff396fbd2436'
PROBE_END = 0xa905d5
HOST_AVX_FLAGS = 0x18de1cc
PROBE_BYTES = bytes.fromhex('803df0dbe40001')


def require(condition, message):
    if not condition:
        raise gdb.GdbError('SCALAR_HOST_REFUSED: ' + message)


try:
    target = select_scalar_target(os.environ.get('SEEDLENS_TCG_SCALAR_HOST_RESEARCH'))
    validate_scalar_preload(os.environ.get('LD_PRELOAD'),
                            os.environ.get('SEEDLENS_TCG_VECTOR_SAVE_RESEARCH'), target)
except ValueError as error:
    raise gdb.GdbError('SCALAR_HOST_REFUSED: ' + str(error)) from None
require(Path(gdb.current_progspace().filename).resolve() == EXE, 'wrong executable')
require(hashlib.sha256(EXE.read_bytes()).hexdigest() == SHA256, 'SDK hash mismatch')
require(hashlib.sha256(target.mapper.read_bytes()).hexdigest() == target.mapper_sha256,
        'mapper hash mismatch')
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
    validate_scalar_argv(argv, target)
except ValueError as error:
    raise gdb.GdbError('SCALAR_HOST_REFUSED: ' + str(error)) from None
maps = gdb.execute('info proc mappings', to_string=True)
matches = [line for line in maps.splitlines()
           if str(EXE) in line and line.split()[3] == '0x0']
require(len(matches) == 1, 'ambiguous ELF base')
base = int(matches[0].split()[0], 16)
require(inferior.read_memory(base + PROBE_END, len(PROBE_BYTES)).tobytes() == PROBE_BYTES,
        'probe instruction mismatch')
applied = False


class ScalarGenerator(gdb.Breakpoint):
    def stop(self):
        global applied
        before = inferior.read_memory(base + HOST_AVX_FLAGS, 2).tobytes()
        require(before == bytes([1, 1]), 'expected real host AVX1/AVX2 probe')
        inferior.write_memory(base + HOST_AVX_FLAGS, b'\0\0')
        after = inferior.read_memory(base + HOST_AVX_FLAGS, 2).tobytes()
        require(after == b'\0\0', 'host policy write failed')
        applied = True
        print('SEED_TCG_SCALAR_HOST_COMPARISON', json.dumps({
            'before': list(before), 'after': list(after),
            'avd': target.avd.decode('ascii'),
            'port': target.port.decode('ascii'),
            'guestCoresRequired': target.required_guest_cores,
            'vectorSaveWrappersRegistered': False,
            'sdkFileModified': False, 'guestApiModified': False,
            'hardwareQualification': False, 'productQualification': False,
        }), flush=True)
        return False


ScalarGenerator('*' + hex(base + PROBE_END), internal=True, temporary=True)
exits = []
gdb.events.exited.connect(lambda event: exits.append(getattr(event, 'exit_code', None)))
for stops in range(10000):
    gdb.execute('continue')
    if gdb.selected_inferior().pid == 0:
        break
    # Keep host faults observable, then pass the actual signal. A stop alone
    # is not a fatal exit, and debugger exit0 is not Android qualification.
    print('SEED_SCALAR_HOST_STOP', gdb.execute('info program', to_string=True), flush=True)
    for inspect in ('x/i $pc', 'info registers', 'bt 12'):
        try:
            gdb.execute(inspect)
        except gdb.error as error:
            print('SEED_SCALAR_INSPECT_ERROR', str(error), flush=True)
else:
    print('SEED_SCALAR_STOP_LIMIT', flush=True)
    gdb.execute('kill')
print('SEED_TCG_SCALAR_FINISHED', json.dumps({
    'applied': applied, 'inferiorExitEvents': exits,
    'debuggerExitIsInferiorExit': False, 'productQualification': False,
}), flush=True)
require(applied, 'TCG host policy was never selected')
