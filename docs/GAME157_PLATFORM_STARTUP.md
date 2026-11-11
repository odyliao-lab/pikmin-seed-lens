# 157.0原遊戲的API31／34獨立啟動驗證

## 2026-11-11：Android12／157再次冷開機實測

使用者要求重新驗證 Android12＋157。先核對新 index5 的同 boot／遊戲身分，
確認0.7.0為OFF、47筆歷史保留，正常關閉 index5；只啟動既有授權的
`SeedLens_API31_x64`，未操作原四台或台中設備。

實際API31／x86_64／`libndk_translation.so`／Enforcing，boot
`3b901604-035f-4768-a910-ca7cd9155bbe`；system_server 525/start2188的完整
服務讀值相隔71.872788秒。官方SDK四個映像與既有私人Magisk31.1 ramdisk SHA
均吻合原pin，沒有換bridge、模組或放寬守門。

PM確認157.0/code1793360009，已安裝base與完整ARM64 split的SHA均與
[原簽章intake](GAME157_OFFICIAL_INTAKE.md)一致。正常launcher僅啟動一次，
遊戲PID4706／UID10149；本次`Loading.AsyncRe` TID4847在
`libunity.so+0x59972c`遇到undefined instruction `0xd50320bf`（SEVL），
其後SIGILL、程序結束並返回Launcher。Unity原件SHA仍為
`534fe2d1f8cadb79e6109ba63cf8f869b32599f915911447e19e38cf34b02149`。
本次位置與上次Preload不同，分別保留，沒有把舊crash當成本輪證據。

**結果：這台API31 x64環境仍無法正常運行原遊戲157，因此透視鏡功能未通過。**
本次未安裝／載入0.7.0，保留原0.4.1；不把來源守門檢查、安裝或Root研究算成
0.7.0裝置資格。普通shell的`su -M`本輪回傳Permission denied；後續僅以
研究用adb root讀取歷史及Unity雜湊，讀完恢復UID2000，不能算普通產品App Root通過。
16筆原研究attempt逐份SHA不變，沒有新增產品載入或重送UNKNOWN。
結束時正常關閉該AVD，host emulator／QEMU及5580／5581 listeners均消失；
四個SDK原映像及私人ramdisk SHA再次核對不變，index5維持正常關閉。

此結論只涵蓋上述x64轉譯環境；原生ARM64／API31仍未驗證，不泛化為所有
Android12裝置失敗。原始日誌與截圖只保留ignored私人目錄。

157.0/code1793360009材料取得後，不從156.x的SEVL失敗推論新版結果。完成新index5／0.5.0的兩頁分項回歸、正常OFF／adbd UID2000後先關閉該機，再依序啟動已獲准的獨立AVD；每次重核AVD名稱／boot／實際API／bridge與原簽章完整splits。沒有修改遊戲、替換bridge、跳過指令或放寬產品資格。

## API31 x64／libndk_translation

同`SeedLens_API31_x64`，2CPU／4GB／host OpenGL／Vulkan OFF，沿用已核SHA的私人Magisk31.1 ramdisk override。實際API31／x86_64／libndk_translation／Enforcing，boot `ff05d118-0657-4f9d-b096-fe516845e782`。原產品0.4.1保留，不啟用；沒有以adb root當普通產品App Root通過。

原遊戲從156.1正常更新157.0，PM精確code、base與完整ARM64 split SHA均與[合法intake](GAME157_OFFICIAL_INTAKE.md)一致。第一遊戲程序PID4254／UID10149，正常launcher啟動；只關閉Android全螢幕說明，不填登入或生日。

實際`Loading.Preload`／TID4440，translator回報undefined instruction `0xd50320bf`；Unity crash PC為原`libunity.so+0x5e2760`、SIGILL。該精確Unity原件SHA為`534fe2d1f8cadb79e6109ba63cf8f869b32599f915911447e19e38cf34b02149`，NDK llvm-objdump同位址確認SEVL、下一指令WFE。未到登入，不能算157或產品可用。

既有16份研究attempt逐份SHA不變，產品attempt仍不存在；沒有重送UNKNOWN。adbd恢復non-root後指定已核AVD的console正常`emu kill`，host程序消失。SDK及遊戲原件不修改。

## API34 x64

同`SeedLens_API34_x64`的原廠image／host OpenGL／Vulkan OFF；原SDK ramdisk SHA再次吻合既有紀錄。沒有安裝Magisk或產品，x64 API34產品仍拒絕。第一次host OpenGL冷開機的ADB／console無回應，正常關閉逾時後僅收容已核對的隔離host；該次未更新或啟動157，另段保留。

同一AVD改用已研究的SwiftShader正常開機，實際API34／x86_64／libndk_translation／Enforcing，boot `c8affa31-fa7a-442f-840e-8343ee7686bb`。先核PM無active session、原156.1仍完整，才正常更新157.0；base／split安裝後SHA均與intake一致，UID10193。曾出現System UI ANR，只選Wait，沒有以此代替遊戲指令失敗。

首遊戲PID4305／Loading.Preload TID4518，隨後translator實際回報undefined `0xd50320bf`，原Unity+`0x5e2760` SIGILL；與API31相同原件同位址SEVL逐一核對。沒有登入／產品載入，沒有Root／產品安裝。結束API34／Enforcing／adbd UID2000、遊戲已因signal退出；指定已核AVD的正常emu kill成功，host與5582／5583 listeners消失，SDK ramdisk SHA不變。這是獨立失敗證據，不宣稱相容。

## 邊界

本文件記的是**產品載入前**的原遊戲／平台能力。API35實際兩頁證據不能解除API31／34失敗，API31與34也不能相互推論。ARM64完整guest未有App就緒資格，不能用x64轉譯或image標籤替代。探險長名縮字、同最終APK各版回歸、OS與長時間仍依[九組矩陣](ANDROID12_15_GAME156_GOAL.md)。

原始AVD console／logcat／PM／截圖只在ignored私人`.tools/lab/`，Git不含遊戲包、原始日誌或帳號資料。

本輪API34第一次host OpenGL冷開機曾讓ADB shell及console停止回應；指定serial的正常emu kill亦在10秒內無回應。重新核對host前端／QEMU完整路徑、AVD、埠與parent關係後，僅停止該隔離host程序，未改其他instance或ADB server。當時尚未更新／啟動157，不能列為157遊戲失敗。保留此非正常關閉紀錄，再使用同一AVD的SwiftShader進行一次比較，下一boot須重新核實系統與PM狀態。
