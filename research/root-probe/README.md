# Ordinary-App Root research client

This separate lab APK is never a product dependency. The product's Root,
Android, NativeBridge and exact game guards are unchanged.

Version0.0.8/code8 additionally reads the fixed `/proc/self/exe` ELF header
from the ordinary Java process before a Root request. The observed API35 MuMu
reports uname=aarch64 despite x64 system executables/PM ABI; uname alone must
not classify it as native ARM or incorrectly refuse the existing x64 path.
`ProbePlatform` requires exact expected/actual API, a64-bit process, matching
packaged/primary ABI, supported uname and **matching process ELF machine**:

| Research backend | API | Primary/packaged ABI | Process ELF64 machine | Reported uname |
|---|---|---|---|---|
| ARM64 |31 or34|arm64-v8a|EM_AARCH64|aarch64|
| x64 |31 or35|x86_64|EM_X86_64|x86_64|
| x64, observed MuMu case |35 only|x86_64|EM_X86_64|aarch64|

API34 x64, API32, ARM35, missing/unknown/mismatched ELF machines and malformed
expected APIs refuse before the Root query. An x64 host executable cannot
be admitted as ARM even with an ARM uname/ABI label. The existing API31 x64
scope still requires uname=x86_64; no unobserved label exception was added.
This research classifier does not establish NativeBridge or product admission.

The bounded64-byte parser checks ELF magic/class/little-endian/version, executable
type and header size, refuses truncation/unknown architecture and propagates I/O
failure without a property fallback. It is architecture evidence from the running
process, not a complete validator for arbitrary files or caller-supplied paths.
The build runs94 ELF and77 platform JVM checks, including the observed same-uname
architecture distinction, short reads, I/O failure and version/ABI refusal.
These are host tests, not Android execution. Normal Root verification still
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

Current x64-only0.0.8 APK SHA256:
`c5d6bc1072ec44b2b2f4efd42db99c1ce910c24064b81245a22a490b74a38845`.
Original research certificate SHA256:
`633286a6f46aae1d6b4bcf26ed2278e72c73bde5046cfa02b7f69fd347ac8870`.
The unchanged API35 native runner SHA256 remains
`b22877ed3f6224fc0d716540048e1c7a6837544f9a73880c964edd393ea6ce84`.
Actual offline checks passed Android Java/dex, manifest0.0.8/code8, original
certificate, stored x64-only runner/ABI/SHA assets, new ProcessElf class and16KB
alignment. Old0.0.5 and0.0.7 APK hashes were independently unchanged. Root and PM
receipts now include process-machine evidence; Root receipts also retain uname
and packaged ABI. No native loader, game guard, Root endpoint or time bound changed.
**Not installed or executed in Android**: actual App access to `/proc/self/exe`,
Root and later PM/JNI behavior still require separate device evidence after the
owner finishes the active game-login handoff. The existing0.0.5 Root pass is not
transferred to0.0.8. This package never becomes a consumer dependency.

Preserved ARM-only0.0.7 preparation SHA256:
`d681d08e20c492a923a5853547b0973b30d0a0a039eb86da94df1933072c65d2`.
Its runner is the preserved ARM JNI0.0.2 verifier:
`1cb26b6a2a6a0fb9e2a579522abb8d3ffaca5275ec77eba12ee30a138613d460`.
Actual host checks covered44 JVM cases, Android Java/dex, original research
certificate, stored ARM-only runner/digest and16KB ZIP alignment. An actual
wrong-ABI invocation refused before output reservation or signing. The old
0.0.5 and0.0.6 APK SHAs remained unchanged. This package has **not** been installed or
run in Android at this checkpoint; full boot, platform safety and fresh device
identity must precede its use. Its44 checks described the earlier uname-based
classifier; that result does not establish the new ELF-based classifier on Android.

The earlier0.0.6 preparation passed41 host checks but lacked the packaged-ABI
binding. Code review caught this before deployment: a system's primary ABI
does not establish which native variant the APK contains. Its preserved APK
SHA is `fbe97d7e88a65a98205d22d1757fa9ba168acb6e74eecd5bfe9e199c086e30ea`;
it was never installed or executed in Android and is superseded by0.0.7.
