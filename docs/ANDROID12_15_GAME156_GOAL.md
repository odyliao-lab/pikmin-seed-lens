# 驗收目標：Android12／14／15 × 遊戲156／156.1

2026-11-09使用者要求各指定版本實際成功可用，後續明確新增Android14/API34，範圍擴為六個交叉組合。此文件拆解完成條件，不改寫既有驗收結果；早期文件的「四組」是新增API34前的階段範圍。

「156」依目前交接採精確156.0/code1792142832；156.1/code1793076318。若使用者另指不同code，必須先核對完整profile，不能用近似版本通過。

## 必要的六個交叉組合

| 實際平台 | 遊戲精確版本 | 本輪進度 | 遊戲兩頁與重啟 |
|---|---|---|---|
| Android12/API31 | 156.0/code1792142832 | 官方x64 AVD與普通App Root再驗通過，原遊戲／0.4.1同APK已安裝；原遊戲在Lens載入前Unity SEVL處SIGILL，自有probe重現31失敗／35通過 | 未到登入；指令能力缺口未解，不算通過 |
| Android12/API31 | 156.1/code1793076318 | 同一官方x64 AVD另次cold boot已正常升級完整原簽章156.1；普通App Root及同0.4.1 SHA再核。遊戲首程序在產品載入前SEVL SIGILL、後續系統另記SIGSEGV | 未完成登入；無product attempt，兩頁仍未驗，不算通過 |
| Android14/API34 | 156.0/code1792142832 | 新授權官方x64 AVD實際API34／libndk_translation／Enforcing，ARM JNI baseline通過、SEVL模式失敗；原簽章156.0已裝，首啟遇system ANR，官方軟體GPU比較中，尚未安裝Magisk或產品 | 未完成登入／產品載入，兩頁未驗 |
| Android14/API34 | 156.1/code1793076318 | 同一新API34隔離目標；x64產品目前仍拒絕此API，須先實測平台／失敗收容再整合 | 未驗 |
| Android15/API35 | 156.0/code1792142832 | 本人登入後，同原簽章0.4.1已驗兩頁名稱／OFF、花苗長字／四次捲動，以及新程序重新確認後再啟用／捲動往返／兩頁OFF；歷史UNKNOWN不變 | 部分已驗；探險長名／離屏重用待更多樣本，OS／長時間未驗 |
| Android15/API35 | 156.1/code1793076318 | 原簽章0.4.1已驗兩頁名稱／OFF、花苗長字／捲動及另一新程序重新確認後再啟用／OFF；首筆UNKNOWN保留 | 部分已驗；探險長名／離屏重用待更多樣本，OS／長時間未驗 |

ARM64與x64/NativeBridge各列實際資格；不能由x64上的ARM轉譯推論原生ARM64通過。最終六組須使用同一個正式候選單APK／原產品簽章及新版本/code，逐組記錄SHA；過程中的不同研究／產品artifact保留原版本與hash，不覆寫。

## 每組驗收項目

1. 新鮮核對設備／instance、bootID、API、ABI、Root provider與SELinux；由正常App身分實際取得所需Root及mount view。adb root只供研究，不算產品App已取得Root。
2. 合法原簽章遊戲base與完整splits：核包名、versionName/code、簽章一致、ELF大小/SHA、prologue與runtime metadata；未知或不完整資料保持原顯示。
3. 使用者完成登入後，在主畫面新鮮核對PID/start/UID並單次同意；離開App或遊戲程序改變後，舊同意失效。
4. 花苗頁及未領回花苗探險頁：原始OFF基準、實際名稱細分、長字縮小；正常捲動造成卡片重用後再次比對。只讀導航與捲動，不領回／拔苗／丟棄／購買／派遣或發遊戲RPC。
5. 正常App OFF後兩頁還原原文及原字型／截斷；不能只用renderer計數或waiting-cards當畫面成功。
6. 正常重啟遊戲，新PID/start不承接舊ready或consent；重新確認後再驗兩頁ON、捲動與OFF。保留全部舊attempt／UNKNOWN，不自動重送不明載入。
7. 保存該組私人截圖、receipt與SHA；repo只留去敏結果。OS重啟及長時間測試另外記錄是否已驗，不把遊戲重啟當OS重啟。

平台／版本某一列通過不推論其他列。六組上述必要項目都有真實效果證據，才可將目標標記完成。若實際156.0被伺服器強制更新，據實保留未達成；不繞過伺服器檢查或把另一遊戲版本冒充指定版本。

## 當前開發順序

1. API31 Enforcing研究：PM安裝路徑與精確host digest、持久journal/ACK及pidfd收容；自有真App ClassLoader、ARM JNI、負例與生命週期。研究包可額外存在，不能變成產品使用者依賴。
2. 使用者已明確授權API31 Root變更；私人ramdisk Magisk31.1標準setup後普通App PATH su-M／同PID1 mount view已驗，仍需產品完整載入驗收。沒有移除-M、改SELinux或放寬bridge；詳見[Root結果](API31_MAGISK_RESEARCH.md)。
3. API31共用dispatcher及普通App自行su-M／PM loader完整研究路徑已驗；Java與native使用同批開關，預設維持API35，明確signed qualification才開啟API31隔離產品驗收。原簽章0.4.1單APK已交付新隔離機；API35／156.1部分效果已驗，API31 x64原遊戲啟動失敗。見[普通App階段證據](API31_APP_ROOT_FLOW.md)、[候選來源](API31_PRODUCT_INTEGRATION.md)與[目前產品結果](PRODUCT041_QUALIFICATION.md)。
4. 原簽章與兩版合法完整splits已齊，API35／兩遊戲版已各有同0.4.1兩頁與新程序分項證據。兩版缺少的探險長名／離屏重用需真實樣本；不為測試派遣或改動物品。API31仍缺可運行原遊戲的目標，API34新AVD已授權且先跑平台／自有fixture／失敗收容。若API34整合需改產品bytes，另分配更高版本/code及原簽章，再以同一新APK重驗六組。
5. 每階段測試、審查、commit/push私人功能分支`codex/zhubei-android12-compat`。不force、不自動merge main或公開發布。

研究lab key、Android15新instance及API31隔離AVD安裝官方Root provider均已明確同意並實做。後續使用者指定D槽搜尋，已核原產品keystore與兩版合法APK；材料不再缺，不能把材料齊全當六組成功。最新產品分項證據見[0.4.1驗收](PRODUCT041_QUALIFICATION.md)。

API31 x64原遊戲啟動與獨立指令probe已找到SEVL缺口。原先本機官方API31 ARM64 AVD提案已因官方frontend的x64 host／API>=28架構拒絕而撤回；未下載、建立或啟動該環境。現待指定API31原生ARM64手機或ARM64主機及具體隔離範圍，不能把新環境計畫算成已驗。見[指令能力紀錄](API31_GAME_STARTUP_BOUNDARY.md)。

使用者另授權直接安裝必要Google套件；新API35隔離機已由MuMu原廠谷歌安裝器完成Play／GMS／GSF，普通App Root再驗通過。Google及兩版遊戲的各次登入均由使用者完成，產品分項結果見上表；沒有複製遊戲session。Google來源與早期階段結果見[Google套件階段](API35_GOOGLE_INTAKE.md)。

新增API34的官方來源、正常stock boot、普通App ARM JNI基準與SEVL失敗，以及原遊戲啟動／GPU比較另見[API34隔離研究](API34_ISOLATED_RESEARCH.md)。新AVD的Root與產品載入尚未進行；不能用歷史Libra ARM64或API35/libnb的結果替代。
