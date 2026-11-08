# 竹北接手提示詞（含 Android 12）

依序使用；第一段是完整任務，後三段可作為分階段續做提示。不要同時啟動多個會操作同一裝置的任務。

## 1．完整接手

```text
請在竹北電腦接手「皮克敏花苗透視鏡」。私人 repo：https://github.com/odyliao-lab/pikmin-seed-lens；請取得 codex/seed-lens-031-zhubei-handoff 分支及遞迴 submodules，不要只取 main。先核對 remote、private、branch、HEAD、status，保留既有修改，再建立自己的功能分支。

完整閱讀 AGENTS.md、README.md、docs/ARCHITECTURE.md、docs/VALIDATION.md、docs/GAME_PROFILES.md、docs/ARM64_LOADER_SAFETY.md、docs/COMPATIBILITY_RESEARCH.md，以及 docs/handoffs/2026-11-09-zhubei-takeover.md。以日期與證據層級判斷歷史內容，不照抄舊 PID、端點或部署腳本。

目標是單一 APK＋既有 Root，不需要控制中心／LSPosed／額外 Magisk 顯示模組，兼容多 Android 與遊戲版本。新增必要範圍是 Android 12／API31，不是 Android12L／API32。現在 0.3.1 僅有 ARM64/API34 和 x86_64/API35/libnb.so＋遊戲156.1 的真實花苗頁及未領回探險頁 ON/OFF、縮字與遊戲重開後再啟用證據；156.0 只有離線 profile，API31 尚未驗證。

先做乾淨 candidate 建置，再盤點竹北可用 API31 測試環境。原電腦效能不足，不要在台中建立 MuMu，也不要操作台中 Libra/Virgo。需要建立隔離 instance 時先提出具體 ABI/API/root/來源/端點，未取得明確範圍確認就不要覆寫或操作既有環境；Google／遊戲登入交由使用者。不要複製正式 scanner、token 或登入 session，不建立第二套正式 worker。

Android12 的 ARM64 與 x64/NativeBridge 分開開發和驗收：先在自有 canary 驗證 kernel、pidfd、CPU context、JNI/ART、ClassLoader、root/SELinux 及錯誤收容，再整合正常 APK UI。x64 現在只允許 API35，不得直接放寬版本 gate、猜 namespace pointer 或關閉 SELinux。保留精確遊戲 code/ELF hash/prologue/metadata、同 PID/start consent、journal/ACK 與 UNKNOWN，不偽成功、不重送不明載入。功能只改顯示，不操作物品或發遊戲 RPC。

完成至少兩 Android、兩遊戲版本的真實效果矩陣，且必須包含實際 API31；每個組合記錄兩頁名稱、縮字、捲動/重用、OFF 還原、遊戲重啟、OS 重啟及長時間測試的通過或未驗範圍，不把不同組合交叉推論。156.0 若不能正常登入，不繞過強制更新，改研究合法可運行的第二版本並新增精確 profile。

簽章從使用者認可的私人受限來源取得，不輸出密碼、不放 Git 或命令列；不同交付 bytes 用新版本/code，保留原簽章與回退。缺 key 可先做 unsigned candidate，不能安裝它。分階段測試、審查、commit/push 私人功能分支並更新交接與驗收；不 force、不自動 merge main 或公開發布。先回報現況與第一步，再持續完成能安全處理的開發。
```

## 2．Android 12 專項

```text
接續 Seed Lens 交接，優先完成 Android12/API31 相容。請先讀 docs/handoffs/2026-11-09-zhubei-takeover.md，核對自己的分支與實際隔離裝置。不要用 API32/API35 或 minSdk 宣告代替 API31。
分別檢查 ARM64 的 pidfd/ptrace/regset/CPU context/root/PM/SELinux，以及 x64 的實際 NativeBridge/ART/ClassLoader namespace。先以自有 canary 跑正常、錯誤、逾時、程序退出及重啟測試，缺能力時實作有等價安全性與測試的方案，不移除既有 guard、不關 SELinux。研究工具不是使用者額外依賴。
通過後用正常 APK UI 驗花苗頁和未領回探險頁名稱、長字縮小、重用、OFF 原狀及新遊戲程序重新同意。保留 UNKNOWN 與原始私有證據；只有載入不算成功。不同交付 bytes 升版本/code、核原簽章。分開提交實作與驗收文件，push 私人功能分支，不自動合併或發布。
```

## 3．第二遊戲版本

```text
接續 Seed Lens 多版本驗證。先讀 GAME_PROFILES 和最新驗收矩陣，以已授權隔離環境驗第二遊戲版本，禁止降版正式掃描裝置。156.0/code1792142832 現在只有離線證據；安裝前核對原簽章、完整 splits、ELF/metadata 與精確 profile。使用者完成登入後才驗正常 UI。
若伺服器強制更新，不繞過驗證；記錄原因並選另一個合法可運行的遊戲版本，新增精確 profile、負面測試及真實兩頁驗收。Android12/API31 仍是必要目標，不能把 API34/35 的第二版本結果宣稱為 API31 也通過。
每個組合保存私人 ON/OFF 截圖、長名稱/縮字/捲動重用、遊戲重開再啟用與回復原狀證據。不要為了測試領回/拔苗/買物品或發 RPC；沒有候選頁面就標待觀察。去敏結果進 Git、原始遊戲資料與帳號不進 Git。完成一階段就測試、commit/push 私人功能分支。
```

## 4．驗收與安全交還

```text
整理 Seed Lens Android12 與多遊戲版本開發驗收。逐組合列 API/ABI/root/SELinux/game versionCode/APK hash，區分建置、canary、安裝、真實兩頁 ON/OFF、遊戲重啟、OS 重啟與長時間運作，不把未測寫成通過。
若另外獲准借用掃描裝置，只解除本輪自己建立的 pause，保留 pending/UNKNOWN/owner，確認原 scanner 新 object、成功上傳與站台接受並回原 job；不以心跳或舊結果代替。未借用正式掃描器就不要操作它。
更新 README 現況、VALIDATION、GAME_PROFILES 與日期交接；僅提交相關來源/測試/去敏文件，不上傳 key、token、原始 log/截圖、APK 或遊戲二進位。審查差異並分開 commit/push 私人功能分支，核對遠端 SHA，不 force、不自行 merge main/公開發布。回報已完成、未完成、具體阻礙及下一步，不把交接或建置通過當全目標完成。
```
