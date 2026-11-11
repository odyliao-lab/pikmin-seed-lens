# ARM64 Android JNI isolation

Research only; not a user dependency or game compatibility proof. The separate
`dev.ody.seedlensarmprobe` APK has no Activity, network permission, game reference
or scanner control. It loads its baseline fixture and starts one bounded native
thread under real ART. A second PM-installed library is deliberately not loaded
by Java: the candidate root backend must load it and invoke its JNI entry.

Build the APK with `build.ps1` and explicit SDK/JDK/NDK/external signing paths.
Signing passwords use existing environment variables only. The output directory
is unique and the existing certificate is checked. No product artifact changes.
Version 0.0.3 reserves a version-specific output directory and refuses rebuilds
over existing bytes. It retains this probe's original signing identity; the
earlier0.0.2 bytes and results remain separate.

Configure app CMake in a **separate build directory**, ABI arm64-v8a, with
`SEED_ARM64_PROBE_LIBRARY` naming this APK's exact `libcanary.so`, then build
`arm64_jni_probe`. Its SHA is compiled into the verifier. This target compiles
the same `arm64_backend.cpp` with `SEED_ARM64_RESEARCH_PROBE`: only the package
and disposable thread selector differ. It cannot choose the game at runtime.
The normal seed-root target never defines this macro.

The root verifier takes `PID START PM_CANARY_PATH EXPECTED_API`, but accepts only the exact probe command,
matching app UID and start time, canonical PM library path, trusted root/system
owners and groups, no symlinks/foreign writers, and the compiled payload SHA.
It creates a private append-only LoadAttempt and uses the product supervisor,
ARM64 context, full before/after snapshot, and pinned Kitty overlay. This phase
now reads `android_get_device_api_level()` and requires an exact expected API.
Research admission is limited to API31 (experimental) and API34 (prior baseline).
API32, mismatches, omitted API, and malformed values fail before process lookup,
attempt creation or attach. The actual API is passed to the shared backend;
there is no API override or product admission change. The earlier fixed34 tool
must not be used to claim API31 evidence. See `../probe_platform_test.cpp` for
22 compile-time selection checks, built for both ABIs; these are not device tests.

On an explicitly authorized, freshly identified **native ARM64** API31 target,
use the new verifier as `arm64_jni_probe PID START PM_CANARY_PATH 31`.
Start its ordinary App fixture with
`adb -s <authorized-serial> shell am instrument -w -e expected_api 31 dev.ody.seedlensarmprobe/.ProbeInstrumentation`.
For the historical API34 baseline, use explicit `expected_api 34` instead.
Version 0.0.2 requires the exact Java API, ordinary App UID, ARM64 as
the primary supported ABI, a 64-bit process, and the Java platform's kernel
uname `aarch64` before loading the fixture. After loading, it also verifies
the actual native API before starting its thread. API32,
missing/mismatched expected API and x64/NativeBridge targets must be refused;
do not treat a translated ARM JNI result as native-backend evidence.
These App guards complement fresh external device identity; they do not grant
authorization for an unapproved device.
Before any successful-target run, check wrong API32 and stale-start refusals
without attach or an attempt; keep the private receipts. Continue with actual
CPU-context/supervisor fault fixtures and JNI lifecycle evidence. A translated
ARM64 App on an x86_64 kernel does not qualify the native ARM64 backend.

Acceptance requires all of:

- wrong game target and stale start refused before attach;
- complete A/C/R/D/RETURNED_UNVERIFIED journal for the exact probe process;
- root loader exit0, not merely an existing library mapping;
- newly created private native receipt: actual VM/GetEnv, worker attach/GetEnv,
  worker detach;
- baseline instrumentation finishes with its original native thread returned
  and a subsequent JNI call correct;
- Lens OFF, no game target selected or modified; any authorized game PID/start
  observation remains separate from fixture success;
- no scanner control or access to existing production nodes. This fixture's
  preparation never authorizes Libra/Virgo or the four existing local instances.

