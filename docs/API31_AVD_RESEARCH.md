# API31 官方 AVD 隔離研究（2026-11-09）

> 最新：研究簽章已獲同意；真App PM／ClassLoader／ARM JNI與修正後timeout收容已通過，正常App Root及遊戲仍未驗。詳見[本輪新增實測](ZHUBEI_LAB_20261109.md)。下方「未簽署／未載入」是較早階段紀錄，保留原始限制與失敗歷史。

## 持續目標後續：PM研究載入器與封裝準備

使用者另要求持續到Android12／15與遊戲156.0／156.1四組成功可用，已建立持續目標；具體完成條件見[四組驗收計畫](ANDROID12_15_GAME156_GOAL.md)。下方原生測試證據不自動升格成四組通過。

新增獨立研究PM載入器：共用既有LoadLease／LoadAttempt／supervisor，只有自有probe／專用thread／API31可呼叫；精確host SHA及canonical PM carrier路徑在ptrace前核對，持久CALLING ACK後才允許遠端寫入／EXITKILL。UNKNOWN保留且同boot/PID/start不能重送。這段來源已編譯，**新載入器的正常載入、實際JNI與timeout正例還沒執行**。

`build-api31-lab.ps1 -Unsigned`完整建置通過：probe0.0.6/code6、carrier0.0.1/code1與runner。新版本與原API35研究probe0.0.5分開；產品版本未變。兩個研究包只是Enforcing／ClassLoader開發工具，不是產品安裝條件。

| 未簽署研究artifact（禁止安裝） | SHA256 |
|---|---|
| probe0.0.6 | bbd6befa8f97ece183aba7562f39cc8bb7316ba5eb1c898a0df42043d3b03703 |
| carrier0.0.1 | 8b29757619e34e907fc6f75df33a640fe027df74a223d1ddd4025e44b987a9b0 |
| carrier內x64 host | 7b8b73f9a2012c1ac9b2d8d294c285997af77de83a7525e67fe32c0086a74179 |
| API31 PM runner ELF | 531e9c0f9492171f00d841660e66ca6822ae233e783fb623a938bac729102642 |

實際ZIP正好一個stored／16KB entry、ARM canary→host→runner三層digest配對均驗證。新的host精確version/code由research manifest生成，不靠放寬版本判斷；署名入口缺明確外部key／憑證時，在建立交付保留目錄前拒絕。簽署正例尚未驗，沒有建立key。

共用host來源改為選擇精確研究profile後，原API35 `seedprobe_host`亦編譯回歸exit0，未部署。產品來源與遊戲profile沒有隨研究版本改動。

重新啟動**同一已授權API31 AVD**，以新bootID及精確AVD名稱核對後，只push新runner ELF。最終SHA的七個pre-attach拒絕案例通過：API32、031、非法PID、非法start、真zygote錯套件、未知option、缺host。拒絕前後沒有建立持久attempt目錄，zygote PID/start持續且TracerPid0；沒有以錯套件負例冒稱真App載入成功。早期不同host-pin的runner負例收據亦保留，不把兩批重複案例加總擴大測試數。

另以**自己的shell子程序**降UID10000，確認stock su再次執行遭Permission denied。這不是已安裝App的SELinux測試，但補充了stock Root並不等於正常App可用的限制；沒有修改su、SELinux、mount或安裝Root provider。

本輪結束再次確認沒有研究／遊戲套件或新runner程序殘留、沒有建立attempt目錄，SELinux仍Enforcing；恢復non-root adbd並正常關閉同一AVD，資料與全部負例保留。沒有重啟host或操作其他instance。

私人收據（`.tools/lab/d4397362aaa945bbb6576d0407a6480a/`）SHA：

- pm-final-guards.txt：`b8daed9203c22f0aee5f51fe43f5f089297079b79aa25b35aa0aa3f04fc2d105`
- stock-su-appuid-test.txt：`aa1b23aafb4976da944bbfb57399972b654f33d4e35d4cac483734386b80f898`
- unsigned-pair-check.json：`98e269714d957a44dab54d6f039b9eee282629b097dfa4309c0c4b5d0b22e447`

研究簽章／新lab key及新Android15 instance的明確確認仍待回覆；未以持續目標替代這些未定範圍或原產品簽章。下一步是取得研究簽章後，在新鮮自有App實測PM載入、ClassLoader、JNI、正負例與收容，再處理正常App Root環境；需要遊戲登入時另提示。

## 本輪結論

已取得並實際啟動Android12/API31，完成x64 kernel／CPU context與失敗收容fixture；尚未通過真實App的JNI／ClassLoader canary、產品Root契約或遊戲畫面。不能把本頁當成API31產品支援公告。

