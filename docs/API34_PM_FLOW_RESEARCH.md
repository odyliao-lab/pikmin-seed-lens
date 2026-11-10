# API34 自有 App PM／JNI 研究

## 實際普通 App 完整鏈與失敗收容（2026-11-11）

以下為來源準備後的新實跑；後面的「尚未安裝／執行」只描述部署前 checkpoint。
同一獲准私人 Magisk31.1 AVD 單次冷啟動，實際 boot
`1452fea3-a892-4569-b6fe-b9e9e1faf72d`／API34／x86_64／libndk_translation／Enforcing。
同 server526/start1547的兩次完整服務／顯示讀值相隔73.671874秒。原26份 signal、
三份 Root歷史及僅 UID10195的授權先核對，再正常安裝下方新研究 APK；沒有清資料、
新授權其他 App、安裝產品或啟動遊戲。probe UID10197、carrier UID10198，client仍10195；
PM抽出的 host／runner SHA逐一吻合本批。

不帶 scope、Root-only附load_pid、PM scope缺target三例在 Root query前拒絕，無新收據。
同新client／新boot的獨立普通 App PID5094先取得 PATH su-M／UID0／magisk domain／
PID1 mount view／同boot確認；這些結果沒有承接舊boot的資格。

| 新自有 probe | 普通 client發起的實際結果 | 完成證據 |
|---|---|---|
| PID5507/start28718 | 錯 PM path6、stale start5，唯一正常載入0 | 真 bridged load、ARM JNI四值、錯package/version/key/library/symbol拒絕、單worker attach/detach；完整七階段journal、90秒Java FINISH PASS與程序消失 |
| PID6241/start48431 | 另程序故意timeout，正常client返回42 | durable CALLING後 END UNKNOWN；精確pidfd收容、worker回收及probe／instrumentation退出，未重送 |
| PID6922/start57226 | timeout後另一新程序，同兩負例及唯一正常載入0 | 同樣namespace／JNI／worker、journal、90秒FINISH PASS與程序退出；前兩輪所有收據SHA不變 |

上述九次 PM命令均由普通 client UID10195／ELF62自行經 PATH su-M執行自身 PM runner。
adbd Root只做證據讀取，沒有代為發出載入命令；每段診斷後實際恢復 UID2000。
前兩輪正常／timeout各先獨立匯出原始收據並記SHA，回歸後逐份重核，不以新的成功
覆寫 UNKNOWN。最終保留13份Root、9份PM、5份fixture／namespace及3份native journal，
共30份自有證據；原26份signal SHA亦不變。native正常journal仍為 RETURNED_UNVERIFIED，
自有鏈通過是另核JNI與Java的分項結論，沒有把journal改寫為成功。

| 原始收據 | SHA256 |
|---|---|
| 首輪正常client PM | `573deb4a8e63a5a4ba629d289de5698ba5f59a2c01d75044d0b9e1040f413fc8` |
| 首輪正常journal | `0568663871e5dbf3c599ff7dfb7d04174d5fb4b44347a3427dfef7121dce634e` |
| timeout client PM | `b52b21b252c33af46fdb1f1fa033e65c434e1164c6654ef41061e8130285934b` |
| timeout UNKNOWN journal | `679c6ef7ba4fe26045a6ac399c033b9b22e8b4a9793efe84dd075cc5f6390833` |
| 回歸client PM | `00ad7396f8bd98e4f2e13d092ec7b341469bff9d25f4bcf20684e994a83f46e7` |
| 回歸journal | `00069d9819237b99f622c40ccbef8c8b7c292829df2a12723f4b7db486fe038e` |

**本次只通過自有普通 App → Root → PM → ARM JNI與失敗收容。** 自建thread不是遊戲
入口；沒有三版遊戲兩頁或原生ARM64資格，既有原遊戲SEVL啟動失敗沒有改記成功。
產品0.5.2位元組及所有NativeBridge／精確遊戲守門未改，研究件不變成消費者依賴。

結束前同boot／server526/start1547、UID2000／Enforcing及無Game／probe／client存活
再核對後，僅發出一次正常console關機。後續實讀SDK程序及5582/5583 listener消失，
SDK四原映像及私人ramdisk SHA不變，index5仍雙false。Zygisk0、空modules、原157
base／split SHA及產品未安裝狀態保留；原四台與台中未操作。

## 本次來源與準備

既有 API34 x64 AVD 已有[普通 App Root 分項通過](API34_MAGISK_RESEARCH.md)，
不是三版原遊戲相容。以下新增工具仍只用於同一已獲准隔離目標；不改產品、
Root provider、SELinux、SDK 映像或 NativeBridge library，研究 APK 不成為產品依賴。

