# Scalar TCG host experiment

This is an explicit modification of a disposable Linux host process. It is not
an Android module, product dependency, stock-emulator or physical ARM64 result.
Never copy it into an APK, attach it to an existing process, or enable it
globally. Authorization and fresh lab identity must be checked independently.

The original approved target is `SeedLens_API31_ARM64_Linux`, software CPU, port5586,
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

## Separately scoped API34 AOSP comparison, 2026-11-10

The existing owner-approved `SeedLens_API34_AOSP_ARM64_Linux` has a separate
scalar marker, `seedlens-api34-aosp-tcg-single-scalar-v1`. It fixes port5590,
two guest CPUs (`-cores 2` before `-qemu`, `-smp 2` after it), and the backend
`-accel tcg,thread=single`. This does not authorize the Google-APIs AVD or
transfer API31 results. The existing AOSP single-thread mapper is retained:
SHA256 `f4b738fdafbffdb3c8f0aec7c0e16e5da6234a33e9fd19ba978452f937ceab88`.

Scalar and vector markers have separate target namespaces. The scalar
controller requires exactly its pinned mapper as `LD_PRELOAD`, verifies its
hash, rejects any vector marker/preload, and checks frontend/backend parser
scope. It requires the real AVX probe bytes to read1/1 before clearing0/0;
no vector-save wrappers are registered. The legacy API31 tuple is retained,
with the stronger preload/hash/probe checks; no new API31 run is inferred.

Windows and dedicated-Linux target tests passed21 methods, including eight
new scalar methods and cross-target, CPU-count, parser-scope and preload
refusals. Eight actual GDB negative cases explicitly refused before `starti`,
with inferior PID0. The initial private reader incorrectly expected a
nonzero debugger exit despite a subsequent successful PID-report command;
that reader failure remains recorded. Corrected checks retain actual GDB
exit0 and require the explicit refusal, PID0 and absent comparison marker.
Debugger status alone proves neither refusal nor guest success.

After fresh target/resource/SDK-image checks and normal closure of the sole
new API35 instance, the original V3c fixture ran at UTC14:54:03.797–14:54:12.557.
The API34 AOSP kernel actually brought up two CPUs. All seven SEVL/vector/
tagged-memory/adjacent-sequence cases passed, zero failed/skipped; the deliberate
NULL SIGSEGV and child-reaped control passed. Guest power-off and actual
inferior exit0 were recorded separately from GDB0, without timeout or
containment. Existing SDK warnings remain. This is CPU-initramfs evidence,
not Android boot, normal App Root, JNI or game qualification.

The first Android launch stopped before the kernel because its private
diagnostic UNIX socket path exceeded108 bytes; actual inferior/GDB exit0
is **not** an Android pass. Logs remain unchanged. A shorter private run
path fixed that launcher error. A separate full-Android run began at
UTC15:01:02.236 with a fixed1800s bound and normal-close timer at1660s,
2 CPUs/4GiB, existing data, no animation and the separately pinned AOSP
worker1 ramdisk. At launch there was no completed-boot/Home/Root proof.
Current Power/WMS boot state, PM/AM and unchanged server identity must pass
twice60s apart, followed by actual Home/interaction, before later deployment.
See [AOSP evidence](../docs/API34_AOSP_ARM_LINUX_RESEARCH.md).
