# API35未領回花苗樣本回歸（2026-11-09）

使用者再次自行更換私人樣本帳號後，只操作新MuMu index5 `SeedLens-Isolated-A15`，created timestamp `1794181954477010`、boot `072e4811-b253-460d-b471-a318cd4bf015`。實際API35／x86_64／libnb.so／原Permissive及既有KernelSU不變。沿用原簽章0.4.1/code14單APK，PM位置回讀SHA256仍為`815de868e747111c9e9739ea9987337c53441e10a15e585d2192cecab4741cce`；沒有重建、換key或新增模組。

## 精確156.0/code1792142832

遊戲base／ARM64 split回讀SHA與合法來源一致。新樣本有25/80庫存花苗、多筆未領回花苗及水果／在途卡片；沒有把上一個帳號的禮品或水果當花苗。只點選清單分頁及捲動，不點花苗卡片、領回、派遣、拔苗或購買。

| 項目 | 真實畫面與程序證據 |
|---|---|
| 原始OFF基準 | frames05–17保存庫存／探險原名；探險實際含藍、黃、紫、白、冰藍等花苗。早期縮面板拖曳只改高度，不算離屏證據；後續真正清單捲動才計入 |
| 首程序正常App載入 | PID1974/start13925／UID10065；原12份歷史attempt均不變。正常App自行su-M，完整七階段journal；waiting-cards僅算載入，畫面另查 |
| 兩頁名稱 | 庫存可見黃色髮圈苗、藍／黃咖啡杯苗、白色韓國泡菜苗等。探險frame22有藍色金幣苗、黃色廚師帽子苗、紫色貼紙苗；frame23另見白色橋樑別針苗、冰色糕點苗、冰色三葉花苗、紫色韓國泡菜苗 |
| 探險真正離屏往返 | frame24首排花苗圖示及名稱完全離開清單可視範圍，frame27返回後名稱仍對應原卡；水果及在途水果維持原顯示。這是實際畫面比對，不以卡片計數或面板高度代替 |
| 正常App OFF | frame28為OFF_COMMITTED；實際frame29探險、frame30庫存回原通用花苗文字及字型。首程序最終status為156.0／ready／enable0、TracerPid0 |
| 遊戲重啟拒絕與重新同意 | 正常OFF後重開為PID9010/start223817。frame35未勾選啟用拒絕，frame36舊status回NO_FRESH_SAME_PID_START_STATUS。重啟遇此帳號既有種花結算／收集畫面，未代按收集，由本人處理並確認回主畫面後才繼續 |
| 新程序顯示／還原 | 重新確認後另一完整journal；frame43庫存名稱ON，frame45探險ON，frame47首排真正離屏、frame48返回仍對應。正常App OFF後frames50–51兩頁原文／字型還原；同PID9010／156.0／ready／enable0、TracerPid0 |
| 尚未完成 | 目前探險名稱均能完整容納，沒有充分的長名稱觸發縮字證據，該項仍待更長樣本；OS重啟同帳號前後連續回歸及長時間運作另列未驗 |

兩筆新journal SHA256依序為`a404982ae4351b6d45649a2b3a574ff0940d679e3691db6dbfdfea4ec4330e33`、`b4ce12aca36472e45a3a9e44c71d36aafce1fe2a9c89a40ce700d7629e496057`。最初12份產品／研究歷史attempt（含UNKNOWN）逐份SHA比對不變；沒有重送不確定載入。正常App完成全部ON／OFF，adbd root只用於唯讀receipt查核，結束回UID2000。

本帳號有既有在途任務與自然結算，不能宣稱整個帳號狀態／物品總量在測試期間完全不變。庫存頁觀察為25/80，沒有以物品操作製造測試樣本。

原始PNG／XML／receipt只存ignored `.tools/lab/api35-seed-expedition-a6fbbca446ca48b784fb8c8522de47ec/`，不提交帳號、位置清單、遊戲資料或原始診斷。此結果只新增API35／156.0的探險名稱與離屏返回證據，不推論其他版本或[九組](ANDROID12_15_GAME156_GOAL.md)全部成功。

