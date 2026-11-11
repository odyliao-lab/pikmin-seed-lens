# 診斷、同意回報與版本更新

2026-11-11；0.11.2/code43 開發候選。以未合併的 0.10.3 工作為基礎，保留四組工具、原簽章及設定；沒有公開發布新 APK。

## 介面與自我排除

工具頁保留四個直接開關，新增「診斷」頁籤。診斷頁先讀取 Android/API、CPU、遊戲精確版本、App 簽章及自有原生檔案配對；使用者按「檢查 Root」並確認後，才呼叫既有 su 讀取 UID0、NativeBridge 與 SELinux。沒有安裝、注入、遊戲 RPC 或系統修改。

提示區分 Android／轉譯後端不符、CPU 不支援、遊戲未安裝／未精確配對、Root 未檢查、找不到 su、未授權或呼叫模式不相容、逾時、App 內建檔案異常，以及曾有未決載入。不能把 Root 失敗一律說成裝置沒有 Root，也不能把 ARM64 backend 的來源准入說成手機已驗收。版本與 UNKNOWN 守門不變。

長按桌面圖示可選「診斷與回報」。這個 Activity 使用獨立 `:diagnostics` 程序，不初始化主畫面 controller。Android30+ 只查自己主程序的退出分類，不讀描述、trace 或遊戲崩潰檔；API 較低／資料不可用時明列無法取得。

## 收集、同意與匯出

- 使用者選擇問題類型後，預覽完整 JSON，再勾選同意。取消／未同意不匯出、不上傳。
- 封閉白名單：軟體／平台版本、簽章摘要、Root／檔案／退出分類與最多 16 個列舉事件／載入退出碼。沒有帳號、定位、裝置序號、穩定追蹤 ID、截圖、完整 logcat、堆疊或記憶體。
- 主程序只在 App 私人檔案記錄少量列舉事件；不搬動、不改寫原生 UNKNOWN／attempt 歷史。事件沒有 session/PID 或時間，因此只能當歷史提示，不冒充當次程序證據。
- 每份報告使用獨立隨機 UUID；預覽的同一份資料才可送出。匯出使用系統文件選擇器，不要求儲存空間權限。
- 上傳僅允許固定 HTTPS 接收端、正常憑證驗證、上限 16 KiB、限制時間及回應大小，不跟隨轉址、不自動重試。必須核對 accepted 收據與同一 report ID 才顯示送出成功。

**依使用者選擇，目前只有接收端程式準備，尚無主機／網域部署。** `DiagnosticEndpoint.URL` 維持空白，上傳按鈕不可用；可先匯出並由使用者選擇私人管道提供，不能宣稱已送達。接收端配置、資料上限、去重、保存與部署前檢查見 [接收端文件](../server/diagnostics/README.md)。資料檔案保持 ignored，不進公開 repo。

## 檢查與安裝更新

以唯讀來源的控制中心更新流程為參考，只選擇性沿用 HTTPS 有界下載與精確 metadata 判定。獨立版流程為：檢查 → 更新說明 → 使用者確認下載 → SHA256／大小／套件／版本／原簽章／單 APK 原生配對核對 → Android 確認安裝。取消或失敗保留原版本與設定；不使用 Root 靜默安裝，不安裝核心 ZIP／模組。

索引固定為本 repo `main/updates/preview.properties`，APK 固定為共用發布庫中 `seed-lens-v<版本>/pikmin-seed-lens-<版本>.apk`。不能讀共用庫 latest 來誤認其他產品，也不接受任意下載來源、同版換檔、低於已見索引的版本回退或低於目前 App 的安裝。

每日自動檢查預設 OFF；使用者開啟後，從可見 App 啟動一次每日 metadata 檢查，不開機啟動、不自動下載、不傳送診斷。安裝透過 App 私人跨程序鎖與 freeze 標記排除載入進行中；更新等待確認時不開啟新的載入，取消後可恢復原本等待，不能把被暫擋的載入記成真正已嘗試。

