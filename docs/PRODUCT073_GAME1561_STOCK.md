# 0.7.3／156.1待種花苗追加驗證（2026-11-11）

使用者指定MuMu內建12及15搭配156.1，仍只驗待種花苗ON／OFF還原／再ON，探險略過。
使用同一已發布原簽章0.7.3/code29，不改APK、遊戲profile或精確程序守門。

## 第一階段：MuMu Android15／API35

僅既有新index5／SeedLens-Isolated-A15／created1794181954477010／16544。
新鮮核對boot05785e90-b24c-4509-9e57-2aac6708cf22、API35/x86_64/libnb、原Permissive。
先從產品UI正常OFF，保留原48份歷史。一般ADB -r -d被VERSION_DOWNGRADE拒絕；
改由已授權既有Root執行標準PM create/write/commit -r -d成功，不清資料、不複製登入。
沒有修改遊戲APK或系統debuggable屬性。已保留登入並正常進入156.1主畫面。

精確156.1/code1793076318；原簽章base SHA
`070c75d6e253317438dc4ac1b0f60b5dbf9e5250763827b7c6b6197f0e81e9dc`，完整ARM64 split SHA
`7b05aa1c10acad9266c1daeea68d466c138b2ecb35a9d03717af273dd13e4836`。
透視鏡SHA仍`c7c72e89515e0bfb69c9cc720a7316ecb20107183e0ff816333dd29ea0a87721`。

第一程序PID9872/start283329/UID10068，單次載入停於ATTACHING，journal END UNKNOWN，
未到CALLING、control OFF、無renderer；SHA
`f12b9a61e256babdd7e38b6667a99b9ec4913f06b13bf167e2c7050590b645f4`。
該輪測試在啟用後4秒切回App，可能影響前景程序；不能據此確認根因。
保留該UNKNOWN且不對同程序重送，正常重開遊戲後另用新程序驗證。

第二程序PID11761/start307278/UID10068。重新READ及兩項當次確認後，只送出一次載入，
遊戲保持前景22秒後才回App。成功waiting-cards，開花苗頁後ready1。
357庫存名稱替換、周年長名縮小、OFF原名／省略／字級還原、同程序再ON均實測成立。
照片鈕扣徽章卡片首輪仍有省略號，等待及切回同頁仍在；OFF／再ON後完整顯示且縮小。
因此保留首輪文字版面限制，不宣稱首輪所有卡片的全名都已通過。

正常新增journal SHA `90855ee410156dc0243b93c49a957a110f90ae2c601965841f3db3a4ab80a886`，
OFF／再ON不再載入，SHA保持相同；本次共原48＋首輪UNKNOWN1＋正常1＝50份，
既有歷史及UNKNOWN均未修改。最終control ON／ready1，194個thread TracerPid0、adbd2000。
Root及產品由普通App正常UI執行，不依賴研究APK或顯示模組；診斷adb root不代為載入。

原始證據僅保留ignored `.tools/lab/product073-api35-game1561-e30465b107714dd58c575aa5099c7770`，
正常回歸在其fresh-process-2；原四台及台中未操作，沒有領取／派遣／物品寫入。

## 第二階段：MuMu內建12／實際API32

待另行實機驗證；本階段結果不承接API35，也不代替必要API31。
