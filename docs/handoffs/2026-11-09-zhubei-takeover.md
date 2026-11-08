# 竹北接手：皮克敏花苗透視鏡／Android 12 與多遊戲版本

## 任務與交付界線

2026-11-09：使用者要求將進度提交私人 repo，由效能較強的竹北電腦接手；本機不再建立隔離 MuMu。新增必要目標為 **Android 12／API 31**，不是 Android 12L／API 32。既有「多 Android、多遊戲版本」目標仍保留。此交接不是 APK 發布或已完成相容性宣告。

- Repo：`https://github.com/odyliao-lab/pikmin-seed-lens.git`，交接時已核對為 private。
- 交接分支：`codex/seed-lens-031-zhubei-handoff`；來源 checkpoint：`f368cea`，原 main 基準：`da67f71`。
- 產品：皮克敏花苗透視鏡，package `dev.ody.seedpreview`；候選來源 0.3.1／code12。
- 目標：單一自足 APK＋使用者既有 Root；不得新增控制中心、LSPosed 或 Magisk 顯示模組作為必要依賴。研究用 probe 只能是開發驗證工具，不能變成使用者安裝條件。
- 僅改顯示、縮字與還原；不做遊戲 RPC、物品異動、GPS、派遣或掃描器功能。

## 現有證據與缺口

| 組合／能力 | 截至交接的證據 |
|---|---|
| Libra ARM64／API34／156.1 | 同一 0.3.1 APK：花苗頁、未領回花苗探險頁名稱與縮字，ON／OFF 還原，新遊戲程序重新啟用已驗 |
| Virgo x86_64／API35／libnb.so／156.1 | 同一 APK：真實 ClassLoader NativeBridge，兩頁 ON／OFF，新程序重新啟用已驗 |
| 156.0／code1792142832 | 精確 profile 與離線來源驗證；**沒有登入後真實遊戲顯示驗收** |
| Android 12／API31 | **尚未驗證**；ARM64 的 API 範圍允許不等於可用；x64 目前只接受 API35／libnb.so |
| OS 重開機、長時間運作、完整詳細頁 | 尚未完整驗證；未繞過詳細頁遇到的容量提示 |

至少要有兩個 Android 版本及兩個遊戲版本的真實效果證據，且必須包含 API31。逐 ABI／API／遊戲 versionCode／Root／SELinux 組合列矩陣；不得把兩個分別通過的組合推論成全部交叉組合通過。優先 API31＋156.1，再在可合法登入的隔離環境驗第二遊戲版本；API31 上的第二遊戲組合也需獨立記錄。

## Android 12 必做開發

1. 取得實際 API31 環境，記錄 ABI、kernel、Root provider、`su -M`、SELinux、PM 安裝路徑與 APK owner。API32／API35 不能代替 API31 驗收。
2. ARM64：檢查 pidfd、ptrace／regset、CPU context 與 TLS／syscall 行為。目前未涵蓋的 SVE／SME／MTE／PAC、缺 pidfd 等仍拒絕。若 API31 實際缺能力，研究有等價安全性且附故障測試的後端；不能直接移除 guard。
3. x86_64 API31 是新的 NativeBridge 後端資格驗證。先核對實際 mapped ART／nativeloader、bridge 與 ClassLoader namespace；以自有 canary 測 JNI、錯誤、生命週期、逾時與重啟，再評估產品整合。不得只把 API35 判斷改成 API31–35、猜 opaque pointer 或關閉 SELinux。
4. ARM JNI 研究工具也有既有 API34 限制，移植時要檢查程式與新增對應測試，不可把工具拒絕誤當產品已失敗或偷偷跳過。
5. 經正常 APK UI、已登入主畫面、單次同意，驗花苗頁與未領回探險頁的名稱、長字縮小、捲動／卡片重用、OFF 原名／字型還原、遊戲重開後重新確認。只有安裝、載入或 waiting-cards 不能算顯示成功。

## 安全契約與來源導覽

先讀 `AGENTS.md`、`README.md`、`docs/ARCHITECTURE.md`、`docs/VALIDATION.md`、`docs/GAME_PROFILES.md`、`docs/ARM64_LOADER_SAFETY.md`、`docs/COMPATIBILITY_RESEARCH.md`、`docs/PROGRESS_2026-11-08.md`。舊研究階段的拒絕與未提交敘述要看日期，不覆寫歷史證據。

以 `rg --files app native research` 定位 UI／GameSession、loader、load_journal、control_transaction、supervised call、ARM64 backend／context、x64 backend／bridge host、game profiles／renderer 與其測試。先理解持久 attempt／ACK 和失敗收容，再修改平台能力選擇。

保留同 PID／start ticks／UID／boot 與版本、code、ELF SHA／大小、prologue、metadata 契約。未知版本維持原顯示；不能偽 ready、刪 UNKNOWN、跨程序補成功或重送不確定載入。呼叫前的 durable ACK／journal 與崩潰安全不可弱化。暫存器／stack snapshot 不等於 heap、TLS 或任意 dlopen 副作用已回滾。自有測試子程序的 EXITKILL 收容不能照抄為正式遊戲的通用回復方案。

