# API34 ARM64／Linux 隔離研究

2026-11-10，使用者明確核准 [具體方案](API34_ARM_LINUX_PROPOSAL.md)，依
[三階段順序](STAGED_DELIVERY_PLAN.md) 優先進行 API34。這是軟體 ARM guest／研究 host 配置，
不代表原生 ARM64 手機、原廠無 helper 模擬器、x64 NativeBridge 或透視鏡產品已通過。

## 來源與隔離目標

新目標為專用 WSL2 `SeedLens-TCG-Linux` 內的 `SeedLens_API34_ARM64_Linux`，
Linux5588／5589、2vCPU／4GiB、全新6GiB ext4 data。沒有複製任何既有 guest userdata、
scanner、Google／遊戲 session 或憑證。原 API31 guest 關閉，index5 正常關閉後才啟動 CPU guest；
原四台及台中設備未操作。

Google 官方 Google APIs metadata 重新讀取，精確選 API34／ARM64／revision14，
archive `arm64-v8a-34_r14.zip` 實際1,610,393,229 bytes，SHA1
`2fe8b46d419a3400e30f31b0152b241b50c8b99f` 符合官方 metadata；實際 SHA256
`bd1c323cbdd3c9dd55dde54588b298d172fd126fe16e127a7bd7aeab6897890a`。
23個解壓檔案逐一驗大小／SHA256，保留 archive、manifest 與原 SDK 映像。

| 官方原件 | SHA256 |
|---|---|
| kernel-ranchu | `4076bd230e0832e23837e62c1fb03daf5eb4fd5aac731818320c26fa1bf8b2e5` |
| ramdisk.img | `5c4a40a87ea671396a683a30fde67a3e51c63a21d332a9c5a18ecf20c53e6185` |
| system.img | `53a061cd8eea937985357abd8d16d9581a6b34512cec241c40633051dd2a05d7` |
| vendor.img | `d5f44441eb7ced018ef09c09e25658bd9218d9633817f67d5b07d8f365876821` |

SDK emulator 仍為37.2.13/build16740729，固定 QEMU SHA
`e697effa1e3f5e0962dab1c8fdbca5787582e6081e2c22eabe74ff396fbd2436`。
來源 metadata 的 API34 不代替完整 Android guest 的實際 API 讀值。

## Host 研究守門

`build-tcg-software-selection.ps1 -Api34Research` 才建置新 mapper，獨立 library／marker、
AVD／5588且唯一參數、明確 software CPU，拒絕 KVM／衝突或重複參數。
新 mapper SHA256 `1b702320855a24203f8fb6b081050965566617fae7630758998b6803e60125a7`。
未加開關的 API31 helper 重建 SHA 仍為原 `bc30eb...e1e795`，沒有替換既有原件。

Vector 控制器以 `tcg_host_targets.py` 的兩個固定 tuple 分開 API31 與 API34；各自 marker、
AVD、port、mapper 路徑／SHA均配對，原 vector library／QEMU／MMU slots pin 不變。
allowlist 不是裝置操作授權。兩者都是 Linux host 研究檔，不打包 Android APK、不成為使用者依賴。

本輪實際驗證：

- 自有 Linux HOST guard fixture：13例通過，包括錯目標／port／marker、重複／衝突參數、
  保留真 host probe 呼叫及缺原函式實際 exit86。此 fixture 使用假的 `/proc` 讀取器，不稱為實際 QEMU 正例。
- 純 Python 固定目標 policy：5個測試方法通過，含跨 API tuple、缺／重複參數與 software 衝突負例。
- 真 gdb 控制器：API31／API34缺 marker、錯 marker／exe／preload共6例，均exit1、在 starti 前拒絕，未啟動 guest。
- 既有精確遊戲 profile evidence 測試16例回歸通過；產品版本、Root及 NativeBridge 守門未修改。

## CPU 與完整 Android 層

新 API34 官方 kernel＋同自有 V2 initramfs，實際 uname為
`6.1.23-android14-4-00257-g7e35917775b8-ab9964412`／aarch64。

