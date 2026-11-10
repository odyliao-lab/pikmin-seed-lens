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
