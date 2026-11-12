皮克敏花苗透視鏡 **0.15.3／code57**（預覽版）

- 修正 Android15 上 App 內更新卡在「等待安裝確認」的問題。
- 安裝確認視窗無法開啟時提示取消與重試；取消失敗保留待處理狀態，不誤報成功。
- 保留 Android 系統確認、原簽章與設定，維持既有 Root＋單一 APK，不需要額外模組。

**舊版 Android15 使用者請先下載下方 APK，直接覆蓋安裝；不要卸載或清除資料。** 舊安裝器不能由下載的新 APK 回溯修復；完成這次手動升級後，後續更新才會使用修正後流程。升級後依提示正常重開遊戲。

**本輪實測**：隔離 MuMu 15.0（API35）完成0.15.2→0.15.3的系統確認、取消、重開App重試、升級及設定保存；實裝APK SHA與原件一致。使用測試快取核對安裝階段，不把它當公開下載驗收。47項HOST回呼回歸通過。

遊戲資格維持精確157.0；MuMu內建12.0（實際Android12L／API32）與15.0的遊戲顯示沿用既有分項證據，本輪未重跑遊戲或API32裝置，手機仍待驗。沒有擴大相容資格。

[Repo](https://github.com/odyliao-lab/pikmin-seed-lens) · [來源](https://github.com/odyliao-lab/pikmin-seed-lens/tree/v0.15.3) · [安裝驗收](https://github.com/odyliao-lab/pikmin-seed-lens/blob/v0.15.3/docs/API35_INSTALLER_FIX.md)

下載 `pikmin-seed-lens-0.15.3.apk`。使用同一份已測APK，不重建或覆寫已交付版本。

APK SHA256：`69464a224086c10cbb5ffa5e455bf91801449bce8e504d9e7ad026edb0bb3129`

發布來源 main `10f9a3c2cbbcb4558010bcdfba978ff0c9e10669`／PR20，main CI43170053849通過後，以同一份已測原簽章APK發布；source tag `v0.15.3`。三份附件白名單為APK、sha256sums與build-provenance，沒有原始診斷或私人資料。

公開Release已確認draft=false、prerelease=true、immutable=true；匿名下載APK大小26,658,424 bytes與上述SHA一致。App預覽索引同步至本版，原已發布版本資產保持不變。

[0.15.3發布頁](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.15.3)
