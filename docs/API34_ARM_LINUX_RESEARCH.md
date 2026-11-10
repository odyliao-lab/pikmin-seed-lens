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

gdb1500／inferior1510，UTC03:10:11起以同kernel、mapper／vector、2vCPU／4GiB及私人
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

當時主機i7-10700KF／8C16T總CPU實讀100%；只讀自身QEMU thread統計，主要CPU時間在兩個
TCG thread。不能單憑CPU繁忙認定唯一根因。完整getprop觀察改為較低頻單boot flag，
避免監測增加負載；保留原40秒逾時與未完成觀察，不把bootanim／API讀值當App可用。