使用者明確同意下載、建立與啟動官方API31 x64 AVD。所有guest操作皆顯式指定`emulator-5580`，每階段核對AVD名稱與當次bootID；未操作原四台、台中Libra/Virgo或掃描器。沒有安裝Root／顯示模組、修改SELinux或登入。

## 具體環境與身分

| 項目 | 本輪實際證據 |
|---|---|
| AVD | `SeedLens_API31_x64`，`D:\SeedLensLab\avd\SeedLens_API31_x64.avd`；新建空白資料，2vCPU／4096MiB |
| Emulator | 官方SDK 37.2.13.0/build16740729；WHPX檢查成功，沒有新增driver或重啟host |
| System image | `system-images;android-31;google_apis;x86_64`；package.xml revision14；包內source.properties仍記revision12，兩者不混寫 |
| 實際OS | native runtime API與`ro.build.version.sdk`皆31，release12，userdebug |
| ABI／bridge | x86_64，ABI list為x86_64,arm64-v8a；`libndk_translation.so`，不是原API35/libnb.so組合 |
| Kernel | `5.10.185-android12-9-00043-g383607d234da-ab10550364` |
| Build | `google/sdk_gphone64_x86_64/emulator64_x86_64_arm64:12/SE1A.220826.008/10564458:userdebug/dev-keys` |
| SELinux | Enforcing，全程不改 |
| Root | 原廠`adb root`成功，uid0／`u:r:su:s0`；原廠`/system/xbin/su`不接受`-M`，產品仍不能照原入口使用 |
| 遊戲／APK | 沒有安裝遊戲、Lens或研究APK；沒有登入，不產生兩頁顯示或PM載入證據 |

原生ARM64/API31尚無實際目標。x64 kernel上的ARM轉譯不能當成ARM64 kernel、regset、SVE等通過證據。

## 真正執行的隔離測試

全部fixture由本repo來源建置，push後逐檔比對裝置SHA256；使用唯一root-owned 0700測試目錄。測試只接受自建fixture或fork child，沒有外部遊戲target、RPC、庫存或GPS操作。

| 測試 | 實際結果與邊界 |
|---|---|
| runtime API guard | `canary-root 0 0 31`先回報實際API31再按預期拒絕非法PID；API32、35不相符及031格式均於process lookup／attach前退出8。沒有把非法PID案例稱為載入成功 |
| `probe_platform_test` | exit0；22個compile-time selection assertions已於兩ABI編譯；不是22次裝置能力測試 |
| `supervised_load_test` | 7類通過：正常、CALLING前後失敗、逾時、錯序、DETACHED後非零、RETURNED未DETACHED；pidfd只收容自有child |
| `load_journal_test` | 85項、6個崩潰點通過；UNKNOWN、半筆與舊attempt不重送，fixture原件保留 |
| `control_transaction_test` | 68項通過；測試目錄內的控制、狀態及profile負例，不寫產品開關 |
| `kitty_syscall_test` | 實際overlay共15例：正常、逾時、tracee/tracer訊號、退出；存活child GPR相同、heartbeat續行、正常回收。無訊號逾時101–102ms；含訊號producer的全例269–290ms |
| `x64_call_context_test` | 15例：正常、逾時、非法指令、SIGUSR1、退出；正常兩次call回值346，GPR/XSTATE/red-zone相同且child恢復；UNKNOWN不再呼叫。逾時102–109ms，不能說任意heap／TLS／dlopen副作用已還原 |
| 命名／identity／ZIP | identity及命名／縮字policy exit0；canary identity7通過；ZIP正例2、負例28、SHA向量2通過 |

以上並未對遊戲故意製造故障，也未證明實際JNI函式／linker任意副作用可取消。沒有把native測試可執行檔的編譯與本輪真正執行混為一談。

私人receipt SHA256（原件位於ignored `.tools/lab/813374f61b8e4e318205c55ae865ccdd/`）：

| Receipt | SHA256 |
|---|---|
| supervisor.txt | 2af353077155220a8d6ac3769b8b0dac9caa3717f8b4ce87ed5db9e1f3672ad6 |
| load_journal_test.txt | 895dd0d47bde3cdeb9d50647ea8af47eae8ea375bd85820b7ac1ca69208278af |
| control_transaction_test.txt | 3c133c5e68d27fc962e628b567086f52f27ac3fd88cb472b356b86e5e65c5fe8 |
| kitty_syscall_test.txt | 09ae35e69a77d57eee9c748981cbad0e543c3bebaa240840168b873fdaef1210 |
| x64_call_context_test.txt | be89f2f68038501516ddbe59d6c6da902f347544f00d57cb162b157fb5a43c1d |
| platform-exports-api31.jsonl | 9974589c031c33ab38a4994c9fd44f5b5ea26ac7b98061675edfc06d3f584d8b |

