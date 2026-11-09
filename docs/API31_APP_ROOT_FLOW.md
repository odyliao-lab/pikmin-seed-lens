# API31：普通App發起PM loader的完整研究路徑

2026-11-09；接續[Magisk環境驗證](API31_MAGISK_RESEARCH.md)。本輪補上先前「普通App Root與adbd發起canary分開觀察」的邊界：**正向載入與故意timeout由普通研究App自行呼叫su-M**，不是由ADB代為執行Root載入命令。仍為自有fixture，不是遊戲或產品兩頁通過。

## 實作與固定身分

- 研究client `dev.ody.seedlensrootprobe`升0.0.4/code4，沿用lab簽章；APK內stored／16KB ZIP對齊的x64 PIE由PM抽出為`libseed_probe_runner.so`，不從tmp執行。
- 普通App先核自己的UID、API、當次boot及Root mount view，再核自身nativeLibraryDir canonical path、可執行性與資產中的runner SHA。只組合固定runner、數字PID/start、固定API31與受限字元host path；沒有任意Root程式／命令參數。
- Native獨立重核自己的canonical PM路徑、精確client包名及全路徑owner/mode，再重核自有probe主程序／UID/start、精確carrier PM path及host SHA。版本、canary SHA、ClassLoader與持久journal/ACK守門沿用。這個target不能接受遊戲程序。
- child使用PATH `su -M -c`，只有一個命令。若App等候超時，保持原命令存活並等待，不殺仍在進行的loader、不自動重送；native有界supervisor與UNKNOWN仍負責實際收容。
- `run-api31-probe.ps1 -RootViaApp`僅限已授權API31 AVD，與shell Root選項互斥；另核client APK及PM runner SHA，解析回報的非Root UID／client PID／native退出碼，保存client自己的收據。ADB只調度instrumentation與讀取證據，不代替正常App取得載入權限。

| 已交付研究artifact | SHA256 |
|---|---|
| client0.0.4 APK | `13c49697379fb75a8fda149ab8607af451c5324bba49af1801b11ab82c8f21b3` |
| PM runner | `01be287fe1726c97fe420edf2d3864f6857f011a537fc885b993a4b70814035f` |

同原probe0.0.7與carrier0.0.6，不重建或覆寫先前APK。研究client只是隔離驗證工具，沒有變成產品使用者需要的額外套件；未簽署／交付產品APK。

## 實際執行

以同一私人ramdisk override冷啟動`SeedLens_API31_x64`；確認新boot、API31／libndk_translation／Enforcing。尚未開research adbd Root時，原client的普通App PATH su-M已在新boot再次通過，Root配置與授權可持續；這不是產品OS重啟驗收。

同一client升級後，仍UID10148，沒有新增或全域Root授權。PM runner同bytes複製到tmp，以研究shell作負例時exit14，在程序查找／attach／journal前拒絕；不能把此負例當作載入成功。

普通client自身發起的PM-path與stale-start負例分別得到6、5。接著一次正常載入由非Root UID10148發起，執行PM runner；取得真App bridged namespace，ARM JNI四值、錯版本／key／library／symbol、singleton與attach/detach均通過，完整七階段journal及90秒FINISH PASS、程序正常消失。原件`.tools/lab/api31-run-7f74c78a29bb47b8ba84999a7aef4222/`，三次client收據分別對應兩個負例與唯一正向命令。

另一新probe程序的故意timeout同樣由普通client UID10148發起，native在durable CALLING後確認exit42／END UNKNOWN、worker回收及精確target退出；client自己正常返回42，未殺in-flight loader或重送。原件`.tools/lab/api31-run-f4c12d5f28a049bda532e93e0320dfd5/`。

timeout後另一新程序的普通App路徑再次完整通過JNI、負例、七階段journal、正常90秒FINISH及程序退出。原件`.tools/lab/api31-run-a2abfaee277c42fa8b9a7fd8c84e552d/`，沒有重送或改寫舊UNKNOWN。

| 私人收據 | SHA256 |
|---|---|
| 首次正常client自身PM收據 | `22636d5f9151f6318824594d3af6cd425ee8abf1929f05ac1d3c7f4c9012764b` |
| 首次正常FINISH | `a32de1b74283fc026a8d689527becd8ca451309e865786b3b954f9e92c79784d` |
| timeout client自身PM收據 | `1434708120810236056740ed2f82b5a8c6fd95c4b6c2565764c3b2ec0aed5231` |
| timeout UNKNOWN journal | `b6c2eb77f8a523aa44d0581b247a29e2e6bc24702b6194df5ed64e82235492cd` |
| timeout後新程序client PM收據 | `d30f811af29182ce6528131b9c520974c2c6602dc6f5cfdbf099415f44452a62` |
| timeout後新程序FINISH | `9c65c79f11b07929e59b887990adb5b4213902cf69613d11950dbd6a40b3c2f4` |

結束逐筆比較上一階段全部13份attempt SHA相同（含舊拒絕與UNKNOWN），本輪三筆正常／timeout也保留。確認無subject存活、Enforcing與原bridge、空modules，恢復adbd UID2000後正常關閉API31 AVD。API31及API35舊shell研究target在新source編譯回歸通過，沒有當作新API35裝置測試。

## 驗收邊界

這證明API31／x64／libndk_translation／Magisk31.1／Enforcing的**普通App → su-M → 自身PM loader → 自有ARM JNI canary**路徑。自建專用thread不是正式遊戲的入口，原生ARM64亦不能由此推論。

產品Root入口與精確遊戲profile沒有放寬；API31產品Java准入仍未開。正常產品兩頁／縮字／捲動／OFF／遊戲重啟及四組矩陣仍未驗，仍缺合法遊戲完整splits與原產品簽章來源。不能以此文件替代實際遊戲驗收。

## 遊戲來源補充盤點

API31正常關閉後，依CLI再確認新MuMu index5 `SeedLens-Isolated-A15`的建立身分、端點與本次boot，僅在該instance查package inventory。實際仍API35/libnb/原Permissive；`pm path`、含uninstalled的package清單及system app目錄均未找到Play商店／GMS／GSF，亦沒有遊戲包名。

僅開啟原廠`com.mumu.store`檢查來源，跳過首啟的推薦遊戲安裝，搜尋Google沒有定位可確認來源的Google安裝材料；沒有安裝遊戲／Google元件或登入，沒有修改Root／SELinux。紀錄在ignored `.tools/lab/api35-store-intake/`，最後正常關閉同一index5，原四台不動。

[官方Play遊戲頁](https://play.google.com/store/apps/details?id=com.nianticlabs.pikmin&hl=zh_TW)可讀，但公開頁不證明可取得任一指定versionCode的完整splits。後續仍需指定合法156.0／156.1來源；若走Play取得目前版本，也需先有可信Google服務來源及使用者本人登入，且實際APK必須通過精確版本／簽章／ELF檢查。不能把另一版本替代指定矩陣。
