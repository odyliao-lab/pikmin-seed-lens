# 竹北隔離研究：API31真App與API35內建Root

2026-11-09新增實測；取代早期「研究簽章／index5待確認、API31真JNI未驗」的當前狀態。**四個遊戲組合全部仍未驗收**。未交付新產品APK，產品x64仍限原API35/libnb資格。

## 授權與研究產物

使用者明確同意建立repo外獨立lab key，以及空白MuMu index5 Android15（2vCPU/4GB、內建Root）隔離研究。原四台與台中Libra/Virgo沒有guest操作，沒有複製scanner、憑證或登入資料。

研究key保存在`D:\SeedLensLab\signing`，密碼採Windows DPAPI、目錄限制ACL；不進Git。研究憑證SHA256 `633286a6f46aae1d6b4bcf26ed2278e72c73bde5046cfa02b7f69fd347ac8870`，與原產品憑證不同，不得用於產品升級。

| 已簽署研究包 | 版本 | APK SHA256 |
|---|---|---|
| `dev.ody.seedlensprobe`，ARM64 fixture/canary | 0.0.6/code6 | `4cbba48567cb6b159a0fa08225d9590d813f01a2d4cdafebdbe4c4b974d81f19` |
| `dev.ody.seedlenshostcarrier`，PM x64 host、無Activity | 0.0.1/code1 | `db1e648998445a62b3a25ca9a4364771a4ecf809d28c97274d7070db6b7f1a33` |
| `dev.ody.seedlensrootprobe`，正常App Root唯讀instrumentation | 0.0.1/code1 | `ffdd8bc738a48d36abdaee90d5fb72f466439645bdff9b785de618c1e7c34c8e` |

前兩包僅裝在API31 AVD，第三包僅裝在新API35 index5。APK與原始收據留ignored `.tools/`。不同研究交付bytes仍須新版本/code。

## API31／Enforcing：實際ClassLoader與ARM JNI

明確核對同一`SeedLens_API31_x64`／emulator-5580的當次boot、API31、libndk_translation、Enforcing及檔案SHA，以原廠adb root執行研究runner。host從PM路徑載入，沒有tmp SO fallback或SELinux變更。

- 真App opaque/raw ClassLoader namespace成對查詢、load返回needs_native_bridge=true、四參數trampoline可用。ARM canary四值、錯套件／versionName/code、錯key、缺library/symbol、JNI worker singleton及attach/detach通過。
- 正常程序完整七階段journal及90秒FINISH PASS通過；修正收容後另一新程序再次正常完成，未沿用舊PID/start或重送舊attempt。
- 第一個手動排程程序錯過自建30秒thread期限，於CALLING前拒絕，保留其UNKNOWN；沒有放寬thread期限。新增`research/canary/run-api31-probe.ps1`核對AVD/boot及APK/runner SHA，等待自有baseline後立即只呼叫一次，驗journal、JNI與FINISH，不自動重試。

這些成功使用adbd Root，**正常App Root仍未合格**。stock su不支援-M，普通UID不能執行；尚未把API31整合到產品。

## 真實故障與共用supervisor修正

第一個真App timeout回報exit43 `UNKNOWN_CONTAINMENT_UNCONFIRMED_DO_NOT_RETRY`。雖然稍後程序消失，仍保留當次未確認結果，沒有追改為成功或重送。

新的多執行緒fixture重現原因：worker ptrace非leader thread，target群組退出可能等待tracer釋放；舊實作先等target pidfd、再回收worker，會錯過有限確認時間。原單執行緒fixture無法重現。

`app/supervised_load.h`現在在durable CALLING後才對精確pidfd發SIGKILL，先有界回收自己fork的worker，再有界觀察target pidfd的POLLIN。signal發送成功本身不算收容；無數字target PID fallback、不做遠端清理／rollback，未確認仍失敗並保留UNKNOWN。正常RETURNED與ACK流程不變。

