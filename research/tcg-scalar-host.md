# Scalar TCG host experiment

This is an explicit modification of a disposable Linux host process. It is not
an Android module, product dependency, stock-emulator or physical ARM64 result.
Never copy it into an APK, attach it to an existing process, or enable it
globally. Authorization and fresh lab identity must be checked independently.

The approved target remains `SeedLens_API31_ARM64_Linux`, software CPU, port5586,
under `/opt/seedlens-lab`. Use `tcg_scalar_host_gdb.py` only with `gdb --args`
through the private launcher. Set only that child's
`SEEDLENS_TCG_SCALAR_HOST_RESEARCH=seedlens-api31-tcg-scalar-host-v1`.
The existing [software-mapper helper](tcg-software-selection.md) is a separate
host experiment; both changes must be named in any combined result.

The script pins the SDK ELF SHA256, executable path, runtime executable, single
AVD/port arguments, explicit software CPU and the probe instruction bytes. It
starts only that child, verifies its ASLR base, and uses one temporary breakpoint
at the end of the real host feature probe. It then clears two host TCG AVX
generator-selection flags in that process's data memory. No SDK file is patched.
It does not change ARM guest instructions, SIMD hardware claims, Android API,
clock, Root, SELinux, game data, timeout contracts or product admission rules.

The [fixed source](https://android.googlesource.com/platform/external/qemu/+/4f8bb7a074c4cd727acfdfa1e7bb092a2480696c/tcg/i386/tcg-target.inc.c)
is a structural comparison, not proof of the complete SDK build source. Actual
ELF disassembly identifies the two flags and their generator-selection reads.
Runtime observation of an own CPU fixture's slow stack store showed XMM0
changing from guest X0 to the later fault address across that helper. The scalar
comparison avoids that host vector-generator path while retaining emulation of
the same guest vector instructions.

Actual tests: the original V2 bytes passed7/7 under normal multi-instruction TB,
with normal power-off/inferior exit0; no single-step flag. The final V3 diagnostic
also passed7/7 plus its intentionally fatal NULL-read/reap control. With only
scalar selection removed, that same V3 retained5 pass/2 SIGSEGV fail. Wrong
marker and wrong executable each refused with exit1 before starting a guest.
These results do not qualify framework startup, ordinary App `su -M`, JNI,
game UI, long-running behavior or physical hardware.

Keep watchdog/pidfd containment outside the debugger. Pass host signals onward;
record actual inferior exit separately from debugger exit. The full Android
comparison must be recorded separately in [the Linux evidence](../docs/API31_LINUX_HOST_RESEARCH.md).
