# API35隔離機Google套件（2026-11-09）

使用者明確授權直接安裝必要Google套件。操作僅限新建MuMu index5 `SeedLens-Isolated-A15`（created timestamp `1794181954477010`），不操作原四台或台中裝置、不複製任何帳號／scanner／憑證。Google與遊戲登入仍由使用者完成。

## 來源與實際安裝

CLI沒有Google安裝指令；先用Windows電腦操作技能核對精確隔離視窗，再在Android原廠桌面「小工具」資料夾找到「谷歌安裝器」。點擊後由`server-file-mumu-app-store`安裝`com.nemu.googleinstaller`，versionName12／code32；使用其一鍵下載與重啟桌面流程。不是搜尋結果中的第三方安裝器，也未複製其他instance的Google資料。

安裝器的標準整包另包含Play遊戲、聯絡人、聯絡人同步、TTS與TalkBack；未為這些App登入、匯入資料或另外授權。三個核心套件實際PM路徑在`/data/gms11/`。此處是MuMu原廠安裝來源與本機簽章驗證紀錄，不以憑證DN文字單獨宣稱Google官方來源驗證。

| 核心套件 | 實際versionName／code | 安裝時base APK SHA256 |
|---|---|---|
| com.android.vending | 52.8.55-34 [0] [PR] 967533532／85285540 | `9ffc6e69d4c3b9b0a89808c4387f9492d168f912754e7a27b029e5519e8bc142` |
| com.google.android.gms | 26.32.34 (260800-968093310)／263234038 | `870258fe35cfc449d4eaf5ae52cf830cc176cf099dafcc3550bfdfb13a5c15a6` |
| com.google.android.gsf | 15-12154782／35 | `748a7b1eb1edf2ca7928fe9f1e1f69df16f913755da7b657e62f38a55f0bbff7` |

三個base均以SDK35 apksigner verify成功。Play／GSF signer SHA256為`7ce83c1b71f3d572fed04c8d40c5cb10ff75e6d87d9df6fbd53f0468c2905053`，GMS為`5f2391277b1dbd489000467e4c2fa6af802430080457dce2f618992e9dfb5402`；共同source-stamp cert SHA256為`3257d599a49d2c961a471ca9843f59d341a405884583fc087df4237b733bbd6d`。這些是實測值，後續更新須另記新bytes。

重啟桌面流程短暫斷開adbd，重新核CLI與guest後boot ID仍為`994efc10-e992-463c-9e3b-44c3c82908bb`，實際API35／x86_64／libnb.so／原Permissive不變。普通App Root probe再次自行呼叫su-M，回報normal-app-su-M-pass、instrumentation -1；此時adbd仍UID2000，不是ADB代替App執行Root。

Play商店已實際啟動到「登入」頁，已通知使用者自行登入；尚未取得或安裝遊戲。安裝成功及商店登入頁都不代表Google登入、Play下載、指定遊戲版本或四組遊戲驗收成功。若商店提供其他版本，不能取代156.0／156.1。

原始套件、metadata與收據只留ignored `.tools/lab/api35-google-intake-4622f4522ead4effb9f933266f93eb17/`；不提交APK、原始診斷或截圖。產品原簽章與兩遊戲精確版本材料仍待取得。
