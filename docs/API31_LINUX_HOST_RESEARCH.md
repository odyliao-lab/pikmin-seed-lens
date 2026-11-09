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

完整比較gdb2667／inferior2677仍在同`+0xd58f91`／RBX0／fault address0終止，沒有改善；非逾時、supervisor未送收容訊號。該輪另外捕捉hash entry的記憶體，與前述結構對照的pingInfoGpa欄位為`0x1166d7000`（大於4GiB），pingInfo欄仍0。這提出位址範圍的候選疑點，不宣稱已證明32-bit截斷或映射根因。下一個比較在同guest runtime將RAM由4GiB降為2GiB，其他設定回到原GLDirectMem自動選擇；先跑同V2 CPU fixture。降低RAM是同私人guest的資源比較，沒有換API／image、改產品守門或將編譯視為合格。

2GiB自有V2：PID3013預設TCG仍5 pass／2 fail，PID3143單指令TB7 pass／0 fail，皆正常power-off／host exit0。完整Android gdb3267／inferior3277仍在同`+0xd58f91` null read並SIGSEGV終止；本輪對應GPA為`0x54fd2000`，低於4GiB，故「僅因超過4GiB」的假設不成立。非逾時、supervisor未送訊號，ADB仍offline，未裝Root、研究App、遊戲或產品。後續改查host的實際位址映射callback，不繼續盲目降低RAM或替此結果補成功。

## 實際callback註冊與software-selection研究

同ELF的`get_address_space_device_control_ops`／relocations與tellPingInfo反組譯確認VM-ops `+0xb8`呼叫位置。只在自有V2 guest啟動時觀察公開setter及該函式實際引用的global寫入，沒有對guest呼叫或改寫callback。PID3777與顯式`-accel off`的4004均註冊主ELF`+0xa24080`：實際三條指令固定寫`found=false`、回傳null。註冊stack含`androidSnapshot_initialize`；因此這條映射路徑無法返回有效host pointer，單換RAM或GL選項沒有修好它。CPU fixture仍7項通過及正常power-off，與完整Android失敗分開。

