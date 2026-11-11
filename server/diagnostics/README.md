# 私人診斷接收端（尚未部署）

Python 3.10+、標準函式庫；只在 `127.0.0.1:8109` 接收 `POST /v1/reports`。由獨立 HTTPS 反向代理對外服務，沒有報告查詢／下載／管理 API。App 的 `DiagnosticEndpoint.URL` 預設空白，尚不會上傳。

## 資料與界線

只接受與 App 對應的封閉 schema：軟體／平台版本、Root／檔案檢查分類、退出分類與最多 16 個列舉事件。拒絕額外欄位、自由文字、路徑、token、帳號、位置、截圖或日誌。報告上限 16 KiB；UUID 是每份報告隨機產生，不是使用者或裝置識別碼。

報告檔案權限 0600、資料目錄 0700。相同 ID／相同內容回傳同一收據；同 ID 不同內容拒絕覆寫。預設每來源每分鐘最多 5 次、最多 1000 份；IP 只以隨機鹽雜湊存於短期記憶體，不寫入報告或 access log。這是匿名回報入口，沒有內嵌 App 密鑰，也不是防垃圾流量的完整解法；正式部署仍須反向代理限制連線與用量。

接收端以 29 天為清除門檻，運行中最慢每小時檢查。營運者須維持清除作業，停止服務或備份也要遵守最長 30 天的保存政策；不要把報告存到 repo、公開 web root 或長期備份。服務未部署前沒有正式收件或遠端上傳驗收。

## 部署前必填

1. 指定由維護者管理的網域與主機，完成正常 CA 的 HTTPS；不接受自簽憑證豁免。
2. 建立專用非 root 帳號與 repo 外資料目錄。程式拒絕以 root 啟動。
3. 以 `python3 receiver.py --data-dir /var/lib/seedlens-reports --trust-loopback-proxy` 執行，對外只能經代理。只有代理能連到 loopback；它必須覆寫 `X-Real-IP`，不可直接信任客戶端 header。
4. 驗證同意 header、封閉 schema、413／429／503、TLS、去重、權限、清除及不可透過 HTTP 讀取報告。關閉代理 access log 中的 IP／請求內容。
5. 將已驗證的 `https://你的網域/v1/reports` 固定於 `DiagnosticEndpoint.URL`，升版並簽署新 APK。不可由 Intent、使用者輸入或伺服器轉址指定任意上傳位置。

反向代理參考（Caddy；請換成實際網域，未在本專案執行安裝／部署）：

```caddy
reports.example.org {
    @reports path /v1/reports
    handle @reports {
        request_body { max_size 16KB }
        reverse_proxy 127.0.0.1:8109 {
            header_up X-Real-IP {remote_host}
        }
    }
    handle { respond 404 }
}
```

成功收據只含 `status=accepted` 與 `report_id`。使用者匯出不等於上傳；逾時或沒有有效收據不能宣稱成功，也不自動重送。人工重試同一份報告可去重。

## 離線／本機測試

`python3 test_receiver.py --fixture <由 DiagnosticPolicyTest 產生的合成 JSON>`

測試會在隨機 loopback 埠實際收送 HTTP 並使用自有暫存目錄；不部署公開端點，不接收網友資料。正式 HTTPS／主機硬化及遠端端到端回報仍待部署後驗收。
