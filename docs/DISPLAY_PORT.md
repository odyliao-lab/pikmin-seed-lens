# 四組唯讀顯示移植

2026-11-11 後續授權：使用者要求合併 main 並更新 README；repo 保持私人、APK 未發布。下文保留各開發階段的原始範圍與驗證紀錄；最新摘要見 [驗收矩陣](DISPLAY_PORT_RESULTS.md)。

同日再授權發布 0.9.5 並將 repo 公開，指定程式碼採 MIT；沿用已實測的原簽章 APK。下方私人測試與禁止發布的文字屬當時階段紀錄，不改寫其驗證範圍。

## 接手與範圍

2026-11-11，目標 remote `odyliao-lab/pikmin-seed-lens`，乾淨 `main@0773bca` 起始；工作分支 `codex/standalone-display-groups`。使用者後續縮減本輪開發／裝置驗收為 MuMu，沒有可用 ARM64 手機；不得宣稱手機通過。不得合併 main 或發布。

來源 remote `odyliao-lab/pikmin-control-center`。實際來源工作副本已在其他研究分支，故全程以 `git show a02b58f35295948cbd29e467ce5ccd7ffe33161a:<path>` 讀取固定版本，不切換或修改來源。選擇性移植，不 cherry-pick monorepo commits。

## 既有功能與必要缺口

| 項目 | 接手時狀態／本輪界線 |
|---|---|
| Root＋單 APK | 0.7.3 已封裝 PM loader、x64 host 與 ARM renderer；沿用，不建立第二載入器 |
| 平台／遊戲守門 | 既有 API32／35 x64-libnb、精確遊戲 version/code/ELF SHA/prologue/metadata、PID/start/boot 與持久 UNKNOWN 保留 |
| 既有顯示 | 花苗名稱、縮字、OFF 原文字／字型還原；來源八項擴充尚未接入 |
| 檔案隔離 | 舊控制／回報與來源同名，必須改成獨立 namespace；不刪除來源或舊 journal |
| App 先開／PID 重綁 | 既有 UI 要求當次遊戲主畫面確認，尚未滿足本輪先啟用、後開遊戲的流程 |
| confirmed context | 來源透過核心的登入／登出 epoch 與 InventoryManager 提供；獨立版尚無，禁止用假資料或外部 TSV 補足 |
| 四組設定 | 尚需獨立擁有的 3／2／2／1 契約、意願先保存、合法舊值 OR 遷移與中斷回復 |

## 本輪自包含基線

新鮮核對既有新建隔離 index4／SeedLens-Isolated-A12、實際 API32／x86_64／libnb.so。0.7.3/code29、Game156.1/code1793076318；普通 shell UID2000、內建 su。沒有控制中心／三個研究 APK，`/data/adb/modules` 不存在；maps 只有獨立 APK 提供的 renderer／host，沒有控制中心核心。

遊戲既有程序上的完整裝飾名稱可見，透過普通產品 UI 關閉後實際還原原名與較大字型；新鮮同 PID/start 回報 ready/control0。這是舊版單 APK 既有功能的有界基線，**不是新獨立 namespace、新載入、新功能、全新安裝或 App 先開驗收**。原始身分、maps、journal SHA 與 ON/OFF 圖片只存 ignored `.tools/display-port-baseline/`。

## 逐項目標（未驗收不得標完成）

### 0.8.0/code30：獨立檔案與最小載入通過

這是控制／回報隔離的 minor 版本，仍是原花苗顯示，沒有宣稱八項新功能已完成。原憑證 SHA256 `9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`；APK SHA256 `1c58745f4faf11258367e6e2ab59fb77b237ab9f313d026fd4382271e2d1a033`。APK 僅私人測試，不發布。

