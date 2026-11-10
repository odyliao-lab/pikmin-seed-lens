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
