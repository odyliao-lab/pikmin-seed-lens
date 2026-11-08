# Pikmin Seed Lens

This private repository is the independent source for 皮克敏花苗透視鏡.
Read README.md, docs/ARCHITECTURE.md and docs/VALIDATION.md before work.
Do not commit keys, tokens, raw diagnostics, screenshots, APKs or game binaries.
Do not operate any device without current task authorization and fresh identity.
Never infer a supported Android/game version from compilation or APK installation.
Preserve exact game version/code/digest/prologue/runtime metadata guards.
Unknown versions keep original display. Do not bypass NativeBridge refusal.
Keep this display-only: no inventory writes, gameplay RPC, GPS or scanner management.
Do not overwrite a delivered APK/version. Current imported baseline is 0.1.4/code5;
repository extraction/build tooling alone does not bump the product. New delivered
bytes require a higher version/code and retained existing signing identity.
No public releases or device deployment are implied by commit/push authorization.

Build/test: PowerShell 7, explicit SDK/JDK/NDK/CMake/Ninja paths:
`pwsh -NoProfile -File app/build.ps1 -SdkPath <sdk> -JavaHome <jdk17> -NdkPath <ndk-r27d> -CmakePath <cmake> -NinjaPath <ninja> -Unsigned`.
This runs compatibility JVM tests and builds both native loader targets, ARM64
renderer and APK. Native test executables are compiled, not automatically run.
Signing uses an external keystore and environment-based passwords; no default key
discovery from another repository. Device evidence remains separate.
