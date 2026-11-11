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

## Actual multi-thread Android observation

All32 APEX mounted and BoringSSL vendor/system/APEX self-tests returned0.
The first full metadata query timed out at120s. Exact host QEMU/start still
matched; both TCG threads were running, and kernel output progressed. This
was an observation failure, not a terminal guest, and no restart was issued.
A bounded read-only recovery retained the failure and first checked a small
shell echo: exit0 in3.774s at UTC12:30:45.529. The subsequent exact metadata
query returned0 in43.370s at12:31:28.899, reading a new boot, UID2000,
Enforcing, API34, ARM64-only/aarch64, NativeBridge0, no-animation1 and actual
CPU-online0-1. The boot flag was empty. No prior boot identity was reused.

The first system_server511 entered at12:31:22.028. Stock Watchdog killed it
at12:37:16.158, annotated during permission/AppOps work in the system-ready
path; this was not a boot or product pass. A stock-shell su0 read-only trace
reader initially exited2 on a missing closing test bracket, before reading
traces; its outer shell actually returned UID2000. That script error is
retained separately, not reported as a platform Root failure.

The corrected separate reader completed: current-boot-time trace15 sampled
server511 at local20:34:10 during font initialization/ART JNI transition;
trace16 sampled it at20:36:20 Runnable in ICU locale data initialization.
Together with the later Watchdog annotation, these changing snapshots do
not establish one permanent lock or a complete root cause. The selected
trace mtimes were after this boot's start. Read-only stock-su child context
and return to outer UID2000 were checked; no adbd restart, Magisk install,
normal-App Root, native loader suite, JNI probe or game deployment occurred.

A second automatic server928 was observed while the global boot flag still
remained empty. Strong boot/display/interaction gates remain required;
the preselected30-minute supervisor and earlier normal-close deadline are
unchanged. An [ARM cloud option](../docs/ARM_CLOUD_RESEARCH_OPTION.md) was
researched from vendor documentation only: no account, subscription, paid
device or new operation target has been created or authorized by this work.

Second server928 was also killed by stock Watchdog at UTC12:46:55.740.
The strong verifier reached its preselected observation bound without a
complete current-service boot; the gated capture refused without proof.
Neither is treated as the VM ending: the same exact QEMU remained live and
the scheduled normal-close timer continued. Fresh console AVD name matched.

Final metadata/sync read UID2000/Enforcing/API34/ARM64 and the same current
boot, still with an empty global completion flag. Normal console-close0 was
recorded at12:49:20.184; actual inferior/gdb0 at12:49:40.717, no timeout or
containment signal, approximately28m13s observed. No QEMU remained afterward.
No native loader suite, Magisk, ordinary-App Root/JNI or game was deployed
on this run. Both Watchdog terminations, the timed-out metadata observation,
the script-error refusal and successful read-only recovery remain separate.
