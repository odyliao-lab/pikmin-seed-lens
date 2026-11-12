# Sites 正式部署與 App 診斷上傳驗收

驗收日期：2026-11-12（Asia/Taipei）。本輪更新回報站相依套件，並驗證正式收件；未產生新 APK。

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

## MuMu 安裝版 App → 正式後台

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

## 驗收界線

- 本輪已完成 API35 安裝版 App 的實際診斷上傳與正式後台查看，不代表遊戲顯示、Root 載入或其他 Android／遊戲版本新增資格。
- 手機、API32 上傳 UI、Android 真實離線／TLS 失敗、跨 Activity／程序重建的收據保留未重跑。
- 正式已登入非站長帳號隔離仍待驗；未登入與偽造身分被拒絕不能替代此項。
- Email 未寄送通知；固定清除排程與耐久未驗。原保存政策仍為 29 天後排除讀取，於請求時清除。
