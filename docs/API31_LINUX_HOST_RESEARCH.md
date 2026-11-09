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

## 完整Android的data初始化比較

第一輪PID514使用尚未格式化的6GiB sparse data；guest實際報invalid ext4 superblock、`vdc cryptfs mountFstab`失敗，後續`/data`仍唯讀，keystore2因無法開啟其persistent.sqlite而abort。不是前述GenerateBcc SIGSEGV，也不是Linux host crash。指定新guest console正常`emu kill`回OK，supervisor取得host exit0／非逾時；ADB一直offline，沒有App、Root、遊戲或產品資格。

保留上述data及overlay，另新建`userdata-android-ext4.img`，以Ubuntu mke2fs1.47.0初始化6GiB ext4／encrypt feature，避免較新orphan_file及metadata_csum_seed feature；沒有修改官方ramdisk、system、fstab或停用加密。第二輪PID807沿用同官方ramdisk／單指令TB，實際`/data` ext4 mount成功，原廠init繼續`cryptfs encryptFstab`、metadata加密及`enablefilecrypto`。這些中間進度仍不是完整開機或產品通過。

第二輪其後完成加密及post-fs-data、開始啟動各HAL，但host於12分12秒實際SIGSEGV／exit -11，未達15分鐘上限；不是正常關閉，亦不是guest signal被收容成功。當時ADB仍offline，沒有實際API／App就緒或Root／遊戲／產品。最後輸出停在sensors／RenderThread附近，不能單憑最後一行認定根因。Linux仍有host退出問題，不能將換OS稱為修好；未修改SDK或放寬產品准入。

## 追加隔離比較與host診斷

仍先用自有V2 initramfs：PID1324明確`-accel tcg,thread=single`、PID1536明確`-d nochain`各自實際5 pass／2相鄰序列SIGSEGV fail／0 skip，均正常power-off、host exit0。這兩個QEMU選項不是單指令TB，均未修復序列失敗；沒有直接套用到遊戲。

新Linux內另安裝Ubuntu官方gdb15.1／binutils，先冷備份本次無帳號／產品的私人data overlay及metadata，再對同guest單指令TB完整啟動作host診斷，最長30分鐘。debugger必須將訊號交還QEMU，分辨其正常protected-memory處理與真正致命退出；**debugger stop或gdb exit0均不當作host正常返回／Android通過**。原始stack、映像、下載及logs只在私人lab；本輪尚無產品變更。

首個gdb輪的inferior1716亦實際以SIGSEGV終止，gdb1706自身exit0；watchdog以pidfd觀察到inferior退出，沒有送出收容訊號。該輪`handle SIGSEGV stop noprint pass`設定有誤：實際`info signals`顯示Stop No，沒有取得fault前stack，不冒稱已有根因。改為`stop print pass`後，先在一次性自有Linux程序故意NULL read：實際捕捉address0／一次stop／指令及stack，再交還訊號並確認程序SIGSEGV結束。這只是debugger診斷能力正例，與Android、Root及產品資格無關；修正後才重跑同guest。

### 修正後捕捉到的實際host fault

gdb2129／inferior2139／startTicks246595的同guest重現：fault位於QEMU主ELF相對位址`+0xd58f91`，指令`mov (%rbx),%rax`，RBX實際0、fault address0；當時為QEMU thread25／LWP2218，不能以最後RenderThread訊息認定該thread崩潰。交還SIGSEGV後，inferior確實因該signal終止；debugger exit0、非逾時、pidfd supervisor未送收容訊號。SDK ELF SHA256 `e697effa1e3f5e0962dab1c8fdbca5787582e6081e2c22eabe74ff396fbd2436`，GNU Build ID `c3842eb0610d4bd8adbe4f493a1a1038679b381c`；未修改binary。

ELF已strip，不能把距離很遠的最近export `goldfish_address_space_memory_state_save`當fault函式名。實際反組譯顯示：caller透過control-ops的第四slot呼叫，callee從`AddressSpaceContextDescription` hash entry取指標後發生null read。與[固定aemu來源的ping／tellPingInfo](https://android.googlesource.com/platform/hardware/google/aemu/+/7e11d40d0aa8ee3cfa8012c5eeaee01e1e765ccd/host-common/address_space_device.cpp)結構一致，支持`pingInfo`為null的推論；這仍不是SDK完整source／build manifest匹配或「為何映射為null」的根因證明。stack後方進入JIT／匿名區段，沒有將其後不確定frame當完整call chain。

runtime實際在軟體CPU下自動啟用GLDirectMem，故下一個比較只明確關閉此選項；先以同自有V2 fixture確認CPU／收容，再測完整Android。這是同一私人guest的官方runtime選項比較，不patch遊戲、SDK／kernel／SELinux，也不調整產品准入。

此比較的自有V2輪PID2535實際7 pass／0 fail／0 skip、正常power-off及host exit0。runtime解析`-Vulkan,-GLDirectMem`並記錄override disabled；初始化仍有無條件的Enabling文字，保留原件，不靠單一文字宣稱完整Android成功。其後才執行同guest的完整Android比較。
