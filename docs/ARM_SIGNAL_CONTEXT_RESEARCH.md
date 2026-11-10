# 自有ARM App訊號context研究

這是原SEVL／WFE不攔截基準之外的獨立能力研究，不修改遊戲或translator，不改產品
守門／主畫面同意，也沒有在遊戲內安裝signal handler。工具契約與來源見
[arm-signal-probe](../research/arm-signal-probe/README.md)，原平台失敗仍以
[API31](API31_GAME_STARTUP_BOUNDARY.md)、[API34](API34_ISOLATED_RESEARCH.md)為準。

## 同一研究APK

`dev.ody.seedlensarmsignalprobe` 0.0.2/code2，原lab憑證633286…8870；非產品簽章。
APK SHA256 `89755ce2c38d26f9412720b027c884093adbc21636b026f5c30d2c3e06151bc9`，
ARM64 SO SHA256 `9261238662a335523f8fd96b3d66818eb96b0f25ba0a187b9b52aaf2ee2ad6fc`。
只打包ARM64 SO，從普通App JNI入口執行；Java先讀自身程序ELF64 header，核對明確
expected_machine，不能由uname或ABI標籤推論原生ARM。

0.0.1簽署原件／source保留但未安裝；0.0.2只增加程序ELF准入及收據，native bytes相同。
94 ELF JVM檢查及10個host parser方法、native／Java／dex／manifest／原憑證、stored
ARM64 SO及16KiB對齊通過。產物與raw收據留在私人忽略目錄，不進Git。

## API35／x64-libnb普通App控制組（2026-11-11）

先完成產品157的有界30分觀察及兩頁OFF還原，才在唯一新index5正常安裝研究APK。
fresh CLI name／created timestamp／16544端點、boot7719af7b…7806、實際API35／x64／
libnb／原Permissive、遊戲2075/start4318／UID10067及control OFF逐次核對。PM讀回
研究包版本／完整SHA、普通App UID10062；產品0.5.2、完整遊戲157及原38份歷史SHA不變。

| 單次新程序案例 | 實際結果與邊界 |
|---|---|
| expected_api=32、035；expected_machine=183 | 各自IllegalArgumentException／instrumentation1，在自有收據及native之前拒絕，沒有新signal檔 |
| baseline，11380 | 實讀自身ELF machine62，普通JNI實際API35；共同sigaltstack／sigaction／alarm設定與取消後返回；native phases1/2/4及Java RETURNED、instrumentation-1 |
| raise，11536 | 自有handler記錄SIGILL，native phases1/2/3；Zygote另實讀own process clean exit86，未Java RETURNED；合成signal觀察，不是指令context證據 |
| UDF，11695 | handler取得記錄，Zygote另實讀exit86。saved PC吻合自己UDF、x19/x20 marker及SP對齊吻合；但si_code=0、si_addr不吻合saved PC，故保留**fault-observed-context-unproven**，不放寬parser或寫成完整context通過 |
| SEVL，11865 | 同setup的自有指令正常返回，native phases1/2/4與Java RETURNED、instrumentation-1；不證明故障context或其他bridge |
| SEVL/WFE，12020 | 同上，正常返回；不轉記API31／34或遊戲資格 |

每例先確認前例自身程序已終止，未重送任何不明案例。raise／UDF的ADB exit0與
instrumentation0／Process crashed不當成功；獨立Zygote只讀與二進位收據分開判定。
handler只記錄後退出，沒有更改PC／context或繼續執行故障指令。

各次收據回讀均驗產品／遊戲SHA、原38份歷史及OFF後恢復adbd UID2000；沒有Root
provider、Zygisk、SELinux或顯示模組變更。probe本身沒有Root／網路權限，只有診斷
回讀自有私人檔案使用adbd Root，不稱其為普通App Root能力證據或使用者安裝條件。

這個結果將API35的PC／GPR吻合與siginfo不吻合分開；它未證明完整架構signal語意，
也未解決遊戲到主畫面前的啟動失敗。下一步在既有獲准API34 x64、再API31各做同APK
獨立對照；該兩平台尚未執行本工具，沒有移植或產品資格。

## API34／x64-libndk_translation獨立對照（2026-11-11）

新index5正常OFF／38份歷史不變／UID2000後已正常關閉，CLI實讀Android及程序皆false。
本輪只啟動已核准的既有`SeedLens_API34_x64`／emulator-5582，2CPU／4GB／原6GiB data，
headless SwiftShader／Vulkan OFF／不使用snapshot。沒有新建AVD、wipe、ramdisk override、
Root provider或SELinux設定變更；原SDK四映像與emulator.exe SHA核對後才單次啟動。

