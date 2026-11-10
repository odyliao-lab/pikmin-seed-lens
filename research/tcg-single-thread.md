# Separate API34 host scheduling comparison

Research preparation only. No guest has run this mode at this checkpoint.
The active API34 boot continues with its original scheduling; this source
change does not alter any running process, SDK file, Android image or APK.

During that boot, three host-only samples covered about40 seconds. GDB CPU
counters stayed at88 user/21 system ticks (100Hz); the two QEMU vCPU threads
each accumulated about13,000 voluntary context switches per second. The host
allowed CPU set contained16 CPUs. This suggests a scheduling comparison is
worth testing; it does not identify a particular lock or prove the source of
slow Trichrome decompression. It does not exclude all tracing overhead.

[Upstream QEMU documentation](https://www.qemu.org/docs/master/devel/multi-thread-tcg.html)
describes `-accel tcg,thread=single` as round-robin emulation of multiple guest
CPUs. That document is not proof this pinned Android SDK backend accepts or
correctly executes the option. Actual SDK CPU tests remain mandatory.

This experiment retains **2 guest vCPUs and4GiB**; it is not a one-vCPU or
single-instruction-TB experiment. The existing vector-preservation wrapper
and all guest CPU, clock and security settings remain unchanged. There is no
API/game/NativeBridge or product timeout override.

The separate mapper is built only with both
`SEEDLENS_API34_HOST_RESEARCH` and `SEEDLENS_API34_TCG_SINGLE_RESEARCH`.
The latter alone is a compile error. PowerShell requires both
`-Api34Research -Api34SingleThreadResearch` before reserving an output.

| Fixed field | Required value |
|---|---|
| AVD / port | `SeedLens_API34_ARM64_Linux` / `5588` |
| Mapper marker | `seedlens-api34-tcg-single-mapper-v1` |
| Vector/controller marker | `seedlens-api34-tcg-single-vector-v1` |
| Mapper filename | `libseedlens-api34-tcg-single-selection-v1.so` |
| Mapper SHA256 | `0aa0e6e0f23048592ac113e14fdfb0f7c6673268e971af46e5ac1839b8f1122e` |
| Mapper directory | `/opt/seedlens-lab/host-helper-api34-single-v1` |
| Frontend software request | Exactly one `-no-accel` |
| Backend request | Exactly one `-qemu`, immediately followed by `-accel tcg,thread=single` |

Both C mapper and Python controller refuse the old API34 marker, missing or
different scheduling, frontend accelerator selection, duplicated delimiter/
accelerator, wrong AVD/port/executable and hardware acceleration. Existing
API34 mode still refuses every `-accel` override. Neither profile grants
device authorization. Both still require the separately verified original
SDK executable, pinned mapper/vector preload set and real AVX/MMU table
checks in the controller. The mapper never fabricates a passed CPU test.

Actual host checks completed:

- Original C guard13/13 and separate single-scheduling guard23/23, Linux exit0.
- Python target policy8 test methods, on both Windows and Linux.
- Real GDB wrong-executable and wrong-preload refusals before `starti`.
- Real C compile and PowerShell build refusals when API34 scope is omitted.
- Original API31 mapper rebuild remains `bc30eb39…e1e795`; original API34
  mapper rebuild remains `1b702320…0125a7`. Live helper files were not replaced.
- Two unique builds of the new mapper produced the same SHA above; the Linux
  staged copy matches. No module, APK, guest install or live registration.

Before any actual use, wait for the current guest's confirmed normal close
and preserved data; recheck authorization, fresh target identity, SDK hashes,
ports and host memory. Run the existing self-contained V2/V3c CPU tests and
intentional-fault/reap control under a pidfd-bound external deadline first.
Only a complete passing result can justify a subsequent full-Android
scheduling comparison. Keep failure/UNKNOWN attempts and actual inferior
exit evidence. Do not transfer CPU or host results into ordinary-App Root,
JNI, game, UI, OS restart or endurance qualifications. If the original mode
becomes sufficient, this prepared mode need not be deployed.
