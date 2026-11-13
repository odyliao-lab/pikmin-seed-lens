# Sites 正式部署與 App 診斷上傳驗收

驗收日期：2026-11-12，2026-11-13 追加 API32 的 0.15.1 與升級至 0.15.3 驗收（Asia/Taipei）。本輪更新回報站相依套件，並驗證正式收件；未產生新 APK。

## 部署身分

| 項目 | 本輪版本 |
|---|---|
| 透視鏡來源 | `d1b422c1367e86c9f03133d36f26ba7a35e32b8c`（#22、#11、#23 已合併） |
| Sites 專用來源 | `958210652a78d131085a3ef9d64f787a503d72b0` |
| Sites 版本 | 3 |
| 正式部署 | `appgdep_6af5c332965c819192b1c3aefd467562`，工具回報 `succeeded` |
| 正式入口 | https://seed-lens-reports.ody-liao.chatgpt.site |
| 管理入口 | https://seed-lens-reports.ody-liao.chatgpt.site/admin |

透視鏡 repo 保留網站鏡像，實際部署使用 Sites 專用來源。同步 `package.json`、`package-lock.json` 與 `tests/http_test.py`，保留專用來源 README；React／React DOM／RSC 為 19.3.1、RSC plugin 0.5.37、vinext 1.4.5、satori 0.33.5、fast-uri 3.1.9。

保留公開免登入回報、站長登入管理、D1 綁定與 runtime environment revision 1；未更改 secrets、執行資料遷移或清空資料。網站版本與 APK 版本獨立。

## 建置與服務驗證

| 分層 | 實際結果 |
|---|---|
| main CI | Node 22 鎖定依賴安裝、TypeScript、Worker／前端建置、4 組政策測試、5 組本機 D1 HTTP 測試通過；Offline policy 亦通過 |
| Sites 專用來源 | 重新安裝依賴、4 組政策測試、TypeScript、正式 Worker／前端建置與封裝通過，再推送來源、保存第 3 版並正式部署 |
| 產品 Java 傳送類別 | JDK17 執行實際 `DiagnosticUploader`／`DiagnosticSubmission`／`DiagnosticReport`／`DiagnosticEndpoint`，無 cookie、瀏覽器 Origin 或特殊 JVM 豁免，合成報告取得精確收據；此項獨立於下方 Android 實測 |
| 正式網頁 | 選取合成 JSON、預覽、填寫 example.invalid 測試 Email、勾選同意後上傳，取得相符收據 |
| 正式管理後台 | 既有站長登入狀態下，核對兩份合成報告編號、明細、Email 空白／填寫與篩選；儲存已處理狀態，重新查詢確認保存 |
| 正式下載 | 下載 JSON 的編號及封閉 schema 正確，不含選填 Email |
| 正式拒絕路徑 | 無登入管理 session／清單、偽造身分均為 401；錯誤 Origin 為 403；缺少同意為 400 |

正式 HTTP 拒絕檢查使用產品 User-Agent。初次使用 Python urllib 預設 User-Agent 得到 403，未用它證明 App 授權路徑；改用產品 User-Agent 後取得上述應用程式分類。未放寬授權或加入重送豁免。