結束確認無研究／遊戲套件及fixture程序殘留，SELinux仍Enforcing；已正常恢復non-root adbd並只關閉這台AVD。AVD資料與fixture保留，後續需新鮮核對新bootID；本輪沒有產品OS重啟或長時間驗收。

## API31介面差異與隔離來源開發

唯讀取得新AVD當次zygote64**已映射**的四個平台library，pull前後比對SHA，再以NDK llvm-readelf查export。原二進位與原始maps、receipt只留忽略的`.tools/lab/`。

| Library SHA256 | 查核結果 |
|---|---|
| libart `d667272152a4a5b80c37744ef58bc55daba63fbd94e70366d8e763da4431febf` | 有JNI_GetCreatedJavaVMs |
| libnativeloader `93b2f0eec926fa165cbb10ac3158ec32f6c246bd33cde13d17c45e46c2f5e062` | 有opaque/raw ClassLoader lookup、OpenNativeLibraryInNamespace、free-error；**沒有IsNamespaceNativeBridged** |
| libnativebridge `6edbcac34e1604bde7b199a4e0c9f38a1fe1285d0af6968726f532308f4a72f2` | 有NativeBridgeGetTrampoline，**沒有NativeBridgeGetTrampoline2** |
| libndk_translation `c05f2b7ef51d3c855576ae53c25f7511d0490695ca1bf6b16269627be6dbdb51` | 有NativeBridgeItf；export存在不證明App namespace可用 |

核對Android12固定tag的[AOSP native loader實作](https://android.googlesource.com/platform/art/+/refs/tags/android-12.0.0_r1/libnativeloader/native_loader.cpp)與[header](https://android.googlesource.com/platform/art/+/refs/tags/android-12.0.0_r1/libnativeloader/include/nativeloader/native_loader.h)，以及[NativeBridge header](https://android.googlesource.com/platform/art/+/refs/tags/android-12.0.0_r1/libnativebridge/include/nativebridge/native_bridge.h)。函式的契約與本輪export相符，但不是本ROM所有實作都與tag逐byte相同的證明。

新增`research/canary/api31_platform.h`及獨立`seedprobe_api31_host` target：

- 固定runtime API31／x64／自有probe包名，仍由真Application/ClassLoader取得opaque namespace；不讀ART或namespace私有layout，不猜pointer、不建namespace、不force-load平台SO。
- 以opaque lookup非空、raw-native lookup為空取得**候選**。這還不是成功；實際OpenNativeLibraryInNamespace必須返回handle且needs_native_bridge=true，才允許trampoline。
- 使用API31四參數NativeBridgeGetTrampoline；不把API35的五參數函式強轉為舊ABI，也不把缺symbol略過後當成功。
- receipt在真正load確認前標為namespace pair候選，不冒稱REAL_BRIDGED_NAMESPACE；正常JNI／四值／負例／生命週期仍需完整實測。
- API35研究host仍用原adapter；產品`app/bridge_platform.h`、Java/native x64/API35/libnb守門完全未放寬。

兩個研究host編譯exit0：API31候選SHA256 `1d60435404edc5baa51f49f2c09b74f40195a00b9cd56e9cfc1015dda57a220a`；API35研究回歸host `1668a2dede8b4aec2891ba2dcc9b6bb0d971391707827b9be27a28f5462d3383`。**新API31 host尚未載入任何App**，不是JNI或產品驗收。

`inspect-native-loader.ps1`可明確選libndk_translation／libnb並包含ART檢查，同時列出API31舊export及API35新export；沒有自動選後端或修改裝置。

## 尚需材料與下一步

下一階段是真正研究App的ARM fixture、ClassLoader canary、錯誤key／symbol／library、attach/detach、正常FINISH、新程序重跑與逾時收容，之後才評估產品整合。Enforcing環境的載入路徑需另驗，不能拿原Permissive ROM的tmp staging效果直接推論。

原簽章keystore／密碼環境變數及合法遊戲splits尚未指定。已詢問是否可為**全新AVD研究包**建立獨立研究簽章，或由使用者提供原簽章來源；未取得答覆前不建key、不簽署／安裝研究APK。產品仍必須原簽章、新版本/code，不能用研究key冒充升級。

目前不需要使用者登入。待具體合法遊戲安裝條件完成後才通知使用者親自登入；不操作或複製任何正式帳號／掃描器資料。