First run refused before attach because the research verifier required root
ownership rather than Android's actual system-owned PM paths. The second run
used the same root/system ownership policy as the production loader. No file
permissions or SELinux policy were changed to make the test pass.

This proves candidate ARM64 JNI mechanics in an isolated App only. It does not
prove game namespace, renderer hooks, naming, OFF restoration, other Android
versions, other root implementations or the second game profile.

## 2026-11-09 API31 preparation: not deployed

0.0.2/code2 Java/dex, both ARM64 libraries, original-certificate signature,
stored ARM64-only APK entries, matching extracted library digests and 16KB ZIP
alignment were checked. A separate CMake directory compiled the paired
`arm64_jni_probe`, `arm64_context_test`, `supervised_load_test` and
`probe_platform_test`; those executables have not been run on native API31.
The App's new admission guards have not been device-tested. No product bytes
or existing device installation changed in this preparation.

| Prepared artifact | SHA256 |
|---|---|
| 0.0.2 APK | `040bd1b0b9e14b8999aa9bf976ca47a9588cd396039f9bd2b0a1039dc01a2da2` |
| baseline libfixture.so | `17800bce942837c22104cc11062cfd5a41bdca3abf319e0f12a91939e04356c7` |
| pinned libcanary.so | `ca754d78dc17eb9dd1d66f8ab03da8c22f043d69cbd3b5889b3fc15a9b551773` |
| paired arm64_jni_probe | `1cb26b6a2a6a0fb9e2a579522abb8d3ffaca5275ec77eba12ee30a138613d460` |

Private artifacts are under ignored `.tools/research/arm64-jni-0.0.2/`.
Before using them, obtain a concrete authorized native ARM64 API31 target;
verify fresh identity, ordinary-App Root, kernel/CPU restrictions and isolated
fault containment. These prepared files do not resolve the existing API31 x64
image's SEVL failure or qualify either game version.

## 2026-11-10 bounded coordination window: 0.0.3 prepared, not deployed

The original fixture's native thread lasts35 seconds, while actual shell
identity reads during API34 TCG boot have taken20–50 seconds. Source inspection
therefore identifies a coordination hazard before any normal-App/JNI execution
on that guest; it is not a newly observed App failure.

0.0.3/code3 keeps the same package, API/ABI/kernel gates, baseline JNI, canary,
worker lifecycle requirements and original certificate. Its owned native test
thread has a fixed120-second lifetime with clock checks batched after65536
NOPs, rather than clock/yield on every iteration. The instrumentation records
that research window, waits135 seconds, then still requires the original
thread to have returned and a subsequent JNI call to be correct. There is no
runtime lifetime override. **Only the disposable fixture's lifetime changed**;
the loader15s supervisor,2s calls,25s verifier alarm, identity checks and
UNKNOWN policy did not change. This never alters a game thread.

Actual build preparation compiled native/Java/dex and checked original signing
identity and signed ZIP16KiB alignment. No device deployment, baseline run,
ordinary-App Root, JNI injection, framework boot or game acceptance follows.

| Prepared0.0.3 artifact | SHA256 |
|---|---|
| Signed APK | `fc8f24b7e66242ee751b94d7b3fb6aab6ccd87293321885b4fe5592f04e9b793` |
| Updated libfixture.so | `b2c811e8215142233b1a11e70f463ae7efb60fa23482e72fd2070f790bd35bdd` |
| Unchanged libcanary.so | `ca754d78dc17eb9dd1d66f8ab03da8c22f043d69cbd3b5889b3fc15a9b551773` |

The canary digest still matches the existing1cb26b… verifier's compiled pin;
that verifier's bytes need not change. The normal-App Root client0.0.7 retains
that same verifier. Actual installation must independently verify PM files,
signature, fresh exact platform/process identity and all isolation gates.
Do not transfer old0.0.2 evidence to this new fixture.
