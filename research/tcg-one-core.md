# API34 AOSP: one guest CPU scheduling comparison

Host research only, on the existing authorized AOSP AVD/5590 target. This
reduces guest CPU allocation from2 to1 within the existing lab allocation;
it creates no new device and changes no host settings. The two-CPU boot's
current Power/Window completion checks remained unsuccessful despite global
flag1. Its final ordinary-shell sync read UID2000/Enforcing/API34/ARM64 and
the same boot. It closed normally UTC11:33:33; actual QEMU/gdb0 at11:33:35,
no timeout/containment. All diagnostics and the insufficient PM/AM pair remain.

The scheduling hypothesis is that SMP guest work on single-thread TCG adds
avoidable cost. It is not an established root cause. Before guest execution,
a new exact marker/helper/profile was added, never a relaxed old predicate:

- Mapper marker `seedlens-api34-aosp-tcg-onecore-mapper-v1`.
- Vector marker `seedlens-api34-aosp-tcg-onecore-vector-v1`.
- New mapper SHA256
  `59b63525581cbde87ce018bf65483e010441379a1f126b434bc4e50038f1643f`.
- Exactly one frontend `-cores 1` before `-qemu` and one backend `-smp 1`
  after it, plus the existing exact `-accel tcg,thread=single` boundary.
- Fixed AOSP AVD/5590, pinned original SDK executable/vector helper and real
  host feature probes. Wrong/missing/ambiguous CPU values or parser scopes
  refuse. The build requires all prior explicit API34/AOSP/single scopes.

Actual C guard cases passed13 default,23 Google-single,26 AOSP-single and35
new one-core cases. Python passed11 methods. Missing compile/build scopes
refused; real GDB wrong-executable/preload cases refused before starti.
All four prior mapper variants rebuilt byte-identically to their previous
hashes. No product, NativeBridge, game, Root or timeout policy was altered.

Only after normal exit/no-QEMU checks, config.ini was retained and changed
at one line (`hw.cpu.ncore=2` to1).4GiB/1080×1920/420dpi, SDK/data files and
the preceding caches remain. A new read-only V3c CPU run used the original
fixture/kernel, UTC11:34:38.445–11:34:43.200. Kernel actually reported one CPU;
all7 tests and the intentional NULL/child-reaped control passed. Fourteen
vector wrappers registered, AVX flags1/1 unchanged, normal power-off and
actual inferior/gdb0, no timeout/containment. Cache/load differ from earlier
runs; do not turn the short duration into a causal speedup benchmark.

The subsequent full-Android comparison retains the existing AOSP-only
worker1 ramdisk7b5da5b6…31d4 and no-animation option. Its separate fixed bound
is chosen before launch. Actual new boot/API/ABI/CPU-online count/Enforcing,
current Power+Window boot state, PM/AM and working home must be verified.
There is no transfer of the CPU result into App Root, JNI, game or product
qualification. The three delivery stages remain incomplete.
