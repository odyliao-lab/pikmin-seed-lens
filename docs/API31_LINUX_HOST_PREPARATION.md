# WSL2隔離研究：runtime已安裝、VMP待主機重開

## 最新：2026-11-09實際安裝結果

使用者回覆前次來不及操作、現在可重新開啟UAC後，重新核MSI pin／Microsoft簽章與既有安裝是否正在進行，才再次提升。UAC通過的PowerShell程序在執行腳本前被本機預設`Restricted`原則拒絕，事件4100另有實際證據；helper未開始，沒有把UAC通過或語法解析記為MSI成功。

改用Microsoft簽署的原生MSI及Windows內建DISM直接操作，沒有變更ExecutionPolicy。兩個必要UAC皆由本人確認：

- WSL3.0.3 MSI於19:30:30返回exit0，MSI log亦為0；`wsl --version`另實際exit0並回報3.0.3.0。這只證明桌面runtime，未當作Linux／Android開機。
- 僅對已授權VirtualMachinePlatform執行`/Enable-Feature /All /NoRestart`，19:32:29返回exit3010，日誌明確記錄restart suppressed。WMI其後InstallState1，但**仍須依3010完成主機重開**，不以此值宣稱可用。
- 舊Microsoft-Windows-Subsystem-Linux元件仍InstallState2，未另啟用WSL1；無ExecutionPolicy、BIOS、GPU驅動或Hyper-V開關操作。沒有發出主機重開指令，原四台與台中設備未操作。
- 新distro目錄仍不存在，index5關閉，研究5580–5587無listener；未匯入Linux、未啟動新Android、未安裝Root／App／產品，九組矩陣不升格。

私人`native-msi-state.json`、`native-vmp-state.json`及日誌保存實際結果；`resume-after-restart.json`保存重開前host boot與材料pointer。**不重跑已成功MSI／feature操作**；本人安排重開並回覆後，先核新的host boot、runtime與功能狀態，再依方案建立指定distro及API31 guest，先跑隔離CPU／失敗收容。是否需要任何額外元件，以當時實際錯誤為準，不盲目啟用。

以下「未開始／取消」為本輪較早的真實紀錄，保留沿革，以本段為最新狀態。

使用者對[具體Linux方案](API31_LINUX_HOST_PROPOSAL.md)回覆「同意」後，重新核對私人origin、HEAD `7c8850a88da30c513424404ea4a055b897466cf3`、乾淨工作區與新index5身分／關閉狀態，未操作原四台。依原方案不自行重開主機。

## 實際完成

| 材料／檢查 | 真實結果 |
|---|---|
| Microsoft WSL3.0.3 MSI | 367,575,040 bytes，SHA256 `ae977286974255bd26bf925f00f40f5692c6b2084155cbc0563c09aea091f211`；Authenticode實際Valid，signer為Microsoft Corporation，MSI ProductName Windows Subsystem for Linux、ProductVersion3.0.3.0 |
| Ubuntu24.04.5 WSL | 388,975,696 bytes，SHA256 `bb415d824822c4b878125729af451a5d18fb13d1cf5cbed9a7393ad64ac6039e`；原gzip保留，不在Windows展開Linux rootfs或宣稱已匯入 |
| Linux emulator37.2.13 | 349,658,551 bytes，官方metadata SHA1吻合；另記SHA256 `cbc7c3cfb83220b8a0f9c513fdab557712e74d339ec19ddffa43f23350ae9bdd`。只下載，未執行Linux binary |
| 官方API31／ARM64 rev11及自有V2 fixture | 僅從官方SDK system-image目錄取得12個檔案，加自有init／cpio共14個；逐檔SHA清單、來源大小／mtime核對及既有ramdisk／fixture pin均相符。私人tar為4,511,877,120 bytes，SHA256 `8e8b457eba5e835549dfac5079e891eb83f57df20b8090b09f1c769ab97eaee7` |
| 管理員腳本準備 | 先核MSI hash／Microsoft簽章；僅安裝此MSI與必要VirtualMachinePlatform。MSI使用`/norestart REBOOT=ReallySuppress`，feature使用`-NoRestart`；結果／restart-needed另存私人JSON。Windows PowerShell5語法解析實際通過，不把語法解析當執行或安裝通過 |

tar中的`userdata.img`只來自Google SDK的全新原廠映像，不是Windows AVD的`userdata-qemu.img`。沒有複製既有VM、遊戲／Google session、正式掃描器、簽章key或其他憑證。所有下載、tar、原始manifest、管理員helper及診斷只留私人資料夾／ignored `.tools`，不進Git。

## 安裝尚未開始的實際界線

目前shell非管理員。首次以portable PowerShell7要求提升時回報路徑錯誤，沒有啟動helper；再以系統Windows PowerShell提升時實際出現UAC consent程序，最後API回報1223／操作被使用者取消。無法由此判斷是本人取消、逾時或其他本機操作；沒有自動再次開啟UAC，亦未當作撤回整個開發授權。

管理員state檔未產生，MSI未啟動；readonly WMI另回報VirtualMachinePlatform及Microsoft-Windows-Subsystem-Linux的InstallState均2／停用。新distro目錄尚不存在，沒有WSL／Linux／Android啟動或App／Root／產品安裝證據，九組矩陣不升格。是否需主機重開，須等真正安裝／feature操作的結果；目前沒有要求立即重開。

私人準備目錄為`D:\SeedLensLab\wsl\prep-aa4f76e4557d414ba7bdca4357692827`，pointer在ignored `.tools/wsl-api31-session-path.txt`。待使用者能在本機完成管理員確認後，再啟動同一已核helper；先核有無已在進行的安裝，不能重複啟動。若實際回報restart-needed，保存狀態，交由本人安排重開；之後再核新host boot與WSL狀態，才建立指定distro及新API31 guest，先跑隔離能力／失敗收容。
