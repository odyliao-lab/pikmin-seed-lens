# API34 x64 共用後端與未簽名候選整合

2026-11-11；接續[獨立普通 App PM／JNI／收容實測](API34_PM_FLOW_RESEARCH.md)。
這是來源與封裝準備，不是 API34產品支援、三版遊戲驗收或新版交付。
原遊戲在 Lens載入前的 SEVL啟動障礙尚未解，原生ARM64與x64資格仍分開。

## 共用來源與明確候選

`app/bridge_platform_api34.h`僅將已實測的四參數adapter共用到候選dispatcher，沒有
另寫一套容易分歧的namespace程式。`PlatformForRuntime`在ART查詢前按精確runtime
API、完整SDK文字、bridge名稱與編譯開關選擇；選定後失敗即拒絕，不換API或trampoline。
API35五參數介面與API31／34四參數介面仍是不同型別。Root loader、x64 worker與
host共同使用同一runtime選擇，原遊戲version/code/digest/prologue/metadata檢查不改。

新增 `-Api34Candidate`只接受 `-Candidate -Unsigned`；沒有 signed API34 qualification
入口，也禁止與signed API31 qualification混用。API31與API34可各自選擇或一起編入
同一未簽名候選；checked-in Java兩旗標皆false，預設簽章路徑仍只保留既有x64/API35
資格。此來源共用不會讓研究probe／carrier／client成為使用者的安裝依賴。

| x64 tuple | 預設 | API31候選 | API34候選 | 兩候選同APK |
|---|---|---|---|---|
| API35/libnb.so | 原後端 | 原後端 | 原後端 | 原後端 |
| API31/libndk_translation.so | 拒絕 | 四參數31 | 拒絕 | 四參數31 |
| API34/libndk_translation.so | 拒絕 | 拒絕 | 四參數34 | 四參數34 |
| API32、交叉bridge、錯SDK文字、未知tuple | 拒絕 | 拒絕 | 拒絕 | 拒絕 |

Java副本只在每個唯一輸出目錄產生；native四種cache分開，建置每次明確傳兩個ON/OFF，
核對cache、實際root/host編譯旗標及封裝adapter marker，不能沿用上次候選設定。
兩ABI loader、同批ARM renderer、x64 host及其配對SHA仍封在單APK內。

## Host建置與封裝證據

四種Java實際變體各55項檢查、反向旗標拒絕；native四種編譯變體各111項policy
檢查通過，API32及大小寫／空白／Zygisk前綴bridge拒絕保留。四個真建置參數負例
在工具、簽章及輸出配置前拒絕，沒有將不合法模式降級成可簽署建置。

預設及兩候選同開兩次完整build均完成雙ABI loader／native tests／ARM renderer／
x64 host、Java/dex、installed-path／session／APK layout／paired digest檢查與16KiB
封裝。native測試的編譯與上方真正host執行分開，不聲稱都在Android執行。

| 唯一目錄內的新unsigned候選，禁止安裝／交付 | SHA256 |
|---|---|
| 預設API31=OFF／API34=OFF | `650b982e66368bbd8cd5fa96172ed5a853aad61992e025b7dc6123941f84dafa` |
| API31=ON／API34=ON | `69454c8c02899cc8d04c388af4ebb4d8c087e248e7b3aeb11442082c4d847ffc` |

獨立核對真正App classes的兩個constant、root/host的Ninja編譯定義、CMake cache、
APK內adapter marker、兩loader資產SHA及bridge／同批renderer配對，皆與所選模式一致。
兩份APK皆無簽章，未安裝、未交付；原簽章0.5.2/code17 SHA9457ab23…1b17原件保留。
產品manifest未升版，不能將候選bytes冒充同版本交付件。

## 共用dispatcher的研究件（尚待本批裝置實測）

為驗證新選擇器而非只重用舊結果，API34 carrier改用和候選相同的
`PlatformForRuntime`，明確編入API31／34兩候選，仍只接受自有probe0.0.7。
新artifact使用更高版本及原lab簽章，舊carrier0.0.10／client0.0.12與30份歷史不覆寫。

| 本批研究件 | SHA256 |
|---|---|
| carrier0.0.11/code11 | `db3397c6a68b46f8be3e18890ca0badfeee920a0af34dec7c31fc0eccb9c93e0` |
| 共用dispatcher host | `b732fe2481c535fa182bddef8edb967624dbd4574f8f07128b675733ee4dd794` |
| PM App runner | `b0e4550237808e584390b51944d9e63f12be82c969abc049a89fca43420675ef` |
| client0.0.13/code13 | `ab22e37f7d6079966384a871d9fe5af0db869ff15bdadd3e4a96938a4de3285d` |

本批已核原lab signer633…8870、stored x64 ELF64／PIE、16KiB對齊及runner SHA資產。
API31／35研究host及PM runner另在本批唯一目錄編譯回歸通過，並未當作新裝置執行。
還須以新boot／原研究歷史SHA為基準，實跑正常、timeout／UNKNOWN及新程序回歸。
本checkpoint不承接上一批直接adapter的JNI資格，亦不把本批編譯／安裝當產品通過。