同binary的實際switch另有`+0xcd6210`的TCG mapper；[固定QEMU來源](https://android.googlesource.com/platform/external/qemu/+/4f8bb7a074c4cd727acfdfa1e7bb092a2480696c/android-qemu2-glue/qemu-vm-operations-impl.cpp)顯示依host CPU detector選映射分支。此source作結構對照，不代替SDK完整build manifest。為同一軟體CPU研究程序加入[明確host helper](../research/tcg-software-selection.md)，透過公開函式符號選NONE，保留真實host probe／status初始化，限制marker／exe／AVD及software flag。只有本輪child env使用，不放全域或產品APK；不覆寫SDK、Android API／Root／遊戲回報，不稱為原廠無helper或實體ARM64驗收。

| 自有研究案例 | 實際結果 |
|---|---|
| 錯誤marker，inferior4232 | 仍選原`+0xa24080`失敗stub，未出現選擇成功notice；CPU7/7、正常power-off／inferior exit0 |
| 正確目標，inferior4375 | 實際選到`+0xcd6210`，會呼叫既有實體位址映射並在有pointer時設found；CPU7/7、正常power-off／inferior exit0 |
| 錯誤exe的自有Linux程序 | 原host detector enum1與加入helper後均為1，沒有NONE選擇notice |
| 缺原函式的自有Linux程序 | 實際exit86／明確失敗notice，沒有偽造原函式或成功 |
| 正確目標、預設TCG，inferior4535 | 仍5 pass／2 SIGSEGV fail，正常power-off／inferior exit0，未修好獨立的相鄰序列問題 |

以上成功只涉及host研究分支與自有CPU fixture。其後回到4GiB／2vCPU／原GL選擇、同官方Android ramdisk及單指令TB，才執行有helper的完整Android研究輪；普通App／Root及遊戲尚未驗，九組矩陣不升格。


## 有helper的完整Android：ADB可用，framework未完成

gdb4670／inferior4680於UTC `2026-11-09T22:31:20`啟動，約13分鐘時ADB首次online。實際讀取Android12／API31、`arm64-v8a`（abilist亦僅ARM64）、NativeBridge `0`、SELinux Enforcing；guest kernel `5.10.110-android12-9-00004-gb92ac325368e-ab8731800`／aarch64。Fingerprint為`google/sdk_gphone64_arm64/emulator64_arm64:12/SE1A.220630.001.A1/9056438:userdebug/dev-keys`。這是有明確host helper／單指令TB的ARM軟體guest平台證據，不是實體ARM64、原廠無helper或完整App就緒。

原廠shell起初UID2000。`adb root`回覆重啟後，第一次等待裝置曾逾時；沒有直接記成功。稍後重新讀取同boot ID `4a56b7c0-3c6a-4de8-b854-8f231a9e6df8`、實際UID0／API31／Enforcing，才進行fork-only自有原生fixture。這僅為原廠adbd Root；沒有安裝Magisk、改SELinux或驗普通App `su -M`，也未安裝研究APK、遊戲或透視鏡。

context／fixture／supervisor三檔先逐一核guest SHA與來源吻合，再單次執行既有`arm64_context_test`，未改100ms call、alarm或UNKNOWN規則。第一輪9個context模式有輸出；第二輪mode0雖返回值等於子PID，狀態仍UNKNOWN、未還原，測試實際exit1／`ARM64_CONTEXT_FAIL`，不冒充正常返回、不重送不確定call。後續supervised loader測項未執行；另一個supervisor binary只有推送與核SHA，沒有執行證據。全部動作只對自有fork子程序，沒有外部目標PID。

約20分鐘讀值仍為bootanim／zygote running、無system_server、無`sys.boot_completed`；窄範圍error logcat未見錯誤不代表完整系統健康。到30分鐘上限仍未完成，watchdog透過本輪已綁定pidfd送收容訊號，gdb最後exit1。**這是逾時收容，不是正常關機、context通過或產品成功。** 收容前尚未恢復adbd至UID2000；guest已退出，不能補寫unroot完成。下一輪必須重新核boot／UID，不沿用本輪Root身分。收尾後實際未見存活QEMU程序。

## V3診斷：保留fatal signal及重試效應

新增`research/build-tcg-memory-init.ps1 -FaultDiagnostics`只產生另一個自有PID1 initramfs；不替換V2或SDK。SIGINFO handler將PC／SP／X0等存入fork共享區，重送原訊號，由父程序wait/reap後列印。另有故意NULL read控制例，須同時取得address0、實際terminal SIGSEGV及child reaped，不計入七項CPU正例。

第一個診斷稿讓handler直接返回；兩個序列雖記到fault，重試後卻exit0、印出7 pass。此稿的成功計數**作廢**，不能替換V2失敗。修正版重送原訊號，並把任何已記錄fault強制列fail：PID9993實際5 pass／2 fail／0 skip，兩個terminal signal均11；NULL控制通過，正常power-off／host exit0。PID10136另只對自有序列地址開TCG dump，結果相同。PID10274以同修正版原件加單指令TB，7 pass／0 fail、NULL控制通過、正常power-off／exit0。沒有故意alarm測項。

修正版init SHA256 `ba06dd9f1b0230d0da750ed48a7114b9203bba7c4f12bc766c5c477e620a509c`；ramdisk `7e7b5fcb6e8ce7d5670f830ef3aa2d9762509735bbaf23994a9751367aa1d36f`。兩序列fault PC均`0x227f5c`，本fixture反組譯實際是STP Q0,Q0,[X0]；X0仍是自有合法mmap（或其top-byte-tagged形式），fault address卻為高位kernel樣式位址。TCG dump顯示guest X0暫存在host XMM0，且跨過一次可走slow path的stack store；這是待驗的clobber疑點，不宣稱已證明或修好根因，不對遊戲／產品繞過守門。


## 實際XMM clobber與scalar host比較

自有V3b的gdb10542／inferior10552，在只含自有序列的TCG dump完成後才對實際slow-store helper作entry／return觀察，沒有改guest。plain case的host XMM0由`0x7d81caf720`變成`0xffffff80ded5f001`；後者與接著STP的fault address完全相同。tagged case同樣觀察到helper覆寫。這是實際暫存器跨helper被改寫的證據，比最後一行log或靜態來源推論更具體；沒有把未驗的其他TCG路徑都宣稱已修好。

新增[scalar host研究腳本](../research/tcg-scalar-host.md)，只對同一自行啟動、固定SHA的QEMU子程序，在真實host feature probe後將兩個TCG AVX generator旗標設0，選既有scalar生成路徑。**這輪確實改變host程序的兩個data bytes**，不同於前面純觀察；不改SDK檔案、Android API／ARM指令／clock／Root／SELinux、遊戲或產品准入，不稱為原廠無修改資格。guard固定exe／SHA／ASLR後指令／唯一AVD／port／software flag與獨立marker，且不attach外部PID。

| 同guest／同官方kernel／2vCPU／4GiB | 實際結果 |
|---|---|
| 原V2 ramdisk、normal TB、scalar host，inferior10734 | 原件7 pass／0 fail／0 skip，正常power-off、inferior exit0，無收容訊號、無single-step |
| 最終V3c、normal TB、scalar host，inferior10902 | 7 pass／0 fail，加故意NULL-fatal控制通過，正常power-off／inferior exit0、無收容訊號 |
| 同V3c、移除scalar host、PID11053 | 仍5 pass／2實際SIGSEGV fail，NULL-fatal控制通過，正常power-off／exit0 |
| scalar script錯marker／錯exe | 均exit1／SCALAR_HOST_REFUSED，在starti前拒絕，沒有啟動guest |

V3c把no-core限制移到故意NULL控制之前；前稿NULL控制未先設定該限制，不能追溯宣稱它全程禁止core。最終init SHA256 `7d996e768c325a089ab77ea8ba7b70e56c684b2f7d1d24f15274e596bace4183`；ramdisk `9f00a9426f92fd22a6be1c9112f6d072bc88ec96e672eba3ca38a7778211b957`。預設未開診斷的build亦實際編譯通過，另存新原件，不覆寫舊V2或APK。以上僅CPU層；有兩項host研究改動的完整Android另驗。


## Normal TB／scalar host的完整Android與原生隔離層

同一AVD／原官方ramdisk／4GiB／2vCPU，以mapper helper加明確scalar host、**不使用single-step**啟動gdb11180／inferior11190。約6分鐘ADB online，實際API31／Android12／ARM64-only／NativeBridge0／同kernel及Enforcing；新boot `6d702f40-d28d-4903-ad7f-a730dd75f844`、初始UID2000。後續logcat確認zygote完成mainline classes驗證並建立system_server685、記錄進入SystemServer；不是只看到程序名或編譯成功。完整boot flag另讀，不由此直接算App就緒。

每次adbd Root都先讀取同boot／API／ABI／kernel／UID／Enforcing／fingerprint，重啟後再次核UID0才測試；不把「restarting」文字當成功。首次三個自有原生檔案逐檔guest SHA吻合，舊100ms context套件的27個case均有預期結果，但後續mode4真ELF載入在3s supervisor下UNKNOWN／target dead，整體exit1。另一舊supervisor套件於scenario3只有ATTACHED、尚無CALLING即100ms拒絕；target保持存活符合安全規則，卻未達該測試期待的CALLING後timeout，因此exit15，不能記10例通過。之後unroot並實際重核同boot UID2000。

另建立[獨立的產品界限研究套件](../research/arm64-product-budget.md)，保留上述失敗，不修改既有stress預期或產品時間限制。新targets只用自有fork，採產品同`-O2`及現有15s supervisor／2s call；suite總alarm另限90s／180s，不是單次loader界限。三檔在Linux及guest再次核SHA後，只執行一次；每一suite／child／attempt皆新建，不重送舊UNKNOWN。

| 新研究artifact | SHA256 |
|---|---|
| arm64_product_budget_test | `326f6e85bc04ee8c41573cf516e026475670daba3384d37ebbfe7e7a234e361b` |
| supervisor_product_budget_test | `d42f9687a53f57e3689a6bc085de7a4cf0fbe8c33e1947e07caea5c67ce571ae` |
| paired ELF fixture | `da8b046ae3b80ebfe18cea6dc48929c22cadda14c1797cdcb47192dc7c026151` |

產品界限ARM suite的正常function、spin／ppoll／SIGILL三個故障各有預期返回或UNKNOWN收容，但真正ELF load仍到整體界限、UNKNOWN且target dead，整體exit1；**沒有ELF正常載入／JNI通過**。獨立supervisor產品界限套件則實際10/10、exit0，包含非leader trace、CALLING前保留target及CALLING後timeout／worker exit。收容過程的中間log `reaped=0`不能單獨當最終失敗或成功，以最終result與各case的pidfd／expected檢查判定。

後續再次unroot並實際讀回同boot／UID2000／API31／ARM64／Enforcing。普通App Root、真ART/JNI及產品仍未驗，沒有安裝Magisk或APK。framework初次logcat有一次空輸出exit255，後次獨立讀取才取得上述SystemServer證據，未將前次當健康正例。本輪不增加產品版本或改0.5.0 bytes；後續將先研究保留host向量暫存器的路徑，不以無限放寬逾時解決效能問題。


## Scalar完整輪的framework失敗與正常收尾

後續實際`sys.boot_completed`仍空、bootanim running、該次`pidof system_server`沒有PID。另次logcat確認Watchdog685記錄`WATCHDOG KILLING SYSTEM PROCESS`，main stack停在PackageManagerService初始化的SELinuxMMAC.readInstallPolicy／XML FileReader，隨後SystemServer因signal9退出、zygote302跟著退出，新zygote865重新preload。這不是完整開機成功，也不能只憑較早「Entered SystemServer」當作framework健康。沒有停用／延長Android Watchdog或跳過安裝政策來使它過關。

本輪兩次Root測試後均已實際重核UID2000。於UTC `2026-11-09T23:41:47`前指定console正常kill回OK，inferior11190實際exit0、gdb exit0，supervisor `timedOut=false`／未送收容訊號；不是30分鐘逾時或強殺。SDK原件保留，沒有Magisk或App安裝。正常關閉後5586／5587短暫TIME_WAIT令下一輪bind拒絕；核對無存活QEMU／listener後等釋放，未kill-server、改埠或略過檢查。

## 保留host向量暫存器的替代研究

新增[MMU vector-save研究來源](../research/tcg-mmu-vector.md)，不關閉真實host AVX generator、不使用single-step。獨立Linux程序先驗weighted參數／整數回傳／SysV stack alignment、YMM0–15及MXCSR保存：PASS／exit0；未初始化入口實際SIGILL／exit-4，harness禁core。此測試只涉及自有Linux程序，不是Android或Root。

新host assembly helper沒有constructor或自動hook；只有有明確marker的gdb自行啟動同SHA／AVD／port／software CPU後，驗實際AVX旗標1/1不變、完整32-slot原表及wrapper範圍，才將本輪host MMU table的14個非null pointer指向保存暫存器的wrapper。這是**host程序資料記憶體的明確變更**，不是原廠無helper證據；SDK／guest檔案、API、ARM指令、clock、Watchdog、SELinux、Root及產品守門不變。原先mapper helper仍另列，不把兩項host研究改動隱藏成一般官方AVD。

| 實際自有測試 | 結果 |
|---|---|
| 原V2、normal TB、vector-save，inferior12132 | 7 pass／0 fail／0 skip，guest正常power-off、inferior exit0、無收容訊號 |
| 原V3c、normal TB、vector-save，inferior12273 | 7 pass／0 fail，故意NULL仍terminal SIGSEGV／address0／reaped；正常power-off、inferior exit0、無收容訊號 |
| controller錯marker／錯exe | 均exit1／VECTOR_SAVE_REFUSED，在starti前拒絕，沒有啟動guest |

helper17,712 bytes／SHA256 `823f6e1826a353ad38a91e7652ec46132c69c349bf6bcc28d1399c512323508c`；host ABI fixture15,416 bytes／`e2c60e7e01d5435cbd4106deeaf5c1373e906773fe96d851caa48fdaf9a28496`。以checked-in build腳本另存唯一目錄重建，兩者SHA相同。不是原廠TCG全面修復、任意host向量狀態或完整Android資格；接下來仍須另驗完整boot／原生loader／普通App及遊戲，九組不升格。
