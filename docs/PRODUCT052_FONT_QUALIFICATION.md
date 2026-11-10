# 0.5.2/code17字型修正：驗收進行中

0.5.1實際改善庫存長名稱，但探險短名縮小並截斷，不能通過，原件與失敗證據保留於[0.5.1紀錄](PRODUCT051_FONT_CANDIDATE.md)。0.5.2依卡片的既有runtime class區分字型下限：庫存維持12點；探險與詳細頁恢復原75%（至少12）。仍讓遊戲自行選字體，原FontState及OFF還原契約不變。沒有改遊戲profile／metadata／程序身分／Root／NativeBridge守門，也沒有對其他遊戲文字或物品寫入。

本版只根據已觀察layout差異限定修正範圍，尚未證明AutoSize內部根因或任意長名皆能容納。探險長名縮字、兩頁重用及OFF／遊戲重啟仍須實測，不把舊版成功或庫存單頁結果轉記通過。

## 產物與檢查

- 原簽章單APK：0.5.2/code17，SHA256 `9457ab23f60d16e3ffc9a729f41571bc9b0066e68ef2c255dccc058d90431b17`；certificate SHA256 `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`，aapt／apksigner另核。0.5.1及所有舊APK未覆寫。
- 既有policy／FontState測試增加庫存與探險不同下限，Linux g++13實際compile/run exit0。這是host policy證據，不是Android native或畫面驗收。
- 明確工具路徑的完整簽章建置通過：48 compatibility、29 PM path/digest、27 session、8 APK layout及4配對digest檢查；雙ABI native／ARM renderer建置與stored SO／16KiB ZIP配對檢查通過。Android native測試只編譯，未冒稱實跑。
- 使用預設API31候選false建置，x64仍只接受API35/libnb.so；API31／34未新增產品資格。

## 安裝

2026-11-10，新index5同boot／API35／x64／libnb.so重新核對，正常App OFF及兩頁原文／字型還原後，標準force-stop確認遊戲已退出，才正常install-r升級。PM讀回0.5.2/code17／相同APK SHA；未清除App或遊戲資料，原26份產品／研究attempt SHA全數不變；adbd回讀UID2000。此時還沒有本版遊戲顯示資格。

## API35／156.1首程序分項

同boot `4868c40f-4546-42d1-beb8-ae744694fc76`、PID6775/start192759／UID10066。本人登入保留，03未確認啟用實際拒絕、04舊status明確拒絕，05新確認後才正常App單次載入。06 waiting-cards不算畫面成功，另以實際UI驗收。

- 08庫存兩張三週年紀念杯子蛋糕金苗完整縮字，09圖示及長名字串真正離屏，10返回仍完整同名；短名保留較大字。
- 11探險藍色金幣、黃色廚師帽子、紫色貼紙苗均完整，未重現0.5.1的短名截斷。12首排圖示與名稱離屏；13是面板收合，**不能算返回或重用通過**。其他完整名含白色橋樑別針、冰色三葉、岩石蒲公英與藍色公車紙模型，但沒有把能完整顯示冒稱長名已觸發縮字。
- 14正常App OFF後，16探險原花苗名／大字、17庫存原文／大字／紀念金苗原截斷已還原；當次33/80。15僅收合面板，不作還原成功證據。
- 同PID/start新鮮156.1／ready／enable0／control OFF；讀到的thread TracerPid均0、adbd回讀UID2000。原26份attempt SHA不變，新增第27份完整journal SHA256 `0b4df844c4ce9bf1d9445e8f375a0b7fe1818ef45b063f627b7b24fa8e68e4e3`。

首程序探險離屏返回仍需補驗，正常遊戲重啟正在進行；本版整組及九組目標尚未完成。

## API35／156.1正常遊戲重啟

正常OFF後標準force-stop／launcher重開，同boot新PID8789/start226079／UID10066。20未確認啟用與21舊status實際拒絕（不是核對中），22重新確認後才從正常App載入，完整七階段journal另核。沒有承接舊ready或重送未知載入。

