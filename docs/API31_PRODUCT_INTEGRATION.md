# API31 native候選整合（2026-11-09）

## 最新：Java與native同一候選開關

普通App自行su-M／PM loader的正常、timeout及新程序回歸完成後，將Java准入接上既有native候選開關；仍只有`-Unsigned -Candidate -Api31Candidate`可以開啟API31。checked-in Java常數為false，建置只在獨立輸出目錄生成Java副本，與CMake使用同一參數；預設／可簽署路徑維持API35資格。API32、錯版本/code、錯bridge與其他精確守門不變。這是完整產品來源接線，不是遊戲能力通過。

兩種完整建置完成雙ABI native／ARM renderer、Java/dex、16KB封裝及29 InstalledNative、27 GameSession、8 APKLayout、4 PairedDigest檢查。Compatibility原完整建置各34項；加入編譯flag的測試參數後，對兩份實際產物各重跑35項通過。測試也刻意傳相反flag，兩份均拒絕；另以javap核對真正App classes為false／true。CMake cache及封裝x64 host marker均與同批Java一致，禁止混用候選與預設輸出。

| 新unsigned candidate，禁止安裝／交付 | SHA256 |
|---|---|
| Java＋native API31候選ON | `961d772b6aa0aba73d81952f5f35c1e167ee11b94d88075ebe75de8ef2778de3` |
| 預設API35資格、候選OFF | `b84bb4c57c260317b62023d748e8a1b903376ffa2c3d7138ee1fc6ef242a2a30` |

要求API31但未指定unsigned candidate時，仍在工具／簽章存取前實際exit1。未升產品版本、未以lab key簽產品、未安裝上述APK；既有0.3.1交付保持原件。四組遊戲驗收、原產品簽章及合法完整遊戲splits仍待補。後續須先完成新版本／原簽章封裝與實測，才能宣告支援。

以下保留先前native-only階段的歷史證據；其「Java仍拒絕API31」描述不代表上述明確unsigned候選。

> 後續使用者已授權API31 Root變更；Magisk31.1／Enforcing／Zygisk關閉後，普通App PATH su-M與Magisk-domain canary／收容已有實測，見[Root階段紀錄](API31_MAGISK_RESEARCH.md)。以下「Root尚缺」保留為候選整合當時狀態；正常產品完整載入與遊戲仍未驗。

此階段把已實測的隔離adapter接入單APK的native來源，**不宣告API31產品支援或四組遊戲通過**。API31正常App Root尚缺、合法遊戲splits與原產品簽章來源待提供；Java正式入口仍拒絕x64/API31及API32。

## 選擇與拒絕規則

`app/bridge_platform_api31.h`為原隔離API31 adapter的共用來源。四參數trampoline、opaque/raw ClassLoader pair、實際load的bridged旗標及handle限制保留；套件名稱由caller傳入後與真正Application比較，產品傳精確遊戲包名，研究runner仍只接受自有probe。版本、digest、prologue及runtime metadata守門不變。

`bridge_policy.h`與`bridge_runtime.h`共同核對native runtime API、完整SDK property文字及精確bridge名稱：

| 組合 | 預設建置 | 明確API31未簽名候選 |
|---|---|---|
| API35 / libnb.so | 原後端 | 原後端 |
| API31 / libndk_translation.so | 拒絕 | API31專用adapter |
| API31 / libnb.so | 拒絕 | 拒絕 |
| API35 / libndk_translation.so | 拒絕 | 拒絕 |
| API32、未知API／bridge、格式不符 | 拒絕 | 拒絕 |

`bridge_dispatch.h`在ART lookup前選定介面，不在失敗後試另一ABI或猜pointer；API35五參數與API31四參數trampoline維持不同型別。root loader與x64 worker也各自核相同runtime tuple，注入設定使用本次實際API，不再對候選API31傳固定35。

`-Api31Candidate`必須與`-Unsigned -Candidate`一起指定，否則在建置／簽章前拒絕。CMake預設OFF，建置腳本每次明確傳ON/OFF且分開native cache，防止上次候選設定滲入預設建置。候選不改Java准入，也不能安裝／交付；之後正常Root、完整產品整合及遊戲驗收具備條件時才推進。

## 本輪建置證據

兩種完整建置均通過Compatibility28（新增API31/32仍拒絕）、InstalledNative29、GameSession27、APKLayout8、PairedDigest4，完成雙ABI native／ARM renderer、Java/dex及16KB封裝。ARM64僅編譯，不當成原生ARM64裝置驗證。

| unsigned candidate，禁止安裝 | SHA256 |
|---|---|
| native API31候選ON，Java仍拒絕API31 | `74a102886d922d08432db840e511108ea06c7c26b8a9d792c92451ec0f0b5a9e` |
| 預設API35資格、候選OFF | `829efe218aa7e547370d2846cfa94c4047273656560a029e4e199888b04fafef` |

API31 AVD上實際執行兩個x64 policy fixture（候選OFF與ON），各51項通過，覆蓋明確版本／bridge交叉負例與編譯設定。這是准入policy執行，不是51次平台或遊戲能力驗證。ARM64兩種fixture僅編譯。

