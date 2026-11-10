# API34 AOSP ARM64/Linux: actual research boundaries

2026-11-10. This addresses **Android14/API34 native ARM64**, one part of the
Android12/14/15 × game156.0/156.1/157.0 goal. It does not replace API31 or API35
evidence. The owner approved the [fixed AOSP scope](API34_AOSP_ARM_LINUX_PROPOSAL.md).

## Preceding Google-APIs guest closed

The independent single-thread Google-APIs run started UTC08:36:17.551 and
never produced a completed boot. Stock Watchdog killed system_server494,
712 and1005 at local16:46:58,17:02:24 and17:15:23. Earlier trace snapshots of
idmap Binder work/GC and idle UI/display threads differ from final annotated
stacks; they do not establish one permanent graphics deadlock. No watchdog,
clock or security policy was altered to get past these failures.

Final ordinary-shell metadata completed UTC09:21:00.641: UID2000, API34,
arm64-v8a, Enforcing and the same boot; boot flag remained empty. Console
normal-close returned0; actual QEMU/gdb exit0 at09:21:01.940, without timeout
or containment signal. Original AVD/data and diagnostics remain separate.
No Magisk, research APK, JNI or game was installed/executed in that run.

## Official AOSP intake and independent host scope

The separate official `system-images;android-34;default;arm64-v8a` revision4
archive matched705115396 bytes and official SHA1
`d9f2011131919abe952814e041f16f317242c3fa`; computed SHA256
`1447958a4c6747c44390ac5f5f4c894be6d1dfce93868a0385a95c5f0ae4c339`.
The safely extracted package has its own per-file hashes. No Google-APIs SDK
image was replaced. The factory userdata is an all-zero1MiB placeholder;
the new AVD received a fresh6GiB ext4/encryption-capable image, readonly
fsck exit0. No other AVD data, account or credentials were copied.

The AOSP-only mapper has a distinct marker, AVD,5590 port and SHA256
`f4b738fdafbffdb3c8f0aec7c0e16e5da6234a33e9fd19ba978452f937ceab88`.
It requires the explicit API34 and single-thread research scopes. Actual C
guards passed13 existing/default,23 Google-single and26 AOSP cases; Python
target tests passed10 methods. Real GDB wrong-executable/preload cases both
refused before `starti`. Original API31, Google API34 default and single
helpers rebuilt byte-identically. This is host-only research, never an APK
dependency or a broader product admission rule.

## New target's own CPU gate

Fresh Windows free physical memory was14.73GiB; Windows/Linux5590/5591 were
free, no QEMU guest existed, and all four official AOSP image hashes matched.
New `SeedLens_API34_AOSP_ARM64_Linux` ran the original V3c CPU fixture at
UTC09:34:41.810–09:34:54.826,2 guest vCPUs/4GiB, normal TB/single host-thread
TCG, read-only data requested. Actual seven tests passed, zero failed/skipped;
intentional NULL SIGSEGV and child-reaped control also passed. Fourteen host
vector wrappers registered while real AVX flags remained1/1. Normal guest
power-off, actual inferior exit0 and gdb exit0, no timeout/containment.

GLAsyncSwap/stop/protected-range-unmap SDK warnings remain in private logs.
This uses a custom CPU initramfs, not Android framework, App or game success.
The subsequent Android run must initially use the original AOSP ramdisk, and verify
actual API34/ARM64/Enforcing, full boot, PM/AM/display and stability before
Magisk or App tests. AOSP has no Google services; game login is a separate
unproven gate. All three delivery stages and all API34 game rows remain open.

## Independent full Android boot: in progress

UTC09:37:14.662 started the new AOSP AVD's first full Android boot. Its10800s
host bound was selected before launch, with a separate normal-close timer;
there is no live deadline extension. The original AOSP ramdisk is used:
**no APEX worker override, Magisk or other private ramdisk**. Both early and
APEX BoringSSL self-tests actually returned0, and32 APEX packages mounted.

