# Own ARM64 signal-context observation

This is a separate laboratory package, `dev.ody.seedlensarmsignalprobe` 0.0.3,
not a product component or an update to `arm-hint-probe`. The original
unhandled-SEVL baseline and all failed game starts remain unchanged.

The question is whether the existing API31/API34 NativeBridge delivers an
ordinary ARM64 App's SIGILL handler with public ARM64 ucontext fields matching
that App's known instruction and callee-saved register markers. This is an
experiment, not a presumed capability or a game-startup workaround.

## Scope and failure handling

- No Root, network permission, ptrace, other-process access, bridge replacement,
  game loading, instruction rewriting or product gate changes.
- Seven explicit modes: `baseline` (common signal/alarm setup), `raise` (synthetic
  SIGILL control), `udf` (own undefined instruction), `sevl`, `pair` (SEVL/WFE),
  `wfe` (WFE alone), and `alarm` (intentional own-process timeout control).
- Each invocation needs a fresh own App process, exact expected API31/34/35 and
  ordinary App UID. Java also reads its own `/proc/self/exe` ELF header and
  requires explicit `expected_machine=62` (x64) or `183` (ARM64); uname/ABI labels
  cannot substitute for that evidence. It uses the existing pure `ProcessElf`
  reader and 94 host checks with only a generated package relocation; no Root
  endpoint or runner is packaged. Java writes fresh private durable phase receipts. Native
  refuses nonempty, non-0600, wrong-owner, linked or non-append record files,
  and a second call in the same process.
- Native uses the NDK's `ucontext_t` fields, never guessed byte offsets. It does
  not dereference saved PC, write context or resume after a signal. Lock-free
  state, `write`, `fsync` and `_exit` are the only handler support operations.
  A successful fault record deliberately exits the own process with 86; it
  must not be reported as instruction return or game success.
- A three-second default SIGALRM bounds the own case. A host run must also use
  a finite observation deadline, preserve crash/partial evidence and never
  replay an uncertain invocation. A different process may be used for the next
  explicitly selected case after the prior process is confirmed terminal.

Build with `build.ps1`, explicit SDK/JDK17/NDK27/Python paths, the external lab
keystore, its expected certificate SHA256 and password environment names. It
runs host parser/refusal checks and verifies signature, version, stored ARM64
library and 16KiB alignment. Output is immutable per version; changed built
bytes need a higher version/code. Build success is not device qualification.

After fresh target identity, ordinary shell UID and original platform/library
hash verification on an already authorized isolated guest, run one mode:

```
adb -s <exact-authorized-serial> shell am instrument -w -e expected_api <31-or-34-or-35> -e expected_machine <62-or-183> -e mode <baseline-or-raise-or-udf-or-sevl-or-pair-or-wfe-or-alarm> dev.ody.seedlensarmsignalprobe/.SignalInstrumentation
```

Read back only this App's private `.tsv` and `.bin` records after the own
process is terminal; any diagnostic adbd Root must be restored to UID2000.
Use `read_records.py` with exact API/mode/PID/UID from that invocation.
Missing/truncated/out-of-order/cross-process records are not proof. Synthetic
signals cannot prove fault-context delivery. A fault match additionally needs
known own PC, signal address, hardware instruction signal code, two register
markers and stack alignment. A mismatch stays observed-but-unproven.
No parser outcome grants a game/platform qualification or changes product
admission. `sevl`/`pair`/`wfe` normal return proves only that own instruction case.
The `alarm` control deliberately waits in its own process after arming the
three-second default SIGALRM. Its two native phase records are merely
`timeout-control-armed-unconfirmed`; the device study must independently verify
the exact own PID's termination by SIGALRM. Missing records, instrumentation
failure or an ADB return code cannot supply that evidence. No App signal handler
is added for SIGALRM, and no PC/context is modified.

## Source basis and pending evidence

