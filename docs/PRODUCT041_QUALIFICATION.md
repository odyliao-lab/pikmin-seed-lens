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

原始證據僅在ignored `.tools/lab/api35-game1561-product041-afc82f4e4d8440f79ef74a6bc4b938b1/`。以下新增其他組合的後續狀態；四個組合尚未全部達成。

結束確認control=off，renderer為同PID12519/start324403／156.1／enable0，TracerPid0，無loader存活，adbd恢復UID2000。原拒絕UNKNOWN SHA不變；兩個正常attempt SHA依序為`271f3116c1da4d615c2a65216f94d5dd60e7835c02d7178b33e87bb9f4b7a896`、`1edf389489fb330c240d4792aad9124438e63c404c3eb023b4f47dcd528ab737`。另一研究journal目錄的七份舊attempt亦逐字SHA不變。PM實際安裝的0.4.1 APK SHA仍與上表相同。沒有完成OS重啟、長時間或探險缺少的樣本項目。

## 後續API31／兩遊戲版與API35／156.0

同一0.4.1原簽章APK亦已安裝至官方API31 x64隔離AVD；精確156.0完整splits啟動時，原遊戲在Lens載入前於Unity SEVL指令SIGILL。沒有product attempt，不算產品效果或API31支援；獨立JNI probe與相同保護機制的baseline對照見[平台啟動界線](API31_GAME_STARTUP_BOUNDARY.md)。

另次cold boot後，該API31 AVD已正常升級原簽章156.1完整splits；普通App su-M及PM位置同一0.4.1 SHA再核。首遊戲程序在產品載入前於另一精確Unity SEVL位置SIGILL，系統隨後另記SIGSEGV；後續另一PID不當成登入就緒，沒有啟用產品。研究歷史16份attempt SHA均不變，仍無product attempt；恢復non-root adbd後正常關閉AVD。兩版API31產品兩頁仍未通過，詳見同份啟動界線紀錄。

API35同一index5重開後仍為API35／libnb／原Permissive，同一0.4.1保留。156.1到156.0的原廠PM降版請求被拒絕，保留拒絕結果；接著只移除該隔離機的遊戲並安裝已核原簽章156.0完整base／split，不繞過PM或複製登入資料。156.0已到首次生日／登入畫面，等待使用者完成；此時沒有156.0產品載入，不推論伺服器可登入或必定強制更新。私人證據位於ignored `.tools/lab/api35-game1560-product041-1f15e80502404dae826afc85e5c8d710/`。

## API35／156.0：本人登入後的產品畫面及新程序驗收

使用者後續明確回覆156.0登入完成。同一index5／created timestamp不變，boot `244903e1-7019-4eb6-bae4-e99697a170cb`，實際API35／x86_64／libnb.so／原Permissive。PM遊戲156.0/code1792142832，base／split SHA與原簽章intake一致；產品仍為上表同一原簽章0.4.1，沒有重建或覆寫。本人已登入且實際進入主畫面／兩頁，本次未遇強制更新。

| 項目 | 分項結果與私人證據 |
|---|---|
| 首程序正常App載入 | PID2274/start17271／遊戲UID10065。正常App重新確認後自行su-M，保持遊戲前景，完整七階段journal；同PID/start的156.0 status與control ON、TracerPid0。frame16只是waiting-cards載入證據，後續才比對畫面 |
| OFF基準與ON名稱／縮字 | frame09原通用金色花苗及截斷；frame18實際紅色照片鈕扣徽章金苗、白／羽翅／黃／藍／岩石咖啡杯金苗，長名完整且較小字；後續亦見冰／紫／紅等名稱 |
| 花苗捲動重用 | frames19–22兩次下捲／兩次上捲，不同卡片後回到原卡，名稱正確；顯示總數持續356 |
| 未領回探險與OFF | frame10黃色花苗→frame23黃色金幣苗；青蘋果及三張完成水果卡未變。正常App OFF回OFF_COMMITTED_REQUIRES_UI_RESTORE，再回遊戲實際觀察frame26黃色花苗、frame27花苗原大字／截斷還原 |
| 遊戲重啟與舊同意／狀態拒絕 | 正常OFF後只重啟隔離遊戲，進主畫面新PID6657/start317279。frame31未重新確認不能啟用，frame32舊status拒絕為NO_FRESH_SAME_PID_START_STATUS；不沿用舊ready或consent |
| 新程序再啟用／捲動／兩頁OFF | 正常App重新核對新程序，完整七階段journal；frames37–40花苗長名／縮字、一次捲動往返與探險黃色金幣苗再次觀察。正常App OFF後，frames42–43兩頁原文／字型還原；總花苗仍356 |
| 尚缺覆蓋 | 探險仍只有一筆短名稱未領回花苗，不能驗長名稱縮字或真正離屏重用；沒有領回／派遣／拔苗／購買。OS重啟及長時間另列未驗；不是六組全部成功 |

結束control OFF，同PID6657/start317279／156.0／enable0／ready、TracerPid0，adbd恢復UID2000。原三份產品attempt（含首筆UNKNOWN）及七份研究attempt逐字SHA不變；本輪兩筆正常journal SHA為`38d7e77fda62a168f9e9ae42ecc9386933f114d15a4a8c64e0789db504ed0b64`與`876f2576274fe1f0b6e1749f0606bd0eaf3f9588e7db324adaabe31c64c42f93`。舊UNKNOWN不補寫成功、不重送。

全部原始PNG／XML／receipt仍只在上方ignored私人156.0資料夾。確認OFF後正常關閉index5，保留登入資料；才進已獲確認的新API34隔離環境，未操作既有四台或台中設備。
