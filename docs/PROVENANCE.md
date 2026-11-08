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

上述第三方授權不代表本專案其他私人程式自動改為開源。
