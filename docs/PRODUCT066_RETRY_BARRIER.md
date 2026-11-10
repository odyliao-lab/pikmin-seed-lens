# 0.6.6：異常退出與禁止重送的介面屏障

2026-11-11。0.6.5的正常載入／兩頁／OFF已有分項證據，但介面對一般非零退出皆呈現REFUSED，
未充分區分被signal中止、無法辨識的native退出，以及native已明示NO_RETRY。原native持久
attempt／lease仍守住不重複載入；本次改善介面提示與再次提出請求的門檻。

## 行為

- ENABLE已進入native的回報若不是可識別的受控拒絕，或程序以signal／負值異常退出，
  顯示結果尚未確認，保留boot/PID/start屏障；不能以一般失敗提供重送入口。
- durable attempt已存在、native NO_RETRY、不同已載renderer或其他必須換程序的退出，
  顯示「這次載入已停止」，鎖住啟用並要求先重開遊戲再核對；與UNKNOWN分開呈現。
- UNKNOWN及結果尚未確認明說「不代表顯示已關閉」。同程序OFF或新鮮只讀狀態都不能
  清除屏障；新的boot/PID/start仍須只讀核對及兩項新確認。
- 屏障phase持久保存、嚴格接受三種阻擋狀態，並相容先前ui-uncertain偏好；不保存啟用同意。
- Root權限拒絕等完整的native入口前錯誤及一般READ失敗，不一律稱為UNKNOWN。

native載入、版本／SHA／metadata／NativeBridge守門、timeout及原始歷史完全不變。

## 產物與已驗範圍

原簽章0.6.6/code24，APK SHA256：
`488904a73e38f03ebfda8293d5535de792836af09e9708edf912f66f0f7e5a79`。
完整建置通過：compatibility55、installed-native29、GameSession27、LensStatus96、
LensContext17、ZIP layout10／paired digest4。API31／34旗標仍皆false；native tests只編譯。
96例含signal／缺少最終marker／NO_RETRY／lease不明／受控拒絕、持久phase選擇、
OFF及只讀不能清除同程序屏障，以及新boot／程序觀察的各自邊界。

此處是來源／建置與協定驗證，不是對遊戲製造signal，也不是完整裝置驗收。
下一步在同index5用真實APK升級／舊renderer路徑衝突驗拒絕、屏障持久性、OFF／只讀不清除，
再正常重開遊戲驗新程序與兩頁回歸。0.6.5原件不覆寫、畫面資格不轉記本版。
