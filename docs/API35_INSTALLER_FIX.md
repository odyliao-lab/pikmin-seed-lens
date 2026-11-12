# Android 15 安裝確認修正

## 問題與修正

[發布後實測](UPDATE_0142_TO_0151.md)發現 0.14.2 在 API35 可下載新版，但建立的 Activity PendingIntent 未明確授予 creator 啟動能力，系統以 BAL_BLOCK 阻擋安裝回呼。0.15.1 沿用同一實作。

修正版只在本次安裝 session 的明確、非匯出回呼 Activity 上設定 `setPendingIntentCreatorBackgroundActivityStartMode(MODE_BACKGROUND_ACTIVITY_START_ALLOWED)`；API33 以下不呼叫此 API。依[Android 官方規則](https://developer.android.com/guide/components/activities/secure-bal)，target35 須明確選用 creator 權限。沒有更改 targetSdk、系統設定或其他通知 PendingIntent。

仍要求 Android 使用者確認；保留原套件、較新版本、原簽章、SHA256、內建 native 封裝、session 身分及載入互斥核對。沒有 Root 靜默安裝或自動確認。缺少確認 Intent 或無法開啟時，取消該 session 並提示重試；若系統拒絕取消，保留 pending 供再次取消，不誤報成功。

47 項主機回歸直接編譯產品 `LensUpdateInstaller`、`LensUpdateInstallActivity`、`UpdateInterlock`：API28／31／32／34／35 callback 選項、USER_ACTION_REQUIRED、重複安裝、跨 session、系統取消、缺失／無法開啟確認、取消失敗、驗證拒絕、安裝權限、載入互斥及完成後清理。Android 與 APK 驗證為替身，裝置驗收另列，不能由此宣稱系統確認成功。

## 隔離驗收方法

公開更新索引維持已發布 0.15.1。為了在不發布測試包、不覆寫已交付版本的情況下驗證更新發起端，先建原簽章 0.15.2/code56，再建同一修正、僅版本與 build ID 不同的 0.15.3/code57 作為較新版目標。

只在既有隔離 MuMu App 私有目錄準備測試快取（目標 APK、對應 SHA256／大小／原格式 manifest）。這是安裝階段 fixture；不新增產品測試入口、不修改固定網路來源、不繞過產品 `verify`。0.15.3 尚未在公開 URL 提供，**本輪不將測試快取當成公開下載成功**；網路查詢／下載已有前輪兩台使用正式0.15.1的證據。

一般使用者日後仍是 App 查詢／下載後交給 Android 確認安裝，不需要這些驗收工具。舊0.14.2／0.15.1的發起端不能被新版下載檔回溯修復；API35使用者首次取得修正版仍需從發布頁手動覆蓋安裝，不卸載、不清設定。

## 裝置結果

2026-11-12，在既有隔離 MuMu index5／SeedLens-Isolated-A15（建立時間1794181954477010）完成；本輪 boot `4966e09d-15eb-4d64-9759-8726ba990013`，實際 API35／x86_64，App UID10063。遊戲仍為157.0/code1793360009，保持關閉，沒有遊戲 UI／載入測試。

| 驗收項目 | 實際結果 |
| --- | --- |
| 修正版建立安裝 session | 產品重新核對較新版 APK，再開系統確認；不是 ADB／Root 安裝目標版 |
| API35 確認視窗 | **通過**；com.android.packageinstaller 顯示「要更新此應用嗎？」 |
| 系統取消 | **通過**；按取消，保留0.15.2/code56，App顯示取消／原設定保留 |
| 取消後重開 App | **通過**；取消狀態保存，確認安裝可再使用 |
| 重試並完成升級 | **通過**；再次顯示系統確認，按更新後為0.15.3/code57，同UID10063，installerPackageName=dev.ody.seedpreview |
| 設定與歷史 | **通過**；升級前後整份lens-interface鍵值一致，包含花苗ON／其他OFF、未授予自動套用、外觀及原retry-barrier歷史；自動查詢OFF保留 |
| 完成與重開 | **通過**；顯示更新安裝完成，重開仍為0.15.3；session／install_code已移除，update-freeze不存在 |
| 原簽章／安裝後位元組 | **通過**；讀回base APK與原簽章目標檔SHA256一致 |

兩個 session（1215338394取消、894379775重試）的系統紀錄為 `BAL_ALLOW_VISIBLE_WINDOW`；仍有平台 creator-only hardening 說明紀錄，沒有把所有系統警告描述為消失。實際可見的確認視窗及完成後版本才是通過證據。主機API32分支有回歸，**本輪未重跑API32裝置**，也沒有手機或遊戲功能的新資格。

0.15.2、0.15.3完整原簽章建置（JVM、雙ABI loader、renderer、封裝與digest）通過。兩個版本各26,658,424 bytes，原簽章憑證SHA256 `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。

- 0.15.2/code56：`bbc76954228edfa8b871154069f0cce1b6644f13578268f2b23864854e6c205e`。
- 0.15.3/code57：`69464a224086c10cbb5ffa5e455bf91801449bce8e504d9e7ad026edb0bb3129`；安裝後讀回完全相同。

## 證據與收尾

本機忽略目錄 `.tools/installer0152-device/` 保存App自有前後備份、安裝後APK及截圖：`api35-fixed-system-confirm.png`、`api35-fixed-cancelled.png`、`api35-fixed-installed.png`、`api35-fixed-settings.png`與`api35-final-public-check.png`。原始資料不進Git／不公開；主機測試與裝置結果分開記錄。

實測確認／安裝期間ADB維持shell UID2000；只在備份及準備／還原fixture時暫用adb root。結束前還原原公開更新快取，重新由App查詢正式索引，顯示「本機版本較新；已發布版本為0.15.1」。測試快取沒有留作使用者依賴，沒有改公開索引或發布任何測試包。

最後由App關閉五項功能，自動查詢仍OFF，Android安裝來源許可還原原本OFF，保留0.15.3與既有歷史。index5正常關機；其他MuMu／營運排程未操作。本次依使用者要求，在裝置通過後才commit/push，不自行合併或發布。
