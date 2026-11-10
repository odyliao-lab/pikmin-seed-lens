# Linux host MMU vector-preservation experiment

This explicitly changes the disposable QEMU host process. It is never an
Android library, Root/display module, APK or end-user dependency. Do not attach
it to existing processes or enable it globally. Independently verify current
authorization, AVD/image identity, original SDK hashes, ports and resources.
Fixed targets are `SeedLens_API31_ARM64_Linux`/5586 and the separately authorized
`SeedLens_API34_ARM64_Linux`/5588 with software CPU in `/opt/seedlens-lab`.
Each has its own marker and pinned mapper in `tcg_host_targets.py`; neither
entry grants authorization. It is not stock-emulator or physical ARM64 evidence.

The earlier scalar-host comparison avoided the observed MMU-helper XMM0
clobber but did not complete framework startup or normal ELF loading. This
alternative retains the SDK's real host AVX generator selection. A Linux x64
assembly wrapper preserves YMM0–15 (including their XMM portions) and MXCSR
around the original integer-returning/void MMU helpers. Integer arguments,
return and SysV stack alignment are retained. No guest instruction or guest
CPU state is rewritten. This does not claim arbitrary AVX512/x87 preservation.

Build with PowerShell7 and explicit NDK27.3.13750724:

```powershell
pwsh -NoProfile -File research/build-tcg-mmu-vector.ps1 -NdkPath <ndk-r27d>
```

The two outputs are Linux host files, not Android executables. Build directories
are unique. Initial helper SHA256 is
`823f6e1826a353ad38a91e7652ec46132c69c349bf6bcc28d1399c512323508c`.
Before actual use, verify both Linux copies and the owning SDK executable.
The standalone host fixture deliberately clobbers every YMM register and
MXCSR while checking weighted arguments, integer result and stack alignment;
it actually returned0/PASS. With an extra argument, it calls an uninitialized
wrapper: actual SIGILL/exit-4, with core dumps disabled by the test harness.
An uninitialized wrapper cannot fabricate a successful memory operation.

The private launcher sets only that child's `LD_PRELOAD` to the verified
[mapper helper](tcg-software-selection.md) and
`/opt/seedlens-lab/host-mmu-save-v1/libseed-tcg-mmu-save.so`, and marker
`SEEDLENS_TCG_VECTOR_SAVE_RESEARCH=seedlens-api31-tcg-vector-save-v1`.
It runs `gdb --args` with `tcg_mmu_vector_gdb.py`; never combine scalar-host
selection with this experiment. There are no constructors or automatic hooks
in the vector library. Merely preloading it does not register any wrapper.

The controller requires the marker, exact executable/SHA, unique AVD/port,
explicit software CPU, expected preload set and probe instruction bytes. After
the genuine host feature probe, it requires flags1/1 unchanged, validates all32
original MMU-table slots against this exact ELF, and verifies every wrapper
address is in the helper's executable mapping. It initializes the helper's
original-pointer array before replacing the14 non-null SDK table entries in
**this process's data memory**. Null slots remain null. This is a guarded host
runtime modification; no SDK file, Android image, API, clock, SELinux, Root,
game, NativeBridge or product admission rule is modified.

Actual initial guest tests: original V2/normal TB7/7, then final V3 diagnostic
7/7 plus its intentional NULL-fatal/reap control; each normal power-off and
inferior exit0, no containment signal. Real guest faults remain observable.
Wrong marker and wrong executable refused with exit1 before starting a guest.
No Android framework, ordinary App Root, JNI or game success follows from
these CPU results. Keep the external deadline/pidfd watchdog and record actual
inferior exits separately from debugger exit. See [full evidence](../docs/API31_LINUX_HOST_RESEARCH.md).

API34 additionally requires marker `seedlens-api34-tcg-vector-save-v1` and the
API34 mapper's exact path/SHA/preload tuple; it cannot use an API31 AVD or port.
The common target policy has5 passing test methods, and6 real gdb refusal
cases covered missing/wrong markers, both targets' wrong executable and wrong
preload before starti. This does not claim actual API34 CPU/Android acceptance;
see [the API34 research](../docs/API34_ARM_LINUX_RESEARCH.md).

A separate, default-disabled [API34 scheduling comparison](tcg-single-thread.md)
adds its own mapper/vector markers and SHA-pinned helper. It alone admits the
exact backend-only `-accel tcg,thread=single` form; existing markers retain
their refusal. This keeps2 guest vCPUs and is not single-instruction stepping.
Host guards pass, but the new scheduling mode has not run a guest at this
checkpoint. Its presence does not change any existing qualification.
