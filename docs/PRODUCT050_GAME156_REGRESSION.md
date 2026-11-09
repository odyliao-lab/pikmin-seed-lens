# 0.5.0單APK的API35／156.x回歸（進行中）

沿用[0.5.0原簽章artifact](PRODUCT050_QUALIFICATION.md)的同一APK SHA256 `80f955156b5273d4a34a8a2cf273346362c0e71796d36c06ddae055663951b77`／code15，不重建、不覆寫。既有0.4.1的156.x證據保留，以下只記本版的新實測。API31／34的原遊戲SEVL缺口及所有精確守門不變。

## API35／156.0首程序

使用者本人完成登入並回覆主畫面就緒後，重新核對新index5建立時間／端點，實際API35／x86_64／libnb、KernelSU3.2.5／原Permissive，boot `f41b9c04-8464-49bc-882e-5ea87def0bfc`。PM精確156.0/code1792142832，先前完整base／ARM64 split正常重裝後的SHA與合法intake一致。遊戲PID3804/start49157／UID10066；原19份產品／研究attempt逐份SHA不變，control OFF、TracerPid0。

| 項目 | 私人frames與實際結果 |
|---|---|
| OFF基準 | 07–08庫存29/80，金苗原文／大字截斷；09探險藍／黃／紫花苗等原名 |
| 正常App首次載入 | 確認主畫面與本程序、同意未知呼叫收容後，保持遊戲前景完成完整七階段journal；12同PID/start／156.0新鮮waiting-cards，另以實際畫面驗證 |
| 花苗名稱／縮字 | 13–14岩石／黃色三週年紀念杯子、岩石剪刀、紫色韓國泡菜、黃色髮圈及藍色咖啡杯等名稱；最長紀念名縮字後仍有省略號 |
| 花苗離屏返回 | 15首排圖示與名稱完全離屏，16返回同一細分名稱 |
| 探險名稱／離屏返回 | 17藍色金幣、黃色廚師帽子、紫色貼紙等名稱；18首排花苗完全離屏，19返回同名；20水果底部再次離屏後，21返回首排仍對應同名。另見白色橋樑別針、冰色糕點、冰色三葉、岩石蒲公英及紫色金幣苗；水果原顯示。19／20檔名的lastrow不作末排花苗離屏證據，實際比較為首排 |
| ON狀態 | 22同PID/start／156.0／ready／enable1、control ON，完整journal獨立保存 |
| 正常App OFF | 23正常App OFF；24探險原文／原字型、25庫存大字及金苗原截斷還原；29/80不變 |
| 最終／歷史 | 26同程序／156.0／ready／enable0、control OFF，讀到的thread TracerPid均0；原19份SHA不變，新增一筆為總20，adbd回UID2000 |

首筆journal SHA256：`35c3ed069eb16f8b47b1db0cefdcdfafd49330fcca8721d1bca7ffb9144f95e2`。探險目前樣本最長名稱仍能容納，未把字體變化或庫存縮字冒稱更長探險名已觸發縮字。

UI擷取工具曾在寫出XML後exit139（首次發生在產品載入前）；沒有將其列為工具測試成功或據此重送載入。畫面判讀使用獨立PNG，並另以持久journal／新鮮runtime狀態核對。原XML與退出結果只留私人材料。

## API35／156.0遊戲重啟

正常OFF後以Android標準force-stop／launcher重開同156.0至PID7631/start201500。初次遇本人帳號種花結算，保持OFF並交由本人；本人其後回覆重啟主畫面就緒，並授權後續依畫面關閉同類提示。此授權不變更登入由本人與不領回花苗／派遣／購買的界線。

30／31分別實際拒絕未重新確認的啟用與舊status；32重新確認後，才對此新程序載入。34庫存名稱／紀念長字縮小、35探險細分名稱再次觀察；36探險首排完全離屏，37返回同名。38正常App OFF後，39探險原文／字型與40庫存原大字／金苗原截斷還原。本人操作前後庫存由29變30，不宣稱跨本人操作的數量不變；本次ON／OFF均30/80。

41同PID/start／156.0／ready／enable0、control OFF；185份讀到的thread TracerPid均0（非原子snapshot），adbd UID2000。原20份attempt SHA不變，新增一筆總21，journal SHA256：`98f4630bb57868c0212d67a5e081acd7e6dbfa9a36389988d90ac0330ca7f3db`。

## API35／156.0同帳號OS關閉再啟動

正常OFF後關閉再啟動同index5，新boot `da15de1a-1d01-4e2d-8b57-153da7c53091`，實際API35／libnb／原Permissive、精確156.0及本人登入保留。PM已安裝產品APK SHA與同0.5.0原件相符，原21份attempt逐份SHA不變。

新遊戲PID1900/start2779／UID10066；OS frame04未確認啟用實際拒絕，06重新確認後才同意本次載入。**frame05只拍到「核對中」，不是舊status拒絕成功證據**；該boot不追加此負例通過，前一遊戲重啟的frame31另有實際拒絕畫面。新程序仍須其自身完整journal及新鮮runtime狀態，未沿用舊ready。

08庫存藍色三葉、紫色金幣、紀念金苗長字縮小；10／12探險末排冰色三葉花苗與岩石蒲公英苗同名，11返回頂部時這兩張卡圖示與名稱完全離屏。13正常App OFF後14探險原名／字型、15庫存原大字／金苗原截斷還原；此boot ON／OFF均31/80，不將不同階段的服務端／本人操作變動歸因於產品。

16同PID/start／156.0／ready／enable0、control OFF；187份讀到的thread TracerPid均0，adbd回UID2000。原21份SHA不變，新增第22筆完整journal SHA256：`4bef34e136e20e9be001394f9c28e4929e1e354860014fc83e2f34c2edd7fe69`。私人材料在同目錄`os-reboot-c060504fef614f17ae592da9644b30d4/`。

156.0的同APK兩頁、遊戲重啟與OS分項已有獨立證據；探險更長名稱縮字、長時間與其他平台仍未通過。後續才正常升級156.1保留本人登入，獨立重驗；不把安裝或先前0.4.1結果轉記通過。九組整體目標仍未達成。

本輪原始PNG／XML／PM／receipt僅在ignored `.tools/lab/api35-product050-game1560-d31938f27e304bcd814b0547c04118a7/`，Git不含帳號、遊戲包、截圖、原始日誌或憑證。
