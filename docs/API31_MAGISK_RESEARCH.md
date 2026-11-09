# API31隔離AVD的Magisk Root驗證（2026-11-09）

使用者明確調整條件並授權安裝Root，原先AVD「不安裝Root」限制僅對此階段解除。目標仍只限`SeedLens_API31_x64`／emulator-5580；原四台及台中Libra/Virgo未操作。四組遊戲驗收仍未完成。

## 來源、隔離與還原材料

使用[官方Magisk v31.1](https://github.com/topjohnwu/Magisk/releases/tag/v31.1) APK與同版本[官方AVD patch流程](https://github.com/topjohnwu/Magisk/blob/v31.1/scripts/avd_patch.sh)。僅將工作目錄改為本次私有guest目錄並加入錯誤即停止，使用APK原始x64二進位；未更改kernel、SDK原映像或AVD設定去冒充API。

| Artifact | SHA256 |
|---|---|
| 官方Magisk APK | `89cefac0a318afa39fb3880c45c87b036c05bc1ebf02a7a715b7f8d7c26ac6e7` |
| 原SDK ramdisk（前後不變） | `bc6aeafd4d38023b1fd184767eec4b1309a934704c4c2bace3300258b51b46c4` |
| 私人ramdisk override | `5537a63d5a4f556b8204b108be4cc5ca41d958c378e72500e2779ea2bb83eb20` |
| Root probe0.0.2/code2 | `644aaeafc5115fc2e63acdd408236cf5a6b16bdc0d23d8b0cf09ce7568c66368` |
| Root probe0.0.3/code3 | `992a64bcabf6d7d2802a6635df990d2f47baee0c781df7bf6834677dc9396cf9` |

root準備與原始收據在repo外`D:\SeedLensLab\root-prep\api31-root-run-9fa677192d9f46409573f1b43b98dfe9`。先備份guest全部舊journal；stock AVD正常關閉後，冷備份userdata／encryptionkey／cache qcow2及config。新ramdisk以emulator `-ramdisk`參數使用，未覆寫共享SDK ramdisk，未wipe或刪除AVD資料。

patch保留KEEPVERITY=true、KEEPFORCEENCRYPT=true，官方流程選出metadata pre-init storage。管理器完成標準additional setup與正常重啟。未安裝LSPosed、顯示模組或其他Magisk modules。

## Root入口與實際普通App證據

初次ramdisk啟動已見Magisk daemon，但管理器尚需additional setup；當時PATH su仍是原廠入口，普通App出現IOException。該失敗原件保留，不稱安裝APK即完成Root。

研究probe0.0.2加入API31固定`/debug_ramdisk/su`比較選項；0.0.3保留PATH、明確入口及研究auto三種固定選擇，沒有任意Root命令參數。沿用原lab憑證，每份新bytes升版本/code，原0.0.1/2不覆寫。普通App結果要求UID>=10000、API精確吻合、su-M exit0、uid0、本次boot相同及兩個mount namespace逐字相同；不只檢查輸出含mnt字樣。

管理器僅授權`dev.ody.seedlensrootprobe`（本輪UID10148），沒有全域授權或授權probe／carrier／Shell。標準setup完成後，PATH新增Magisk提供的`/system_ext/bin/su`；Zygisk關閉後的新boot，**PATH su-M與明確/debug_ramdisk入口均從普通App實際通過**。uid0、u:r:magisk:s0、兩個namespace同為PID1的mount namespace。觀察到這點後維持產品原`su -M`入口，沒有加入產品fallback或降低-M要求。研究auto比較helper僅保存在research。

管理器初始setup啟用Zygisk，bridge字串曾是`libzygisk.solibndk_translation.so`。這不符合原准入規則，沒有剝除前綴或冒稱原組合；在管理器關閉Zygisk並正常重啟後，確認UI Disabled、實際API31、`libndk_translation.so`、SELinux Enforcing、Magisk31.1/code31100。modules目錄為空。沒有手動放寬SELinux或修改bridge property。

## Magisk domain載入與收容分層

`run-api31-probe.ps1 -RootViaMagisk`明確限API31，native runner透過`su -M -c`執行；APK/runner SHA、精確AVD/boot/API/bridge/SELinux及PM path檢查不變。host仍為原carrier0.0.4及probe0.0.6，同bytes重用；不重建或覆寫已簽署host。

普通App Root probe與native canary目前是分開觀察：後者由研究adbd發起Magisk root child，不能冒稱正常產品App已完成整個載入流程，更不是遊戲效果。

- 同一Magisk domain下10類supervisor fixture實際exit0，含非leader ptrace、CALLING前不得殺target、CALLING後timeout及worker退出。收據SHA256 `5df8aaf931ca0d693f6fabff601df9783796f1902bf9e1674cc8b2fa8545faa0`。
- 最初兩個新probe程序在CALLING前未取得合格user-space入口，exit44／END UNKNOWN；各自正常FINISH，沒有載入、強殺或同程序重送。原件`api31-run-dc9cb9a9ea534aba897ed74c3911c38f`、`api31-run-1eb73b0406b14c51acdad306fb189014`保留。
- 新增研究診斷僅記錄拒絕階段、候選數、getRegs／orig_rax／owner。沒有移除orig_rax=-1或其他入口檢查。後續另一新程序觀察到orig_rax=228時拒絕，取得-1及同owner後才准入；完整七階段journal、namespace/JNI四值與負例已返回，正常FINISH及故意timeout的最終結果另外補列。

### 最終JNI及故意timeout

原probe0.0.6的新程序完整JNI／負例與90秒FINISH通過，收據`api31-run-adc7fc88b7d64c5a80018db3eb8bba48`。接著另一新程序的timeout案例，12次皆orig_rax=228，在CALLING前exit44拒絕，收據`api31-run-ce51d6d5517a46cd8633a8e076b86223`；仍列TIMEOUT_CASE_NOT_PROVEN，不改成收容成功。

定位到自有測試App每次volatile增量都查System.nanoTime，API31轉譯環境大量停在clock_gettime。probe0.0.7/code7改為每65536次純user-space增量才檢查一次時限，仍用原30秒deadline及90秒FINISH；這只改善自建fixture，不是遊戲可用的專用thread，也沒有改native user-space／orig_rax守門。新carrier0.0.6/code6精確綁probe0.0.7，lab key不變；原APK／attempt均保留。API35 lab carrier下一版預留0.0.7/code7，本輪未建置或安裝到API35。

| 本輪新研究artifact | SHA256 |
|---|---|
| probe0.0.7 | `1d8d187188bcbf5822fbed6a75ea7fcedd71b503cd94fe7d5b7d07d337174110` |
| API31 carrier0.0.6 | `3c80fc64644c07830ae59dda68ed2a4f6d7bead110b16472ed62c77faea5b8d4` |
| API31 host，精確probe0.0.7 | `c0bfd9c8771b2b4cbfe3d8483e42df47edd60db9aafceb982d6e77e297ffad8d` |
| native runner | `476d9806886f5cef8c2add052a47aca803106cd5e31156c5ea99dfa2a31176d2` |

新probe0.0.7的故意timeout第一輪取得orig_rax=-1才進durable CALLING；父程序確認exit42、END UNKNOWN、worker回收及精確target pidfd退出。原件`api31-run-6527ce2c52e54d198b143a9bc2b88bd5`。之後另一新程序開始正常回歸，沒有重送或清除timeout程序的紀錄。

timeout後的新程序完整JNI四值／所有負例／singleton、七階段journal及90秒FINISH再通過，程序正常消失。原件`api31-run-ef4762592be24cf8b215b8628a09e9fb`。namespace收據SHA256 `92699143aea045d717e266a7fdc5568aedefa030a5f4c679a4460a8e25b810ad`；FINISH `238f698503178aa920b94e0de2ac665680e8d77d7adf75fb71fea789b9a5fba8`；完整journal `61b5acc52de21a9956e0eac0d82cc95409dd2e01d541a41c10062a62ff932278`。timeout UNKNOWN journal SHA `eb3074fb2932750a94f7f3b3c375c094ea4d8bbe7c0096ecbc680b1ddbd3e939`。

結束逐檔比對Root安裝前的7份attempt，SHA全部不變（包括最早拒絕與收容未確認紀錄）。本輪新增拒絕與timeout UNKNOWN亦保留。確認無probe存活、Enforcing／原bridge／空modules，再sync、恢復adbd UID2000、正常關閉同一AVD；原SDK ramdisk SHA再次核對相同。

這次Root只存在於以該私人override啟動的研究環境：後續需沿用紀錄中的`-ramdisk <private-path>`與SHA，不能用AVD Manager預設stock啟動後仍推論已Root。Android正常重啟已驗Root配置，但不是產品OS重啟或長時間驗收；不需要改共享SDK映像。

這些是x64/NativeBridge與新Root環境的隔離證據。原生ARM64/API31、正常產品單APK、四組遊戲兩頁／縮字／捲動／OFF／遊戲重啟仍未驗。合法156.0／156.1完整splits及原產品簽章來源仍待提供；沒有以lab key簽署產品或開始登入。
