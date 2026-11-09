# 驗證分層

## 2026-11-09：ARM64後端自有initramfs CPU比較

新index5完成0.5.0／156.1回歸並OFF、正常關閉後，同已核准空白ARM64研究AVD使用私人自有initramfs override，官方SDK ramdisk SHA未變。單獨SEVL／plain與tagged store等5項實跑通過；加入SDK fault附近向量序列後，V2實際5 pass／2 SIGSEGV fail／0 skip，父程序收容後正常power-off。只改`-singlestep`的同V2原件實際7 pass／0 fail／0 skip，正常power-off且host／兩埠消失。不是Android App或產品資格，也未測試故意alarm逾時；後續官方原ramdisk完整Android開機比較仍待實際API／ADB／App證據。來源與產物SHA見[CPU研究](API31_ARM64_CPU_MEMORY_RESEARCH.md)。

## 2026-11-09：同0.5.0／156.1同帳號OS回歸

正常OFF並關閉再啟動同index5，新boot984fe73b-9a82-41cb-abdd-f05bdbb8de18、實際API35／libnb／原Permissive、精確156.1及同0.5.0 APK SHA再次核對，登入保留。原24份attempt SHA不變；新PID1967/start4003／UID10066實際拒絕未確認啟用與舊status，新同意後才完成本程序完整journal。實際兩頁名稱、庫存紀念長字縮小／首排離屏返回、探險末排真正離屏返回及正常App OFF原文／字型還原已觀察，32/80。frame13為切頁動畫，不作穩定畫面證據；新公車苗名完整顯示不等於已確認長名縮字。最終control OFF／156.1／ready／enable0、178份讀到的thread TracerPid均0，adbd UID2000；原24份逐份SHA不變，新第25筆journal SHA442f1fcfa9dec5855e7beba3ed489da33a1bf7782c6298939a92e9c6ea3076bb。API35三版現各有同0.5.0首輪／遊戲重啟／OS分項；探險更長名稱縮字、長時間與API31／34仍未完成，見[本版156.x回歸](PRODUCT050_GAME156_REGRESSION.md)。

## 2026-11-09：同0.5.0／API35／156.1首輪與遊戲重啟

156.0正常OFF後，以合法完整原簽章base／ARM64 split正常升級精確156.1/code1793076318，保留本人登入。首程序3894/start48959完整新journal後，實際驗庫存名稱／紀念長字縮小及首排離屏返回、探險末排完全離屏返回與兩頁OFF，當次32/80。載入前既有探險卡片仍原名，重開列表後才更新；保留此限制，不以waiting-cards或ready替代所有卡片效果。

正常遊戲重開至6349/start125778，未重新確認／舊status分別實際拒絕；新同意後完成本程序完整journal，兩頁名稱、庫存縮字／離屏返回、探險末排真正離屏返回與兩頁OFF再驗，當次32/80。兩筆新journal SHA5e1a8fb973153d728188adf78c72644cac36550166248c3120663d10580fe2a0、7eefc34a864e3ad2057a24aeaf14b2f29aec24fc50ff6e8666db92db381afbea；各自原22／23份SHA不變，總24。最終control OFF／156.1／ready／enable0、178份讀到的thread TracerPid均0、adbd UID2000。同帳號OS回歸進行中；探險更長名稱縮字、長時間及API31／34仍未完成，見[本版156.x回歸](PRODUCT050_GAME156_REGRESSION.md)。

## 2026-11-09：0.5.0／156.0遊戲及OS重啟回歸

本人處理結算並授權關閉後續同類提示後，同API35新遊戲PID7631/start201500完成未確認／舊status實際拒絕、重新同意與本程序完整載入；兩頁名稱／庫存縮字、探險首排真正離屏返回及OFF還原再次觀察，當次30/80。再正常關閉／啟動同index5至新boot da15de1a-1d01-4e2d-8b57-153da7c53091，PID1900/start2779完成新同意與本程序完整載入、兩頁ON／OFF及探險末排真正離屏返回，當次31/80。OS frame05僅核對中，未將該截圖列為舊status負例通過。兩筆新journal SHA98f4630bb57868c0212d67a5e081acd7e6dbfa9a36389988d90ac0330ca7f3db、4bef34e136e20e9be001394f9c28e4929e1e354860014fc83e2f34c2edd7fe69；各自原20／21份SHA不變，總22。最終control OFF／156.0／ready／enable0、187份thread讀值TracerPid0、adbd UID2000。各階段庫存基準分開記錄，探險長名縮字／長時間仍未完成，見[156.x同APK回歸](PRODUCT050_GAME156_REGRESSION.md)。

## 2026-11-09：同0.5.0／API35／156.0首輪回歸

本人完成156.0登入後，重新核對API35／libnb、精確156.0/code1792142832與同原簽章0.5.0；PID3804/start49157／UID10066由正常App確認並首次載入，完整七階段journal與新鮮同程序狀態。兩頁細分名稱、庫存長名縮字與首排完全離屏返回、探險首排完全離屏返回及正常App OFF原文／字型還原已實際比對，29/80不變。原19份attempt SHA不變，新增一筆35c3ed069eb16f8b47b1db0cefdcdfafd49330fcca8721d1bca7ffb9144f95e2，總20。最終ready／enable0／control OFF、209份讀到的thread TracerPid均0，adbd UID2000。正常遊戲重開至7631後遇本人既有種花結算，未載入新程序、待本人處理；重啟回歸與156.1本版UI尚未完成。探險更長名稱縮字、庫存最長名稱仍截斷的限制保留，見[同APK 156.x回歸](PRODUCT050_GAME156_REGRESSION.md)。

## 2026-11-09：0.5.0／API35／157同帳號OS重啟回歸

新index5先OFF並正常關閉，再啟動為新boot f41b9c04-8464-49bc-882e-5ea87def0bfc，同原簽章0.5.0 SHA、精確157.0與本人帳號。18份歷史SHA不變；新PID1948/start8264的未確認／舊status實際拒絕，重新同意後完成新完整journal。兩頁細分名稱、庫存長字縮小、探險末排真正離屏返回與正常App OFF原文／字型還原再次觀察；最終ready／enable0／control OFF、165份thread讀值TracerPid0，adbd UID2000。新journal SHA8758bbf120f9611178cacbe6d120a00f87d49432b8a4673e6cec339718c6affb，原18份不變、總19。探險長名縮字與長時間仍未驗，不移作其他組合通過；見[0.5.0紀錄](PRODUCT050_QUALIFICATION.md)。

## 2026-11-09：157原遊戲在API31／34獨立啟動失敗

完成API35分項回歸後依序關閉／啟動隔離機，實際API31／34的原簽章157.0各自正常更新、安裝後完整SHA相符，首遊戲程序分別4254／4305；兩者translator均回報原Unity+0x5e2760的SEVL未實作並SIGILL，發生在產品載入前。API31原16份研究attempt SHA不變／無產品attempt；API34沒有安裝Root或產品，資格拒絕保留。API34首輪hostGL boot無回應、指定正常關閉逾時後精確host收容，未啟動157；後輪SwiftShader才取得上述實際結果並正常關閉。兩平台SDK ramdisk SHA不變，原件與界線見[157平台啟動](GAME157_PLATFORM_STARTUP.md)。

## 2026-11-09：157.0原簽章來源、未知拒絕與0.5.0

新index5的Play當時未提供157更新；後續使用者指定AutoFarm已安裝版本，限定唯讀取得原簽章base／完整ARM64 split，核實157.0/code1793360009。未操作來源UI／遊戲／scanner／憑證或Root狀態，其餘原instance及台中未操作。0.4.1在新index5實跑未知版本拒絕，control OFF／無Lens mapping／無新attempt，16份歷史SHA不變，見[來源界線](GAME157_OFFICIAL_INTAKE.md)。

新增精確157 profile，三版完整來源重驗3 ELF／15 hook／3 veneer／14同布局／39破壞負例，Python16項通過。0.5.0/code15保留原簽章、新SHA與舊APK；預設與API31 qualification完整建置48／29／27／8／4檢查通過，API34 x64仍拒絕。新API35自有control fixture76項實跑通過，沒有改動遊戲或16份歷史紀錄。

0.5.0只安裝新index5。使用者本人處理157獎勵後，正常App對PID2033/start22615首次載入，完整七階段journal與新鮮157.0 status；實際兩頁細分名稱、花苗長字縮小／離屏返回、探險末排冰色三葉花苗完全離屏後返回同名、兩頁正常App OFF原文／字型還原已觀察。最長紀念名稱縮字後仍截斷，探險長字縮小尚未觸發；正常遊戲重開至PID5504/start188935後，未確認及舊status實際拒絕；新同意／新完整journal後再驗兩頁ON／OFF、紀念長字縮小與探險末排完全離屏返回。OS／長時間與其他平台未驗。原16份SHA不變，兩筆新完整journal保留，總數18；最後control OFF／157.0／ready／enable0、讀到的thread TracerPid均0，adbd UID2000。詳見[0.5.0分項驗收](PRODUCT050_QUALIFICATION.md)。

API31 ARM64 virt比較已到kernel／init，但keystore2反覆SIGSEGV、ADB offline，無實際API或App就緒證據；已正常關閉，未安裝Root／遊戲／產品，詳見[後端研究](API31_ARM64_BACKEND_RESEARCH.md)。下列較早階段材料不足與未執行敘述保留當時狀態，不覆寫最新結果。

## 2026-11-09：API35／156.1樣本兩頁、離屏返回與遊戲重啟

本人完成全部後續獎勵並回主畫面後，重新核對同API35／libnb／精確156.1/code1793076318／同原簽章0.4.1。PID10717/start267134具有完整持久journal與新鮮status，正常App重新同意再啟用；沒有追加attempt或重送載入。實際兩頁名稱、探險首排花苗完全離屏後返回同名、正常App OFF原名／字型還原均已比對。