| 新目標 CPU 輪 | 實際結果 |
|---|---|
| PID493，預設 TCG、無 mapper／vector helper | 5 pass／2 fail／0 skip；兩個 SDK 相鄰序列實際 SIGSEGV，父程序收容並繼續；正常 power-off／host exit0，非逾時 |
| gdb499／inferior509，normal TB＋API34 mapper／vector-save | 同 V2 原件7 pass／0 fail／0 skip；正常 power-off、inferior及debugger皆exit0，無收容訊號；不是以debugger exit0單獨判定 |
| gdb508／inferior522，同配置＋既有 V3c 診斷原件 | 7 pass／0 fail／0 skip；故意 NULL read 記到 address0／signal11，父程序 wait/reap 控制通過，沒有把 fault 算成功；正常 power-off／inferior exit0，非逾時／無收容訊號 |

故意 NULL-fatal 診斷控制已通過，完整 Android 另驗。CPU 正例不代替實際 Android API 讀值或完整開機。
沒有普通 App `su -M`、JNI、Root／產品或任何三遊戲資格，不更改九組矩陣的未通過狀態。

### 第一輪完整 Android：APEX 內路徑缺失，未完成 boot

原官方 ramdisk、同 mapper／vector-save、normal TB 的 gdb654／inferior664，
2026-11-10 UTC02:27:09啟動，25分鐘上限。資料分割區加密、APEX解壓及系統服務啟動有實際進度，
但ADB一直offline，沒有讀到實際Android API／boot完成，也沒有請求Root或安裝任何APK。

guest約675秒，init明確記錄找不到 `/apex/com.android.conscrypt/bin/boringssl_self_test64`，
無法啟動 `boringssl_self_test_apex64`；其後約691秒系統自行要求
`reboot,boringssl-self-check-failed`。另有netd退出1、由onrestart帶動zygote及相關服務重啟。
第二個guest boot中，vendor與system的非APEX BoringSSL self-test各自exit0。
因此目前證據是APEX路徑／啟動失敗，不宣稱已證明密碼運算錯誤、OOM或完整根因。

保留自動重啟與失敗原件後，以此新guest的console正常關閉，UTC02:40:50 inferior及debugger
皆實際exit0，非逾時／未送收容訊號。正常host退出不是Android成功；未讀取到UID時不補記unroot。
關閉後對此空白研究AVD的18個檔案做冷備份、逐檔SHA一致，SDK原映像保留。

