# 森林手帳原生 UI 驗收

2026-11-11，依使用者最新指示先完成森林手帳 UI。Android12／14及九組遊戲驗收門檻不變。

## 0.6.0/code18 首輪

首頁／工具／設定改為可捲動原生介面、dp/sp、安全區及獨立淺深色；首頁使用離線插畫，
工具目前只有花苗名稱透視一個直接開關。啟用在原頁展開兩項當次確認，OFF直接提出。
偏好僅儲存外觀；boot/PID/start確認不持久保存。程序級操作鎖不隨Activity離開而釋放，
原始native lease、版本／SHA／程序守門及UNKNOWN歷史不變。

新呈現模型嚴格解析native回報，ON提交只先呈現等待卡片；新鮮同程序回報才可呈現已啟用。
UNKNOWN／不完整啟用輸出保留重試屏障，OFF或一般讀取錯誤不能清除同程序屏障。

- 原簽章APK SHA256：`4968a7e37b4301967599cb59313754f7f94a4525ab790512e979e20785ea9dbc`。
- 憑證SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。
- 完整建置通過：compatibility55、installed-native29、GameSession27、LensStatus52、
  LensContext17、ZIP layout8／paired digest4；native test只編譯，不當裝置實跑。
- 僅升級安裝新index5／SeedLens-Isolated-A15（created1794181954477010）。實際API35、
  x86_64／libnb.so、原廠Permissive；boot `400309d5-ec04-4a31-b354-68076ca55a35`。
  精確157.0/code1793360009的base／split及原0.5.2 SHA皆先核對，原38筆歷史SHA一致，
  control OFF、遊戲未啟動；安裝用-r保留資料，未操作原四台／台中。
- 已實際看到首頁／設定、淺深色切換與固定三分頁。**首頁插畫未顯示，未通過**；
  APK內圖片SHA與原件一致，仍需修正Android讀圖／顯示。0.6.0原件保留，不覆寫。
- UIAutomator已輸出有效節點，但自身Shutdown thread SIGSEGV／exit139；不是產品崩潰，
  也不能將測試工具整體記為通過。原始截圖、XML及診斷只存私人忽略目錄。

尚未完成320dp、大字、觸控／鍵盤、完整開關同意流程或本版遊戲兩頁回歸。
0.5.2與其他平台證據不轉記成本版資格。插畫來源及完整提示詞見[插畫記錄](UI_ARTWORK.md)。

## 0.6.1–0.6.4：封裝路徑定位與修正

0.6.1/code19改用ImageDecoder仍FileNotFound；0.6.2/code20的自身APK固定entry也找不到，
0.6.3/code21增加私人診斷後，Android Java ZipFile列出的原始entry為
`assets/art\forest-lens-mascot-v1.png`，而非預期的斜線路徑。Windows Python ZipInfo.filename
會將反斜線正規化，因此先前Python內容／SHA檢查未發現。這是**Windows封裝路徑問題**，
不再把先前推測的ROM AssetManager問題當成原因。原件全部保留，不覆寫。

| 版本 | APK SHA256 | 本輪結果 |
|---|---|---|
| 0.6.1/code19 | `804b8c99ecc5d27261a01c3203cca4637146d995c19d69ba77854a7bff7bed9f` | 圖片未顯示 |
| 0.6.2/code20 | `f41a2064d4d32a5dda5e8483c1a7225452cfcedae2fce9d4aaeaf1d519bfe6b0` | 圖片未顯示，包含冷啟動 |
| 0.6.3/code21 | `a9a17079094f6d009b8a1186e2fb150106e8725201fa22949fe20a79b977ccb5` | 私人診斷確認literal反斜線 |
| 0.6.4/code22 | `8c535cf1ce79d00c42bc2cf0c29c8c36f106c4ca4575451a2709c27c4e60fcf3` | 首頁圖片實際正常顯示，UI其餘項目分驗 |

