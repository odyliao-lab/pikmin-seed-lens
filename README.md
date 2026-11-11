# 皮克敏花苗透視鏡

在 Pikmin Bloom 中顯示花苗的裝飾名稱，長名稱自動縮小，關閉後還原原名與字級。

<p align="center">
  <img src="docs/images/readme/illustration.jpg" width="260" alt="皮克敏拿著放大鏡觀察花苗的示意插畫">
</p>

**[下載 0.7.3 預覽版 APK](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.7.3)** · 開發者：odyliao-lab

## 使用方式

需要既有 **Root＋單一 APK**，不需控制中心、LSPosed 或額外顯示模組。介面固定直式，功能直接開關；僅修改顯示，不變更物品或執行遊戲操作。

1. 開啟遊戲並停在主畫面。
2. 開啟透視鏡、允許 Root，完成當次確認後啟用。
3. 保持遊戲在前景等待完成，再開啟花苗頁；卡片未更新時重新開啟列表。

若顯示結果不明，依提示重開遊戲並重新核對，不對同一程序重送載入。

## 已驗證環境

以下均為 **MuMu x86_64＋NativeBridge** 的待種花苗實測，範圍為名稱、縮字、OFF 還原與再次 ON；探險不在本輪驗收範圍。

| MuMu Android | 實際 API | Pikmin Bloom | 驗證結果 |
|---|---|---|---|
| 內建 12.0（Android 12L） | 32 | 156.1 | 0.7.3 通過 |
| 內建 12.0（Android 12L） | 32 | 157.0 | 0.7.2 開關通過；0.7.3 已驗顯示延續 |
| 15.0（Android 15） | 35 | 156.1 | 0.7.3 重開遊戲後通過，見下方限制 |
| 15.0（Android 15） | 35 | 157.0 | 0.7.3 通過 |

- API35／156.1 首輪掛接未完成，保留 UNKNOWN 後改新程序通過；照片卡首輪曾省略，OFF／再 ON 後完整顯示。
- Android 12／API31、Android 14／API34 尚未通過；API32 不代表 API31，其他裝置也不承接上述資格。
- [156.1 驗收紀錄](docs/PRODUCT073_GAME1561_STOCK.md) · [157／API35 紀錄](docs/PRODUCT073_API35_STOCK.md) · [完整驗收紀錄](docs/VALIDATION.md)

## 效果預覽與介面參考

下圖由使用者提供，並非 0.7.3 驗收截圖。圖中的收集統計與成長後皮克敏列表不屬於目前版本功能。

<table>
  <tr><th>花苗列表參考</th><th>皮克敏列表參考</th></tr>
  <tr>
    <td valign="top"><img src="docs/images/readme/seedling-reference.jpg" width="300" alt="花苗裝飾名稱與收集統計的參考畫面"></td>
    <td valign="top"><img src="docs/images/readme/pikmin-reference.jpg" width="300" alt="成長後皮克敏列表的介面參考，非本版功能"></td>
  </tr>
</table>

## 建置與文件

需要 PowerShell 7、JDK 17、Android SDK platform 35／build-tools 35.0.0、NDK r27d、CMake 3.21+ 與 Ninja。將下列工具路徑改為本機位置：

```powershell
git submodule update --init --recursive
pwsh -NoProfile -File app/build.ps1 `
  -SdkPath "C:/Android/sdk" -JavaHome "C:/Tools/jdk17" `
  -NdkPath "C:/Tools/android-ndk-r27d" `
  -CmakePath "C:/Tools/cmake/bin/cmake.exe" -NinjaPath "C:/Tools/ninja.exe" -Unsigned
```

未簽章產物僅供建置驗證。簽章使用 repo 外的 keystore 與環境變數密碼；新交付 APK 須升版並保留原簽章。

[架構](docs/ARCHITECTURE.md) · [精確遊戲設定檔](docs/GAME_PROFILES.md) · [來源與第三方授權](docs/PROVENANCE.md)

專案開源授權尚待選定；第三方元件依各自授權使用。
