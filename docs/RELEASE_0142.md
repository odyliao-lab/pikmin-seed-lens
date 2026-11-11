# 0.14.2／code53

依使用者2026-11-12授權，先合併PR #17，再同步README／App更新索引；最終main的Offline policy CI通過後，才公開本版預覽。使用既有已測同一份APK，不重建或覆寫舊版。

- 緊湊五個功能列、橫排小圖示標題、按需展開說明、固定底部操作與版本；[版面比較](COMPACT_UI.md)。
- 隔離MuMu API35，360×640dp、原視窗、深淺色與1.3倍字體已驗五個開關及操作可見。說明展開／捲動、GPS開關及關閉全部已操作。
- 相鄰列高度約146→61dp，縮減約58%；不是宣稱所有裝置／長錯誤／展開說明皆完全免捲動。
- 原簽章完整建置、80項controller及既有JVM／APK封裝檢查通過。控制器與原生邏輯未改，本輪未開遊戲；原功能實測依[0.14.1紀錄](FEATURE_INTENT.md)，不增加手機或遊戲資格。
- APK：26,642,042 bytes；SHA256 `799d3aef43930385611cabe84657c70738990943f2e24df1cea8a82d184213b8`。
- 原簽章SHA256 `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。
- 已測程式來源 `8559af1`；其後僅文件、更新索引與測試期望值調整。來源tag建立於最終main。

[0.14.2發布頁](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.14.2)。README與main更新索引指向同一份APK，舊版tag與資產保留不改寫。