OFF後正常遊戲重啟至PID14356/start423650。未重新勾選啟用、舊status分別實際拒絕；確認主畫面並重新同意後完成新完整journal，兩頁名稱／庫存縮字、探險真正離屏返回與兩頁OFF再次觀察。結束同PID／156.1／ready／enable0、control OFF，adbd UID2000；先前15份attempt（含UNKNOWN）逐份SHA不變，新增一筆後總數16。實際frame及journal SHA見[樣本回歸](API35_SEED_EXPEDITION_SAMPLE.md)。

156.0與156.1目前都已有探險離屏返回的畫面證據。**探險長名稱觸發縮字仍未覆蓋**；庫存最長紀念金苗名稱雖縮字，仍有省略號，保留此限制。OS重啟同帳號連續回歸、長時間及其餘Android／157仍未通過，不把本輪結果升格為九組完成。

## 2026-11-09：API35／156.0補齊未領回花苗離屏往返

使用者再次本人更換有花苗樣本的帳號。同原簽章0.4.1在API35／libnb的PID1974/start13925與正常重開後PID9010/start223817，從正常App確認並載入；實際觀察多類探險名稱、首排花苗完全捲出畫面後返回名稱仍相符、兩頁OFF原文／字型還原。新程序未確認與舊status實際拒絕後才重新同意。原12份歷史attempt（含UNKNOWN）SHA不變，兩筆新完整journal另存；結束control OFF／enable0／TracerPid0、adbd UID2000。

這次補的是156.0真正探險離屏返回的畫面證據。探險名稱目前都能完整容納，尚未覆蓋需要縮字的更長名稱；OS重啟同帳號前後與長時間仍未驗。本人處理既有種花結算／獎勵，沒有以領回或派遣製造測試樣本；帳號有自然狀態更新，不宣稱所有物品總量恆定。詳見[本輪樣本紀錄](API35_SEED_EXPEDITION_SAMPLE.md)。當時156.1已正常升級保留登入，首次載入完整但遇後續選獎勵彈窗，正常App OFF後待本人處理，尚不算156.1樣本畫面通過；後續本人處理後的結果見上節。

## 2026-11-09：新增157時的階段紀錄，必要範圍擴為九組

使用者補充157已釋出，要求加入功能驗證。最新必要範圍為Android12/API31、14/API34、15/API35 × 156.0、156.1、157；[九組矩陣](ANDROID12_15_GAME156_GOAL.md)是目前完成條件，以下較早的四組／六組敘述保留歷史範圍。

新增當時157尚未從官方來源取得精確Android versionName/code、完整splits、簽章、ELF／metadata，沒有profile或任何裝置通過結果。現有0.4.1只有156.0與156.1精確profile，未知版本維持原顯示；不得沿用舊偏移或放寬NativeBridge。完成既有156.x分項驗收後，才在已授權新index5從官方Play取得157，先驗未知版本拒絕，再進隔離研究與新版本原簽章產品；不同交付bytes必須新version/code，最終九組以同一APK逐組重驗。

| 157必要組合 | 目前狀態 |
|---|---|
| API31／157（精確code待取得） | 未取得、未安裝、未驗；156.x在既有x64 image的SEVL失敗仍保留，不能跨版推論 |
| API34／157（精確code待取得） | 未取得、未安裝、未驗；x64產品資格仍拒絕 |
| API35／157（精確code待取得） | 未取得、未安裝、未驗；沒有把API35／156.x部分結果算為157通過 |

目前新樣本帳號由使用者本人完成156.0登入，boot `072e4811-b253-460d-b471-a318cd4bf015`。既有12份產品／研究attempt在OS重開後SHA不變，control保持OFF；尚未在本次boot啟用產品，不能算OS重啟再啟用通過。探險列表實際捲到底目前只觀察到禮品與水果，未觀察到未領回花苗；已請本人確認花苗樣本，沒有點擊領回／派遣，也沒有把禮品的捲動算成花苗重用驗收。帳號與原始截圖只留ignored私人資料夾。

API34後續三次stock boot的圖形比較已完成：156.0在SwiftShader、156.1在host OpenGL分別於原Unity的SEVL SIGILL，均在產品載入前；取消首輪未完成安裝、確認恢復後才提交新156.1更新。未安裝Magisk或產品，詳見[API34研究](API34_ISOLATED_RESEARCH.md)。當時獲准的[API31完整ARM64 guest後端研究](API31_ARM64_BACKEND_RESEARCH.md)仍未boot成功；後續virt到達kernel／init但App未就緒，不能新增ARM64實機或遊戲通過欄。

## 2026-11-09：API35／156.0實際畫面與新增API34範圍

使用者確認156.0登入完成後，同一原簽章0.4.1／SHA `815de868e747111c9e9739ea9987337c53441e10a15e585d2192cecab4741cce`在新index5 API35／libnb分別以PID2274/start17271與重啟後PID6657/start317279完成正常App確認／載入、兩頁名稱ON／OFF、花苗長字縮小及捲動重用。新程序未確認啟用與舊status均實際拒絕；重新確認後兩頁再次ON／OFF。花苗數維持356，未領回／派遣或改物品。原UNKNOWN及七份研究紀錄SHA不變，新兩筆完整journal保留；adbd恢復non-root後正常關閉index5。

兩版API35目前都缺探險長名稱及真正離屏重用樣本，OS／長時間另列未驗。使用者後續要求Android12／14／15與兩版遊戲共六組；新API34 x64 AVD及私人Magisk31.1 Root研究已明確確認，官方image rev14已安裝並完成stock boot，實測API34／libndk_translation／Enforcing及自有ARM JNI baseline；後續能力研究中。產品x64的API34仍拒絕，須先驗平台與失敗收容；不能由API34歷史Libra ARM64結果推論新x64通過。詳見[六組條件](ANDROID12_15_GAME156_GOAL.md)及[0.4.1分項證據](PRODUCT041_QUALIFICATION.md)。

## 2026-11-09：原生ARM64 API31研究入口補強（尚未部署）

自有ARM64 JNI研究App升至0.0.2/code2、保留原簽章。載入fixture前核明確expected API／Java實際API、普通App UID、64-bit process／主要ARM64 ABI及Java平台kernel uname `aarch64`；載入後再核native實際API，才啟動thread。研究建置保留version目錄，拒絕覆寫。Java/dex／簽章／stored ARM64-only libraries／16KB封裝與配對digest已查；ARM64 root verifier、context、supervisor及platform test executables均編譯通過，未裝置執行。原產品0.4.1未變，新裝置操作範圍仍待指定，不能新增原生API31通過欄。SHA與後續條件見[ARM64研究準備](../research/arm64-jni/README.md)。

## 2026-11-09：API31原遊戲啟動缺口與自有ARM指令對照

官方API31 x64 AVD本輪cold boot仍為API31／libndk_translation／Enforcing，普通App su-M通過；同一原簽章0.4.1及精確156.0完整splits已安裝。原遊戲在透視鏡載入前Unity SEVL處SIGILL，沒有product attempt。獨立無Root ARM-only probe 0.0.2的baseline（含相同signal／alarm設定）通過，SEVL及SEVL＋WFE各在新程序SIGILL；同一APK於API35/libnb三模式均返回。16份API31歷史attempt SHA不變；恢復adbd non-root並正常關閉AVD。不能由先前JNI canary或安裝成功宣告這個API31 image可運行遊戲。本機ARM64 AVD提案已因官方frontend架構限制撤回，待指定新的原生目標；原件與SHA見[啟動指令界線](API31_GAME_STARTUP_BOUNDARY.md)。

## 2026-11-09：原簽章0.4.1與API35／156.1產品畫面（進行中）

使用者指定D槽搜尋後，已取得並驗證原產品簽章與兩版遊戲原簽章APK；材料不再缺。新增明確signed `-Api31Qualification`，預設仍API35，API32與所有精確守門保留。0.4.0未安裝且原件保留；修正動態標題後使用0.4.1/code14／SHA `815de868e747111c9e9739ea9987337c53441e10a15e585d2192cecab4741cce`，只安裝新index5。

首筆ATTACHING未完成exit44／UNKNOWN保留；新程序保持前景後，正常App su-M完成完整journal及啟用。實際花苗名稱／長字縮小／四次捲動重用、未領回黃色花苗→黃色金幣苗、兩頁正常App OFF原文／字型還原已觀察，花苗數356不變。再重啟新PID，舊consent／status拒絕，重新確認後兩頁ON／OFF與花苗捲動再次通過；原UNKNOWN及七份研究attempt SHA不變。探險僅一筆花苗，長名及離屏重用仍待樣本；API31／156.0及OS／長時間待後續。不可把這個部分結果寫為四組通過。詳見[0.4.1分項證據](PRODUCT041_QUALIFICATION.md)。

## 2026-11-09：Google套件後API35普通App載入回歸

- 使用者授權後，只在新MuMu index5由原廠小工具安裝器安裝Play／GMS／GSF，記錄實際版本／APK及cert SHA；商店到登入頁，登入由使用者完成，未取得指定遊戲。API35／libnb／原Permissive不變，普通App su-M再驗通過。見[Google套件紀錄](API35_GOOGLE_INTAKE.md)。
- 新研究client0.0.5／carrier0.0.7（同lab簽章、新版本）重用原probe0.0.7，普通App UID10052自行su-M執行自身PM runner。正常JNI／全部負例／journal／90秒FINISH通過；另一新程序timeout實際42／END UNKNOWN／精確退出；再一新程序正常完整回歸通過。tmp runner副本14、錯API參數instrumentation1及PM-path／stale-start6／5均拒絕。
- 前四份attempt SHA不變，三份新attempt保留，無probe存活；adbd恢復2000後普通App Root再驗。研究App結果不是遊戲／產品支援，四組仍未驗，原簽章與遊戲材料待補。詳見[API35普通App研究](API35_APP_ROOT_FLOW.md)。

