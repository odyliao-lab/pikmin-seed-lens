# APK＋Root 跨平台相容研究

> 最新進展：0.3.1已在Libra與Virgo完成正常App入口、真實花苗／探險頁ON/OFF及新遊戲程序再啟用；
> 詳見[最新驗收矩陣](VALIDATION.md)。下文「未修改／未交付／x64關閉」均為各歷史研究階段，
> 不再代表目前狀態。第二遊戲版本實際效果、Android12/API31及完整OS重啟矩陣仍未完成。
> 2026-11-09來源checkpoint與接手範圍見[竹北交接](handoffs/2026-11-09-zhubei-takeover.md)。

2026-11-08；目標維持一個自足 APK＋使用者既有 Root，不要求控制中心、LSPosed、
額外 Magisk 顯示模組或每次使用都連接電腦。這是研究設計與隔離證據，**不是已完成多平台相容**。
本輪新增 research 工具與文件及自有測試 APK；未修改已安裝的 Lens／遊戲 APK、
未升產品版本、未 commit/push／發布。

## 現況與定位

| 層 | 已知情況 | 改善路線 |
|---|---|---|
| ARM64／Android | Libra API34＋156.1，0.1.5 的真實兩頁與 OFF 已驗 | 同一 APK 依 ABI／能力選後端；逐 Android／Root provider 實測，非僅檢查 SDK 範圍 |
| x86_64／NativeBridge | Virgo API35 的遊戲是轉譯 ARM64；目前 backend 關閉 | 從實際 ClassLoader 取得 bridged namespace，先以無遊戲邏輯的 canary 驗證 |
| 遊戲版本 | 現在 Java、ELF SHA／大小、RVA／prologue、metadata 都只配156.1 | 統一多 profile 清單，逐版加入審核過的 digest／ABI／方法／欄位契約；未知版本不掛接 |
| 顯示／還原 | 花苗與探險文字 hook、縮字、OFF 是獨立驗收 | 每個已支援組合驗列表、詳細頁、重用、OFF、重啟及長時間運行 |

### 已定位的兩個問題

1. pinned AndKittyInjector／KittyMemoryEx 的 `_callSyscall` 使用
   `wait(&status, __WALL)`，未傳入 remote-call timeout。`wait` 的預設0代表阻塞，
   因此上層設定5000ms並不會限制這個等待。僅加 host timeout 或殺 tracer 不是安全修復。
2. 現行 NativeBridge 分支只查 `default`、`classloader-namespace`、
   `classloader-namespace-shared` 三個 exported 名稱；upstream 找不到時會使用
   `0x1`，本產品目前拒絕這個 fallback。先前只能證明「這些查詢未取得 namespace」，
   **不能推論整個 Virgo 沒有可用的 classloader namespace**。

## 原始來源與候選方案

