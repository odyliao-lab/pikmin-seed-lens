# 架構

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

`game_profiles.h`分開列出156.0與156.1的精確version/code、ELF大小／SHA、五組hook
入口／指令及layout編號；loader、bridge host及renderer共用同一選擇規則，狀態驗證也
核對所選版本。未審查版本不套用。156.0已完成離線來源比對，尚非實機驗收。
新增profile的來源工具、負例及證據界線見[精確設定檔流程](GAME_PROFILES.md)。

`app/` 包含 Android UI、root loader、JNI payload 及建置入口；
`native/renderer/` 是自控制中心抽取的花苗命名／縮字 renderer、純 policy、SHA 與 GC handle 支援；
`native/tests/` 保留既有命名／字型 policy 測試。

平台載入後端與遊戲精確 profile 是兩個獨立相容性層。
遊戲 version/code、ELF size/SHA、prologue、runtime metadata 必須全部對應。
編譯出某 ABI 不表示該裝置可載入；不放寬守門來宣稱多版本。

APK 內嵌兩個 loader build targets 及 ARM64 renderer。沒有 Magisk 模組、服務、
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
