# 0.7.2：MuMu API32／157待種花苗（2026-11-11）

使用者要求在現有環境實作Root＋單一APK，並限定本輪待種花苗ON／OFF，略過探險。
本輪實際通過；最終保持同一遊戲程序、待種花苗頁及ON，供本人查看。
這是MuMu「12.0」的**Android12L／API32**，不能代替Android12／API31驗收。

## 交付與環境

| 項目 | 核對結果 |
|---|---|
| APK | 0.7.2/code28，`dev.ody.seedpreview`，原簽章 |
| APK SHA256 | `1320d51231fa912bb7d8b712d7bac821aafc80d59b2e858d5b9d50bc319f656e` |
| 憑證 SHA256 | `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e` |
| 目標 | index4／SeedLens-Isolated-A12／created1794178164127859；127.0.0.1:16512 |
| 實際平台 | API32／x86_64 ELF／libnb.so；原Permissive，未改SELinux |
| Root | MuMu內建Root；普通產品App UID10042透過su-c取得Root，驗UID0及同PID1 mount namespace；adbd全程2000 |
| 遊戲 | 157.0/code1793360009，既有精確base／split及IL2CPP guard均核對 |
| 程序 | boot31bef082-9840-44b4-b3e2-4689765855da；PID14218/start280512/UID10041 |

研究用fixture、Root client及host carrier三個APK**均在安裝產品前移除**。
產品APK自己包含x64 loader／host與配對ARM64 renderer；不依賴研究APK、控制中心、
LSPosed、額外Magisk顯示模組或新Root管理器。此機原有遊戲登入所需Google服務仍保留，
不是透視鏡載入的外部helper。內建Settings的Root授權已允許產品。
0.7.1/code27私有候選在部署前發現Root前綴引號問題，未安裝；其原件保留，修正另升0.7.2。

## 先研究，後整合

[隔離研究](API32_MUMU_BACKEND_RESEARCH.md)先完成普通App Root、15項自有CPU／故障案例、
正常PM／ARM JNI、故意timeout收容及另一新程序回歸。API32使用獨立四參數NativeBridge
adapter，不套用API35五參數介面；負context標記必須另有syscall-info的arch／IP／SP證據，
拒絕正在進行的syscall及Linux restart。沒有直接放寬Android版本而沿用不相容後端。

精確遊戲版本/code/digest/prologue/runtime metadata、boot/PID/start/UID、單worker及
UNKNOWN不重送規則保持。su-c是API32 x64事先選定路徑，不是在失敗後改命令重試。
其他ABI／API仍選原su-M；API31／34候選開關均false。

## 實際待種花苗驗收

| 操作 | 畫面與狀態證據 |
|---|---|
| OFF基線 | 357庫存，金色花苗原名稱／省略顯示 |
| 首次ON | 完成當次核對及兩項同意，只送出一次正常產品載入；最初已開的卡片尚未刷新，記waiting_cards，不當成成功 |
| 重新開花苗頁 | 同一遊戲程序，不重載；ready1，紅色五周年紀念玩具珠寶、照片鈕扣徽章、各色咖啡杯完整名稱可見；長名自動縮小 |
| 直接OFF | 原名、省略顯示及原字級實際還原；control off／ready0，庫存仍357 |
| 再ON | 同一程序重新確認，沿用已載入renderer；完整名稱再次可見，control on／ready1，沒有第二次載入 |

產品只有一筆正常journal，SHA256
`b44d3c2560c7b0c4988569d51a8f280c20294085f646b065dc9cf62a8f0d31c6`，
ON／OFF／再ON均不變。原六筆研究journal SHA全部不變，三筆既有UNKNOWN保留。
產品journal仍是RETURNED_UNVERIFIED；功能成功另依實際畫面及新鮮renderer status判定，
沒有修改journal來偽造成功。最終173個遊戲thread的TracerPid全0。

原始畫面、audit、版本及身分證據保留本機ignored
`.tools/lab/api32-product072-aaeaa31131bd4ea9b1b9a85efa9fbcba`，不提交Git。
主要檔為stock-before-on、stock-on-confirmed、stock-off-restored、stock-final-on及各audit。

## 建置與界線

完整原簽章建置通過Compatibility57、InstalledNative29、RootCommand57、GameSession27、
LensStatus102、LensContext34、APK layout10及paired digest4；native測試執行檔由建置編譯，
不把編譯當實機執行。自有CPU15例與PM三輪是另行實際執行的研究證據。

本次未測探險、離屏重用、遊戲／OS重啟或長時間；其他API／遊戲版本也沒有本APK的新資格。
API31／34原遊戲啟動障礙仍未解決，完整九組目標未完成。現有API35與舊APK紀錄保留，
不自動轉記0.7.2通過。原四台、index5及台中設備未操作。
