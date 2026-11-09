# 驗收目標：Android12／14／15 × 遊戲156／156.1／157

2026-11-09使用者要求各指定版本實際成功可用，後續明確新增Android14/API34及遊戲157，範圍擴為九個交叉組合。此文件拆解完成條件，不改寫既有驗收結果；早期文件的「四組／六組」是新增API34／157前的階段範圍。

「156」依目前交接採精確156.0/code1792142832；156.1/code1793076318。若使用者另指不同code，必須先核對完整profile，不能用近似版本通過。

157已由使用者指定AutoFarm唯讀取得完整原簽章安裝包，實際157.0/code1793360009；0.4.1未知版本拒絕已驗。0.5.0新增精確profile及完整來源／負例，僅在新API35隔離機取得兩頁分項證據，見[0.5.0紀錄](PRODUCT050_QUALIFICATION.md)；新增profile不能替代逐平台真實驗收。見[來源與界線](GAME157_OFFICIAL_INTAKE.md)。

## 必要的九個交叉組合

| 實際平台 | 遊戲精確版本 | 本輪進度 | 遊戲兩頁與重啟 |
|---|---|---|---|
| Android12/API31 | 156.0/code1792142832 | 官方x64 AVD與普通App Root再驗通過，原遊戲／0.4.1同APK已安裝；原遊戲在Lens載入前Unity SEVL處SIGILL，自有probe重現31失敗／35通過 | 未到登入；指令能力缺口未解，不算通過 |
| Android12/API31 | 156.1/code1793076318 | 同一官方x64 AVD另次cold boot已正常升級完整原簽章156.1；普通App Root及同0.4.1 SHA再核。遊戲首程序在產品載入前SEVL SIGILL、後續系統另記SIGSEGV | 未完成登入；無product attempt，兩頁仍未驗，不算通過 |
| Android14/API34 | 156.0/code1792142832 | 官方x64 AVD實際API34／libndk_translation／Enforcing，ARM JNI baseline通過、SEVL模式失敗；原遊戲在軟體GPU boot實際於Unity SEVL SIGILL，未安裝Magisk或產品 | 未完成登入／產品載入，兩頁未驗，不算通過 |
| Android14/API34 | 156.1/code1793076318 | 同一AVD另一host OpenGL boot，取消前輪未完成安裝並核恢復後，新的更新session實際applied；精確156.1首程序亦於Unity SEVL SIGILL。產品API34 x64仍拒絕 | 產品兩頁未驗，不算通過 |
| Android15/API35 | 156.0/code1792142832 | 同原簽章0.5.0：首程序3804、正常重開7631與OS後1900，各自有兩頁名稱／OFF與庫存長字縮小；兩頁離屏返回已有分項畫面，歷史UNKNOWN不變，總22份attempt | 遊戲重啟未確認／舊status拒絕及重新同意已驗；OS後兩頁回歸已驗，但OS舊status截圖只到核對中，不記負例通過。探險長名縮字與長時間仍未完成 |
| Android15/API35 | 156.1/code1793076318 | 同原簽章0.5.0首程序3894、遊戲重開6349及OS後1967，各有兩頁名稱、庫存長字縮小／離屏返回、探險末排真正離屏返回與兩頁OFF；歷史UNKNOWN不變，總25份attempt。首輪載入前既有探險卡片需重開列表才更新的觀察保留 | 遊戲及OS重啟未確認／舊status實際拒絕與新同意已驗；探險更長名稱縮字仍未覆蓋，庫存最長名稱縮字後仍截斷；長時間未驗，不算整組完成 |
| Android12/API31 | 157.0/code1793360009 | 原簽章157正常更新、完整base／split SHA吻合；實際API31／Enforcing首程序在Lens載入前Unity+0x5e2760的SEVL SIGILL，16份歷史SHA不變 | 未到登入／無產品attempt，兩頁未驗，不算通過 |
| Android14/API34 | 157.0/code1793360009 | 原簽章157在同AVD的SwiftShader boot正常更新、完整SHA吻合；實際API34／Enforcing原Unity+0x5e2760 SEVL SIGILL；前次hostGL boot無回應另記，沒有Root／產品 | 未完成登入，兩頁未驗；產品API34 x64仍拒絕，不算通過 |
| Android15/API35 | 157.0/code1793360009 | 後續使用者指定AutoFarm原簽章來源，已正常更新新index5至精確157.0；0.4.1未知拒絕與歷史不變已驗，0.5.0原簽章在兩個獨立遊戲程序完成兩頁ON／OFF、花苗縮字／離屏返回及探險末排冰色三葉花苗真正離屏往返，舊同意／status拒絕與新同意已驗 | 探險長名稱縮字仍待觸發；最長庫存名縮字後仍截斷。同帳號OS關閉／啟動後兩頁ON／OFF與探險離屏返回已驗，長時間未驗，不算整組完成 |

