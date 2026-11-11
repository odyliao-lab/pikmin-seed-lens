# API34／31下一步研究界線

2026-11-11竹北盤點。API34仍優先於API31，平台與三個精確遊戲的產品資格皆未完成，
森林手帳UI尚未進入實作階段。兩輪AOSP scalar Android失敗已正常關閉；不重複無新假設的
TCG配置輪，也不以自有CPU通過替代完整Android、普通App與遊戲效果。

## 官方映像目錄重新核對

UTC2026-11-10 17:13僅下載兩份官方XML metadata，沒有下載image、修改SDK或建立AVD。

| 目錄中的x64映像 | stable revision | archive bytes |
|---|---:|---:|
| API34 Google APIs | 14 | 1,563,721,130 |
| API31 Google APIs | 14 | 1,470,822,277 |
| API34 Google Play | 14 | 1,510,752,654 |
| API31 Google Play | 9 | 1,423,631,458 |

來源：[Google APIs](https://dl.google.com/android/repository/sys-img/google_apis/sys-img2-3.xml)、
[Google Play](https://dl.google.com/android/repository/sys-img/google_apis_playstore/sys-img2-3.xml)。
本次XML SHA256分別為
`f46a7c36aba029eba520189f731901a95c8f423bc446d16c326bf351efd8855e`、
`e26d8e04b593ab4d085e80e7905261343e73ff958e6041a4e0ba5b9982dc0865`。

Google APIs的31／34仍為已研究的revision，未取得更新image可修SEVL的證據。
不同tag的revision不能直接比較，沒有讀取Play image內的bridge，也不能因同為rev14
就宣稱bridge相同或不同。此目錄查核不授予任何平台資格；暫不以猜測為由新增AVD。

官方[emulator release notes](https://developer.android.com/studio/releases/emulator)
將新QEMU11+ engine預覽限定API37以上Phone AVD；不拿它代替API31／34驗收。
同頁ARM64 Linux host的KVM路徑需要ARM64 host，目前使用者沒有可提供的該硬體且已決定
先不使用雲端，不延伸為新裝置／付費資源授權。
[ATD文件](https://developer.android.com/studio/test/managed-devices#use-atds)
列出的效能縮減包含停用硬體rendering；ATD套件存在不是正常遊戲兩頁顯示可用的證據，
本輪沒有建立ATD來取代原目標。

## 獨立App訊號觀察

新增[ARM signal probe](../research/arm-signal-probe/README.md)只回答一個能力問題：
既有translator遇到自有ARM指令SIGILL時，是否向該App自己的handler交付可核對的
ARM64 PC、signal address與兩個暫存器marker。原hint probe繼續保留不攔截訊號的基準。

先在既有新API35隔離機做同APK的正常／synthetic signal／UDF控制，再在已核准的既有
API34 x64 AVD研究，最後API31各自另驗。每次先完成前輪OFF／關閉及新鮮裝置身分核對，
一次只運行一台研究guest。實測前的簽章與host檢查不當普通App或跨平台能力通過。

probe不Root、不存取遊戲、不改PC／context／指令或translator。fault record後自身exit86
只表示取得觀察，不表示原指令正常返回；中途／不吻合資料留為未證實。產品仍0.5.2，
精確profile、NativeBridge准入、主畫面／同程序確認、持久UNKNOWN與15s／2s界限皆不改。
即使自有handler得到吻合context，也未證明能安全處理遊戲啟動前的缺口，不能直接升格為
產品修復或新增遊戲啟動前注入流程。

API31套件版本另外核對：本機package.xml與官方catalog的revision均14，但官方r14 ZIP
與本機source.properties都寫12，兩個properties／build.prop檔的SHA分別相同。僅讀
3,322 bytes的官方archive中繼資料，沒有下載新映像或建立環境；不能以12欄位推論另有
較新translator可用。見[核對範圍](ARM_SIGNAL_CONTEXT_RESEARCH.md)；非完整archive
digest／新指令能力或產品資格。