## 2026-11-09：API31 Java／native候選接線（未安裝）

- 普通App研究路徑實測後，將Java與native接上同一`-Api31Candidate`開關；只允許unsigned candidate。預設Java常數false／native OFF，API35資格不變；API32及不精確遊戲版本仍拒絕，native bridge／digest／prologue／runtime metadata守門保留。
- 兩種完整建置及29/27/8/4 JVM與封裝檢查通過。Compatibility新增編譯flag核對後各35項通過，對兩份實際App classes核對flag且相反flag負例均拒絕。兩種CMake cache／封裝host marker與Java一致；API31簽署路徑實際pre-build exit1。
- 新unsigned候選SHA、來源與歷史分層見[候選整合](API31_PRODUCT_INTEGRATION.md)。未新簽署／安裝／交付產品，四組遊戲全未驗。使用者已授權在新API35隔離機安裝必要Google套件，來源／實際安裝結果後續記錄；登入仍由使用者完成。

## 2026-11-09：普通App → su-M → PM研究loader完整API31路徑

- 新研究client0.0.4/code4內嵌精確SHA的x64 PIE，PM安裝後從自身nativeLibraryDir執行。Java核canonical path／digest，native重核精確client PM位置、祖先owner/mode及原probe／host守門；同bytes tmp副本實際exit14拒絕。
- 已授權API31 Root AVD新cold boot中，普通App UID10148自行呼叫PATH su-M，完成PM-path／stale-start負例（6／5），再一次正常JNI／其他負例、完整journal及90秒FINISH。載入命令由普通App發起，不是ADB代執行Root。
- 另一新程序由相同普通App路徑故意timeout，確認42／END UNKNOWN／精確target退出；之後新程序正常完整回歸。client不強殺in-flight loader、不自動重送。13份前輪attempt SHA均不變，原四台／台中未操作。
- 這是自有probe的完整普通App載入路徑，不是產品／遊戲／原生ARM64驗收。四組遊戲仍未驗，產品原簽章與遊戲材料尚缺。SHA與角色邊界見[普通App路徑](API31_APP_ROOT_FLOW.md)。

## 2026-11-09：已授權API31 Magisk Root及同domain研究

- 使用者調整條件並明確授權只替API31隔離AVD安裝Root。官方Magisk31.1 APK／同版本AVD patch，私人ramdisk override與冷備份；共享SDK ramdisk SHA不變。標準setup後關閉Zygisk並正常重啟，確認API31／libndk_translation／Enforcing／modules空；原四台與台中未操作。
- 普通App UID10148實測PATH su-M及/debug_ramdisk固定入口均exit0、uid0、u:r:magisk:s0、同boot且mount namespace與PID1相同。產品可沿用原su-M，不需fallback。初始未完成setup的IOException保留。
- Magisk domain十類supervisor實跑exit0。原probe0.0.6有兩筆CALLING前exit44與一筆timeout入口拒絕，全部保留UNKNOWN、不重送；另一新程序JNI／負例與90秒FINISH通過。
- 自有fixture每增量查時鐘導致常停在clock_gettime；probe0.0.7以有界batch降低時鐘頻率，產品orig_rax／user-context與thread守門不變。新carrier0.0.6精確綁0.0.7；在Magisk domain實際timeout取得exit42／END UNKNOWN／精確程序退出。普通App Root與adbd發起的Magisk canary分層記錄，不稱產品整個App載入已驗。
- API31 Root變更不再待批准；產品原簽章與兩遊戲完整splits仍缺，四組遊戲仍未驗。完整SHA、失敗歷史與最終回歸見[Root階段](API31_MAGISK_RESEARCH.md)。

## 2026-11-09：API31 native候選整合與兩平台dispatcher回歸

- 已把API31 adapter移至共用native來源，新增依實際API／完整SDK property／精確bridge的dispatcher；預設API35/libnb不變，只有明確unsigned native候選可包含API31/libndk_translation。Java仍拒絕API31/32，候選簽署實際pre-build exit1；未交付或安裝產品APK。
- 候選ON／預設OFF兩種完整建置均通過Compatibility28、InstalledNative29、GameSession27、APKLayout8、PairedDigest4、雙ABI native與Java/dex/16KB封裝。實際APK與cache核對沒有候選設定外洩。
- API31／Enforcing以carrier0.0.4、API35／原Permissive以carrier0.0.5各完成與產品相同dispatcher的真App namespace／ARM JNI／版本與其他負例、七階段journal、正常FINISH及程序退出；同一probe0.0.6 bytes不變。API31正常App Root仍缺，API35新boot正常App su -M再次通過。
- API31實跑兩份policy fixture各51項；原生ARM64僅編譯。兩台舊UNKNOWN核對保留，先後正常關閉新隔離環境；沒有原四台／台中操作、遊戲或登入。**四組遊戲仍全部未驗**。SHA、版本及證據邊界見[候選整合](API31_PRODUCT_INTEGRATION.md)。

## 2026-11-09：API31真App canary、實際收容缺陷修正與新API35

- 使用者同意兩項後，建立repo外lab key與新空白MuMu index5 Android15。研究簽章與產品原簽章分開；產品未新交付，四組遊戲驗收仍全部未完成。
- API31／Enforcing上，已簽署probe0.0.6與carrier0.0.1實際通過PM host、真ClassLoader／bridged namespace、ARM JNI四值、版本／key／缺library及symbol負例、worker生命週期、七階段journal及正常FINISH。不是正常App Root或遊戲通過。
- 真App timeout首次exit43未確認收容，保留UNKNOWN。多執行緒ptrace非leader fixture重現舊supervisor順序問題；先回收本地worker再觀察target pidfd後，10類fixture於API31及新API35實跑通過。API31另一新App timeout確認exit42／END UNKNOWN／精確程序退出，再於另一新程序正常JNI及FINISH通過，舊attempt未清除／重送。
- 新API35實際x64/libnb、原Permissive、內建KernelSU3.2.5；僅授權Root probe。真正App UID10052執行su -M exit0／uid0／同boot、mount namespace與PID1相同。後續同probe0.0.6配新carrier0.0.2完成真App ClassLoader/JNI與負例、七階段journal、正常FINISH；另一新程序timeout確認exit42／UNKNOWN及精確退出。遊戲仍未驗。
- 共用修正完整unsigned candidate建置通過26/29/27/8/4 JVM／封裝檢查與雙ABI native編譯；沒有新產品APK安裝或版本覆寫。細節、SHA與待取得材料見[本輪隔離研究](ZHUBEI_LAB_20261109.md)。以下各「待確認／未載入」保留為先前階段紀錄。

## 2026-11-09：持續目標與API31 PM研究準備

- 使用者要求持續至Android12/API31與Android15/API35、遊戲156.0/code1792142832與156.1/code1793076318四組成功可用；[驗收條件](ANDROID12_15_GAME156_GOAL.md)逐組要求真實兩頁、縮字、捲動重用、OFF及遊戲重啟後重新同意。**目前沒有新增任何遊戲通過組合**。
- 新PM研究runner及probe0.0.6／carrier0.0.1未簽署建置、stored/16KB與canary→host→runner配對通過；新host精確版本由manifest生成。無明確key／憑證則簽署入口拒絕，不搜尋其他repo的key；沒有簽署或安裝APK。
- 同一已授權API31 AVD的新boot，最終runner SHA `531e9c0f9492171f00d841660e66ca6822ae233e783fb623a938bac729102642`實際通過7項pre-attach拒絕；無attempt建立，錯套件zygote PID/start／TracerPid0保持。這不是正常載入或JNI／timeout新backend正例。
- 自有shell子程序降至UID10000後無法再執行stock su；不把這個替身子程序當真正App domain已驗，也不直接移除-M。產品正常App Root資格仍缺。
- PM載入／真ClassLoader／JNI／新runner未知呼叫收容尚待研究簽章與實測；Android15新空白MuMu index5方案待確認。原四台及台中裝置未操作。詳見[更新的API31研究](API31_AVD_RESEARCH.md)。

## 2026-11-09：真正API31官方AVD／Enforcing原生隔離驗證

- 使用者明確同意後安裝官方image package.xml rev14與emulator37.2.13，建立`SeedLens_API31_x64`（2vCPU/4096MiB，獨立D槽資料，顯式serial emulator-5580）；WHPX檢查成功。實際native API與property均31，x86_64、libndk_translation.so、kernel5.10.185、SELinux Enforcing。未新增driver或重啟host。
- 原廠adb root可用，但su實際拒絕`-M`。沒有把adb root當成產品Root契約已驗；沒有安裝任何Root／顯示模組、關閉SELinux或改property。
- 以下是在該API31 guest**真正執行**、exit0：監督收容7類，journal85項含6個crash點，控制／狀態68項，真overlay syscall15例，GPR/XSTATE/red-zone call15例；另有命名／identity、canary identity7、ZIP正2負28/SHA2。全部使用自有fixture／fork child，無遊戲操作。
- native runtime API31及API32/35不相符、031格式拒絕在attach前實測；非法PID正確拒絕，不稱載入成功。沒有拿compile-time assertions當成裝置能力案例。
- 真mapped ART／native loader／native bridge／libndk_translation經pull前後SHA比對再查export，確認缺少IsNamespaceNativeBridged與NativeBridgeGetTrampoline2。依Android12固定AOSP介面新增**隔離**API31 adapter，使用opaque/raw namespace成對查詢、實際load的bridged旗標及四參數trampoline；不猜private pointer/layout、不force-load、不放寬產品守門。
- API31及原API35研究host都編譯通過；**新API31 adapter尚未載入真App**，沒有ClassLoader／JNI canary通過結論。未簽署／安裝研究APK、Lens或遊戲，沒有登入。
- 結束確認沒有研究／遊戲套件或fixture程序殘留，Enforcing不變；adbd恢復non-root後正常關閉這台AVD，資料／fixture／receipt保留。這是平台測試交還，不是遊戲重啟、OS重啟或長時間產品驗收。
- 原始receipt與平台二進位僅在ignored `.tools/lab/`；來源、SHA、逐層結果與下一步見[API31研究紀錄](API31_AVD_RESEARCH.md)。原四台與台中裝置沒有guest操作。

