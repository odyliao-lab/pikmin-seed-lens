# 四組唯讀顯示：私人測試交付

2026-11-11；四組唯讀顯示移植。原簽章 0.9.5/code36，APK 尚未發布；功能已接入真實 Game157 資料與 UI，仍有缺樣本及平台待驗，不能標成全部驗收完成。[完整證據與失敗紀錄](DISPLAY_PORT.md)。

## 使用流程

1. 在表列 MuMu 安裝單一透視鏡 APK，首次允許 App 使用既有 Root。
2. 在 App 啟用需要的主開關，首次確認載入失敗的停止／不重送規則。
3. 開啟精確版本遊戲；App 內建前景服務自行核對並綁定程序。
4. OFF 後回遊戲確認原文字與字型還原。APK 升級後若仍映射舊版，正常重開遊戲。

APK 已包含 loader、x64 NativeBridge host、ARM renderer 與 helper，不需其他 App／顯示模組、電腦注入或手動複製檔案。App 不從開機自動啟動；主機／模擬器重開後先開透視鏡。結果不明不得對同程序重送，依提示正常重開遊戲。

## 功能對照

| 主開關 | 已實接 | 實際畫面結果／剩餘樣本 |
|---|---|---|
| 花苗資訊 | 原名稱／縮字、D01 confirmed 已收／持育待／未知原因、育苗詳細頁步數列 | 庫存未收、待種正數及已知零數、長名、離屏往返與 OFF 有實測。已收／持有正數、育苗正數／詳細頁、具體未知原因缺實境樣本 |
| 皮克敏資訊 | D03 當前花態／可收瓣／任務、D04 已取得裝飾，合併短副標 | 葉、任務及已取得萬聖節／巴厘島／禮物貼紙／奶嘴、無飾、原詳細名稱及 OFF 有實測。花蕾／開花／枯萎／已確認可收瓣數待樣本 |
| 探險資訊 | D05 普通白盒對象與裝飾、遊戲狀態／時間及原可見性守門 | 待派期限、OFF 原地點及詳細頁原地址有實測；普通白盒、去程／搬運／返程、可見 Returned、長名稱／足量重用待樣本。隱藏 footer 不算顯示成功 |
| 精華／花瓣對照 | D02 實際 nectar-stock-preview 契約，原精華數及對應花瓣／容量／空間 | 精華 173 對瓣 97/300 餘 203；黃 164 對 15/300 餘 285；已知零、顏色切換與 OFF 有實測。滿容量、缺失／過期實境待樣本 |

四個直接開關，沒有細項或「部分開啟」；說明預設收合，可逐一展開／收合。主意願先保存後同步成員；已保存 OFF 不會被殘留 ON 蓋回。原生 writer 的各中斷位置、非法值、讀取失敗及舊值擁有權已離線測試；實際 0.7.3 ON 升級與四組 OFF 升級亦已觀察。

## 相容與生命週期

| 環境 | 遊戲 | 範圍與結果 |
|---|---|---|
| MuMu 內建 12.0，實際 Android12L／API32，x86_64＋libnb | 157.0/code1793360009 | 同一 0.9.5，遊戲先開→App 保存四組→自行載入，表列四組 ON／OFF、花苗離屏往返通過。先前遊戲單獨重啟遇音訊 native crash，無自有 SO 的對照亦重現；0.9.5 重啟／App 先開仍待驗 |
| MuMu 15.0，實際 Android15／API35，x86_64＋libnb | 157.0/code1793360009 | 0.9.5 App 先開、遊戲已開時重開 App、保存設定、沿用同 renderer、遊戲單獨重啟後新 PID 自動綁定及表列 ON／OFF 通過 |
| ARM64 手機 | — | 使用者已排除本輪，沒有手機資格 |

API32 不是 API31；不將這次結果擴大為 API31／34 或新四組的 Game156.x 資格。保留精確 version/code/ELF SHA/prologue/metadata、NativeBridge tuple 與 PID/start/boot 守門。