實際API34／x86_64／libndk_translation／Enforcing／adbd UID2000，新boot
`83081800-18a9-4ffb-9b41-423a5cc50ad4`。system_server546/start2019／UID1000，在相隔
99.678秒的兩次讀取均核對當前Power boot、Window system-booted、PM／AM及內建display
state ON／committedState ON，查詢前後server身分不變。這是服務／display state讀值，
不宣稱互動Home或遊戲畫面通過。該ROM沒有讀到舊讀取器要求的mDisplayEnabled欄位，
改讀其實際提供的內建display state；首份metadata與console空白行解析拒絕均保留，
只修正唯讀觀察，沒有重啟guest、改property或以boot flag單獨授予資格。

普通shell實讀自身ELF machine62，原bridge SHA仍a2086fbc…bb3e。原遊戲157的完整base／
split SHA逐次保留、未啟動；產品套件與兩個舊attempt目錄均不存在，前後狀態保留。
通過上述核對後，才正常安裝同一0.0.2研究APK／SHA89755ce2…51bc9，PM版本／SHA吻合、
普通App UID10194；不清資料、不複製帳號。原hint0.0.3留在原處，不替換其不攔截基準。

| 單次新程序案例 | API34實際結果 |
|---|---|
| expected_api=32、034；expected_machine=183 | 各自instrumentation1／IllegalArgumentException；在自有收據及native前拒絕，無新signal檔 |
| baseline，5718 | 自身ELF62、普通JNI實際API34，共同sigaltstack／sigaction／alarm設定及取消後正常返回；native1/2/4、Java RETURNED與instrumentation-1 |
| raise，5885 | 合成SIGILL的handler觀察，native1/2/3；獨立Zygote讀回自身clean exit86，不是指令context資格 |
| UDF，6044 | 自有故障進入handler，保存PC吻合已知UDF、兩個GPR marker與SP對齊；si_code=-6、si_addr落在fault PC前12 bytes，故完整context未證實；Zygote另核exit86 |
| SEVL，6203 | 進入handler後退出，未正常返回；PC／marker／SP對齊分項吻合，但同樣code=-6／address差12 bytes，維持fault-observed-context-unproven；Zygote另核exit86 |
| SEVL/WFE，6393 | 停在第一條SEVL，同上；handler後exit86，**尚未執行WFE**，不能推論WFE本身失敗或可用 |

三個指令fault的六項判讀中，instruction mode、own PC、兩個marker及SP對齊吻合，
instruction signal code及signal address不符；保留原二進位收據和未證實判定，沒有放寬
parser，也沒有改PC／context、略過指令或在遊戲安裝handler。每例只執行一次且自身程序
終止後才進下一例，沒有因instrumentation0或ADB0補成功／重送。三秒alarm未在這輪觸發，
不冒稱已執行timeout控制；本輪驗到的是自有handler記錄後的有界自身exit86。

每例收據回讀後均實讀adbd UID2000／Enforcing／同boot；診斷adbd Root不當普通App Root。
最後server546/start2019仍相同，沒有game／probe程序，產品仍未安裝、原遊戲157 SHA與
兩個attempt目錄的不存在狀態不變。正常指定console關閉後，另核host／5582及5583皆
消失，原SDK四映像SHA不變、資料保留，index5亦維持關閉；未操作原四台或台中設備。

API34本輪新增的是普通App訊號觀察／失敗退出證據，沒有原遊戲正常啟動或產品兩頁資格。
下一步API31必須另做同APK對照；不得從這個image的code／address／PC分項交叉推論。

## API31／x64-libndk_translation獨立對照（2026-11-11）

API34正常關閉且host／埠消失後，才單次啟動既有獲准的`SeedLens_API31_x64`／5580，
2CPU／4GB／原6GiB data，headless SwiftShader／Vulkan OFF／無snapshot。既有SDK的
source.properties寫12，但package.xml／官方catalog為**revision14**；官方r14 ZIP的同檔
也寫12，不能從該欄位推論這是舊套件（後續中繼檔核對見下）。本輪未更新image，四映像SHA
先核，私人Magisk31.1 ramdisk override仍為已核`5537a63d…eb20`，沒有新Root安裝、模組、
SELinux或主機設定變更。

