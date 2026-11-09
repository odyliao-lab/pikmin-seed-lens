# API31 ARM64完整guest／Windows軟體CPU研究（kernel／init已達、App環境未就緒）

使用者已同意官方API31 ARM64隔離研究，後續也確認目前沒有可用的ARM64實機／主機。標準Windows frontend方案先因架構限制撤回；完成目前API35可取得樣本的測試後，另檢查同一官方SDK內的ARM64後端，仍不代表官方支援此組合或產品已相容。

研究目標沿用已確認的空白`SeedLens_API31_arm64`、`D:\SeedLensLab\avd`、2vCPU／4GB。因API34研究先使用5582，本目標預定5584／5585並在啟動前重查；後續不能只憑serial認設備，須重核AVD名稱／boot／實際API／kernel及程序架構。原四台與台中設備不操作，不複製VM或登入資料。

## 官方artifact與實際入口結果

官方[SDK metadata](https://dl.google.com/android/repository/sys-img/google_apis/sys-img2-3.xml)的`system-images;android-31;google_apis;arm64-v8a` rev11、archive `arm64-v8a-31_r11.zip`／1,415,899,687 bytes／metadata SHA1 `21bcb00f4ac3dc91c4040da7980b8c5f3c681781`已由SDK manager安裝。新增ARM DBT SDK條款限應用開發／除錯用途，本研究只作相容性開發與隔離測試。實際source.properties為API31／arm64-v8a／rev11；原SDK ramdisk SHA256 `401dd0cfe27762bc353a3459e3d52c6156968ec6825be52b0a9cddb6ffba76e3`。尚未patch ramdisk、安裝Root或產品。

標準emulator37.2.13實際啟動已exit1，明確報ARM64 image在x86_64 host不受支援；這次是runtime拒絕證據，沒有只以source字串推論。沒有修改frontend或產品准入規則。

另將官方後端需要的SDK DLL位置放入本次process PATH，直接以SDK內`qemu-system-aarch64-headless.exe`啟動同一空白AVD，維持軟體CPU與既有host OpenGL。該後端到達QEMU初始化後exit1：`PCI bus not available for hda`，沒有guest ADB／Android boot或JNI成功。原process18748已結束，未建立產品attempt。

## 固定來源與實際virt主板比較

固定[官方QEMU glue來源](https://android.googlesource.com/platform/external/qemu/+/ae9d18d2b6261179fbd57fffec720a04f7bfb053/android-qemu2-glue/main.cpp)顯示：ARM guest在ARM host選`virt`主板，其他host選舊`ranchu`；API>=26預設加入HDA聲音裝置。[soundhw來源](https://android.googlesource.com/platform/external/qemu/+/ae9d18d2b6261179fbd57fffec720a04f7bfb053/hw/audio/soundhw.c)會在PCI裝置沒有PCI bus時拒絕。這與本次錯誤相符，但尚未證明改主板即可開機。

2026-11-09 15:32:36，先正常關閉index5並核埠空閒／記憶體餘裕後，實際執行同一後端及image的公開QEMU `-machine virt -smp 2`比較。2vCPU／4GB、軟體CPU cortex-a57、host OpenGL／Vulkan OFF；沒有修改SDK、kernel、遊戲指令、SELinux或偽造API。

本次已到達guest kernel及Android init；guest約73秒起，`keystore2`反覆SIGSEGV／SEGV_MAPERR，位於`libpuresoftkeymasterdevice.so+0xc438`的`PureSoftRemoteProvisioningContext::GenerateBcc(bool)+144`，後續stack含PureSoftKeymasterContext／libkeymint。沒有觀察到adbd／zygote／完整開機就緒，指定serial的ADB一直offline，故**沒有實際Android API／JNI或正常App能力證據**，image標籤與kernel開機不能代替。

console仍可回報已核對的`SeedLens_API31_arm64`名稱。多次相同crash後，重核host執行檔／AVD／埠，使用指定serial的正常`emu kill`關閉；host程序與5584／5585 listener均已消失。沒有安裝Root、probe、遊戲或產品，原SDK ramdisk SHA與上方完全相同。未建立產品attempt，沒有清除既有UNKNOWN。下一步須針對CPU／主板與keymaster實際來源定位，不任意停用安全服務或修改guard。

即使後續boot成功，也只是**完整ARM64 guest在x64 host的軟體CPU模擬**，不是原生ARM64實機資格，也不是x64/NativeBridge的成功。須以實際kernel／Java與native API、ARM JNI、CPU context／Root／正常與失敗收容全部實測，再進產品；不能用image標籤、process啟動或編譯通過代替。

原始資料與本次腳本只在ignored `.tools/lab/api31-arm64-0bc610f9cca14c298aa7b0a2f4c2890b/`。標準frontend限制仍成立；目前API31三個遊戲組合均未通過，先前API31 x64的156.x SEVL失敗及所有UNKNOWN亦未改寫；157未在本ARM64 guest實測；x64另次原遊戲SEVL失敗見[157獨立啟動](GAME157_PLATFORM_STARTUP.md)。