ARM64與x64/NativeBridge各列實際資格；不能由x64上的ARM轉譯推論原生ARM64通過。最終九組須使用同一個正式候選單APK／原產品簽章及新版本/code，逐組記錄SHA；過程中的不同研究／產品artifact保留原版本與hash，不覆寫。

## 每組驗收項目

1. 新鮮核對設備／instance、bootID、API、ABI、Root provider與SELinux；由正常App身分實際取得所需Root及mount view。adb root只供研究，不算產品App已取得Root。
2. 合法原簽章遊戲base與完整splits：核包名、versionName/code、簽章一致、ELF大小/SHA、prologue與runtime metadata；未知或不完整資料保持原顯示。
3. 使用者完成登入後，在主畫面新鮮核對PID/start/UID並單次同意；離開App或遊戲程序改變後，舊同意失效。
4. 花苗頁及未領回花苗探險頁：原始OFF基準、實際名稱細分、長字縮小；正常捲動造成卡片重用後再次比對。只讀導航與捲動，不領回／拔苗／丟棄／購買／派遣或發遊戲RPC。
5. 正常App OFF後兩頁還原原文及原字型／截斷；不能只用renderer計數或waiting-cards當畫面成功。
6. 正常重啟遊戲，新PID/start不承接舊ready或consent；重新確認後再驗兩頁ON、捲動與OFF。保留全部舊attempt／UNKNOWN，不自動重送不明載入。
7. 保存該組私人截圖、receipt與SHA；repo只留去敏結果。OS重啟及長時間測試另外記錄是否已驗，不把遊戲重啟當OS重啟。

平台／版本某一列通過不推論其他列。九組上述必要項目都有真實效果證據，才可將目標標記完成。若實際156.0被伺服器強制更新，據實保留未達成；不繞過伺服器檢查或把另一遊戲版本冒充指定版本。

## 當前開發順序

1. API31 Enforcing研究：PM安裝路徑與精確host digest、持久journal/ACK及pidfd收容；自有真App ClassLoader、ARM JNI、負例與生命週期。研究包可額外存在，不能變成產品使用者依賴。
2. 使用者已明確授權API31 Root變更；私人ramdisk Magisk31.1標準setup後普通App PATH su-M／同PID1 mount view已驗，仍需產品完整載入驗收。沒有移除-M、改SELinux或放寬bridge；詳見[Root結果](API31_MAGISK_RESEARCH.md)。
3. API31共用dispatcher及普通App自行su-M／PM loader完整研究路徑已驗；Java與native使用同批開關，預設維持API35，明確signed qualification才開啟API31隔離產品驗收。原簽章0.4.1單APK已交付新隔離機；API35／156.1部分效果已驗，API31 x64原遊戲啟動失敗。見[普通App階段證據](API31_APP_ROOT_FLOW.md)、[候選來源](API31_PRODUCT_INTEGRATION.md)與[目前產品結果](PRODUCT041_QUALIFICATION.md)。
4. 原簽章與156.0／156.1合法完整splits已齊，API35／兩遊戲版已各有同0.4.1兩頁與新程序分項證據。兩版探險離屏返回已有0.4.1證據，探險長名縮字仍需樣本；不為測試派遣或改動物品。API31仍缺可運行原遊戲的目標，API34新AVD已授權且先跑平台／自有fixture／失敗收容。157的原簽章intake、舊0.4.1未知拒絕與完整profile來源審查已完成；0.5.0在API35有兩頁及遊戲重啟分項證據，其餘平台另驗。任何整合改動產品bytes都另分配更高版本/code及原簽章，再以同一新APK重驗九組。
5. 每階段測試、審查、commit/push私人功能分支`codex/zhubei-android12-compat`。不force、不自動merge main或公開發布。

