# WSL2隔離研究：已授權、材料已核、待本機管理員操作

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
