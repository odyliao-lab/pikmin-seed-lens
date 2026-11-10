# 待確認：API34 ARM/Linux隔離研究

這是原API34 x64 AVD之外的新目標，目前只完成官方metadata查核及方案，**未下載映像、建立或操作新guest**。API31 Linux的vector-save完整boot獨立觀察於25分鐘仍未完成，已核UID2000／Enforcing後正常關閉；沒有普通App或遊戲資格。先前scalar輪有Watchdog失敗，不能承諾API34成功。既有API34 x64三個遊戲的SEVL失敗及產品拒絕維持不變。目前繼續已授權新index5的API35產品回歸，新增API34 ARM/Linux仍待本人確認，兩者不並行啟動guest。

需要另確認的依據：[已核准Linux方案](API31_LINUX_HOST_PROPOSAL.md)第5項：「API34 ARM guest、新裝置、主機重開或本提案之外的主機變更另確認」。這次只請求下列具體新目標，既有API31工作仍可獨立繼續。

## 官方來源與具體目標

2026-11-10直接讀取[Google官方system-image metadata](https://dl.google.com/android/repository/sys-img/google_apis/sys-img2-3.xml)，逐層核revision而非將dependency版本混作revision：

| 項目 | 本次提案 |
|---|---|
| SDK package | `system-images;android-34;google_apis;arm64-v8a`，revision14／channel-0／API34／extension7 |
| 官方archive | [arm64-v8a-34_r14.zip](https://dl.google.com/android/repository/sys-img/google_apis/arm64-v8a-34_r14.zip)，1,610,393,229 bytes，約1.50GiB |
| metadata checksum | SHA1 `2fe8b46d419a3400e30f31b0152b241b50c8b99f`；下載後必核大小／此checksum，另記SHA256及逐檔manifest，不把metadata當guest實際API |
| Linux host | 既有專用WSL2 `SeedLens-TCG-Linux`，實體資料仍在`D:\SeedLensLab\wsl\SeedLens-TCG-Linux`，不新增Windows元件／driver／全域設定 |
| 新AVD | `SeedLens_API34_ARM64_Linux`，`/opt/seedlens-lab/avd/SeedLens_API34_ARM64_Linux.avd`，全新空白data，不複製API31／API35或正式機userdata |
| 資源／連接埠 | 2vCPU／4GiB、6GiB sparse data，預定Linux5588／5589，啟動前重核兩側listener／身分／資源；下載準備時D槽約818GB可用，執行前仍重讀 |
| emulator | 同已固定SHA的Linux SDK37.2.13後端、ARM軟體CPU／virt／SwiftShader。標準x64 frontend仍不支援此ARM image，這是直接後端研究，不冒稱官方前端或實體ARM64合格 |

## 限定執行順序

1. 本人確認後，等API31完成本輪、恢復adbd UID2000並正常關閉，才下載／建立API34。任何時候只運行一台新Android研究guest；新index5保持關閉，原四台及台中Libra／Virgo不操作。
2. 只傳入此官方SDK image與本repo自有fixture；不複製正式scanner、帳號session、憑證、keystore、Google或遊戲資料。保留原SDK files，另建data、metadata及每輪唯一receipts。
3. mapper／vector-save兩個**host研究變更**須新增明確的AVD／port allowlist與新marker／artifact，先完成錯目標／錯marker／未初始化等拒絕控制。不得將現有API31 guard改成任意AVD或產品NativeBridge寬鬆判斷。
4. 先以API34原kernel＋自有PID1 fixture驗實際kernel、CPU／SEVL／向量序列、signal及收容，再原官方ramdisk獨立觀察完整boot、實際API34／ARM64-only／NativeBridge0／Enforcing、system_server及套件服務。失敗或逾時如實保留，不停用Watchdog、改clock或冒充API。
5. 平台允許後，Root依同官方Magisk31.1方針，以此新機自己的私人ramdisk override與cold backup執行，保留SDK原件、SELinux Enforcing、Zygisk關閉、無顯示模組／LSPosed。不推論原廠adb root就是普通App `su -M`成功。
6. 驗普通App Root、ARM context／15s及2s既有產品界限下的正常與故障收容、真ART／JNI生命週期，才整合產品。研究App若需API34入口須升新研究version/code且沿用原研究簽章；不以API31或35參數冒充。
7. 原簽章精確156.0／156.1／157與單APK逐組另驗兩頁、長字、捲動重用、OFF及新遊戲程序。登入仍由本人；不領回／派遣／改物品／RPC／GPS。新產品bytes必升版本/code、保留原簽章；不把研究host元件變成產品依賴。

這是新增隔離研究範圍，沒有公開發布、main合併、force push或其他裝置操作授權。只有實際分層證據才能更新九組矩陣。