另核對兩個實際candidate APK的SHA、CMake OFF/ON值及APK內x64 host的API31 adapter marker：預設不含、明確候選才含，防止cache或封裝外洩。這不是裝置能力驗收。帶`-Api31Candidate`但未指定unsigned candidate時實際exit1，沒有進簽署或產生產品交付。

研究carrier0.0.3先驗共用adapter來源，真App完整JNI／負例／正常FINISH通過。接著carrier0.0.4/API31及0.0.5/API35改用與產品相同的runtime dispatcher；每次新APK均升版本/code、同lab簽章，原產物及attempt不覆寫。probe持續重用原0.0.6的同SHA APK，研究建置先驗其簽章／版本及ARM fixture/canary與來源建置bytes相同，否則要求新probe版本。

原始收據與APK留ignored `.tools/`。裝置證據以後續實際結果補入本頁，不把建置或安裝當通過。

## API31共用dispatcher實際結果

同一已授權AVD的新boot，確認API31/libndk_translation/Enforcing、既有probe SHA及PM carrier／runner SHA後，carrier0.0.4使用產品候選相同的`PlatformForRuntime`進入真App。套件／name/code／key／缺library/symbol負例、真ClassLoader pair、load bridged旗標、ARM JNI四值、singleton、七階段journal與90秒FINISH全部通過。只載入自有probe，沒有遊戲或Root provider變更。

| Artifact／私人收據 | SHA256 |
|---|---|
| 過渡carrier0.0.3，共用adapter | `28c6bf0edfba92e0dee95b0a34a874cfa388e082a24191fd2ec4a003491553a5` |
| carrier0.0.4，共用dispatcher | `1897f29295a32320a88f4ab3ac45f59715af14608824522a289ef3651ac67dc1` |
| API31 dispatcher host | `6cc16cd9cd8f80352610aa0cf6f48ff3faff1132c86eb59c7888c5173d84874e` |
| API31 dispatcher runner | `f8d6b5ce84fee2b21c0a93a15bf9480dce5e3ee383f43cf785046593e91485f1` |
| API31 namespace收據 | `92699143aea045d717e266a7fdc5568aedefa030a5f4c679a4460a8e25b810ad` |
| API31 FINISH收據 | `295ea68648c03986464180594c52e305d5bcb2db8a2adde71b42c9cedc527cae` |

本次dispatcher收據位於`.tools/lab/api31-run-b549ed10aaf94dfeb19f9a2da856b1bb/`。結束核對前次boot五份attempt的SHA均與前輪相同（含拒絕與未確認UNKNOWN），新增兩筆正常收據不取代它們。Enforcing不變，無probe程序，恢復non-root adbd後正常關閉AVD。

這仍不證明正常App Root、遊戲實際thread／renderer／兩頁或原生ARM64。產品候選的完整遊戲驗收待原簽章與遊戲材料，不能用這個JNI結果直接打開Java准入。

## API35共用dispatcher回歸

API31正常關閉後，才重新啟動已授權MuMu index5；CLI現查名稱／端點與新boot、API35/libnb/原Permissive。正常App Root probe在新boot再次su -M通過，之後才以研究adbd Root驗PM canary。沒有改provider、SELinux或授權其他App。

carrier0.0.5使用產品預設相同的`PlatformForRuntime`（API31候選OFF），在同一probe0.0.6完成真namespace、ARM JNI四值與全部負例／singleton、完整journal、90秒FINISH及程序退出。新來源沒有只回歸API31而省略API35。

| Artifact／私人收據 | SHA256 |
|---|---|
| carrier0.0.5 | `ed300b142dc4cfb3a008155cbf3739f2987f4c786c6199821cc63a7be3484ae8` |
| API35 dispatcher host | `74890be24c5b146ce738ad967d261a8632f9e2b12d91c442c9590e8f47805249` |
| API35 dispatcher runner | `cc9ab1ff5d006ef858a81cb62c141dd512e17545151e893bcc1d161c96aed3df` |
| API35 namespace | `4e6eb467717d07cec7e6c8fbf2d8759ab593e33653e068538daf6fbeb8fd89ad` |
| API35 FINISH | `af6cb586dbc58a01e5325382970ad740ea13a8f98bd8b3653d0a1a96a571c3ad` |

回歸原件`.tools/lab/api35-run-067557d5ec2440ee97335bd10b715e09/`；新boot Root及結束收據在`.tools/lab/api35-dispatch-a26ed223f5d34a91910d0caa78405999/`。逐字比對前次boot的timeout UNKNOWN仍與原收據相同，無研究probe存活後只正常關閉index5。兩環境資料、研究包與所有舊attempt保留；原四台及台中裝置未操作。

目前仍待API31 AVD具體Root provider方案回覆、合法156.0／156.1完整splits，以及原產品簽章來源。沒有登入或新產品APK交付；四組遊戲兩頁／縮字／捲動／OFF／重啟仍全未驗。待條件具備，先驗正常App Root與候選封裝，再分配新產品版本及原簽章單APK進行真正UI驗收。