index4 同 boot，正常關閉 Game／Lens、保存原 APK 與 Lens data 後原簽章升級。透過普通 App UI 的主畫面／fail-stop 確認，在新 Game156.1 PID12359/start385694 僅載入一次。新控制 `seed-lens-seed-preview.txt` 與新回報 `seed_lens_seed_preview_status.tsv` 同 PID/start；舊 `pikmin-seed-preview.txt` 全程 OFF。實際花苗頁完整長名／縮字可見，App UI OFF 後原名與原字型還原；同程序 ready/control0，357 不變。原兩份 attempt SHA 不變，新增一份正常 journal；不以此推論重複啟用或新四組已驗。

雙 ABI loader、ARM renderer、Java 與 APK 簽章／16KB 配對檢查通過。新增 group policy 涵蓋 3／2／2／1 分組、OR 遷移、非法值拒絕、已存 OFF 優先；Linux CI `42960432389` 的兩個 native 純 policy 測試實際執行通過。CI 不是 Android 原生或遊戲驗收。原始備份與截圖留在 ignored `.tools/display-port-baseline/` 及隔離機 root0700 備份目錄。

| 組別 | 已接入內容 | Game157 實際樣本（0.9.1／API32） |
|---|---|---|
| 花苗資訊 | 顏色／種類、D01 已收／持育待／未知原因、育苗詳細頁步數列 | 庫存名稱、持育待、長名、離屏往返、OFF／再 ON 可見；未成熟育苗詳細頁缺樣本 |
| 皮克敏資訊 | D03 目前花態／可收瓣／任務、D04 已取得裝飾、主清單合併短副標 | 無飾·葉副標、原完整詳細名稱、OFF 還原可見；其他花態／已取得裝飾待驗 |
| 探險資訊 | D05 普通白盒對象／一般裝飾、明確狀態及遊戲時間、尊重原可見性 | 可見「待派·3天到期」、OFF 恢復 Kochi、詳細頁仍有原地址；白盒與進行中狀態待驗 |
| 精華／花瓣對照 | D02 保留精華數，補 confirmed 花瓣／容量／空間；契約為 nectar-stock-preview | 白精華 173、瓣 97/300 餘 203；黃 164、瓣 15/300 餘 285；切換顏色與 OFF 原數字還原可見 |

### 四組來源整合（實機驗收前）

原生 policy／adapter 依功能分開移植；新增 Game157 專用 renderer，156.x 保留原 renderer，沒有猜新入口。獨立 context 從 Unity 現存唯一 PlayerManager 的 PlayerInfo.PlayerId 與 inventoryManager 讀取，僅在既有 UI thread 使用；以弱參考、登入／登出 epoch、帳號 SHA、manager、revision 與 initialized 前後核對。登入／登出入口額外驗證精確 Game157 ELF 的 24-byte prologue。原始 player ID 不落地、不讀 token、不發 RPC；找不到或不唯一保持未知。

四組 root 設定經既有 LoadLease 序列化，主意願先持久寫入，再逐一同步獨立成員並讀回。非法／symlink／讀取失敗不是 OFF。新 App UI 只有四個主開關、預設收合說明及共用錯誤；內建 foreground service 在使用者啟用後等待遊戲、精確綁定新 PID，沿用原載入器與持久 UNKNOWN 屏障。服務不從開機啟動、不自動重播同 PID 的失敗、不引入外部 APK／模組。

來源八個實際 Gateway 的原 CONTROL／REPORT 與檔案 SHA 記於 `DISPLAY_SOURCE_CONTRACTS.json`；交叉測試直接核對本版 Java、native 契約及 renderer 的實際路徑。D02 保留 nectar-stock-preview，不使用 nectar-stock。

0.9.0/code31 僅為本機簽章草稿，未安裝；之後修正遊戲 library 尚未映射時的等待與 metadata 查找成本，保留草稿並升至 0.9.1/code32。前景服務遵循 Android specialUse 類型，僅供 App 內管理唯讀顯示，不要求通知權限測試。

來源 Game157 資格不轉移至其他遊戲。MuMu 普通白盒、部分探險狀態、跨帳號與耐久樣本不足亦不繼承為通過。先建立獨立載入與隔離檔案的實測，再擴大 renderer。

