# API34 AOSP: two-CPU multi-thread comparison

This is Linux host research for the existing authorized AOSP AVD/5590 only.
It is not an Android APK dependency, a new device, or product admission.
The scheduling hypothesis is that two software guest CPUs on separate host
TCG threads might reduce startup latency; it has not been demonstrated.
Watchdog, guest clocks, SELinux, original SDK files and product15s/2s remain.

The new `-Api34AospMultiResearch` build requires explicit API34/AOSP scope
and refuses single-thread/one-core scopes. The independently pinned mapper:

- Marker: `seedlens-api34-aosp-tcg-multi-mapper-v1`.
- Vector-controller marker: `seedlens-api34-aosp-tcg-multi-vector-v1`.
- SHA256: `d329efe1dd2ea9ca086105240431136bc90362ef0569f20366e94bdf825b692a`.
- Exact original SDK executable, AOSP AVD and port5590.
- Exactly one frontend `-cores 2`, then `-qemu -accel tcg,thread=multi`
  and one backend `-smp 2`. Missing/duplicate/wrong counts, parser scope,
  modes, original-function lookup and conflicting acceleration refuse.

Actual dedicated-WSL C guard results were13 default,23 Google-single,
26 AOSP-single,35 AOSP-onecore and43 new multi cases, all passed. The
reported counts were independently checked against actual pass lines.
The new cases retain valid CPU arguments while varying scheduling, so
refusal is not attributed to unrelated missing-count checks. Python passed
13 methods, including cross-target/mode/resource refusal. Three actual
invalid compile scopes refused; unscoped PowerShell build refused before
artifact reservation. Real GDB wrong-executable/preload cases refused
before `starti`. All five earlier mapper variants rebuilt byte-identically.

The one-core run was first retained for its two complete same-server boot
checks and Launcher image. Its server then died with another network-stack
fatal before interaction; the historical proof bytes were retained and the
deployment gate retired. After fresh replacement-server/UID checks and sync,
it closed normally at UTC12:19:07, actual inferior/gdb0, no timeout or signal.

No QEMU remained. The first cold-transition attempt refused the occupied
port before any configuration edit; `ss` then showed only TIME-WAIT, no
listener. After the ports cleared, all four original SDK image hashes were
checked. The original config bytes were retained, and only guest CPU count
was restored from1 to the originally authorized2;4GiB/display/data remained.
New index5 was freshly confirmed closed with its original name/created time;
Windows had12.90GiB physical free before the new research run.

The new original-kernel/V3c CPU run at UTC12:20:35.721–12:20:43.477 actually
reported two CPUs. All7 tests and intentional NULL-fault/child-reap control
passed. Fourteen vector wrappers registered, real host AVX flags1/1 remained,
normal power-off and actual inferior/gdb0, no timeout/containment. It is not
an isolated speed benchmark: caches and preceding workload differ.

Only then, at12:21:27.585, a separate full Android run began with the pinned
private AOSP worker1 ramdisk, no-animation option and a fixed1800s bound.
The same current-service two-read proof and separate visual/interaction
gates apply, with a fresh boot and actual CPU-online0-1 required. Android
boot, ordinary-App Root, JNI, game and Lens acceptance remain unproven;
the CPU test is not transferred to those layers. All stages are incomplete.
