# 架構

0.15.2／0.15.3 修正 API35 更新安裝回呼：只對明確 session 的非匯出 Activity PendingIntent 加上 creator BAL opt-in，保留 Android 使用者確認與 APK／session／載入互斥守門。確認 Intent 缺失或不能開啟時取消並提示重試；不靜默 Root 安裝。[裝置驗收與版本界線](API35_INSTALLER_FIX.md)。

0.15.0 開發：首次使用入口與本機診斷使用獨立 RootDiagnostic 嚴格解析器，不改載入命令或遠端回報 schema。基本檢查與即時遊戲資格分開；[設計與驗證界線](SETUP_NOTIFICATIONS_UPDATES.md)。

通知沿既有 LensService／LensController，定期更新固定狀態，關閉按鈕走原持久 pending-OFF。新增通知權限僅供系統呈現，不是載入前提；不產生第二個控制器或使用活動頁的 observer 槽。

0.15.1 先提交唯讀回報再啟動 monitor，避免 monitor 的 active gate 讓背景回報永遠被跳過。回報完成於主執行緒更新通知，仍核對原程序與5秒時效；服務銷毀後不發布回呼。[MuMu 回歸](VALIDATION_0151_MUMU.md)。

UpdateCheckPolicy只決定開App時的metadata檢查；失敗次數／嘗試及成功時間持久化，手動檢查可略過等待但不能略過安裝互斥／版本與來源核對。快取不表示剛取得新回覆。

0.14.2工具頁分為固定橫排標題／頁籤、可捲動的緊湊功能列表，以及固定底部操作／版本。僅MainActivity與LensTabs呈現層調整，診斷內容、意願控制器及原生邏輯不變；共通錯誤集中顯示，完整文字仍可換行。[版面驗證](COMPACT_UI.md)。

## 可預先選擇的功能意願

0.14.1 將五個開關意願保存於 App 私有偏好；原生控制／遊戲效果分別確認。單一 worker 背景同步，修訂號拒絕舊回報覆蓋新選擇；只有取得首次套用同意且資格成立，才同步有效 ON。pending-OFF 仍先取得原生關閉確認，再套用之後的新意願。Root／原生載入 gate 不再控制 UI 是否可點選。[行為、遷移與驗證](FEATURE_INTENT.md)。

## 獨立 GPS 複製

0.13.3 在四組下方新增預設 OFF 的第五開關，沿用自包含載入、lease、精確 Game157／PID／build 守門與重綁服務。Canvas UI-thread tick 只處理單一 focused／active 詳情頁，session 與項目穩定 400ms 後，以 APK 內 JNI ClipboardManager 寫入並讀回確認；弱參考上限八頁、失敗每項三次。自有控制／回報只有意願與計數，不含座標。所有開關可排隊寫入，背景報告查詢不占用操作 gate；仍用同一 serial worker。詳細行為及資格見 [GPS_COPY](GPS_COPY.md)。

## 診斷與更新

0.12.1/code45 候選以 APK 自有 HTTPS 直傳固定 Sites `/api/submit`，同意後才產生 `{report,email}` 信封，原報告／匯出不含 Email。原生 client header 選擇無瀏覽器 Origin 的公開協定，不提供管理權限；不得放寬錯誤 Origin／讀取授權。無重導向或自動重送，精確收據後才確認成功。[行為、Android 實測與限制](APP_DIRECT_UPLOAD.md)。

Sites 版回報站使用 Vinext Worker＋D1，診斷與選填通知 Email 分表保存；上傳免登入，管理讀寫以 Sites 登入身分及 runtime ADMIN_EMAIL 守門。原 App schema 不變，Email 不放診斷下載。Sites 已開放公開入口，未改 APK 上傳端點。[原始碼與資料保存限制](../server/sites/README.md)。Python 版保留為既有本機接收端，不用它提供 Sites 能力。

