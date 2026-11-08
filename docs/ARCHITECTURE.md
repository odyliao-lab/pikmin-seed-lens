# 架構

`app/` 包含 Android UI、root loader、JNI payload 及建置入口；
`native/renderer/` 是自控制中心抽取的花苗命名／縮字 renderer、純 policy、SHA 與 GC handle 支援；
`native/tests/` 保留既有命名／字型 policy 測試。

平台載入後端與遊戲精確 profile 是兩個獨立相容性層。
遊戲 version/code、ELF size/SHA、prologue、runtime metadata 必須全部對應。
編譯出某 ABI 不表示該裝置可載入；不放寬守門來宣稱多版本。

APK 內嵌兩個 loader build targets 及 ARM64 renderer。沒有 Magisk 模組、服務、
INTERNET 權限、開機自啟、掃描器控制或遊戲 RPC。
控制檔 `/data/local/tmp/pikmin-seed-preview.txt` 與遊戲的 `seed_preview_status.tsv`
沿用原協定；只接受同遊戲 PID 五秒內回報，`waiting-cards` 不代表實際顯示成功。

Root loader 僅接收精確主遊戲程序，核 UID、start ticks、bootID、payload owner/mode、
ELF digest 及重複 renderer；native flock 加持久 attempt 防止不確定載入後重送。
不提供 upstream CLI 的任意程序、隱藏、memfd、watch 或 unload 選項。
OFF 由遊戲 UI thread 還原已綁定文字；不能 dlclose 尚有 live hooks 的 library。

## 尚未完成的安全工程

- NativeBridge 必須先透過隔離 harness 確認合法 namespace，而非猜 pointer。
- 所有 remote syscall 等待需真正有界，證明中斷時 register/PC 可恢復。
- Java 的 payload staging 在 native lock 之前，需整合持久 privileged owner；
  UI process-wide AtomicBoolean 不是跨程序崩潰安全交易。
- 禁止因新 PID 出現就把舊 attempt 算成功；未知結果與失敗證據保留。

本 repo 與原控制中心之後分開管理；此處不引入派遣、GPS、HC、育苗或更新模組系統。
