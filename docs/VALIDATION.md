# 驗證分層

## 匯入前的 0.1.4/code5 原型

- Java/dex/原簽章 APK 與 ARM64/x86_64 loader、ARM64 renderer 建置通過。
- 19 個 compatibility JVM 案例通過：API 邊界、ABI 拒絕與精確遊戲版本/code。
- Virgo 獨立 shell 的 identity 九例及命名／字型 policy 執行通過。
- 原控制中心完整 app/native verify 最終通過，但不涵蓋獨立 App 全部行為。
- Virgo Android15/x86_64/156.1：0.1.3 無公開 namespace 而退出，沒有 renderer 顯示成功。
  0.1.4 停用此 backend。測試後原掃描恢復，五個新 object／非零上傳／完成 ACK 有紀錄。
- Libra Android14/ARM64/156.1 與 Virgo Android15/x86_64/156.1 的後續畫面都是原地圖基準，
  不是本 App 花苗清單或開關驗收。未宣稱 Libra 已安裝本 App。

## 獨立 repo 初次匯入

本次只抽取來源、調整 include/build 路徑、使用 pinned submodule，並將簽章改成
明確 repo 外 keystore＋環境變數；產品執行來源不改，保留0.1.4/code5歷史身分。
不操作手機／模擬器，不發布二進位。

2026-11-08 在新 repo 的獨立建置通過（exit 0）：
- 19 個 compatibility JVM 案例通過。
- ARM64/x86_64 loader 與 native 測試程式編譯通過；本輪沒有執行 native 測試。
- ARM64 renderer、Java/dex、未簽章 APK 打包及 zipalign 通過。
- 未簽章 APK SHA256：`596ef7408e2aac6b19b3f3ec70be10861a82e7fedb42151e963a208dfbf5fa29`。
- 新的外部 keystore 簽章入口尚未實際簽署驗證；未簽章產物不等同既有已簽署 APK。
- 以上僅為建置／JVM 證據，不代表 root 載入、花苗畫面或多版本相容已通過。

## 下一階段

1. NativeBridge 隔離測試、timeout/restore 與 staging owner 交易。
2. 原生 ARM64 的獨立授權裝置驗收，含 Enforcing Root 環境。
3. 逐版本檢查庫存、探險列表／詳細頁、OFF 還原、捲動重用及遊戲／App 重啟。
4. 每個新遊戲版本新增整組已審查精確 profile；未知版本維持原文。
5. 不以本機測試、建置、安裝或心跳替代實際畫面驗收。