伺服器沿用封閉 JSON 接收端，新增同源上傳頁與私人管理後台；報告仍為不可覆寫 JSON，SQLite 只保存處理狀態。管理 session 保留記憶體，密碼雜湊從外部環境讀取；APK 未設定端點。[後台部署與驗收界線](../server/diagnostics/README.md)。

0.11.2 新增獨立診斷程序、封閉回報 schema 與同意匯出；Root 檢查只讀，不經遊戲載入器。新增 INTERNET 僅供使用者允許的版本檢查／下載及日後同意的回報，接收端目前未設定。更新安裝使用 Android PackageInstaller 與 App 私人載入 interlock；不引入核心／模組或 Root 安裝。[本輪設計與驗收界線](DIAGNOSTICS_UPDATES.md)。

## 執行狀態與關閉全部

0.10.3 新增只讀狀態彙整與前景定期核對；關閉全部使用 App 持久 pending-OFF，加上既有原生 lease／原子 writer，同步四主意願及八成員。載入、版本與 UNKNOWN 守門不變。[行為與驗證](STATUS_ALL_OFF.md)。

## 四組顯示移植（開發中）

沿用既有 APK 自包含載入器；[最新功能對照與驗收界線](DISPLAY_PORT_RESULTS.md)。Game157 使用獨立唯讀 PlayerManager／InventoryManager context、八項 seed-lens 控制／回報與四個先保存主意願。0.9.5 在 MuMu API32／35 表列畫面與 OFF 還原已觀察，API35 新 PID 自動重綁亦通過；API32 重啟及缺樣本仍分別待驗。

App 內建使用者啟用的前景服務管理新 PID 綁定，不從開機啟動，不重送 UNKNOWN。loader 對 Game157 要求同 APK build 的實體回報；遊戲仍映射舊 PM APK 時先拒絕，不再次注入。舊設定只在獨立 App 自有控制交易紀錄證明擁有權後一次遷移，不把其他 App 全域檔當執行來源。歷史 attempt 原件保留。

## 0.7.3：桌面圖示與直式

Manifest的application icon及roundIcon指向原生adaptive icon，森林底色及放大鏡／花苗
vector foreground隨桌面遮罩縮放；aapt2先compile再link資源。MainActivity宣告
screenOrientation=portrait，只限制透視鏡Activity，不修改遊戲或系統旋轉設定。
controller、Root、native及版本／程序守門不變。[實測與發布](PRODUCT073_ICON_PORTRAIT.md)。

## 0.7.2：已實測的MuMu API32獨立後端

在隔離普通App Root／CPU／正常PM-JNI／timeout收容／新程序研究通過後，才開放
API32＋libnb.so的獨立四參數adapter及Java/native一致准入。API35仍使用既有五參數
adapter，選定後不fallback。精確SDK文字與native API、bridge、遊戲及程序guard保留。
API31／34候選旗標仍false；下方舊階段「僅API35／拒絕API32」不代表本版政策。

`RootCommand`僅API32 x86_64事先選定內建su-c；前綴驗UID0、非空且相同PID1 mount
namespace，關閉stdin。其他tuple仍su-M，不在失敗或UNKNOWN後換命令重試。
原context的orig_rax=-1路徑保留；額外負標記僅API32/libnb啟用，需停止task的syscall-info
arch/IP/SP吻合、非restart，初始只接受NONE；自己完成且恢復的remote syscall後才允許EXIT。
CPU研究與產品共用這段受限判別，沒有跳過進行中syscall。

產品仍是同一APK自帶loader／host／配對renderer，不依賴已移除的三個研究App。
單頁介面與UNKNOWN禁止重送保持；[本版實機與界線](PRODUCT072_API32_STOCK.md)。

0.7.0/code26依使用者最新要求改為單頁精簡介面，移除分頁／外觀選單／常駐技術區。
`MainActivity`仍只持有當次同意，`LensController`／`LensOperations`及native守門保持原樣；
只讀核對、直接OFF與UNKNOWN禁止重送保留。外觀跟隨系統，舊外觀偏好保留但不再作為入口。

