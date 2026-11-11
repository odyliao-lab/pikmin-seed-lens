# 0.13.4／code50

依使用者更正，先依序合併 #5～#10 到 main，再製作此版本。0.13.3 發布庫啟用 immutable，無法轉回草稿；保留原 tag／APK，不刪除或覆寫。0.13.4 只增加版本及對應 build marker，GPS／診斷等功能邏輯不變。

- 原簽章保留，既有設定沿用。
- APK 與原生 loader／renderer 重新建置；既有自包含與精確遊戲／程序守門不變。
- GPS 完整分層紀錄以 [0.13.3 實測](GPS_COPY.md) 為準，不因升版擴大平台或遊戲資格。
- 本版在同一隔離 MuMu API35／x86_64＋NativeBridge／Game157.0 code1793360009，從0.13.3原簽章升級保留四組 OFF，GPS開啟後 App 自行載入；水果詳情複製1次、失敗0次，關閉全部後 ack0、次數維持1。載入時 adbd UID2000，事後才 root 讀取回報；無額外模組或其他研究／控制中心 APK。
- 0.13.4 沒有重跑 API32、左右換卡、遊戲重啟或全部四組內容；它們仍依各原版紀錄，不冒稱本版重新實測。
- 原簽章 SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。
- APK：26,642,039 bytes；SHA256：`68a545ac2818f746ea112a8b63d9baefa67bbb62a85179b2dca44acc6c83e1ab`。
- 建置執行44項 controller、10項GPS、34項更新 policy 及既有JVM／APK封裝檢查；原生策略／設定交易由 CI 另執行。
- 左右換卡與花苗仍缺樣本，手機、其他遊戲版本、跨帳號及耐久仍待驗。

最終發布前，先合併本版 PR、確認 main CI；再公開 [0.13.4 預覽版](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.13.4)。README 與更新索引同版本，不變更既有發版檔案。
