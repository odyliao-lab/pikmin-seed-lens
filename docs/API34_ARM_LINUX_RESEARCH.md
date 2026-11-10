# API34 ARM64／Linux 隔離研究

2026-11-10，使用者明確核准 [具體方案](API34_ARM_LINUX_PROPOSAL.md)，依
[三階段順序](STAGED_DELIVERY_PLAN.md) 優先進行 API34。這是軟體 ARM guest／研究 host 配置，
不代表原生 ARM64 手機、原廠無 helper 模擬器、x64 NativeBridge 或透視鏡產品已通過。

## 來源與隔離目標

新目標為專用 WSL2 `SeedLens-TCG-Linux` 內的 `SeedLens_API34_ARM64_Linux`，
Linux5588／5589、2vCPU／4GiB、全新6GiB ext4 data。沒有複製任何既有 guest userdata、
scanner、Google／遊戲 session 或憑證。原 API31 guest 關閉，index5 正常關閉後才啟動 CPU guest；
原四台及台中設備未操作。

Google 官方 Google APIs metadata 重新讀取，精確選 API34／ARM64／revision14，
archive `arm64-v8a-34_r14.zip` 實際1,610,393,229 bytes，SHA1
`2fe8b46d419a3400e30f31b0152b241b50c8b99f` 符合官方 metadata；實際 SHA256
`bd1c323cbdd3c9dd55dde54588b298d172fd126fe16e127a7bd7aeab6897890a`。
23個解壓檔案逐一驗大小／SHA256，保留 archive、manifest 與原 SDK 映像。

| 官方原件 | SHA256 |
|---|---|
| kernel-ranchu | `4076bd230e0832e23837e62c1fb03daf5eb4fd5aac731818320c26fa1bf8b2e5` |
| ramdisk.img | `5c4a40a87ea671396a683a30fde67a3e51c63a21d332a9c5a18ecf20c53e6185` |
| system.img | `53a061cd8eea937985357abd8d16d9581a6b34512cec241c40633051dd2a05d7` |
| vendor.img | `d5f44441eb7ced018ef09c09e25658bd9218d9633817f67d5b07d8f365876821` |

SDK emulator 仍為37.2.13/build16740729，固定 QEMU SHA
`e697effa1e3f5e0962dab1c8fdbca5787582e6081e2c22eabe74ff396fbd2436`。
來源 metadata 的 API34 不代替完整 Android guest 的實際 API 讀值。

## Host 研究守門

`build-tcg-software-selection.ps1 -Api34Research` 才建置新 mapper，獨立 library／marker、
AVD／5588且唯一參數、明確 software CPU，拒絕 KVM／衝突或重複參數。
新 mapper SHA256 `1b702320855a24203f8fb6b081050965566617fae7630758998b6803e60125a7`。
未加開關的 API31 helper 重建 SHA 仍為原 `bc30eb...e1e795`，沒有替換既有原件。

Vector 控制器以 `tcg_host_targets.py` 的兩個固定 tuple 分開 API31 與 API34；各自 marker、
AVD、port、mapper 路徑／SHA均配對，原 vector library／QEMU／MMU slots pin 不變。
allowlist 不是裝置操作授權。兩者都是 Linux host 研究檔，不打包 Android APK、不成為使用者依賴。

本輪實際驗證：

- 自有 Linux HOST guard fixture：13例通過，包括錯目標／port／marker、重複／衝突參數、
  保留真 host probe 呼叫及缺原函式實際 exit86。此 fixture 使用假的 `/proc` 讀取器，不稱為實際 QEMU 正例。
- 純 Python 固定目標 policy：5個測試方法通過，含跨 API tuple、缺／重複參數與 software 衝突負例。
- 真 gdb 控制器：API31／API34缺 marker、錯 marker／exe／preload共6例，均exit1、在 starti 前拒絕，未啟動 guest。
- 既有精確遊戲 profile evidence 測試16例回歸通過；產品版本、Root及 NativeBridge 守門未修改。

## CPU 與完整 Android 層

新 API34 官方 kernel＋同自有 V2 initramfs，實際 uname為
`6.1.23-android14-4-00257-g7e35917775b8-ab9964412`／aarch64。

| 新目標 CPU 輪 | 實際結果 |
|---|---|
| PID493，預設 TCG、無 mapper／vector helper | 5 pass／2 fail／0 skip；兩個 SDK 相鄰序列實際 SIGSEGV，父程序收容並繼續；正常 power-off／host exit0，非逾時 |
| gdb499／inferior509，normal TB＋API34 mapper／vector-save | 同 V2 原件7 pass／0 fail／0 skip；正常 power-off、inferior及debugger皆exit0，無收容訊號；不是以debugger exit0單獨判定 |
| gdb508／inferior522，同配置＋既有 V3c 診斷原件 | 7 pass／0 fail／0 skip；故意 NULL read 記到 address0／signal11，父程序 wait/reap 控制通過，沒有把 fault 算成功；正常 power-off／inferior exit0，非逾時／無收容訊號 |

故意 NULL-fatal 診斷控制已通過，完整 Android 另驗。CPU 正例不代替實際 Android API 讀值或完整開機。
沒有普通 App `su -M`、JNI、Root／產品或任何三遊戲資格，不更改九組矩陣的未通過狀態。

新 index5 關閉前，實際 OFF、30份歷史 attempt可見、無正在執行的 loader，讀取的遊戲
thread tracer皆0；adbd實際恢復UID2000，再正常 shutdown。關機後 CLI 不再提供 adb_port，
初稿因此在收尾解析報錯；重核名稱／created timestamp及關閉狀態完成確認，沒有重送 shutdown。
專用 WSL distro 在無 QEMU 並 sync 後重啟以回收下載快取；不改全域 WSL／Windows設定或重開主機。
啟動前 Windows 可用實體記憶體實讀約6.7GiB，Linux5588／5589可用。