### 0.9.1/code32：首輪四組實際顯示

原簽章 APK SHA256 `2fa4f9d68cb996e8a9d5a4aecd79e0d896bea5b5dd2eeca0e1f5d9309c501029`。index4 升級官方原簽章 Game157.0/code1793360009，保留登入與資料。App 先啟用、遊戲原本未開，由 App 的「開啟遊戲」建立 PID15442/start540241，內建服務自行呼叫 APK loader，一份新 journal；沒有主機手動注入。表列結果均是遊戲實際畫面，沒有用 report ready 代替。

舊全域控制保持 OFF，八個獨立成員與四個主意願位於 seed-lens namespace；沒有控制中心／研究 APK 或 `/data/adb/modules`。未執行餵食、收瓣、派遣、領取、拔苗或種植。兩株育苗已發光疑似成熟，保留不點取，不能算詳細頁驗收。隱藏的 completed 卡片 footer 不列為「已返」可見成功。

### 0.9.2/code33：升級阻擋與重啟缺口

原簽章 APK SHA256 `71913ad3d192ff7532f598cc8394e9616be06e29d321c4619d0369ad4ea79555`。Game157 八個實體回報增加第 12 欄 `seed-lens-four/0.9.2-code33`，loader 先驗證同 APK build，再正規化舊的 11 欄 App protocol。舊 Game156 仍保留原 protocol。既有 PM payload path 守門也保留。

在舊 0.9.1 遊戲程序仍運行時升級 APK，App 明確要求重開遊戲，不重新注入、不翻動原成員控制；原 attempt hashes 不變。四組說明可直接展開／收合，升級保留三組 ON、一組 OFF。

正常重開後 PID673/start769679 發生音訊 native crash；journal 原件保留 RETURNED_UNVERIFIED，不重送同 PID。後續一次名義上的全 OFF 對照仍有探險主意願 ON，故實際有載入，**不能當無注入對照**。完全停止 Lens App／服務後再開原遊戲，PID5127 仍在 `AAudioStream_getBufferCapacityInFrames` null dereference；tombstone 無自有 SO，沒有新載入 journal。這支持目前也存在原遊戲／模擬器音訊啟動問題，不能據此宣稱新 renderer 無問題或重啟通過。

開關的背景讀取操作可在畫面重繪後取得 gate，原 listener 忽略忙碌期間的點擊卻未立刻回復已保存值；0.9.3/code34 改為立即還原畫面，避免將尚未保存的 OFF 誤認已關閉。載入失敗不再因遊戲消失而被「等待開啟遊戲」覆蓋。新版本仍須實際回歸。

### 0.9.3/code34：API32 重開模擬器後的有界回歸

原簽章 APK SHA256 `e4c6baf81e6b9b1fceea57d79580091d1437546759d22421b1dbc9e0690352fc`。正常關閉再啟動 index4，boot be528f7a-d29c-4f74-8f57-6e5626d74572；原遊戲無自有 SO 時恢復主畫面。產品普通 App 保存四組 ON，由既有服務在 Game157 PID1922/start1040 自行完成載入，八個回報均帶本 build。

再次看到花苗周年／照片卡長名與持育待、成人「無飾·葉」、探險「待派·3天到期」、白精華 173 對瓣 97/300 餘 203。兩次快速捲離再返回花苗列表；所有四組 OFF 後控制檔與主意願均實際 OFF，回遊戲看到原金色花苗名字級、成人副標消失、Kochi 原地點與精華對照原數字 97／15／0。新鮮同程序回報為 control0。圖片以 `093-` 命名，包含四組 ON／OFF、快速往返與 App 全 OFF。

之後正常重開遊戲仍遇音訊崩潰，故單獨遊戲重啟／新 PID 再啟用仍未通過，不能把模擬器整機重啟後的一次成功替代。保留全部舊 journal 與新失敗原件；最後 App 四組 OFF、正常關閉 index4，才啟動 index5。0.9.4/code35 補背景 monitor 釋放 gate 後立即重繪，防止等待頁保留舊的 disabled 外觀；不改 native 平台資格。

