# pikmin-seed-lens
皮克敏花苗透視鏡

獨立 Android APK 的開發原型：只顯示花苗名稱與縮小長文字，不修改遊戲物品。
目標是使用既有 Root，不另安裝控制中心、LSPosed 或花苗顯示模組。

**目前仍是驗證原型，不是多版本正式版。** 竹北隔離驗收版 `0.5.0/code15`（新增157精確profile，分項實測）；先前交付基線保留`0.3.1/code12`，包名
`dev.ody.seedpreview`，沿用原簽章。單一APK內含ARM64後端、x64 host及同批ARM renderer，
不需要控制中心或額外顯示模組。PM路徑／SHA／stored ZIP／16KB對齊均在建置與執行時核對。

2026-11-09竹北已取得完整交接分支與recursive pins，乾淨candidate建置通過。
新建隔離MuMu「12.0」實際回報API32，已停止並關閉，不能算必要API31驗收。
另已在官方API31 x64 AVD／Enforcing完成原生隔離安全測試，定位到平台export及
`su -M`差異；API31專用adapter已在自有真App通過JNI／ClassLoader canary及逾時收容，
產品完整流程與遊戲仍未驗。新API35隔離機內建KernelSU的正常App `su -M`及十例收容已驗。
後續使用者已授權API31安裝Magisk31.1：普通App `su -M`與Magisk-domain JNI／收容已驗，
正常產品完整流程及遊戲仍未驗，見[API31 Root階段](docs/API31_MAGISK_RESEARCH.md)。
其後普通研究App已自行完成`su -M → 自身PM loader → ARM JNI`、故意timeout及新程序回歸；
仍不是產品或遊戲驗收，見[普通App完整研究路徑](docs/API31_APP_ROOT_FLOW.md)。
產品x64仍限API35；詳見[本輪隔離研究與失敗修正](docs/ZHUBEI_LAB_20261109.md)。
見[API31實際研究結果](docs/API31_AVD_RESEARCH.md)及
[本輪接手、環境及下一步](docs/handoffs/2026-11-09-zhubei-intake.md)。

目前驗收目標為Android12/API31、Android14/API34、Android15/API35與遊戲156.0／156.1／157九組真實可用；
每組都要驗兩頁、縮字、捲動、OFF及遊戲重啟，不以研究建置代替完成。
157已由使用者指定AutoFarm唯讀取得原簽章完整base／split，核為157.0/code1793360009；舊0.4.1未知版本拒絕已實跑。新增精確profile及來源負例後，原簽章0.5.0僅安裝新index5，已在新API35的兩個遊戲程序驗兩頁ON／OFF、花苗縮字及探險末排離屏返回；探險長名縮字、OS／長時間及其他平台仍未驗。見[157來源界線](docs/GAME157_OFFICIAL_INTAKE.md)。
見[九組驗收條件與進度](docs/ANDROID12_15_GAME156_GOAL.md)。

API31平台來源已接入明確的unsigned候選，Java與native使用同一開關，
與隔離研究共用adapter／runtime dispatcher；預設建置仍維持API35資格，明確signed qualification另見下方，API32不接受。API31正常產品完整流程與遊戲仍待驗，
見[API31候選整合及測試邊界](docs/API31_PRODUCT_INTEGRATION.md)。

使用者授權後，新API35隔離機已用MuMu原廠谷歌安裝器安裝Play／GMS／GSF，
普通App su-M再驗通過；其後使用者已完成Google與兩個精確遊戲版本的各次登入。
見[Google套件來源與實際結果](docs/API35_GOOGLE_INTAKE.md)。
安裝後API35普通App自行su-M／PM loader的正常、timeout及新程序回歸亦通過，
見[API35普通App研究](docs/API35_APP_ROOT_FLOW.md)，仍不是遊戲驗收。

使用者指定D槽搜尋後，已取得原產品簽章及156.0完整base／ARM64 split，Play取得的156.1亦符合精確profile。
原簽章0.4.1單APK已在新API35／156.0與156.1分別實際驗花苗名稱、長字縮小、捲動與兩頁OFF還原；
兩版各自的新程序重新確認／再啟用與OFF亦已驗；本人更換樣本後，兩版探險花苗真正離屏返回及兩頁OFF已有獨立畫面證據。
探險長名稱觸發縮字仍未覆蓋；庫存最長紀念金苗名稱雖縮字仍會截斷，見[樣本回歸與限制](docs/API35_SEED_EXPEDITION_SAMPLE.md)。其他組合仍未完成。
明確`-Api31Qualification`才允許原簽章新版本開啟API31驗收，預設仍API35；見[0.4.1分項紀錄](docs/PRODUCT041_QUALIFICATION.md)。

API31 x64上156.0與156.1各次實際啟動均在Lens載入前出現Unity SEVL指令SIGILL；156.1系統後續另記SIGSEGV。自有ARM-only probe已重現31失敗、35返回。
這個image仍未通過遊戲能力；本機標準ARM64 AVD frontend實際拒絕，獲准的完整ARM64 guest後端研究已到kernel／init，但keystore2反覆SIGSEGV、ADB offline，尚無正常App環境，見[指令能力界線](docs/API31_GAME_STARTUP_BOUNDARY.md)及[ARM64後端研究](docs/API31_ARM64_BACKEND_RESEARCH.md)。

以下為0.3.1的歷史基線：Virgo API35／x86_64／libnb.so／遊戲156.1已從正常App入口實際套用花苗頁及探險頁，
兩頁關閉後還原原名及字型。Libra API34／ARM64也已用同一0.3.1 APK完成兩頁
顯示與OFF還原回歸。該階段156.0僅完成離線profile；本輪0.4.1才新增上方API35／156.0分項實測。
主畫面確認綁定同PID/start單次使用，離開App就失效。本輪未重新操作上述正式節點。

