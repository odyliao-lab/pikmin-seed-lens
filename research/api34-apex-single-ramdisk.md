# API34 private APEX concurrency comparison

Linux host research only; not an APK, module, stock-image claim or end-user
dependency. Independently verify authorization and the exact disposable guest.
Targets are the separately approved `SeedLens_API34_ARM64_Linux` and the
owner-approved `SeedLens_API34_AOSP_ARM64_Linux`; each keeps its own exact
image-family pin and candidate. This does not authorize other guests.

Actual guest serial evidence showed24 APEX activation failures/0 activated,
including Conscrypt/adbd/resolv, with device-mapper creation/path waits. The
SDK `-prop apexd.config.boot_activation.threads=1` attempt issued a warning
that only qemu properties are supported; the requested value was never read
back, and APEX failures remained. Do not report that setting as effective.

Android14's [APEX source](https://android.googlesource.com/platform/system/apex/+/refs/tags/android-14.0.0_r1/apexd/apexd.cpp)
has an official activation-worker-count property. The [init source](https://android.googlesource.com/platform/system/core/+/refs/tags/android-14.0.0_r1/init/property_service.cpp)
loads ramdisk second-stage properties, with the path defined in
[second_stage_resources.h](https://android.googlesource.com/platform/system/core/+/refs/tags/android-14.0.0_r1/init/second_stage_resources.h).
These are source comparisons, not an assertion of the SDK's complete build manifest.

`build_api34_apex_single_ramdisk.py` requires the exact official API34 ARM64
rev14 ramdisk SHA256 and a new output outside its SDK directory. It appends
one legacy-LZ4/newc archive to the unmodified original compressed bytes. The
effective entry `system/etc/ramdisk/build.prop` retains its original bytes,
mode/owner/time and receives only `apexd.config.boot_activation.threads=1`.
All other effective entries and original compressed prefix are checked equal.
It changes no init executable, fingerprint/API, security property or timeout.
It does not install Magisk or grant ordinary App Root.

```sh
python3 research/build_api34_apex_single_ramdisk.py \
  --sdk-ramdisk /opt/seedlens-lab/sdk/system-images/android-34/google_apis/arm64-v8a/ramdisk.img \
  --output /opt/seedlens-lab/api34-apex-one-v1/ramdisk.img
```

Initial private candidate SHA256 is
`ccba894ee41e3388f4228949418be61c728fc02c79caa631241ddf6fa1c3e072`.
32 effective entries were checked, with one changed entry. Real negative
invocations refused an API31 input, an output within the SDK image directory,
and an existing candidate; SDK/candidate hashes remained unchanged.

Use only an explicit `-ramdisk` override and retain the official SDK image,
all failed runs and cold backup. The next guest must read the actual property,
API/ABI/kernel/SELinux and boot state before claiming it applied. A successful
host build or valid archive alone is no boot, App, Root, JNI or game evidence.
The comparison retains the existing dm wait, verification, Watchdog and
product15s/2s budgets. No product/NativeBridge admission is modified.

## Separately pinned AOSP rev4 candidate

The later AOSP original-ramdisk/no-animation boot reproduced device-mapper
path waits; netd and zygote then lacked their unmounted APEX libraries. ADB
never became ready. That early failure prevents evaluating the animation
change's effect on the later PermissionController/Watchdog problem.

`--aosp-research` selects only original AOSP ARM64 rev4 ramdisk SHA256
`91d2259c141607b5fda903afa4d0a6912389753f40e24015aed74db19fd6bd6c`.
The default continues to select the original Google-APIs rev14 pin; neither
flag accepts the other family's input. There is no caller-supplied hash or
arbitrary property. Actual wrong-family/unscoped, SDK-directory-output and
existing-output invocations refused. The default Google candidate rebuilt
byte-identically to the earlier `ccba894e…3e072` artifact.

The new AOSP-only candidate SHA256 is
`7b5da5b6c43f3f53c11fcff19de299540b0ea502f2b98f2435b271232cde31d4`.
The same entry/metadata/prefix checks passed. Independently, official31.1
standalone magiskboot decoded it and reproduced both original init bytes
and the original build.prop plus exactly the one worker property. This is
host-only validation, not Magisk installation or actual property/boot proof.

Use the separately pinned AOSP launch/proof guards, retain2vCPU/4GiB and
1080×1920/420dpi, and verify runtime behavior. No other module, Root provider,
permission-state bypass, API/fingerprint, clock or Watchdog change is included.