研究lab key、Android15新instance及API31隔離AVD安裝官方Root provider均已明確同意並實做。後續使用者指定D槽搜尋，已核原產品keystore與156.0／156.1合法APK；三版材料及精確profile均已取得，不能把材料齊全當九組成功。最新產品分項證據見[0.4.1驗收](PRODUCT041_QUALIFICATION.md)。

API31 x64原遊戲啟動與獨立指令probe已找到SEVL缺口。原先本機標準API31 ARM64 AVD frontend提案因x64 host／API>=28架構限制撤回，使用者確認沒有可用原生ARM64手機或ARM64主機；後續獲准的直接後端研究如下，仍未解決實際遊戲啟動。見[指令能力紀錄](API31_GAME_STARTUP_BOUNDARY.md)。

後續獲准的官方rev11／同名空白ARM64 AVD，標準frontend實際拒絕；直接後端先在PCI/HDA初始化失敗，再於index5關閉後實際以virt主板比較，到達kernel／init，但keystore2反覆SIGSEGV、ADB offline，正常關閉。沒有App就緒／實際API證據或Root／產品安裝。這是[完整ARM guest軟體CPU研究](API31_ARM64_BACKEND_RESEARCH.md)，不覆寫前述標準frontend限制，也不算原生實機或NativeBridge已驗。

後續[自有CPU initramfs](API31_ARM64_CPU_MEMORY_RESEARCH.md)的相鄰向量序列在預設TCG失敗、單指令TB同原件7項通過；官方Android完整guest在hostGL及SwiftShader仍均於ADB就緒前host退出，後者保存0xC0000005。沒有把此CPU正例轉記實際API／App／產品資格，所有API31列仍未通過。[Linux host新方案](API31_LINUX_HOST_PROPOSAL.md)已由使用者確認；固定材料已核，較早UAC取消／腳本原則拒絕之後，改用原生工具實際完成WSL3.0.3 MSI exit0與VMP DISM exit3010。等待本人安排主機重開，尚未建立distro／新Android，不重跑安裝或將桌面元件當產品資格。見[準備／安裝紀錄](API31_LINUX_HOST_PREPARATION.md)。

使用者另授權直接安裝必要Google套件；新API35隔離機已由MuMu原廠谷歌安裝器完成Play／GMS／GSF，普通App Root再驗通過。Google及兩版遊戲的各次登入均由使用者完成，產品分項結果見上表；沒有複製遊戲session。Google來源與早期階段結果見[Google套件階段](API35_GOOGLE_INTAKE.md)。

新增API34的官方來源、普通App ARM JNI基準與SEVL失敗、兩版原遊戲啟動及GPU比較另見[API34隔離研究](API34_ISOLATED_RESEARCH.md)。新AVD的Root與產品載入尚未進行；不能用歷史Libra ARM64或API35/libnb的結果替代。

157原遊戲亦已分別在API31／34 x64實際於Unity+0x5e2760 SEVL SIGILL；不是由156.x跨版推論。API34本次首輪hostGL freeze的指定正常關閉逾時與精確host收容、後輪SwiftShader正常關閉均另記，見[157原遊戲獨立啟動](GAME157_PLATFORM_STARTUP.md)。

同最終0.5.0的156.x已獨立重驗：156.0與156.1各自首輪、遊戲重啟及同帳號OS後兩頁已有分項證據；探險更長名稱縮字仍缺確認、庫存最長名稱仍截斷、長時間未驗，見[本版156.x回歸](PRODUCT050_GAME156_REGRESSION.md)。
