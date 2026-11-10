# Isolated NativeBridge canary (not a product backend)

The separate API34 experiment now has a four-argument adapter, exact SDK/bridge
admission, self-only PM runner and explicit ordinary-App research scope.
Carrier0.0.10 and Root client0.0.12 are signed preparation artifacts, not device
or game qualifications. API31/35 hosts and PM runners passed a compile
regression; their previous device evidence is not reassigned to API34.
See [API34 sources, hashes and actual-execution gates](../../docs/API34_PM_FLOW_RESEARCH.md).

Package `dev.ody.seedlensprobe` targets only itself with Android instrumentation.
It has no Activity, Internet, boot receiver, root request or game functions. It
loads its APK-installed ARM64 fixture through Java, checks four return values,
writes PID-bound receipts, remains alive for 90 seconds, and finishes. The game
can remain foreground; no scanner pause is required for this background probe.

`build.ps1` takes explicit SDK/JDK/NDK and an external signing key/password-env
source. It builds ARM64-only libraries and rejects an unpackaged C++ dependency.
Code4 stores the canary only at `assets/bridge/libcanary.so` inside the APK;
only the fixture is in `lib/arm64-v8a`. Both are stored/page-aligned for 16KB.
The signed APK is checked again for alignment and absence of an extracted canary.
Use the same test signing identity for upgrades. Research versions are separate
from the Lens product; keep previous APKs, hashes and receipts.

The x86_64 CMake targets are built with NDK r27d/API28, `c++_static`, and the pinned
clean AndKittyInjector/KittyMemoryEx dependencies. The build creates one isolated
copy of `KittyTrace.cpp` using `research/bounded-trace.cmake`: monotonic waits,
bounded SEIZE interruption, a shared syscall deadline, checked stopped-state
restoration and rejection of internal Linux restart results. Pinned anchors must
match exactly; updates fail closed. `kitty_syscall_test` traces only its own forked
children and tests normal/timeout/signal/exit behavior, not arbitrary dlopen.
No submodule or production trace implementation is changed. xDL resolves **already mapped**
platform-library exports; it is never called with force-load flags.
Configure CMake with `-DSEED_CANARY_LIBRARY=<build>/payload/assets/bridge/libcanary.so`
to pin the canary's SHA in the host. The library must be the exact APK entry.

## Boundaries

- `canary-root` accepts only the exact self-test process command (including normal
  zero padding), matching app UID and current process-start ticks. Its payload
  path is fixed, root-owned and not writable by apps. No arbitrary package/library
  interface is exposed. `canary_identity_test` covers exact/padded/negative names.
- A parent watchdog uses a PID fd and `PTRACE_O_EXITKILL`. On uncertain execution it
  terminates **the disposable test process**, not the game. This is containment,
  NOT graceful recovery and MUST NOT be copied into a game loader as a solution.
  The explicit `--timeout-test` option deliberately stalls only after attaching
  to the verified self-test process. Its three-second watchdog requires both
  tracer reaping and PID-fd exit readiness before reporting containment passed.
- The root runner stages verified build bytes in a root-owned directory. The host
  constructor repeats the self-package guard and starts a JNI worker. Receipts
  are exclusive-create, per PID, in that test package's private files directory.
- The worker gets the real current Application/ClassLoader and its opaque native
  namespace. It requires a bridged namespace and loads only the PM APK's embedded
  ARM64 canary via `base.apk!/assets/bridge/libcanary.so`. It validates the stored
  ZIP entry, 16KB alignment, local/central agreement, absence of duplicate entries
  and its build-pinned SHA; an open FD pins the checked file through load.
  The shared `app/apk_payload.h` reader does not replace PM/signature/owner checks.
  It checks a missing library, missing symbol and four actual JNI trampoline
  results. No exported-name guessing, pointer `1`, ART-object layout
  guessing or SELinux changes.
- Wait for `FIXTURE_PASS` in the current PID's receipt before calling the host
  runner. A fixed three-second startup delay was insufficient in one run.
- Validate both the namespace receipt and subsequent Java `ALIVE`/`FINISH`;
  `dlopen` or exit 0 alone is insufficient. Verify no attached tracer remains.
  Keep all prior failures. Never rerun into the same process after an UNKNOWN.
- The translated `JNI_OnLoad` test rejects a wrong entry key before mutation,
  accepts the expected key twice but creates only one ARM64 worker, and checks
  that worker's JVM attach/GetVersion/detach. This is not the real renderer.
- `apk_payload_test` has synthetic positive/negative ZIP policy and SHA vectors;
  optional APK path + expected entry SHA checks actual bytes and a wrong digest.
  This executable only reads files; it does not attach or load code into apps.

## Evidence limits

