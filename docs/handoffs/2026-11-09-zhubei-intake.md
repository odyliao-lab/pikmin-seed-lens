# 竹北接手第一階段（2026-11-09）

## 來源與現況

- 在原本空白的 `D:\AI_Workspace\PikminLens` 取得指定私人 repo；沒有既有修改被移走或覆寫。
- `origin` fetch/push 都是 `https://github.com/odyliao-lab/pikmin-seed-lens.git`；GitHub metadata 確認 private。
- 交接 branch `codex/seed-lens-031-zhubei-handoff`，HEAD `60ba00d1ffc0e92b3b56b3eecaa4b7759364a29b`；乾淨取得後建立 `codex/zhubei-android12-compat`。
- Recursive pins：AndKittyInjector `9ece8bc0f05344ab23e882a38728ab91f70bfed0`；KittyMemoryEx `be4cdc9a79e3e1b93357e8d30608e84a3518f5d3`。兩者保持乾淨。
- 已讀完整 AGENTS、README、ARCHITECTURE、VALIDATION、原交接與 ZHUBEI_PROMPTS，另讀 GAME_PROFILES、ARM64_LOADER_SAFETY、COMPATIBILITY_RESEARCH、PROGRESS_2026-11-08。

## 本輪離線檢查

本機顯式工具：PowerShell7、Python3、JDK17.0.20.1+1、SDK35/build-tools35.0.0、NDK27.3.13750724、CMake3.22.1/Ninja；位於本機 LocalAppData/Android。沒有從其他專案取得簽章或 scanner 資料。

在交接 HEAD 完成全新 candidate 建置，exit0：Python profile 9、Compatibility 26、InstalledNative 29、GameSession 27、APK layout 8、paired digest 4；雙ABI native、Java/dex及16KB封裝通過。
Unsigned candidate SHA256 `5cde60aec6330b8cce99ddf64373d2dc6093956ba997b3883be55934db622c49`。
產物在忽略的 `.tools/candidates/`，不供安裝／交付。Native 測試執行檔只有編譯，未在裝置執行。

## 竹北本機資源盤點（host 唯讀）

- Intel i7-10700KF，8C/16T，約32GB RAM；D槽約800GiB可用。
- MuMu6.8.8.0；host CLI顯示四個既有Android引擎標籤為15.0的運行instance：index0 Capricorn、1 AutoFarm、2 Sagittarius、3 Aquarius。這些標籤不是新鮮Android API量測；本輪沒有連線它們或操作遊戲。
- 本機已有官方安裝內附的12.0 engine與空白base映像，CLI支援新建指定index及engine版本。
- 唯讀掃描12.0 base VDI找到31與32的多組SDK property字串，且ABI字串混合；不能據此認定啟動後是API31或某ABI。
- 本機未發現已建立的Android SDK AVD；SDK目錄沒有emulator/system-images。原簽章與合法遊戲完整splits尚未取得使用者指定來源。

## 已確認的具體隔離方案

使用者已在聊天明確同意本方案的新建、啟動及隔離能力測試範圍。index4已在新鮮確認空缺後由空白12.0 base建立並命名，設定2vCPU/4GB RAM；保留四個既有instance。

1. 用本機安裝內附12.0空白base，新建一台 `SeedLens-Isolated-A12`，預定index4，2vCPU/4GB RAM；執行前重查index空缺，不覆寫或clone任一既有instance。
2. 新instance端點由建立後CLI的loopback ADB host/port現查，不能猜測或沿用歷史端點。先核index/name/engine/端點，再以顯式serial查bootID、實際API、ABI、kernel、bridge、SELinux與Root。
3. 必須 `ro.build.version.sdk=31`，並與native runtime API一致。若是API32，記為不符合必要目標，停止API31路線、另提真正API31來源；不修改property冒充。
4. Root限新instance既有能力，檢查 `su -M`、mount view、PM路徑／owner；能力不足先記錄。此確認不包含新增Magisk、LSPosed、顯示模組或變更SELinux。
5. 只有fresh identity與確認範圍一致才進自有probe及fault fixtures；ARM64原生kernel與x64/NativeBridge分開。新instance若是x64，不能拿translated ARM App當native ARM64通過證據。
6. 研究APK需明確簽章來源；沒有來源先停在建置。正常遊戲安裝需合法原簽章完整splits，登入由使用者完成；產品不同交付bytes另外分配新版本/code及保留原簽章。候選APK不能安裝。

