# API34普通App Root／loader前置研究

這是已獲准的既有SeedLens_API34_x64／5582的私人Magisk31.1 ramdisk研究範圍，
不是新AVD或新的操作目標。授權與原平台界線見[既有研究](API34_ISOLATED_RESEARCH.md)。
保持SDK原件、Enforcing、Zygisk OFF及無額外顯示模組，一次一台研究guest，原四台及台中
不操作。Root安裝不能修復SEVL/WFE，也不是產品／九組或普通App Root通過。

## 私人 override 與普通 App Root 實測（2026-11-11）

下列是後續實跑結果；後文「尚未啟動／安裝」僅描述各歷史 checkpoint。
同一獲准 AVD 單次以已核私人 ramdisk `4c45244a…1025` 啟動，未覆寫 SDK。
官方 Magisk31.1 管理器正常安裝後，接受一次標準 additional setup，由 guest 自行重啟；
未執行 avd_setup.sh、未重開 Windows、未另建 AVD。設定完成後的新 boot 為
`d2306435-faad-4cae-8691-0e8acce92f11`，同 system_server525/start1162/UID1000
的兩次完整服務與顯示讀值相隔114.817735秒。實讀 API34/x86_64、原
libndk_translation.so、Enforcing、adbd UID2000；官方 core31100、Zygisk=0、modules空。

普通研究 App0.0.10/UID10195 在此新 boot 的不帶 scope、附 load_pid 兩例仍在
Root query 前拒絕，未新增收據。首次明確 Root-only 查詢實際 su exit13/
Permission denied，沒有授權對話框，也沒有 policy，保留此失敗與 stock IOException。
沒有換入口重試：在原管理器的 Grant Superuser 頁核對唯一研究套件／UID與 APK SHA，
只選取 `dev.ody.seedlensrootprobe`；其後實讀 policy 唯一一筆為 UID10195，
沒有授權遊戲、signal probe、Shell 或全域規則。未直接寫入猜測的 policy SQL。

授權完成後以**另一個普通 App 程序** PID5403 執行新的、固定唯讀 Root-only 查詢：

| 實讀項目 | 結果 |
|---|---|
| App/API/ELF/封裝 | UID10195、API34、ELF machine62、x86_64 |
| Root 入口 | PATH `su -M`，exit0，無 debug_ramdisk fallback |
| Root 身分 | UID0/root、`u:r:magisk:s0` |
| self / PID1 mount namespace | 兩者皆 `mnt:[4026532358]` |
| Root boot ID | 與上方新 boot 完全一致 |
| App 及 instrumentation | durable PASS 與 code-1，兩者均核對 |

本次 PASS 收據 SHA256 為
`f66a53c300289508e8aa14565a2193aaad78c5233a1edbcd18956343047756db`。
結束前獨立重讀三份 Root 收據（stock失敗、未授權失敗、新程序成功）及原26份 signal
收據，逐份 SHA 保留；原157完整 base/split SHA不變，產品 APK與兩個 PM attempt目錄
仍不存在。所有 adbd 診斷均恢復 UID2000。

正常 console 關閉只發出一次；先前關機檢查時官方短暫的 `-kill … -sleep 20`
helper仍在，沒有重送關機或殺除其他程序。後續實讀 SDK guest/helper程序及5582/5583
listener皆消失，SDK四原映像與私人 ramdisk SHA不變，index5仍雙false；自有資料保留。

**這次通過僅為普通 App Root 能力。** 固定指令未呼叫 PM runner、JNI、NativeBridge
loader或遊戲。API34 PM backend仍拒絕，沒有放寬產品守門；既有三版原遊戲 SEVL
啟動失敗保留，API34三組產品、API31與森林手帳 UI階段仍未完成。研究 APK／管理器
不是新增的消費者產品依賴，產品仍為原0.5.2/code17且位元組未改。

其後已在另一新boot，以獨立新研究client／四參數adapter完成自有普通App PM／JNI、
timeout收容及新程序回歸，詳見[後續獨立證據](API34_PM_FLOW_RESEARCH.md)。上述
0.0.10本輪僅讀scope與原backend拒絕的界線保留，不把後續結果倒記本輪通過。

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

## Stock對照與私人patch輸出（2026-11-11）

冷備份後單次啟動同stock AVD，新boot2846db04-cb88-49f8-8e4e-247f87fa1c45／API34／
x86_64／libndk_translation／Enforcing／UID2000；同server516/start1500與兩次服務／display
讀值相隔87.123秒。正常安裝原lab簽章0.0.10後UID10195：不帶scope、帶scope卻附
load_pid兩例各在Root query前拒絕、無新Root receipt；明確scope的PATH su-M只得到
IOException，保存失敗收據，普通App Root未證實。原26份signal收據、原157完整SHA及
產品／attempt不存在狀態均保留，未以adbd Root代替App Root。

因此才用研究adbd Root單次執行已核官方流程，工作只在本次私有guest目錄及ramdisk
副本。取得私人ramdisk SHA4c45244a…1025，未覆寫共享SDK／API31映像；原Root query
失敗收據亦保留。診斷後adbd UID2000／Enforcing、同server，無Game、Root client或
Magisk daemon，管理器尚未安裝；正常關閉後host／5582、5583消失，SDK四原件SHA不變。

host獨立解析私有gzip／CPIO及stock串接archive，核31個stock最終entry與39個private
entry：除init，其餘stock mode／UID／GID／data不變；新增恰為八個官方backup／overlay
entry。init／magisk.xz／init-ld.xz逐bytes吻合官方APK，init.xz備份解壓吻合stock原init，
KEEPVERITY／KEEPFORCEENCRYPT皆true，PREINITDEVICE=metadata。首個parser對重複dev
空目錄及壓縮備份名的拒絕保留；修正為只接受一致空目錄、辨識串接CPIO與init.xz後，
v1／v2 audit分別保留，未重做patch或改image bytes。

本checkpoint私人override尚未啟動，管理器及Root未完成安裝，App Root／loader／產品
資格均未通過。後續只能以已驗私人image冷啟動同目標，另核Root設定與App實際行為。