## 森林手帳原生介面（驗收中）

`MainActivity`只保有頁面與當次同意；`LensController`以程序級單一操作鎖持有執行，
`LensOperations`承接既有Root／PM／native命令。`LensContext`綁boot/PID/start，
`LensStatus`嚴格轉譯回報及UNKNOWN屏障，外觀偏好不能授權載入。`ToolCatalog`目前
只有花苗名稱透視一項，行內直接開關；`ForestStyle`及離線`OfflineArtwork`負責呈現。
APK原簽章與版本遞增；進度／未通過項目見[UI驗收](FOREST_UI_QUALIFICATION.md)。

## API34共用後端的unsigned來源準備

已實測API34直接adapter的普通App Root／PM／JNI及收容後，新增獨立
`-Api34Candidate -Candidate -Unsigned`，可與API31候選同APK，沒有signed API34入口。
`PlatformForRuntime`在ART前按精確runtime tuple及編譯旗標選擇，兩種四參數adapter與
API35五參數型別分開、失敗不fallback。checked-in Java兩旗標皆false，native cache／
編譯定義／封裝marker與Java生成副本同一選擇；預設及signed的x64資格仍只API35。
新共用dispatcher研究件尚需獨立實跑，原遊戲啟動與產品驗收未因此通過。見
[本次來源、建置與邊界](API34_PRODUCT_INTEGRATION.md)。

最新隔離產品驗收增加明確`-Api31Qualification`：只接受原簽章的新version/code，與unsigned candidate模式互斥；Java／native仍同一開關，預設仍API35。API31／35 canary及普通App研究通過後，才以此變體進行真正遊戲驗收；不是直接宣告API31合格。來源、APK與實測限制見[0.4.1紀錄](PRODUCT041_QUALIFICATION.md)。以下unsigned-only描述為前一準備階段。

## API31 native候選來源整合（未交付）

API31隔離真App證據後，`bridge_platform_api31.h`共用四參數adapter，`bridge_dispatch.h`按native API／完整SDK文字／精確bridge選擇介面；root loader與worker也重核相同tuple。預設仍僅API35/libnb，明確unsigned候選才編入API31/libndk_translation，不在失敗後換ABI。`app/build.ps1 -Api31Candidate`強制`-Candidate -Unsigned`，同一開關生成獨立Java准入副本並選擇native cache／ON或OFF，且核對cache及封裝host marker。checked-in Java維持API31候選false；預設／可簽署建置仍拒絕API31，所有變體仍拒絕API32。實測與未達條件見[候選整合](API31_PRODUCT_INTEGRATION.md)，不是API31產品驗收或新交付。

## 0.3.1 ARM64安全整合

工作區`arm64_backend.cpp`已與`supervised_load.h`整合，兩ABI UI皆須明確fail-stop同意。
`arm64_context.h`核對GPR／FPSIMD／TLS pointer／syscall regset／原stack；正常call使用
獨立guarded stack，syscall只使用已存在SVC。離開syscall-exit後才還原snapshot，避免
ARM64 kernel恢復其保存的x7而覆寫參數。任何UNKNOWN停止worker，由父程序處理精確pidfd。
`research/arm64-bounded-trace.cmake`對固定upstream生成overlay，不改第三方來源。
候選拒絕未涵蓋的SVE／SME／MTE／PAC及缺pidfd環境，沒有宣稱泛用所有ARM64。
32例fork隔離證據含真實SO載入／呼叫／卸載；另外自有Android App已完成真實JNI／ART
及thread lifecycle，0.3.1在Libra新遊戲程序完成兩頁ON/OFF，原掃描已恢復。
Virgo同一0.3.1 APK亦已完成兩頁ON/OFF。下列0.3.0維持原產物，不追溯具備新保護。

## 目前入口（0.3.0引入，0.3.1沿用）