ON／載入／OFF由單一native lease協調，保留持久attempt；新啟用要求同PID/start的新鮮
renderer回報。NativeBridge僅對已驗環境提供明確同意的候選入口：按下啟用先切回遊戲，
核對安全user-space thread後載入；呼叫結果不明時只停止pidfd鎖定的該次遊戲，保留UNKNOWN，
不自動重送、不稱為副作用回滾。實測與失敗修正見[驗收矩陣](docs/VALIDATION.md)。

## 已知限制

- 精確設定包括 **156.1/code1793076318**、**156.0/code1792142832** 與 **157.0/code1793360009**，不接受近似版本；
  兩版API35均有0.4.1分項畫面證據；API31未通過，API34新隔離環境研究中。未知版本保持原顯示。
- Android API28–35／ARM64 是候選範圍；同版0.3.1 APK在Libra API34及Virgo API35有實際
  顯示與OFF證據。其他Android、NativeBridge ROM及遊戲版本尚未通過。
- x64僅允許API35/libnb.so的既有測試組合。取得namespace不等於全部NativeBridge相容；
  CPU呼叫收容的七類故障測試使用自己的子程序，未刻意在正式遊戲製造逾時。
- Root provider 需支援 `su -M` 的全域 mount view；沒有宣稱所有 Root／SELinux 環境支援。
- 無自動開機或遊戲重啟接續；目前由使用者確認已登入主畫面後啟用。
- 模糊載入結果保留append-only attempt，不自動重試；0.3.1 ARM64已有界收容，但拒絕尚未
  涵蓋的SVE／SME／MTE／PAC與缺pidfd環境，不宣稱所有API28–35硬體都適用。
  Libra同版正常遊戲重開／再啟用、Virgo新程序再啟用已驗兩頁ON/OFF；OS重啟、
  詳細頁、長時間運作及其他平台上的第二遊戲profile仍待驗證。

0.3.1已補齊ARM64呼叫有界收容、CPU context與實際JNI／ART隔離測試，並在Libra
完成新遊戲程序兩頁ON/OFF、恢復原掃描；Virgo同版兩頁ON/OFF亦已完成。既有0.3.0產物不改，
不追溯附加新保護。見[ARM64整合進度](docs/ARM64_LOADER_SAFETY.md)。

兩台均已有實際花苗及探險頁ON/OFF私人截圖，不再只有地圖或載入基準。
Libra 保留既有 Magisk、LSPosed 及蘑菇掃描套件，未安裝新的顯示模組；
因此證明的是不依賴控制中心，不是已驗證移除所有其他框架的乾淨系統。
原始截圖與私人紀錄不進 Git。詳細驗證範圍見驗收文件。

## 取得來源與建置

```powershell
git clone --recurse-submodules https://github.com/odyliao-lab/pikmin-seed-lens.git
cd pikmin-seed-lens
# 已經 clone 時：git submodule update --init --recursive
pwsh -NoProfile -File app/build.ps1 `
  -SdkPath C:/Android/sdk -JavaHome C:/Tools/jdk17 `
  -NdkPath C:/Tools/android-ndk-r27d `
  -CmakePath C:/Tools/cmake/bin/cmake.exe -NinjaPath C:/Tools/ninja.exe -Unsigned
```

將範例工具路徑改為本機實際位置。需要 PowerShell7、JDK17、SDK platform35／
build-tools35.0.0、NDK r27d、CMake3.21+及Ninja。建置不安裝、不操作裝置。
`-Unsigned` 產物只供建置驗證，不可視為原簽章升級包。

簽章建置不帶 `-Unsigned`，改傳 repo 外的 `-SigningKey`。
密碼由既有安全來源放入 `SEED_LENS_STORE_PASS`／`SEED_LENS_KEY_PASS` 環境變數；
可用 `-StorePassEnv`／`-KeyPassEnv` 指定其他變數名稱。不得把密碼放命令列或 repo。
腳本驗證既有憑證指紋，沒有新建／複製 keystore，也不依賴控制中心的私有建置 helper。

開發中的未交付來源可加`-Unsigned -Candidate`，輸出到唯一的`.tools/candidates/`子目錄。
此模式僅供完整建置／封裝檢查，保留開發manifest並明確禁止安裝／交付；正式交付仍須分配
新版本、原簽章及實機驗收，不可用candidate覆蓋任何既有APK。

輸出在忽略的 `.tools/`，同一路徑已有最終產物時拒絕覆寫。
重建結果不是先前已交付 0.1.4 的位元組保證；日後交付不同 APK 必須升版本／code。

## 文件

- [竹北接手與 Android 12 必要開發範圍](docs/handoffs/2026-11-09-zhubei-takeover.md)
- [竹北分階段接手提示詞](docs/ZHUBEI_PROMPTS.md)
- [架構與來源界線](docs/ARCHITECTURE.md)
- [驗證結果與待辦](docs/VALIDATION.md)
- [APK＋Root 跨平台研究與隔離測試](docs/COMPATIBILITY_RESEARCH.md)
- [精確遊戲設定檔與新增版本流程](docs/GAME_PROFILES.md)
- [ARM64載入安全研究與整合門檻](docs/ARM64_LOADER_SAFETY.md)
- [來源沿革與第三方授權](docs/PROVENANCE.md)

專案原始碼保持私有；第三方元件各依其原有授權，沒有替整個專案新增開源授權。
