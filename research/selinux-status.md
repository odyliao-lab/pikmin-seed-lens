# Read-only SELinux query comparison

Research only. Neither product backend uses `selinux_status.h`. The separate
`SEED_ARM64_DIRECT_SELINUX_RESEARCH` option defaults OFF and requires ARM64,
product-budget research and load diagnostics. It never changes Android/API,
NativeBridge admission, SELinux policy, the15s supervisor or2s remote calls.

On the authorized API34 ARM software guest under framework boot load, the
unchanged supervisor fixture passed10/10. The ARM fixture reached expected
normal-function and three intentional-failure outcomes, then its ELF case
became UNKNOWN/contained; whole executable exit1. A separate new-child load
diagnostic also became UNKNOWN/contained. It reached durable CALLING at6197ms
but never printed `before-injector-init`; supervisor finished at16631ms,
including child setup/containment outside its unchanged15s operation window.
Neither uncertain child/journal was reused or retroactively marked successful.

Source inspection shows the intervening local configuration calls include
`Utils::selinux_state()`, which executes `popen("getenforce")`. This is a
candidate cause, not yet a measured proof that it alone explains the timeout.
Additional diagnostic markers now surround SDK and SELinux queries. The old
compiled842a7e… diagnostic remains preserved; new bytes use a unique build.

The comparison fixture changes only the local SELinux query at the same point
in the transaction. It opens the fixed `/sys/fs/selinux/enforce` read-only,
with O_NOFOLLOW/CLOEXEC, checks regular-file type and SELINUX_MAGIC, then accepts
only exact0/1 with an optional single newline and EOF. Missing, malformed,
unreadable or wrong-filesystem data is Unknown; there is no shell fallback or
assumption of Permissive/Disabled. It never writes SELinux configuration.

Configure in a new unique directory using explicit NDK27.3.13750724,
CMake/Ninja paths and `ANDROID_ABI=arm64-v8a`:

```text
-DANDROID_PLATFORM=android-28 -DCMAKE_BUILD_TYPE=Release
-DSEED_ARM64_PRODUCT_BUDGET_TESTS=ON
-DSEED_ARM64_LOAD_DIAGNOSTICS=ON
-DSEED_ARM64_DIRECT_SELINUX_RESEARCH=ON
```

Build `selinux_status_test`, `arm64_direct_selinux_budget_test`,
`arm64_direct_selinux_diagnostic_test`, `arm64_load_diagnostic_test` and their
paired `arm64_loader_fixture`. No device test runs automatically. The direct
budget and diagnostic executables still accept no external target arguments;
each creates its own new fork child and append-only attempt. Diagnostic logging
can change timing, so it never substitutes for the ordinary budget fixture.

Initial separate build:

| Artifact | SHA256 |
|---|---|
| Read-only status test | `a2d26ec77f7f8c9186e23fda6053055d48cec85835e9a9f57acd2e45035a40e6` |
| Direct-query budget | `bc61862efe8096b2e1a4844df3581dc5541f9a482b0094cbed61fc201fe39bf2` |
| Direct-query diagnostic | `fd03b019c272d067ded9437d38be5e3d90a4e543746d782cdc89ffddc5e33d4c` |
| Expanded baseline diagnostic | `dafeb8b03a250fe929417173b298648ba77d2a95b419128037a39a6a32995a4c` |
| Paired ELF fixture | `39d84232fdebf1b8eda3169c5818775fecf02c19c367022dfbdcc9a20fa74faf` |

All five were compiled/checked as AArch64. The read-only status test actually
passed17 parser/negative checks, including a regular file containing1 on the
wrong filesystem, and real Enforcing readback/exit0. No Root transition was
requested for that getter test.

On a subsequent cold boot of the same authorized API34 guest, fresh actual
API34/arm64-v8a/aarch64/NativeBridge0/Enforcing and UID2000 were read before
one factory-adbd Root switch. Actual UID0 and the same boot were then verified;
all executable and paired-library guest SHA256 values matched. Three separate
own-child comparisons actually completed with exit0:

| Comparison | Observed result |
|---|---|
| Expanded baseline diagnostic | ELF load/export call/unload, snapshot equality and returned/detached ACK passed; target alive |
| Direct-query diagnostic | Same complete ELF lifecycle and snapshot/ACK checks passed; target alive |
| Direct-query ordinary budget fixture | All5 cases passed: normal return, three intended UNKNOWN/contained faults, normal ELF lifecycle |

The baseline SELinux query interval was2884–10469ms (7585ms); direct-query
interval3208–3221ms (13ms). Diagnostic total elapsed values were15121ms and
7663ms respectively; these include setup/teardown outside the unchanged15s
supervisor operation window. They are individual sequential samples under
changing boot load, not a benchmark or a causal isolation of the old timeout.
**The baseline also passed.** The earlier UNKNOWNs remain failures of their
own attempts; this comparison does not prove that the shell query alone
caused them or that the candidate eliminates load-dependent timeouts.

The five-case result deliberately includes three UNKNOWN outcomes with
confirmed containment, rather than claiming those calls returned normally.
These executions used factory-adbd UID0 while framework boot was incomplete.
They do not establish ordinary-App su-M, ART/JNI, full Android boot, game,
product integration or compatibility qualification. The one Root session's
later actual unroot and guest shutdown must be recorded separately.

### Later boot-load comparison: both ordinary suites failed

A second, separately recorded comparison used two ordinary five-case suites
with the same newly built paired fixture (`39d842…`), verified guest hashes,
the existing Root session, and new own children/attempts. It ran direct-query
first, then baseline, while PackageManager approached the end of system scans.
No Root switch or additional diagnostic logging was added. The baseline plain
executable SHA256 is
`e9c18e6c7e78ab04b369eefe5a956cf1245a19e9a4e29a5724117b1265e7cfd5`.

Both executions exited1. Both passed the normal-function case and three
expected contained-fault cases. The direct-query ELF case actually printed
load/call/unload success but its supervisor recorded all4 phases without a
reaped worker before the original deadline (`uncertain=0`, Refused, target
alive). That is **not** a complete successful transaction. The baseline ELF
case recorded only2 phases and UNKNOWN/contained, target dead. The private
journals remain separate; no failed or uncertain target was called again.

Thus the early five-case pass is a bounded sample, not stable platform
qualification. Direct querying alone did not make the later boot-load suite
pass. Stop additional loader tests during this initialization phase; obtain
full boot and assess isolated normal-App/JNI behavior under an identified
load condition before considering product integration. No timeouts or
worker-reaping requirements were relaxed.

Actual CMake controls also refused x64 and missing-budget configurations;
default ARM64 configuration contains none of these new research targets.