新boot`96f23546-66e0-4485-a382-c71eb061c924`實讀API31／x86_64／libndk_translation／
Enforcing／adbd UID2000。當前Power／Window boot、PM／AM、內建display state ON與
同system_server533/start2326／UID1000，相隔75.547秒另核。API31該display dump沒有
API34的committedState欄位，觀察器改讀這個ROM實際提供的state ON，沒有把早期boot
未完成讀值當通過，也沒有因此授予互動Home／遊戲／產品資格。自身shell ELF62與原bridge
SHA c05f2b7e…db51另核，未以uname判斷ARM64。

與API34不同，這台原產品0.4.1/code14仍存在；原SHA815de868…1cce、精確157.0的base／
完整split SHA及原16份研究attempt逐份SHA，每次診斷回讀均保留，產品attempt仍不存在。
沒有啟動遊戲或透視鏡、不清資料／複製帳號；同研究APK0.0.2／SHA89755ce2…51bc9正常
安裝後，實際普通App UID10152／自身ELF62，沒有把adbd Root算作普通App Root資格。

| 單次新程序案例 | API31實際結果 |
|---|---|
| expected_api=32、031；expected_machine=183 | 各在自有收據／native前以IllegalArgumentException拒絕，instrumentation1且沒有新signal檔 |
| baseline，5868 | 普通JNI／共同handler及alarm設定後正常返回；native1/2/4、Java RETURNED、instrumentation-1 |
| raise，6086 | 合成SIGILL記錄1/2/3，獨立Zygote確認自身clean exit86 |
| UDF，6268 | handler記錄後exit86；已知PC／兩個GPR marker／SP對齊吻合，但si_code=-6、si_addr在PC前12 bytes，完整context未證實 |
| SEVL，6449 | 同樣有界記錄／exit86，指令未正常返回，context分項同上，維持未證實 |
| SEVL/WFE，6630 | 第一條SEVL就進handler並exit86，WFE尚未執行；不能由這例推論單獨WFE的能力 |

四個signal程序的exit86均另讀Zygote確認，不以ADB0／instrumentation0替代；三個指令fault
仍由原parser判為`fault-observed-context-unproven`。沒有改PC／context／指令／translator，
沒有在遊戲安裝handler，也未觸發三秒alarm，故不宣稱這輪實測了timeout控制。

最後同boot／server、adbd UID2000／Enforcing，Game及probe都已不在；原產品、遊戲、16份
歷史仍不變。指定AVD的正常console關閉只發一次；第一個host觀察尚有SDK正常關閉helper，
保留讀值、等待退出，不把close command返回當terminal。後續另核SDK程序與5580／5581
皆消失，四份SDK映像及私人ramdisk SHA不變，原資料保留，index5亦維持關閉。

同APK的API31／34／35普通App對照現已各自執行，但API31／34原遊戲啟動缺口仍未解，
沒有新增產品資格。下一個可分離的研究問題是**WFE單獨執行**與**實際逾時退出控制**；
兩者在0.0.2尚無執行證據，不能由SEVL/WFE組合或alarm設定成功補推。

## 0.0.3：獨立WFE與alarm控制的來源準備

保留0.0.2原APK，研究套件另增0.0.3/code3，仍用原lab簽章；產品0.5.2及守門不變。
只新增`wfe`的自有單指令函式，以及`alarm`在自有程序完成共同setup後等待三秒SIGALRM
的負控制。後者預期由系統終止，parser只把1/2收據列為armed-unconfirmed，必須另核
自身PID的實際signal14終止，不能以檔案缺尾段、instrumentation失敗或ADB返回猜成功。
沒有新增context寫入／續跑、遊戲handler、Root能力或其他程序存取。

12個host parser測試方法、94個ELF JVM檢查與原lab簽章／Java／dex／ARM64／16KiB封裝
通過，反組譯另核自有WFE及其後restore／return；原0.0.2 APK／SO不覆寫。新APK
SHA54265523…b886、SOed0dc066…a767，完整pin與操作方式見研究README。此節是host準備，
**尚未在裝置安裝或執行0.0.3**，沒有新增平台或產品資格。

## API31／0.0.3：WFE單獨故障與實際alarm退出（2026-11-11）

先核0.0.3新parser回讀三平台共15份0.0.2真實native收據，其完整decoded結果逐份與
原紀錄相同，沒有改寫未證實判讀。舊研究APK及產品0.5.2原SHA亦不變。然後在既有API31
同隔離AVD另次單次冷啟動，實際boot`f16e455b-69a9-4bfe-8cf3-cd11b5f649c9`、API31／
x64-libndk_translation／Enforcing／UID2000；同server560/start2179及當前服務／display ON
相隔72.959秒另核，未以早期system_server尚不存在的拒絕讀值代替就緒。

