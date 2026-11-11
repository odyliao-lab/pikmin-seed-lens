# 花苗透視鏡回報站（Sites）

目前依使用者最終更正，已開放免登入上傳；只有管理後台需要站長的 ChatGPT 帳號。正式匿名 HTTPS 提報已以合成資料取得 201 收據，未登入／偽造身分的管理請求皆為 401。固定清除排程與寄信尚未實作。

使用 Sites／Vinext Worker＋D1，不需 VPS、R2 或外部郵件服務。來源由獨立透視鏡 repo 的 `server/sites/` 保留鏡像，Sites 專用 source repo 負責部署。不要把本機 `.dev.vars`、報告或截圖提交。

## 使用流程

使用者匯出診斷 JSON，選檔預覽、選填 Email，勾選同意後上傳。Email 欄旁明示「僅用於問題修正後通知；不填也能提報，不會用於行銷」。Email 是未驗證地址；本輪不自動寄信、不做訂閱／行銷，也未建置郵件服務。

管理者在 `/admin` 使用 ChatGPT 登入，伺服器還會比對外部 `ADMIN_EMAIL`。一般登入者不能讀報告；未知或未設定管理者一律拒絕。上傳頁與後台頁殼可以公開，所有讀取、下載與狀態 API 都獨立授權。Sites audience 初始為 owner-private，需使用者指定後才改公開。

`/api/submit` 接受 `{report,email}`；原 App `/v1/reports` 仍接受原封閉 JSON，不能新增 email 或任意欄位。兩者要求同意 header。Email 保存在獨立 contacts 表，只在授權管理明細顯示；診斷下載、列表與收據都不含 Email。重複內容同編號可去重，若改 Email 則拒絕覆寫。

## 限制與保護

- 每份診斷最多 16 KiB，網頁信封最多 18 KiB，最多 1000 份；每來源每分鐘最多 5 次，最多 1024 個活躍來源 bucket。
- 來源 IP 以外部隨機 RATE_KEY 加鹽 SHA256 後短期存於 D1，不保存原始 IP；不在程式印出 payload／Email／識別 header。託管商的基礎設施紀錄仍依 Sites 政策。
- 只接受封閉 schema，重複 JSON key、過深 JSON、Email header injection、額外欄位拒絕；SQL 全部參數化，畫面用 textContent。
- 管理 POST 要求同源 Origin 與 JSON，沒有 CORS；登入由 Sites 維護，不自建密碼庫或繞過 token。
- 29 天後立即排除讀取，處理 API 請求時清除過期報告、聯絡資料與 rate bucket。**無流量時不保證即時實體刪除**，沒有自動排程或郵件通知；正式承諾固定日期刪除前，仍須另完成排程與託管備份保存政策核對。頁面已揭露請求時清除，沒有宣稱永久背景清理。
- 不需要 OpenAI API key。ADMIN_EMAIL／RATE_KEY 透過 Sites runtime secrets 設定，不能放 manifest／前端或 repo。
- 尚未設定 APK 的固定上傳 URL，沒有新 APK／合併 main／公開 APK 發布。

## 建置與測試

依 Sites skill 執行 install-dependencies／build-site／site-workflow，使用 Node 22.13+。`.openai/hosting.json` 宣告 D1=`DB`、R2=null。`npm run db:generate` 產生的 migration／snapshot 不可改寫，後續需新增 migration。

```sh
node --test tests/policy.test.mjs
npx tsc --noEmit
```

本機 Miniflare：依 starter README 將 migration 套至本機 D1，設定 ignored `.dev.vars` 的 `ADMIN_EMAIL=seedy@sites.test` 與合成 RATE_KEY，再 `npm run dev -- --hostname 127.0.0.1 --port 5189`。`python tests/http_test.py` 只測該 loopback，會建立合成資料，不可用於正式服務。測試 4 組：Email 留空／填寫、同意／Origin／偽造身分拒絕、原生協定／限流、狀態／篩選；政策另 3 組通過。瀏覽器已完成 Email 提報、登入、私人明細，確認下載不含 Email。

公開匿名提報已於正式 Sites 取得有效收據；未登入與偽造身分的讀取拒絕亦通過。正式已登入非站長的身分隔離仍待分項驗收，不可把本機模擬登入當成正式登入結果。資料庫空白起站後新增了一筆明列於 DEPLOYMENT.md 的合成驗收資料，本機其他報告未上傳。

託管依 Sites 現行內含額度／價格，不能保證永久免費。維持平台網址、1000 份限制與小型封閉報告，未建立付費 VPS／購買網域。
