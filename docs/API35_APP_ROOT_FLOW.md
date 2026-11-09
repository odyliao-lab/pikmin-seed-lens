# API35普通App自行載入研究（2026-11-09）

在[Google套件安裝](API35_GOOGLE_INTAKE.md)後，於同一已授權新MuMu index5補齊普通App發起的PM loader研究。CLI現核名稱／created timestamp／端點，guest核API35、x86_64、libnb.so、原Permissive及boot `994efc10-e992-463c-9e3b-44c3c82908bb`。不是原四台或台中裝置。

## 研究來源與不可替代的邊界

新增獨立`api35-app-root`目標，沿用API35 adapter與相同PM loader／supervisor來源；編譯時固定API35、精確carrier host名稱及SHA，要求自身位於Root client的canonical PM executable位置。沒有把API31 runner放寬成任意平台或替換產品守門。

Root client0.0.5/code5由普通App UID10052執行原`su -M`，Java先核實際API與明確expected_api（只允許31／35）、自身PM路徑及runner SHA；該批內嵌runner只接受35。API31先前結果仍使用原client0.0.4，不由此次API35結果推論。錯expected_api=31在API35實際拒絕，instrumentation1，未呼叫載入。

native再核自身及carrier PM路徑、ancestor owner／mode、實際API、自有probe的精確UID／PID／start與host bytes。相同runner的tmp副本實際exit14，未進target檢查。普通App正常路徑的錯PM-host路徑／stale start各exit6／5後，才對當次新probe單次載入。沒有通用Root命令入口、遊戲目標或重送。

研究client／carrier升新版本，沿用外部lab簽章；probe重用已交付0.0.7的同SHA原件。產品版本及原簽章不變，lab key未用於產品。

| 本輪研究artifact | SHA256 |
|---|---|
| Root client0.0.5/code5 | `1b57205d8f095c3cdf565e6678ed40081414f0825227011568794db8deead6f2` |
| api35-app-root | `b22877ed3f6224fc0d716540048e1c7a6837544f9a73880c964edd393ea6ce84` |
| carrier0.0.7/code7 | `5b764e8fff15b662fc6dd92984f35d3b3c755fe83a26593c69c7ccedb799952d` |
| carrier API35 host | `e4808e2c6972a71390216b148466c22c5533168c7a58ec19be5ca7d341df71bb` |
| 同一probe0.0.7/code7 | `1d8d187188bcbf5822fbed6a75ea7fcedd71b503cd94fe7d5b7d07d337174110` |

## 裝置分階段結果

首次普通App正例：probe PID5840/start51959，載入client PID5976/UID10052，exit0。真App ClassLoader／namespace、ARM JNI四值、精確版本與錯套件／key／缺library／缺symbol負例、JNI OnLoad／thread singleton、完整journal與90秒FINISH均通過，probe實際退出。回傳exit0本身不當成JNI或遊戲成功。

另一新probe PID6149/start62809，client PID6285/UID10052故意在durable CALLING後stall；實際exit42、END UNKNOWN，精確target退出且instrumentation結束。保留這筆UNKNOWN，不對同程序再載入。client不因等待逾時殺掉in-flight runner，native監督收容仍負責確定結果。

後續新probe PID6401/start63967，client PID6542/UID10052再次exit0，完整JNI／負例／journal／90秒FINISH與程序退出通過。前四份attempt逐字SHA相同，新三份全部保留；沒有刪除舊UNKNOWN或重送。結束無probe程序，原libnb.so／Permissive不變，modules目錄不存在；adbd恢復UID2000後，普通App PATH su-M再次通過。只保留index5開啟供使用者登入，未檢查或操作登入內容。

| 本機保存收據 | SHA256 |
|---|---|
| 首次normal namespace | `4e6eb467717d07cec7e6c8fbf2d8759ab593e33653e068538daf6fbeb8fd89ad` |
| 首次normal baseline／FINISH | `ac02835e0f9e80dbceef1b0c72ce4acce4b2999ca9460cbea4ca31c0b13650f1` |
| timeout journal／UNKNOWN | `097c94abd9a6636605a6b0f0b88661ef90365dab3ca4c194c97615b95cda1b08` |
| 新程序normal namespace | `4e6eb467717d07cec7e6c8fbf2d8759ab593e33653e068538daf6fbeb8fd89ad` |
| 新程序normal baseline／FINISH | `7bb59b09df5fcc6f22c0e5c140208a17fefafd5be381fe27839c513507185ab4` |

完整遊戲兩頁、名稱／縮字／捲動／OFF／重啟與原生ARM64仍未驗；普通App研究不能替代四組產品驗收。這不是OS重啟或長時間產品驗收。

私人原件目錄：正常`.tools/lab/api35-run-53a88f7fe685437a84c9a1f2e8441de4/`、timeout `.tools/lab/api35-run-37a2346ca9664089a8839dd67b531c4d/`、新程序`.tools/lab/api35-run-79e26255914a4f6cad44f710be479474/`。不提交APK／原始診斷／截圖。
