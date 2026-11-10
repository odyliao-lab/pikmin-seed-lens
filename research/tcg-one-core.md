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

## Actual full-Android follow-up, still not stable

The one-core run started UTC11:37:10.441 with the same original images,
private worker1 ramdisk and no-animation option, a preselected10800s bound.
Actual UID2000/Enforcing/API34/ARM64/aarch64/NativeBridge0/new boot and
CPU-online mask0 (one CPU) were read. The first proof reader stopped on a
normal not-yet-completed boot flag because its inherited predicate assumed1;
that failed read is retained. The read-only predicate was corrected to wait
on an incomplete flag, preserving the actual identity and deployment guards.

At UTC11:44:19.508 the first **current-service** Power/Window/PM/AM/server
check passed for server496/start16743/UID1000. The next consecutive check did
not pass: server496 died at11:44:46 with a ConnectivityModuleConnector
`Lost network stack` fatal exception, and zygote then exited. This was not a
Watchdog kill. The single passing read is not stable boot/display acceptance.

A later events-buffer sample independently recorded a TetheringService
execution ANR for the shared NetworkStack process, followed by a background
ANR kill. This identifies a real startup failure to investigate; it does not
prove the precise cause of the first disconnection. A subsequent server
also had a stock Watchdog termination. Protections remain enabled.

Fresh package/ART reads found original TetheringNext APK under the tethering
APEX and NetworkStackNext under system/priv-app. Both were ARM64 `verify`;
the dump explicitly showed Tethering code used by NetworkStack. One ordinary
shell request was issued UTC11:54:36.971:

```
cmd package compile -m speed -p PRIORITY_BACKGROUND --primary-dex --include-dependencies com.android.networkstack
```

This is a bounded data-cache experiment on already installed SDK code, with
no force/reset, package installation, permission bypass, Root, Watchdog or
clock changes. There is no automatic resend of an uncertain request. Actual
RPC outcome, independent cache state and later service stability must be
recorded separately. No ordinary-App Root, JNI, game, Lens deployment or UI
qualification follows from the request.

The single compile returned UTC11:56:03.853, exit0/Success after86.882s.
At11:56:13.563 an independent ART dump confirmed NetworkStackNext ARM64
`status=speed / reason=cmdline`. The subsequent Tethering dump returned224
Broken pipe at11:56:22 during another service loss, so its cache state is
still unknown; that failed read is retained. A new bounded reader waits for
the package service and repeats only state queries. No compile resend or
boot/game qualification follows from this result.
