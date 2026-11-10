# 森林手帳原生 UI 驗收

2026-11-11，依使用者最新指示先完成森林手帳 UI。Android12／14及九組遊戲驗收門檻不變。

## 0.6.0/code18 首輪

首頁／工具／設定改為可捲動原生介面、dp/sp、安全區及獨立淺深色；首頁使用離線插畫，
工具目前只有花苗名稱透視一個直接開關。啟用在原頁展開兩項當次確認，OFF直接提出。
偏好僅儲存外觀；boot/PID/start確認不持久保存。程序級操作鎖不隨Activity離開而釋放，
原始native lease、版本／SHA／程序守門及UNKNOWN歷史不變。

新呈現模型嚴格解析native回報，ON提交只先呈現等待卡片；新鮮同程序回報才可呈現已啟用。
UNKNOWN／不完整啟用輸出保留重試屏障，OFF或一般讀取錯誤不能清除同程序屏障。

- 原簽章APK SHA256：`4968a7e37b4301967599cb59313754f7f94a4525ab790512e979e20785ea9dbc`。
- 憑證SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。
- 完整建置通過：compatibility55、installed-native29、GameSession27、LensStatus52、
  LensContext17、ZIP layout8／paired digest4；native test只編譯，不當裝置實跑。
- 僅升級安裝新index5／SeedLens-Isolated-A15（created1794181954477010）。實際API35、
  x86_64／libnb.so、原廠Permissive；boot `400309d5-ec04-4a31-b354-68076ca55a35`。
  精確157.0/code1793360009的base／split及原0.5.2 SHA皆先核對，原38筆歷史SHA一致，
  control OFF、遊戲未啟動；安裝用-r保留資料，未操作原四台／台中。
- 已實際看到首頁／設定、淺深色切換與固定三分頁。**首頁插畫未顯示，未通過**；
  APK內圖片SHA與原件一致，仍需修正Android讀圖／顯示。0.6.0原件保留，不覆寫。
- UIAutomator已輸出有效節點，但自身Shutdown thread SIGSEGV／exit139；不是產品崩潰，
  也不能將測試工具整體記為通過。原始截圖、XML及診斷只存私人忽略目錄。

尚未完成320dp、大字、觸控／鍵盤、完整開關同意流程或本版遊戲兩頁回歸。
0.5.2與其他平台證據不轉記成本版資格。插畫來源及完整提示詞見[插畫記錄](UI_ARTWORK.md)。
