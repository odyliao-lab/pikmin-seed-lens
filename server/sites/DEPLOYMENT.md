# Sites 部署紀錄

2026-11-13 升級後追加：API32 隔離機保留設定覆蓋升級至公開透視鏡 **0.15.3／code57**，新版 APK 直接上傳的同編號正式報告已收件，明細與下載內容核對通過；站台仍為既有第 3 版，沒有重新部署。[本輪流程與界線](../../docs/SITES_PRODUCTION_20261112.md)。

2026-11-13 裝置追加驗收：既有正式第 3 版接收 MuMu API32／安裝版透視鏡 **0.15.1／code55** 的實際 App 診斷，後台明細與下載同編號核對通過。本次未重新部署站台；與 API35／0.15.3 的結果分列於[驗收矩陣](../../docs/SITES_PRODUCTION_20261112.md)。

## 2026-11-12：第 3 版正式更新

專用來源 `958210652a78d131085a3ef9d64f787a503d72b0` 同步透視鏡 main `d1b422c1367e86c9f03133d36f26ba7a35e32b8c` 的配套升級與 HTTP 測試，正式部署 `appgdep_6af5c332965c819192b1c3aefd467562` 回報 succeeded。公開入口、站長管理與 D1／runtime secrets 保留，無資料遷移。

正式網頁、產品 Java 傳送、管理查看／狀態／下載與拒絕路徑通過；另有 MuMu API35 安裝版 0.15.3／57 的 App 直接上傳及後台精確編號核對。詳見[本輪驗收矩陣與限制](../../docs/SITES_PRODUCTION_20261112.md)。原始報告與截圖不進 Git；下方為歷史紀錄，較早的「未設定 APK URL」不代表目前狀態。

## 2026-11-11 Sites 私人預覽歷史

## App 原生上傳契約

Sites 來源 `c5bb667e9d47e5d1b9c4ff0ac47beadfb14b63dd`、version 2；部署 `appgdep_6af468d073a8819196f050e6638f02f6` succeeded，保留 public 入口與原管理權限。

`/api/submit` 增加原生 JSON 契約：無 Origin 時必須有 `X-Seed-Lens-Client: android-v1`；同意 header、大小、schema、限流、去重仍必須通過。此 header 不是身分憑證，沒有管理權限。若有 Origin，必須精確同源，不能藉此 header 放行跨站或管理 API；不開放 CORS。

修正前正式 JVM 上傳收到 403／origin（JDK 會省略受限制的 Origin header），不是報告已成功。修正後，實際產品 DiagnosticUploader 以正常 JVM、無特殊參數／cookie／token，完成 Email 空白與合成地址兩份收據核對；合成編號末碼 12／13，App 版本標示 0.0.0。另有 MuMu App 0.12.1 的實際診斷提報，專項見 APP_DIRECT_UPLOAD.md；測試地址皆為 example.invalid，不寄信。

## 後續更正：免登入上傳已開放

使用者撤回「上傳前也必須登入」並更正為不需要登入。2026-11-11 將 access mode 改為 public，重新部署既有相同版本，部署 `appgdep_6af45e89e1488191bd76f8d472b78ff2` succeeded。沒有修改讀報告的管理者授權，沒有把管理資料設成公開。

正式無 cookie HTTPS：上傳頁 200、後台清單／session 401、偽造身分 401、缺少同意 400、非法 schema 400、跨站上傳 403。公開權限更新初期曾短暫回 sign-in-required，等待生效後以上檢查全數通過。

一次合成匿名提報取得 201 accepted：編號 `00000000-0000-4000-8000-000000000011`，App 版本 `0.0.0`、問題 other、其他資料 unknown、Email 空白。此為後台的一筆驗收資料，不是網友故障紀錄；沒有上傳實際裝置或聯絡資料。正式登入的非站長帳號、實際網友提報與寄信仍未驗證。

以下保留首次私人預覽的歷史，不代表目前開放範圍。

- 站台：https://seed-lens-reports.ody-liao.chatgpt.site
- 管理入口：https://seed-lens-reports.ody-liao.chatgpt.site/admin
- Sites source commit：`3b5fd0a624a2ee0f0c106791116e10a49086684f`
- 部署：`appgdep_6af44f9b73508191bcf927ead098ac4b`，工具回報 succeeded、runtime environment revision 1。
- 權限維持 owner-private；尚未取得公開上傳頁選項的答覆，未變更存取範圍。
- D1 空白起站；本機合成報告／聯絡地址／截圖與 dev vars 未隨部署上傳。
- Sites runtime secrets 已設定 ADMIN_EMAIL／RATE_KEY，值不存 repo。沒有租 VPS、買網域或引入寄信服務。

本機政策 3 組、真實 Miniflare／D1 HTTP 4 組、TypeScript 與 Worker build 通過。HTTP 身分驗證使用 starter 的本機模擬登入 cookie；直接偽造身分 header 仍回 401。瀏覽器已看到有 Email 的報告收據與私人管理明細；Email 留空與下載不帶 Email 由 HTTP 測試確認。

早期 HTTP 測試錯把偽造 header 當成本機管理登入，回 401 而失敗；改以 starter 模擬登入後通過，沒有放寬身分守門。正式 Sites 登入／非站長帳號／匿名上傳仍待分項驗收。

29 天後讀取排除，API 請求觸發實體清除；無請求時沒有獨立清除排程。本輪沒有宣稱固定日期背景刪除或已能寄送通知。Email 為使用者未驗證填寫值，僅供修正通知，不用於行銷。

後續：核對上傳頁開放對象；驗正式端到端與非管理者拒絕；若要自動通知或固定日期清除，再實作排程／郵件來源並驗收。APK URL 仍空白，原 App 版本／簽章及 Android／遊戲資格完全不變。
