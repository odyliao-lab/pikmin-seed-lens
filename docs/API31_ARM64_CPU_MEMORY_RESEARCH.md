# API31 ARM64後端：隔離initramfs記憶體指令研究

這是已核准空白`SeedLens_API31_arm64`／5584的完整ARM guest軟體CPU研究，**不是Android App／Root／產品資格**，也不是x64/NativeBridge通過。新index5已OFF並正常關閉才執行；原四台與台中設備不操作。沿用官方API31 rev11 kernel、同2vCPU／4GB／virt主板，私人自有ramdisk以runtime override指定，沒有修改SDK映像、停用keystore2或修改遊戲指令。

## 從既有crash縮小位置

原Android init的keystore2重複SIGSEGV位於`libpuresoftkeymasterdevice.so+0xc438`。SDK system.img中找到GNU Build ID `173cb4246d5421fcc58a6a333ab84b1e`相符的ELF，離線擷取連續區段作候選（77,152 bytes／SHA256 `a6bf7aecbe8b682623baba9527c1e26a8c81de614341c8bf59938230fb3fbe2d`）；ELF notes、symbols與反組譯相符，不宣稱完成整個檔案系統的檔案／extent驗證。

該位址為`STP Q0,Q0,[X0]`，前方依序有兩個`DUP`、`MOVI V0.2D,#0`、向量`ADD`及stack `STR`。原crash的X0含top-byte tag，但fault address與X0不同；不能僅由此認定tag、allocator或TCG何者為根因。[固定AOSP歷史來源](https://android.googlesource.com/platform/system/keymaster/+/3c44456c54c4f111ca6a301cb50bac8a07bd13d0/contexts/pure_soft_remote_provisioning_context.cpp)亦有兩個vector配置；這是來源對照，不是SDK Build ID對應的完整建置manifest證明。

## 自有probe與實際結果

`research/tcg_memory_init.cpp`只允許PID1，使用自有匿名mmap及guard pages；每個case在獨立fork子程序執行，有5秒guest alarm、禁止core dump，父程序wait/reap並繼續。沒有Android partition mount、網路、遊戲／帳號、ptrace或產品載入。全部case後呼叫guest power-off；initramfs builder使用唯一私人輸出，不覆寫舊產物。

建置：`pwsh -NoProfile -File research/build-tcg-memory-init.ps1 -NdkPath <ndk-r27d>`。NDK API31 ARM64 static ELF只是建置標的；真正執行的是下面的kernel／CPU fixture，不把target triple當Android API實測。

| 實際執行 | 結果及限制 |
|---|---|
| V1，host22052 | kernel實際`5.10.110-android12-9-00004-gb92ac325368e-ab8731800`／aarch64；SEVL、plain STP Q、tagged STRB、tagged STR Q、tagged STP Q共5項實跑通過。單獨STP／tagged store未重現原問題；正常power-off，host與兩埠均消失 |
| V2，host25696 | 同kernel／AVD／CPU設定；前5項仍通過，新增plain與tagged SDK相鄰序列兩項均SIGSEGV，由父程序收容、繼續並正常power-off。實際總5 pass／2 fail／0 skip，不寫成7項通過 |
| 單指令TB比較，host23716 | 同V2 init／ramdisk原件，只新增`-singlestep`；7項實跑全部通過、0 fail／0 skip，正常power-off，host與兩埠消失。這是對照正例，不等於完整Android修復 |

V1 init SHA256 `deaf940b58dbb1b2342cfda7b75580cab8ef018e0f01d07984129a13c62876fa`；ramdisk `a758fda51ccda309da49eb27d23b6c3a3b9311bdde8153dabfad77813766b945`。V2 init `9e42bb6a3c852f222aa777d66178e68f5a78fce22be4c4f606853bd046b6ecc1`；ramdisk `6233a95b1f17476d16c7aa38adea601093bdf613edb1172de5902c04af54c00c`。反組譯另核V2包含欲比較的SDK指令序列；沒有修改官方ELF或產品0.5.0。

## 官方Android ramdisk的後續比較

同一研究AVD恢復官方原ramdisk、只新增`-singlestep`，host8212的實際kernel／init已越過先前反覆GenerateBcc SIGSEGV的位置，掛載／初次加密data後啟動zygote。仍未取得online ADB／實際API／App證據。2026-11-09 18:17:12 hostGL輸出停於guest約365秒的vendor.usb-hal啟動附近，之後host與5584／5585消失；沒有正常關機ack或退出碼。**這是未釐清的host退出，不算正常收尾或Android開機成功**，也不認定最後一行就是根因。

其後同目標另以SwiftShader比較GPU，保留單指令TB及原Android ramdisk；增加私人背景supervisor持有本次host handle。host24888於2026-11-09 18:23:34退出，實際exit code `-1073741819`／Windows `0xC0000005`；仍未取得online ADB、實際API或App證據。不是正常關閉，不能把最後一行vendor.usb-hal當根因。host／supervisor及5584／5585均已消失，SDK原ramdisk SHA不變；新index5亦保持OFF／關閉，未操作原四台。

本次既有本機minidump的ProcessId實際24888，exception code同為`0xC0000005`，exception address位於`qemu-system-aarch64-headless.exe+0x659e84`，exception資訊為read／address0。SDK PE無COFF symbols；該位址反組譯為`MOV RAX,[RSI]`，不能用距離很遠的最近export label當函式名。這是有限的例外位置證據，沒有完整symbolicated stack或根因結論；沒有上傳dump或啟用crashreport同意。未安裝Root、研究App、遊戲或產品。

目前自有fixture可重現「相鄰向量序列失敗／單獨store通過」，並在單指令TB模式取得同原件7項正例；這支持進一步測TCG區塊相關問題，但不是完整Android keystore根因證明，也未解決完整guest的host退出。Android完整開機、實際API、App／Root、JNI／context／失敗收容及原遊戲仍須逐層另驗；九組矩陣不升格。後續使用者已同意[Linux host隔離方案](API31_LINUX_HOST_PROPOSAL.md)，本人重開後已實際建立WSL2 distro並跑同CPU原件，預設仍5 pass／2 fail，單指令TB7 pass；見[Linux實跑](API31_LINUX_HOST_RESEARCH.md)。Windows私人輸出在ignored `.tools/lab/api31-arm64-0bc610f9cca14c298aa7b0a2f4c2890b/`的`cpu-probe-*`與`virt-singlestep-*`；Git不含ramdisk、SDK ELF或原始診斷。