The first ordinary-shell read returned actual UID2000/Enforcing/API34,
ARM64-only/aarch64/NativeBridge0 and fresh boot identity. Official runtime
fingerprint is
`Android/sdk_phone64_arm64/emu64a:14/UE1A.230829.036.A1/11228894:userdebug/test-keys`.
The whole read exited1 because its final `pidof system_server` was still
empty; do not call that command a complete successful platform proof.
System_server535 later entered normally. PackageManager reconciled179 app
data packages, then began first-boot dexopt at UTC09:46:01. A preceding
Watchdog WAITED_HALF warning did not yet become a kill; stock Android itself
paused its main checker for dexopt. The research code did not change Watchdog.

That dexopt resumed the stock checker at UTC09:52:56 and reached services
ready, but at09:55:49 Watchdog killed system_server535 for a main-handler
wait of101s. Its final annotated stack was `CompletableFuture.get` inside
`PermissionPolicyService.grantOrUpgradeDefaultRuntimePermissionsIfNeeded`,
called during user/boot-phase setup. PermissionController925 subsequently
reported DeadSystemException after the server died. That consequence does
not establish an earlier independent PermissionController crash or the root
cause of its delayed callback. This is a **failed first framework attempt**,
not a completed boot or permission initialization.

The OS automatically created system_server1099 at UTC09:57:07 in the same
guest boot. It reconciled179 app-data packages and passed its subsequent
boot-after-mainline-update dexopt in about11s. First-boot compilation and
cache state differ, so this is recovery progress, not a clean-boot benchmark.
The single original QEMU/host deadline remains; no manual guest restart,
timeout extension, permission-state bypass, extra package or Root operation
was performed. The next check is actual boot completion and stable services.

Full boot still requires flag1 plus two fresh same-boot/fingerprint/server
PM/AM reads and actual visual display review. Conditional proof/capture tools
are waiting on these real results; they do not start Root, App or game work.
No platform or product row is promoted before those gates pass.

Official31.1 standalone magiskboot also decoded this original AOSP ramdisk
on the host and reproduced its build.prop bytes, with the input hash
unchanged and no APEX worker property. This was read-only host preparation:
no patch, guest file push, Root transition or provider installation occurred.

System_server1099 was also killed at UTC10:01:45 after a107s main-handler
wait, with the same PermissionPolicyService/CompletableFuture annotation.
The OS next created server1546 at10:03:11. This repeats the observed waiting
site, but still does not prove whether callback execution, scheduling, code
initialization or another dependency caused the delay.

## Bounded standard ART-cache experiment: requested, not a fix claim

After fresh UID2000/API34/ARM64/Enforcing/same-boot checks, the actual package
CLI's own help showed `compile -m speed` and background priority support.
The help returned255 despite printing the complete text; the strict reader
stopped and the saved output was reviewed, rather than treating it as a
successful mutation. Two earlier read-only path queries during server
reconstruction correctly reported no package service and performed no work.

At UTC10:06:45, `cmd package art dump com.android.permissioncontroller`
actually reported ARM64 `status=verify`, `reason=first-boot`, for the original
PermissionController APK in the official permission APEX. Android14's
[ART Service](https://source.android.com/docs/core/runtime/configure/art-service)
supports on-device AOT compilation; this suggests a cache experiment, not
proof that interpretation caused the Watchdog failures.

At UTC10:07:36.956 exactly one ordinary-shell request was issued:

```
cmd package compile -m speed -p PRIORITY_BACKGROUND --primary-dex com.android.permissioncontroller
```

This targets one system app's data-side compiled cache. It requests no force,
profile reset, package install, permission flag override, Root, SDK/image edit,
Watchdog/clock/SELinux change or product guard change. The original failure
receipts remain. Preserve an uncertain/failed result instead of automatically
resending. Actual command completion, ART `status=speed` readback and later
boot/service/display results are separate pending checks. Even a completed
cache experiment does not qualify ordinary-App Root, JNI or any game.

Server1546 was subsequently killed at UTC10:08:37 (final annotation follows
the same permission-initialization wait). At a later ordinary-shell process
read, same boot/UID2000/Enforcing were verified and the one requested job was
still real: shell cmd1868, artd1874 and its dex2oat64 child1892, with the exact
PermissionController input path and `--compiler-filter=speed`. This is a
verified live compiler after the third server failure, not a completed cache,
successful command or boot. The original1200s compiler observation bound
and10800s guest bound remain; no second request was sent.
