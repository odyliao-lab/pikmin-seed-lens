# 診斷上傳與管理後台（尚未部署）

Python 3.10+，只使用標準函式庫。瀏覽器上傳頁 `/`、維護者後台 `/admin`、App 接收介面 `POST /v1/reports` 共用既有封閉 schema。App 端點仍為空，這次沒有新 APK、網域或正式收件服務。

## 使用者與維護者流程

使用者在 App「診斷與回報」匯出 JSON → 網頁選檔 → 預覽 → 勾選同意 → 上傳 → 保留回報編號。沒有背景上傳或自動重送；逾時為結果未知，人工重試同一份檔案會去重。

維護者以獨立管理密碼登入 `/admin`，查看待處理／處理中／已處理數量，依問題、App／遊戲版本與 Android API 篩選，讀取詳情、下載 JSON、儲存處理狀態。每頁 25 份，依接收時間排序；重新整理才取新資料，目前沒有排程通知、郵件或背景輪詢。

## 私人資料與安全界線

- 僅接受 16 KiB 以內的封閉診斷 JSON；不收帳號、token、位置、物品、自由文字、截圖或原始 log。伺服器重新驗證，瀏覽器預覽不是驗證依據。
- 預設最多 1000 份、每來源每分鐘 5 次。重複 ID／相同內容取得原收據；不同內容拒絕覆寫。IP 僅加鹽雜湊保留短期記憶體，不寫入報告或 access log。來源頻率與登入頻率在重啟後會重設，代理仍須限制流量。
- 報告權限 0600，目錄 0700。SQLite 只存編號與處理狀態，不改原報告。29 天到期，運行中每次請求後或最慢每小時清除報告與關聯狀態；服務停止時不會執行清除。不可將資料放 repo、公開 web root 或長期備份；磁碟快照與匯出也須遵守最長 30 天政策。
- 無公開讀取報告介面。管理 API 必須登入；8 小時固定 session 到期，重新啟動或更換密碼雜湊後重啟會使 session 失效。session 只存記憶體，cookie 使用 HttpOnly／SameSite=Strict，正式環境另有 Secure／__Host- 前綴。
- 管理寫入要求精確 Origin＋session CSRF token；登入每來源 15 分鐘最多 5 次，沒有預設帳密。密碼以 scrypt 儲存雜湊，不放 APK 或 repo。瀏覽器跨站上傳拒絕；原生 App 仍可使用既有無 Origin 協定，同意 header 並非驗證身分或防濫用密鑰。
- 不使用外部 CDN、分析追蹤或第三方 JavaScript；CSP 禁止 inline script／iframe。這是單維護者、小流量版本，未完成獨立滲透測試、MFA 或多角色權限；可再用私人 VPN／身分代理限制 `/admin`（不要限制使用者上傳路徑）。

## 啟動設定

正式部署建議 Linux 專用非 root 帳號，資料位於 repo 外的私人持久磁碟；Windows 僅完成本機測試，NTFS ACL 尚未完成正式驗收。

1. 維護者在安全終端執行 `python3 server/diagnostics/web.py`，互動輸入至少 16 字密碼，取得 scrypt 雜湊；勿在命令列貼密碼。
2. 把雜湊存為主機外部環境變數 `SEEDLENS_ADMIN_HASH`。systemd 可用權限 0600 的外部 EnvironmentFile，內容為 `SEEDLENS_ADMIN_HASH=scrypt$...`；不要放 Git 或公開部署 log。
3. 以服務帳號執行：

```sh
python3 server/diagnostics/receiver.py \
  --data-dir /var/lib/seedlens-reports \
  --public-origin https://reports.example.org \
  --trust-loopback-proxy
```

僅綁 `127.0.0.1:8109`，HTTPS 反向代理為唯一入口；必須覆寫 `X-Real-IP`，不能信任客戶端自行提供的值。HTTPServer 是單程序、低流量後端，不直接暴露網際網路；代理須限制併發、請求逾時、用量並停用包含 IP 的 access log。只允許受信任管理員在同主機執行程式，loopback 不是對不受信任本機帳號的安全隔離。

Caddy 參考（替換網域；設定尚未在正式主機驗收）：

```caddy
reports.example.org {
    request_body {
        max_size 16KB
    }
    reverse_proxy 127.0.0.1:8109 {
        header_up X-Real-IP {remote_host}
    }
}
```

Caddy 沒有加入 `log` 指令；仍須核對主機商／上游代理自己的存取紀錄與保存期限。將服務交由 systemd 管理與自動重啟，設定每日安全更新、磁碟／用量告警；若做短期加密備份，應連同快照清除排程一起驗收，不套用超過 30 天的主機預設備份。

本機預覽可使用 `--public-origin http://127.0.0.1:8109 --local-http`；HTTP 模式只允許 loopback 且 cookie 不加 Secure，絕不可對外使用。未設定 `--public-origin` 時保持原本 API-only 行為。

## 空間建議

建議先用 **1 vCPU／1 GiB RAM 的 Linux VPS**，主機磁碟 20–25 GiB、報告目錄先預留 1 GiB，保留 1000 份上限。1000 × 16 KiB = 15.625 MiB 的原始輸入上限，另加檔案配置、SQLite 與系統開銷；1 GiB 是預留，不是現有寫入配額。無需先另買物件儲存或大型資料庫。

2026-11-11 查詢：DigitalOcean Basic 1 GiB／1 vCPU／25 GiB SSD 為 **US$6／月**，網域、稅與備份另計：[官方價格](https://www.digitalocean.com/pricing/droplets)。優先選你已有的可信任 VPS；新主機需確認實際可選區域、付款與管理方式後才建立。

免費服務僅適合預覽。例如 [Render Free](https://render.com/docs/free) 15 分鐘無流量會休眠，檔案在休眠／重新部署時可能遺失，無法作為這個本機磁碟版的正式報告儲存。若用 PaaS，必須選付費服務＋持久磁碟並另外適配綁定與代理；本次未實作該部署路徑。

## 驗收與待辦

```sh
python3 server/diagnostics/test_receiver.py
python3 server/diagnostics/test_web.py
```

2026-11-11 本機：原接收端 7 組＋新增網站 5 組測試通過（真實 loopback HTTP、合成報告），涵蓋隱私／格式、同意、去重、容量／頻率、未登入禁止讀取、登入限制、Origin／CSRF、session 到期／登出、篩選／分頁、處理狀態持久化、下載與到期清除。CI 同時執行。

瀏覽器實際完成選檔／預覽／同意／收據、登入／查看明細／儲存狀態／登出，並檢查手機寬度與桌面斷點；截圖與合成報告保留 ignored `.tools/`，不提交。正式 Linux 權限由 Linux CI 覆蓋，實際主機、TLS、代理限流、備份清除與網友端到端流程仍待部署後驗收。

**上線前還要完成：** 選定主機與網域 → 部署 HTTPS／設定管理密碼 → 驗收安全與清除 → 公布使用者上傳頁。若要直接從 App 上傳，再將已驗證固定 HTTPS URL 寫入 `DiagnosticEndpoint.URL`，升版並沿用原簽章發出新 APK；本輪保持空白。