main 驗證：[網站 CI](https://github.com/odyliao-lab/pikmin-seed-lens/actions/runs/43227983828)、[Offline policy](https://github.com/odyliao-lab/pikmin-seed-lens/actions/runs/43227983277)。本機 HTTP 使用 loopback、合成資料與模擬站長登入，不等同正式非站長帳號隔離驗收。

驗收文件 PR #24 的 CI 通過後，合併後網站 CI 首輪在限流測試遇到整分鐘切換：第六次請求於新分鐘得到 200，而非前一分鐘額度用盡的 429。保留該失敗紀錄；後續測試在分鐘末先等待新窗口，並核對整個請求批次未跨分鐘，仍要求第六次回 429，未放寬正式限流或加入失敗重試。[首輪紀錄](https://github.com/odyliao-lab/pikmin-seed-lens/actions/runs/43235456950)。

## MuMu API35 安裝版 App → 正式後台（2026-11-12）

| 項目 | 觀察結果 |
|---|---|
| 隔離目標 | 既有 `SeedLens-Isolated-A15`／index5；啟動前重新核對名稱、狀態，啟動後確認專屬 ADB 端點與實際 API |
| 安裝版 | 透視鏡 0.15.3／code57，Android 15／API35、x86_64；不是主機 JVM 或合成 App 報告替代 |
| 操作 | 診斷 → 預覽問題回報 → 其他 → 預覽 → Email 留空 → 勾選同意 → 同意並上傳 |
| App 收據 | 安裝版 APK 顯示「已收到報告」及本次回報編號，沒有透過電腦代送 |
| 正式收件 | 2026-11-12 22:08:47（Asia/Taipei），站長後台找到完全相同編號並查看實際欄位／事件 |
| 內容核對 | 下載 JSON 與 App 編號相同；App 0.15.3／57、API35／Android15、x86_64、遊戲157.0／code1793360009、原生檔案 `match`，符合隔離機觀察；僅含既有封閉欄位 |
| 未知保留 | Root=`not_checked`，NativeBridge／SELinux=`unknown`，不補成已授權或平台成功 |
| 測試結束 | 功能開關維持 OFF、遊戲未開，未檢查 Root 或載入遊戲；隔離機正常關閉，原四台營運 MuMu 程序身分不變 |

兩份合成網站驗收報告已標為已處理；MuMu 真實診斷保留原收件紀錄。沒有修改其他使用者的報告。回報編號、原始診斷、下載檔、畫面與封裝檔留在 ignored 私人取證目錄，不提交 Git；本輪私人索引為 `.tools/sites-deploy-smoke/RESULTS.md`。

## MuMu API32 安裝版 App → 正式後台（2026-11-13）

| 項目 | 觀察結果 |
|---|---|
| 隔離目標 | 既有 `SeedLens-Isolated-A12`／index4；啟動前核對名稱及關閉狀態，啟動後核對專屬 ADB 端點與實際 API32／x86_64。MuMu 標示內建 12.0，實際為 Android 12L／API32 |
| 安裝版 | **透視鏡 0.15.1／code55**，沿用已安裝版本，未升級或重設設定；不是 0.15.3／57 的 API32 上傳驗收 |
| 操作 | 診斷 → 預覽問題回報 → 其他 → 預覽 → Email 留空 → 勾選同意 → 同意並上傳；安裝版 APK 自行連線，電腦未代送 |
| App 與正式收件 | App 顯示「已收到報告」；2026-11-13 10:49:03（Asia/Taipei）正式站長後台找到完全相同編號，能查看實際欄位與事件 |
| 下載核對 | JSON 編號與 App 相同，App 0.15.1／55、API32、Android 版本欄位 `12`、x86_64、遊戲157.0／code1793360009、原生檔案 `match` 均符合觀察；封閉 schema 正確，沒有 Email、帳號或座標欄位 |
| 未知保留 | Root=`not_checked`，NativeBridge／SELinux=`unknown`，不推定已授權、無轉譯或載入成功 |
| 測試結束 | 功能開關維持 OFF，保留原有歷史狀態；未開遊戲、檢查 Root 或注入。index4 正常關閉；原四台營運 MuMu 程序身分不變，API35 隔離機維持關閉 |

正式報告保留原收件狀態，未修改其他報告。原始診斷、完整編號、下載與截圖僅留在上述 ignored 私人取證目錄；未提交 Git。此階段只驗 **API32＋0.15.1／55 的診斷上傳／後台查看**，其後的 0.15.3 升級與重驗另列如下。

## API32 保留設定升級至 0.15.3／57 後重驗（2026-11-13）

| 項目 | 觀察結果 |
|---|---|
| 目標與原版 | 同一既有 index4 隔離機，重新核對 API32／x86_64 及已安裝 0.15.1／code55 |
| 升級原件 | 使用已發布 0.15.3／code57 APK，SHA256 `69464a224086c10cbb5ffa5e455bf91801449bce8e504d9e7ad026edb0bb3129`；簽章 SHA256 `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。未重建或覆寫發布產物 |
| 安裝與保存 | 驗證工具以 `adb install -r` 覆蓋安裝，沒有卸載／清資料。App UID 不變，首次啟動前的三份偏好設定檔與升級前逐位元組一致；啟動後五個開關仍 OFF、每日自動檢查仍 ON |
| 安裝後核對 | 實裝 0.15.3／57，Package Manager 的 base APK SHA256 與公開原件完全一致 |
| App 直傳 | 新版診斷 → 預覽問題回報 → 其他 → 預覽 → Email 留空 → 勾選同意 → 同意並上傳，App 顯示相符成功收據；未由電腦代送 |
| 正式後台 | 2026-11-13 11:50:40（Asia/Taipei）收到同一編號，可查看實際明細與事件，Email 空白 |
| 下載／歷史 | JSON 編號及封閉 schema 正確，App 0.15.3／57、API32、Android版本欄位 `12`、x86_64、遊戲157.0／code1793360009、原生檔案 `match` 均吻合；事件陣列與前次 0.15.1 報告完全一致 |
| 未知／隔離 | Root=`not_checked`、NativeBridge／SELinux=`unknown` 保留。未開遊戲或啟用顯示；原四台營運程序身分不變，API35 隔離機維持關閉，本輪 index4 正常關閉 |

此項通過 **API32＋0.15.3／57 的覆蓋升級保存設定、診斷直傳與後台收件查看**。覆蓋安裝不等於 App 內更新下載／系統確認流程驗收，也不表示遊戲載入已驗；工具頁的未核對狀態未作為上傳成功或遊戲資格證據。私人偏好設定備份、雜湊比對、診斷與畫面保留於 ignored `.tools/api32-0153-upload/`，不提交 Git；正式報告維持原收件狀態。

## 驗收界線

- 已完成 API35＋0.15.3／57、API32＋0.15.1／55 及升級後 API32＋0.15.3／57 安裝版 App 的實際診斷上傳與正式後台查看，不代表遊戲顯示、Root 載入或其他 Android／遊戲版本新增資格。
- 手機、Android 真實離線／TLS 失敗、跨 Activity／程序重建的收據保留仍未驗；本輪也未重跑 App 內更新流程。
- 正式已登入非站長帳號隔離仍待驗；未登入與偽造身分被拒絕不能替代此項。
- Email 未寄送通知；固定清除排程與耐久未驗。原保存政策仍為 29 天後排除讀取，於請求時清除。