24庫存長紀念名與短名正確；28兩張長名卡圖示及字串完全離屏，29返回同名且完整。25探險末排藍色公車紙模型苗、冰色三葉／岩石蒲公英等名稱正確；26回到頂部時這些末排卡圖示及名稱完全離屏，27返回相同細分名稱，補得真正探險重用證據。

30正常App OFF後，31庫存原文／大字／紀念名原截斷、32探險原花苗名／大字均還原，當次33/80。最終同PID/start／156.1／ready／enable0、control OFF，讀到thread TracerPid均0、adbd回讀UID2000。原27份attempt SHA不變，新增第28份完整journal SHA256 `1f91781f1de3bec649b6863a04139938a0b4a721e1f7322a9c2d1662fc03109d`。

本版API35／156.1已有兩頁名稱、庫存長字、兩頁真正離屏返回、OFF及新程序分項。探險更長名稱觸發縮字、OS／長時間及其他平台／遊戲版仍未驗，不記整組或九組完成。

## API35／157.0首程序分項

156.1正常OFF／退出後，正常install-multiple-r升級精確157.0/code1793360009，合法原簽章base／完整ARM64 split各自PM SHA再次吻合既有intake，產品仍同0.5.2 APK；沒有清資料或複製session。原28份attempt SHA不變。遊戲初次使用系統簡體設定，經正常設定切回繁體並接受其遊戲重啟，登入保留；沒有把語言切換算產品的載入後重啟回歸。

同boot、PID10899/start264686／UID10066；私人157子目錄12／13為兩頁OFF基準，33/80。14未確認啟用與15舊status實際拒絕，16新確認後正常App單次載入；完整七階段journal另核。first-loaded中途snapshot仍control OFF／waiting-cards，保留其原時序；18稍後才實際ON_COMMITTED，first-on另核同程序157.0／ready／enable1，不重送載入、不倒填中途成功。

19完整庫存紀念長名／短名，20兩張長名卡圖示及名稱完全離屏，21返回同名且完整。22探險金幣／廚師帽子／貼紙等完整；23末排藍色公車紙模型、冰色三葉等名稱，24回頂部時末排卡圖示與名稱完全離屏，25返回同名。26正常App OFF後27探險原花苗名／大字、28庫存原文／大字／原紀念截斷還原，當次33/80。

最終同PID/start／157.0／ready／enable0／control OFF，175份thread TracerPid讀值均0（非原子snapshot）、adbd回讀UID2000。原28份attempt SHA不變，新增第29份完整journal SHA256 `39d75dfd595e5d518cad5989bafab2b1708902cbabaf6b5057299bf799e21e49`。正常遊戲重啟回歸進行中，探險更長名稱縮字、OS／長時間與其他組合仍未通過。

## API35／157.0正常遊戲重啟

正常OFF後標準force-stop／launcher至新PID13405/start314334／UID10066，繁體與登入保留。31未確認啟用及32舊status實際拒絕，33新同意後才載入本程序；34正常App實際ON_COMMITTED，完整七階段journal另核。沒有自動承接或重送載入。

35庫存長紀念名／短名，36長名卡圖示及文字完全離屏，37返回完整同名；38探險末排公車紙模型苗等完整，39回頂部時末排圖示與名字完全離屏，40返回同名。41正常App OFF後，42探險原文／字型與43庫存原文／大字／原紀念截斷還原，當次33/80。

最終同PID/start／157.0／ready／enable0、control OFF；165份thread TracerPid讀值皆0（非原子snapshot），adbd實際UID2000。原29份attempt SHA不變，新第30份完整journal SHA256 `4adab4babb015b953bab0e59c9dbcdec4d80414b22f37805d5026443f9feb9d1`。本版API35／156.1與157各有兩個程序的分項證據；探險更長名字縮字、OS／長時間及其他組合仍未通過，九組目標未達成。

## API35／156.0接續準備