新增三例真ptrace非leader測試（共10例）：CALLING後timeout、CALLING後worker退出、CALLING前worker退出且target繼續存活。舊實作在多執行緒timeout案例失敗，修正後10例於**API31及新API35均實際exit0**。兩平台使用同一fixture ELF，SHA256 `bb160d8f8b90c83abdd75eea9ad9b47713e0d6d71f5cedb683f5e52e951e1266`。

修正後API31新自有App故意timeout：exit42、durable CALLING、END UNKNOWN、worker回收及精確target pidfd退出均確認；再於另一新程序完成全部JNI與正常FINISH。舊UNKNOWN未清除／升格。最終研究runner SHA256 `d75bfba0ae09165fa44afc771dcdeb50813e71b82b76e963663cb8df8c0af938`。

私人收據SHA256：

| 收據 | SHA256 |
|---|---|
| supervisor舊多執行緒失敗 | `6609bd7da4401e363cbbd77c42b1c321403c356e047272a9e065fa09d57685c0` |
| 修正後API31十例 | `af8673dbedca3dc888575676d9c225a44d9f458e37e3fb4804908f008703b799` |
| 修正後API35十例 | `5df8aaf931ca0d693f6fabff601df9783796f1902bf9e1674cc8b2fa8545faa0` |
| 新API35正常App Root | `9e5b63c2070f3e33f6802c3eeac8dcca373f078cfa689370af8fb9fad5816dee` |

API31 timeout及最終正常結果分別留在`.tools/lab/api31-run-a9739295d43b4010b48fe01eb75675d4/`與`api31-run-c52927736e7a4416aca2182fb9ee310c/`；原失敗及全部guest attempt保留。結束時API31 Enforcing不變、無probe程序、adbd恢復non-root並正常關閉AVD。不是產品OS重啟／長時間驗收。

## 新API35隔離機與正常App Root

由MuMu CLI現查index5名稱`SeedLens-Isolated-A15`及loopback端點後才ADB操作。實際Android15/API35、x86_64/arm64-v8a/x86、libnb.so、kernel6.1.90-perf+；原廠Permissive，未修改。內建Root設定生效後正常重啟一次再核boot；不當成遊戲重啟驗收。

Root provider是內建KernelSU管理器v3.2.5（32525-2），未另裝Magisk。僅在管理器授權`dev.ody.seedlensrootprobe`（UID10052），未全域授權、未安裝模組或更新provider。

真正正常App UID以固定唯讀`su -M -c`實際exit0、uid0、u:r:ksu:s0、本次boot吻合；自身與PID1 mount namespace相同。這證明此環境正常App Root契約，**不是JNI／遊戲／產品顯示通過**。同環境另實際執行前述10例自有child supervisor通過。

## 建置與剩餘工作

共用修正後完整unsigned candidate通過Compatibility26、InstalledNative29、GameSession27、APKLayout8、PairedDigest4；雙ABI native、ARM renderer、Java/dex/16KB封裝完成。native執行檔編譯不當成ARM64裝置實跑。Candidate SHA256 `362b25dc25333d521148d9380a3701d1465279eaf4a799d5a57e5022fb0abdb7`，未簽署、安裝或交付，不覆寫原0.3.1 APK。

下一步為新API35真App canary回歸、API31正常App Root及產品後端整合。已另提出僅對API31 AVD使用官方Magisk Root provider／私人ramdisk override、保留Enforcing及原SDK的範圍，**尚待回覆**；僅下載官方材料及複製原ramdisk準備，未patch／安裝／啟動。lab key及index5授權不推論為同意改AVD Root。

合法原簽章遊戲156.0／156.1完整splits及產品原簽章來源仍未提供。尚無遊戲登入條件，需登入時通知使用者完成；不複製其他instance帳號。四組真實兩頁、縮字、捲動、OFF及遊戲重啟全部待驗；原生ARM64/API31亦尚無確認目標。