## 精確156.1/code1793076318

156.0正常OFF後，透過Package Manager將已核原簽章的完整base／ARM64 split正常升級至156.1，保留本人登入；沒有清資料或複製session。PM回讀base／split SHA與既有Play intake一致，產品仍為上方同一0.4.1。首次PID10717/start267134／UID10065正常App載入完成七階段journal，但後續任務獎勵彈窗阻擋清單；主動正常App OFF，未把waiting-cards或獎勵畫面當兩頁驗收。本人完成全部獎勵選擇、回覆主畫面就緒後才續測。

| 項目 | 實際結果 |
|---|---|
| 同程序正常再啟用 | 重核同boot／PID10717/start267134、完整持久journal與新鮮156.1／waiting-cards／enable0；正常App重新勾選程序確認與fail-stop同意才ON。沒有重新注入或追加attempt；結束比對原15份attempt逐份SHA不變 |
| 花苗頁名稱與縮字 | frames23–24實際岩石／黃色三週年紀念金苗、黃色髮圈苗、藍／黃咖啡杯苗、白色韓國泡菜苗等；長金苗名稱字體較小，但最長名稱仍有省略號，保留此限制，不能稱全字串完整顯示 |
| 探險多類名稱 | frame25藍色金幣苗、黃色廚師帽子苗、紫色貼紙苗；frame27另見白色橋樑別針苗、冰色糕點苗、冰色三葉花苗、紫色韓國泡菜苗與岩石剪刀苗。水果及在途水果仍為原顯示 |
| 探險真正離屏返回 | frame27首排花苗圖示與名稱完全離屏，frame28返回後仍為同卡對應名稱；沒有把只縮面板當捲動重用 |
| 正常App OFF兩頁 | frame29 OFF_COMMITTED後，frame30探險恢復藍／黃／紫等原花苗名稱與字型，frame31庫存恢復原大字與金苗原截斷；同PID／156.1／ready／enable0、TracerPid0 |
| 探險長名稱縮字 | 本次探險名稱都能容納；需要觸發縮字的更長探險名稱仍未覆蓋，不能由庫存縮字跨頁推論 |

這次既有完整journal SHA256為`b8673c731f59c8323b9790d77e0ffb9072de4c351ec8718d3316fc69ffa42419`。本人處理獎勵後庫存基準變為27/80，本輪ON／OFF觀察均為27/80；不把本人取得獎勵造成的數量變動歸因於產品或宣稱跨本人操作的數量恆定。上述156.1 PNG／XML／receipt只在同一私人資料夾的`game1561/`子目錄。

正常OFF後，156.1另次遊戲重啟為PID14356/start423650。frames34–35分別實際拒絕未重新勾選的啟用、舊status（NO_FRESH_SAME_PID_START_STATUS）；frame36確認登入主畫面後，正常App才重新核對並取得本次同意。新程序完整七階段journal，沒有繼承舊ready或補寫歷史成功。

frames41–43再次觀察庫存名稱／縮字與探險各類名稱；frame45首排花苗完全離屏、frame46返回同名。正常App OFF後，frames48–49實際探險原名與庫存原大字／截斷還原，庫存仍27/80。結束同PID14356/start423650／156.1／ready／enable0，讀到的thread TracerPid均0；部分短命thread在列舉期間退出，這是非原子的snapshot。adbd已回UID2000，control OFF，沒有以後台新PID冒稱成功。

第二筆完整journal SHA256為`98ee32c9f6cf5081c9581091f482d0e9d3733e0c6b10a63793c56edc180ad50d`；重開前15份attempt（包括第一筆156.1、兩筆156.0及原UNKNOWN／研究紀錄）逐份SHA不變，總數16。沒有清除或重送UNKNOWN。

兩個精確遊戲版本目前都有同0.4.1的探險名稱／真正離屏返回、兩頁OFF及遊戲重啟後重新同意等分項畫面；**探險長名稱縮字、OS重啟同帳號連續回歸及長時間仍未完成**。最長庫存紀念名稱縮字後仍截斷的限制保留，沒有據此新增其他API／ABI或157通過結論。