### 0.9.4/code35：API35 遷移與第一程序

原簽章 APK SHA256 `481e3aad633a29fc81ac9e8fc2e1875456afec84e1458e05215a0582ba365443`。index5 名稱／建立時間／16544 重新核對，實際 API35／x86_64／libnb、Permissive，boot 9a75932d-412b-48c8-b13a-8b7a40523f82。沒有控制中心、研究 APK 或顯示模組。舊版 0.7.3 的獨立交易紀錄與舊 global ON 保留，所有新主意願原本不存在；App 首次正常讀取後只有花苗整組 ON，其他三組 OFF。按「開啟遊戲」補一次 fail-stop 同意後自行載入新 Game157 PID2439/start24286；登入保留。

花苗長名與持育待、快速離屏往返可見；成人清單往下捲後看見已取得萬聖節／巴厘島／禮物貼紙／奶嘴與「葉任」合併副標，詳細頁保留完整原名及裝飾模型。探險可見「待派·3天到期」，沒有代領三筆完成水果；D02 原精華 173／164／78／20 與瓣 97/300 餘 203、15/300 餘 285、已知 0/300 對照可見。

MuMu API35 的 shell PATH 仍無 su；普通產品 App 能使用既有 su-M。ADB root 僅於產品操作完成後唯讀取證，立即恢復 UID2000，沒有代為執行 loader 或寫入成員。UIAutomator dump 再次 exit139，停止使用該工具，改由截圖核對；不安裝或開啟 TalkBack。

同程序四組 OFF 後，花苗恢復原名與字型、成人已取得裝飾及花態副標消失、Kochi 地點復原、白精華對照恢復單獨花瓣 97。八個新控制／四個主意願確實 OFF、fresh control0，而舊 global 仍 ON，證明沒有靠舊 global 提供顯示。App 重開後四組意願保存；舊程序警告仍停留的 UI 問題另修於 0.9.5。

### 0.9.5/code36：程序提示與 Root 負向測試界線

原簽章 APK SHA256 `795fe4222820aabd885788c511aa861131102cfc7fcc07459bbceee31374dc8d`。新鮮核對到不同 boot／PID／start 時只退下舊程序的畫面警告，不刪除原 journal、保存的 barrier 或改變同程序禁止重送。15 個新增純 JVM 檢查涵蓋同程序、不同 PID、不同 boot、未核對與未定 scope；合計 LensStatus 117 個。升級保留既有四組 OFF，舊 global ON 未反向開啟；實際 App 已不再把另一程序的舊警告放在目前遊戲上。

隔離 index5 的 `root_permission=false` 即時及關閉／啟動後，普通 App 仍能執行需要 UID0 的設定讀寫，因此不是有效的 Root 拒絕樣本，不能算負向通過；已恢復原 `true`。沒有為此安裝額外 Root manager／套件或更改其他 VM。Root 錯誤字串與非法／失敗不視為 OFF 的離線檢查通過，實際拒絕權限情境仍待可安全撤權的環境。

恢復設定後 boot25c53776-83a4-4c21-8909-c6e0ded74775，0.9.5 App 先開保存四組 ON，再開 Game157 PID2454/start16276；花苗持育待、成人已取得裝飾／葉任、待派期限與 D02 實際畫面再次成立。正常關閉／開啟 App，四組保存且舊程序警告正確退下；回遊戲沿用同一 renderer。再只正常重開遊戲，PID4871/start40605 由產品服務自行載入一次，新 context 下四組實際畫面再觀察，包括重新開啟精華 selector 的對照；沒有主機注入。OFF 後原金色花苗名／字級、成人副標消失、Kochi 及花瓣原數字還原。

