# 精確遊戲設定檔與新增版本流程

`0.3.0/code11`新增156.0設定檔，因此採minor／次版本；不是把整段156.x放行。
該階段156.0只有離線來源比對；2026-11-09已由使用者本人登入，在新API35隔離機以同一原簽章0.4.1完成兩頁名稱／OFF、花苗長字／捲動及遊戲重啟後重新確認再啟用等分項驗收。本人更換樣本後，兩版探險真正離屏返回及OFF亦各自實測；探險長名稱觸發縮字仍待樣本，庫存最長紀念名稱縮字後仍截斷，API31／34未通過，不能稱全部組合成功。
156.0／156.1的各APK實際證據請看[驗收矩陣](VALIDATION.md)及[0.4.1分項結果](PRODUCT041_QUALIFICATION.md)。

## 157.0新增精確設定檔（0.5.0）

使用者於2026-11-09回報157釋出並要求加入Android12／14／15功能驗證。其後指定AutoFarm已安裝版本，限定唯讀取得完整原簽章base／split，核為157.0/code1793360009。三版完整來源比對通過，新增精確profile；未知／近似code仍拒絕。原簽章0.5.0的新UI驗收獨立於0.4.1。

新index5官方Play當時只提供156.1；後續使用者指定的AutoFarm來源已取得，沒有操作其UI／scanner／憑證。0.4.1在新index5對真實157的未知版本拒絕、OFF／無mapping／無新attempt及16份歷史SHA不變已驗；詳細來源、雜湊及hook見[157 intake](GAME157_OFFICIAL_INTAKE.md)。以下為新增版本的一般流程，已執行結果以該紀錄為準。

取得與整合順序：

1. 先保留156.0／156.1既有APK與私人證據，完成可取得樣本的分項驗收；產品OFF後只在已授權新index5以已核合法原簽章完整splits正常更新157；來源唯讀例外另記，不部署正式節點或複製session。
2. 由PM核實完整versionName/code／base與全部splits路徑；逐份pull前後及本機SHA一致，apksigner核原簽章與split集合。遊戲包、ELF、metadata及dump全部只存私人ignored目錄。
3. 舊0.4.1對此未知版本須在載入前拒絕、原顯示不變且無新attempt；這是負例，不能當157功能通過。取得精確code後再新增可重跑的未知版本測試資料，不以猜測code驗收。
4. 重新抽取並審查完整hook簽名、可執行segment／前綴／veneer、所有用到的類別欄位與runtime metadata契約。布局改變須獨立實作，不只追加digest；檢查工具不得靜默忽略新profile。保留錯code／SHA／RVA／指令／布局等拒絕測試及歷史UNKNOWN。
5. 新profile與功能整合使交付bytes改變，分配更高產品version/code並保留原簽章；舊APK不可覆寫。先隔離平台／失敗收容，再以正常App及同一新APK逐組驗[九組必要項目](ANDROID12_15_GAME156_GOAL.md)。研究或安裝成功不算功能通過。

## 兩個獨立相容層

- 平台層：Android API、ABI、Root mount view與載入後端。ARM64與NativeBridge不同，
  同一APK打包兩種後端不等於全部Android／ROM已合格。
- 遊戲層：versionName/code、完整ELF大小／SHA、五個hook完整簽名／入口指令、文字入口
  veneer及實際metadata守門。任一不符便保留原顯示，不猜偏移或採用「最接近版本」。

`native/renderer/game_profiles.h`是本次三組完整設定的來源；loader、x64 host及ARM renderer
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

### 2026-11-09私人來源重新取得

使用者授權D槽搜尋後，找到156.0的base／ARM64 split；156.1由已登入的新MuMu index5 Play商店直接安裝，再依PM列出的完整兩個APK逐份pull並比對前後及本機SHA。兩版base／split均以apksigner驗證，signer SHA256皆為`944bda466eaf64be1c93896d5c8e9a454a07c8d8fcbc4adeeefb3545b335e979`；不複製帳號資料或scanner。

| 版本 | base SHA256 | ARM64 split SHA256 | metadata SHA256 |
|---|---|---|---|
| 156.0/code1792142832 | `61c13513f3710b469f33ae3f63af9eaeca7b80fa046f3669ac73fa35392bdd8a` | `6aa9c204928b5f80164b4206c27960d0467038db2cc192827e5e102785b4f960` | `5240b679b85fc1b02b1284f0fe2d8eddd33fecfde3c0688c798035719a83f878` |
| 156.1/code1793076318 | `070c75d6e253317438dc4ac1b0f60b5dbf9e5250763827b7c6b6197f0e81e9dc` | `7b05aa1c10acad9266c1daeea68d466c138b2ecb35a9d03717af273dd13e4836` | `b3e4d8ec9a9e8794f68132b01c49a7b05f1be57497654efd1d785892e5810eff` |

直接由兩版APK抽取的完整ELF大小／SHA均符合上表既有profile，五個hook完整前綴及文字B+4 veneer逐一重驗相符。這是來源比對，不宣告156.0能登入。156.1已由使用者在index5完成登入，0.4.1畫面驗收進度見[本輪產品驗收](PRODUCT041_QUALIFICATION.md)；原始APK與metadata只留ignored `.tools/lab/`。

每個私人輸入目錄含`libil2cpp.so`、`global-metadata.dat`與`dump/dump.cs`。
遊戲APK、dump、原始結果、帳號與截圖不進Git。

```powershell
python -m unittest discover -s research -p test_profile_evidence.py
python research/check_profile_sources.py --game1560 <private-1560-folder> --game1561 <private-1561-folder> --game1570 <private-1570-folder> --output <new-private-report.json>
```

輸出必須是新檔，拒絕覆寫；工具不連裝置、不改來源、不自動核准設定檔。
Python測試16項含錯架構、非可執行segment、BSS／越界／重疊映射、錯誤overload、重複簽名及完整profile陣列／精確code拒絕。2026-11-09三版實際來源重驗為3 ELF／15完整hook／3 veneer／14類別相同布局／39個破壞負例；不是裝置驗收。
新增版本仍需審查整組metadata欄位與runtime guards，再在經授權的隔離裝置完成主畫面、
花苗頁、未領回探險、長字縮小、OFF還原、捲動重用、正常重開遊戲及掃描恢復驗收。
不可為第二版驗收直接降版正式掃描中的Libra／Virgo；另建instance／帳號要先取得授權。
