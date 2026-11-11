# MuMu API32後端前置驗證（2026-11-11）

使用者明確要求確認API32後端，通過才開放產品；本次遊戲功能範圍僅待種花苗ON／OFF，
探險略過。只操作既有index4 `SeedLens-Isolated-A12`／created1794178164127859，
端點由CLI重新核對為127.0.0.1:16512；原四台、index5、官方AVD及台中設備未操作。

## 普通App Root研究

開啟該機既有`root_permission`並正常重啟；保留帳號／遊戲資料，不安裝Root或顯示模組，
不改原Permissive。新boot `31bef082-9840-44b4-b3e2-4689765855da`，實際API32、
x86_64／libnb.so；uname雖報aarch64，研究App自己的ELF確認EM_X86_64=62，沒有
當作原生ARM64。普通App UID10048發起查詢期間adbd為UID2000。

Root probe增加明確`api32_root_only`範圍，只允許上述API／ABI／kernel／實際ELF，
拒絕load_pid及無scope；不開放PM runner或遊戲目標。使用既有研究簽章，研究件
0.0.14、0.0.15、0.0.16各自保留新版本與原件；產品仍是原簽章0.7.0/code26，未重建。
既有封裝的API34 runner僅供沿用研究封裝格式，API32 root-only不可執行它。

| 普通App獨立查詢 | 結果 |
|---|---|
| 0.0.14：PATH su -M -c | exit2，實際回報`su: invalid option -- M`，Root未通過 |
| 0.0.15：僅API32 root-only可選PATH su -c | 10秒內未完成，收容只讀子程序，Root未通過 |
| 0.0.16：明確關閉查詢stdin後再測su -c | 仍未完成；SU_C_EXIT=-1、ROOT_CAPABILITY_NOT_PROVEN |
| 0.0.16：無scope／指定load_pid=1 | 各自instrumentation1拒絕，沒有新Root收據／PM載入 |

每個正例嘗試都只讀UID、掛載namespace及boot，沒有native操作；逾時只收容此查詢，
不涉及不確定loader重送。查詢未取得UID0／PID1同mount view的完整證據，不能因
host的Root選項已開、su存在或adb root可用而宣稱普通App Root通過。

研究App三份收據均複製至私人證據目錄並逐份SHA保留；沒有刪除UNKNOWN或新增native
attempt。adb root只用於回讀研究收據／平台maps，結束逐次確認恢復UID2000；
它不是普通App Root成功的替代證據。未見rootprobe或su殘留程序，遊戲未載入透視鏡。

## NativeBridge原件唯讀核對

對新目標實際映射的兩個平台原件只讀取、核對遠端及本機SHA，未替換系統檔。

| 原件 | SHA256 | 本機ELF匯出實測 |
|---|---|---|
| libnativebridge.so | `1ce757b34548a939d3f8bed53565703e77ae821dae1919ebf1b1a0d344a61ff3` | 有NativeBridgeGetTrampoline／Initialized，無現有API35使用的GetTrampoline2 |
| libnativeloader.so | `8cd324dc8769bed697d97291d6bc3a472c57df9c797f3029524f77af250ca8a7` | 有兩個ClassLoader namespace查詢、OpenNativeLibraryInNamespace及FreeErrorMessage，無IsNamespaceNativeBridged |

因此不能只放寬版本判斷便共用現有API35 adapter。這只是匯出清單，不是JNI、真namespace
載入或失敗收容通過；Root前置尚未通過，本次沒有向自有fixture或遊戲注入native。

## 結論與保留條件

**API32尚未通過，產品不開放。** 本次已實作並實跑Root前置的獨立研究及拒絕範圍，
但普通App Root路徑仍失敗，後續NativeBridge載入／收容和待種花苗ON／OFF皆未驗成。
下一步需先取得符合產品契約的普通App Root能力，再開發獨立API32 adapter、自有fixture
正常／失敗收容與新程序回歸，通過才整合遞增且原簽章的新產品APK。
不把API32當作必要的API31通過，不新增主機helper、控制中心或顯示模組依賴。

研究build通過ProcessElf94、ProbePlatform77、RootOnlyPlatform48、Api34PmScope496個
JVM檢查；這些是host測試，普通App結果另列上表。原始收據、平台ELF、日誌與APK只留
ignored私人目錄，不提交帳密或遊戲資料。