157正常OFF／兩頁還原後，重新核精確156.0/code1792142832的合法base／完整ARM64 split，aapt與兩份apksigner皆通過，game cert仍`944bda466eaf64be1c93896d5c8e9a454a07c8d8fcbc4adeeefb3545b335e979`。沿用先前已核准隔離機流程，標準force-stop／uninstall後乾淨安裝完整splits，不複製或備份帳號session，登入交本人。

PM讀回兩份APK SHA各與intake一致，產品仍同0.5.2 SHA，原30份attempt逐份不變，control OFF、adbd實際回UID2000。本版156.0尚未登入／載入／兩頁實測；材料與安裝不列為相容通過，後續也不繞過可能的強制更新。

後續API34 AOSP正常退出後，恢復此新index5、核新OS boot、同產品與156.0完整PM SHA。
普通App唯讀su-M／global mount view新收據與原30份歷史SHA另核，control仍OFF、readback後
adbd回UID2000。正常啟動精確156.0，依本人既有關閉提示授權選擇MuMu「不加速」，未開啟
額外服務；現在停出生日期起始頁，已請本人登入樣本帳號。未代填生日或帳密、未啟用Lens，
不是本版156.0兩頁或OS後產品相容通過。原始UIAutomator helper退出139而留下dump的失敗
另存，未當成遊戲崩潰或默認操作成功；其後由新鮮畫面核對才選擇關閉該提示。

## API35／156.0本次樣本：兩個程序的庫存分項

本人完成登入後，實際庫存357/30；本次探險頁只有三筆已完成水果，沒有未領回花苗。
本人表示稍後更換有花苗探險的樣本帳號。本輪不領取水果、不派遣，不把水果頁或舊帳號
的證據當成本版156.0花苗探險通過。

同新boot `7ffb85f6-02df-46d4-add2-6049f5761944`，首程序3056/start65165／UID10067。
06是完整庫存OFF基準，08證實水果探險樣本不足；04／07屬畫面過渡，不能作基準。
10未確認啟用與12舊status實際拒絕，14新程序確認完成、15兩項同意後才正常App單次
載入。當時遊戲停在登入後的探險地圖收合面板，保留該上下文，不將其稱為主畫面截圖。
17回報ON_COMMITTED／waiting-cards，仍不算顯示成功；另讀first-loaded control ON及
持久journal，之後實際開庫存才驗效果。

- 19「紅色五週年紀念玩具珠寶金苗」及照片鈕扣徽章金苗完整縮字，咖啡杯短名較大。
- 21上述首排卡片的圖示與名稱皆完全離屏；22返回完整同名，真正庫存重用分項通過。
- 23正常App OFF_COMMITTED後，24庫存原花苗名／大字及原有截斷均還原。
- first-on與first-off另核同PID/start／156.0／ready，enable分別1／0；最終control OFF，
  207份thread TracerPid讀值皆0（非原子snapshot），adbd實際回UID2000。

OFF後正常force-stop／launcher至新PID9084/start359211，UID10067相同；25另驗真主畫面。
26未確認啟用及27舊status最終拒絕，不只截到核對中；28新程序確認及29兩項同意後才
載入新程序。31的waiting-cards不計畫面通過，34／35／36另有相同長名完整縮字、整張
卡真正離屏及返回同名；37正常App OFF後38原文／大字／原截斷再次還原。

restart-on／off實讀新PID/start／156.0／ready、enable1／0與最終control OFF；最後175份
thread TracerPid皆0（非原子snapshot），adbd回UID2000。同產品APK與完整156.0兩份PM
APK SHA再次吻合。原30份attempt逐份不變；首輪新journal後續SHA不變，新增兩份總32份：

| 程序 | journal SHA256 |
|---|---|
|3056/start65165|`2fa12e364ca78f5cf90f8254c74d2f7837caad5e7e97759b0263cfd7bb0483d1`|
|9084/start359211|`f94be0a75a355e09479159da431dc10fbffb470ea728ce943f82eefe3c95d39d`|

