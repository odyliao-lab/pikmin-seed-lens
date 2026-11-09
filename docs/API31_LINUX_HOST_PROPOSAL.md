# 已授權：Linux host上的API31 ARM64隔離研究（待主機重開）

**使用者已回覆「同意」，後續也完成兩個必要UAC。** 三份固定官方安裝檔及SDK／fixture已核；WSL3.0.3原生MSI實際exit0、CLI版本再核，VirtualMachinePlatform原生DISM實際exit3010，要求本人安排主機重開。兩者均抑制自動重開；Linux distro與新Android尚未建立，不能當作相容成功。較早UAC取消及腳本原則拒絕均保留於[準備／安裝紀錄](API31_LINUX_HOST_PREPARATION.md)，不重跑已成功安裝。本方案不改九組矩陣。Windows ARM64後端的自有CPU序列已有對照證據，但完整Android在hostGL與SwiftShader均未到ADB就緒；後者實際Windows access violation，見[本次研究](API31_ARM64_CPU_MEMORY_RESEARCH.md)。Linux比較是否能避開此問題仍須實測，不能保證。

## 新鮮盤點與需重新確認的原因

竹北本機`wsl --status`實際exit50，明確顯示WSL未安裝；沒有發現可用`New-VM`／`Get-VM`命令。只作唯讀盤點，未啟用任何功能。本機目前新研究AVD及index5均關閉，原四台未操作。

原先已確認的環境界線包含「不改Hyper-V或驅動」。新WSL2方案會新增Microsoft WSL runtime，必要時啟用Virtual Machine Platform Windows元件，可能需要管理員權限與主機重開；[Microsoft安裝說明](https://learn.microsoft.com/windows/wsl/install-manual)及[Canonical說明](https://documentation.ubuntu.com/wsl/latest/guides/install-ubuntu-wsl2/)均列出這些前提。因此需使用者明確確認放寬此範圍；既有Android Root授權不被當作Windows元件或重開機授權。

## 具體目標與固定來源

| 項目 | 待確認範圍 |
|---|---|
| WSL runtime | [Microsoft WSL 3.0.3 x64 MSI](https://github.com/microsoft/WSL/releases/download/3.0.3/wsl.3.0.3.0.x64.msi)，367,575,040 bytes；官方release digest SHA256 `ae977286974255bd26bf925f00f40f5692c6b2084155cbc0563c09aea091f211`。下載後先核雜湊及Microsoft Authenticode，使用WSL2 backend |
| 新Linux distro | 名稱`SeedLens-TCG-Linux`，新資料`D:\SeedLensLab\wsl\SeedLens-TCG-Linux`；[Canonical Ubuntu24.04.5 WSL amd64](https://releases.ubuntu.com/24.04/ubuntu-24.04.5-wsl-amd64.wsl)，388,975,696 bytes；[官方SHA256SUMS](https://releases.ubuntu.com/24.04/SHA256SUMS)為`bb415d824822c4b878125729af451a5d18fb13d1cf5cbed9a7393ad64ac6039e`。新建乾淨環境，不匯入既有VM／帳號／憑證 |
| Linux emulator | [Google官方SDK metadata](https://dl.google.com/android/repository/repository2-3.xml)的stable channel0／37.2.13，`emulator-linux_x64-16740729.zip`／349,658,551 bytes／metadata SHA1 `f41f9e31eb5ca64926603c03265c50bbc0ff4172`，同Windows研究版本。下載後另記SHA256，不取channel2的37.3.8 |
| Android guest | 新`SeedLens_API31_ARM64_Linux`，2vCPU／4GB，預定Linux側5586／5587，啟動前重查埠；官方同API31 ARM64 rev11原始SDK映像另存Linux私人lab並核SHA，不複製Windows AVD userdata或遊戲session |
| CPU與GPU | 先官方Linux後端／軟體CPU、自有initramfs原件，預設與單指令TB比較；再官方Android ramdisk及軟體GPU測實際boot／API／ABI。不是原生ARM64主機或x64/NativeBridge資格 |

MSI／Ubuntu／emulator合計約1.11GB下載，另需套件與Android研究檔案空間。只一台新Android guest同時運行；index5與其他新研究guest保持關閉。Linux host本身的額外記憶體與實際效能須核對，不以QEMU的4GB當WSL整體限制。

## 執行界線與順序

1. 使用者確認後才下載、驗章／雜湊、安裝runtime與必要Windows元件。**不自行重開主機**；若需重開，保存狀態並等使用者安排，不能為此停止原四台或其他正式工作。
2. 建立上述專用distro，不改其他distro、全域`.wslconfig`、BIOS、GPU驅動或Hyper-V開關。只傳入官方SDK映像及本repo自有研究fixture；不複製正式scanner、帳號、簽章key、SSH設定或其他憑證。
3. 核Linux host／QEMU程序及新guest身分，先跑自有CPU／失敗收容，保留失敗與完整退出碼；再觀察官方Android。取得實際API31／AArch64／完整App環境之前，不裝產品或宣告相容。
4. 若能開機，再依已核准Root方針用官方Magisk31.1私人ramdisk override、保留SDK原件與SELinux、不啟用Zygisk／顯示模組，先完成普通App Root／ARM JNI／CPU context／正常與失敗收容，再進產品。Google／遊戲登入由本人；遊戲與透視鏡仍各自精確版本／原簽章／新鮮程序守門。
5. API34 ARM guest、新裝置、主機重開或本提案之外的主機變更另確認；不將本次API31研究擴張為任意設備操作。所有研究材料留私人ignored路徑，repo只收來源／去敏結果。
