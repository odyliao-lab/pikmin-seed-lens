# API34普通App Root／loader前置研究

這是已獲准的既有SeedLens_API34_x64／5582的私人Magisk31.1 ramdisk研究範圍，
不是新AVD或新的操作目標。授權與原平台界線見[既有研究](API34_ISOLATED_RESEARCH.md)。
保持SDK原件、Enforcing、Zygisk OFF及無額外顯示模組，一次一台研究guest，原四台及台中
不操作。Root安裝不能修復SEVL/WFE，也不是產品／九組或普通App Root通過。

## Host準備與冷備份（2026-11-11）

最後0.0.4自有訊號研究已正常關閉API31／34及index5；新一輪host準備再次核SDK程序及
5582/5583 listener皆無、index5雙false後，才冷備份這台自有研究AVD的11個設定／資料/
encryptionkey／cache檔。總3,453,821,393 bytes，各來源／備份SHA相符，前後無guest，
沒有複製其他VM、正式掃描器或其憑證。私人還原材料只在repo外SeedLensLab/root-prep，
不提交磁碟、APK、raw logs或秘密。

沿用已驗[官方Magisk31.1原件](https://github.com/topjohnwu/Magisk/releases/tag/v31.1)：
APK SHA89cefac0…ac6e7。另直接讀取同tag的
[官方avd_patch.sh](https://github.com/topjohnwu/Magisk/blob/v31.1/scripts/avd_patch.sh)，
完整bytes與原保存版本相同，SHA ded1e6c3…877e。從該APK抽出的五個x64原始binary均
核ELF64／machine62及逐檔SHA；原SDK ramdisk66b20746…5bbe僅複製至私人工作目錄。

私人script只替換本次guest目錄並加入fail-fast、固定basename、API34／UID0／本次boot
檢查；官方KEEPVERITY／KEEPFORCEENCRYPT、cpio／backup流程不改。未執行會stop zygote
或改boot property的avd_setup live setup script。此checkpoint尚無guest push／patch/
Root安裝或產品操作；準備成功不等於Root成功。

## 僅讀Root client0.0.10：source／host檢查

研究client新增明確api34_root_only scope，僅普通App呼叫固定su-M讀取UID／domain、
self與PID1 mount namespace及本次boot。除精確API34、x64／packaged ABI、ELF62、
64-bit／uname檢查外，任何load_pid key或未知scope均在Root query前拒絕。既有
ProbePlatform／native PM runner仍拒絕API34；不改產品NativeBridge准入。

原lab簽章新0.0.10/code10 APK SHA5010db60…5ac2，94 ELF／77原platform／38 Root-only
scope與拒絕檢查、Java／dex、stored x64 PIE／ABI／digest與16KiB封裝通過。原x64 runner
b22877ed…ce84 bytes重用，但新scope不執行它。詳見[client介面與界線](../research/root-probe/README.md)。
本checkpoint尚未安裝或執行0.0.10，普通App Root、PM/JNI、任何遊戲資格均未因此通過。

後續須在已核新guest身分下先取得及核對私人patch輸出、原件保留、正常關閉後才改用
私人override；Root配置完成後另核Enforcing／原bridge／Zygisk OFF／空modules及普通App
實際su-M。完整PM loader、負例／timeout與新程序回歸需要獨立能力驗證，不由本scope授予。