## 2026-11-09：竹北接手、研究工具修正與隔離環境

交接來源`60ba00d1ffc0e92b3b56b3eecaa4b7759364a29b`；private remote與recursive pins核對完成，乾淨clone後建立`codex/zhubei-android12-compat`。完整盤點及具體操作範圍見[本輪接手](handoffs/2026-11-09-zhubei-intake.md)。

- 在本機重跑Python profile9；candidate的Compatibility26、InstalledNative29、GameSession27、APKLayout8、paired digest4通過。雙ABI native、Java/dex及16KB封裝exit0。
- 本輪交接來源unsigned candidate SHA256：`5cde60aec6330b8cce99ddf64373d2dc6093956ba997b3883be55934db622c49`，不供安裝／交付。Native測試只有編譯。
- 研究工具修正`420596b`：ARM JNI及x64 canary以native runtime API核對明確expected API，移除固定34/35參數；API31僅研究准入，API32／不相符／格式錯誤拒絕。22項compile-time selection assertions在ARM64及x64各通過；兩研究loader、x64 host與故障fixture編譯exit0，**尚未裝置執行**。產品x64/API35及libnb守門不變。
- 研究binary SHA256：ARM JNI `dd5ceff5a0a16f62736f9b5189d9c9096ccfe7557e39430d47551da8c2763e78`；x64 loader `319b3449d9b1888543673f968fdf36a0383efc441052123fe97f40e8e994c536`；x64 host `e74942bd0edafb6e94fc7a81f432260f0d5715efdc152e0062cb84357e37e30b`。這些是本機來源產物，沒有簽署或安裝研究APK。
- 使用者明確同意後，只在新確認空缺的MuMu index4建立`SeedLens-Isolated-A12`，2vCPU/4GB，空白12.0原廠base；由CLI現查到loopback端點後才唯讀查該instance。
- 實際`ro.build.version.sdk=32`、release12、x86_64、libnb.so、kernel`5.4.32-perf-gda349bfae95e`、SELinux原為Permissive、Root設定未開。**不符合必要API31**，立即停止該路線並關閉新instance，保留資料；未安裝probe／遊戲、未開Root或改SELinux。
- 未連線台中Libra/Virgo或本機既有四台的Android系統，未讀取／複製scanner、token、帳號session，未重啟host。四台既有instance未改動。

### 目前組合矩陣（不得跨列推論）

最新新增API35／157.0及0.5.0結果見本頁首節與[九組矩陣](ANDROID12_15_GAME156_GOAL.md)；下表保留156.x各原APK的分項證據，不能移作0.5.0重驗結果。

歷史產品兩列均為0.3.1/code12，同SHA`e6262ccdbe50314ebe80063c67943e59a3cae4b5cc9dbae9059197849d2eb284`；本輪沒有重新操作它們。

| 平台／精確遊戲 | Root／SELinux | 平台／canary | 花苗及未領回探險名稱、縮字、OFF | 捲動重用／遊戲重啟／OS重啟／長時間 |
|---|---|---|---|---|
| ARM64/API34/156.1 code1793076318（歷史Libra） | 既有Magisk，provider版號本輪未量測／Enforcing | 歷史context、supervisor、真JNI已驗 | 同0.3.1兩頁已驗 | 0.3.1獨立捲動紀錄待補／遊戲重啟已驗／未驗／未驗 |
| x64/API35/libnb.so/156.1 code1793076318（歷史Virgo） | provider版號本輪未量測／Permissive | 歷史namespace、JNI、故障fixtures已驗 | 同0.3.1兩頁已驗 | 本版獨立捲動紀錄待補／新程序重新同意已驗／未驗／未驗 |
| 原生ARM64/API31/156.1 code1793076318 | 尚無確認目標 | 研究工具已編譯，裝置未驗 | 未驗 | 全部未驗 |
| 官方AVD x64/API31/libndk_translation.so／156.0 code1792142832 | 新授權Magisk31.1，普通App su-M／PID1 mount view已驗；Zygisk OFF／Enforcing | 真App JNI／ClassLoader／PM載入／timeout及10類supervisor已驗；原遊戲SEVL SIGILL，自有0.0.2 probe含相同alarm基準通過、SEVL失敗 | 原簽章0.4.1同APK已安裝但未啟用；遊戲尚未到登入，無product attempt，不算產品通過 | 全部未驗；Root環境正常重啟另記，AVD已關閉 |
| 官方AVD x64/API31/libndk_translation.so／156.1 code1793076318 | 同上，另次cold boot普通App su-M再通過／Enforcing | 原簽章完整splits正常升級；首程序Unity SEVL SIGILL、後續系統另記SIGSEGV，同0.4.1 SHA未變 | 未啟用產品，無product attempt，未完成登入；未驗 | 全部未驗；16份研究attempt保留，AVD已正常關閉 |
| 新MuMu index5 x64/API35/libnb.so／156.1 code1793076318 | 內建KernelSU3.2.5，正常產品App su-M已驗／原Permissive | 10類supervisor、普通App PM loader與JNI／timeout已驗；0.4.1遊戲載入／同程序再啟用及新程序已驗，首筆UNKNOWN保留 | 同0.4.1兩頁名稱／OFF及庫存縮字已驗；最長庫存名仍截斷，探險長名縮字待樣本 | 花苗重用及探險真正離屏返回已有畫面／新程序重新確認再啟用、兩頁OFF再驗／OS未驗／長時間未驗 |
| 新MuMu index5 x64/API35/libnb.so／156.0 code1792142832 | 同機KernelSU3.2.5，正常產品App su-M已驗／原Permissive | 原簽章完整splits及profile已核；本人登入後0.4.1載入及新樣本兩程序已驗，歷史UNKNOWN不變 | 兩頁名稱／OFF、庫存長名與縮字已驗；探險長名縮字待樣本 | 花苗重用、探險真正離屏返回及遊戲重啟／重新確認再啟用已驗；OS／長時間未驗 |
| 新官方AVD x64/API34／156.0及156.1各有獨立啟動 | 未安裝Magisk／Enforcing；沒有產品 | 自有ARM JNI baseline通過、SEVL模式SIGILL；排查圖形環境後兩版原遊戲各自於Unity SEVL SIGILL，發生在產品載入前 | 產品x64 API34仍拒絕；兩頁未驗，不算通過 | 全部未驗；來源、取消安裝與GPU比較見[API34研究](API34_ISOLATED_RESEARCH.md) |
| 新MuMu12.0/API32/x64/libnb.so/無遊戲 | Root關閉／原Permissive | 只量測OS；不符合API31 | 未驗，未安裝Lens | 全部未驗，已關閉 |

至少兩Android及兩遊戲版、包含真API31的完成條件仍未達成；編譯、API範圍、安裝及waiting-cards不替代兩頁實際驗收。

## 2026-11-09：竹北交接乾淨建置（非裝置驗收）

- 從來源 checkpoint `f368cea` 建立無 ignored cache／私人檔案的乾淨 clone，重新取得 pinned submodules。
- Python profile測試9、Compatibility 26、InstalledNative 29、GameSession 27、APKLayout 8、PairedDigest 4通過；雙ABI native、Java/dex及unsigned candidate封裝完成，exit0。
- Candidate SHA256：`37c8c992acf60d2f8179e6bae6b2a3014db1335e871113a637fe7e2bb135c527`；NOT_FOR_INSTALL_OR_DELIVERY，未簽署／安裝／覆寫舊APK。
- Native測試執行檔僅編譯，本輪未操作任何裝置，不新增第二遊戲或Android12/API31通過證據。
- 使用者新增API31為必要開發／實際驗收範圍，轉交竹北隔離環境；見[交接](handoffs/2026-11-09-zhubei-takeover.md)及[提示詞](ZHUBEI_PROMPTS.md)。

## 2026-11-08 23:29–23:48：同一0.3.1重開後再啟用

- 本輪沒有重建／覆寫APK，兩台仍為同SHA的0.3.1/code12及遊戲156.1。
  各自建立新owner維護pause、核pending空、原scanner SHA、私人App資料與attempt備份
  SHA及root-only owner/mode。沒有清除舊UNKNOWN或沿用先前載入同意。
- Libra正常遊戲重開29467/start21011096→20077/start21183908，先確認新程序沒有Lens
  mapping，舊status不算新ready。正常UI重新確認並啟用一次，同程序七階段journal完整；
  實際探險與花苗頁名稱／縮字可見，OFF後兩頁原名與字型還原。
  私人ON PNG尾碼1794152008500993800、1794152029584634000；OFF尾碼
  1794152080457361100、1794152103031453300。23:35解除本輪pause（1794152132）。
- Virgo使用原scanner在上輪恢復後自行重開的新程序31558/start5775696，不額外重開。
  啟用一次後先有host mapping／舊status，未冒稱ready也未重送；稍後同程序waiting-cards，
  進入頁面後新鮮ready。七階段journal完整。探險「黃色金幣苗」、花苗細分金苗名稱與
  長字縮小實際可見；OFF後兩頁原名／截斷還原。
  私人ON PNG尾碼1794152622642351800、1794152666540413400；OFF尾碼
  1794152733772113700、1794152766878326600。23:46解除本輪pause（1794152819）。
- Libra恢復後direct/fallback曾逾時，原scanner依既有政策自行冷啟動；沒有追加worker、
  手動重送或修改scanner。23:47新程序1034/start21268806已無Lens mapping，出現
  非零direct掃描（6 rows／623 bytes）及新上傳。站台data1794152832766／upload1794152832946
  晚於本輪解除時間；不能用先前心跳代替這項證據。