0.6.4將圖片封裝為flat `assets/forest-lens-mascot-v1.png`，正常使用AssetManager，
不採用ZIP workaround或ROM特例。解碼最多4MiB、sample2，原PNG／alpha／SHA保留。
建置直接解析ZIP central directory，拒絕任何literal反斜線，並核對正式圖片entry／SHA。
新增真ZIP斜線正例與反斜線負例（layout10例）；新版checker亦實際拒絕0.6.3原APK。
全部既有JVM／封裝檢查及原簽章檢查通過，native可執行檔仍只編譯。

同一index5／boot，逐版-r升級，未清除資料。0.6.4已實際驗：

- 首頁透明插畫、暖色紙張、三分頁；工具只有一個Switch（76×56dp），沒有功能內頁。
- 點Switch原頁展開兩個未勾選確認，送出disabled；只勾停止程序同意仍disabled。
- 切換分頁、取消再開、離開App／返回，都清除兩項同意；沒有載入或自動啟用。
- 遊戲尚未執行時，主畫面確認的只讀程序核對拒絕，兩項清除，送出仍disabled。
- 專用guest暫設density540，1080px寬對應320dp；font_scale1及2皆實際檢查。
  字級2可捲至開關、同意與送出／取消，未見水平溢出。這不是TalkBack實際朗讀驗收。

窄版觀察到OFF開關把手與紙色對比不足，下一版修正後重驗；本版不稱完整UI通過。
設定只暫改新guest，進行遊戲前必須核對還原原density280／font_scale1與旋轉偏好。
尚待本版或後續版完整淺深色／橫向、鍵盤／TalkBack、busy／UNKNOWN呈現與真正遊戲回歸。

## 0.6.5/code23：控制辨識與後續回歸

OFF把手改為有對比的灰綠，ON仍為紙色把手／綠色軌道；disabled另有透明度，
狀態繼續以文字及原生Switch語意表示。底部分頁移除原生Button浮起陰影。
APK SHA256 `e39fde94120e40735e8166877d557f4d391af26a7da20ce656ffb1102926995d`，
原9080…34a7e憑證；完整建置全部既有檢查通過，API31／34旗標皆false。

同index5／boot以-r升級後，已實際看到淺色工具OFF把手、深色首頁圖片、深色橫向首頁，
以及320dp／font_scale2的深色工具；均無水平溢出，開關可捲動到並保持76×56dp。
鍵盤Tab可移動焦點、上鍵可到Switch，空白鍵在原工具頁展開行內確認；兩項皆未勾選、
送出disabled。這是原生鍵盤／節點檢查，不替代TalkBack朗讀。

遊戲回歸前再次核對全部38筆歷史SHA不變、控制OFF、精確157.0兩個APK及本產品SHA，
偏好只有外觀、沒有同意持久保存。測試density／font_scale／旋轉已還原為原280／1.0／
auto1,user0；adbd恢復UID2000。原157遊戲已正常啟動，兩頁及重啟回歸進行中，尚不記通過。

其後同版首輪與新遊戲程序各完成兩頁ON／OFF及庫存長名分項；庫存真離屏亦有首輪證據。
探險長名／真離屏仍缺樣本，未辨識異常退出／NO_RETRY呈現需下一版加強，詳見
[本版157回歸與限制](PRODUCT065_GAME157_REGRESSION.md)。

0.6.6/code24另補異常退出及NO_RETRY的介面屏障，完整建置／96例協定檢查已通過，
尚不轉記裝置資格，詳見[本版屏障修正](PRODUCT066_RETRY_BARRIER.md)。

0.6.6實機抓到舊偏好字串的持久化缺陷，0.6.7/code25改用無換行v2、精確遷移舊值。
在同遊戲程序已實際通過遷移、App重開、只讀control0及OFF ACK不清除屏障；41筆歷史一致。
其後正常重啟157遊戲，新程序重新核對與重新同意後，兩頁ON／OFF、庫存完整長名縮小、
真正離屏返回已有本版實測。原41筆不變，新增正常程序一筆後總42筆SHA於OFF後保持一致。
探險長名／真離屏、本版156.x與TalkBack仍待驗；九組及UI階段不記完成，詳見
[本版偏好修正與驗收](PRODUCT067_PREFERENCE_BARRIER.md)。