2026-11-11 直接讀取 Android14 固定 tag 的官方
[native bridge header](https://android.googlesource.com/platform/art/+/android-14.0.0_r1/libnativebridge/include/nativebridge/native_bridge.h)
與 [native loader header](https://android.googlesource.com/platform/art/+/android-14.0.0_r1/libnativeloader/include/nativeloader/native_loader.h)：
本介面的 trampoline 為四參數，不能套用 API35 五參數函式型別。原始 header SHA256
分別為 `606511daee88e25d90c136203348293bdf29a7201bd5ba22cdae1f2824650a88` 與
`16af58a72fb817a689c6d8d09dcb9896c46464ab25e1dee48450d82c2ae2510b`。
這是原始碼契約，尚不證明這台 guest 真 mapped library、namespace 或 JNI 成功。

獨立 `api34_platform.h` 沿用已驗 API31 的 opaque/raw ClassLoader namespace 成對查詢
及四參數介面，但在任何 ART lookup 前先要求實際 API34、完整 SDK 文字34與精確
libndk_translation.so。成對查詢只記候選，實際載入必須回報 bridged=true 才記確認，
trampoline 只能使用剛核對的 handle。沒有 private namespace layout、force-load、
五參數強轉、失敗後 fallback 或遊戲指令修補。

研究 client 新增獨立 `api34_pm_research` 明確 scope，限普通 x64 App／ELF62與相符
packaged ABI，且必須有 load_pid。原預設 ProbePlatform 仍拒絕 API34 x64；
`api34_root_only` 仍拒絕任何 load_pid。PM 路徑、runner SHA、root-owned carrier path、
host SHA、自有 probe package／UID／PID/start、lease、journal／ACK、15秒／2秒與
pidfd 失敗收容均保留。native runner 另重核同一 SDK／bridge tuple，不能接受遊戲程序。

| 保留研究 artifact | SHA256 |
|---|---|
| 共用 ARM-only probe0.0.7/code7（重用原件） | `1d8d187188bcbf5822fbed6a75ea7fcedd71b503cd94fe7d5b7d07d337174110` |
| API34 carrier0.0.10/code10 | `57cc54605d3105d8d89aeecc5e1afc26ac94f01f6eee4a68e3c75104de1286ad` |
| API34 carrier host | `ab38340baa4af08bace80a4094de817e8a2cc874131b9ad7e4ec265078023201` |
| API34 PM App runner | `60449edbbc2e278a73e147141c73f47af4e70cb0b43b61cd7b560770f1d368ce` |
| Root client0.0.12/code12 | `551143a41e76cbe813b3d9552f63ca167623fcda03b48698a6d389ea738d60f3` |

原研究 signer633286a6…8870、Java/dex、x64 ELF64 PIE／shared object、native entry
stored／16KiB alignment與 runner ABI／SHA資產已核。94 ELF、77原 platform、38
Root-only、496新 scope／ABI host檢查與240 native tuple host檢查通過；不算裝置執行。
早先 carrier0.0.8建置 namespace 命名錯誤、0.0.9／client0.0.11的已建產物均保留；
後者在部署前補正「成對查詢仍未載入」收據標記，改交付上述更高版本，沒有覆寫。
原0.0.10 Root client與產品0.5.2/code17 SHA另核不變。

同一新建置目錄另編譯 API31／35原 host及 PM App runner皆通過，未覆寫舊交付件，
也沒有當作新裝置回歸。API34調度腳本 PowerShell語法解析通過。

## 執行門檻與尚未完成

此 checkpoint 尚未安裝／執行本次 carrier、client 或 PM runner。下一輪須先核同一
AVD的新 boot／system_server／完整服務／原 bridge、Root配置與原歷史，才安裝研究件。
`run-api34-probe.ps1` 僅接受 emulator-5582／SeedLens_API34_x64／34及明確 RootViaApp；
adbd Root只供證據讀取，真正載入必須由普通 App自行 su-M，沒有 shell loader模式。

先核自有 ARM fixture 四值，依序測錯 PM path及 stale start拒絕，然後同程序只發一次
正常載入。必須同時取得 namespace/JNI四值、錯 package/version/key/library/symbol拒絕、
單 worker attach/detach、完整 journal與90秒 Java FINISH／程序消失，不能只看 loader0。
另以新程序測故意 timeout／durable UNKNOWN／精確 pidfd收容，之後再用另一新程序回歸。
任何未知結果保留且不重送。未有實跑證據前，本鏈全部列待驗。

即使上述自有鏈通過，原遊戲 SEVL 啟動障礙仍要另外解決；API34 x64不因此取得產品
資格，API31／34三版遊戲、原生 ARM64及森林手帳 UI階段仍未完成。
