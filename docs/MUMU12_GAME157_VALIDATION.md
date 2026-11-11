# MuMu內建12.0／遊戲157驗證（2026-11-11）

## 登入後：本次僅驗待種花苗開關

使用者其後要求協助完成指定生日及Google帳號登入；帳密／驗證由本人完成。
空白index4原先沒有Google核心服務，點Google登入時實際回報登入錯誤。
使用MuMu原廠桌面「小工具→谷歌安裝器」正常下載並套用該機提供的套件；
未套用API35的GSF（其minSdk35不適用），沒有複製任何帳號資料。
安裝器的「重啟桌面」實際改變boot，之後按新boot重新核對身分及精確遊戲SHA，
不能沿用前一程序。沒有啟用TalkBack或新增其驗收。

使用者回覆登入完成，並縮小本次範圍為**待種花苗ON／OFF顯示，探險略過**。
新boot `e9d630dc-1542-4137-a911-f5fbd1ec8d12`、實際API32／x64／libnb／
原Permissive／adbd2000；157.0/code1793360009的base／完整split SHA再次相符。
遊戲PID5715/start49614/UID10041，正常登入後實際開啟花苗庫存，357個花苗；
展開列表可見原名及原本的省略顯示，沒有領取、盆栽、派遣或其他物品操作。

隨後切到原簽章0.7.0，實際仍顯示「暫不支援／此 Android 的 NativeBridge 尚未納入
候選範圍，保留原顯示。」；唯一Switch checked=false、enabled=false。
因此**待種花苗ON／OFF驗收仍被API32平台支援阻擋，未通過**；不能把遊戲登入、
庫存開啟或開關保持OFF當作啟用與關閉還原成功。尚無產品載入／兩個attempt目錄，
沒有嘗試點停用開關、繞過NativeBridge或新增不確定操作。探險本次依使用者要求未操作。
保持登入資料，切回待種花苗頁，供後續平台開發使用；未把帳號或生日寫入Git。

下方為登入前的初次結果，保留歷史；其「未登入／未安裝Google／已關機」只描述當次。

使用者另指定使用MuMu內建12驗證，故重新啟動先前已授權、空白且已關閉的
index4 `SeedLens-Isolated-A12`，created1794178164127859；沒有使用原四台。
index5及官方AVD保持關閉，一次只運行這台新隔離機。

## 環境與材料

- CLI重新核對名稱／建立時間／index／端點127.0.0.1:16512後才操作。
- 實際release12、**API32**、x86_64、`libnb.so`、原Permissive；
  boot `9f5fa371-e1f2-4e5f-ab85-513e366e4f9d`，adbd UID2000。
- 原環境未開Root、shell無su；本輪沒有變更Root、SELinux或安裝額外模組。
- 在此空白隔離機正常安裝原簽章157.0/code1793360009的base＋完整ARM64 split，
  安裝後PM版本及兩份SHA與[合法來源](GAME157_OFFICIAL_INTAKE.md)一致。
- 安裝既有原簽章0.7.0/code26，APK SHA
  `caba3b1f299fcda053b2dc121074ab75cf30510d9f94c3435ecbee683b9ce364`；
  沒有重建、覆寫交付物或改產品守門。

## 實際結果

原遊戲單次正常launcher啟動，PID2876；App切回前後核對相同PID／start／UID。
實際畫面已到157.0出生日期起始頁，本輪觀察未出現官方API31 AVD的SEVL／SIGILL。
首次畫面曾出現MuMu加速服務提議，切換App後已消失；未啟用或安裝加速套件。
沒有填生日、操作帳號登入或複製憑證，亦未操作遊戲物品。

0.7.0實際頁面顯示「暫不支援」及「此 Android 的 NativeBridge 尚未納入候選範圍，
保留原顯示。」；Switch checked=false、enabled=false。這是API32／x64平台守門，
尚未進入Root或native載入流程，不能以登入、安裝成功或開Root解除產品資格。
兩個native attempt目錄均不存在，沒有執行載入或新增／重送歷史。

**結論：MuMu12可啟動精確157至出生日期頁，但目前0.7.0不能在此啟用，功能未通過。**
登入後遊戲操作及花苗／未領回探險兩頁皆未驗；不要求使用者為已知平台阻擋先行登入。
API32是新增獨立環境，不能取代必要的Android12／API31，也不承接API35的產品資格。
若後續開發此平台，仍須先以自有隔離程式驗證其Root／NativeBridge載入及失敗收容，
通過後才整合新版本產品，不能只把API32加入版本判斷。

結束時保留安裝與資料，正常關閉遊戲、透視鏡及該index4，CLI確認guest／host程序
均停止。原四台、台中設備與帳號資料未操作。原始日誌、PM與截圖只留ignored私人目錄。
