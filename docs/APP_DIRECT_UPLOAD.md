# App 直接回報（0.12.1/code45 候選）

2026-11-11。診斷頁 → 預覽問題回報 → 選問題 → 預覽診斷與選填 Email → 勾選同意 → 同意並上傳 → 保留回報編號。不需登入、瀏覽器、手動 JSON 上傳或額外套件。離線匯出仍保留，且不包含 Email。

固定接收位置為 `https://seed-lens-reports.ody-liao.chatgpt.site/api/submit`。拒絕其他 host／path／query／轉址，維持系統 TLS 驗證；不接受使用者或 Intent 指定位置。報告封閉 schema 不變，Email 是獨立信封欄位，僅供修正通知，不用於行銷，也未啟用自動寄信。

Email 非法時停在預覽並提示更正或留空；修改地址後取消原同意。未勾同意不能上傳。30 秒總期限、連線／讀取逾時、512-byte 收據上限；只有 HTTP200／201 且回報編號精確吻合才顯示已收到。409／429／503 等分類提示，不印出原始例外或伺服器資料。沒有背景佇列、自動重送、登入 cookie 或內嵌密鑰。

失敗時顯示本機編號，提供「重試此份回報」；人工點選後重新預覽、保留原編號／地址並再次同意。改地址會與既有收件衝突，不覆寫原資料。重試資料只保留本次 Activity 記憶體，程序重建不自動恢復或上傳，請保留編號；也可先匯出 JSON 作為離線備份。

保存政策與網站一致：29 天後排除讀取，處理 API 請求時清除；尚無閒置時獨立清除排程。管理者讀寫仍須 Sites 登入＋外部 ADMIN_EMAIL，原生 client header 不賦予讀取權限。

## 驗證

| 項目 | 結果 |
|---|---|
| JVM policy | 49 項隱私／診斷、37 項上傳檢查通過；mock HTTPS 驗未同意／非法地址不開連線、Email 不改報告、200／201 收據、拒絕錯編號／多欄位／超大回應、302／400／403／409／429／503 無重試及斷線清理 |
| Sites policy | 4 組 Node 測試通過，原生契約不允許錯誤 Origin、非 JSON 或管理路徑 |
| 真實 HTTPS | 實際產品 DiagnosticUploader 在一般 JVM 取得兩份正式收據，分別空白 Email／合成 example.invalid 地址，沒有特殊 JVM flag 或轉址豁免 |
| Android 真機制 | 既有隔離 MuMu index5、SeedLens-Isolated-A15，created timestamp 1794181954477010，新 boot 99f612fe-16cf-4290-b322-34ff4c7ae68c，API35；原簽章由 0.11.2 升級 0.12.1 保留設定。實際 UI 驗初始未同意不可送、非法 Email 提示、改 Email 重設同意，最後同意直傳並顯示與本次報告一致的成功收據 |
| 原有功能 | 未開遊戲／啟用顯示、未掃描 Root、未重跑遊戲顯示驗收；不擴大 Android 或遊戲資格。原四台、台中、掃描器／排程未操作 |

MuMu 使用一般 App 自有 HTTPS 連線，不靠 ADB 上傳；ADB 僅用於安裝、UI 操作與取證。診斷中包含基本版本／App 簽章／錯誤分類，通知地址使用 example.invalid，沒有帳號、位置或原始 log。收據與畫面保留 ignored `.tools/diag0121-device/`，不提交 Git。關閉自有 App 後已正常關閉 index5。

尚未實測手機、API32 的上傳 UI、Android 真實離線／TLS 失敗、跨 Activity／程序重建的收據保留、正式非站長登入、寄信與到期排程；不把 mock、CI 或 APK 安裝當成這些驗收。

## 產物與失敗保留

- 0.12.1/code45 簽署 APK：SHA256 `06a49bc9b990e0692488820e80c0e59a910784cdbfef0490afdc7f574fc2bb91`。
- 原簽章 SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。
- 本機 `.tools/build/0.12.1/signed/pikmin-seed-lens-0.12.1-signed.apk`，不放 Git，未公開發布／合併 main／更新發布索引。
- 0.12.0/code44 首份候選保留；其原生上傳在正式端點遇 403／origin，未標成通過或交付。修正原生契約後以更高版本 0.12.1 重建，沒有覆寫已生成候選。
- 原遊戲精確版本、NativeBridge、SO、程序身分及 UNKNOWN 守門未放寬。新 APK 的 native build 配對由原工具重建與核對，未宣稱新遊戲載入資格。