下方0.1.6／未交付候選段落保留工程沿革；「x64仍拒絕／未啟用」只描述該階段。
現在API35/libnb.so精確測試組合已接入`x64_backend.cpp`，其餘NativeBridge仍拒絕。
UI明確確認主畫面與未知呼叫的fail-stop，再正常切回遊戲，避免cached-app freezer使
所有候選thread凍結。兩秒是切換緩衝，不代替PID/start/實際user-context核對。

`supervised_load.h`由父程序持久記錄A/C/R/D後ACK，子worker才能進行下一步。
CALLING之前不設定EXITKILL、不寫tracee；其後使用pidfd限定目標、15秒整體界限與2秒
單call界限。未知函式／syscall停止worker，不再remote cleanup或重試；父程序收容精確
遊戲並保留UNKNOWN。這是明確授權的停止，不是任意dlopen副作用能安全回滾的主張。
正常返回仍須完整journal、同PID/start新鮮renderer、真正兩頁畫面才能分層驗收。

host的receipt目錄允許Android標準0771且GID屬同App，仍拒絕外部可寫、不同可寫GID、
非目錄及錯owner；不更改App目錄mode或SELinux。0.3.0在Virgo及Libra已有同APK兩頁
ON/OFF證據，均在正常重開遊戲後重新確認、啟用與還原；原掃描已解除本輪pause並有新上傳。
OS重啟、詳細頁及第二遊戲版本尚未完成。

0.3.0將主畫面確認綁定當次PID/start，離開App或使用一次即清除。
切回遊戲前再比對，native呼叫前又核對同tuple；不以新PID承接舊同意。
純Java的嚴格session parser已有27項測試；不回溯聲稱0.2.2有此行為。

`game_profiles.h`分開列出156.0、156.1與157.0的精確version/code、ELF大小／SHA、五組hook
入口／指令及layout編號；loader、bridge host及renderer共用同一選擇規則，狀態驗證也
核對所選版本。未審查版本不套用。以下早期狀態以[最新驗收](VALIDATION.md)為準；0.4.1已有156.x分項實測，新增157的0.5.0須另驗。
新增profile的來源工具、負例及證據界線見[精確設定檔流程](GAME_PROFILES.md)。

`app/` 包含 Android UI、root loader、JNI payload 及建置入口；
`native/renderer/` 是自控制中心抽取的花苗命名／縮字 renderer、純 policy、SHA 與 GC handle 支援；
`native/tests/` 保留既有命名／字型 policy 測試。

平台載入後端與遊戲精確 profile 是兩個獨立相容性層。
遊戲 version/code、ELF size/SHA、prologue、runtime metadata 必須全部對應。
編譯出某 ABI 不表示該裝置可載入；不放寬守門來宣稱多版本。

APK 內嵌兩個 loader build targets 及 ARM64 renderer。0.1.x 當時沒有 Magisk 模組、服務、
INTERNET 權限、開機自啟、掃描器控制或遊戲 RPC。
0.1.5 將 renderer 同時打包到 `lib/arm64-v8a/`，以 `extractNativeLibs=true`
由 Package Manager 安裝，從 `ApplicationInfo.nativeLibraryDir` 取得載入路徑。
不再從 `shell_data_file` 載入 renderer，也不修改 SELinux 或要求額外模組。
asset 副本只供內容 SHA 核對；loader 要求 canonical 套件路徑、檔案及祖先目錄
owner/mode 合法且非 symlink。仍保留精確遊戲與程序身分驗證。
控制檔 `/data/local/tmp/pikmin-seed-preview.txt` 與遊戲的 `seed_preview_status.tsv`
沿用原協定；只接受同遊戲 PID 五秒內回報，`waiting-cards` 不代表實際顯示成功。

Root loader 僅接收精確主遊戲程序，核 UID、start ticks、bootID、payload owner/mode、
ELF digest 及重複 renderer；native flock 加持久 attempt 防止不確定載入後重送。
不提供 upstream CLI 的任意程序、隱藏、memfd、watch 或 unload 選項。
OFF 由遊戲 UI thread 還原已綁定文字；不能 dlclose 尚有 live hooks 的 library。