0.9.5 四組說明逐一展開／收合，最後全部 OFF；foreground service 已停止。API35 接手時實存 43 份 attempt，0.9.4 一份、0.9.5 兩個新 PID 各一份，共 46；原 43 份及首個 0.9.5 journal SHA 全部不變。最後觀察到 180 個 thread 的 TracerPid 均 0，adbd2000，Root 設定恢復 true，正常關閉 index5。正常 journal 仍為 RETURNED_UNVERIFIED，沒有竄改成成功；畫面與 fresh status 另行證明各有界結果。

同一 APK 最後回歸 index4 API32，boot1de6d198-b393-4ef1-ad38-93d41ec68801；由 0.9.3 原簽章升級，保留四組 OFF、登入及歷史。先讓原遊戲 PID2774/start1891 到主畫面；未開透視鏡前 maps 無自有 SO、TracerPid0。再由正常 App 開啟四組並回遊戲，自行載入一次，八個實體回報帶 0.9.5-code36。實際看到花苗長名／持育待、離屏往返、成人已取得萬聖節／巴厘島／禮物貼紙／奶嘴及葉任、待派期限、精華 173／164／78／20 與花瓣／容量／餘量；四組 OFF 後原名與字級、副標、Kochi 及原花瓣數均還原。八個控制／四個主意願 OFF、fresh control0；原 8 份 attempt SHA 不變，本輪新增一份，ON／OFF 後仍共 9 份不變。最後觀察到 175 個 thread 的 TracerPid0，adbd2000，停止自有服務並正常關閉 index4。

此 0.9.5 API32 回歸是**遊戲先開**；沒有再次執行會遇既有音訊崩潰的遊戲單獨重啟。不能把這次成功改記為 API32 重啟或 0.9.5 App 先開通過。最新精簡矩陣、功能對照、私人安裝與截圖索引見 [DISPLAY_PORT_RESULTS.md](DISPLAY_PORT_RESULTS.md)。

## 私人安裝流程與限制

1. 使用表列可用的既有 Root MuMu；安裝同一原簽章 APK，首次允許透視鏡 Root。
2. 在 App 啟用所需主開關並確認失敗收容。舊 ON 設定升級後可按「開啟遊戲」補一次確認，不必先關再開。
3. 按「開啟遊戲」或自行開遊戲；App 內建服務等待符合資格的程序並載入 APK 內的 loader／host／renderer。
4. 關閉主開關並回原頁確認文字／字型還原。升級後若遊戲仍映射舊 APK，依提示正常重開遊戲。

不用安裝其他套件、模組、helper APK 或從電腦手動注入。以上是產品流程；本輪完整資格仍以實際矩陣為準。手機已由使用者排除本輪範圍；API31／34、新四組 Game156.x、root 拒絕實測、缺失／過期資料實境、跨帳號與耐久均未通過。

## 證據索引

原始圖片／診斷留在 ignored `.tools/display-port-baseline/`，不提交或公開：

| 功能 | 圖片檔名 |
|---|---|
| App 先開、四個收合開關 | 091-four-switches-initial.png、091-app-first-enabled.png |
| 花苗 ON／OFF／往返 | 091-seed-full-on.png、091-seed-off.png、091-seed-scroll-away.png、091-seed-scroll-return.png |
| 成年清單／詳細／OFF | 091-adult-settled.png、091-adult-detail-settled.png、091-adult-off.png |
| 探險可見狀態／OFF／原地址 | 091-expedition-on.png、091-expedition-off.png、091-expedition-detail-location-on.png |
| 精華對照／切換／OFF | 091-nectar-picker-open.png、091-nectar-yellow-filter.png、091-nectar-red-filter.png、091-stock-off.png |
| 升級明確要求重啟 | 092-upgrade-restart-required.png |

離線測試：九個 C++ policy、實際原子設定 writer／所有 fan-out 中斷位置／舊值擁有權／同 build 回報、JVM report parser、八項來源契約交叉檢查。GitHub CI `42962860320`、`42966730138` 通過；均不代表 Android 遊戲或耐久驗收。所有來源讀取固定 a02b58f；原四台與 AutoFarm 未操作。