0.0.2正常升級為同lab簽章0.0.3，PM精確版本及完整SHA吻合，普通App UID10152實讀
自身ELF62。原十個0.0.2自有bin／tsv逐份SHA、原16份研究attempt、產品0.4.1與精確157
完整原件逐次保留；Game／Lens未啟動。第一個host檔名對照器使用錯誤路徑分隔符而拒絕，
當時沒有發出install呼叫且已unroot2000；保留原拒絕收據，只修正host路徑讀取後才單次
安裝，不是重送不確定安裝或載入。

| 單次自有案例 | API31實際證據 |
|---|---|
| wfe-invalid | 在收據／native前IllegalArgumentException拒絕，沒有新檔 |
| baseline，4701 | 共同setup返回；native1/2/4、Java RETURNED／instrumentation-1 |
| WFE單獨，4916 | 獨立自有WFE進handler，native1/2/3、未正常返回；原parser仍context-unproven，Zygote另核clean exit86 |
| alarm，5130 | 刻意等待三秒後，instrumentation實際耗時3.391秒，自身程序消失；native只有1/2，parser仍只列armed-unconfirmed；**獨立Zygote明確記錄signal14終止**才確認這次自有逾時退出 |

WFE這輪獨立實測補上0.0.2組合尚未走到的指令；alarm這輪亦實際觸發，而非只成功設定。
這不代表ARM WFE事件語意、完整context或遊戲啟動已相容，signal14也不是native成功返回。
沒有PC／context修改、替換bridge、遊戲handler或產品守門變更。最後同boot／server、無
Game／probe程序、adbd UID2000／Enforcing；指定console正常關閉一次後，另核host／埠
消失、原SDK四映像及私人Magisk ramdisk SHA不變，index5仍關閉。API34／35尚未執行
0.0.3，不能由API31跨平台推論。

## API34／0.0.3獨立WFE與alarm對照（2026-11-11）

API31正常關閉後才啟動既有API34 x64／5582，沿用原SDK／data與同SwiftShader參數，
無Root provider或其他平台變更。實際boot62dc3e1a-701e-4dde-be85-98bb6c6cd3fb／API34／
libndk_translation／Enforcing／UID2000；同server546/start2030及當前Power／Window／
display ON／committedState ON在相隔81.447秒的兩次讀取另核。正常將0.0.2升為相同
0.0.3／原lab簽章／完整APK SHA，普通App UID10194實讀自身ELF62；原十份0.0.2 bin／tsv
逐份SHA、產品不存在／原157完整SHA／attempt目錄不存在的狀態逐次保留。

四例各新程序／單次執行且收據完整回收：wfe-invalid在native前拒絕；baseline PID4607
正常1/2/4返回；**WFE單獨PID4809**進handler，已知PC／marker／SP分項吻合但code=-6、
address在PC前12 bytes，原parser維持context-unproven，Zygote另核clean exit86。
刻意alarm的PID5001實際instrumentation耗時3.406秒，native僅1/2、parser仍
armed-unconfirmed；另由Zygote明確記錄自身signal14終止才確認本次自有逾時退出。

未啟動Game或安裝產品、未改PC／context／bridge／產品守門。最後同boot／server、
adbd UID2000／Enforcing、無Game或probe程序，指定console正常關閉一次後，另核host／
5582與5583皆消失、原SDK四映像SHA不變，index5仍關閉。API35尚未實跑0.0.3，
兩個libndk_translation平台的結果也不能代替
libnb或原生ARM64的指令語意。九組及三階段未因此完成。

## API31套件revision與archive內部欄位核對

