# ARM64 Android JNI isolation

Research only; not a user dependency or game compatibility proof. The separate
`dev.ody.seedlensarmprobe` APK has no Activity, network permission, game reference
or scanner control. It loads its baseline fixture and starts one bounded native
thread under real ART. A second PM-installed library is deliberately not loaded
by Java: the candidate root backend must load it and invoke its JNI entry.

Build the APK with `build.ps1` and explicit SDK/JDK/NDK/external signing paths.
Signing passwords use existing environment variables only. The output directory
is unique and the existing certificate is checked. No product artifact changes.

Configure app CMake in a **separate build directory**, ABI arm64-v8a, with
`SEED_ARM64_PROBE_LIBRARY` naming this APK's exact `libcanary.so`, then build
`arm64_jni_probe`. Its SHA is compiled into the verifier. This target compiles
the same `arm64_backend.cpp` with `SEED_ARM64_RESEARCH_PROBE`: only the package
and disposable thread selector differ. It cannot choose the game at runtime.
The normal seed-root target never defines this macro.

The root verifier takes PID/start/path, but accepts only the exact probe command,
matching app UID and start time, canonical PM library path, trusted root/system
owners and groups, no symlinks/foreign writers, and the compiled payload SHA.
It creates a private append-only LoadAttempt and uses the product supervisor,
ARM64 context, full before/after snapshot, and pinned Kitty overlay. This phase
is restricted to the freshly verified Libra API34 environment (API is fixed34).

Acceptance requires all of:

- wrong game target and stale start refused before attach;
- complete A/C/R/D/RETURNED_UNVERIFIED journal for the exact probe process;
- root loader exit0, not merely an existing library mapping;
- newly created private native receipt: actual VM/GetEnv, worker attach/GetEnv,
  worker detach;
- baseline instrumentation finishes with its original native thread returned
  and a subsequent JNI call correct;
- original game PID/start unchanged, all game threads untraced, Lens OFF,
  scanner config/code hashes unchanged, no scanner maintenance pause.

First run refused before attach because the research verifier required root
ownership rather than Android's actual system-owned PM paths. The second run
used the same root/system ownership policy as the production loader. No file
permissions or SELinux policy were changed to make the test pass.

This proves candidate ARM64 JNI mechanics in an isolated App only. It does not
prove game namespace, renderer hooks, naming, OFF restoration, other Android
versions, other root implementations or the second game profile.
