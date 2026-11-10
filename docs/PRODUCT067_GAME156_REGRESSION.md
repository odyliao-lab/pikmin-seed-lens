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

### 156.0首程序：庫存分項（2026-11-11）

本人回覆主畫面就緒後，重新核對完整APK／產品SHA及原42筆歷史。
實際PM欄位為appId=10068（首個host讀取假設userId而停止，未啟用；失敗保留）；
遊戲PID21939/start529785/UID10068，同boot400309d5…5a35。
只讀觀察新程序後舊UI屏障解除，兩項新同意、只送出一次啟用。

實際庫存357個，紅色五週年紀念玩具珠寶金苗完整縮字、照片鈕扣徽章與咖啡杯名稱正確；
首排完整離屏再返回仍正確。工具頁顯示已啟用，Switch直接OFF後原始金苗名稱及字級還原。
探險只有三筆完成水果，沒有未領回花苗；未領取／派遣，不把空缺當通過。

首次較早Root快照是在ON commit前，看到OFF且無renderer row，原件保留；不是最終結果。
後續UI新鮮READ已啟用及庫存實際效果另驗。最終OFF稽核為同PID/start的156.0 ready/control0、
205個thread TracerPid0、adbd UID2000。原42筆SHA不變，新增正常程序一筆，總43筆；
新journal SHA `63e732702a8d07cd70c5809fb5ac5afcf4f52d42a5c96f68eac859afe884bf46`，
仍為RETURNED_UNVERIFIED，不改成功、不重送。本組未記完成。

### 156.0正常重啟：庫存分項

OFF後正常停止舊程序，只啟動一次新程序PID26468/start603694/UID10068，同boot與帳號。
實際回到主畫面，未遇強制更新；產品冷回前景仍保留舊UI屏障，Switch disabled，
只讀核對新程序後才可重新同意。首次座標操作在disabled開關無作用、其後點到只讀；
該輪未展開確認／未送出，不記為新同意。另以節點確認兩項新勾選後，真正只載入一次。

357庫存的完整紀念長名縮小、首排真正離屏及返回均重驗；中間標為return的圖尚非頂端，
以後續完整頂端畫面為準。直接Switch OFF，實際原金苗名稱／字級恢復，UI OFF回報另核。
新程序載入後第一次稽核遇thread退出造成/proc讀取失敗，原件保留；重新只讀觀察通過，
沒有重送載入。最後同PID/start的156.0 ready/control0、172個thread TracerPid0、adbd UID2000。
原43筆SHA不變，新增一筆後44筆於ON至OFF期間SHA一致；新journal SHA為
`cba455888cea9645a95213be7437bf59e7ea96437b918c62f54373440b65ebc1`，仍為RETURNED_UNVERIFIED。
探險未領回花苗缺樣本，156.1本版回歸仍待接續；不記整組或UI階段完成。

156.0登入主畫面後重新綁定boot/PID/start/UID，驗新UI兩頁、庫存長名／真離屏、
OFF與遊戲重啟；之後正常升級精確156.1並獨立回歸。遇到強制更新保留現況，不繞過。
使用者選擇先保留現有短名探險樣本，探險長名縮字／真離屏仍缺有效樣本。
Android12／14的原遊戲SEVL障礙未解，不以此準備或API35結果轉記通過。
