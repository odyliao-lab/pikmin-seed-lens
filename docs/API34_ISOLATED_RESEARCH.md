# Android14/API34隔離研究（2026-11-09）

使用者新增Android14/API34後，確認目前沒有私人API34手機，並明確同意新官方x64 AVD及私人Magisk31.1 ramdisk Root研究。只限新`SeedLens_API34_x64`／emulator-5582（console5582／adb5583），2vCPU／4GB、資料`D:\SeedLensLab\avd`；API35驗收結束且正常關閉index5後才啟動，原四台與台中不操作。登入仍由本人完成，不複製任何session。

## 官方來源與stock boot

[Google SDK image清單](https://dl.google.com/android/repository/sys-img/google_apis/sys-img2-3.xml)列出`system-images;android-34;google_apis;x86_64` rev14，archive `x86_64-34_r14.zip`／1,563,721,130 bytes，metadata SHA1 `e0f6c9a0691aa27bd597d0deb1bcfdc943ac8ca7`。SDK manager安裝後，package.xml與source.properties均為rev14。原SDK ramdisk SHA256為`66b2074665da2aaa88229b2b495cc34ca6c7aa1db6b1c8992a306d3afddc5bbe`，未修改。

新空白AVD採Pixel2／1080×1920，僅設定2CPU／4096MiB及6GiB資料分割區，沒有複製既有VM。首次boot `64f764f9-df84-469a-823e-007adca3c4d7`，實際property API34、kernel x86_64、libndk_translation.so、SELinux Enforcing；等待sys.boot_completed=1才安裝研究App。此階段沒有Magisk、Zygisk、額外顯示模組或產品APK。

## 同一自有APK的實際指令測試

ARM-only hint probe新增0.0.3/code3，沿用研究簽章，Java與native都要求精確expected API；只增加API34研究准入，API32／不符參數仍拒絕。其三模式維持相同signal／alarm保護，不攔截SIGILL、不替換遊戲指令或bridge。產品API34 x64仍拒絕，沒有由此研究開放產品版本判斷。

| Artifact | SHA256 |
|---|---|
| hint0.0.3 APK | `eabde3ede42e569e0adf6e94bf4a36fa2cd54c5c3cebe0b22432fdafb02a4efd` |
| ARM64 JNI library | `ee9dcc91918107d1bae099f9b6807f68ecac488e8d4e0ab306b519103b29a7bf` |
| 本機實測stock bridge | `a2086fbc807f697f9b91222065c9d9000ca9727748699c0d9b751319258cbb3e` |

| 項目 | 實測 |
|---|---|
| expected_api=32／034 | 各自instrumentation1／IllegalArgumentException，未進JNI |
| baseline，包含相同signal／alarm設定 | 普通App UID10192／PID4154，native實際API34，durable RETURNED、instrumentation -1 |
| SEVL | 新PID5403，BEFORE後SIGILL、instrumentation0，未RETURNED |
| SEVL＋WFE | 新PID5457，BEFORE後SIGILL、instrumentation0，未RETURNED |

沒有因ADB exit0把crashed算通過。研究adbd root只用來讀自有receipt；不是普通App Root證據，讀完恢復UID2000。

另從本機實際bridge讀取可辨識的mode字串後，在同一guest暫時設定`ndk_translation.mode=interpret-only`，重跑自有三模式。baseline返回，兩個指令模式仍crashed；只觀察到property值及結果，沒有獨立證明引擎內部切換，也沒有據此推論遊戲可用。原property為空，實驗後已核對還原為空，未改ro屬性、API、SELinux或任何library bytes。

## 原遊戲啟動與軟體GPU比較（進行中）

stock boot正常安裝原簽章156.0/code1792142832完整base／ARM64 split；只從PM解析launcher啟動一次，PID6768。觀察到Scopely啟動畫面與「Process system isn't responding」ANR，沒有登入就緒或遊戲SEVL crash PC證據。已保存原logs及畫面，不能把自有probe的SIGILL冒稱遊戲同位置崩潰。

關閉全螢幕教學後選擇Wait，另保存系統負載；遊戲未做任何操作或產品載入。為隔離後續mode測試，正常force-stop的只限這個無登入的隔離遊戲。保留資料，adbd回non-root後正常關閉此AVD，確認原QEMU terminal才重開。

第二次使用同一AVD及官方emulator `-gpu swiftshader`軟體GLES／Vulkan模式，比較顯示服務ANR；2CPU／4GB、API／bridge及遊戲bytes不變。boot `7c7d1219-f44d-46c1-a894-bafc2fd78a85`，GLES回報Google SwiftShader。156.0首程序PID1729在Loading.AsyncRe／TID1822發生SIGILL，translator明確指出`0xd50320bf`；Unity PC為原始libunity.so+`0x599200`，對應已核156.0 ELF的SEVL。這次有遊戲自己的指令失敗證據；不再由probe失敗推論遊戲。

同次截圖工具停滯，核對其唯一PID1832／UID2000／start15647及screencap命令後，只停止自有診斷工具及其ADB client，未停止任何loader或改遊戲。該畫面不能當成功截圖。隨後156.1更新的自身PackageInstaller session1593143407（UID2000、180,611,291 bytes）在committed／未applied階段未返回；只取消此session，記錄mDestroyed=true，沒有重送安裝。正常關閉該失常圖形環境後，另次boot確認active sessions清空、原156.0仍在，才重新進行版本升級。

第三次仍為同一AVD，官方`-gpu host -feature -Vulkan`使用既有NVIDIA OpenGL驅動，沒有更新驅動或修改Hyper-V。boot `6ea3863d-b1c5-4e7c-b05f-f057b44894c5`，實際API34／原bridge SHA／Enforcing與空mode不變；GLES為NVIDIA RTX2060／OpenGL ES3.1。新的session769034203實際finalStatus1／applied=true／Session installed；PM精確156.1/code1793076318，base與split SHA逐一吻合Play intake。

156.1只啟動一次，PID4192／UID10193，在Loading thread／TID4356先出現undefined `0xd50320bf`與SIGILL，Unity PC亦為該版libunity.so+`0x599200`（BuildId110e530d888cf082）。此位置與已驗簽原split的反組譯一致；不能拿API31先前另一thread的`0x5e2234`冒充本次PC。後續pidof回4390而前景已是Launcher，沒有把另一程序當登入就緒，未再導引登入或產品載入。

原始資料只在ignored `.tools/lab/api34-stock-f222f40580ae48a8b3d8e72985e60f16/`、`.tools/lab/api34-swiftshader-5806d8ce52514b0282126c5d70cfe8d5/`及`.tools/lab/api34-hostgl-00e20dc07a554bd29c03055ef485d19e/`。結束前確認產品套件與product attempt不存在、adbd UID2000／Enforcing；正常關閉AVD。共享SDK ramdisk／bridge及遊戲指令未改。兩版原遊戲都已有產品載入前SEVL失敗證據，未安裝Magisk或產品，不把Root安裝或版本放寬當成CPU修復；兩個API34產品組合仍未通過。
