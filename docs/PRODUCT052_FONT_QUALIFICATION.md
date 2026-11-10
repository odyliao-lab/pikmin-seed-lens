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