- 23:48 Virgo本機pause已解除、scanner SHA不變、pending空、Lens OFF、所有threads
  無tracer；當時站台僅心跳更新，新業務接受仍待後續確認。兩頁效果與掃描恢復分開記錄。
- 23:52補查：Virgo原scanner自行按既有恢復政策重開為52984/start5934382，沒有Lens
  mapping；一次fallback逾時後出現新的direct/object掃描（3 rows／328 bytes）及上傳。
  站台data1794153122777／upload1794153122956晚於本輪resume1794152819，原job429
  的target亦前進。Libra同刻持續新接受（data1794153122857／upload1794153123038）。
  兩台enabled且未paused；本輪掃描交還確認完成，不代表無中斷或全天穩定。
- 全程未派遣／領取／拔苗／GPS操作，沒有OS重啟。這是兩Android、同156.1的重開再啟用
  回歸，不是第二遊戲版本、所有Android或長時間穩定驗收。

## 2026-11-08 23:12–23:20：同一0.3.1 Virgo兩頁回歸

- Virgo instance3/API35/x86_64/libnb.so/156.1，具owner暫停、pending空與私人備份核SHA／
  owner/mode後，原簽章升級與正常遊戲重開13929→19670/start5721202。使用與Libra完全
  相同的0.3.1 APK；沒有改Root、SELinux、遊戲版本、帳號或安裝額外顯示模組。
- 正常UI確認主畫面與fail-stop後啟用一次。nativebridge完整七階段journal核對同boot／
  PID/start/UID；初始waiting-cards沒有冒稱成功。進入兩頁後新鮮ready，探險「黃色金幣苗」、
  金苗細分名稱與長字縮小實際可見。正常UI OFF後兩頁原名／原截斷均還原。
- 全threads無tracer、同PID正常返回主畫面，23:20只解除本輪pause（1794151231），
  scanner SHA不變且pending空。剛解除時只有心跳較新，沒有將舊上傳算作恢復，業務復查見後補。
- 私人PNG、backup及attempt保留；未派遣／領取／拔苗／購買。0.3.1目前完成兩個Android
  環境、同一156.1版本的兩頁ON/OFF，**第二遊戲版本尚未驗收**；OS重啟亦未驗。

23:27補查：Virgo原scanner在解除後首次direct/fallback逾時，自行依既有policy冷啟動遊戲，
新PID31558原生掃描ready，後續已有三筆`mode=direct source=object`完成與上傳。
站台新資料1794151625653／上傳1794151625829均晚於解除pause時間；Libra亦持續新接受。
此刻兩台enabled且未paused、currentJobId空，故不虛構新job ACK。
沒有另行kill／重啟／啟動第二worker或修改scanner。這是短期掃描恢復，不是全天穩定。
Libra自建JNI研究APK已在核對SHA及程序結束後移除；私人APK／receipt保留，可重新安裝，
不是使用透視鏡所需的額外套件。


## 2026-11-08 22:55–23:11：ARM64 JNI與0.3.1 Libra驗收

- 自有Android研究App使用與正式版相同ARM64 backend／KittyTrace／supervisor（僅編譯期
  固定不同套件與thread白名單），完成真實JavaVM的JNI_OnLoad、GetEnv、worker attach／
  detach、原native thread正常返回及後續JNI呼叫。錯game PID與錯start均在attach前拒絕。
  初次PM路徑owner過嚴亦在attach前拒絕；改用正式版原有root/system owner規則後通過，
  不修改裝置權限／SELinux。研究期間game PID/start、scanner SHA不變、無pause或tracer。
- JNI研究receipt SHA256 `86f08989babc47813c88aa2d89f5fa1badff2cb6309448cd044af7c36677b8f4`；
  journal核對同PID/start/UID及完整七階段，SHA256
  `64986247bf93ff625ff587e3b9636591a3cda6574307ca18c4309fdadf373fc5`。
  研究APK不是終端使用者依賴，也不能單獨代表遊戲效果。
- 產品0.3.1/code12為patch：修正ARM64呼叫、snapshot及有界收容；不新增遊戲profile。
  26 compatibility、29 installed、27 session、8 ZIP及4配對SHA檢查、雙ABI與原簽章建置通過。
  不可變APK SHA256 `e6262ccdbe50314ebe80063c67943e59a3cae4b5cc9dbae9059197849d2eb284`。
- Libra API34/ARM64/Enforcing、156.1：正常具owner暫停、pending排空、原APK/App資料/attempt
  私人備份並核SHA/owner/mode後原簽章升級。遊戲22516→29467/start21011096正常重開。
  正常UI要求主畫面與fail-stop兩項同意；缺第二項時拒絕，重新確認後只啟用一次。
- 同PID/start新鮮ready及實際花苗／未領回探險名稱可見，長金苗名稱縮字。
  正常UI OFF後兩頁還原原名／字型，沒有派遣、領取、拔苗或庫存操作。
  載入journal七階段完整，沒有以RETURNED_UNVERIFIED本身代替畫面驗收。
- 23:11解除僅本輪pause（1794150663），Lens OFF、遊戲主畫面、全threads無tracer、
  scanner SHA未變、pending空；站台新資料1794150676589／上傳1794150676754晚於恢復時間，
  enabled且未paused，原job429有target。只證明短期恢復，不是全天穩定。
- 此時Virgo仍0.3.0，0.3.1同版回歸另行記錄；第二遊戲156.0、其他ROM及OS重啟未驗。
  未commit/push或公開發布。下列0.3.0及「未安裝」敘述為對應時間的歷史紀錄。

## 2026-11-08 22:42後：ARM64整合開發候選（未安裝）

- `arm64_backend`及實際KittyTrace overlay接入工作區；持久CALLING ACK、完整CPU狀態、
  有界syscall／函式、UNKNOWN停止worker與pidfd收容，不再依原upstream無界呼叫。
- Libra自有程序32例通過，其中真正KittyInjector完成SO載入、constructor／export呼叫、
  卸載、snapshot相同及原程序繼續。包含修復syscall-exit的x7 kernel覆寫回歸。
  **沒有JNI／ART，也沒有對遊戲載入新候選**；詳見[ARM64證據](ARM64_LOADER_SAFETY.md)。
- 本輪沒有pause／APK安裝／遊戲重開；game PID/start及scanner SHA不變、全部threads
  無tracer、Lens OFF。已安裝產品仍為下列0.3.0，不能追溯附加本輪保護。
- 最終新增載入前後snapshot核對後已重跑完整候選建置：26 compatibility、29 installed、
  27 session、8 ZIP及4配對SHA通過，雙ABI與封裝完成。未簽章候選SHA256
  `a1c17b9012fc13984c8232def85aa78dc9d480697f55fc90cd112a383fb3966b`，明確禁止安裝／交付。
  不代表App／遊戲驗收；交付不得沿用0.3.0產物或版號。第二遊戲156.0仍待隔離環境與登入。
- 22:50前後正式唯讀GET：兩台enabled、未paused、原job429、更新的data/upload接受，
  uploadSilent=false。已交付0.3.0 SHA重新核對一致；未commit/push／簽章／發布候選。

## 2026-11-08 22:23–22:31：ARM64安全研究補驗（不改已安裝APK）

- 新的純自有fork-child ARM64工具在Libra API34／Enforcing通過15類別輪次及90次
  參數拒絕；正常call核對GPR／FPSIMD／TLS pointer／syscall regset／原stack，未知結果
  鎖住context並只回收測試child。100ms期限實測100–101ms，不假設任意副作用可回滾。
- 共用supervisor fixture可在兩ABI建置，Libra及Virgo各7類隔離協定／pidfd測試通過。
  沒有ptrace正式遊戲、未更新／安裝APK、未建立pause；兩台遊戲PID/start及scanner SHA
  不變、全部threads無tracer、Lens OFF。22:31站台有新資料／上傳及原job429進展。
- 該輪只改研究／測試，產品仍0.3.0/code11。當時ARM64入口仍待整合，不能將fixture
  升格為產品驗收；第二遊戲156.0仍只有離線證據。詳見[ARM64研究](ARM64_LOADER_SAFETY.md)。

## 最新狀態：0.3.0/code11候選（2026-11-08 22時段）

| 環境 | 安裝 | 本版實際效果 | 待驗 |
|---|---|---|---|
| Virgo API35/x86_64/libnb.so/156.1 | 0.3.0/code11 | 新遊戲程序正常UI啟用、花苗／未領回探險細分名稱、縮字、兩頁OFF還原；確認單次使用／離開失效 | 156.0、其他ROM、詳細頁、長時間、OS重啟 |
| Libra API34/ARM64/156.1 | 同一0.3.0/code11 APK | 新遊戲程序正常UI啟用、花苗／未領回探險細分名稱、縮字及兩頁OFF還原 | 156.0、其他Android、詳細頁、長時間、OS重啟 |

本批為minor／次版本：新增156.0精確profile，不是對任意遊戲版本開放。
原簽章APK SHA256：`74ff5b6fa214f8c711279fff42046f4a3ed6153a0408d00c495d73be2b15aff8`。
既有APK不覆蓋。兩台遊戲仍為156.1；沒有降版或新帳號測試。

- 完整建置通過：26 compatibility、29 installed-path/digest、27 GameSession、8 ZIP
  與4配對SHA檢查；雙ABI native／renderer／host／Java/dex／16KB封裝及原簽章。
- Libra與Virgo各執行68項native控制／狀態／profile隔離檢查及命名／字型policy，exit0。
  這些只操作自有fixture，不能替代兩頁畫面。
- 兩個遊戲ELF／metadata來源比對與26個故障變體拒絕通過；9項離線parser測試通過。
  156.0尚未實機驗證；來源與可重跑流程見[精確設定檔](GAME_PROFILES.md)。