## 工程沿革（以下為各階段歷史，不代表目前未整合）

目前仍未完成的是其他Android／ROM與第二遊戲版本
實機驗收。以下「未啟用／未交付」屬當時的0.1.x或研究階段，最新狀態以上方及
[驗收矩陣](VALIDATION.md)為準。

### 0.1.x安全工程紀錄

- NativeBridge 的真實ClassLoader namespace、JNI canary與單APK ZIP載入已有Virgo隔離證據；
  正式後端與renderer尚未接入，仍不得猜pointer或解除拒絕。
- 所有 remote syscall 等待需真正有界，證明中斷時 register/PC 可恢復。
- 0.1.6移除Java loader staging：兩ABI的PIE以`lib/<abi>/libseed_loader.so`交由PM抽出，
  App核對APK內SHA後直接執行；loader核對`/proc/self/exe`為canonical PM路徑及祖先owner/mode。
  舊暫存副本不能執行新版loader。已安裝0.1.6的ON/OFF尚未納入native交易；
  未交付候選來源已完成下述整合。UI AtomicBoolean仍只是操作提示，不作跨程序鎖。
- 禁止因新 PID 出現就把舊 attempt 算成功；未知結果與失敗證據保留。

本 repo 與原控制中心之後分開管理；此處不引入派遣、GPS、HC、育苗或更新模組系統。

`app/apk_payload.h`是尚未啟用的共用封裝檢查基礎：限定stored ZIP entry、唯一名稱、
16KB對齊及SHA，研究host先行使用。它不負責PM簽章／owner及root交易；不能單憑此工具
把任意APK路徑交給遊戲載入。研究APK不是使用者額外需要安裝的元件。

0.1.6的loader來源已接入`load_journal.h`：root-owned、單一inode/nonblocking flock，
以boot/PID/start與loader owner標記append-only階段，fsync檔案及新目錄項；正常結束只記
RETURNED_UNVERIFIED，異常／半筆／舊v1紀錄均不能重送同程序。0.1.6另以PM執行檔消除
Java先staging的覆寫競態；這不代表未知函式副作用已還原。測試僅使用自己的臨時目錄及子程序。

`--self-check`是明確的唯讀封裝檢查：只驗root及目前執行檔路徑，不找遊戲、不取得載入鎖、
不寫控制檔、不ptrace。x86_64自檢成功仍輸出backend未合格，正常載入入口仍拒絕。

`research/x64_call_context.h`是研究call-backend：獨立scratch stack、完整x86 XSTATE與GPR
保存／還原read-back、原stack red-zone不變；非預期stop即UNKNOWN並鎖住該context。
`bounded_function.inc`僅允許`dev.ody.seedlensprobe`，透過有界syscall配置兩側guard page
的1MiB stack；保留原linker caller的NX mapping。UNKNOWN立即退出研究worker，由既有
EXITKILL／pidfd watchdog收容**測試程序**，不可作為正式遊戲恢復策略。
研究APK0.0.5提供30秒專用thread；loader核對TID/TGID/start並與process memory身份分離。
實際APK chain已通過，但不代表遊戲本身具有同樣安全注入點或跨ROM XSTATE一致性。

## NativeBridge候選整合（未啟用）

`app/bridge_platform.h`共用adapter只讀已映射ART／native loader／bridge的匯出API，
經真正Application及ClassLoader取得bridged namespace；明確套件及PM版本不符則拒絕。
JNI local frame與自行attach的thread由RAII釋放，不猜ART內部結構，不force-load平台SO。
`research/canary/host.cpp`使用同一adapter，已驗正例、錯誤套件及name/code負例。

