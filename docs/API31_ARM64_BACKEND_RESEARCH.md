# API31 ARM64完整guest／Windows軟體CPU研究（未開機成功）

使用者已同意官方API31 ARM64隔離研究，後續也確認目前沒有可用的ARM64實機／主機。標準Windows frontend方案先因架構限制撤回；完成目前API35可取得樣本的測試後，另檢查同一官方SDK內的ARM64後端，仍不代表官方支援此組合或產品已相容。

研究目標沿用已確認的空白`SeedLens_API31_arm64`、`D:\SeedLensLab\avd`、2vCPU／4GB。因API34研究先使用5582，本目標預定5584／5585並在啟動前重查；後續不能只憑serial認設備，須重核AVD名稱／boot／實際API／kernel及程序架構。原四台與台中設備不操作，不複製VM或登入資料。

## 官方artifact與實際入口結果

官方[SDK metadata](https://dl.google.com/android/repository/sys-img/google_apis/sys-img2-3.xml)的`system-images;android-31;google_apis;arm64-v8a` rev11、archive `arm64-v8a-31_r11.zip`／1,415,899,687 bytes／metadata SHA1 `21bcb00f4ac3dc91c4040da7980b8c5f3c681781`已由SDK manager安裝。新增ARM DBT SDK條款限應用開發／除錯用途，本研究只作相容性開發與隔離測試。實際source.properties為API31／arm64-v8a／rev11；原SDK ramdisk SHA256 `401dd0cfe27762bc353a3459e3d52c6156968ec6825be52b0a9cddb6ffba76e3`。尚未patch ramdisk、安裝Root或產品。

標準emulator37.2.13實際啟動已exit1，明確報ARM64 image在x86_64 host不受支援；這次是runtime拒絕證據，沒有只以source字串推論。沒有修改frontend或產品准入規則。

另將官方後端需要的SDK DLL位置放入本次process PATH，直接以SDK內`qemu-system-aarch64-headless.exe`啟動同一空白AVD，維持軟體CPU與既有host OpenGL。該後端到達QEMU初始化後exit1：`PCI bus not available for hda`，沒有guest ADB／Android boot或JNI成功。原process18748已結束，未建立產品attempt。

## 已查來源與下一個尚未執行的比較

固定[官方QEMU glue來源](https://android.googlesource.com/platform/external/qemu/+/ae9d18d2b6261179fbd57fffec720a04f7bfb053/android-qemu2-glue/main.cpp)顯示：ARM guest在ARM host選`virt`主板，其他host選舊`ranchu`；API>=26預設加入HDA聲音裝置。[soundhw來源](https://android.googlesource.com/platform/external/qemu/+/ae9d18d2b6261179fbd57fffec720a04f7bfb053/hw/audio/soundhw.c)會在PCI裝置沒有PCI bus時拒絕。這與本次錯誤相符，但尚未證明改主板即可開機。

私人下一比較腳本僅準備好並通過PowerShell parse，尚未執行：同一後端及image，使用公開QEMU `-machine virt -smp 2`，保留完整kernel／stderr紀錄；不改SDK／遊戲指令或偽造API。啟動仍須先確認前一guest已關閉、埠空閒及記憶體餘裕。因使用者將在index5切換有樣本帳號，暫不另啟4GB guest，避免影響登入與實測。

即使後續boot成功，也只是**完整ARM64 guest在x64 host的軟體CPU模擬**，不是原生ARM64實機資格，也不是x64/NativeBridge的成功。須以實際kernel／Java與native API、ARM JNI、CPU context／Root／正常與失敗收容全部實測，再進產品；不能用image標籤、process啟動或編譯通過代替。

原始資料與未執行腳本只在ignored `.tools/lab/api31-arm64-0bc610f9cca14c298aa7b0a2f4c2890b/`。標準frontend限制仍成立；目前兩個API31產品組合均未通過，先前API31 x64的SEVL失敗及所有UNKNOWN亦未改寫。
