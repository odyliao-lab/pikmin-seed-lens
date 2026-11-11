# 0.6.7：屏障偏好格式與實機持久化

2026-11-11，修正[0.6.6實測未通過的App重啟](PRODUCT066_RETRY_BARRIER.md)。
舊值尾端的LF在實際偏好XML讀回後附有4個縮排空白，不能交由嚴格runtime parser直接解析。

## 修正

- UI偏好另用無換行／tab的`v2:PID:start:boot`，每一欄仍回到原嚴格parser驗證。
- 舊格式只遷移完整有效record或本輪測得的LF＋4空白；不做任意trim、不放寬runtime輸入。
- 原legacy值及native append-only歷史保留；新值優先，無效新值不能退回舊scope繞過檢查。
- 已存在卻無法解碼的scope保持不確定／禁止啟用；先只讀核對目前程序建立保守基準，
  再手動重開遊戲後核對新程序。OFF或一般錯誤不能清除此限制。
- 不保存啟用同意、不加入網路／背景服務／模組，不更動native版本、SHA、metadata或ABI守門。

原簽章0.6.7/code25，APK SHA256：
`adfad3a8af7273870b0c220e68ba009347b06a19ace36121f8e60cff339c81e4`。
完整建置：compatibility55、installed-native29、GameSession27、LensStatus102、LensContext34、
ZIP layout10／paired digest4通過。新增實際舊XML形狀遷移及無法解碼scope的保守呈現檢查。
native tests仍只編譯，API31／34旗標仍false；非產品相容性驗收。

## 實際偏好遷移／App重啟／只讀及OFF

僅新index5／SeedLens-Isolated-A15，created1794181954477010，API35／x86_64／libnb.so。
boot400309d5…5a35；原157.0/code1793360009遊戲PID12384/start296654/UID10067，
仍保留0.6.5已正常載入的舊renderer。0.6.6已真實得到stage26拒絕，沒有混用新renderer。

- -r升級0.6.7後，未再提出啟用，舊值正確恢復「這次載入已停止」。
- 實讀v2值與原scope完全一致、沒有尾端空白；原legacy字串保留。
- 正常force-stop並重開產品App，App PID15312→15708、遊戲身分不變，屏障仍恢復。
- 先回遊戲取得新鮮狀態再只讀：實際native回報同程序v1／control0，介面仍保持屏障。
- 首頁OFF得到實際OFF_COMMITTED ACK，工具Switch仍disabled，未被ACK誤清除。
- 原41筆journal SHA全部一致，沒有新增attempt或重送載入；Root稽核後adbd UID2000。

這是實際APK升級衝突／偏好及交易呈現驗證，不是假造native UNKNOWN，也未對遊戲製造signal。
signal與其他不完整回報仍只列協定測試。舊0.6.6產物及持久化失敗證據不覆寫。

## 新遊戲程序恢復與兩頁實測

正常關閉舊遊戲後，新157程序為PID16590/start404069/UID10067，boot不變。
舊屏障先保留，明確只讀核對新程序後才允許重新同意；兩項重新勾選、只送出一次啟用。
native正常返回，另以新鮮renderer狀態及實際畫面確認，沒有以返回碼代替顯示驗收。

- 庫存25個保持不變；五週年紀念零食、玩具珠寶及三週年杯子蛋糕金苗完整長名縮小。
- 同一批庫存整張捲出畫面，再返回仍正確命名／縮字，保留真正離屏畫面。
- 探險三筆短名為藍色葉子帽苗、紅色三葉花苗、岩石三葉花苗；水果／禮物未改名。
- 工具頁已啟用Switch直接OFF，沒有第二層頁面或再次同意；兩頁實際恢復原名與原字級。
- 最後native為同PID/start、157.0、ready/control0，全部thread TracerPid0；adbd還原UID2000。
- 原41筆SHA不變，正常新程序新增一筆，合計42筆；ON後至OFF後全部SHA相同。
  新journal SHA256為`4a7215d5c2b8deb8f723542610c1f8efe994a625e044ac179ee40f01d652e428`。
  journal仍是RETURNED_UNVERIFIED，沒有改寫成成功或重送不確定載入。

本輪只有三筆短名探險，尚不足以證明探險長名縮字或整張離屏重用。
TalkBack、其他兩個遊戲的本版回歸及Android12／14仍分別待驗；九組及整個UI階段未記完成。
