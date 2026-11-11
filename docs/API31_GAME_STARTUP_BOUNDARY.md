# API31遊戲啟動指令界線（2026-11-09）

API35／156.1產品畫面階段結束並正常關閉新MuMu index5後，重新以既有私人Magisk31.1 ramdisk cold boot `SeedLens_API31_x64`／emulator-5580。原SDK ramdisk及私人override SHA均核對不變，2vCPU／4GB，沒有wipe或修改共享映像。

本次boot `30e9dc85-2ed9-4268-9875-f97d9797a501`，實際API31、x86_64、libndk_translation.so、Enforcing，普通App PATH su-M再次通過。安裝完整156.0/code1792142832 base／ARM64 split及同一原簽章0.4.1單APK（SHA `815de868e747111c9e9739ea9987337c53441e10a15e585d2192cecab4741cce`）。安裝不是遊戲驗收。

## 原遊戲在透視鏡載入前崩潰

原遊戲首次PID4159／UID10149，在`Loading.AsyncRe` thread發生SIGILL。Unity自己的crash backtrace指向未改動的ARM64 libunity.so offset `0x599200`；直接從已驗簽的156.0 split抽取並反組譯，該位置為`d50320bf`（SEVL），下一指令為`d503205f`（WFE）。libunity完整SHA為`38dc64f4667a69e99c7882439249edace536b38fcefa752ee541e879c9d96f04`。

此時未呼叫透視鏡啟用、沒有product attempt目錄；不能歸因或宣稱為產品hook結果。原Game ELF／metadata、bridge、SELinux均未修改；沒有跳過指令或攔截SIGILL。

## 獨立App重現及API35同APK對照

新增`dev.ody.seedlensarmhintprobe` 0.0.1/code1，外部lab簽章（非產品key）。只含ARM64 JNI library，沒有Root／網路權限；先durable寫BEFORE，再執行SEVL或SEVL＋WFE。三秒alarm只終止自有probe，不干預遊戲。每個模式使用新程序。

| 同一research artifact | SHA256 |
|---|---|
| hint APK | `dbd09df2fb75911d31e13d84c40a399f8cbc3cf2e2952509122d8dff43db1641` |
| ARM64 JNI library | `e99276ba1d42125fa5748468e9d1ed92e82b38ea8ab2eaf156b60521fa23bb5b` |

| 平台 | baseline JNI／native API | SEVL | SEVL＋WFE |
|---|---|---|---|
| API31／libndk_translation／Enforcing | PID4622、UID10151，RETURNED／instrumentation -1 | PID4656，保存BEFORE後process crashed／instrumentation0 | PID4697，同樣BEFORE後process crashed／instrumentation0 |
| API35／libnb／原Permissive | RETURNED／instrumentation -1 | RETURNED／instrumentation -1 | RETURNED／instrumentation -1 |

API35對照只重開同一已授權index5，現查相同created timestamp／端點、新boot `244903e1-7019-4eb6-bae4-e99697a170cb`及實際API／bridge，沒有操作原四台。沒有因ADB exit0把crashed結果算通過。

### 0.0.2：相同signal／alarm的嚴格基準對照

0.0.1的baseline在signal／alarm設定前返回，尚不能單獨排除保護機制的影響。保留原artifact，另以新0.0.2/code2把baseline也走過相同signal／alarm設定及取消；只有SEVL／WFE指令分支不同。產品、遊戲與translator沒有變更。

| 同一0.0.2 artifact | SHA256 |
|---|---|
| hint APK | `d52c3f8e379c3d515f822775f267a05d5aad97c2e3d1da5c3074ed7893546e14` |
| ARM64 JNI library | `566a5b734dd25b7cb28c1329bf6f94ca137992b9fb189cecc00e25f7a8bf274f` |

| 平台 | baseline＋相同signal／alarm | SEVL | SEVL＋WFE |
|---|---|---|---|
| API31／libndk_translation／Enforcing，新boot `9135f9ac-b9f6-4471-b669-bb1c938c02d8` | PID1741／UID10151，RETURNED、instrumentation -1 | PID1882，BEFORE後SIGILL、instrumentation0 | PID2355，BEFORE後SIGILL、instrumentation0 |
| API35／libnb／原Permissive，同前述index5 boot | PID2998／UID10064，RETURNED、instrumentation -1 | PID3042，RETURNED、instrumentation -1 | PID3070，RETURNED、instrumentation -1 |

API31 headless重開的是同一已授權AVD，未新增環境。三個模式各用新程序；兩筆失敗的durable receipt均停在BEFORE，crash記錄為SIGILL而非alarm。這排除了「僅signal／alarm設定就崩潰」的替代解釋，仍只代表這個精確image／bridge的能力界線。API35全部RETURNED也不是原生ARM64或產品四組資格。

