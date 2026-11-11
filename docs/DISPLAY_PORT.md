# 四組唯讀顯示移植

## 接手與範圍

2026-11-11，目標 remote `odyliao-lab/pikmin-seed-lens`，乾淨 `main@0773bca` 起始；工作分支 `codex/standalone-display-groups`。使用者後續縮減本輪開發／裝置驗收為 MuMu，沒有可用 ARM64 手機；不得宣稱手機通過。不得合併 main 或發布。

來源 remote `odyliao-lab/pikmin-control-center`。實際來源工作副本已在其他研究分支，故全程以 `git show a02b58f35295948cbd29e467ce5ccd7ffe33161a:<path>` 讀取固定版本，不切換或修改來源。選擇性移植，不 cherry-pick monorepo commits。

## 既有功能與必要缺口

| 項目 | 接手時狀態／本輪界線 |
|---|---|
| Root＋單 APK | 0.7.3 已封裝 PM loader、x64 host 與 ARM renderer；沿用，不建立第二載入器 |
| 平台／遊戲守門 | 既有 API32／35 x64-libnb、精確遊戲 version/code/ELF SHA/prologue/metadata、PID/start/boot 與持久 UNKNOWN 保留 |
| 既有顯示 | 花苗名稱、縮字、OFF 原文字／字型還原；來源八項擴充尚未接入 |
| 檔案隔離 | 舊控制／回報與來源同名，必須改成獨立 namespace；不刪除來源或舊 journal |
| App 先開／PID 重綁 | 既有 UI 要求當次遊戲主畫面確認，尚未滿足本輪先啟用、後開遊戲的流程 |
| confirmed context | 來源透過核心的登入／登出 epoch 與 InventoryManager 提供；獨立版尚無，禁止用假資料或外部 TSV 補足 |
| 四組設定 | 尚需獨立擁有的 3／2／2／1 契約、意願先保存、合法舊值 OR 遷移與中斷回復 |

## 本輪自包含基線

新鮮核對既有新建隔離 index4／SeedLens-Isolated-A12、實際 API32／x86_64／libnb.so。0.7.3/code29、Game156.1/code1793076318；普通 shell UID2000、內建 su。沒有控制中心／三個研究 APK，`/data/adb/modules` 不存在；maps 只有獨立 APK 提供的 renderer／host，沒有控制中心核心。

遊戲既有程序上的完整裝飾名稱可見，透過普通產品 UI 關閉後實際還原原名與較大字型；新鮮同 PID/start 回報 ready/control0。這是舊版單 APK 既有功能的有界基線，**不是新獨立 namespace、新載入、新功能、全新安裝或 App 先開驗收**。原始身分、maps、journal SHA 與 ON/OFF 圖片只存 ignored `.tools/display-port-baseline/`。

## 逐項目標（未驗收不得標完成）

| 組別 | 需實接內容 | 初始狀態 |
|---|---|---|
| 花苗資訊 | 顏色／種類、D01 已收／持育待／未知原因、育苗詳細頁步數列 | 待移植 |
| 皮克敏資訊 | D03 目前花態／可收瓣／任務、D04 已取得裝飾、主清單合併短副標 | 待移植 |
| 探險資訊 | D05 普通白盒對象／一般裝飾、明確狀態及遊戲時間、尊重原可見性 | 待移植 |
| 精華／花瓣對照 | D02 保留精華數，補 confirmed 花瓣／容量／空間；契約為 nectar-stock-preview | 待移植 |

來源 Game157 資格不轉移至其他遊戲。MuMu 普通白盒、部分探險狀態、跨帳號與耐久樣本不足亦不繼承為通過。先建立獨立載入與隔離檔案的實測，再擴大 renderer。
