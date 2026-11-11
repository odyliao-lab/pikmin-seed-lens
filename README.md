# 皮克敏花苗透視鏡

在 Pikmin Bloom 補充花苗、皮克敏、探險與精華資訊。僅修改顯示，關閉後還原原文字與字級。

<p align="center">
  <img src="docs/images/readme/illustration.jpg" width="260" alt="皮克敏拿著放大鏡觀察花苗的示意插畫">
</p>

**[下載 0.9.5 預覽版 APK](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.9.5)** · 開發者：odyliao-lab

0.9.5 對應遊戲 157.0；遊戲 156.1 使用[舊版 0.7.3 APK](https://github.com/odyliao-lab/pikmin-control-center-releases/releases/tag/seed-lens-v0.7.3)（僅花苗名稱顯示）。

## 四個功能開關

| 功能 | 顯示內容 |
|---|---|
| 花苗資訊 | 顏色、裝飾、收集與持有／育苗／待種統計、育苗詳細資訊 |
| 皮克敏資訊 | 目前花態、可收瓣／任務狀態、已取得裝飾 |
| 探險資訊 | 一般白盒對象與裝飾、探險狀態與遊戲剩餘時間 |
| 精華／花瓣對照 | 保留精華數量，補對應花瓣、容量與剩餘空間 |

單頁直接開關，說明預設收合。各項實測範圍見下表與[完整驗收矩陣](docs/DISPLAY_PORT_RESULTS.md)。

## 使用方式

需要既有 **Root＋單一 APK**，不需其他 App、額外顯示模組或電腦注入。介面固定直式，不變更物品或執行遊戲操作。

1. 安裝透視鏡並允許 Root。
2. 依下表流程開啟所需主開關，完成首次確認。
3. 保持遊戲在前景等待套用；卡片未更新時重新開啟列表。

MuMu 12.0 先讓遊戲到主畫面，再啟用透視鏡；MuMu 15.0 可先啟用透視鏡再開遊戲。若結果不明或升級後仍載入舊版，依提示重開遊戲，不對同一程序重送載入。

## 已驗證環境

以下均為 **MuMu x86_64＋NativeBridge**。0.9.5 已實測花苗庫存統計、成年裝飾／葉與任務副標、待派期限、精華／花瓣對照的 ON／OFF；資格限這些樣本與表列流程。

| MuMu Android | 實際 API | Pikmin Bloom | 驗證 APK | 通過範圍／流程 |
|---|---|---|---|---|
| 內建 12.0（Android 12L） | 32 | 157.0 | 0.9.5 | 上述樣本 ON／OFF、花苗離屏往返；遊戲先開 |
| 15.0（Android 15） | 35 | 157.0 | 0.9.5 | 上述樣本 ON／OFF、App 先開、遊戲重啟後自動重綁 |
| 內建 12.0（Android 12L） | 32 | 156.1 | 0.7.3 | 待種花苗名稱、縮字、OFF／再 ON |
| 15.0（Android 15） | 35 | 156.1 | 0.7.3 | 待種花苗名稱、縮字、OFF／再 ON；重開遊戲後通過 |

156.1 為舊版 0.7.3 結果，不代表 0.9.5 四組功能的資格。

[0.9.5 驗收矩陣](docs/DISPLAY_PORT_RESULTS.md) · [156.1 舊版紀錄](docs/PRODUCT073_GAME1561_STOCK.md) · [完整驗收紀錄](docs/VALIDATION.md)

## 效果預覽與介面參考

下圖由使用者提供，作為效果參考，並非本輪驗收截圖。

<table>
  <tr><th>花苗列表參考</th><th>皮克敏列表參考</th></tr>
  <tr>
    <td valign="top"><img src="docs/images/readme/seedling-reference.jpg" width="300" alt="花苗裝飾名稱與收集統計的參考畫面"></td>
    <td valign="top"><img src="docs/images/readme/pikmin-reference.jpg" width="300" alt="成長後皮克敏列表的介面參考"></td>
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

[架構](docs/ARCHITECTURE.md) · [精確遊戲設定檔](docs/GAME_PROFILES.md) · [來源與第三方授權](docs/PROVENANCE.md) · [安全回報](SECURITY.md)

提交變更請勿包含金鑰、帳號資料或遊戲檔案；[repo 安全設定與公開前待辦](docs/REPOSITORY_SECURITY.md)。

程式碼採 [MIT License](LICENSE)；第三方元件保留各自授權，角色與圖片不包含在程式碼授權範圍內。