- Virgo在具owner暫停、pending空、OFF且全部threads無tracer後，備份原APK／App資料／
  全部attempt，核SHA與root-only權限，再正常遊戲重開20468→41667/start5260856，
  install-r相同原簽章0.3.0。舊status不計入新程序證據。
- Virgo實測未確認時按啟用拒絕；勾選後先離開App再返回，勾選清除；重新確認後正常
  啟用，不重送。新鮮同PID/start ready；探險黃色金幣苗、花苗細分名稱及長字縮小實際可見。
  正常UI OFF後兩頁原名及原截斷還原，全部threads TracerPid0，回到地圖。
- Virgo只解除本輪pause（epoch1794146940），scanner SHA未變、pending空；22:10正式GET
  新資料1794147006788／上傳1794147006954晚於解除時間，原job429 target繼續前進。
  不是以heartbeat或controller paused=false替代本機pause／業務恢復證據。
- Libra同樣在具owner pause、pending空、OFF與無tracer後備份／核SHA／owner/mode，
  正常遊戲重開31942→22516/start20702111，原簽章install-r升級。正常UI重新確認／
  啟用後，新鮮同PID/start ready；探險黃色金幣苗、花苗細分名稱及長字縮小實際可見。
  正常UI OFF後兩頁原文／截斷還原，全部threads TracerPid0，回地圖。
  只解除本輪pause（1794147395）；scanner SHA未變，pending空。

22:17正式唯讀GET補查：Libra新資料1794147449087／上傳1794147443377均晚於恢復時間；
Virgo亦持續有較新的資料／上傳接受。兩台enabled、未paused且uploadSilent=false。
此刻兩台currentJobId皆空，不虛構新的job target ACK；僅證明短期恢復收集與上傳。

以上僅顯示測試，沒有派遣、領取、拔苗、購買、GPS或庫存寫入。原始PNG／備份與attempt
保存在私人忽略目錄／root-only裝置目錄，不進Git。未commit/push或公開發布；156.0需先
獲准隔離測試instance並由使用者登入，不能直接降版兩台正式掃描機。目標尚未完成。

## 0.2.2歷史基準（2026-11-08 21時段）

| 環境 | 安裝 | 實際效果 | 待驗 |
|---|---|---|---|
| Virgo API35/x86_64/libnb.so/156.1 | 0.2.2/code10 | 正常UI啟用、兩頁名稱／縮字及OFF還原；新遊戲程序再次啟用／還原 | 詳細頁、長時間、OS重啟與其他遊戲版 |
| Libra API34/ARM64/156.1 | 0.2.2/code10 | 正常UI啟用、兩頁細分名稱／縮字與OFF還原；正常遊戲重開後再啟用、捲動重用及還原 | 詳細頁、長時間、OS重啟與其他遊戲版 |

### 0.2.0→0.2.2 NativeBridge正式入口測試

0.2.0/code8為minor：新增獨立NativeBridge候選入口及native控制交易；0.2.1/code9、
0.2.2/code10為實測修錯patch。已交付每版位元組保留，不覆蓋舊產物。

- 22 compatibility、29 installed path/digest、8 ZIP及4配對SHA JVM測試通過；雙ABI
  native、ARM renderer、x64 host、Java/dex／原簽章建置通過。原生權限policy新增8正反例，
  Libra及Virgo執行通過。七類監督程序fixture通過，只使用／停止自有子程序，沒有執行ptrace，
  不代表在正式遊戲刻意製造未知呼叫亦已驗收。
- Virgo明確暫停掃描，確認pending空、保留scanner SHA；原0.1.6 APK／App資料私有備份，
  SHA及owner/mode核驗後升級。沒有清資料、換簽章、加顯示模組或修改SELinux。
- 0.2.0實際在ATTACHING後拒絕：切到Lens使game所有候選thread進入do_freezer_trap，
  尚未遠端呼叫。保留UNKNOWN，未對同程序重送。0.2.1改為明確按鈕先切回遊戲。
- 0.2.1在新程序正常載入host並detach，卻沒有renderer狀態。唯讀確認files為同App UID/GID
  的0771；原receipt目錄policy過嚴。0.2.2允許同App group-write，外部可寫仍拒絕。
  兩次有新原因／修正後才各正常重開遊戲，全部舊attempt保留；沒有無限重啟。
- 0.2.2在game PID9927/start4905599取得完整RETURNED_UNVERIFIED journal；host receipt
  逐步通過精確game／APK驗證、ARM64 load及translated JNI入口。獨立renderer新鮮ready
  與實際畫面才作為效果證據，非單憑上述receipt。
- 花苗頁實際可見「紅色照片鈕扣徽章金苗」、「白色咖啡杯金苗」等及長字縮小；
  探險未領回「黃色花苗」實際顯示「黃色金幣苗」。正常UI關閉後兩頁原文及原截斷還原。
  同PID ready ON診斷bound14/mask255/applied51；OFF後restored36。這些是顯示計數，
  不是遊戲物品操作數；沒有拔苗、派遣、領取、購買、GPS或庫存寫入。
- 全部game threads TracerPid0。私人畫面及原始紀錄位於忽略的`.tools/research/`，不進Git。
  本階段尚未commit/push／公開發布；尚不能宣稱多遊戲版本目標完成。

| 已簽署測試APK | SHA256 |
|---|---|
| 0.2.0/code8 | bf9cb303f85b7bb9c9d08b64ee53c2f231cf561b1f04a9939d17d72c773e9a22 |
| 0.2.1/code9 | 8ce24119564339f78a37e90cab70153e378a2041fcd0e3aec8673d78500d68c5 |
| 0.2.2/code10 | 632130788a8b06447b41f81f1b35fff670e37fcff1ddd7f714cab59a2039ee49 |

21:04–21:05復查：正常UI回地圖、Lens OFF後，僅解除本輪具owner的pause；scanner SHA不變。
正式站GET兩台enabled/未paused，Virgo新的資料與上傳接受均晚於解除pause，scan.pending空。
這證明短期恢復，不代表長時間穩定；此時沒有目前job，不虛構新job target ACK。

### 21:08–21:16 Libra同版回歸

Libra API34/ARM64/SELinux Enforcing、156.1：確認無未決掃描後以本輪owner暫停，
原0.1.6 APK及App資料root-only備份並核SHA，install-r相同0.2.2 APK，不改簽章或清資料。
正常UI啟用後同game PID18143/start19682491，PM renderer映射、新鮮11欄ready，
全部threads TracerPid0。實際未領回探險「黃色金幣苗」及花苗「紅色照片鈕扣徽章金苗」
等細分名稱／長字縮小可見；正常UI OFF後兩頁恢復原名稱與原截斷。沒有領取已完成任務、
拔苗、購買或庫存寫入。私人對照圖與status留在忽略的`.tools/research/`。

回原遊戲地圖、OFF後解除本輪pause（epoch1794143729），scanner SHA不變、pending空、
遊戲PID不變、全部threads無tracer。21:16正式GET的新資料及上傳接受均晚於解除pause；
Libra與Virgo都回到既有job429，target持續前進。這不是全天穩定或第二遊戲版證據。

### 21:21–21:40 同版新程序再啟用與掃描恢復

Libra在本輪具owner的pause、pending空、全部thread無tracer且顯示OFF後，私有備份
attempt原件並核SHA/owner/mode，正常重開遊戲18143→31942/start20410703。正常UI
重新確認主畫面後，同一0.2.2 APK在新程序完成探險／花苗ON、捲動重用及兩頁OFF。
最後applied57/restored57、全部threads TracerPid0；只解除本輪pause（1794144331）。
嘗試查看未領回花苗詳細資訊時遇到容量滿提示，選取消，沒有購買、丟棄、領取或拔苗；
因此詳細頁仍待驗。遊戲重開不等於手機OS重啟。

Virgo重新核對時遊戲已由9927換到20468/start5003555；本輪沒有重啟它，原因未確認。
舊PID的status不計為證據。在新的具owner pause及pending空後，正常UI重新確認並啟用，
同PID新鮮11欄ready；探險「黃色金幣苗」與花苗細分名稱／縮字再次可見。
OFF後兩頁原文／截斷還原，applied51/restored51、全部threads TracerPid0；回原地圖並
只解除本輪pause（1794145110），scanner SHA未變、pending空。

21:40正式唯讀GET確認兩台新資料及上傳接受均晚於各自解除pause時間，enabled且未paused。
Virgo仍在原job429且target前進；Libra此刻無目前job，不虛構新target ACK。這是短期恢復。
本輪發現UI勾選會沿用舊確認，實測均先重新勾選；後續來源已新增單次確認、離開App失效、
勾選時綁定PID/start及載入前再次核對。27項session parser／tuple正反例及完整candidate
建置通過，但尚未交付或驗證此UI修正；不得當作已安裝0.2.2行為。

以下為歷史分層證據，不能把舊「未啟用」描述套用到上述最新候選。

## 匯入前的 0.1.4/code5 原型

- Java/dex/原簽章 APK 與 ARM64/x86_64 loader、ARM64 renderer 建置通過。
- 19 個 compatibility JVM 案例通過：API 邊界、ABI 拒絕與精確遊戲版本/code。
- Virgo 獨立 shell 的 identity 九例及命名／字型 policy 執行通過。
- 原控制中心完整 app/native verify 最終通過，但不涵蓋獨立 App 全部行為。
- Virgo Android15/x86_64/156.1：0.1.3 無公開 namespace 而退出，沒有 renderer 顯示成功。
  0.1.4 停用此 backend。測試後原掃描恢復，五個新 object／非零上傳／完成 ACK 有紀錄。
- Libra Android14/ARM64/156.1 與 Virgo Android15/x86_64/156.1 的後續畫面都是原地圖基準，
  不是本 App 花苗清單或開關驗收。未宣稱 Libra 已安裝本 App。

