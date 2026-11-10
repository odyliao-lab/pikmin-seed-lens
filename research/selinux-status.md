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

All five were compiled/checked as AArch64. Only the read-only status test has
been executed in Android at this checkpoint: actual17 parser/negative checks,
including a regular file containing1 on the wrong filesystem, and real
Enforcing readback passed/exit0. No Root transition was requested for that
test. The direct ELF/budget comparisons remain **compiled, not run**. No App,
JNI, game, product integration or compatibility qualification follows.

Actual CMake controls also refused x64 and missing-budget configurations;
default ARM64 configuration contains none of these new research targets.
