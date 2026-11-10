# Ordinary-App Root research client

This separate lab APK is never a product dependency. The product's Root,
Android, NativeBridge and exact game guards are unchanged.

Version0.0.7/code7 adds a packaged-ABI pin to the native ARM research path while
preserving earlier signed artifacts. `ProbePlatform` requires an exact
expected/actual API, a64-bit process, matching packaged/primary ABI and kernel:

| Research backend | Accepted tuple |
|---|---|
| ARM64 | API31 or34, arm64-v8a, aarch64 |
| x64 | API31 or35, x86_64, x86_64 |

API34 x64, API32, translated ARM on an x64 kernel, mismatches and malformed
expected APIs refuse before the Root query. The build runs44 JVM checks; these
are host admission tests, not Android execution. Normal Root verification still
uses the existing10s wait, `su -M`, ordinary App UID, actual UID0, exact boot and
identical PID1/global mount namespace. A successful Root query alone does not
prove JNI, game display or product compatibility.

The optional PM command uses only the client's own canonical PM-installed PIE
and compiled runner digest. Explicit `-Abi arm64-v8a` requires an AArch64 ELF64
PIE and packages only that ABI. `runner.abi` is an immutable APK asset; both the
Root query and PM command compare it with the actual primary ABI. System ABI
alone cannot identify the package's native-library variant. The ARM command invokes the separately pinned
`arm64_jni_probe PID START CANARY_PATH API`; that verifier independently checks
the exact isolated ARM probe identity, start, UID, PM path, payload SHA and
actual API, then uses the unchanged product backend/journal/budgets. The existing
x64 command order remains `PID START API HOST_PATH`. ARM rejects the x64-only
`timeout_test=true` option instead of silently running a different test. ARM
fault containment must be verified with the separate own-child fixtures.

Build with explicit SDK/JDK, external original **research** key, password env
names, native runner SHA, certificate SHA and ABI:

```powershell
pwsh -NoProfile -File research/root-probe/build.ps1 `
  -SdkPath <sdk> -JavaHome <jdk17> -SigningKey <external-lab-key> `
  -NativeRunner <pinned-arm64_jni_probe> -ExpectedRunnerSha256 <sha256> `
  -ExpectedCertificateSha256 <original-research-certificate-sha256> `
  -Abi arm64-v8a
```

Every version output directory is immutable. Different later bytes, including
an x64 package, require another version/code. No key or APK belongs in Git.

Current ARM-only0.0.7 APK SHA256:
`d681d08e20c492a923a5853547b0973b30d0a0a039eb86da94df1933072c65d2`.
Its runner is the preserved ARM JNI0.0.2 verifier:
`1cb26b6a2a6a0fb9e2a579522abb8d3ffaca5275ec77eba12ee30a138613d460`.
Actual host checks covered44 JVM cases, Android Java/dex, original research
certificate, stored ARM-only runner/digest and16KB ZIP alignment. An actual
wrong-ABI invocation refused before output reservation or signing. The old
0.0.5 and0.0.6 APK SHAs remained unchanged. This package has **not** been installed or
run in Android at this checkpoint; full boot, platform safety and fresh device
identity must precede its use.

The earlier0.0.6 preparation passed41 host checks but lacked the packaged-ABI
binding. Code review caught this before deployment: a system's primary ABI
does not establish which native variant the APK contains. Its preserved APK
SHA is `fbe97d7e88a65a98205d22d1757fa9ba168acb6e74eecd5bfe9e199c086e30ea`;
it was never installed or executed in Android and is superseded by0.0.7.
