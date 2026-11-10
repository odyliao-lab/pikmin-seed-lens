# 0.6.7：156.x本版回歸（進行中）

2026-11-11，只在授權index5／SeedLens-Isolated-A15，created1794181954477010，
API35／x86_64／libnb.so。產品0.6.7/code25原簽章及APK SHA維持不變；
下列準備不代表遊戲或兩頁已驗收，不承接0.5.2的資格。

## 合法原件與版本切換

兩版完整base／ARM64 split逐份重核原遊戲944b…e979憑證、version/code、APK SHA、
libil2cpp及metadata SHA，符合[既有精確profile](GAME_PROFILES.md)。

- 156.0/code1792142832，base `61c13513f3710b469f33ae3f63af9eaeca7b80fa046f3669ac73fa35392bdd8a`，
  split `6aa9c204928b5f80164b4206c27960d0467038db2cc192827e5e102785b4f960`。
- 156.1/code1793076318，base `070c75d6e253317438dc4ac1b0f60b5dbf9e5250763827b7c6b6197f0e81e9dc`，
  split `7b05aa1c10acad9266c1daeea68d466c138b2ecb35a9d03717af273dd13e4836`。

157已完成本版OFF／兩頁還原、42筆歷史一致後正常關閉。標準install-multiple -r -d
只提出一次，系統明確INSTALL_FAILED_VERSION_DOWNGRADE，原157 APK不變。
首個host斷言只看stdout、錯誤實在stderr；保留該失敗，另以只讀觀察確認拒絕，沒有重送安裝。

隨後依既有精確遊戲回歸範圍，在同一新隔離機乾淨安裝156.0完整兩件，PM版號與逐件
SHA均核對成功；本產品067、外觀偏好、UI／native歷史保留，42筆SHA仍一致、control OFF、
adbd UID2000。未複製遊戲登入資料／憑證；乾淨安裝須由使用者本人重登同一樣本帳號。
只正常啟動一次供登入，沒有提出產品ENABLE，沒有派遣或領取。

## 待驗

156.0登入主畫面後重新綁定boot/PID/start/UID，驗新UI兩頁、庫存長名／真離屏、
OFF與遊戲重啟；之後正常升級精確156.1並獨立回歸。遇到強制更新保留現況，不繞過。
使用者選擇先保留現有短名探險樣本，探險長名縮字／真離屏仍缺有效樣本。
Android12／14的原遊戲SEVL障礙未解，不以此準備或API35結果轉記通過。