兩邊自有App receipts已保存。API31原16份研究attempt SHA逐一不變、仍無product attempt目錄；adbd恢復UID2000後正常關閉AVD。0.0.2原始證據另存ignored `.tools/lab/api31-hint002-4e9d0905e8bb497db8b70d1009922dc5/`，API35對照沿用下述私人資料夾。

API31該bridge檔案`/system/lib64/libndk_translation.so` SHA為`c05f2b7ef51d3c855576ae53c25f7511d0490695ca1bf6b16269627be6dbdb51`。這個結果定位目前image的指令能力缺口；先前JNI／namespace與普通App載入研究仍成立，但不足以證明可運行完整Unity遊戲。不能單純放寬Java／native版本判斷、用API32替代、把遊戲指令改成NOP或加額外模組來冒稱API31成功。

原始crash、反組譯、自有App receipts及APK僅在ignored `.tools/lab/api31-game1560-product041-cc84fc3b924642b0a2444390c3376121/`、`.tools/lab/api35-arm-hint-3e85c3fd673e4abf902dfb45753e77d8/`。API31資料與所有歷史attempt保留，恢復adbd non-root後正常關閉。

## 156.1獨立實際啟動：同樣未到產品載入

沿用同一已授權AVD、相同私人ramdisk及stock ramdisk SHA，另次headless cold boot `830a2982-53f0-494d-8c8c-4a51ce890819`，仍API31／x86_64／libndk_translation／Enforcing。普通App UID10148的PATH su-M再次exit0、Magisk domain、同PID1 mount namespace；沒有用adb root代替普通App Root。

將該隔離機遊戲由156.0正常升級為原簽章156.1/code1793076318，完整base／ARM64 split在PM位置的SHA與Play intake逐一相同。產品仍為同一0.4.1／SHA `815de868e747111c9e9739ea9987337c53441e10a15e585d2192cecab4741cce`，沒有啟用產品。

解析PM實際launcher後只啟動一次，首程序PID4221／UID10149約12秒後退出。Unity自己的第一段crash為Loading.Preload／TID4360／SIGILL，PC `libunity.so+0x5e2234`；同PID translator明確記錄undefined instruction `0xd50320bf`。從已核split抽出的未修改ELF反組譯，該offset為SEVL，下一條為WFE。系統隨後另記同程序的SIGSEGV／null PC；兩段記錄均保留，不把後一訊號冒充第一段原因，也不宣稱已完成登入。

後續pidof曾回報另一程序4400，沒有驗證其就緒、重新導引登入或嘗試產品載入；不能據此把首程序崩潰算成功。16份研究attempt在boot前後及本次遊戲啟動後SHA全數不變，仍無product attempt目錄；bridge SHA亦不變。adbd恢復UID2000後正常關閉這個AVD。新證據只在ignored `.tools/lab/api31-game1561-product041-1758e0434c004ea8b8c02e929e0015f4/`。目前兩個指定遊戲版都在這個精確API31 x64環境遇到產品載入前的啟動失敗，兩列的產品兩頁驗收仍未通過。

## 下一環境：撤回x64 host上的ARM64 AVD提案

官方[Google SDK image清單](https://dl.google.com/android/repository/sys-img/google_apis/sys-img2-3.xml)實際列出API31 google_apis/arm64-v8a rev11，archive `arm64-v8a-31_r11.zip`、1,415,899,687 bytes、SHA1 `21bcb00f4ac3dc91c4040da7980b8c5f3c681781`；本機SDK亦含ARM64 QEMU執行檔。但這不證明Windows frontend接受ARM64軟體模擬或效能足夠。

原先提出`SeedLens_API31_arm64`／emulator-5582／D槽資料／2vCPU／4GB方案，尚未下載image、建立或啟動。進一步只讀核對發現，本機emulator37.2.13（exe SHA `06af3c5da3c2cef7266822fe653f54371fc746d78db6f4282e1b2338a70970c8`）內含x86_64 host／image架構不符的拒絕訊息；[官方來源的明確檢查](https://android.googlesource.com/platform/external/qemu/+/ae9d18d2b6261179fbd57fffec720a04f7bfb053/android/emulator/main-emulator.cpp#1050)在x86_64 host對arm64且API>=28直接拒絕。這是來源與本機靜態證據，沒有把未執行的新AVD寫成實測boot失敗。

因此撤回這個本機ARM64 AVD提案，不因QEMU執行檔存在就下載大型image，也不繞過frontend或修改SDK。已改詢問可用的真API31 ARM64測試手機或ARM64主機；取得具體目標與隔離操作範圍後，先核原生能力、Root及自有fixture的正常／失敗收容，再進產品驗收。兩個ABI分開記錄，不把現有x64缺口當成原生ARM64的結果。

兩版原簽章split內libunity：156.0為上述`38dc64…`，156.1為`69614a13a3200aad5964eb6698ea832e8397f562028128afd785b75abbdf9cc2`，兩者同為23,635,664 bytes但SHA不同。上方156.1結果來自獨立實際啟動及其精確crash PC，不由相同大小或156.0的crash推論。