[AOSP NativeBridge 介面](https://android.googlesource.com/platform/art/+/c8d6e3b523f31baa548934f822c6e36adc2a52e8/libnativebridge/include/nativebridge/native_bridge.h)
區分 exported lookup、namespace 建立、指定 namespace 載入及 trampoline。
公開名稱查不到不代表能隨意猜地址；也不把 vendor namespace 當成遊戲 namespace。

[AOSP native loader](https://android.googlesource.com/platform/art/+/refs/heads/main/libnativeloader/native_loader.cpp)
提供 `FindNativeLoaderNamespaceByClassLoader`、`IsNamespaceNativeBridged`、
`OpenNativeLibraryInNamespace`；這是 platform/native 介面，不是一般第三方 APK 的穩定 SDK。
[namespace 實作](https://android.googlesource.com/platform/art/+/refs/heads/main/libnativeloader/native_loader_namespace.cpp)
另有 system/default 的處理與 bridged load。這些來源指出候選方向，不代表 MuMu ROM 行為已驗。

優先研究流程：APK 內嵌 x86_64 小型入口及 ARM64 display payload；在**自有測試 App**
取得正確 JVM／JNI thread 與其 ClassLoader，再從相符 namespace 載入無副作用 ARM64 canary。
要確認 bridged 類型、library handle、trampoline／JNI 生命週期、同PID結果與重啟行為。
通過後才移到單一授權遊戲短測，不使用 root shell 的 namespace 冒充遊戲 ClassLoader。
不能以注入任意 Java 物件、猜 ART 結構／opaque pointer 或關掉 SELinux 代替。

候選仍可能因隱藏 API、匯出符號、SELinux、linker namespace、轉譯器實作或 JNI ABI 差異失敗。
如果目標 ROM 不具備安全入口，就保持原顯示並明示不支援；不以額外模組偷偷改掉 APK＋Root 需求。

## 本輪實證：不操作遊戲的隔離測試

- 已新鮮核對 Virgo instance3／API35／x86_64／libnb.so 與正式 Agent 身分。
- `research/trace_probe.cpp` 僅 trace 自己 `fork` 的子程序，不接受外部PID／payload／syscall參數。
  正常案例固定 `getpid`；逾時案例固定 `pause`。採 monotonic deadline＋WNOHANG，
  超時後先 `PTRACE_INTERRUPT` 並確認 stopped，再還原暫存器、detach、驗 heartbeat 前進及正常退出。
  這不是載入器，也不寫遊戲、控制檔、掃描排程或 NativeBridge 狀態。
- NDK r27d、x86_64／minAPI28、`-Wall -Wextra -Werror` 編譯通過。
  產物 SHA256 `b40d69fde5809496c0a922c325f9edfecdd9089bbe21e6d20d22b6b8a4fb7b88`。
- 在 Virgo API35 實際執行5輪正常＋5輪逾時，10項通過；正常3–7ms，
  設定100ms的逾時測試含停止／還原總耗時103–400ms。暫存器逐byte比對一致、
  子程序恢復heartbeat並exit0、已回收。這不驗證任意函式呼叫的stack/FP/TLS/lock副作用，
  不代表 upstream 已修好，也不代表可在正式遊戲安全取消任何載入。
- 遊戲PID25980前後TracerPid均0；未暫停掃描、未安裝APK、未注入或重啟遊戲。

### Virgo 實際 platform library 查核

唯讀取得當前映射的三個 library，下載後核對SHA；原件留在私有忽略目錄，不進Git。
`research/inspect-native-loader.ps1` 對 ELF dynamic symbols 查核：

- `libnativeloader.so`：上述三個 namespace／load 函式確實匯出。
- `libnativebridge.so`：Initialized、GetVersion、GetExportedNamespace、CreateNamespace、
  LoadLibraryExt、GetTrampoline2 確實匯出。
- `libnb.so`：NativeBridgeItf 確實匯出。

僅證明符號存在，未呼叫，未證明從 App 可用或可載入 display payload。這使 ClassLoader 方案
值得進入隔離 App 實驗，但仍不足以解除 production x86_64 blocker。

## 實作／驗收順序

1. 自有 bridged 測試 App＋ARM64 canary：完成正常 JNI 載入、真實 namespace、錯誤／重啟測試。
   測試 APK 只供研發；正式使用者仍只有一個 Lens APK，不要求安裝測試套件。
2. 將「有界停止／還原」整合所有 attach、syscall、remote call 路徑；涵蓋 exit、signal、
   child threads、timeout與restore失敗。deadline不能只套在最外層。任意dlopen超時可能已產生
   side effects，保留UNKNOWN attempt，不能只還原暫存器就宣稱可重試。
3. 收斂 privileged staging／lock／owner／crash journal；保持原簽章與不可變交付身分。
4. 以確定ABI、PID/start/UID、遊戲digest/profile的單次正常入口接入 payload，
   再驗 Libra/Virgo 探險與花苗頁、OFF、卡片重用及掃描恢復；不擴大到其他裝置。
5. 把156.1的硬編碼匯成單一版本profile來源，生成Java/native檢查，增加錯版／錯digest／
   缺欄位／錯方法負例；每個新增遊戲版另審完整profile並驗畫面。

研究工具不升APK版。完整雙平台能力交付時預計升 minor（至少0.2.0，實際code依當時已交付最大值），
不得覆寫0.1.5。跨遊戲支援意指「同一APK可包含多個已驗profile」，不是保證所有未來版本自動可用。

## 2026-11-08 18:13–18:29：ClassLoader 路徑的隔離實證

持續目標已建立：單一 APK＋既有 Root，至少兩個 Android 與兩個遊戲版本的真實兩頁、
OFF 與重啟驗證。尚未達標，不宣稱通用相容。

新增 `research/canary/`，自有包名 `dev.ody.seedlensprobe`。只有背景 instrumentation，
無 Activity／網路／遊戲操作，不需要暫停掃描。測試 APK0.0.2/code2 的 SHA256：
`2f79940b47721fb783415bc76a74343f1f582123a956b9a6f58c0b76de56bc63`；裝置安裝檔一致。
首個測試包漏帶 C++ shared runtime 的失敗保留；已改為 static linkage 並加入建置檢查。

Virgo API35／x86_64／libnb.so 的既有 SELinux 為 Permissive，未變更。ARM64 fixture
由正常 Java 載入，4項回傳值比對通過。研究 loader 只接受測試包的精確 cmdline、UID、
start ticks，固定 host payload，並由 pidfd/watchdog/EXITKILL 限制不確定操作到可丟棄的
測試程序；**這不是正式遊戲 timeout 復原方案**。7項 self-target名稱測試在 Virgo 通過。

初次 host 一般 dlopen/NOLOAD 查不到 JVM；改用既有 xDL 解析已映射 platform libraries
的真實匯出符號，不 force-load、不修改 namespace、不猜 JVM/ART 物件。
成功在測試 PID2995 取得真實 Application/ClassLoader、bridged namespace、ARM64 handle，
由 `NativeBridgeGetTrampoline2` 呼叫4項常數測試均正確；其後 Java heartbeat前進並正常FINISH。

新程序 PID4368 再次通過，並新增缺失 library／缺失 symbol 均正常拒絕的負例；
same-PID receipt記錄 `REAL_BRIDGED_NAMESPACE`、`MISSING_LIBRARY_REJECTED`、
`MISSING_SYMBOL_REJECTED`、`CANARY_PASS_4`，TracerPid=0，Java heartbeat繼續。
首個成功host SHA為 `d9268139daef7ee2d1fcef7018c22644bdfe4aeafe3748d4d3d31a4e48cf8b9b`；
含負例host SHA為 `f931c11a4ab4e7c68760dedc817892f94735ef67a5b0a5f978550988e092b08b`。
舊host均核SHA／owner／mode後另存，沒有覆寫原證據。固定3秒等待曾不足，已改為等待fixture條件。

這推翻「只有三個exported namespace名稱可選」的實作假設，證明這台ROM有可用的真實
ClassLoader路徑。沒有注入遊戲／改遊戲設定／重啟模擬器，遊戲PID37527與TracerPid=0保持；
沒有建立掃描pause。仍需正式載入器完整deadline／staging owner／UNKNOWN安全流程、
renderer生命週期與實際遊戲兩頁驗收、多遊戲profile及其他Android／ROM實測。
目前正式x86_64 blocker保留；隔離canary通過不是Virgo花苗顯示通過。

18:30後補查：兩個成功測試PID均已正常FINISH，沒有遺留測試程序或掃描pause。
私人證據快照 `virgo-canary-evidence-1794133857.txt` 的 SHA256 為
`9f9f26de7ea7b54b4e656ea9f1ca5453df27a63915a90a173fe5df47c08f3e85`。
隨後遊戲PID由37527變為5119；唯讀日誌顯示 scanner 的 direct refresh timeout／
no new rows 觸發既有 cold restart fallback。不是本輪發出的遊戲重啟指令；也不因此宣稱
已排除所有測試資源影響。正式站GET顯示兩台 enabled且未paused，仍持續檢查新接受資料。

下一次接續時先重新核對目標與雜湊，不照抄本文PID；先補嚴格重啟後同產物重複測試、
JNI_OnLoad與回呼生命週期，再重構正式loader。現在的測試watchdog會終止自有測試App，
絕不能把這個行為直接移入正式遊戲。另需修正現行UI「無公開命名空間」的過度概括訊息，
但不得藉此解除production blocker。遊戲profile目前仍只有156.1，其他版尚待精確提取與驗證。

18:34–18:35補驗：相同code2 APK與相同f931c11a host在全新PID6281再次通過4項回傳值、
缺library／symbol負例，Java持續到90秒後正常FINISH。私有第二快照SHA256
`f8cb11d43018217bef0a5cda780b34b54191ebc791e2aee1ded62af9b7c6ef6e`。
新增僅自有測試程序可用的`--timeout-test`，PID7512在確認attach後刻意等待；3秒watchdog
收束該測試程序，確認pidfd exit及tracer已回收，回報`SELF_PROBE_CONTAINMENT_PASS`。
遊戲5119前後相同且TracerPid=0。這是測試程序的終止隔離，不是遊戲狀態無損還原。
新runner SHA256 `47c292034136dcdf51615adc63f7f96144f09ed03f52a784a4e2975be6df10cd`。

正式站再次唯讀核對：Virgo於epoch1794134091123/1794134091287有新的資料／上傳接受，
target從2306727推進至2306724；Libra同樣有新接受紀錄。兩台enabled=true、paused=false。
測試未建立／刪除任何掃描pause，未改排程或Hunter；最終無測試程序。目標保持進行中。

## 2026-11-08 18:37後：JNI生命週期與單一APK封裝

研究APK code3（SHA256 `98873a825bcedf414d81c9a7f75b0abb675cab95b75ed38e9e54450f64f4e0e3`）
於新自有程序通過真實namespace、4項trampoline、缺library/symbol負例，並新增：
錯誤JNI入口key回傳JNI_ERR且未建立worker；正確入口重複呼叫只有一個ARM64 worker；
該執行緒JVM attach/GetVersion/detach完成。程序其後正常FINISH，沒有遺留tracer。
host SHA256 `450af0c1afa3f4d945ab3992f881a6bb35f8cc88e248c1e12d3009a0863a9927`。

code4不依賴Package Manager解出兩種ISA的原生檔：只有fixture在nativeLibraryDir，
canary只存在同一APK的`assets/bridge/libcanary.so`，未額外解出／另安裝依賴套件。
從實際ApplicationInfo.sourceDir取得APK，透過同一真實namespace載入APK ZIP entry。
API35 Virgo新程序11526通過上述全部canary/lifecycle項目，Java持續到正常FINISH。
研究APK SHA256 `246bcd9e8c2507ceb01afbfaba1e7892b930ddbce4c044dbbba3877a24408453`；
ARM64 entry SHA256 `5c5cc80f86510cfd21573a8e5ac2cfb013d45c57748ea8f691165745c9783f48`；
host SHA256 `5e0540c215782d67c3594543bd763d4d5a4cc1b61252adf4929c563a06ae00b9`。
私人快照`virgo-canary-evidence-1794134862.txt` SHA256
`cc4467faade151302c3795bae9b52d0e52047623e4dc9a29459f91a4674d0b27`。

新增可共用但尚未接入產品的`app/apk_payload.h`：有界ZIP32目錄掃描、選定檔名唯一、
stored且無加密／descriptor、local/central名稱及長度一致、16KB對齊、32MiB上限、
SHA256、同FD檔案metadata與讀取deadline。不解壓、不使用ZIP64／多磁碟回退。
第一次實際APK測試發現無關resources entry含zipalign舊式原始padding；改為只嚴格解析
目標payload的extra欄位，其他entry仍檢查目錄邊界／非ZIP64尺寸。失敗未用於載入。
Virgo原生測試：2項正例、28項拒絕案例、2項SHA標準向量，以及實際APK正確SHA接受／
錯誤SHA拒絕均通過。這不是APK簽章或owner驗證的替代品。

所有變動仍限隔離測試；未解除正式x86_64 blocker。正式renderer的hook、OFF與畫面仍未驗。
期間遊戲由既有scanner再次更換PID（18:47讀到12414），沒有由本研究發出重啟；當前TracerPid=0。
後續站台GET仍確認Libra/Virgo enabled、未paused且有新資料／上傳接受；沒有建立pause。
不能憑未主動重啟就斷言測試完全沒有資源影響。第二遊戲版profile及完整loader安全整合仍待完成。

18:53–18:55：加入上述SHA gate的host（SHA256
`841367944b5c20fd61c1090ea92e385595f2107747eb5702cfa6a6417acd9455`）在新自有程序15639
記錄`APK_PAYLOAD_SHA_VERIFIED`，通過全部namespace/canary/lifecycle項目，Java正常FINISH；
無遺留測試程序，遊戲12414無tracer。私有快照`virgo-canary-evidence-1794135279.txt`
SHA256 `77a0d05ff33734e14bde8230555854c2643f94a7f9365228972d7f336418af58`。

期間發現scanner有direct refresh timeout／cold restart cooldown／零筆結果，暫停新增裝置
實驗並唯讀追蹤，不將enabled或零筆當成功。後續正式站Virgo的新接受資料時間為
1794135218251、上傳1794135212707，target前進至2304825；Libra亦有新接受資料。
這只確認掃描重新出現接受進展，不保證持續穩定或排除資源競爭；未改scanner與排程。
下一階段先看當時新鮮業務進展，再進正式loader有界操作／持久owner整合。

## 2026-11-08 18:58–19:10：實際loader journal與syscall overlay

`app/load_journal.h`已接入現有loader的ARM64來源流程，不僅是獨立示意工具：
取得root-owned單一inode flock後，以boot/game PID/start/UID及loader PID/start建立
O_EXCL、append-only、fsync紀錄，再依序寫PREPARED、ATTACHING、ATTACHED、CALLING、
RETURNED、DETACHED、RETURNED_UNVERIFIED。早退留下UNKNOWN；crash半筆仍阻擋同程序重試。
舊`attempted`原件同樣拒絕重送，不被覆寫或升格成功。沒有新增清鎖／清紀錄捷徑。

兩ABI loader編譯通過。原生測試僅自建`seed-lens-journal-test-*`fixture，無遊戲讀寫或
ptrace；測試fixture保留，不碰正式`pcc-seed-root-v1`。Libra API34／SELinux Enforcing與
Virgo API35各85項檢查通過，含六個中途crash點、live foreign owner競爭、owner死亡後
舊attempt仍阻擋、symlink/hardlink/FIFO/弱權限拒絕及不改原件。

- ARM64測試SHA256 `307b0e0419829b26d97a7e05eee983bf3b6a1c57910e8abad88849564b31bbfe`；
  私人receipt `libra-journal-test-1794135688.txt` SHA256
  `a1e070ffebd1f03a6f24bfacce9e02b4a322e5ba46e105410e7ed39e981901cd`。
- x86_64測試SHA256 `786db775663cd2b0ac250061746199e9cfdec1350975ef5a0f6a1fe411adcb6e`；
  `journal-test-1794135732.txt` SHA256
  `d9ce9116b2fd30c37f5dcb912523028c15294170e94e48b082e4e5249b3f588c`。

進一步以repo-owned `research/bounded-trace.cmake`改寫建置目錄的upstream副本，pinned
submodule未修改。精確anchor缺失／重複即拒絕建置。不是把一次host timeout包在原呼叫外面：
wait改用monotonic deadline，EINTR共用預算；stop限SEIZE且等待有界；syscall entry/exit/
訊號共用deadline；還原前證明stopped，檢查code/GPR還原結果。不使用猜測namespace。

測試使用真正KittyTrace overlay而非另寫的trace示意實作。首個tracee訊號案例發現
Linux內部restart值`-514`被upstream當作SUCCESS；不是延長timeout或放寬測試，已修成
明確失敗、不重播syscall，仍檢查GPR還原／child持續與回收。分開測tracee訊號中斷與
tracer訊號干擾等待。Virgo五類各三輪共15項通過：getpid、pause逾時、tracee訊號、
tracee退出、tracer訊號。無訊號逾時約100–199ms；含訊號producer join的全案例總時間
235–470ms。每個case有1500ms界線、子程序alarm與父程序alarm，僅pidfd指向自建child。

測試binary SHA256 `129a02391dfb385c0f3118e4b9f376f99288da84208ea7712f02d053ca734745`；
`kitty-test-1794136093.txt` SHA256
`d72d32ec6ad4bae091d9a30761b944c74cfd0e825831185ce0b429dc8c02dd07`。

**範圍限制**：production尚未使用此trace overlay；一般function-call、attach/step及
breakpoint舊路徑仍需整體處理，FP/stack/TLS與dlopen鎖／side effects未證明可取消。
不可把測試App的EXITKILL策略套到遊戲。Java privileged staging仍在native lock之前，
journal不代表這個競態已解決。新source未交付APK，原0.1.5/0.1.4不可覆蓋。
修改UI拒絕文字後19項JVM admission案例通過；仍拒絕NativeBridge與未知遊戲版。

19:09站台唯讀GET：Libra、Virgo enabled、未paused，兩者相較上輪均有新資料／上傳接受；
Virgo新接受1794136122533/1794136122693，Libra1794135891227/1794135885477。
未建立pause或改scanner，遊戲程序無tracer。這不等於全天穩定或完整多版本驗收。
目標active，未commit/push／部署／發布；下一步是完成正式函式呼叫安全整合與PM封裝入口，
再進實際Virgo兩頁/OFF/重啟及第二遊戲版profile驗收。

## 2026-11-08 19:11–19:27：獨立stack／XSTATE與同程序thread載入

目標是排除原upstream function-call覆寫原stack、只還原GPR的風險，不放寬版本或
NativeBridge admission。新`x64_call_context.h`僅研究：x86_64、最多6個integer/pointer
參數、user-space原始context，完整GET/SET NT_X86_XSTATE與GPR read-back；保留原
128-byte red-zone，call使用guarded獨立stack。共享monotonic預算，不吞掉或重播未知
訊號。正常返回需expected synchronous fault／PC／SP匹配；錯誤或還原失敗鎖住context。
CPU還原不代表TLS、heap、constructor、dlopen鎖或其他執行緒副作用回滾。

Linux原始實作參考：
[x86 FPU regset](https://raw.githubusercontent.com/torvalds/linux/master/arch/x86/kernel/fpu/regset.c)。
測試讀出kernel實際長度，限制小於64KiB，不能以固定512-byte浮點區聲稱完整XSTATE。
目前只有Virgo API35這個kernel的裝置證據，沒有延伸承諾所有Android／CPU支援。

`x64_call_context_test`只fork自己的child且無外部target參數：正常函式主動使用8KiB local
stack並改XMM/x87、spin逾時、UD2 fault、SIGUSR1打斷、SYS_exit，五類各三輪通過。
正常回傳後再呼叫一次、detach後heartbeat前進／正常reap；UNKNOWN案例不恢復業務，
直接處置測試child並確認context禁止重試。最後一次逾時case觀測101–402ms，全部小於
case的1000ms上限；Android排程不是硬即時，不能把100ms設定說成精確wall-clock保證。
receipt `x64-context-1794137259.txt` SHA256
`610d16c0e8144a1631223943864687dccc1ea5deb0ab11d97d95b7f99c171a69`。
syscall15項重新通過；最新receipt SHA256
`9d0b4ad668f0b8c3a5f1f48d393611466184642f0fa5fef4b92b90743565968b`。

將function replacement接入**研究**KittyTrace生成副本（不改pinned submodule／production）。
每次呼叫用有界syscall配置PROT_NONE兩側guard+1MiB RW stack，保留原linker NX caller，
僅成功還原後munmap；UNKNOWN直接退出研究worker，禁止injector繼續dlerror/cleanup。
這依賴研究probe EXITKILL/pidfd收容，尚不能部署於遊戲。

真APK第一次因Android argv[0]後多個NUL被拒絕，修成只允許精確名稱＋末端NUL，仍拒絕
colon child／額外參數。第二次因主執行緒停在kernel syscall拒絕；沒有偽造orig_rax或
解除檢查。改讓研究APK0.0.5/code5建立30秒限定`SeedLensProbe`thread，memory仍以TGID
識別，trace改選同TGID且名稱、start匹配的TID。正式遊戲沒有此測試thread，後續仍要解決
安全選取／停止位置及未知副作用的正式策略。

- 研究APK SHA256 `843c1350c1b119ff027010e2a0fdcb8845884a45db6a122c1236bb5e14476564`，
  `.tools/research/canary-20261108-192413/probe.apk`；原簽章
  `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`，install-r不清資料。
  ARM canary SHA仍為`5c5cc80f86510cfd21573a8e5ac2cfb013d45c57748ea8f691165745c9783f48`。
- 研究root loader SHA256 `11bba24b0588af2200f042aa9b126f68e42324920bc38e9e62ce5286a3e85f1b`，
  寫入新的SHA命名檔，原r3及失敗候選完整保留，未覆蓋已交付Lens APK。
- 真實probe PID28908/TID28929完成host load、CPU還原、真bridged namespace、APK SHA gate、
  四值canary、缺失library/symbol拒絕、JNI wrong-key與單workerAttach/Detach生命週期。
  loader receipt `canary-context-1794137120.txt` SHA256
  `1ea8164df86851eeccff091a473bc79d1c99342d0eec3ea60543380c876f34ef`。
- 19:27新鮮完整snapshot `virgo-canary-evidence-1794137279.txt` SHA256
  `30145c9084efb3b22d4f6ec736c6840954f813afc67bf57e24938db43cd552af`，
  CALL_THREAD_FINISHED/FINISH PASS，probe結束，game PID25272/TracerPid0，無scan pause。
- 失敗receipt `canary-context-1794136895.txt`／`1794136930.txt`保留原拒絕證據，不算成功；
  最初ADB無權直接push root目錄未啟動測試，改為唯一暫存、SHA核對後root複製／權限核驗。

19:25站台GET：Libra1794137155151/1794137149523、Virgo1794137084898/1794137085080
為新資料／上傳接受時間，enabled且未paused；未控制scanner。未commit/push／發布產品，
正式x86_64 refusal仍在。第二遊戲版、正式UI兩頁/OFF/重啟及跨ROM backend未完成。

## 2026-11-08 19:37–19:40：PM原生執行檔取代root staging

0.1.6/code7是安全patch，仍只接受原156.1設定，不解除x86_64。兩ABI loader放到
`lib/<abi>/libseed_loader.so`，PM負責抽出；不是System.loadLibrary載入PIE。
Java讀內嵌SHA核對後直接exec，移除asset解壓／cp／mv mutable loader；native要求
`/proc/self/exe`與當前ABI、確切套件、canonical PM路徑及所有祖先owner/mode匹配。
每次load使用mktemp私有日誌，不能覆蓋原`load.log`的UNKNOWN證據。沒有清除舊loader／紀錄。

本機19+27 JVM與雙ABI編譯通過；兩ABI的identity/path native檢查已在各裝置實際通過。
signed APK `.tools/build/0.1.6/signed/pikmin-seed-lens-0.1.6-signed.apk` SHA256
`d13a20ca9ac450acf5f98978f167a88f544be9afafc154df94735740638de350`，原憑證9080c5d8…934a7e。
loader SHA：ARM64 `9b74cb8c9e53377fb939d75e64d49d61e3d18f18b15af6664bcc629deb4a44cc`，
x64 `ddc89c8aef63af63c9600bec79498b5514e0b7847db25de1dad04269a7142770`。
ZIP內loader stored／digest等值及簽章後16KB對齊已核對；無舊loader asset。

兩台先新鮮核對遊戲、ABI/API、agent／實體或instance身份、OFF及無tracer，再保留root-only
backup。備份目錄700、檔案600、root:root；原APK與備份SHA相等，App-data tar也留SHA。
Libra原APK0.1.5/code6、Virgo0.1.4/code5；install-r未清資料、不用ABI override。
PM自動選對架構，新APK回讀SHA相同。遊戲PID18143／25272前後不變；不修改scanner。

私人裝置備份：Libra `/data/local/tmp/seed-lens-pm-backup-1794137862`；
Virgo `/data/local/tmp/seed-lens-pm-backup-1794137880`。只保全並核驗檔案，不宣稱舊版本
可直接降版恢復（若需回退仍須更高code的相容回退包，不卸載／清資料）。

| 私人receipt（`.tools/research/`） | SHA256 |
|---|---|
| pm-install-Libra-1794137862.txt | c91a861d9d2cfead86abe855dabe39ce24ba7fda605e86ef1464ae2731f823c4 |
| pm-install-Virgo-1794137880.txt | 52bdad45718928cbec3d47a3a826ed1a89e9615d0639d6431c03b2753ecb4e03 |
| pm-exec-Libra-1794137872.txt | b5ce4fd806de1708cdcf1bc628148764b7d2b98a6f7e972f18abd3319d626781 |
| pm-exec-Virgo-1794137935.txt | efd72e0b121fc304ef9ff50c04977c1c1c2d0b8157ee37c0321f24cf040c70e4 |
| pm-policy-Libra-1794137990.txt | 12ffeee324f57bf0ebd4b14bf39d2aa556b5d86b3bd373c3bcf3f9ab34f2715e |
| pm-policy-Virgo-1794137999.txt | 257a1112022d7d2af38263041a28e7e15734cb467f460c263659c3629d0925f9 |

PM抽出檔案owner1000:1000/mode755；兩台root `--self-check`成功，同位元組暫存副本被
stage23拒絕。自檢不查遊戲、不建立attempt／控制檔、不ptrace；x64仍輸出未qualify。
19:39站台新資料/上傳接受：Libra1794137935547/1794137929734，Virgo1794137934878/
1794137935040；enabled、未paused。這是新進展，不是全天穩定性證明。

仍需正式thread/call策略、PM host＋renderer/NativeBridge整合、ON/OFF控制owner交易，
以及0.1.6起新流程的實際兩頁顯示/OFF/重啟及第二遊戲版本證據。原0.1.5 Libra效果證據
不自動移轉成0.1.6驗收。目標仍active，未commit/push／公開發布。

## 2026-11-08 19:50–20:00：共用adapter及正式候選host

正式候選host `app/bridge_host.cpp`（未打包／未啟用）SHA256
`970a2f8dcb1afb89452757351ad2bbc1ff8e72dc75883f9c7e98768697a5987d`，編入既有ARM renderer
`2de0117c0802483a65cc761ce7bc7c65f8d0b27865f376954fa68043b126c61a`。
共用adapter只用已映射平台export，核實Application套件與PM版本、真bridged ClassLoader
namespace；候選host另做遊戲ELF、UID、PM路徑／owner及同APK payload hash，append-only階段。
沒有將其載入遊戲，也沒有解除正式x64 gate。CMake要求explicit ARM64 renderer檔案。

隔離host第一版SHA `305e88831fe08bc9315b18efd6248449d3f703ce928b6a5c96e7cadd568deed6`，
PID40256正／負套件版本、APK完整性、缺失library/symbol、四值canary及單worker全部通過，
最後FINISH PASS。第二版加入跨APK控制，SHA
`7f1fcb3ea346c53e1fa8c5cb5aeb55631ec102977018a8654e633787a834351b`；PID44342能從已安裝
Lens0.1.6 APK的stored `lib/arm64-v8a/libseed_display.so`取得translated JNI入口，僅以錯誤key
證明拒絕初始化，沒有呼叫renderer start／寫遊戲狀態。原canary正確key僅用於測試library。
這明確區分跨APK／ABI可達性與遊戲掛接，不額外安裝新的fixture APK或平台模組。

AOSP `library_namespaces.cpp` 的permitted-directory規則提供研究方向，但ROM實測才是證據：
[AOSP來源](https://android.googlesource.com/platform/art/+/refs/heads/main/libnativeloader/library_namespaces.cpp)。
不能由此推論所有ROM、SELinux或遊戲ClassLoader都相同。

0.1.6現有ARM asset是deflated；新build用`-0 so`，簽名前後執行host Java layout／SHA驗證。
8個fixture通過，舊APK如預期拒絕；只用既有位元組生成NOT-FOR-INSTALL的未簽名布局fixture，
SHA `8ff19c0620ab4a8369aa17509500a2b8cc977d779d09d569016fd6f5696bd387`，stored/16KB/必要
entry及loader/renderer SHA一致通過。未覆蓋、簽署或安裝不同位元組的0.1.6。

私人receipt（原件在ignored `.tools/research/`，不進Git）：

| Receipt | SHA256 |
|---|---|
| bridge-adapter-1794138617.txt | a2b82a81e712be5ca8ba04fa3914da2e48c31deb0edadfba6c9827dd83c8a108 |
| virgo-canary-evidence-1794138750.txt | 953ab790ec9e670ca2b815877b205db509dc9a7e8db67a00075e55b01946e36f |
| pm-policy-Libra-1794138779.txt | 74911095f60362e9f45c977650d1bf4881024b02e1c60065201ab67c275df1af |
| pm-policy-Virgo-1794138788.txt | fe6eff3e8fc020f30e6fcc45837a62c701ed2572a8c38912b4841e2edd5e6511 |
| bridge-adapter-1794139125.txt | 01207c3f1abbdcb731c1595b04789c99daab45bc3b55d6505593fcd98afd75cd |
| virgo-canary-evidence-1794139270.txt | 58759f848b344b7b2c1dedf458dcd2d8269790ff1c43ce82bf9e0e8f25afebc6 |

首次ADB向root-owned目錄push被權限拒絕，未變更live host；保留已核SHA／owner/mode備份，
改用唯一暫存檔後由root核SHA並替換**測試host**。各舊host備份、舊receipt與失敗紀錄不刪。
原game25272不變，無tracer，沒有scanner pause；19:54兩台站台接受時間皆前進。
待完成：安全正式thread/call與控制owner、完整PM host打包／資格測試、實際兩頁ON/OFF／
重啟、第二遊戲profile。研究成功不是多版本完成，目標active。

20:01的第二probe44342也FINISH PASS、無殘留。Libra／Virgo最新站台均enabled、未paused、
資料與上傳接受前進，已進入既有job429；不要用先前無job的snapshot安排遊戲中斷。
下一輪先做可離線進行的production call／control owner實作；實機遊戲驗證前重新核查在途。

## 2026-11-08 20:16：控制owner已接入候選Java/native/renderer

原本Java在native鎖之前寫OFF並於loader返回後自行寫ON，現改為單一native lease範圍內的
`--enable`交易；`--off`不重送載入、`--status`完全唯讀。完整load record與新的v1/start
回報是ON必要條件；舊／未知／部分attempt不能用mapping或新鮮caps之類證據補成功。
全域legacy開關內容仍為on/off，原renderer可正常關閉；新renderer回報在末尾附start欄，
並未移除原欄。新loader不接受舊缺start回報授權啟用，也不載入第二份renderer。

兩ABI最終隔離各53項；較早34／52項receipt保留但不代表最終版本。

| Receipt | SHA256 |
|---|---|
| control-policy-Libra-1794140147.txt | d526ae588491029877c3f096e9448fecd39103f944a54938904ec145573fbd8d |
| control-policy-Virgo-1794140154.txt | fb0f5a3ef0a059baeec40b3256d992418135e25658983df2e198b188ae453b52 |

Virgo原load journal85項再通過，receipt SHA
`f77ac33e210db3b7d628394f5768cfd4d5bf2e16240fcfc885339358a682214f`。
兩台game PID18143/25272不變、TracerPid0；沒有換APK或關閉掃描。
需注意：候選bridge CMake仍pin已交付0.1.6 renderer，真正包新host前必須改綁**本次**不可變
ARM產物，不可把舊pin搬進新版。控制交易已在來源整合，實際完整操作與ARM/nativebridge
trace恢復仍未驗收；下一步進入正式entry與遊戲驗證，目標仍active。

## 2026-11-08 20:25–20:32：實際遊戲CPU邊界與封裝

只讀`StoppedEntry`取得後必須核對orig_rax=-1，不把kernel/restart狀態強制改成user-space。
此輪沒有single-step、SETREGSET、remote call或任何載入；沒有對game設EXITKILL。
[Linux ptrace語意](https://man7.org/linux/man-pages/man2/ptrace.2.html)只作機制依據，
是否可取得及釋放以Virgo實測為準；CPU邊界不保證沒有TLS／heap／linker鎖副作用。

第一版遇多個UnityMain名稱，ptrace前拒絕，receipt `game-entry-1794140781.txt`
SHA `7612db0a54c2d284dfa9df8510fe1f984a1122d345ad5baaaf614fd40e3924e6`。
解除pause後外部game換PID，本輪沒有重啟命令，不歸因於工具或scanner而無證據。
第二版篩選非sleeping候選，再核每個TID/TGID/start；新PID57557/TID57783有10/12個
user-space快照、2/12 kernel或不可讀，全部detach成功。
receipt `game-entry-1794140954.txt` SHA
`f44fff46d095153b00e0706b0f8310d0ae71f7a95367ee3e3cd91d9c822f4f4f`。
兩次具期限自有pause均已解除，沒有刪其他owner檔、改pending或掃描設定；兩台站台資料與
上傳在恢復後繼續前進。正式產品尚未接入取得thread後的call／UNKNOWN收容流程。

候選完整建置新增host/renderer配對檢查，19+27+8+4 host tests通過、雙ABI與Java/dex成功，
未簽名研究APK SHA `543af7948558c9d0183286bb8acb297d717432c6e5d594d17264643cecba080f`。
本批ARM renderer SHA `c088b1a77940cca4d71bdb08ba3f427617ad240a45ca803b0b5d6b17c553d674`
直接編入host，收據以LF固定格式，候選不覆蓋任何已交付產物／不供安裝。
完整NativeBridge載入、實際畫面/OFF/重啟和第二遊戲版本仍待實作驗證，goal保持active。
