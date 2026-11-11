# 0.14.1／code52

依使用者2026-11-12授權，先合併 PR #15，再合併 README／更新索引與本發布紀錄；確認最終 main 的 Offline policy CI 通過後，才公開0.14.1預覽版。使用既有已實測APK，不重新建置、不覆寫舊版本。

- [功能與實測界線](FEATURE_INTENT.md)：五開關先保存意願，依條件核對有效控制及遊戲回報；首次套用同意與UNKNOWN不重送守門保留。
- MuMu API32／35、x86_64＋NativeBridge、精確 Game157.0/code1793360009：預選ON、等待遊戲、App自行載入、水果複製與OFF實測通過；API35另驗取消首次確認仍ON、App重建保留。
- 80項實際controller host檢查、原簽章完整建置、既有JVM與APK封裝檢查通過。Root拒絕／版本不符等以host條件替身驗證，未在裝置另造該條件。
- 本版未重驗所有四組畫面、花苗／左右換卡、手機、跨帳號與耐久；保留歷史驗收界線。
- APK：26,642,040 bytes；SHA256：`d41947c13e15b247df25218a5af468e8149d28bbbe98c7c364ea1f333bdc45c6`。
- 原簽章SHA256：`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`。
- 程式實作來源 `f7903ca6550e0c708b8de879d2f5724ddd35ef85`；其後只有文件、更新索引與測試期望值變更。發布來源tag以最終main建立。

[0.14.1發布頁](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.14.1)。App的main更新索引指向同一份APK；0.13.4及更早tag／資產保留不改寫。
