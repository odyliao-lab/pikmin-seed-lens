# 0.5.1/code16長名稱候選：探險字型回歸，未通過

0.5.0/code15及其原APK／證據保留。0.5.1/code16已完成unsigned與原簽章建置，2026-11-10以正常更新安裝至新index5，PM讀回APK SHA與原簽章產物相同；畫面驗收進行中，不能把0.5.0的兩頁／重啟證據轉記本候選成功。

0.5.0的API35實測已觀察最長紀念庫存名縮字後仍省略。程式原本將IchigoText AutoSize下限設為原字體75%（至少12），因此原字體24只能縮至18。候選維持既有12點可讀底線，讓遊戲在12到原大小間自行選最大的可容納字體；不固定把每張卡縮到12。

只調整有實際名稱替換之title的AutoSize下限。原max font、精確runtime metadata／game guards、未知資料／語言保持原文、卡片重用與FontState保存／正常OFF還原規則不改。更低下限仍不保證任意長名稱都能容納，必須看實際畫面。

後續先跑既有policy／FontState還原及必要建置檢查，再以原產品簽章的新APK在新index5實看長名、短名、離屏重用、兩頁OFF及新遊戲程序。探險更長樣本、Android12／14／15 × 156.0／156.1／157仍須各自取得證據；新版本不能繼承舊APK通過項目。任何signed artifact使用唯一新路徑，不覆寫0.5.0，亦不公開發布或合併main。


## 已完成的準備檢查

- Linux host g++13以既有C++ policy／FontState套件實際編譯並執行exit0；這不是Android或畫面驗收。
- PowerShell7／JDK17／NDK27d／CMake3.22.1／Ninja明確路徑，JVM compatibility／PM digest／session／APK layout套件實際執行通過，兩ABI native建置及stored／16KiB ZIP／配對renderer digest檢查通過。其他Android native測試程式只編譯，不冒稱新device run。
- unsigned candidate SHA256 `d540cd7e6b86d4c86e70352981e81bcae143dd9a21d0f8ce8154b22655645d9e`，不安裝。
- 原簽章正式建置輸出SHA256 `706ac9187d2cf1d027f08c047081270c671f161a68834f880cb03fe38d7d4599`；aapt實際versionName0.5.1／code16，apksigner實際certificate SHA256 `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。
- 本版使用default Java/native設定，x64仍僅API35/libnb；沒有以字型修正放寬其他NativeBridge。ARM64及其他平台仍須真實驗收。

原簽署環境從本專案先前已使用的私人來源恢復，只在簽署child env供值，未輸出／寫Git或傳入Android；原key與0.5.0 APK不變。signed artifact在ignored `.tools/build/0.5.1/signed/`，不是公開發布。接續只操作新鮮核對的index5；在完成真正兩頁及OFF／重啟前，本版仍未驗收。

## 安裝與歷史保存

新index5的建立時間、名稱、CLI端點及boot重新核對；實際API35／主要ABI x86_64／libnb.so／原Permissive。kernel uname雖回報aarch64，仍按x64／NativeBridge評估，不記為原生ARM64。普通研究App以自身身分再次取得su-M；adbd Root只用於唯讀歷史／安裝SHA核對，已回讀UID2000。

更新前後合計25份attempt（18份產品、7份PM研究）逐份SHA不變。初次只列產品目錄得到18份的稽核不完整，保留該私人原件；補列兩個既有目錄後與先前25份final-state完全一致，沒有遺失或補造紀錄。正常install-r後PM versionName0.5.1／code16及base.apk SHA均相符，未清除App或遊戲資料。

## API35／156.1實際結果：不通過

同boot `4868c40f-4546-42d1-beb8-ae744694fc76`，精確156.1/code1793076318，PID3367/start113475／UID10066。本人既有登入保留；安全提醒及活動公告按本人授權關閉，未領回花苗或派遣。正常App先實際拒絕未勾選啟用（frame17）及舊status（18，明確NO_FRESH_SAME_PID_START_STATUS），再重新核對本程序與收容同意，單次載入（21）。waiting-cards不列為顯示成功。

| 項目 | 實際結果／私人frame |
|---|---|
| 庫存基準 | 14原文／大字及紀念金苗截斷，33/80 |
| 庫存長字及重用 | 25完整顯示岩石／黃色三週年紀念杯子蛋糕金苗，短名維持較大字；26兩張長名卡圖示與文字完全離屏，27返回仍完整同名 |
| 探險回歸 | 28、29穩定畫面：原本短名稱也變成「藍色…」「黃色廚師…」「紫色…」；關頁重開後32仍截斷，不記名稱或縮字通過。30是動畫，不作穩定證據 |
| 正常App OFF | 33 OFF提交；34探險還原原花苗名與大字，35庫存還原原文／大字／紀念名截斷，當次33/80 |
| 最終 | control OFF、同PID/start／156.1／ready／enable0；讀到thread TracerPid皆0，adbd回讀UID2000；隨後正常force-stop本遊戲，沒有再次載入0.5.1 |

原25份attempt逐份SHA不變，新增第26份完整本程序journal SHA256 `9435009988bf442310f67ab0f3c7887c81da840a288a4dbc013224baf3ef6a3a`。本版沒有完成探險重用／遊戲重啟／其他版本驗收。0.5.1原APK永久保留為本次失敗候選，不冒稱修復或用同版不同bytes覆寫。

源碼接續準備0.5.2/code17：庫存仍用12點下限，探險／詳細頁先恢復先前75%下限；不同layout的適用性須實看。這是依已觀察回歸限定修正範圍，未宣稱已證明遊戲AutoSize內部根因或任意探險長名都能容納。
