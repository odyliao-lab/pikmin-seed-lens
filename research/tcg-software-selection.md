# Linux host software-selection experiment

Research source only. Do not package this Linux x64 library into an APK, install
it in Android, or enable it globally. It selects an existing SDK software-mapper
branch for the already software-emulated, explicitly authorized disposable AVD.
It does not establish Android, Root, JNI, NativeBridge or game qualification.

The approved target is `SeedLens_API31_ARM64_Linux` under `/opt/seedlens-lab`,
SDK emulator37.2.13/build16740729. Current host QEMU SHA256 is
`e697effa1e3f5e0962dab1c8fdbca5787582e6081e2c22eabe74ff396fbd2436`.
Always independently verify authorization, target identity, image, ports and
resource headroom before launching it. The helper's checks grant no permission
to operate another device or process.

Build with PowerShell7 and an explicit NDK27.3.13750724 path:

```powershell
pwsh -NoProfile -File research/build-tcg-software-selection.ps1 -NdkPath <ndk-r27d>
```

This cross-compiles a Linux System V x86_64 shared library with no Android
runtime or C++ runtime dependency. Its libc imports resolve in the Linux
host. It is not an Android native test or product build. Each output directory
is unique; preserve prior artifacts and record the hash before copying into
the private Linux lab. The initial 5,448-byte artifact SHA256 is
`bc30eb3904692cd475cef5eda8d0127979b57b2451f451434542a39f28e1e795`.

Only a separately scoped child-process environment may set `LD_PRELOAD` to that
verified library and `SEEDLENS_TCG_SOFTWARE_ONLY` to
`seedlens-tcg-mapper-selection-v1`. Do not change a shell profile or global
loader configuration. The research launcher must verify the SDK/helper hashes
and refuse an existing unrelated preload. Keep the original SDK, ramdisk and
guest SELinux unchanged.

The interposed public SDK `GetCurrentCpuAccelerator()` first invokes the real
host probe, retaining its initialization/status. It selects enum NONE only
when the marker, canonical QEMU executable, AVD argument and explicit software
flag match. Other executable/marker cases delegate to the real function;
failure to locate that function exits86. This is an explicit host test override
of the mapper decision. All results must name this modified host configuration,
not attribute them to an unmodified emulator or physical ARM64 hardware.

Actual initial tests: wrong marker retained the constant-failure mapper; the
correct target selected the real SDK TCG mapper and V2 CPU fixture passed7/7,
normal power-off/exit0. A different Linux executable retained its real enum1;
a missing original function actually exited86. Default multi-instruction TCG
still produced5 pass/2 SIGSEGV fail under the helper, so it does not fix that
separate CPU problem. Keep the single-instruction comparison explicit.

Full Android boot, actual API/ABI, ordinary App Root, CPU context, JNI lifecycle,
failure containment and game UI remain separate acceptance layers. See
[the Linux evidence](../docs/API31_LINUX_HOST_RESEARCH.md).

## Separately authorized API34 target

`-Api34Research` creates `libseedlens-api34-tcg-selection-v1.so`, SHA256
`1b702320855a24203f8fb6b081050965566617fae7630758998b6803e60125a7`.
It admits only `SeedLens_API34_ARM64_Linux`, one `-port 5588`, one explicit
`-no-accel`, and marker `seedlens-api34-tcg-mapper-selection-v1`; duplicate
target options and competing accelerator options refuse selection. It first
calls the original host probe just as the old helper does. The default build
still reproduces the old API31 helper bytes; old artifacts remain immutable.

The isolated `tcg_software_selection_test.c` fixture mocks only its own `/proc`
reader, never the SDK or guest, and exercises13 production-guard cases. All13
actually passed on the dedicated Linux host, including a forked exit86 case.
This is a guard unit test, not a QEMU, Android, Root or game acceptance claim.
Actual API34 source/platform evidence is tracked [separately](../docs/API34_ARM_LINUX_RESEARCH.md).