兩份journal實際含PREPARED至DETACHED及RETURNED_UNVERIFIED；不改寫成native已驗證
成功。新鮮renderer ready、ON／OFF與真正畫面另核，歷史UNKNOWN不變、不重送載入。
本輪只完成兩個程序的庫存分項，探險名稱／長字／捲動／OFF仍缺新樣本；也不算本版
OS重啟或長時間驗收。API31／34與九組目標保持未完成，沒有修改或交付新產品bytes。

## API35／156.0本人更換未領回花苗樣本

本人回覆「156.0 花苗探險樣本就緒」後，CLI／新boot／精確PM SHA／PID/start/UID重新核。
換帳號仍在同9084/start359211，透視鏡OFF且32份attempt SHA不變，並非另一遊戲程序。
41確認本人新帳號主畫面；45庫存25/80，44探險OFF基準有藍色／紅色／灰色花苗及水果／
禮品。未領取、派遣或修改任何物品，也不由切換帳號推論已重啟。

47重新確認同一程序、48兩項同意後正常App啟用；49為ON_COMMITTED。已載入的已知
ready程序只重新切換顯示，sample-account-on實核沒有新增attempt，不重送native載入。
51與53探險完整顯示藍色葉子帽苗、紅色三葉花苗、岩石三葉花苗，未重現0.5.1的短名
截斷。只有兩排／三筆花苗，尚不足以證明探險長字縮小或整張卡離屏重用。

52面板收合不是離屏返回通過。為觀察短視窗，在同隔離機以Android原生wm暫設
1080×1440；原無override、density280不改，PID/start／boot不變，54實際截圖保留。
遊戲等比例縮放，仍不足以形成有效離屏證據；其後wm size reset實讀1080×1920且無
override、density280。API35 input help確實支援mouse scroll，但55／56的滾輪與57觸控
觀察沒有實際移動列表，不冒稱成功。沒有額外套件、主機／安全設定或產品bytes更動。

60庫存羽翅五週年紀念零食、紫色五週年玩具珠寶與黃色三週年杯子蛋糕金苗皆完整縮字；
61這些卡片圖示與名稱完全離屏，62返回完整同名。63正常App OFF後64庫存原名／大字／
原截斷、66三筆探險原花苗名／字型均還原。65只見第一排，不代表末排也已驗，另以66
補核。最後同PID/start／ready／enable0／control OFF，196份thread TracerPid讀值0，
adbd實讀UID2000；原32份attempt及產品／完整splits SHA不變。

隨後正常force-stop／launcher重開至15495/start499521／UID10067，沒有沿用舊ready或
因觀察失敗重送載入。67開場、68仍載入素材、69真正主畫面分開保存。70未確認啟用與
71舊status實際拒絕，72重新確認完成、73兩項同意後才開始新程序的單次正常App載入。
74是ON_COMMITTED／waiting-cards，不單獨當效果通過。75庫存三種紀念長名完整縮字，
76長名卡圖示及文字完全離屏、77返回完整同名；78三筆探險細分名亦完整。79正常App
OFF後，80探險三筆原花苗名／大字、81庫存原名／字型與原截斷皆還原。

sample-restart-on／off另核同15495/start499521／156.0／ready，enable1／0，最終control
OFF；182份thread TracerPid讀值0（非原子snapshot）、adbd實讀UID2000。產品及兩份
完整156.0 PM SHA不變，原32份attempt逐份不變，新增第33份journal SHA256
`fc47a0a762a1b9956a51007dbc83577dca3a4c0b2c4c45000614307d603a5f97`，七階段至
RETURNED_UNVERIFIED；renderer ready與畫面另驗，不改寫native結尾或歷史UNKNOWN。

本版156.0現有兩個樣本程序的兩頁名稱／OFF、庫存長名與真正庫存離屏返回分項；
探險長字、真正探險整張卡離屏返回、OS／長時間仍未完成，不記整組或九組完成。

