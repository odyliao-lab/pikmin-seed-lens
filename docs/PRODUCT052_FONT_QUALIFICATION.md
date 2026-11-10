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