Virgo Android15/API35 has baseline ARM64 JNI and real-classloader canary evidence
(see `docs/COMPATIBILITY_RESEARCH.md`). Its ROM was already SELinux Permissive;
we did not change it. This does not qualify Enforcing x86 ROMs, Android28–34,
game rendering, JNI_OnLoad/lifecycle for the real renderer, or cancellation of
arbitrary dlopen side effects. The refusal described here is the historical
research-stage state; 0.3.1 has a separately qualified API35 product candidate.
Android12/API31 NativeBridge is not qualified by this canary evidence.
The test APK is development tooling, not another package required by end users.

## API31 research preparation (2026-11-09; no device qualification)

The root runner now requires `canary-root PID START EXPECTED_API [--timeout-test]`.
It reads the device API at runtime, admits only an exact API31 (experimental) or
API35 (prior baseline) match, and passes that actual API to KittyInjector instead
of the old fixed35 value. API32, missing/malformed API, and mismatches fail before
process lookup, pidfd creation, fork or attach. Product x64/API35 and libnb guards
remain unchanged. `probe_platform_test` compiles 22 selection assertions for both
ABIs; it is not runtime, NativeBridge or containment acceptance.

Only on a confirmed disposable API31 instance, use `canary-root PID START 31`;
run `--timeout-test` against a **separate fresh probe process**, never the process
used by a preceding uncertain load. Confirm API32 mismatch and stale-start
refusals before the positive run. Runtime mapped-library exports, ClassLoader
namespace, baseline ARM JNI, four canary values, wrong package/version/key,
missing library/symbol, worker attach/detach, normal FINISH, timeout containment
and a new-process repetition all still need private device receipts. Do not use
this research admission to widen `Compatibility.java` or the production loader.

The official API31 AVD uses `libndk_translation.so` and lacks both
`IsNamespaceNativeBridged` and `NativeBridgeGetTrampoline2`. The separate
`seedprobe_api31_host` target uses the API31-only `api31_platform.h` experiment;
the existing `seedprobe_host` and product adapter keep the API35 interface.
This target is built, not yet loaded into an App. Namespace pair lookup is only
a candidate; the actual load must report `needs_native_bridge=true` before the
four-argument trampoline is usable. See [actual API31 results and source
contracts](../../docs/API31_AVD_RESEARCH.md). Do not deploy this experimental
host to a game or treat fixture results as product qualification.

## API31 PM carrier and durable runner preparation

`build-api31-lab.ps1` prepares an ARM-only probe (0.0.6/code6), a separate x64
host carrier (0.0.1/code1), and the `api31-canary-root` executable. The carrier
exists only to obtain a trusted PM library path under Enforcing; it has no Java,
Activity, network, game hooks or scanner role. These two packages are laboratory
tools, never an end-user requirement. The product still uses one APK.

Pass explicit SDK/JDK/NDK/CMake/Ninja paths and `-Unsigned` for preparation. These
APKs are NOT_FOR_INSTALL_OR_DELIVERY. Signing additionally requires an explicit
external `-SigningKey`, `-ExpectedCertificateSha256` and password environment
variable names. No key discovery/generation or device operation occurs. Signed
versions are reserved outside the temporary build outputs; another build cannot
replace them without increasing the manifest versions/codes. A key/permission
failure is not a successful signed build.

The API31 host's exact name/code come from `AndroidManifest.api31.xml`. The ARM
canary SHA is pinned in that host; its immutable PM-carrier copy is then pinned
into a separately configured runner. Packaging checks enforce ARM-only probe,
stored canary asset, stored x64 carrier library and 16KB alignment.

Runner arguments: `api31-canary-root PID START 31 PM_HOST [--timeout-test]`.
Only the exact self-probe command/UID/start is accepted. The library must be the
canonical system/root-owned PM path of `dev.ody.seedlenshostcarrier`, with the
exact build SHA, safe ancestors and stable file identity. The durable lease and
LoadAttempt use `/data/local/tmp/seed-lens-probe-pm-v1`. Never delete it to retry.
The supervised backend is a research derivative of the 0.3.1 x64 implementation,
restricted to the self-probe thread/API31. CALLING is acknowledged durably before
EXITKILL or a remote call; uncertain calls preserve UNKNOWN and use exact pidfd
containment. The timeout option deliberately stalls only a disposable fresh
probe after CALLING. It is not a game recovery strategy.

As of this stage, unsigned pair/layout checks and seven **pre-attach refusal**
cases have run on API31; positive PM-host load, actual JNI and timeout execution
for this new runner remain untested. Existing supervisor fixtures do not prove
these App-level cases. See the latest [API31 evidence](../../docs/API31_AVD_RESEARCH.md).
