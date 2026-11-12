# 0.15.1／code55

**發布後已知問題**：0.14.2→0.15.1 的 App 內更新在 MuMu API32 完整通過；API35 會停在等待 Android 安裝確認。0.15.1 的安裝回呼仍需修正。[追加驗收](UPDATE_0142_TO_0151.md)。

後續0.15.3已完成修正、API35隔離安裝驗收並[發布](RELEASE_0153.md)；不覆寫本版APK。[修正紀錄](API35_INSTALLER_FIX.md)。

新增首次檢查與 Root 排除建議、背景通知狀態與「關閉全部」，更新查詢失敗後保留快取並延後重試。修正背景通知未讀到新畫面回報、持續顯示等待的問題。

- 維持既有 Root＋單一 APK、原簽章升級與設定保存，不需額外模組或電腦注入。App 不會代改 Root 管理器設定。
- MuMu 內建 12.0（實際 Android 12L／API32）及 15.0（API35），精確遊戲 157.0/code1793360009：待種花苗 ON／OFF 還原、背景通知狀態、通知關閉全部通過；API35 另驗通知拒權時功能仍可使用。
- 真實更新查詢與 API32 離線後 5 分鐘退避／恢復已驗。30 分鐘／24 小時級距、Root 拒權與命名空間錯誤分支僅有主機測試；全新安裝導覽、手機及其他遊戲版本未在本輪驗證。其他遊戲頁面沿用既有驗收，不擴大資格。
- [詳細裝置證據與限制](VALIDATION_0151_MUMU.md) · [功能設計與離線測試](SETUP_NOTIFICATIONS_UPDATES.md)。

發布使用同一份已測 APK，不重新建置。程式來源 `7826033331a4b9e7ca8ac9ed0f8c861aca940f39`；其後只有驗收文件、README、更新索引與測試期望值變更。依授權先合併，確認最終 main CI 通過後才發布；來源 tag 為 `v0.15.1`。

- APK：26,658,425 bytes。
- SHA256：`a4d2e1578b344d0bdef0e89a0ce5a6664f3ba380e682ad7ccc463f5ef67f3bed`。
- 原簽章 SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。

[0.15.1 預覽版發布頁](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.15.1)。舊版 tag 與資產保留，README 與 App 預覽更新索引指向本版。