The local NDK27 `sys/ucontext.h` defines the ARM64 named fields used here.
The Android kernel's [ARM64 hwcap self-test](https://android.googlesource.com/kernel/common/+/833b0f07b915f40db352063df0e13bc91fb0f42f/tools/testing/selftests/arm64/abi/hwcap.c)
uses a SIGILL/ucontext handler for its own instruction tests. That native
kernel example does not prove NativeBridge delivery; this probe deliberately
only records and exits instead of changing PC as the self-test does.
Arm's [A64 instruction reference](https://documentation-service.arm.com/static/67e40f3398aa3c3b6eea6a85)
describes SEVL's local-event role; no NOP substitution is inferred from it.
The older [AOSP NativeBridge interface](https://android.googlesource.com/platform/system/core/+/96d6745/libnativebridge/include/nativebridge/native_bridge.h)
describes bridge-managed signal handlers, not a guarantee of translated App
SIGILL context behavior on either image. Do not transfer other architectures,
ROMs or bridge implementations into the result.

0.0.1 was signed and retained with its source before adding the own-process ELF
requirement. Its APK SHA256 is
`829f0154f140c61c6a28e33f941befef14902cd723f388249b836960f06884a0`;
it was never installed. Changed Java/manifest bytes use new 0.0.2/code2.
Signing, installation and all ordinary App cases must be recorded separately.
Product remains 0.5.2 and API35/libnb-only for x64.

0.0.2 host signing/packaging passed with original lab certificate
`633286a6f46aae1d6b4bcf26ed2278e72c73bde5046cfa02b7f69fd347ac8870`.
APK SHA256:
`89755ce2c38d26f9412720b027c884093adbc21636b026f5c30d2c3e06151bc9`.
ARM64 SO SHA256:
`9261238662a335523f8fd96b3d66818eb96b0f25ba0a187b9b52aaf2ee2ad6fc`
(same as retained 0.0.1 native bytes). The 94 ELF JVM checks, Android Java/dex,
manifest/version, original signer, ARM64-only stored SO and 16KiB alignment
passed. Ten host parser test methods additionally cover truncated/mixed
identity, forged fault-context matches and synthetic-signal misclassification;
these generated fixtures are not device receipts. Disassembly confirms the
three own fault labels, register markers, UDF, SEVL and SEVL/WFE bytes.
Neither version has been installed at this preparation checkpoint.

Subsequent API35 ordinary-App control results are recorded separately in
[signal context research](../../docs/ARM_SIGNAL_CONTEXT_RESEARCH.md). That run
installed only 0.0.2. Its UDF record matched own PC/register markers but not
instruction signal code/address, and remains context-unproven. It grants no
API31/API34 or product qualification.

The same 0.0.2 APK subsequently ran on the existing API34 x64 guest. Baseline
returned, while UDF/SEVL/pair produced own handler records and independently
verified exit86. Own PC/register-marker/aligned-SP observations matched; signal
code/address did not, and remain context-unproven. Pair stopped at SEVL before
WFE. The original game was never launched. The detailed research page records
this separately from API35 and all product qualification. API31 is still pending.

The same APK also completed the independent API31 x64 comparison: baseline
returned, and raise/UDF/SEVL/pair recorded faults followed by independently
verified own exit86. Instruction context remains unproven for the same failed
signal-code/address checks; pair never reached WFE. Original product/game bytes
and all sixteen research journals were retained, and the guest was normally
closed. Neither WFE alone nor an actually triggered alarm timeout has yet been
tested by 0.0.2. No product qualification follows from these results.

0.0.3/code3 adds only the independent WFE instruction and intentional alarm
control; original 0.0.1/0.0.2 artifacts remain immutable. Its host build passed
12 parser test methods and the same 94 ELF JVM checks, native/Java/dex/version,
original lab signer and stored ARM64/16KiB packaging. Disassembly separately
confirms `own_wfe_fault` contains WFE followed by the register restore/return.
APK SHA256:
`542655236bdf1661a30636a19add51eb49ef192b805e66aca9d4955a7160b886`.
SO SHA256:
`ed0dc066e70b4b689d93d5aaa57c37a01540bdabad75be8e560f0b684cbaa767`.
At this preparation checkpoint 0.0.3 has **not been installed or run**. These
host checks do not prove WFE execution, timeout containment or product support.

The subsequent API31 0.0.3 study independently executed WFE alone (own handler
fault and Zygote exit86), and the intentional alarm control (native phases 1/2
only, independently verified own Zygote signal14 termination). The parser still
calls the latter armed-unconfirmed; its records alone never prove a timeout.
All fifteen real 0.0.2 decoded results also stayed exactly identical under the
extended parser. See the research page for fresh boot identity, retained
receipts and qualification limits. API34/API35 0.0.3 cases remain pending.

The independent API34 0.0.3 study subsequently reproduced the standalone WFE
fault with own Zygote exit86, and separately verified the intentional alarm
case's own signal14 termination. The original 0.0.2 receipts and game bytes
were retained, product remained absent, and no context or bridge was changed.
These are independent API34 results; API35 0.0.3 is still pending.