`app/bridge_host.cpp`為x64候選host，僅在精確遊戲主程序／UID啟動worker，重核PM版本、
現有game ELF SHA、自己的PM安裝路徑與owner/mode，再核同APK內ARM renderer的ZIP／SHA。
透過translated JNI_OnLoad進入ARM renderer，append-only私人receipt最多記
`ARM64_ENTRY_RETURNED_NOT_READY`；真正ready仍由renderer同PID新鮮狀態與畫面證明。
明確傳入`SEED_BRIDGE_RENDERER`才建置此target，檢查ELF64/AARCH64/shared與大小後編入SHA。
候選建置現已打包，但未部署，x86 loader/UI拒絕仍保留；正式game thread/call入口仍待整合，
控制owner已接入候選來源，見下文。

0.1.6既有`assets/libseed_display.so`是壓縮entry，不能直接作為NativeBridge mmap來源；
手機使用PM抽出的native副本不受此問題影響。未來建置用aapt2 `-0 so`，並在簽名前後由
host Java `ApkNativeLayout`檢查stored／16KB／唯一entry／central-local一致、必要ABI、
loader內嵌SHA與兩個renderer副本一致。這不改已交付0.1.6，也不新增終端使用者依賴。

## 候選控制交易（尚未交付）

Java現在只核對PM loader及payload SHA、目前PID/start，再exec固定native入口；
不再mkdir/chmod交易目錄、寫共享`control.new`或在鎖外開關顯示。
`--enable PID START PAYLOAD`取得既有LoadLease後，OFF、必要載入、檢查完整持久載入紀錄，
有界等候同PID/start的renderer，再ON；整段鎖不釋放。`--off PID START`使用同鎖，但不要求
載入成功，也不重新載入。`--status`只讀、不取得鎖、不改控制檔。x64啟用仍在寫控制前拒絕。

`control_transaction.h`以getrandom產生唯一暫存檔與audit名稱，先fsync INTENT、檔案與目錄，
核驗既有目標為root-owned regular單link／安全mode及inode未變，再atomic rename並fsync。
不使用共享temp、不刪舊未知receipt；COMMITTED只表示控制檔已寫，不是UI效果驗收。
載入階段未知／缺欄／不完整／跨UID或PID/start時，新ON被拒絕且保留原件。正常OFF仍可要求。

`seed_preview_status.tsv`保留原v1前10欄，末尾附第11欄`/proc/self/stat` start ticks。
舊APK的awk讀取不受追加欄影響；新啟用需要完整11欄、同程序、5秒內狀態及ready/waiting-cards。
舊renderer不夠證據時保持OFF並要求正常重開遊戲，不把舊ready當新版已載入。
未知／載入失敗不自動重送；此處沒有解決upstream ARM remote call的全部timeout問題，
也沒有完成NativeBridge正式entry策略。只有未交付來源與隔離測試，不能替代後續APK+UI驗收。

### 候選單APK配對及實際game entry觀測

`build.ps1`固定先建ARM，複製本批renderer後再將該不可變副本傳給x64 CMake；host的編譯SHA
與配對receipt來自同一個輸入。APK layout checker要求x64 host、host SHA及renderer SHA全部
對應，且所有SO stored／16KB。不再使用上次CMake cache裡的0.1.6 renderer pin。
`-Unsigned -Candidate`只建立獨立研究產物，不能安裝或交付，已交付APK不覆蓋。

`entry_context.h`候選元件只做PTRACE_SEIZE/INTERRUPT、讀取暫存器及DETACH，不寫記憶體／
暫存器、不single-step或跳過syscall，也不設EXITKILL。TID/TGID/start逐次核對；只把自己的
interrupt-stop當成可消耗事件，其餘訊號於detach保留。這不是完整遠端呼叫／未知結果復原。
`research/game_entry_probe.cpp`在明確維護pause、pending空、精確game主程序下取樣；
NativeBridge多個thread繼承UnityMain名稱，故名稱不能當唯一身份。wchan=0只作候選提示，
實際user-context仍需停止後GETREGSET證明orig_rax=-1。正式loader尚未使用該元件。
