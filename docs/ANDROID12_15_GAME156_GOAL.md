# 持續目標：Android12／15 × 遊戲156／156.1

2026-11-09使用者明確要求設定持續目標並推進到各指定版本實際成功可用。此文件拆解完成條件，不改寫既有驗收結果。

「156」依目前交接採精確156.0/code1792142832；156.1/code1793076318。若使用者另指不同code，必須先核對完整profile，不能用近似版本通過。

## 必要的四個交叉組合

| 實際平台 | 遊戲精確版本 | 本輪進度 | 遊戲兩頁與重啟 |
|---|---|---|---|
| Android12/API31 | 156.0/code1792142832 | 官方x64 AVD與普通App Root再驗通過，原遊戲／0.4.1同APK已安裝；原遊戲在Lens載入前Unity SEVL處SIGILL，自有probe重現31失敗／35通過 | 未到登入；指令能力缺口未解，不算通過 |
| Android12/API31 | 156.1/code1793076318 | 尚未在該平台安裝／啟動；目前x64 bridge缺少SEVL能力，不能推論本列通過或以API32替代 | 未驗 |
| Android15/API35 | 156.0/code1792142832 | 新MuMu index5已核原簽章完整splits及profile；原廠PM拒絕直接降版後，只將該隔離機遊戲乾淨重裝為156.0，已到首次生日／登入畫面，等待使用者本人登入 | 產品未載入；未驗，尚不能判定伺服器是否要求更新 |
| Android15/API35 | 156.1/code1793076318 | 原簽章0.4.1已驗兩頁名稱／OFF、花苗長字／捲動及另一新程序重新確認後再啟用／OFF；首筆UNKNOWN保留 | 部分已驗；探險長名／離屏重用待更多樣本，OS／長時間未驗 |

ARM64與x64/NativeBridge各列實際資格；不能由x64上的ARM轉譯推論原生ARM64通過。最終四組須使用同一個正式候選單APK／原產品簽章及新版本/code，逐組記錄SHA；過程中的不同研究／產品artifact保留原版本與hash，不覆寫。

## 每組驗收項目

1. 新鮮核對設備／instance、bootID、API、ABI、Root provider與SELinux；由正常App身分實際取得所需Root及mount view。adb root只供研究，不算產品App已取得Root。
2. 合法原簽章遊戲base與完整splits：核包名、versionName/code、簽章一致、ELF大小/SHA、prologue與runtime metadata；未知或不完整資料保持原顯示。
3. 使用者完成登入後，在主畫面新鮮核對PID/start/UID並單次同意；離開App或遊戲程序改變後，舊同意失效。
4. 花苗頁及未領回花苗探險頁：原始OFF基準、實際名稱細分、長字縮小；正常捲動造成卡片重用後再次比對。只讀導航與捲動，不領回／拔苗／丟棄／購買／派遣或發遊戲RPC。
5. 正常App OFF後兩頁還原原文及原字型／截斷；不能只用renderer計數或waiting-cards當畫面成功。
6. 正常重啟遊戲，新PID/start不承接舊ready或consent；重新確認後再驗兩頁ON、捲動與OFF。保留全部舊attempt／UNKNOWN，不自動重送不明載入。
7. 保存該組私人截圖、receipt與SHA；repo只留去敏結果。OS重啟及長時間測試另外記錄是否已驗，不把遊戲重啟當OS重啟。

平台／版本某一列通過不推論其他列。至少四組上述必要項目都有真實效果證據，才可將此持續目標標記完成。若實際156.0被伺服器強制更新，據實保留未達成；不繞過伺服器檢查或把另一遊戲版本冒充指定版本。

## 當前開發順序

1. API31 Enforcing研究：PM安裝路徑與精確host digest、持久journal/ACK及pidfd收容；自有真App ClassLoader、ARM JNI、負例與生命週期。研究包可額外存在，不能變成產品使用者依賴。
2. 使用者已明確授權API31 Root變更；私人ramdisk Magisk31.1標準setup後普通App PATH su-M／同PID1 mount view已驗，仍需產品完整載入驗收。沒有移除-M、改SELinux或放寬bridge；詳見[Root結果](API31_MAGISK_RESEARCH.md)。
3. API31共用dispatcher及普通App自行su-M／PM loader完整研究路徑已驗；Java與native以同一unsigned候選開關整合，預設／可簽署路徑仍維持API35。兩種完整建置通過，未安裝或交付；正常產品及遊戲驗收待補。見[普通App階段證據](API31_APP_ROOT_FLOW.md)及[候選來源](API31_PRODUCT_INTEGRATION.md)。
4. 取得原產品簽章與合法遊戲splits，分配新產品版本及同批APK，才進四組正常UI實測。需要登入時提示使用者。
5. 每階段測試、審查、commit/push私人功能分支`codex/zhubei-android12-compat`。不force、不自動merge main或公開發布。

研究lab key、Android15新instance及API31隔離AVD安裝官方Root provider均已明確同意並實做。後續使用者指定D槽搜尋，已核原產品keystore與兩版合法APK；材料不再缺，不能把材料齊全當四組成功。最新產品分項證據見[0.4.1驗收](PRODUCT041_QUALIFICATION.md)。

API31 x64原遊戲啟動與獨立指令probe已找到SEVL缺口；已提出新的官方API31 ARM64具體隔離方案待確認，不能把新環境計畫算成已建立或已驗。見[指令能力紀錄](API31_GAME_STARTUP_BOUNDARY.md)。

使用者另授權直接安裝必要Google套件；新API35隔離機已由MuMu原廠谷歌安裝器完成Play／GMS／GSF，普通App Root再驗通過。Google及156.1遊戲登入已由使用者完成，156.1產品分項結果見上表；改裝156.0後另待本人登入，不複製156.1 session。Google來源與早期階段結果見[Google套件階段](API35_GOOGLE_INTAKE.md)。
