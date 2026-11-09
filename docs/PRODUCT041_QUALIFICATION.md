# 0.4.1隔離產品驗收（2026-11-09，進行中）

使用者完成Google／遊戲登入，並指定在D槽尋找原簽章與遊戲材料。找到原產品keystore後，實際開啟並核憑證為`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`；另一份MuMu key不符，未使用。密碼僅在建置程序的環境變數暫存，不輸出、不提交或複製憑證資料。建置腳本仍只接受明確的repo外keystore，不加入其他repo的預設key搜尋。

## 單APK與明確驗收變體

隔離研究的普通App／su-M／PM loader及API31、35 dispatcher／正常／timeout／新程序回歸完成後，新增`-Api31Qualification`用於原簽章的隔離產品測試。它與`-Unsigned`、`-Candidate`及`-Api31Candidate`互斥，要求產品version/code均高於0.3.1/code12，仍核原憑證及Java／native同批設定。

預設建置仍為API35資格；原`-Unsigned -Candidate -Api31Candidate`保留為不能安裝的準備模式。只有明確qualification才在同一APK開啟API31/libndk_translation與原API35/libnb後端，API32與錯bridge／版本／digest／prologue／runtime metadata仍拒絕。**這是隔離驗收入口，不是多平台支援或公開發布宣告。**

0.4.0/code13原簽章準備產物SHA `12c9cdf4f86c4282e5e850f281a2ec67a2c5af3e4a74fd90964bb46a030d949a`因App標題仍寫死舊版號而未安裝。原件保留；標題改讀實際PackageInfo後，使用新0.4.1/code14，不覆寫0.4.0或舊0.3.1。

| 0.4.1/code14 artifact | SHA256 |
|---|---|
| 原簽章、API31＋35隔離驗收單APK | `815de868e747111c9e9739ea9987337c53441e10a15e585d2192cecab4741cce` |
| 預設API35 unsigned candidate，禁止安裝 | `fd16b8da87e8b9ba4962475b68d32fc1dea9a36a209e69a379e5f5904d30327d` |

兩種完整建置均通過Compatibility35、InstalledNative29、GameSession27、APKLayout8及PairedDigest4；Java/dex、雙ABI native、16KB封裝及原簽章檢查完成。三個互斥／缺少unsigned candidate模式的CLI負例在工具存取前拒絕。原生ARM64沒有新裝置通過結論。

## API35／156.1：已觀察與尚未完成

只在新MuMu index5 `SeedLens-Isolated-A15`操作；created timestamp `1794181954477010`，boot `994efc10-e992-463c-9e3b-44c3c82908bb`，API35／x86_64／libnb.so／原Permissive。原四台與台中未操作。產品UID10063由內建KernelSU單獨授權；無新模組、SELinux或bridge變更，正常App使用原su-M。

首筆PID8097/start174019因未完成載入而exit44，journal只有PREPARED／ATTACHING／END UNKNOWN，control保持OFF，遊戲存活／TracerPid0。當時曾在操作開始約5秒後切回Lens觀察；不能排除影響。原attempt SHA `608e8c2bdace2ed476582f6632f513bcae485b85382f89a23f50f51c3609852a`保留，不對同程序重送。

重開後PID10845/start273394，正常App重新勾選兩個確認並啟用；保持遊戲前景到原命令完成，完整七階段journal／RETURNED_UNVERIFIED、同PID/start的新鮮renderer status與ON control完成。`waiting-cards`僅當載入證據，接著才進行下列畫面比對：

| 項目 | 本輪結果 |
|---|---|
| 花苗頁名稱 | 實際顯示紅色照片鈕扣徽章、各色咖啡杯金苗，含白／冰／羽翅／岩石等差異；原通用金色花苗文字被替換 |
| 花苗長字縮小 | 「紅色照片鈕扣徽章金苗」完整顯示且較短名稱字小；不是只讀計數 |
| 花苗捲動重用 | 兩次下捲、兩次上捲，觀察不同卡片後回到原卡，名稱保持正確；保存全部截圖與前後receipt |
| 未領回花苗探險 | 黃色花苗實際改為黃色金幣苗；青蘋果與已完成水果保持原顯示，未點領回或派遣 |
| 探險長名稱縮字／離屏重用 | 待更多樣本；目前僅一筆未領回花苗及一筆水果，另三個完成水果卡。拖曳只改變面板高度，不冒稱已覆蓋回收重用 |
| 正常App OFF | adbd還原UID2000後從正常App提交OFF；探險回到黃色花苗，花苗回原大字／截斷，總數仍356 |
| 遊戲重啟後再確認／再啟用 | 新PID12519/start324403：未勾選直接啟用被拒；舊status回NO_FRESH_SAME_PID_START_STATUS。重新確認後完整載入，兩頁ON／OFF、長字縮小及一次捲動往返再次實際比對通過 |
| OS重啟／長時間 | 未驗 |

原始證據僅在ignored `.tools/lab/api35-game1561-product041-afc82f4e4d8440f79ef74a6bc4b938b1/`。原簽章APK已在index5安裝，尚未在API31或156.0實測；四個組合尚未全部達成。

結束確認control=off，renderer為同PID12519/start324403／156.1／enable0，TracerPid0，無loader存活，adbd恢復UID2000。原拒絕UNKNOWN SHA不變；兩個正常attempt SHA依序為`271f3116c1da4d615c2a65216f94d5dd60e7835c02d7178b33e87bb9f4b7a896`、`1edf389489fb330c240d4792aad9124438e63c404c3eb023b4f47dcd528ab737`。另一研究journal目錄的七份舊attempt亦逐字SHA不變。PM實際安裝的0.4.1 APK SHA仍與上表相同。沒有完成OS重啟、長時間或探險缺少的樣本項目。