下一輪只增加[官方錯誤logcat輸出參數](https://developer.android.com/studio/run/emulator-commandline)，
定位APEX啟動原因。此診斷不移除self-test、改SELinux／clock／Watchdog或產品界限。

新 index5 關閉前，實際 OFF、30份歷史 attempt可見、無正在執行的 loader，讀取的遊戲
thread tracer皆0；adbd實際恢復UID2000，再正常 shutdown。關機後 CLI 不再提供 adb_port，
初稿因此在收尾解析報錯；重核名稱／created timestamp及關閉狀態完成確認，沒有重送 shutdown。
專用 WSL distro 在無 QEMU 並 sync 後重啟以回收下載快取；不改全域 WSL／Windows設定或重開主機。
啟動前 Windows 可用實體記憶體實讀約6.7GiB，Linux5588／5589可用。

## 原生載入診斷準備（僅編譯）

針對先前 API31 正常 ELF load 超出產品界限的未解問題，另建 fork-only
`arm64_load_diagnostic_test`，不改15s／2s或既有正常驗收套件；輸出 manager initialize、
attach／snapshot、CALLING ACK、injector init、ELF entry／export call及收尾的時間標記。
沒有外部 target 參數，必須新 child／新 attempt，不能用來重送舊 UNKNOWN。
四個新 build artifacts 皆核為 AArch64，正常 budget fixture 不含 diagnostic marker；
缺 research-budget 開關或選 x64 ABI 的兩個獨立 CMake configure均實際拒絕。
此 checkpoint 尚未在 Android 執行，不替代 ARM 原生、Root、JNI 或產品資格。
詳見[獨立診斷契約](../research/arm64-product-budget.md)。

## APEX 啟動診斷：實際 device-mapper 等待失敗

同新guest的錯誤logcat首次與kernel console爭用stdio，SDK啟動即拒絕；stderr原件保留。
該SDK路徑甚至以inferior／debugger exit0結束，證明不能單憑退出碼判成功。
移除kernel console後的stdio-only logcat另輪沒有取得guest診斷，約6分33秒正常console關閉；
不把沒有錯誤輸出當系統健康。兩輪皆無Root／APK安裝。

其後使用SDK自帶`-logcat-output`檔案與`-shell-serial`私人Unix socket（非網路Root端口），
在同AVD、原kernel／ramdisk上得到guest serial輸出。gdb933／inferior943的guest約129–135秒，
實際記錄各APEX的`Failed waiting for dm device`／`Failed to create Apex Verity device`，
最後24個套件失敗、0 activated；Conscrypt、adbd及resolv均在其中。這縮小了啟動失敗鏈，
但仍不稱完整根因或修好。串列未取得shell prompt／實際API；沒有發出Root shell命令、
請求adbd Root或安裝Magisk／任何APK。隨後正常console關閉，inferior exit0、非逾時／無收容訊號。

對照[固定Android14 AOSP來源](https://android.googlesource.com/platform/system/apex/+/refs/tags/android-14.0.0_r1/apexd/apexd.cpp)，
APEX預設可為每個套件建立一個並行worker；dm裝置等待預設1秒。官方
[sysprop定義](https://android.googlesource.com/platform/system/apex/+/refs/tags/android-14.0.0_r1/apexd/sysprop/ApexProperties.sysprop)
提供`apexd.config.boot_activation.threads`。這是來源對照，不冒稱SDK完整build manifest一致。
下一個單獨比較只要求worker數1，以實際property／啟動紀錄確認是否生效；**不增加dm等待時間、
loop重試次數、產品15s／2s界限，不停用self-test／Watchdog或修改SELinux／clock／API**。
目前只是並行負載的假設，尚未取得有效設定或成功開機證據。

### 固定單一屬性的私人 ramdisk 候選

`-prop`比較輪有SDK「only qemu properties supported」警告，仍未讀回實際設定，APEX仍失敗；
已正常關閉，不當作worker數1有效或成功。唯讀檢視官方ramdisk確認存在
`system/etc/ramdisk/build.prop`；AOSP init的second-stage property讀取流程有對應來源。
另建[固定用途工具](../research/api34-apex-single-ramdisk.md)，保留整份原壓縮ramdisk作prefix，
只附加一份覆蓋該build.prop的newc／LZ4 archive，原屬性內容及owner／mode／time保留，
僅增加`apexd.config.boot_activation.threads=1`，沒有加入Root或修改init可執行檔。
32個effective entries逐一比對，差異只有該檔；原SDK不變，私人候選SHA
`ccba894ee41e3388f4228949418be61c728fc02c79caa631241ddf6fa1c3e072`。
錯API31來源、SDK內輸出、覆寫既有候選三個實際負例均拒絕，原件SHA保留。
同新guest的明確private ramdisk override已開始獨立觀察；目前仍未讀回property或完整boot，
不稱為原官方無修改ramdisk、Root、App或產品資格。所有原失敗與冷備份保留。

### 私人候選的實際部分進展

gdb1500／inferior1510，UTC03:11:15起以同kernel、mapper／vector、2vCPU／4GiB及私人
ramdisk候選啟動，boot `7c3bd34b-36c9-421a-902b-a40418ff5617`。guest約117–131秒，
34個APEX由同一activation worker依序掛載成功；Conscrypt、adbd、ART及resolv皆有獨立mount紀錄。
APEX BoringSSL self-test實際exit0，沒有停用該測試。

後續一般shell實際讀取exit0：Android14／API34、arm64-v8a（abilist亦只有ARM64）、
uname aarch64、NativeBridge0、Enforcing、UID2000與上述boot；fingerprint為
`google/sdk_gphone64_arm64/emu64a:14/UE1A.230829.050/12077443:userdebug/dev-keys`。
但完整屬性讀取多次40秒逾時；獨立echo約17.4秒、單API約11.8秒、多項identity約58.5秒，
屬於讀取延遲，不套用或放寬產品15s／2s限制。

為讀取受限內部設定與framework狀態，僅暫用原廠adbd Root診斷：同boot／API34／Enforcing
實際UID0後，讀回`apexd.config.boot_activation.threads=1`，確認此私人候選設定生效。
當時zygote／bootanim running、沒有system_server，logcat有zygote預載class警告。
沒有安裝Magisk、APK或執行原生載入。unroot初次回覆逾時不記成功；其後重新讀取同boot、
UID2000及Enforcing才確認恢復。此診斷不是普通App Root或完整開機資格。

當時主機i7-10700KF／8C16T總CPU實讀100%；只讀自身QEMU thread統計，主要CPU時間集中在兩個
thread，未以stack確認其全部用途。不能單憑CPU繁忙認定唯一根因。完整getprop觀察改為較低頻單boot flag，
避免監測增加負載；保留原40秒逾時與未完成觀察，不把bootanim／API讀值當App可用。

收尾前一般shell再次讀到同boot、UID2000與Enforcing，`system_server` PID618已出現，
但`sys.boot_completed`仍空白。UTC03:35:56正常console關閉，03:35:57 inferior／debugger
皆exit0，非逾時、未送收容訊號；此約24分42秒觀察仍記為「完整開機未完成」。

### 獨立一小時安靜觀察

同一新AVD／原kernel／私人單worker ramdisk／mapper及vector配置，另輪UTC03:38:15啟動，
gdb2216／inferior2226。這輪只將開發者的完整開機觀察窗口設為3600秒，並減少ADB讀取：
首次600秒後才讀單一boot flag，之後間隔180秒；串列輸出留在私人檔。
沒有改guest時鐘、Watchdog、dm等待、產品15s／2s或版本守門。前輪未完成與失敗保留，
此輪必須另取實際完整boot、身分及App證據，不以延長觀察視為通過；尚未安裝Magisk或APK。

UTC04:03:11完成的一次唯讀篩選logcat，實際記到system_server495的Package Manager
逐項掃描內建APK／overlay並收集certs；因此此刻有初始化進展，不能僅由boot flag空白推論死鎖。
先前stack dump逾時及錯誤日誌仍保留，不把沒有Watchdog kill紀錄當完整健康。

等待boot期間另準備[ARM普通App Root client0.0.6](../research/root-probe/README.md)：
精確API／ABI／kernel guard、41項JVM檢查、Android Java／dex、原研究簽章與ARM-only封裝
已驗，錯ABI建置實際在輸出／簽章前拒絕；沒有部署或Android執行。
此研究包不增加產品版本、NativeBridge資格或使用者依賴。

後續來源複查發現0.0.6尚缺「實際封裝ABI」與系統primary ABI的綁定；它未部署，原件SHA
保留。另升[0.0.7/code7研究包](../research/root-probe/README.md)，APK asset固定runner ABI，
Root查詢及PM command皆須匹配；44項JVM正負例、原研究簽章與ARM-only asset／runner封裝
已驗，仍未安裝／執行Android，不追認0.0.6有此檢查。

官方Magisk31.1 ARM材料僅在Linux host備妥；另以同APK內standalone x64 magiskboot做
host-only decompress／單檔extract，實際保留三段ramdisk的單worker property，私人輸入SHA不變。
沒有執行Magisk daemon、patch ramdisk或向guest傳入Root檔案；完整boot及載入安全通過後才進Root。

### 一小時上限輪的實際收尾：仍未完整開機

此輪boot `2e864ece-9e3c-4955-ac67-79137f47e86e`；開機flag的低頻讀取皆exit0但空白，
最後一筆單flag約55分22秒。Package Manager實際掃完258個system packages，記錄耗時
869689ms／cached34；最後取得的PM進展是解壓系統Trichrome stub。後續唯讀stat找不到
該暫時目的路徑，不能由此判定解壓完成、CPU死循環或完整阻塞根因。此輪沒有取得Watchdog
kill證據，也不以無kill推論健康。

因剩餘時間不足以保留原生測試及unroot窗口，本輪沒有請求Root、安裝APK或執行native／JNI。
正常收尾前最後讀取exit0，約62.9秒取得UID2000／Enforcing／API34／arm64-v8a及同boot，
system_server495仍在、boot flag仍空白。UTC04:37:12 console kill回OK，04:37:13 inferior
及debugger皆exit0，`timedOut=false`／未送收容訊號；實際觀察約58分58秒，含正常收尾，
不是「足60分鐘後逾時」或完整App／Root／遊戲資格。

下一輪先保存此空白研究AVD的冷備份，再比較同一AVD已產生初始化快取後的冷開機；
不清資料、不改SDK／kernel／clock／Watchdog或產品界限，也不把研究OS重啟記成產品重啟驗收。

關閉後確認無存活QEMU，對此AVD的24檔做私人冷備份，每檔大小／SHA一致；原SDK四個
kernel／ramdisk／system／vendor SHA再次吻合intake。檔案讀寫後Windows可用實體記憶體
一度約0.6GiB，故沒有立即啟動下一台；先sync並只terminate指定專用WSL distro回收資源，
不停止原四台、不改全域WSL或重開Windows。下一輪仍須重新核實可用實體記憶體與埠。

## 保留初始化資料後的獨立冷開機與堆疊診斷

指定distro停止後Windows可用實體記憶體約13.5GiB，index5仍關閉，Windows／Linux目標埠
無listener；四個原SDK SHA再次核對。原AVD資料保留，UTC04:44:26另啟gdb503／inferior513，
沒有改image、CPU數、clock、Watchdog或產品時間界限。此輪34個APEX再次activated，
加密data完成ext4 journal recovery；不把掛載前raw block的invalid ext4訊息誤記資料損毀。

早期ADB未就緒的一次讀取在任何Root命令前拒絕，原收據保留；連線就緒後實讀boot
`035e363f-02bd-45e6-a4e7-462f1438aa96`、API34／arm64-v8a／aarch64／NativeBridge0／
Enforcing／UID2000。為讀前兩輪系統堆疊，兩次暫用原廠adbd Root，皆實讀同boot的UID0，
最後各自實讀UID2000／Enforcing，沒有安裝Magisk或APK。這輪含有界唯讀Root診斷，
**不是完全無干擾的quiet boot**，不單憑開機時間差推論快取改善。

前輪ANR摘要只存等待狀態，完整Java／native堆疊另在trace檔。以trace header的system_server
PID及block的sysTid核對後：

- trace_01／11:54:39／PID495：當時main等待DisplayManager的SyncRoot；持鎖的Java thread16
  正在display configuration的UnixFileSystem.normalize。後續確有PM掃描進展，所以這一張
  不能代表最後Trichrome停滯或持續死鎖。
- trace_00／11:32:12／PID618：當時main在IntentFirewall讀目錄／ART NewObjectArray的等待。
  首次僅依名稱選出的另一個「main」實為sysTid645，**該選取作廢**；原收據保留，另存
  按PID618選出的正確block。不以thread name代替身分。

這些是不同時間點的初始化stack，不宣稱已證明crypto、GC或解壓根因，也沒有停用相關服務。

另開始與framework資格分開的底層診斷準備：只對自有fork跑既有產品15s／2s界限，先收容
suite，才正常ARM context／ELF；若需要時間標記，也只能新child／新attempt，不重用UNKNOWN。
在開機負載下的結果另列，不替代完整boot／普通App Root／JNI或遊戲驗收，正式full-boot gate
保留。首次嘗試尚未進suite：ADB Root切換host等待逾時，但後來實讀UID0；initial unroot在
offline時未送達。隨後同boot再提出一次unroot（沒有重送Root），host等待仍逾時，最後實讀
UID2000／Enforcing才確認恢復。沒有原生target、載入或新attempt，不把host exit1直接等同
權限沒有改變；後續以實際UID／boot／Enforcing決定是否能進下一層。

### 啟動負載下的實際底層結果

後續以Root切換後的實讀UID0／同boot／API34／Enforcing准入，沒有重送Root。三個原生檔
guest SHA逐一吻合後，supervisor產品界限suite實際10/10／exit0。ARM產品界限fixture則
正常function及三種故意故障各達預期，mode4真ELF case仍UNKNOWN／target dead，整體exit1。
另開全新child／attempt跑獨立diagnostic：manager initialize約1375–5807ms，CALLING ACK
6197ms，卻沒有到`before-injector-init`，16631ms記supervisor收容完成，exit1。
這縮小到injector初始化前的本機準備，不將兩份UNKNOWN改為成功或重送舊child。

來源確認該區間包含`popen("getenforce")`；另備[唯讀直接查詢比較](../research/selinux-status.md)，
保持同一位置與15s／2s，沒有改產品backend。新read-only helper的17項格式／錯檔拒絕與
真Enforcing讀值已在Android實跑exit0；直接查詢的ELF／budget新artifact僅編譯，尚未執行。
結束unroot雖host等待再次逾時，最後仍實讀UID2000／同boot／Enforcing；不只憑host退出碼。

錯誤日誌另出現updatable adbd在boot前退出四次及APEX revert warning；本輪確有多次預期
Root／unroot daemon restart，故不把它直接記成自發crash，也不忽略診斷對開機的干擾。
後續合併一次Root會話、收尾才恢復UID2000，不停用APEX健康檢查；仍須另做無切換干擾的
完整boot驗證。原本無Root的一小時上限輪未完成結果也保留，不用此觀察抹去它。

此輪最後再次實讀UID2000／API34／ARM64／Enforcing與同boot，system_server564仍在、
boot flag空白。UTC05:43:18 console正常關閉，05:43:20 inferior／debugger皆exit0，
非逾時／無收容訊號，實際約58分54秒。自己的UNKNOWN及raw fixture保留，沒有安裝Magisk／
研究APK／產品。直接SELinux比較另通過x64／缺budget的兩個真configure拒絕與預設不生成
研究target檢查；產品兩個backend來源仍未整合該候選。

### 單次 Root 會話的獨立 ELF 比較

保存前輪24檔冷備份且逐檔SHA吻合、原SDK四個映像再次核對後，同一AVD於UTC06:13:41
另啟新cold boot。實讀boot `0c076561-3d4b-44df-8895-80300b53ee8f`、API34／ARM64-only／
aarch64／NativeBridge0／Enforcing及UID2000；只送一次原廠adbd Root，再實讀同boot的UID0。
保持同一Root會話比較三個各自新fork，guest executable／paired fixture SHA皆核對。

新增標記的原查詢baseline與唯讀直接查詢diagnostic均實際exit0，確認ELF載入、export呼叫、
卸載、完整snapshot相等、returned／detached ACK與target仍活。普通direct-query budget fixture
亦實際5/5／exit0：正常function、三個預期UNKNOWN且已收容的故障，以及正常ELF lifecycle。
未放寬產品15s／2s，沒有重用或重送前輪UNKNOWN child／attempt。

baseline的SELinux區間約7585ms，direct樣本13ms；整體diagnostic標記15121ms與7663ms包含
產品operation以外的setup／teardown。這是先後兩筆、負載會改變的樣本；**baseline這輪也通過**，
因此不能宣稱已證明shell查詢是舊逾時唯一根因或已修復所有負載條件。詳見
[逐項比較與artifact SHA](../research/selinux-status.md)。產品backend尚未整合；framework仍在
初始化，沒有普通App su-M、JNI、遊戲或完整API34資格。此單次Root會話的實際恢復UID2000及
正常關機另待收尾核對，不提前宣告。

同Root會話稍後以共用39d842… fixture、各自新child比較兩個無額外diagnostic標記的普通
budget套件，順序direct再baseline，兩者皆exit1。direct的ELF雖載入／呼叫／卸載並達四階段，
原期限內未確認worker reap，因此Refused／target alive；baseline僅兩階段，UNKNOWN且target
已收容。正常function與三故障分項各達預期，不把整體失敗改記5/5。這進一步顯示直接getter
本身不足以讓較後段boot負載穩定通過；先停止新增loader測試，沒有放寬期限或reap條件。
PM同時於UTC06:42:05完成258個system packages（625310ms、cached258），發現前輪未完成的
Trichrome目錄缺base APK，正常刪除無效目錄後開始新解壓。此記錄不是已有完整解壓或boot成功。

### Trichrome 實際進度與同次開機的有界延長

在既有單次Root會話內，以同boot、system_server PID570／UID1000／start81150前後核對，
只讀FD與fdinfo，沒有signal、Root切換或寫系統／data。實際開啟的輸入是官方image內
`TrichromeLibrary.apk.gz`，輸出則為AtomicFile的`TrichromeLibrary.apk.new`；正式目的檔尚未
存在，不等於沒有寫入或大小0。AOSP14r1的
[decompressFile](https://android.googlesource.com/platform/frameworks/base/+/refs/tags/android-14.0.0_r1/services/core/java/com/android/server/pm/PackageManagerServiceUtils.java)
亦使用AtomicFile；這是來源對照，實際進度仍由本輪FD讀值判定。

兩次後續只讀樣本的command開始時間UTC06:47:27與06:50:52，相隔約205秒；各讀取本身
耗時約53／51秒，因此不是瞬時或精確吞吐基準。輸入位置3350538→6809098，輸出位置
9436452→16781107，官方壓縮檔大小58715906bytes。兩者都向前，足以否定此區間完全沒有
I/O進展；不能據此宣告解壓完成、沒有其他卡點或整體boot成功。`stat`未加dereference時
取得的是proc FD symlink長度64，不當作輸出APK大小。粗略速率只供安排開發環境觀察窗口，
不作產品效能或成功證據。

為保留同次初始化，UTC06:55另以新host guardian接手**同一QEMU／boot**的觀察，上限由
3600延到總7200秒（UTC08:13:41）。新guardian先以pidfd與exe／start／parent綁定自己的
QEMU、gdb、原supervisor及原關閉計時器；實讀舊supervisor暫停及舊timer退出後才接手。
原state.json的3600保留為原設定，另外的私人handoff receipt才記有效新界限，不覆寫歷史。
新guardian負責有界console關閉／必要時精確pidfd收容，結束後恢復原supervisor記錄實際退出。
這不改guest CPU／RAM、image、clock、Watchdog、SELinux或產品15s／2s；原單次Root會話
仍在原2700秒點結束並實讀UID2000。新guardian的完整boot／PM／AM雙次核對與最終退出
仍待實際回報；此時未提前宣稱正常收尾。

原單次Root會話其後已完成：UTC06:59:09 unroot命令exit0，06:59:35實讀UID2000／同boot／
Enforcing；新guardian讀到實際cleanup後才接續普通shell的唯讀framework log stream。
尚無Magisk、研究APK、普通App su-M、JNI或遊戲安裝。

主機側40秒取樣顯示GDB CPU計數不增，兩個vCPU thread各約每秒1.3萬次voluntary switch，
因此另備[單執行緒TCG排程比較](../research/tcg-single-thread.md)，保留2 guest vCPU／4GiB。
新獨立marker／helper與C23、Python8組、實際GDB拒絕及編譯／建置拒絕檢查完成，舊API31／
API34 helper重建SHA仍吻合原件。**新排程未啟動guest**，原boot繼續；先自有CPU／故障控制，
再考慮完整Android，不把主機計數或準備當作根因或相容證明。

另備原簽章自有ARM JNI probe0.0.3/code3：固定研究thread窗口由35秒改120秒、批次NOP後才
讀clock，instrumentation等135秒後仍要求原thread返回及後續JNI正確。原因是本輪20–50秒
身分讀取可能用盡舊fixture的協調窗口，這是來源層風險，不冒稱已發生App失敗。產品15s／2s、
verifier25s alarm及全部guard未改。新APK實際ARM64／stored／digest／16KiB／原簽章檢查
通過，canary SHA不變、舊APK及verifier bytes不變；**未安裝或裝置執行**，見
[新fixture與SHA](../research/arm64-jni/README.md)。

後續另外核實原廠`/system/xbin/su 0`只供shell診斷的能力：外層adbd shell實讀UID2000；
唯讀子程序實讀UID0／u:r:su:s0／Enforcing／同boot，返回後外層仍UID2000。沒有再送adb
root/unroot、重啟adbd、裝Root provider或修改產品入口；這**不是普通App su-M資格**。
只用此能力再次讀同system_server start81150的固定Trichrome FD，UTC07:30:38開始、約92.5秒
完成的樣本取得輸入位置58435082／檔案總58715906bytes（約99.5%），輸出位置128601484，
稍後dereference stat實際輸出大小129048576bytes。不同命令間會繼續寫入，兩個輸出值不是
同一瞬間；同boot及外層UID2000再次核對。這證明後段仍有大量進展，並不證明gzip CRC、
AtomicFile rename、PM驗證／安裝或boot已完成。

其後PM於UTC07:32:22開始讀取新Trichrome package憑證，07:32:48實際記錄system package
code path由Stub更新到剛解出的data路徑；下一秒開始Chrome stub解壓。這是Trichrome後續
PM進展的直接證據，不再把它列為仍停在解壓起點；整體boot／所有stub／App仍未完成。

### 本輪兩小時上限收尾與後續準備

Chrome於UTC07:45:25亦記錄code path更新，接著處理WebView；關閉前的PM日誌已越過
WebView解壓並進入app-data準備。沒有取得boot flag1或雙次PM／AM穩定核對，不將初始化
進展當完整開機。原擬再延長總10800秒的接管，其實際execute在第一個會變更程序狀態的
步驟之前，因已超過預定安全接管窗口而assert拒絕；**沒有三小時guardian接手**，有效上限
仍7200，原監督繼續收尾。不能把先行提出的延長計畫記成已執行。

最後metadata從UTC08:11:37開始，68.6秒後exit0：UID2000／Enforcing／API34／arm64-v8a／
同boot，system_server570仍在、boot flag仍空。08:12:46 console正常關閉，實際觀察約
119分5秒；QEMU／gdb均exit0、原supervisor恢復並記錄、無pidfd收容訊號、非逾時。原state
的3600與handoff的7200分開保留。沒有Magisk、研究APK、JNI或遊戲安裝。

關閉後24檔冷備份逐檔大小／SHA核對，全部原生fixture的UNKNOWN／Refused歷史和本輪初始化
資料一起保存；原SDK四個映像SHA再次吻合。備份後Windows可用實體記憶體約0.69GiB，故先
確認指定distro沒有存活QEMU並sync，只terminate專用`SeedLens-TCG-Linux`回收資源。
不停止原四台、不改全域WSL或主機設定；回復足夠實體記憶體並重核兩埠後，才可啟動下一輪。

另已備妥官方scrcpy5.0.1 Linux portable及發布SHA核對，`--version`／`--help`在主機通過，
WSLg存在；尚未連線Android。它是owner登入用的開發端視窗準備，不是產品依賴或裝置驗收。
新JNI probe0.0.3、Root client0.0.7及既有verifier亦只複製到Linux私人host目錄並核SHA，
沒有安裝到guest。下一輪仍先核實平台能力，未通過前不整合或部署產品。

記憶體回收至約11.3GiB後，單thread TCG的原V2與V3c各自實際7/7；V3c另有故意NULL
SIGSEGV／child-reaped控制。兩輪14個vector wrapper／AVX flags1/1未變，正常power-off、
inferior與gdb exit0、無外部收容。原SDK警告另留，不當Android App通過。其後UTC08:36:17
同AVD保留資料另冷開機，啟動前即選10800秒開發觀察上限，沒有對存活程序再做延長接管。
新boot早期到system_server494，系統255項／parser cache258與data三項掃描合計99.751秒，
沒有重做三個stub解壓；快取與排程同時改變，不據此單獨歸因效能。實際完整boot、普通App
Root／JNI、三遊戲仍待各自核對，見[新排程結果](../research/tcg-single-thread.md)。
