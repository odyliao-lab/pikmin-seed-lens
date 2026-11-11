# 已發布0.7.3：MuMu Android15待種花苗驗收（2026-11-11）

使用者明確要求發布後以MuMu Android15驗收，方式與前次MuMu12相同，排除相容問題至
功能可用；範圍為待種花苗ON／OFF／再ON，探險略過。本輪已通過，**同一0.7.3無需修補**。

## 目標、原件與保留

先正常關閉新index4，再只啟動既有核准的新index5 `SeedLens-Isolated-A15`，
created1794181954477010；CLI端點重新確認127.0.0.1:16544。
新boot `05785e90-b24c-4509-9e57-2aac6708cf22`，實際API35／x86_64／libnb.so，
SELinux維持原Permissive。原四台、台中及其他AVD未操作。

| 材料 | 核對 |
|---|---|
| 透視鏡 | 已發布0.7.3/code29，包名dev.ody.seedpreview |
| APK SHA256 | `c7c72e89515e0bfb69c9cc720a7316ecb20107183e0ff816333dd29ea0a87721`；本機、已發布及裝置PM實際APK一致 |
| 原簽章憑證 | `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e` |
| 遊戲 | 157.0/code1793360009，既有合法原簽章base＋完整ARM64 split；從156.1正常-r升級，保留登入、不清資料 |
| 遊戲base SHA256 | `4d28ce6c8e5c7efa1e6a864edaa545991ffe52597f751d37db55bce69c4af957` |
| 遊戲split SHA256 | `253ebec010e59980579cb114d75282202ac1da814f28eeecee71470156f3bcf6` |
| 新程序 | PID2380/start16266/UID10068；產品UID10063 |

研究三App的files先保留本機再移除APK；原始ADB archive有39 bytes診斷前綴，原件保留，
另存有效tar並驗13個成員均限自有研究files。沒有複製遊戲登入／憑證或正式scanner。
47份原native歷史逐份SHA保留。API35 shell的PATH找不到su是既有Root端點限制，
不據此宣告產品Root失敗；實際產品普通App透過既有su-M完成程序核對、status及載入。
診斷adb root只在產品操作完成後唯讀取證，每次恢復adbd UID2000；不是代為載入。
實際產品操作均由正常UI在adbd2000執行，沒有新Root／顯示套件依賴。

## 基本功能實測

1. 登入保留，原遊戲正常到主畫面；載入前展開357庫存，金色花苗原名／省略字級可見。
2. 產品舊boot屏障要求重新核對，沒有沿用前次同意；只讀核對後，兩項當次同意及
   真Root程序核對完成才送出**一次**啟用。App回遊戲後正常載入；尚未開卡片時顯示
   waiting-cards，不把這個階段當功能通過。
3. 正常開啟花苗頁並展開：紅色五周年紀念玩具珠寶、照片鈕扣徽章及各色咖啡杯完整名稱
   可見，最長名稱自動縮小；ready1、357庫存不變。
4. App直接Switch OFF，回遊戲後同一列表恢復原名、省略及原字級；新鮮control off／ready0。
   剛OFF而遊戲仍在背景時status曾維持ready1舊值，未以此當還原證據；回遊戲後另核ready0。
5. 同一程序重新確認並啟用，沿用已載入renderer；再開花苗頁完整名稱及縮字再次正常。
   最終保留展開的待種花苗頁、ON／ready1供使用者查看，沒有第二次載入。

唯一新增正常產品journal SHA256：
`fb5145098de7813c8a26da17aaa45756ae68efb3b680542c46fc19f2f1fedf7c`。
原47份SHA不變；本次一份於OFF、再ON與最後核對均不變。journal仍為RETURNED_UNVERIFIED，
沒有新增UNKNOWN；成功另依實際畫面／fresh status分層判斷，既有UNKNOWN不清除、不重送。
最終168個遊戲thread TracerPid全0、adbd2000；研究三APK／控制中心不存在，無顯示模組。

MuMu的uiautomator dump兩次exit139，保留失敗後停止使用，改由實際截圖核對產品UI；
未安裝或開啟TalkBack，也未把自動化工具故障算產品不相容或新增驗收範圍。

原始版本／Root／history／畫面／audit只保留ignored
`.tools/lab/api35-product073-5299c20f4cfc45f88e8c2e47bcd836e3`，不提交Git。
主要畫面stock-original-confirmed、stock-on-expanded、stock-off-restored、stock-final-on；
最終版本、精確splits、無研究App／模組及history核對為final-verification.json。

## 結論與界線

已發布的同一原簽章0.7.3在本MuMu API35＋157待種花苗範圍正常，不需要新版本或放寬守門。
沒有改產品來源、遊戲profile、Root路徑或超時規則，未重建或覆寫已發布APK。
探險、真正離屏重用、遊戲／OS再次重啟及其他遊戲本輪未驗；API31／34仍未通過，
不能把本分項轉記157整格或完整九組達成。前次MuMu API32完整ON／OFF屬0.7.2證據；
0.7.3的API32發布前證據仍僅原生相同下的圖示、直式與同程序顯示延續。

[同一已發布APK](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.7.3)