## API35／156.0同版本APK的系統重啟回歸

2026-11-10，唯一核准的新index5重新核CLI name／created timestamp／16544。正常OFF、
原33份attempt SHA逐份不變、同15495/start499521／UID10067與thread TracerPid讀值0後，
adbd回UID2000，才正常關閉該instance。CLI另核Android及模擬器程序皆停止後，單次啟動
同一instance；不操作原四台／台中，不把遊戲force-stop當系統重啟。

新boot為 `553dee59-d6dc-4f97-b901-8a2ae3898921`，與前輪不同；實讀API35／x86_64／
libnb.so／原Permissive／boot_completed1／1080×1920無override／density280及adbd UID2000。
同0.5.2/code17與精確156.0/code1792142832完整PM APK SHA均吻合，原33份歷史保留。
control仍OFF、舊status仍指舊PID，沒有自動啟用。Root回讀modules目錄不存在；本輪未安裝
任何套件／模組或更新Root，未啟動研究Root client/instrumentation，不增加產品依賴。

正常launcher開啟156.0，登入保留，新PID2103/start9454／UID10067。01開場與02真正主畫面
分開記錄；03庫存OFF基準為半高面板，04雖檔名含expedition，實際仍在庫存，**不當探險
基準**。06／07分別為半高及展開的探險，08才是三筆花苗皆可見的OFF基準。09回到真正
主畫面後，10未確認啟用與11舊status最終拒絕，不只截到核對中；12普通產品App經su-M
確認新程序，13兩項同意後才單次啟用本程序。

14回報ON_COMMITTED／waiting-cards，不單獨算畫面成功。first-loaded也保留原waiting
時序；稍後first-on另實讀同新PID/start／156.0／ready／enable1與control ON。

- 16／17庫存五週年零食及玩具珠寶長名完整縮字；17首排圖示與名稱皆可見，18這兩張
  卡的圖示與名稱完全離屏，19返回仍完整同名。15只是滾到中段，不冒稱全卡長名驗收。
- 20探險藍色葉子帽苗、紅色三葉花苗、岩石三葉花苗完整；21面板收合不能算真正重用，
  22嘗試拖可見捲軸未產生內容位移，亦不計成功。沒有重跑解析度／滾輪實驗。
- 23正常產品App OFF_COMMITTED後，24三筆探險原花苗名／字型、25庫存原名／大字及
  紀念金苗原截斷皆還原，當次仍25/80；26仍在回主畫面的過渡，27才另核真正主畫面。

最終同新boot／PID/start／156.0／ready／enable0、control OFF，183份thread TracerPid
讀值0（非原子snapshot）、adbd實讀UID2000。原33份attempt逐份不變，總34份，新journal
SHA256 `e6b06c4ca0beb88f178ad62e03ca34e2aec1c827524980240446725316dd31a3`；完整七
階段至RETURNED_UNVERIFIED，ON／OFF兩次SHA相同，不改寫native結尾或歷史UNKNOWN。
本輪完成本版156.0系統重啟後的兩頁名稱／OFF與庫存長名／真正離屏返回分項；探險
長字／真正離屏、長時間及API31／34仍未通過，不稱整組或九組完成。

## API35／156.1新樣本與本版OS重啟回歸

2026-11-10晚至11-11凌晨，兩輪API34 scalar Android失敗並正常關閉後，僅恢復新index5。
先核新boot5efaddbf…8727、API35／x64-libnb／原Permissive／1080×1920無override／
density280與UID2000。舊host盤點器因uname=aarch64而拒絕，原失敗保留；另以自身程序
ELF64/type3/machine62/header64及精確PM SHA核對，不由uname推論原生ARM，不改產品守門。

156.1原簽章base／完整ARM64 split、version/code1793076318、ELF／metadata SHA重核後，
正常install-multiple-r升級，未清資料或複製登入session，產品仍同0.5.2 APK。另一host讀取器
誤要求split有非空versionName，在Root／安裝前拒絕，原件保留；原split本來為空，修正為
base精確version＋split精確code/name／原憑證及binary SHA後，才執行唯一一次標準升級。
原34份attempt逐份不變，control OFF、adbd UID2000；modules目錄不存在。

