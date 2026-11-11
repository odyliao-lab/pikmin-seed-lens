# 0.7.3：圖示與強制直式（2026-11-11）

本次使用者明確要求製作App icon、強制直式，並將APK上傳控制中心同一發布頁。
發布授權僅限本APK及必要版本／SHA文件，不包含私人來源公開、main合併或控制中心索引修改。

## 實作

- 原創森林綠底、金色放大鏡及陶盆花苗，以Android vector／adaptive icon製作。
  圖像工具角色圖生成失敗後改用repo原生vector，未使用失敗圖像；App內既有插畫保持。
- application icon、roundIcon使用同一adaptive資源，適應桌面遮罩。
- MainActivity的screenOrientation=portrait，不改系統或遊戲旋轉設定。
- aapt2 compile／link新資源。初次誤用overlay參數的link失敗原件保留本機，修正後才建出APK。
- 版本0.7.3/code29，原簽章；0.7.2與未交付失敗中間檔均保留。

APK SHA256：`c7c72e89515e0bfb69c9cc720a7316ecb20107183e0ff816333dd29ea0a87721`。
憑證SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。

## 必要驗證

完整建置Compatibility57、InstalledNative29、RootCommand57、GameSession27、LensStatus102、
LensContext34、APK layout10及paired digest4通過；native執行檔只編譯，不冒稱實跑。
新APK內八項native／digest entries與已驗0.7.2逐位元一致，Root／loader／renderer未改。

新鮮核對已授權index4名稱／created／16512／原boot、實際API32/x86_64/libnb、adbd2000；
同一157.0/code1793360009程序PID14218/start280512/UID10041。
原簽章-r升級保留資料。MuMu橫向桌面實際顯示新icon，點icon後App1080×1920，
ActivityManager為SCREEN_ORIENTATION_PORTRAIT；原始Manifest亦portrait=1。
App新程序初始「尚未核對」保持真實狀態，按只讀核對後顯示已啟用、Switch true。
回遊戲後完整花苗名稱仍在、357庫存不變；沒有新native載入、也沒有重送UNKNOWN。
這是同程序顯示延續，未轉記為0.7.3新程序載入／完整ON-OFF驗收。

唯一產品journal仍`b44d3c2560c7b0c4988569d51a8f280c20294085f646b065dc9cf62a8f0d31c6`，
六筆研究SHA不變；最後188個thread TracerPid0。三個研究APK仍不存在，沒有額外套件依賴。
診斷曾遇thread正常退出的讀取競態、查不存在package的exit1；只修正唯讀檢查，沒有重載。
原始證據在ignored `.tools/lab/product073-ca73c7a97eaf4a3f9df75d4bdb5a69ad`及前版證據目錄，
不公開原始截圖／診斷。探險、其他平台／版本及重啟本次未新增驗收。

## 發布

控制中心實際發布庫為公開`odyliao-lab/pikmin-control-center-releases`。
使用獨立tag `seed-lens-v0.7.3`、預覽版、非latest，不覆寫既有Release、不改控制中心更新索引。
只附原簽章APK、SHA256清單及去敏建置來源版本資訊；不附私人來源、研究APK或憑證。
發布後另核遠端metadata與匿名下載SHA。

實際已於2026-11-11T02:28:26Z發布，Release id441167378；draft=false、prerelease=true、
immutable=true。APK 21,857,845 bytes，GitHub資產digest與上述SHA一致。
匿名下載因速度較慢採分段回收，完整重組21,857,845 bytes後SHA與原APK完全一致；
未使用GitHub登入token下載，確認公開下載可用。
實作來源commit `afbb83a` 已推送私人功能分支；控制中心latest及發布庫main保持原值。

[下載頁](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.7.3)