先前只讀source.properties的12而稱其「不是最新rev14」並不精確，現已更正上述紀錄。
Google官方[catalog](https://dl.google.com/android/repository/sys-img/google_apis/sys-img2-3.xml)
的API31 google_apis/x86_64是revision14、archive x86_64-31_r14.zip／1,470,822,277 bytes；
本機同套件package.xml的revision major也為14。

另對[官方r14 archive](https://dl.google.com/android/repository/sys-img/google_apis/x86_64-31_r14.zip)
只作有界HTTP206 range讀取中央目錄及兩個小檔，總3,322 bytes，ZIP解碼／CRC通過；
沒有下載或解壓系統映像、更新SDK、新建AVD或操作另一台裝置。官方archive內的
source.properties同樣寫Pkg.Revision=12，其SHA256為117fe498…be68；build.prop為API31／
Android12、build SE1A.220826.008／incremental10564458，SHA67a0723b…46d。
這兩個檔在本機SDK的完整SHA各自與上述內容相同。

這項證據只釐清catalog／package revision與archive內部properties的差異，不是整個遠端
archive digest或所有映像byte相同的驗證；不能把source.properties的12當成另有較新
translator的證據，也不能把revision14當SEVL／WFE或產品相容通過。

## API35／0.0.3獨立WFE與alarm控制（2026-11-11）

前輪SDK guest正常關閉後，重核無SDK emulator／5580-5583 listener、專用WSL無QEMU
及可用RAM，才單次正常啟動原新index5（名稱與created timestamp另核）。CLI實讀端點
127.0.0.1:16544，只連該端點、未重啟ADB server。新boot
803b1862-af66-41e0-a372-59bf3dba547c／API35／x86_64／libnb／原Permissive／UID2000；
同server807/start585／UID1000及當前Power／Window／內建display ON／committed ON，
相隔85.922秒另核。API35 dump使用FLAG_ALLOWED_TO_BE_DEFAULT_DISPLAY；首個依舊flag
過濾的唯讀拒絕保留，修正實際欄位後才繼續，未重啟guest或改產品守門。

同0.0.3 APK／SHA54265523…b886正常升級0.0.2，普通App UID10062／自身ELF62。原十份
自有0.0.2收據、38份產品／研究attempt逐份SHA、產品0.5.2及精確157完整原件逐次不變；
控制OFF、Game／Lens未啟動，不清資料、不複製登入或憑證，各案例只發一次。

| 自有案例 | API35實際結果 |
|---|---|
| wfe-invalid | 在收據／native前拒絕，無新檔 |
| baseline，2750 | 共同setup正常1/2/4返回，Java RETURNED／instrumentation-1 |
| 獨立WFE，2944 | 自有指令正常1/2/4返回，Java RETURNED／instrumentation-1，非完整事件語意或signal context資格 |
| alarm，3138 | 刻意等待instrumentation3.828秒；native僅1/2、parser仍armed-unconfirmed，另由Zygote確認自身signal14終止 |

沒有PC／context／bridge／產品變更或Game handler。最後同boot／server、UID2000／原
Permissive、Game／probe不在、38份歷史不變且控制OFF；指定index5正常shutdown一次後，
另核CLI Android／process兩者false，SDK guest與埠亦不在。原四台及台中不操作。

| 相同0.0.3指令／控制 | API31／libndk_translation | API34／libndk_translation | API35／libnb |
|---|---|---|---|
| 獨立WFE | fault、exit86，context未證實 | fault、exit86，context未證實 | 自有指令返回 |
| 刻意alarm | 另核自身signal14 | 另核自身signal14 | 另核自身signal14 |

這是三平台各自的普通App研究分項，不是原生ARM64、完整context或遊戲啟動修復資格；
沒有新增消費者依賴或產品九組通過結論。

## 0.0.4自有UDF受限回復：host準備

既有觀察不能證明公共PC欄位寫入後會被translator採用，故另用新研究0.0.4/code4，只測
自有UDF的已知位置與下一指令，不在遊戲安裝handler、略過SEVL/WFE或改產品准入。
原七個觀察／alarm模式不改語意；兩個新模式共用精確own PC、siginfo/context、
signal code、兩個GPR marker與aligned SP守門。負例故意改自身x19，預期在寫入前exit89；
第二次handler一律exit88，不得迴圈重試；仍有三秒alarm與host有限觀察。

正例先耐久記錄write intent，再僅將自身public PC設為已知UDF+4；landing的原生canary
與phase4、Java RETURNED和實際terminal須另核。只有intent／中途收據仍UNKNOWN，
不能把synthetic signal-code／address當完整context，或把UDF未執行而續跑當原指令成功。
NDK公開欄位與前述kernel self-test提供原生介面依據，不保證NativeBridge round trip。

16個host parser方法、94個ELF JVM檢查及24份真實舊收據的逐份完整decoded回歸通過，
原lab簽章／Java／dex／ARM64／16KiB封裝通過；反組譯另核兩個UDF+4 canary及負例marker。
新APK SHA66d3e87b…98b3、SOcf0b74cc…886e，原研究版本與產品0.5.2 SHA保留。
這是host準備，**尚未安裝／實跑0.0.4**，沒有產品整合或新平台資格。
