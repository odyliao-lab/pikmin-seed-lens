# MuMu API32後端前置驗證（2026-11-11）

## 最新整合：單一產品APK待種花苗通過

完成下述研究後，已交付原簽章0.7.2/code28。先移除三個研究APK，再以內建Root及
產品自身完成精確157的待種花苗ON／OFF／再ON，長名縮字及原名字級還原實測通過。
沒有額外Root管理器、控制中心或顯示模組依賴。詳見[產品獨立驗收](PRODUCT072_API32_STOCK.md)。
下方「尚未整合產品」與「旗標尚未進產品」保留研究checkpoint沿革。

## 研究checkpoint：內建Root及自有載入鏈通過，尚未整合產品

進一步讀取本機Root服務紀錄，確認先前su-c查詢不是已證明不可能，而是內建Settings
Root授權視窗等待後自動拒絕。完成自有研究App的內建授權後，同boot、普通UID10048、
adbd2000實際su-c取得UID0、同PID1 mount namespace及boot。沒有新增Root管理器／模組。
研究App19保留原lab簽章，新增ACCESS_SUPERUSER宣告及精確api32_pm_research範圍；
Root-only仍拒絕load_pid，一般ProbePlatform仍不納入API32，產品守門本階段不改。

對照Google AOSP [Android12L NativeBridge介面](https://android.googlesource.com/platform/art/+/android-12.1.0_r1/libnativebridge/include/nativebridge/native_bridge.h)
及[NativeLoader介面](https://android.googlesource.com/platform/art/+/android-12.1.0_r1/libnativeloader/include/nativeloader/native_loader.h)，
獨立API32／libnb adapter使用四參數trampoline；由真ClassLoader namespace、raw-native
查詢必須null及實際load的needs_native_bridge=true確認橋接，不讀私有ART結構或猜namespace。
同一adapter來源供後續整合，沒有把API35五參數介面硬套進API32。

MuMu的原生運算中子程序回報orig_rax=-254、-237等負標記，而阻塞read實測orig_rax=0、
rax=-512；不能只採原本的-1。新增僅API32/libnb研究旗標生效的停止狀態判別：非負
syscall號及Linux restart結果仍拒絕，另要求syscall-info的x64 arch／IP／SP與GPR相符；
最初擷取只接受NONE，自己完成且恢復的remote syscall後才接受EXIT，從不接受ENTRY。
自有fork程序15項正常返回、逾時、SIGILL、外部SIGUSR1及退出案例通過；返回案例完整
GPR／XSTATE／redzone逐字相同且heartbeat恢復，UNKNOWN不重試。旗標尚未進產品。

第一次自有載入因舊context守門拒絕；第二次因remote syscall EXIT處理仍不足而UNKNOWN
收容；兩筆均保留。第三次載入及JNI成功，但診斷切換adbd中斷了instrumentation啟動連線，
缺Java FINISH，仍記未通過。修正研究driver保持adbd2000全程不重啟；診斷只用既有su-c
唯讀回讀，自有UID10048親自執行PM loader，沒有以shell代發載入。

| 同boot的新自有程序 | 獨立結果 |
|---|---|
| PID8129/start205510 | exit0；真namespace、ARM JNI四值、package/version/key/library/symbol負例、單worker、完整journal及90秒Java FINISH PASS／程序退出 |
| PID9314/start215767 | 故意在durable CALLING後逾時，exit42／UNKNOWN、精確target及worker收容；不重送 |
| PID10497/start222608 | 另一新程序exit0，完整JNI／負例／journal／Java FINISH與程序退出回歸通過 |

研究Root client19的ProcessElf94、ProbePlatform77、RootOnly48、API34 scope496及
API32 scope496 JVM檢查通過。host carrier12、Root client19及研究fixture7僅供上述隔離驗證，
不會成為產品安裝依賴；後續交付仍必須原簽章、遞增版本、單一APK，且要另外驗遊戲待種
花苗ON／OFF。**目前通過的是自有後端鏈，還不是產品功能通過。**

下方保留最初失敗階段；「尚未通過」描述當時checkpoint，後續進度以上段為準。


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