## 竹北取得來源與乾淨建置

```powershell
git clone --branch codex/seed-lens-031-zhubei-handoff --recurse-submodules https://github.com/odyliao-lab/pikmin-seed-lens.git
cd pikmin-seed-lens
git remote -v
git status --short --branch
git submodule status --recursive
git switch -c codex/zhubei-android12-compat
```

若已有 repo，先核對 remote／branch／HEAD／status，保留修改，不直接 reset／覆蓋。不要只 clone main 而漏掉交接分支。

依賴：PowerShell7、Python3、JDK17、SDK platform35／build-tools35.0.0、NDK r27d、CMake3.21+、Ninja。使用本機實際、短 ASCII 工具路徑，不複製本機快取。build SDK35 與運行 API31 不衝突，不因目標 Android12 就盲降 targetSdk。

```powershell
python -m unittest discover -s research -p test_profile_evidence.py
pwsh -NoProfile -File app/build.ps1 -SdkPath <sdk> -JavaHome <jdk17> -NdkPath <ndk-r27d> -CmakePath <cmake> -NinjaPath <ninja> -Unsigned -Candidate
```

替換參數後逐條檢查 exit code。Candidate 僅供建置驗證，不得安裝或交付。Native 測試執行檔有編譯，不代表已在裝置執行。

本機本次從 `f368cea` 新建無 ignored cache／私人檔案的乾淨 clone，遞迴取得 pinned submodule 後通過上述建置：profile Python 9、Compatibility 26、InstalledNative 29、GameSession 27、APKLayout 8、PairedDigest 4；ARM64／x64 native、Java／dex 與封裝檢查通過。Unsigned candidate SHA256：`37c8c992acf60d2f8179e6bae6b2a3014db1335e871113a637fe7e2bb135c527`。本次未操作裝置、未執行 native 裝置測試、未簽署或安裝。

Submodule pins：AndKittyInjector `9ece8bc0f05344ab23e882a38728ab91f70bfed0`；KittyMemoryEx `be4cdc9a79e3e1b93357e8d30608e84a3518f5d3`。

## 不在 Git 的材料

- 舊已驗 0.3.1 簽署 APK 在原機忽略的 `.tools/build/0.3.1/signed/`；未隨本次 push 上傳。SHA256：`e6262ccdbe50314ebe80063c67943e59a3cae4b5cc9dbae9059197849d2eb284`。需要時走使用者認可的私人傳輸並核 SHA；乾淨重建不保證同位元組。
- 既有簽章憑證 SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。Keystore／密碼不在 repo，不貼提示詞、不放命令列；由使用者提供受限私人來源，程式用檔案／密碼環境變數讀取。缺簽章仍可進行 candidate 開發，不能用新 key 冒充原版升級。
- 不同交付位元組必須使用新的版本／code，保留舊 APK、hash、原簽章及可行回退；不得覆寫已交付 0.3.1。
- 原遊戲 APK／splits／metadata／dump 不在 Git；需合法來源、原簽章、完整 splits 與精確 profile。缺料只阻擋該安裝，不捏造測試結果。
- 原始截圖、裝置日誌、attempt、App 資料與私人備份不進 Git。保存於受限私有位置，repo 只留必要去敏摘要及雜湊。
- 隔離測試不需要複製掃描器 token、Google 登入 session 或正式帳號資料；登入由使用者完成，不代填或繞過驗證。

## 隔離環境與交還

竹北先盤點資源與既有 instance；明確提出 API31／ABI／Root 的測試 instance、安裝來源和操作端點。如果尚未取得該新建／安裝範圍的明確確認，先確認；不沿用台中歷史端點或 PID，不覆寫／刪除既有 instance，不降版正式遊戲。每輪操作前新鮮核對目標；不全域 kill-server 或 disconnect。

本次不要操作台中 Libra／Virgo 或啟動第二套正式 scanner。若未來另外授權借用既有掃描裝置，先核 pending／owner，正常暫停自己的作業；結束只解除自己建立的 pause，並以新 object／上傳／接受驗證回到原掃描工作，心跳不能代替。

若 156.0 強制更新而不能正常登入，不繞過伺服器／版本檢查；記錄限制，選另一個合法可執行版本，新增精確 profile 與實測，不能把離線 profile 算成第二遊戲版本完成。OS 重開機只在已授權隔離裝置做，不重啟整台電腦。

最後歷史交還證據是 2026-11-08 23:52：兩台 Lens OFF、自有 pause 解除、scanner SHA 未變，恢復後都有新的業務上傳／站台接受。本次交接沒有重新連裝置，**不代表現在狀態**。

## 提交與完成條件

分階段提交平台修復／測試與文件，push 已核對的私人功能分支，不 force、不自動合併 main、不公開發布。更新驗收矩陣，清楚分開離線、canary、安裝、真實兩頁效果、遊戲重啟、OS 重啟與長時間結果。已知未通過項目保留具體原因與下一步，不能用建置／載入代替相容完成。

可直接貼給竹北的分階段提示詞見 [ZHUBEI_PROMPTS.md](../ZHUBEI_PROMPTS.md)。