本次索引只引用已發布且雜湊已確認的 0.9.5；沒有把開發候選加到正式發布。**索引檔仍在功能分支，必須合併到 main 才會啟用正式線上檢查。** 本輪實機對尚不存在的索引顯示檢查失敗，沒有假稱最新版本。較新 APK 的實際下載→Android 安裝成功／取消，仍需發布下一個原簽章版本後另驗。

## 驗證與資格

| 項目 | 結果／限制 |
|---|---|
| 診斷／隱私 policy | 48 項 JVM 檢查通過；包含不同 Root 失敗、平台與遊戲負例、資料白名單、未同意／無接收端不可上傳與收據核對 |
| 更新 policy | 34 項 JVM 檢查通過；包含跨產品 URL、非法 metadata、轉址拒絕、截止時間、原生配對與跨程序鎖 |
| Controller | 32 項 host 檢查通過；包含 OFF 排隊／恢復與更新取消後重新等待；Android／Root 為 doubles，不當作裝置驗收 |
| 私人接收端 | 7 組 Python 測試通過；實際 loopback HTTP、同意 header、禁止讀報告、格式／容量／頻率限制、去重與清除。正式 HTTPS／部署未驗 |
| 真實下載 | 以產品 LensUpdateClient 下載官方已發布 0.9.5，SHA256 與原生配對核對通過；沒有降版安裝 |
| 隔離 MuMu15/API35 | 0.11.2 已實測獨立診斷、Root 與內建檔案 match、同意控制、實際 SAF 匯出並通過接收端 schema、工具頁切換；0.11.1 另有真實長按捷徑證據，兩版使用相同捷徑資源。0.11.2／精確 Game157.0 另完成待種花苗 ON、已回報狀態與 OFF 原名／原字級還原，無 ADB 注入 |
| 其他平台 | 實際無 Root 裝置、Root 拒絕／逾時、手機/API32 的 UI 與原生載入未於本輪重驗；不能以規則負例當實境驗收 |

實際資料與截圖保留 ignored `.tools/diag0110-device/`，不加入 Git。0.11.0 的深色預覽對比問題已修正並以新版本保留舊產物；0.11.2 的首次 build 因 host 測試 double 缺少新回報路徑而中止，沒有 APK，失敗輸出已另存，修正 double 後 32 項檢查通過再建置。

## 最終產物與收尾

0.11.2/code43 APK SHA256：`40ff1f9177e5619760222916cd028accb9461d7170b143e6913431ac2ddc3272`，原簽章 SHA256 `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。本機產物保留 ignored `.tools/build/0.11.2/signed/`，未發布。

本輪只操作既有 index5／SeedLens-Isolated-A15，核對 name／created timestamp／端點／新 boot。原 49 份 journal 的 SHA 均不變，0.11.1／0.11.2 各一個新遊戲程序新增一份。最終四主值與八成員皆 OFF，auto-arm／pending-OFF false，自有服務停止，遊戲各 thread TracerPid0，沒有額外顯示模組；使用期間 adbd2000，取證後恢復同 UID 並正常關閉隔離機。原四台與台中設備未操作。

最終圖片索引：`final112-diagnostics.png`、`final112-root.png`、`final112-preview.png`、`final112-exported.png`、`final112-tools-armed.png`、`final112-seed-on.png`、`final112-applied.png`、`final112-all-off.png`、`final112-seed-restored.png`。UIAutomator 的 MuMu 退出碼139仍保留為平台工具失敗；使用實際 screencap、全新 XML、匯出檔與同程序回報交叉核對，沒有加裝 TalkBack 或其他測試 APK。

更新與載入互斥的取消情境由實際 controller 搭配 Android／Root doubles 執行；沒有在裝置上安裝較新版來冒充正向更新驗收。私人 HTTPS 接收服務、正式 main 索引啟用，以及未來較新版的 Android 安裝成功／取消，是後續明確待驗項目。