## 獨立 repo 初次匯入

本次只抽取來源、調整 include/build 路徑、使用 pinned submodule，並將簽章改成
明確 repo 外 keystore＋環境變數；產品執行來源不改，保留0.1.4/code5歷史身分。
不操作手機／模擬器，不發布二進位。

2026-11-08 在新 repo 的獨立建置通過（exit 0）：
- 19 個 compatibility JVM 案例通過。
- ARM64/x86_64 loader 與 native 測試程式編譯通過；本輪沒有執行 native 測試。
- ARM64 renderer、Java/dex、未簽章 APK 打包及 zipalign 通過。
- 未簽章 APK SHA256：`596ef7408e2aac6b19b3f3ec70be10861a82e7fedb42151e963a208dfbf5fa29`。
- 新的外部 keystore 簽章入口尚未實際簽署驗證；未簽章產物不等同既有已簽署 APK。
- 以上僅為建置／JVM 證據，不代表 root 載入、花苗畫面或多版本相容已通過。

### 2026-11-08 15:22 補充：效果截圖前置查核

本輪只讀核對正式節點身分、套件、ABI、遊戲 PID、載入映射及新鮮回報；
沒有暫停掃描、安裝、注入、導航、重啟或修改裝置設定。

| 裝置 | 新鮮環境 | 顯示前置條件 |
|---|---|---|
| Libra | API34／arm64-v8a，遊戲156.1/code1793076318，PID26135 | 獨立 App 未安裝；控制中心0.15.25/code139仍在。目前沒有獨立renderer映射、控制檔或狀態檔。現行App僅依控制中心套件存在就拒絕接管，因此不能直接當作可驗的獨立App環境。 |
| Virgo | API35／x86_64／libnb.so，遊戲156.1/code1793076318，PID54319 | 獨立0.1.4/code5已安裝，preview OFF，無renderer映射及狀態檔；現行Java/native明確拒絕x86_64載入。 |

使用者要求的是兩台的「探險頁」及「花苗頁」套用效果，前次地圖基準圖不滿足要求。
本次兩台均未達套用前置條件，沒有產生或冒稱任何套用成功截圖。
後續需先完成各自載入／共存條件，再擷取實際頁面、核對同PID新鮮套用回報，
並驗證OFF還原；不得把原生未修改頁面或其他裝置舊圖當成獨立APK成果。

APK可以共用UI與遊戲renderer並內含多ABI載入器；需要分離的是ARM64與NativeBridge
載入後端，而不是僅以手機／模擬器名稱拆包。現行雙ABI建置不等於雙平台可用。

## 2026-11-08 15:37–15:52：Libra 移除控制中心與獨立 APK 實際驗證

目標：Libra ARM64／Android14 API34／遊戲156.1/code1793076318。
版本：0.1.5/code6，**patch**，原因為修正 Enforcing 環境的已觀測載入路徑錯誤。

- 先以具期限、可歸屬本次的本機暫停入口暫停掃描並等待 pending 排空。
  私人備份包含控制中心 APK／測試套件／資料及舊核心，核 SHA、owner/mode 後操作。
- 移除 `dev.ody.pikmincontrol`、`dev.ody.pikmincontrol.tests`；原已 disable 且未映射的
  `pikmin_nectar_rpc` 從 modules 移至 root-only 私人備份，不刪除原件。
  保留 scanner、Hunter、Magisk、LSPosed 與其他非控制中心功能；掃描設定及模組 SHA 未改。
- 0.1.4 實際遇到 `dlopen ... Permission denied`：renderer 位於 shell 暫存目錄。
  保存原 attempt／load log 並核 SHA；確認 tracer=0、未載入 renderer、無待處理掃描，
  只重新開啟遊戲一次。未清除未決、未停用 SELinux，也沒有重啟手機。
- 0.1.5 改用 Package Manager 原生程式庫路徑，原簽章升級安裝後，裝置 APK SHA
  與本機產物一致：`12b0a142653c548e98a9ea2cd1d4bd2d0b2dc1bad19b50105469784b789585a3`。
- 19 項 compatibility JVM 測試、雙 ABI loader／ARM64 renderer／APK 建置及簽章驗證通過。
  Libra 原生執行 identity/path policy 與 seed naming/font policy 測試通過；
  新路徑測試含 2 項合法、10 項拒絕案例。
- 新遊戲 PID6420 同 PID 新鮮 `ready`、renderer 映射、TracerPid=0。
  ON 時已觀察 bound17、mask255、applied66；OFF 時 restored43。計數是顯示診斷，非物品變更數。
- **實際花苗頁**：可見「紅色照片鈕扣徽章金苗」、「藍色咖啡杯金苗」等細分名稱；
  長文字自動縮小。**實際探險頁**：未領回的「黃色花苗」顯示為「黃色金幣苗」。
  正常 UI 關閉後，兩頁均還原原名；花苗頁原有截斷也恢復，形成同裝置 ON/OFF 對照。
  未拔苗、派遣、領取、購買或改動庫存。私人 PNG 不進 Git。
- 測試後關閉顯示、回到遊戲地圖，15:52 移除的僅是本輪所擁有的暫停檔。
  scanner／Hunter 雜湊一致；恢復掃描後的業務證據另見下方補記。

15:53–15:54 復查：新 object refresh、`completed point=80 rows=+1`、新增上傳118 bytes，
upload offset由26334173前進至26334291；已安裝 agent 的 completed 訊息只在
`send_scan_ack` 成功後輸出。正式站唯讀 GET 的資料／上傳接受時間均已晚於解除暫停時間，
enabled=true、paused=false；不是只憑心跳判定恢復。最終保留 Lens 安裝、顯示 OFF、
遊戲前景及原掃描。此 patch 尚未 commit/push／發布，不宣稱已同步遠端。

### Virgo 同輪核對

15:51 新鮮確認 Virgo instance3／API35／x86_64／libnb.so／遊戲156.1，
獨立 APK0.1.4/code5、無控制中心；當前遊戲 PID60297、TracerPid=0、preview OFF，
無 renderer 映射及狀態。Java/native 仍明確拒絕此 NativeBridge 後端。
本輪未安裝／注入／重啟／暫停 Virgo；正式站唯讀 GET 顯示 enabled、未 paused 且有新資料接受時間。
**Virgo 未通過，沒有兩個環境都能顯示的結論。** 不重複前次失敗試載或繞過拒絕守門。

## 下一階段

2026-11-08 16:57後續：新增僅trace自有fork子程序的研究工具，Virgo API35十項正常／逾時／
暫存器還原／恢復執行測試通過。唯讀核實現行libnativeloader的ClassLoader namespace等匯出符號。
未載入NativeBridge canary、未解除x86_64 blocker、未改APK或遊戲、未暫停原掃描；
不是模擬器花苗顯示驗收。細節及下一階段見[相容研究](COMPATIBILITY_RESEARCH.md)。

1. NativeBridge 隔離測試、timeout/restore 與 staging owner 交易。
2. 擴充 ARM64 的 Android 版本與 Root provider 實機驗收；Libra API34 一台不能代表全部。
3. 逐版本檢查庫存、探險列表／詳細頁、OFF 還原、捲動重用及遊戲／App 重啟。
4. 每個新遊戲版本新增整組已審查精確 profile；未知版本維持原文。
5. 不以本機測試、建置、安裝或心跳替代實際畫面驗收。

### 2026-11-08 18:29：NativeBridge 隔離階段進展

自有背景測試APK在Virgo API35通過正常ARM64 JNI，並在兩個新程序取得真正的ClassLoader
bridged namespace、載入ARM64 canary、呼叫trampoline取得4項正確結果。第二程序亦通過
缺library／缺symbol拒絕；7項測試包程序名稱policy通過。原遊戲未注入，掃描未暫停。
詳細產物／失敗／限制見[相容研究](COMPATIBILITY_RESEARCH.md)。
這是隔離載入路徑證據，不是正式透視鏡雙平台／多遊戲版本／SELinux Enforcing相容驗收。

18:35補驗：同產物新程序重跑、兩個缺失項目負例與90秒正常FINISH通過；另以自有測試
程序刻意逾時，watchdog只終止該程序並核對回收，遊戲PID不變且無tracer。
站台確認Libra/Virgo有後續新資料／上傳接受。正式產品尚未整合此路徑，goal仍active。

18:37後補驗：Virgo自有測試通過translated JNI_OnLoad的錯key拒絕、重複初始化單worker、
ARM64 worker接入／離開JVM。再以code4把ARM64 canary只放在同一APK asset，直接從APK
ZIP entry載入，全部canary與lifecycle項目通過，Java正常FINISH。簽章後16KB對齊已查核。
有界ZIP讀取／SHA policy在Virgo通過2正例、28負例、2個SHA向量及實際APK正確／錯誤SHA。
詳細產物與失敗留存在[相容研究](COMPATIBILITY_RESEARCH.md)。
**仍非正式遊戲顯示、多Android／多遊戲版驗收；production refusal未解除。**

18:55補驗：SHA gate已接入研究host，新程序完成驗證後載入、JNI生命週期與正常FINISH。
無測試程序／tracer殘留。掃描曾有逾時，停止新增裝置實驗後唯讀確認兩台有新的站台資料／
上傳接受；未修改掃描器，不能把這次進展當成全天穩定驗收。未commit/push或發布。

### 2026-11-08 19時段：載入交易與系統呼叫隔離

- `load_journal.h`已接入loader來源；兩ABI native編譯通過。Libra API34／Enforcing與
  Virgo API35各執行85項檢查，包括六個崩潰點、跨程序鎖、舊v1／UNKNOWN不覆寫、不重送、
  symlink/hardlink/FIFO及權限拒絕。只操作自建fixture；沒有對遊戲載入新版loader。
