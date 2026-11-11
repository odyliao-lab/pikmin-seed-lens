# API34 AOSP ARM/Linux alternative: authorized research

2026-11-10. The owner explicitly approved this fixed scope with
「同意新增 AOSP 隔離研究」. Intake, a fresh data image and the owned CPU gate
have now executed; see [actual results](API34_AOSP_ARM_LINUX_RESEARCH.md).
No full Android boot, Root, ordinary-App or game success follows from that gate.

The authorized Google-APIs ARM64 guest has progressed through all three
compressed browser packages. A separate scheduling experiment passed its
owned V2/V3c CPU tests, but its full Android boot has had two stock Watchdog
terminations. PID494's final main stack was waiting during WindowManager
initialization; earlier full dumps instead captured idmap Binder work and GC,
with UI/display threads then idle. PID712 later progressed further before a
second termination during Smartspace service/receiver initialization. A third
Watchdog kill was later logged for PID1005. These
different snapshots do not prove one permanent graphics deadlock. Repeated
initialization latency is a hypothesis, not a demonstrated root cause.

A smaller official default/AOSP image removes the Google image's additional
packages from the initial baseline. It may still fail or be too slow; neither
its smaller size nor successful installation can qualify a game.

| Field | Proposed fixed value |
|---|---|
| Dedicated host | Existing WSL2 `SeedLens-TCG-Linux` |
| Official package | `system-images;android-34;default;arm64-v8a`, revision4 |
| Archive | `arm64-v8a-34_r04.zip`,705115396 bytes |
| Official SHA1 | `d9f2011131919abe952814e041f16f317242c3fa` |
| New AVD | `SeedLens_API34_AOSP_ARM64_Linux` |
| New ports | Linux5590/5591; recheck Windows/Linux before launch |
| Resources | 2 guest vCPUs/4GiB; new6GiB encrypted-capable data |
| Image destination | Separate SDK `android-34/default/arm64-v8a` directory |
| AVD destination | Dedicated Linux lab AVD directory; no existing data clone |
| Initial host bound | Explicit10800 seconds, chosen before launch |

Source: [official repository metadata](https://dl.google.com/android/repository/sys-img/android/sys-img2-3.xml),
read2026-11-10; [fixed archive](https://dl.google.com/android/repository/sys-img/android/arm64-v8a-34_r04.zip).
Intake must recheck metadata, size/SHA1, compute SHA256, safely extract and record
all original files before any execution. No API32 or altered API properties.

Only one research guest runs at a time. Normally close and verify the current
guest first; keep its data, complete diagnostics and UNKNOWN/Refused history.
Original four MuMu instances, AutoFarm and Taichung Libra/Virgo are untouched.
No formal scanner, account state, credential or signing-key copy into the lab.

The host mapper/vector controller needs a **new exact marker/AVD/port/hash
tuple**, not a relaxed existing guard. Keep2 guest vCPUs, real feature probes
and the original SDK executable; no Hyper-V/driver/global WSL changes. Run
owned V2/V3c CPU tests and real fault/reap control first. Full Android then
requires actual API34, native ARM64 primary ABI/aarch64 kernel, NativeBridge0,
Enforcing, completed boot, working PM/AM/display and independent stability.
Do not stop Watchdog, self-tests or SELinux enforcement to obtain a pass.

Only after platform gates, research official Magisk31.1 in a private ramdisk
override; preserve originals, Enforcing, Zygisk OFF and no display modules.
Then verify ordinary-App `su -M`, PID1 mount view, own JNI lifecycle and failure
containment before product integration. Product15s/2s limits, exact game and
process guards and append-only UNKNOWN history remain unchanged.

This default image lacks Google services. Google login and game startup are
separate unproven gates. If required, propose a trusted official-package
installation path separately; do not silently add a GApps/Magisk module or
copy a signed-in device. Owner performs all login. No three-game qualification
or root-plus-single-APK product claim follows from an AOSP boot alone.

Authorization is required because this is a different image and new AVD from
the approved Google-APIs target. The user's original instruction requires
confirmation of concrete operation targets; AGENTS.md also says:
“Do not operate any device without current task authorization and fresh identity.”
