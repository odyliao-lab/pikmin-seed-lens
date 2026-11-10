# 分階段交付：先森林手帳 UI，再續平台驗收

## 最新順序（2026-11-11 使用者明確調整）

使用者回覆「先完成森林手帳 UI（建議）」。先收尾當時已啟動的API34共用dispatcher
隔離研究（正常／timeout／新程序回歸已完成並正常關閉），接著立即開發已選定的森林
手帳UI、工具頁直接開關、原簽章新版本APK與回歸。Android12／14三版遊戲仍列未通過，
繼續保留原必要目標與平台障礙，不以UI或自有JNI通過替代。

此次只調整順序，不授權新裝置／雲端、不操作原四台或台中，也不放寬預設NativeBridge
及精確遊戲守門。每階段commit／push私人分支、原簽章與新version/code要求不變。
以下原API34→API31→UI順序是先前歷史，各階段驗收門檻仍全部適用。

森林手帳已實作至原簽章0.6.5/code23，原生版面／直接開關有分項實測，完整產品回歸
進行中。圖片封裝修正與所有未通過項目見[UI驗收](FOREST_UI_QUALIFICATION.md)，不因來源
已commit而標記整階段完成。

2026-11-10 使用者確認依此順序持續推進，每階段分別 commit／push 私人功能分支。
工作分支 `codex/zhubei-android12-compat`；起點 `8b0624016c8d0d4f6446295f078b7d9cac54fdee`，
remote 為 `https://github.com/odyliao-lab/pikmin-seed-lens.git`，起點工作區乾淨、遞迴 pins 正確。

## 各階段完成門檻（原排列保留）

1. **Android 14／API34**：先自有平台與失敗收容，原遊戲正常啟動，再普通 App Root／JNI／產品。
   156.0／156.1／157.0 分別驗兩頁名稱、長字、真離屏重用、OFF 與新遊戲程序。
   既有 x64／libndk_translation 原遊戲 SEVL SIGILL 保留；不放寬 NativeBridge 守門。
2. **Android 12／API31**：同樣逐層驗證；API32 不替代。先前 ARM/Linux 未完成 boot、
   正常 ELF load UNKNOWN 與獨立 supervisor 正例各自保留，不轉記為完整 App 或產品成功。
3. **森林手帳 UI**：依 [已選定規格](UI_UX_REDESIGN.md) 開發，工具頁每項功能直接一個開關，
   必要啟用同意原地展開，OFF 直接提出且守住交易鎖。新 APK 使用新版本／code 及原簽章，
   再對目標 Android／遊戲組合回歸，不承接舊版畫面資格。

研究進展可有獨立中間 commit；階段完成須對照實測門檻，不能因已 commit 而標完成。
登入仍由使用者完成。單一產品 APK＋既有 Root，不將研究 helper／probe 變為使用者依賴。
原四台、台中 Libra／Virgo、正式 scanner 與憑證不操作／不複製；不 force、合併 main 或公開發布。

本機TCG尚未取得穩定平台資格時，另有[原生ARM雲端備案查核](ARM_CLOUD_RESEARCH_OPTION.md)。
使用者後續明確決定「先不使用雲端」；先前官方文件查核保留為歷史，不進行帳號、目錄
session、訂閱或雲端裝置操作。這不是既有本機授權的延伸；交付順序依上方最新指示。

## 本輪新鮮盤點

Windows 開機時間仍為本次已核准重開後的 2026-11-10 04:58:25，專用 WSL2
`SeedLens-TCG-Linux` 為 Ubuntu24.04.5／x86_64；未見存活的 QEMU guest。
Linux5586–5589 未占用，D 槽約817GB可用。Windows 當時可用實體記憶體約4.5GiB，
新 guest 啟動前須正常關閉本輪 index5 並再核資源，不把 WSL 虛擬可用量當主機實體餘量。
index5 CLI 名稱、created timestamp、16544端點與前輪相符；仍啟動中，未操作其登入畫面。

API34 ARM/Linux 是既有 API34 x64 之外的新目標，具體方案見
[API34 ARM/Linux 提案](API34_ARM_LINUX_PROPOSAL.md)。依使用者原有「具體目標確認後執行」
界線，本輪再次送出完整目標確認，使用者明確回覆「同意上述 API34 ARM64／Linux 隔離範圍」。
回覆前僅做來源、建置與拒絕測試準備，回覆後才開始下載與新 guest 準備；授權不等於產品驗收。
