# 0.5.0/code15隔離驗收（進行中）

本版加入精確157.0/code1793360009的來源設定檔；保留156.0／156.1與所有程序、ELF、prologue、runtime metadata／UNKNOWN守門。API34 x64沒有放行。新簽署位元組只安裝新隔離MuMu index5，不是公開發布或九組完成。

- APK SHA256：`80f955156b5273d4a34a8a2cf273346362c0e71796d36c06ddae055663951b77`，19281214 bytes。
- 原簽章cert SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。
- 明確`-Api31Qualification`變體：API31/libndk及API35/libnb；預設仍API35，API32／34 x64仍拒絕。安裝／編譯不構成資格。
- Python16、三版來源39個破壞負例通過；預設及qualified完整建置各48／29／27／8／4檢查通過，兩ABI native編譯／16KB／stored ZIP／同批配對通過。
- API35 x64自有control fixture76項實跑通過；ARM64只有編譯，未裝置執行。fixture不載入／改寫遊戲，16份歷史attempt SHA不變。

## API35／157.0首程序

新index5重新核建立時間及端點，實際API35／x86_64／libnb、KernelSU3.2.5／原Permissive；boot `1774a036-4821-48f3-9eec-e31e64252589`。精確157.0由完整原簽章splits正常升級保留本人登入；本人處理帳號獎勵後才測。繁體中文透過遊戲正常設定切換，沒有改遊戲檔案或登入資料。

PID2033/start22615／UID10065，正常App重新確認主畫面並同意未知呼叫收容；載入時保持遊戲前景。完整七階段journal止於RETURNED_UNVERIFIED，另以新鮮同PID/start／157.0狀態及畫面驗證，不把journal當UI成功。

| 項目 | 私人frames與實際結果 |
|---|---|
| OFF基準 | 34庫存29/80、金苗原文大字截斷；35探險藍／黃／紫花苗等原名 |
| 花苗名稱／縮字 | 39–40岩石／黃色三週年紀念杯子、岩石剪刀、紫色韓國泡菜、黃色髮圈、藍色咖啡杯等細分名稱；最長紀念名已縮字但仍有省略號 |
| 花苗離屏返回 | 41首排完全離屏，43返回相同細分名稱；中間42只回到中段，沒有冒稱已回首排 |
| 探險名稱 | 44／47藍色金幣、黃色廚師帽子、紫／藍貼紙、白色橋樑別針、冰色糕點與冰色三葉花苗；蘋果／在途水果原顯示 |
| 探險離屏返回 | 47末排冰色三葉花苗可見，48回頂部後該卡圖示／名稱完全離屏，49到底部回同名；45雖標題含offscreen但首排仍可見，46僅縮面板，兩者均不當首排離屏證據 |
| 正常App OFF | 50 OFF_COMMITTED，51探險還原原名／字型、52庫存還原原大字及金苗原截斷；29/80不變 |
| receipt與歷史 | 53同PID/start／157.0／ready／enable0、control OFF、TracerPid0；原16份attempt SHA不變，新增第17份 |

首筆journal SHA256：`f772700c28e2920bec86b3e76faacda2f690b6f659b66d4d4b3c1b0aa1f36855`。沒有重送或補寫舊UNKNOWN，adbd回UID2000。

正常OFF後重開遊戲至PID5504/start188935；未勾選的啟用與舊status分別拒絕，56／57保存實際結果；58重新確認後才取得本次同意。60新鮮同程序status、完整新journal；61–62實際庫存名稱／紀念長字縮小，63／65探險末排冰色三葉花苗同名、64返回頂部時該卡完全離屏。62仍是庫存動畫畫面，不冒稱探險畫面。正常App OFF後67探險原名／字型、68庫存原大字／截斷再次還原，29/80不變。69同PID/start／157.0／ready／enable0、control OFF，讀到的全部thread TracerPid均0；這是非原子snapshot，不代替長時間驗收。adbd回UID2000。

第二筆journal SHA256：`70d4c3a8b845b518d71ac2c670ad63934916773b13be1e72ba47f1f5ded14733`。原17份逐份SHA不變，總數18；沒有以新PID承接舊同意或改寫UNKNOWN。

## API35／157.0同帳號OS關閉再啟動

上述157回歸結束OFF後正常關閉index5，再啟動同instance，boot由`1774a036-4821-48f3-9eec-e31e64252589`改為`f41b9c04-8464-49bc-882e-5ea87def0bfc`；實際API35／libnb／原Permissive與精確157.0／0.5.0不變。PM安裝的產品APK SHA再次吻合上方原簽章artifact，18份歷史attempt逐份SHA不變。本人帳號保留登入，無複製session或代操作獎勵。

新遊戲PID1948/start8264／UID10065，OS私人frames03／04實際拒絕未重新確認的啟用與舊status；05重新確認並取得本次同意，正常App su-M首次載入此新程序。完整七階段journal與同PID/start新鮮157.0狀態，未承接舊ready。

08實際庫存紀念長字縮小／岩石剪刀苗；09探險金幣／廚師帽子／貼紙等名稱。10末排冰色三葉花苗可見，11回頂部時該卡完整離屏，12到底部仍同名。13正常App OFF後，14探險原名／字型、15庫存原大字與金苗原截斷還原；29/80不變。16最終同程序／157.0／ready／enable0、control OFF、165份讀到的thread TracerPid均0（非原子snapshot），adbd UID2000。

新journal SHA256：`8758bbf120f9611178cacbe6d120a00f87d49432b8a4673e6cec339718c6affb`；原18份SHA不變，總數19。這是同帳號157的OS重啟後分項回歸，不把其他平台／遊戲版或長時間升格為通過。原始frame只留同私人intake目錄的`os-reboot-5d8aa35dcc46419c8ae6a31d3bf34c39/`。

## 未完成與限制

探險長名稱縮字尚未觸發，不從庫存跨頁推論；最長庫存名稱縮字後仍截斷。API35／157的上述OS分項已驗；長時間、API31／34與156.x的本版完整重驗未完成；156.0首輪兩頁已有獨立證據，後續遊戲重開及156.1見[本版156.x回歸](PRODUCT050_GAME156_REGRESSION.md)。0.4.1既有156.x證據保留為其原APK結果，不能轉記本版。九組矩陣見[目標](ANDROID12_15_GAME156_GOAL.md)。

原始PNG／XML／PM／receipt只在ignored `.tools/lab/game157-official-intake-d31158fd456e4abbb7dd3e69370156b9/`；不提交帳號、遊戲包或dump。

## 同一0.5.0的156.x回歸準備

完成157同帳號OS回歸後，依既定隔離測試範圍僅在新index5正常移除157並乾淨安裝156.0/code1792142832；不繞過Package Manager降版／伺服器檢查、不複製登入資料。完整base／ARM64 split安裝後SHA再次吻合既有合法intake，產品仍同一0.5.0/code15／原簽章SHA。遊戲停在出生日期／登入起始頁，交由本人；透視鏡保持OFF、沒有嘗試本次遊戲載入。156.0／156.1仍需各自取得本版UI證據，不能轉用0.4.1結果。

本人其後已完成156.0登入，同0.5.0首輪兩頁名稱／縮字／離屏返回與OFF已有新證據；正常遊戲重開後的結算交由本人，詳見[156.x本版進度](PRODUCT050_GAME156_REGRESSION.md)。
