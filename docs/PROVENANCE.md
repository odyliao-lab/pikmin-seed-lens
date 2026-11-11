# 來源與授權

2026-11-08 使用者要求建立獨立私人 `odyliao-lab/pikmin-seed-lens` 並提交推送。
保留使用者建立的初始提交；原 Control Center 工作副本和未提交改動未刪除或提交。

## 原型來源

- 原私人 repo：`odyliao-lab/pikmin-control-center`。
- 讀取基底：`0a213ef2ba464e6585ef49b04e59f183f0a8b6ac`。
- Android/root prototype 來自當時未提交的 `experimental/seed-preview-app` 0.1.4/code5，
  不宣稱它已存在於上述 commit。
- Renderer 取 `native/automation/zygisk-module/cpp/seed_preview*` 及 `managed_gc.h`。
- Policy test 取同來源 `seed_preview_policy_test.cpp`，只調整 include 路徑。
- 不帶入原 repo 歷史、遊戲二進位、簽章、App資料、截圖或原始診斷。

## 第三方

- `third_party/AndKittyInjector`：MIT，Git submodule 精確固定
  `9ece8bc0f05344ab23e882a38728ab91f70bfed0`。
- 其 `KittyMemoryEx`：MIT，巢狀 submodule 固定
  `be4cdc9a79e3e1b93357e8d30608e84a3518f5d3`。
- `third_party/And64InlineHook`、`third_party/xdl`：從原 renderer 使用的來源原樣保留，
  原 copyright／license headers 保留，完整文字見 [RENDERER_NOTICES](../third_party/RENDERER_NOTICES.md)。
  該沿革 notices 含 Zygisk header 條目，本 repo 未使用／安裝 Zygisk 模組。
- 建置將上述必要 notices 包入 APK，沒有提供第三方通用 injector CLI。

## 本專案授權

2026-11-11 使用者授權公開 repo，並指定程式碼採 [MIT License](../LICENSE)。第三方元件保留各自原授權；程式碼的 MIT 授權不授予皮克敏角色或參考圖片的權利。

Game157 四組顯示另依固定來源 `a02b58f35295948cbd29e467ce5ccd7ffe33161a` 選擇性移植；來源與契約見 [DISPLAY_SOURCE_CONTRACTS](DISPLAY_SOURCE_CONTRACTS.json)，獨立整合與驗證見 [DISPLAY_PORT_RESULTS](DISPLAY_PORT_RESULTS.md)。

## 診斷與更新來源

診斷 schema、接收端、同意／匯出與私人更新 interlock 為本 repo 實作。`LensUpdateInfo`／`LensUpdateClient` 選擇性改寫自使用者授權唯讀來源的 `UpdateInfo.java`／`UpdateClient.java`（來源 HEAD `89569e047ee3ad590d4092e6d44eea3a9f5c04cb`）；安裝流程只參考 Android PackageInstaller 使用者確認方式，不移植核心、GPS、排程或自動化流程，也未修改來源 repo。