- 研究overlay修正真正KittyTrace的syscall/stop等待與internal restart結果判定，Virgo執行
  15項fork-child案例通過：正常、逾時、tracee訊號、tracee退出、tracer訊號干擾。
  存活子程序GPR逐byte還原、heartbeat繼續並正常回收；不是FP/stack/TLS/dlopen副作用驗收。
- 新增payload／journal／overlay尚未交付成正式APK。x86_64仍拒絕，UI拒絕原因文字改為
  尚未完成正式載入／復原驗證，不再誤稱ROM沒有任何可用namespace。
  產物、失敗與限制見[相容研究](COMPATIBILITY_RESEARCH.md)。

### 2026-11-08 19:27：x86_64呼叫狀態與實際隔離APK載入

- 新增研究用獨立stack／GPR／完整NT_X86_XSTATE保存及read-back檢查；逾時、未知訊號、
  fault或退出均為UNKNOWN，禁止同context重試，不假設TLS／heap／dlopen鎖可回滾。
  Virgo五類各三輪共15項通過；正常call另做連續第二次返回與原child heartbeat恢復。
  舊syscall15項也重新通過。這不是production loader驗收。
- 研究APK升至0.0.5/code5，同簽章、不清資料；提供30秒限定的專用user-space thread。
  實際研究loader改在同TGID、名稱／start核對的thread工作，成功以guarded scratch stack
  載入host，CPU狀態逐byte還原；原生host完成真ClassLoader namespace、APK內ARM64
  canary四值、缺失lib／symbol拒絕、JNI bad-key／單worker生命週期檢查。
  PID28908最後CALL_THREAD_FINISHED、FINISH PASS，程序結束，無probe/tracer殘留。
- 曾因Android argv零填充被嚴格identity拒絕，修正為只允許末端NUL；另一次主執行緒
  system-call context被拒絕，**沒有解除守門**，改用自有測試thread。失敗原件仍保存。
- 未對遊戲注入、重啟或建立pause；新snapshot遊戲PID25272/TracerPid0。兩台站台接受時間
  有前進，但非全天穩定驗收。正式遊戲沒有這個自建thread，安全thread選取、PM封裝、
  正式兩頁顯示/OFF/重啟及第二遊戲版本仍待完成；產品x86拒絕未解除。

### 2026-11-08 19:37–19:40：0.1.6單APK雙ABI PM-loader驗證

目標：Libra API34/ARM64/Enforcing與Virgo API35/x86_64/Permissive。
版本0.1.6/code7，patch，修正取得native鎖之前覆寫loader執行檔的競態；不新增遊戲支援，
不聲稱NativeBridge正式可用。原0.1.5及更早APK不覆蓋。

- 19項compatibility＋27項installed path/digest JVM檢查通過；雙ABI loader／native tests／
  ARM renderer／Java/dex／APK建置及原簽章驗證通過。APK內兩個loader均stored、SHA與
  內嵌digest相符，無legacy `assets/seed-root-*`；最終APK 16KB ZIP對齊檢查通過。
- 新鮮核對兩台原遊戲156.1、Lens OFF、無tracer、無scan pause。root-only備份原APK及
  App data，核APK SHA及備份owner/mode後install-r，不清資料、不指定ABI、不重開遊戲。
- 同一signed APK SHA256 `d13a20ca9ac450acf5f98978f167a88f544be9afafc154df94735740638de350`。
  PM在Libra自動選arm64-v8a、Virgo選x86_64，均回讀code7／0.1.6及相同APK SHA。
- 兩台實際執行PM安裝位置的`libseed_loader.so --self-check`成功；檔案owner1000:1000、
  mode755。同位元組複製到`/data/local/tmp`後必定stage23拒絕，沒有修改SELinux。
  兩ABI更新的identity／installed path原生policy各在對應裝置執行通過。
- 安裝前後Libra遊戲PID18143、Virgo25272不變，TracerPid0；控制檔仍OFF。未啟用顯示，
  未暫停／修改scanner，站台兩台有後續新接受資料／上傳。原備份及所有attempt保留。
- **這是安裝、ABI／root exec／policy驗收，不是0.1.6花苗或探險效果驗收。**
  UI整個ON/OFF交易仍需native owner整合；ARM正式call仍用原upstream、x86載入仍拒絕。
  未commit/push或公開發布；不把這兩個Android版本的自檢升格成多版本顯示相容。

### 2026-11-08 19:50–20:00：共用bridge adapter／正式候選來源／跨APK隔離

- `bridge_platform.h`由canary與正式候選host共用。Virgo實際隔離程序40256通過錯誤套件、
  versionName、versionCode拒絕及正確0.0.5/code5接受；原APK SHA／namespace／4值JNI／
  bad-key／單ARM worker生命週期項目均通過，最後FINISH PASS並結束。
- 新正式候選`seed_bridge`以156.1精確guard及既有ARM renderer SHA編譯通過；
  **未載入遊戲、未包入已安裝APK**。兩ABI新增package/path policy在Libra及Virgo執行通過。
- 發現0.1.6 ARM asset壓縮；新Java封裝檢查8項fixture通過，對現有0.1.6確實拒絕。
  使用相同既有payload做未壓縮／16KB隔離重封裝，必要native entry與SHA檢查通過。
  該未簽名檔明確標示NOT-FOR-INSTALL，未交付／安裝；現有0.1.6不修改、不冒換版本。
- 第二次隔離程序44342經真ClassLoader namespace直接載入**另一App已安裝APK**中的
  ARM renderer，能取得translated JNI_OnLoad；只傳錯誤key驗證拒絕，沒有啟動renderer或hooks。
  接著原canary全鏈仍通過。這比自身APK fixture更接近正式布局，但不是遊戲顯示驗收。
- 遊戲PID25272、TracerPid0，未建立scanner pause；兩台有新的站台接受資料與上傳。
  不宣稱全天穩定。沒有第二遊戲profile，正式ON/OFF／重啟及安全game thread入口仍未完成。
  本階段不交付新產品版本、不commit/push或發布；目標持續進行。

20:01復查：44342亦正常FINISH PASS、probe消失，game25272無tracer、無scanner pause。
19項compatibility／27項installed-path JVM回歸再通過。兩台站台已有新接受資料／上傳，
且正在既有掃描job；後續遊戲UI／注入測試需重新確認批次狀態並正常協調暫停，不打斷在途。

### 2026-11-08 20:03–20:16：候選native控制交易

Java／native／renderer共同改動：native持有同一lease完成OFF→必要載入→新鮮同PID/start
回報→ON；OFF可獨立要求，但不繞過正在操作的lease。Java不再在鎖外改寫控制檔。
新狀態為原v1追加start欄，舊10欄回報不能授權新啟用。未知載入紀錄永不補成成功。

- 兩ABI loader、ARM renderer與control/journal tests編譯通過；Android Java全來源編譯通過。
- Libra API34與Virgo API35，各執行最終53項控制／狀態／載入紀錄檢查通過；涵蓋正常ON/OFF、
  同／跨程序鎖、writer退出後持久性、symlink/hardlink/FIFO/mode拒絕、PID/start／時效／版本／
  欄位不足與UNKNOWN／部分載入紀錄拒絕。只操作自建fixture，無遊戲寫入／注入。
- Virgo原journal85項回歸（含六個crash點）再通過；這不是新控制寫入的每個crash點故障注入。
- 遊戲PID18143/25272與TracerPid0持續；兩台0.1.6不變，未暫停scanner。20:13站台Libra仍
  執行job429，Virgo已無目前job，兩台有新接受資料／上傳。不能用此snapshot取代下輪核對。
- 沒有產生／交付新APK、未commit/push。下一交付需配對候選APK/renderer；若同批完成
  NativeBridge新功能，按minor選下一版本，現階段不能把未交付來源標成0.1.6實機行為。
  後續重點為正式NativeBridge入口及真正遊戲頁面驗收，不以更多fixture代替它。

### 2026-11-08 20:25–20:32：Virgo實際遊戲entry及同批單APK配對

- 新的只讀stop元件在自有fork child通過wrong TGID/start拒絕及12次user-space停止／釋放／
  heartbeat繼續；沒有對child注入或修改register。不是signal/fault全矩陣驗收。
- Virgo instance3/API35/x86_64/libnb.so/156.1：首次限定單一UnityMain名稱的工具因重名拒絕，
  **在ptrace前退出5**；立即解除本輪pause。之後觀察到game PID由25272變成57557，
  本輪沒有發出重開／kill；不猜測重啟原因。新程序有多個繼承UnityMain名稱的thread。
- 修正候選篩選，重新核對身份、建立第二個具期限的自有pause、等agent明確locally paused且
  scan.pending空。對新game PID57557、TID57783做12次只讀取得／釋放：10次user-context、
  2次kernel或不可讀狀態；沒有寫register、遠端call、load、UI操作或game RPC。
  每次detach成功，遊戲PID不變；解除本輪pause，原scanner SHA不變。
- 20:32正式站GET顯示兩台enabled/未paused、有晚於解除pause的新資料與上傳接受。
  這是短期恢復證據，不是全天穩定性或新renderer效果。
- 候選APK完整建置通過：19 compatibility、27 installed-path、8 ZIP layout及4新配對SHA
  正負例；雙ABI loader、ARM renderer、x64 host、Java/dex/封裝均完成。
  host綁本批renderer，不再沿用已安裝0.1.6 pin。首次因Windows CMake receipt CRLF而被
  layout拒絕，改為明確LF後通過；沒有放寬receipt長度。
- 未簽名candidate SHA256 `543af7948558c9d0183286bb8acb297d717432c6e5d594d17264643cecba080f`。
  **不供安裝／交付**，裝置仍0.1.6；產品版本不因研究建置推進，正式x64 gate仍關閉。
  未commit/push。遠端call未知副作用的正式收容／復原、真正遊戲兩頁ON/OFF／重啟、第二
  遊戲profile尚未完成，不能宣稱多版本目標達成。
