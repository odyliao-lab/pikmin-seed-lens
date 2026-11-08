# pikmin-seed-lens
皮克敏花苗透視鏡

獨立 Android APK 的開發原型：只顯示花苗名稱與縮小長文字，不修改遊戲物品。
目標是使用既有 Root，不另安裝控制中心、LSPosed 或花苗顯示模組。

**目前不是可交付使用的正式版。** 匯入基準為 `0.1.4/code5`，包名
`dev.ody.seedpreview`；既有已交付 APK／簽章不改寫。本次只是將開發原型獨立管理，
不安裝、不發布，也不把原型改名後重新交付成相同版本。正式產品名稱採「皮克敏花苗透視鏡」，
原型 App 內標題暫保留原交付身分，後續改版再同步。

## 已知限制

- 只有遊戲 **156.1/code1793076318** 的精確 renderer 設定；其他版本保持原顯示。
- Android API28–35／ARM64 只是候選範圍，尚未有這個 root-only APK 的顯示通過證據。
- Virgo Android15／x86_64 的 NativeBridge 試載失敗：沒有可用的公開 namespace，
  另有 remote syscall 等待未受預期 timeout 限制。0.1.4 已在 Java/native 兩層停用 x86_64 載入。
- Root provider 需支援 `su -M` 的全域 mount view；沒有宣稱所有 Root／SELinux 環境支援。
- 無自動開機或遊戲重啟接續；目前由使用者確認已登入主畫面後啟用。
- 模糊載入結果保留 attempt，不自動重試；完整跨程序 staging/owner 交易仍待完善。

Virgo 測試後已恢復原掃描並取得新的非零資料／上傳／ACK；Libra、Virgo 後續截圖
都只是原地圖基準，不是花苗顯示成功證據。原始截圖與私人紀錄沒有匯入這個 repo。

## 取得來源與建置

```powershell
git clone --recurse-submodules https://github.com/odyliao-lab/pikmin-seed-lens.git
cd pikmin-seed-lens
# 已經 clone 時：git submodule update --init --recursive
pwsh -NoProfile -File app/build.ps1 `
  -SdkPath C:/Android/sdk -JavaHome C:/Tools/jdk17 `
  -NdkPath C:/Tools/android-ndk-r27d `
  -CmakePath C:/Tools/cmake/bin/cmake.exe -NinjaPath C:/Tools/ninja.exe -Unsigned
```

將範例工具路徑改為本機實際位置。需要 PowerShell7、JDK17、SDK platform35／
build-tools35.0.0、NDK r27d、CMake3.21+及Ninja。建置不安裝、不操作裝置。
`-Unsigned` 產物只供建置驗證，不可視為原簽章升級包。

簽章建置不帶 `-Unsigned`，改傳 repo 外的 `-SigningKey`。
密碼由既有安全來源放入 `SEED_LENS_STORE_PASS`／`SEED_LENS_KEY_PASS` 環境變數；
可用 `-StorePassEnv`／`-KeyPassEnv` 指定其他變數名稱。不得把密碼放命令列或 repo。
腳本驗證既有憑證指紋，沒有新建／複製 keystore，也不依賴控制中心的私有建置 helper。

輸出在忽略的 `.tools/`，同一路徑已有最終產物時拒絕覆寫。
重建結果不是先前已交付 0.1.4 的位元組保證；日後交付不同 APK 必須升版本／code。

## 文件

- [架構與來源界線](docs/ARCHITECTURE.md)
- [驗證結果與待辦](docs/VALIDATION.md)
- [來源沿革與第三方授權](docs/PROVENANCE.md)

專案原始碼保持私有；第三方元件各依其原有授權，沒有替整個專案新增開源授權。
