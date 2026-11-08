# ARM64 載入安全研究（2026-11-08）

## 狀態與界線

本頁區分來源、研究與遊戲效果：0.3.1/code12已將ARM64完整CPU context及supervisor
交付Libra API34，正常UI在新遊戲156.1程序完成兩頁ON/OFF；原掃描已恢復並有新資料接受。
0.3.0原產物不改，也不追溯宣稱具備新保護。完整證據見[驗收矩陣](VALIDATION.md)。

目標不只是等待有上限：若遠端函式逾時或收到非預期訊號，回復CPU暫存器不等於
還原linker鎖、TLS內容、heap或已發生的副作用；不能繼續cleanup／重送或假裝成功。

## 開發候選的保護與限制

- ARM64入口現在使用父程序持久A/C/R/D ACK、15秒整體界限、精確pidfd目標收容；
  確認game與thread的PID/start/owner及user-space狀態後才准入，不提供任意target CLI。
- 每次call最多2秒；函式用獨立guarded stack，正常返回核對GPR、FPSIMD、TLS pointer、
  `NT_ARM_SYSTEM_CALL`與原SP上128 bytes。載入結束再核對整體初始snapshot才准detach。
- syscall只用已存在RX的精確SVC指令，不修改可執行記憶體。入口／退出共用deadline；
  kernel restart回傳、其他signal、逾時、snapshot失敗立即結束worker，不做remote cleanup。
- ARM64 syscall-exit停點的x7受kernel管理：僅SETREGSET回讀一致仍可能在resume被覆蓋。
  候選先經精確PC0/MAPERR user-fault邊界離開kernel syscall stop，再還原完整snapshot。
  測試曾抓到8參數函式錯值，修正後連續call、snapshot與child恢復全部通過。
- SVE／SME涉及額外每thread狀態，不能以保存V0–V31替代。候選仍拒絕SVE／SME／MTE／PAC
  能力及可讀SVE/SSVE regset；pidfd不可用亦保持OFF。這是明確相容限制，不是支援所有
  API28–35硬體／kernel的保證；不得為擴大表面覆蓋而移除拒絕。
- APK UI兩ABI都要求明確fail-stop確認；0.3.1 Libra正常UI、JNI及兩頁效果已有下述驗收。

機制依據為[Linux SVE說明](https://www.kernel.org/doc/html/latest/arch/arm64/sve.html)、
[Linux SME說明](https://www.kernel.org/doc/html/latest/arch/arm64/sme.html)及
[Android common kernel ARM64 ptrace](https://android.googlesource.com/kernel/common/+/790c2f9d15b59/arch/arm64/kernel/ptrace.c)。
這些來源不代表Libra實際kernel版本或行為，裝置結論只取自下列測試。

## 已執行的隔離測試

`research/arm64_call_fixture.cpp`拒絕任何命令列target，僅建立並追蹤自己的fork child。
使用pidfd、PDEATHSIG、EXITKILL及本機watchdog；guarded scratch stack與原stack分離。
正常返回需精確SIGSEGV/MAPERR、PC=0與預期SP，然後核對GPR、FPSIMD、TLS pointer、
syscall number及原SP上方128 bytes。這不是TLS內容或任意記憶體完整回滾。
未知結果鎖住context、不還原後繼續、不再呼叫，只終止／回收自己的測試child。

Libra API34／ARM64／SELinux Enforcing實際通過：

- 五類各三輪，共15案例：正常返回、100ms逾時、非法指令、SIGUSR1中斷、child退出。
- 90次不合法參數／重疊stack等拒絕，拒絕前後原snapshot相同。
- 正常函式真的改動FP狀態、使用8192-byte local stack並回傳459；還原後第二次呼叫
  及原child heartbeat恢復。不是只測未變動狀態。
- timeout實測100–101ms，訊號20–21ms；未知結果不重試，不稱CPU或副作用復原。
- 研究binary SHA256：`7537c83df83f09eaf5b4e5a414bfa7cc51b831b381171d8727e86d681129ea07`。
- 私有receipt `arm64-fixture-libra-1794147983406650300.txt`，SHA256
  `1f747c17f14221adfcfa6dbf2b775137e167cd6a143f34ffde1afbce083f47d6`。

`app/supervised_load_test.cpp`移除不必要x64 entry-header依賴，兩ABI均建置。
Libra與Virgo各7類真實自有子程序測試通過：正常、CALLING前失敗、CALLING後失敗、
逾時、錯序協定、DETACHED後非零退出、RETURNED但尚未DETACHED的失敗。
測試沒有ptrace或dlopen；僅驗證監督協定／pidfd收容，不能單獨證明ARM64 loader安全。

本輪遊戲PID/start持續不變，全部threads無tracer，Lens OFF、沒有建立scanner pause，
scanner程式／設定SHA一致。22:31正式唯讀GET顯示兩台仍在原job429且有新的站台資料／
上傳接受。原始receipt在忽略的`.tools/research/`，不進Git。

## 下一個整合門檻

本輪新增`research/arm64_context_test.cpp`直接編譯實際KittyTrace overlay，而非另寫一份
模擬實作。Libra API34/Enforcing通過27個context輪次與5個supervisor整合案例，共32例：

- 正常syscall／函式、ppoll逾時、內部restart、程序退出、函式逾時／SIGUSR1／非法指令。
- 實際overlay的mmap／mprotect／munmap、八參數call、兩次呼叫及原snapshot相等。
- 三類UNKNOWN結束worker，父程序只回收自有target；共享旗標明確檢查沒有錯誤fallthrough。
- 真正KittyInjector在自有fork child載入獨立fixture SO、constructor生效、export回值正確、
  正常卸載後mapping消失，CPU狀態還原且child heartbeat繼續。**這項沒有JNI／ART**，
  不能聲稱Android App namespace、JNI_OnLoad或遊戲載入已驗。
- receipt `arm64_context_test-Libra-1794149292851061200.txt` SHA256
  `fbe9f62ec2c164d849375e94cfb47b62efbfc68e36d253c099155b142c2b9949`；
  test binary SHA256 `8588d1958be92dd89c9a81b1a4d2e06c7168b8afac45e995fa4f6693b6d0ebef`。
  兩次game PID/start相同、全部threads無tracer、Lens OFF、沒有pause、scanner SHA未變。

後續已完成：

1. `research/arm64-jni/`自有Android App的真實JNI／ART載入、worker attach/detach與原thread
   lifecycle通過；使用同一backend編譯期獨立白名單，正式binary不含研究套件或thread入口。
2. 0.3.1/code12完整建置／原簽章／唯一產物已交付Libra；正常遊戲重開後兩頁ON/OFF通過，
   原掃描恢復並有新站台接受。未在正式遊戲刻意製造逾時，也未主張可回滾任意函式副作用。

同版NativeBridge兩頁ON/OFF回歸亦已完成。仍需更多Android/ROM與第二遊戲版本；研究App與32例fork測試
不能代表所有硬體／kernel或正式遊戲每種異常路徑均已驗收。

第二遊戲版本156.0仍需使用者核准隔離測試instance並親自登入；以上研究不替代該驗收。
