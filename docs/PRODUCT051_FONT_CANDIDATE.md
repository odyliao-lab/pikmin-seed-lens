# 0.5.1/code16長名稱候選：未交付、待實測

0.5.0/code15及其原APK／證據保留。當前source準備0.5.1/code16，已完成unsigned與原簽章建置，尚未安裝／實看；不能把0.5.0的兩頁／重啟證據轉記本候選成功。

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
