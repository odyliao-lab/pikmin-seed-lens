# ARM64 own-child tests with product budgets

These separate executables use existing product limits. They neither replace
the stricter stress tests nor change a product loader, APK, timeout or guard.
They accept no external target arguments; every target is a direct fork child.

In a unique CMake build directory, select `ANDROID_ABI=arm64-v8a`, an explicit
NDK/toolchain/Ninja, and `SEED_ARM64_PRODUCT_BUDGET_TESTS=ON` (default OFF).
Build `arm64_product_budget_test`, `supervisor_product_budget_test`, and their
paired `arm64_loader_fixture`. No tests run automatically during the build.
Keep the other `arm64_context_test` and `supervised_load_test` results separate.

The ARM fixture uses the existing supervisor default15000ms, 2000ms remote
calls, the same snapshot/restore checks, Kitty overlay and actual ELF loader.
It tests a normal function, spin, blocked syscall, illegal instruction, and
actual load/call/unload without JNI. The five-case suite has a90s outer alarm;
each disposable child has a20s failsafe beyond the unchanged15s supervisor.
The ten-case supervisor suite also uses the unchanged default per operation,
with a180s whole-suite failsafe. Those suite alarms are not loader deadlines.
Both new targets use the product's `-O2`/function-section/garbage-collection
options. Default stress targets retain their existing budgets and build flags.

Actual first API31 ARM software-guest results, under the explicit Linux mapper
helper plus scalar-host generator experiment, Enforcing, stock adbd Root:

| Separate test | Actual result |
|---|---|
| Existing100ms context stress | All27 context cases reached their expected state/restore results; subsequent3s supervised ELF-load case failed/UNKNOWN. Whole executable exit1, not32-case PASS |
| Existing tight supervisor stress | Cases0–2 passed; scenario3 reached ATTACHED but not CALLING before100ms, safely refused/live, expected post-CALLING containment not proven; exit15 |
| New product-budget ARM fixture | Normal function and three intentional failures passed their expected return/containment outcomes. Actual ELF load exceeded the overall limit, UNKNOWN/target stopped; whole executable exit1 |
| New product-budget supervisor | All10 cases passed, including post-CALLING timeout, worker exit, nonleader trace and pre-CALLING preservation; exit0 |

No uncertain load was retried on its old child or journal. Each suite used new
fork targets, a new staging directory and append-only attempts. adbd was then
unrooted and actually re-read as UID2000 on the same boot/API/ABI/Enforcing.
These results do not qualify ordinary App Root, JNI, game or product UI.
See [the full Linux evidence](../docs/API31_LINUX_HOST_RESEARCH.md).

## Separate ELF-load diagnostics

`SEED_ARM64_LOAD_DIAGNOSTICS=ON` additionally builds
`arm64_load_diagnostic_test`; it requires ARM64 and the explicit product-budget
research option. Default product builds do not create it. It takes no target
arguments, creates a new own fork child and attempt, and runs only the ELF case.
It retains the same15s supervisor,2s calls, snapshot, ACK and UNKNOWN policies.
It adds elapsed-time markers around manager initialization, attach/snapshot,
durable CALLING, injector initialization, ELF entry/export call and final state.
Logging can affect timing; diagnostic results are not substituted for the
unmodified acceptance fixture, and an old uncertain child is never reused.

The initial diagnostic SHA256 is
`842a7e6750e9e1159aeb8a675dbd03e4e5362ad8e44d859a4c8df241651fa508`.
It was compiled, not executed in Android at this checkpoint. No APK, Root,
game/profile gate or delivered artifact was changed by this research target.