### 重啟前：新帳號的156.1程序

正常launcher至2463/start55603／UID10067，登入保留，12是真正主畫面。14未確認啟用、
16舊status最終NO_FRESH_SAME_PID_START_STATUS拒絕；15只是核對中。18新兩項同意後
才正常產品App su-M單次載入；first-loaded的ON／waiting-cards保留，不當顯示成功。

22庫存三種紀念金苗長名完整縮字，23整張卡圖示／文字離屏，25完整返回同名；24為部分
返回。27探險藍色葉子帽苗、紅色三葉花苗、岩石三葉花苗皆完整，只有三筆／兩排，尚無
長字縮小或整張卡離屏返回證據。28是收合面板設定；29雖檔名含fixed-half，實際觸控使
面板自動展開，**不是固定半高列表或真正重用證據**。未重跑解析度／滾輪／捲軸實驗。

31正常App重新讀到同PID/start／156.1／ready／enable1，與30先前cached waiting-cards
訊息分開。32普通App OFF後，33探險三筆原名／字型、34庫存原名／大字／金苗原截斷
皆還原。35為跨日安全提醒、36為活動新聞，依本人授權以確定／X關閉，沒有領物品；
37才是真正主畫面。最終ready／enable0／control OFF，186份TracerPid讀值0、adbd UID2000。
原34份SHA不變，新增第35份journal `d63313721e94fd2c2d59d87f98e581fb8858c107cfbf4b80cf109dc3ce37b025`。

### 真正OS關閉／啟動後

正常OFF／歷史35份SHA／原PID/start/UID／TracerPid與adbd UID2000另核後正常關閉。
CLI實際讀到Android與模擬器程序皆false，再單次啟動同index5，不以遊戲force-stop代替。
新boot `4ed5c704-e318-4a07-94d7-2f6df59761d2`、API35／x64-libnb／原Permissive、
boot_completed1／1080×1920無override／density280／自身ELF machine62另核。相同產品與
完整156.1 PM SHA、原35份歷史均不變，control仍OFF、舊status不自動授權；modules目錄
不存在。本輪沒有研究client/instrumentation、額外套件／模組或Root provider變更。

正常launcher至2112/start12127／同UID10067，02確認登入保留，04／05為兩頁OFF基準。
08未確認啟用、09舊status最終拒絕；10重新核真正主畫面，11兩項新同意後才單次正常App
載入。first-loaded仍waiting-cards，12只有主畫面，不記效果通過。稍後14庫存三種紀念
長名完整縮字，15整張卡圖示與名字完全離屏，16完整返回同名；17三筆探險細分名完整。
18普通App新鮮ready／enable1另核，19正常OFF後20探險原名／字型、21庫存原名／大字／
原截斷還原，22為真正主畫面。07是Lens啟動過渡，不作明確checkbox證據。

最終同新PID/start／156.1／ready／enable0、control OFF；181份thread TracerPid讀值0
（非原子snapshot），adbd實讀UID2000。原35份attempt逐份不變，新增第36份journal
`4f04c97ae49497f521a73a65e702baee4dc112d7e5542514fb022801a3e47923`。兩份新journal均
完整七階段至RETURNED_UNVERIFIED，ON／OFF讀到的SHA各自相同；renderer新鮮ready及
真實畫面另驗，不把native結尾或UNKNOWN改寫成成功，也沒有重送不確定載入。

本輪補得同0.5.2／156.1的OS後兩頁名稱／OFF、庫存長名與真正庫存離屏返回分項。
早先同版156.1其他樣本的探險離屏返回證據保留，但不轉記為本次OS後已驗；本次探險
長名縮字／真正離屏、長時間、157本版OS及API31／34仍未完成，不記整組或九組完成。
