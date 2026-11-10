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
unknown at that time; that failed read is retained. A later bounded reader
at11:58:40 confirmed Tethering remained `verify / first-boot`, while the
NetworkStack dump at11:58:44 remained `speed / cmdline`. The include-dependencies
request therefore did not establish speed compilation of this provider.

After a fresh exact ordinary-shell identity read, one separate request for
the still-uncompiled Tethering package was sent at UTC11:59:30.908:

```
cmd package compile -m speed -p PRIORITY_BACKGROUND --primary-dex com.android.networkstack.tethering
```

It returned exit0/Success at12:00:29.058 after58.149s. A separate ART dump at
12:00:31.311 confirmed the same installed APEX TetheringNext ARM64 code was
`speed / cmdline`. This was not a resend of NetworkStack or of an uncertain
request. No force/reset, Root, package installation or protection changes
were used. Both original RPC/read results remain in the private evidence.

The current run still has not established stable boot/home. A third stock
Watchdog kill occurred at12:01:13 for server3195, with the annotated main
stack in logging during system-user startup. A single stack snapshot does
not prove logging is the root cause. The next automatic server3863 remained
under observation. A12:05 resource sample found about2.46GiB guest RAM free,
no guest swap use and no measured WSL CPU/memory/I/O pressure; sampling
itself consumed guest CPU. These are diagnostic snapshots, not an isolated
performance benchmark or product acceptance. No compile resend, Root/App
deployment or game qualification follows from these results.

Server3863 also had a stock Watchdog kill at12:06:58.965, annotated in
wallpaper-settings serialization. These changing snapshots do not establish
a single deadlock or an ART-cache root cause. A stock-shell su0 read-only
child later observed the original services.odex executable mapping in the
next server; its parent returned UID2000. This is not ordinary-App Root.

The next automatic server4779/start185191/UID1000 completed both strong
current-service checks at UTC12:13:30.525 and12:15:32.770: PowerManager boot,
WindowManager system/display, PM/AM and unchanged before/after identity all
passed. The full proof records the actual current boot/fingerprint. A new
1080x1920 capture at12:16:08 displayed Launcher icons/search instead of the
earlier black frame. Actual Settings/home interaction is being checked
separately; no stock native suite, Magisk, App Root, JNI, game or product
qualification is claimed from the two reads or image alone. The same live
guest is retained. A [multi-thread comparison](tcg-aosp-multi-thread.md) was
prepared and host-tested only, without replacing this running guest.

Subsequent identity checking at12:17:08 found server4779 absent and stopped
the interaction script **before** Settings resolution/start or any input.
The separate error log records another `Lost network stack` fatal exception
in4779 at12:16:35.463, followed by zygote exit. This was not a fifth Watchdog
kill. It occurred after both network packages had independently read speed;
that cache improvement did not establish stable networking or boot. The
underlying network-process failure remains unproven by this side-effect log.

Fresh UID2000/Enforcing/API34/ARM64/same-boot checks at12:19:05 confirmed a
different server6279/UID1000 and absence of4779. The two-read proof's original
bytes/hash were retained under a historical filename and its deployment
gate retired. No display-review acceptance was created; no native suite,
Magisk or APK ran. After sync and exact console AVD identity, normal-close0
completed at12:19:07.119 with actual inferior/gdb0, no timeout/containment.
Thus the image and two reads remain useful partial evidence, not stable
home interaction or product qualification.
