# API31 ARM64 Linux host隔離研究

## 2026-11-10：本人重開後的新鮮核對

本人回覆主機已重開後，實際Windows boot為`2026-11-10T04:58:25.5+08:00`，與前輪`2026-10-28T21:35:26.4583330+08:00`不同。WSL仍為3.0.3.0，VirtualMachinePlatform實際InstallState1；WSL1 optional feature仍為2，未另啟用。沒有重跑MSI／DISM、改ExecutionPolicy、全域`.wslconfig`、BIOS或驅動。

依[已核准方案](API31_LINUX_HOST_PROPOSAL.md)，先重核Ubuntu原件SHA，再以明確`--version 2`匯入新`SeedLens-TCG-Linux`至`D:\SeedLensLab\wsl\SeedLens-TCG-Linux`，實際import exit0。新distro內實際`Ubuntu 24.04.5 LTS`、WSL host kernel `6.18.54.1-microsoft-standard-WSL2`／x86_64；不是Android guest的API／ABI證據。首輪綜合盤點命令最後因`unzip`未安裝返回1，不把它記作import失敗或工具成功；後續使用Python標準zip工具。

新index5／`SeedLens-Isolated-A15`重新核身分且維持關閉，Windows研究5580–5587未占用。原四台及台中設備未操作，不以其開機後的歷史PID作任何授權。新distro只安裝官方Ubuntu必要函式庫及Google SDK工具；Linux emulator實際回37.2.13.0/build16740729。Google官方Linux platform-tools下載SHA256為`d230f13842f60f782a8645f9c813f8f845bf36089ea7289f28c48f17979313f1`，實際ADB37.0.1/build15733141；不替換Windows ADB或全域kill-server。

官方SDK／自有fixture由前輪固定tar匯入，來源與pins見[準備紀錄](API31_LINUX_HOST_PREPARATION.md)。Linux lab位於`/opt/seedlens-lab`；不複製Windows AVD userdata、Google／遊戲session、scanner、簽章key或其他憑證。原始輸出及下載均留私人路徑，不進Git。

## 執行與驗收界線

先核逐檔manifest、SDK原ramdisk與V2 initramfs SHA，再以新`SeedLens_API31_ARM64_Linux`、2vCPU／4GB／5586–5587逐輪執行自有CPU fixture。每輪有獨立輸出、實際host PID與退出碼，逾時只收容該輪自己的程序群；預設與單指令TB分開記錄。完整Android須另取得online ADB、實際API31／ARM64／kernel／App能力，才能進後續Root及正常App研究。CPU正例不當作產品或遊戲通過，九組矩陣仍未完成。

## 同V2原件的Linux實跑

匯入後14個檔案逐一核大小／SHA全部相符，SDK為API31／ARM64 rev11；這是來源資格，不是guest實際API。兩輪使用同kernel、2vCPU／4GB／virt、SwiftShader與自有V2 ramdisk，只有單指令TB開關不同。

| Linux host程序 | 實際結果 |
|---|---|
| PID474，預設TCG | SEVL、plain STP Q、tagged STRB／STR Q／STP Q共5項通過；plain及tagged SDK相鄰序列各SIGSEGV，由父程序wait/reap後繼續。總5 pass／2 fail／0 skip，guest正常power-off，host實際exit0，非逾時 |
| PID633，單指令TB | 同原件7 pass／0 fail／0 skip，guest正常power-off，host實際exit0，非逾時 |

guest fixture uname實際`5.10.110-android12-9-00004-gb92ac325368e-ab8731800`／aarch64。此結果與Windows對照一致，**Linux並未解決預設TCG的相鄰序列失敗**；仍不是原keystore fault的根因或完整Android修復證明。沒有故意觸發alarm的測項，不宣稱逾時故障已驗。

首輪backend另記錄factory userdata placeholder的e2fsck resize exit8、尚無Linux ADB server、GLAsyncSwap read-only及其他backend警告，原件保留；自有init不掛載Android partition，因此不把CPU通過延伸成檔案系統或App通過。後續只啟動此新Linux的ADB server，未操作Windows ADB。完整Android另用新建6GiB sparse私人data檔，保留SDK的1MiB全零factory userdata及前兩輪檔案；不複製任何既有AVD使用者資料。