台中Libra/Virgo不連線；既有四台不改設定、不停機、不重啟、不複製scanner、token、Google session或正式帳號資料；不全域kill-server/disconnect。先跑一台隔離環境；資源不足時另提方案，不自行關閉既有工作。

## API31研究工具來源變更

原ARM64 JNI工具傳固定34，x64 canary傳固定35，會把實際API31當錯平台參數。研究loader改為明確的expected API參數，與`android_get_device_api_level()`精確相符才進程序查核；實際API傳入原後端。
ARM64研究僅31/34，x64研究僅31/35；拒絕32、其他API、格式錯誤及不相符值。31是研究准入，**未宣稱能力已合格**。產品版本／Java/native x64守門、遊戲profile、durable ACK、UNKNOWN與重試規則不變。

22項compile-time assertion涵蓋API32、跨API／ABI、未知API、查詢失敗、格式錯誤與正例；測試與兩個研究loader的建置結果在VALIDATION記錄。沒有裝置fault/JNI證據前不整合API31產品後端。

## 新MuMu實測結果與API31替代方案

使用者確認後實際建立、啟動index4。host CLI現查端點`127.0.0.1:16512`，核對名稱/index後才連線該serial。OS回報API32、release12、x86_64、libnb.so；kernel`5.4.32-perf-gda349bfae95e`、原SELinux Permissive、Root設定false且shell無su路徑。沒有安裝probe或遊戲、沒有開Root或改SELinux。因API不符停止並正常關閉新instance，CLI確認程序已停止；不刪除它。原四台與台中裝置沒有Android操作。

下一方案已在聊天取得明確確認並執行：Google SDK官方`emulator`及`system-images;android-31;google_apis;x86_64` rev14，空白AVD `SeedLens_API31_x64`，資料`D:\SeedLensLab\avd`，2vCPU/4GB；console5580/ADB5581、serial`emulator-5580`，執行前重查未占用及AVD身份。emulator版本37.2.13，WHPX實測可用；沒有安裝driver或重啟host。

這是x64/API31平台研究方案，不保證提供NativeBridge或產品的`su -M`。Google官方有[ARM轉譯的emulator研究說明](https://android-developers.googleblog.com/2020/03/run-arm-apps-on-android-emulator.html)，但不能把Android11的說明當新API31映像實測；[AVD官方說明](https://developer.android.com/studio/run/managing-avds)的adb root亦不等於產品Root契約。先新鮮核對實際API、原廠Root、mapped ART／native loader／bridge與失敗收容，再決定能驗哪些層；原生ARM64/API31仍需另外指定實際環境。

本輪實際API31／Enforcing與原生安全fixtures已完成；同時確認su -M拒絕、兩個新adapter所需export缺失。已開發獨立API31研究host，但真App JNI仍待研究簽章材料或明確研究key授權；未安裝APK、未登入。結束已恢復non-root adbd並正常關閉新AVD，保留全部fixture與資料。完整範圍及收據SHA見[API31研究](../API31_AVD_RESEARCH.md)。

## 下一階段驗收順序

取得具體目標確認 → 新鮮環境身份與API31 → 自有CPU／syscall／supervisor故障測試 → 真JNI／ART或NativeBridge ClassLoader → 新程序重跑／逾時收容 → 審查後端整合 → 新版本原簽章單APK → 正常UI兩頁名稱、縮字、捲動重用、OFF還原、遊戲重開重新同意。

每個API/ABI/Root/SELinux/精確遊戲版/code/APK SHA組合獨立記錄；OS重啟與長時間測試另列。156.0強制更新時不繞過，選可合法登入的第二版再新增完整profile。未知／未測保留原樣，沒有把本輪接手當成相容完成。
