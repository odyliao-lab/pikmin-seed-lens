# 精確遊戲設定檔與新增版本流程

`0.3.0/code11`新增156.0設定檔，因此採minor／次版本；不是把整段156.x放行。
目前156.0只有離線來源比對，尚未通過登入、花苗／探險畫面與OFF實測。
156.1的既有實機證據及各APK版本請看[驗收矩陣](VALIDATION.md)。

## 兩個獨立相容層

- 平台層：Android API、ABI、Root mount view與載入後端。ARM64與NativeBridge不同，
  同一APK打包兩種後端不等於全部Android／ROM已合格。
- 遊戲層：versionName/code、完整ELF大小／SHA、五個hook完整簽名／入口指令、文字入口
  veneer及實際metadata守門。任一不符便保留原顯示，不猜偏移或採用「最接近版本」。

`native/renderer/game_profiles.h`是本次兩組完整設定的來源；loader、x64 host及ARM renderer
都重新驗證所見的實際ELF。狀態回報必須屬於被選中的版本、同PID/start及新鮮時間。
OFF不必取得新版本的載入資格；不能以新程序替舊UNKNOWN載入補成成功。

## 2026-11-08離線證據

| 遊戲 | versionCode | libil2cpp大小 | SHA256 |
|---|---|---|---|
| 156.0 | 1792142832 | 256261664 | b1a9781e8e7ddbdf984f6e3e90c7f753dd3ac149881fc9c14a46892eda6f6b5d |
| 156.1 | 1793076318 | 256384336 | 97ef6650f73d99ca8c7ebbbdf2d059d0d41ea68c036fde05634003f6a052aefb |

156.0的既存base／ARM64 split均通過原遊戲簽章驗證；156.1 ELF及metadata從已核對Virgo
目前安裝的原遊戲包唯讀取得。沒有降版、安裝另一遊戲、變更帳號或修改遊戲原檔。
14個涉及顯示的類別欄位布局相同；兩版合計10個完整簽名hook body／前綴、2個文字veneer
均逐一與ELF可執行segment比對。文字metadata入口都是單一B+4，實際hook位於其body。
26個刻意改壞的RVA／指令／SHA／大小／布局版本均被來源檢查拒絕。
這些結果不證明156.0目前可連線登入，也不取代遊戲畫面驗收。

## 可重跑的離線檢查

每個私人輸入目錄含`libil2cpp.so`、`global-metadata.dat`與`dump/dump.cs`。
遊戲APK、dump、原始結果、帳號與截圖不進Git。

```powershell
python -m unittest discover -s research -p test_profile_evidence.py
python research/check_profile_sources.py --game1560 <private-1560-folder> --game1561 <private-1561-folder> --output <new-private-report.json>
```

輸出必須是新檔，拒絕覆寫；工具不連裝置、不改來源、不自動核准設定檔。
ELF映射測試9項含錯架構、非可執行segment、BSS／越界／重疊映射、錯誤overload及重複簽名。
新增版本仍需審查整組metadata欄位與runtime guards，再在經授權的隔離裝置完成主畫面、
花苗頁、未領回探險、長字縮小、OFF還原、捲動重用、正常重開遊戲及掃描恢復驗收。
不可為第二版驗收直接降版正式掃描中的Libra／Virgo；另建instance／帳號要先取得授權。
