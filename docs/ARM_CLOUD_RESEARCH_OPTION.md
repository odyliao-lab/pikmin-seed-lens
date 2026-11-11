# 原生 ARM 雲端隔離備案：僅查核，尚未核准或建立

2026-11-10 使用者決定「先不使用雲端」。目前不進行帳號、目錄 session、訂閱或裝置
建立／啟動；下列為先前查核的歷史備案，不是當前執行計畫，費率亦非未來報價承諾。

2026-11-10。本機仍繼續已核准的 API34 AOSP／WSL2 研究。TCG 是 x64 主機上的
ARM 指令軟體模擬；CPU fixture 通過不代表 Android 啟動負載、普通 App 或遊戲可用。
下列備案用來解除主機架構／效能限制，不取代 API31、API34 或九組產品驗收。

## 官方來源已核對的能力

- Genymotion SaaS 的 ARM64 虛擬裝置在 ARM servers 上執行；不是本機 x64 NativeBridge。
  [原廠 ARM64 架構公告](https://www.genymotion.com/blog/genymotion-arm64-devices/)。
- 官方目錄有 ARM64 Android14.0 與12.0；Root 文件分別列 API34 與API31，不能用
  12.1/API32 替代。[映像目錄](https://docs.genymotion.com/usage/saas/basic_steps/)、
  [Root 文件](https://docs.genymotion.com/features/root/)。
- 原廠有動態 Root 與普通 App 的 Superuser 授權流程，但這不證明 `su -M`、全域 mount
  view、pidfd、ptrace／CPU regset、SELinux 或產品呼叫時間界限已合格。
- Google 服務未預裝。原廠提供內建 GApps 安裝流程，警告不要自行混用網站下載的舊包。
  如後續採用，須先核對該目標與來源；登入仍由本人完成。
  [原廠 Google 服務說明](https://support.genymotion.com/hc/en-us/articles/4414586104977-I-can-t-find-Google-Play-Store-Where-is-it)。
- 查核時官方隨用隨付牌價為每個運行裝置 US$0.06／分鐘；30分鐘是US$1.80，稅費與
  實際帳戶報價另核。這只是預算換算，沒有訂閱、付款或啟動任何服務。
  [官方費率](https://docs.genymotion.com/usage/saas/pricing/)。

## 歷史提案：目錄核對（目前不執行）

目前沒有已確認可用的 SaaS 帳號／登入 session，也沒有 recipe UUID、可用機房、實際
CPU／RAM／GPU規格或帳戶報價。因此**尚不是可直接啟動的具體操作目標**，不假定有
免費試用或帳戶額度，不建立帳號、不代填付款、不使用任何已登入的其他雲端服務。

若本人提供此服務的登入操作，先只讀查官方 Android14.0／ARM64 的可用 recipe、image
UUID／revision、硬體規格、機房與實際費率，再提出新機名稱
`SeedLens-Cloud-API34-ARM64-Preflight`、唯一 recipe／image ID、最多一台、預檢時限與
停止方法供核對。新雲端目標／付費範圍須另外確認，現有 WSL／MuMu 授權不自動延伸。

初步建議只核准一個最多30分鐘的 API34 平台預檢，先排除硬體／Root介面不合格，
而不是先購買長期方案或承諾整組遊戲成功。尚未取得帳戶目錄前，不猜 CPU 型號、
SVE／SME／MTE／PAC、GPU 加速、SELinux 模式、普通 App Root 或遊戲登入能力。

## 若後續具體目標獲准，仍須逐層驗

1. 正常關閉現有研究 guest，新機唯一ID／boot／實際API34／ARM64-only／NativeBridge、
   kernel／SELinux／CPU feature／pidfd與Root provider 新鮮核對。只用新機，不操作原四台
   或台中，不複製scanner、憑證或任何帳號資料。
2. 自有 CPU／context／故障收容，產品15s／2s不變；未支援feature保持拒絕，不能隱藏
   CPU能力或移除guard。之後才驗普通App Root／mount view／PM loader與真JNI生命週期。
3. 平台合格後另依已核對來源處理Google服務與本人登入，原簽章精確三遊戲各自測原版啟動，
   再正常透視鏡App入口驗兩頁／縮字／重用／OFF／遊戲及OS重啟。伺服器或完整性拒絕保持
   原狀並記失敗，不繞過，不因商用平台宣稱支援ARM就認定Pikmin Bloom可用。
4. API34完成後，API31須新目標與同樣分層；雲端同樣不自動建立x64/NativeBridge資格。
   不將開發用服務、host CLI、研究probe或GApps安裝器變成產品使用者依賴。

目前三交付階段與九組產品狀態均未因此改變。研究仍先API34，再API31，最後森林手帳UI。