API35 環境沒有控制中心、研究 APK 或 `/data/adb/modules`；舊 global 保持 ON 時，新四組 OFF 仍還原。產品載入／設定均由普通 App 執行，adbd2000；ADB root 只於操作完成後唯讀取證並立即恢復。沒有讀取其他 App 的 TSV／快取提供功能。

## 測試與待驗

- 本機：雙 ABI loader、ARM renderer、原簽章、APK native 配對／16KB layout、Compatibility 57、RootCommand 57、LensStatus 117、LensContext 34、DisplayGroups 62 等離線檢查通過。
- CI：九個 native policy、真實原子設定 writer／中斷恢復、JVM protocol、八項來源契約交叉檢查均實際執行通過。原生 Android 測試程式僅編譯不算執行；CI 不代替遊戲。
- API35 本輪接手時 43 份 journal 全部 SHA 不變；0.9.4 一份、0.9.5 兩個新 PID 各一份，共 46。沒有對 UNKNOWN 重送或重寫歷史。最後觀察 180 個 thread 的 TracerPid 為 0。
- API32 最後同一 0.9.5 回歸，原 8 份 journal SHA 不變，僅新增一份，共 9；ON／OFF 仍不變，最後觀察 175 個 thread 的 TracerPid0。兩台最終皆四組 OFF、自有服務停止、adbd2000、正常關閉；原四台未操作。
- Root 關閉設定即時及重啟後仍未讓這台 MuMu 的普通 App 失去 UID0，故實際「Root 拒絕」仍待驗，原設定已恢復 true。
- 缺資料／過期、跨帳號、耐久、全新清潔安裝與上述缺樣本仍待驗；本輪保留既有登入／設定，以原簽章升級，不清除 App 資料來冒充全新安裝。

## 產物與實際截圖

私人 APK 位於 ignored `.tools/build/0.9.5/signed/pikmin-seed-lens-0.9.5-signed.apk`。

SHA256：`795fe4222820aabd885788c511aa861131102cfc7fcc07459bbceee31374dc8d`。

原簽章憑證 SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。

截圖／原始診斷依 AGENTS 不提交，保留 ignored `.tools/display-port-baseline/`。主要 API35 0.9.5 檔名：

| 驗收 | 實際圖片 |
|---|---|
| 單頁 UI／App 重開 | 095-api35-final-ui.png、095-api35-app-reopen-correct-session.png |
| 新 PID 花苗 ON／OFF | 095-api35-restart-seed-on.png、095-api35-seed-off.png |
| 新 PID 成年裝飾／花態 ON／OFF | 095-api35-restart-adult-on.png、095-api35-adult-off.png |
| 新 PID 探險 ON／OFF | 095-api35-restart-expedition-on.png、095-api35-expedition-off.png |
| 新 PID 精華對照 ON／OFF | 095-api35-restart-stock-visible.png、095-api35-stock-off-settled.png |

API32 同一 APK 的對照為 `095-api32-{seed,adult,expedition,stock}-{on,off}.png`；離屏往返為 `095-api32-seed-away.png`／`095-api32-seed-return.png`，最終 App 為 `095-api32-final-ui.png`。只描述實際截圖，不以檔名或 report ready 當畫面通過。

## 分階段 commit

| Commit | 內容 |
|---|---|
| 9fa4083 | 接手與自包含盤點 |
| c50814d、b60cef8 | 獨立 namespace、最小載入與 OFF 基線 |
| 5e6601b | D01 policy／adapter |
| b4c5653 | D03／D04 policy／adapter |
| 6638419 | D05／探險狀態 |
| 19e0eff | 育苗詳細頁步數列 |
| e0c26a3 | D02 精華／花瓣對照 |
| 4b2eb2e | 獨立 Game157 context、四個開關及自有前景服務 |
| 2c9b188 | 同 APK build 回報、具有擁有權的舊設定遷移 |
| 9e465f7、8769f28 | 背景核對與開關畫面一致 |
| 37b1dcf | 舊程序警告不冒充目前程序結果 |
| fc64937 | 實測與失敗紀錄 |

來源固定 `a02b58f35295948cbd29e467ce5ccd7ffe33161a` 唯讀選擇性移植；未修改來源 repo、既有模組、AutoFarm 或原四台。
