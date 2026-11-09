# 157.0原簽章來源與精確設定檔（2026-11-09）

新隔離MuMu index5的官方Play當時仍只提供156.1，正常重開商店後沒有更新按鈕；這是當下來源限制，不代表157不存在。後續使用者指出AutoFarm已安裝157，才進行下述限定唯讀取得。

## AutoFarm來源例外：只讀遊戲安裝包

重新核對AutoFarm的host instance名稱／建立時間／端點、guest boot／API及PM套件身分，只讀PM列出的遊戲base與完整ARM64 split，逐份確認讀取前後及本機SHA一致。沒有操作AutoFarm UI、啟動或停止遊戲、管理scanner、讀取憑證或複製登入資料。來源adbd在開始與結束時原本就是uid0，未變更來源Root狀態。遊戲pidof當時無結果，只描述當下程序查詢，不宣稱掃描業務健康。

早期「原四台未操作」描述此前階段；本次AutoFarm例外僅限上述唯讀遊戲來源，其餘原instance及台中Libra／Virgo未操作。後續安裝與UI驗收只在新index5。

PM與manifest一致為 **157.0/code1793360009**，minSdk28／targetSdk36。base與split均經apksigner驗證，原遊戲signer SHA256：`944bda466eaf64be1c93896d5c8e9a454a07c8d8fcbc4adeeefb3545b335e979`。

| 材料 | bytes | SHA256 |
|---|---|---|
| base APK | 93796584 | `4d28ce6c8e5c7efa1e6a864edaa545991ffe52597f751d37db55bce69c4af957` |
| ARM64 split | 87435413 | `253ebec010e59980579cb114d75282202ac1da814f28eeecee71470156f3bcf6` |
| libil2cpp.so | 257043464 | `6c36ef78a6d1f45cf5b36c73761ed526226a081d981001b71608984afde246a5` |
| global-metadata.dat | 32230812 | `3f6c34de9955a27c0b273ad72d9ea52baa3d7a54aefb060eb75a065a4bca5eb2` |
| libunity.so | 23638640 | `534fe2d1f8cadb79e6109ba63cf8f869b32599f915911447e19e38cf34b02149` |

## 舊版未知版本負例

只在新index5正常更新完整splits並保留本人登入，核API35／libnb／精確157.0。使用原簽章0.4.1，由正常App確認當次主畫面、同意收容並啟用，實際回報「遊戲沒有精確配對的設定檔，保留原顯示；不會嘗試舊位址。」控制檔仍OFF、無Lens mapping、沒有新attempt；既有16份產品／研究紀錄逐份SHA不變，TracerPid0。這是未知版本拒絕負例，不是157功能通過。

## 精確設定檔與離線審查

僅複用已核SHA的公開Il2CppDumper分析工具及通用設定，針對本次合法取得的ELF／metadata重新產生私人dump；未引入其他專案的app、scanner、helper或憑證。自動解析完成，保留工具的保護啟發式警告，沒有ForceDump。

| hook | ARM64 body RVA |
|---|---|
| inventory | `0x66f6730` |
| text | `0x6f098e4`（metadata入口`0x6f098e0`為單一B+4 veneer） |
| canvas | `0xe1b5ba0` |
| expedition | `0x60c92b4` |
| detail | `0x60d66c0` |

完整方法簽名、可執行ELF segment及各6-word prologue逐一比對；14個相關類別完整欄位布局與156.0／156.1一致，才沿用layout1。來源工具必須解析全部三個已審查profile，拒絕未解析／重複／多餘／缺漏紀錄及錯誤code；不靜默略過新版本。

Python16項測試通過，三版實際來源比對涵蓋3個ELF、15個完整簽名hook body、3個文字veneer、14個相同類別布局及39個刻意破壞負例。這不證明runtime metadata通過、遊戲兩頁可用或其他平台合格。

## 產品候選與待驗

原簽章 **0.5.0/code15** 單APK SHA256 `80f955156b5273d4a34a8a2cf273346362c0e71796d36c06ddae055663951b77`，19281214 bytes；cert仍為`9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e`，舊0.4.1不覆寫。預設與明確`-Api31Qualification`建置均通過48／29／27／8／4 JVM與封裝檢查；qualified版本只開API31／35的既有精確後端，API32／34 x64仍拒絕。

API35以新x64 control fixture實跑76項通過，只操作自建fixture；原16份attempt SHA不變，adbd恢復UID2000。ARM64 fixture只有編譯。0.5.0正常升級安裝於新index5；切換繁體中文後的帳號獎勵由本人處理。其後兩個遊戲程序各自驗兩頁ON／OFF、花苗縮字與探險末排真正離屏返回，未知歷史保留；探險長名縮字與OS／長時間仍未驗。詳見[0.5.0驗收](PRODUCT050_QUALIFICATION.md)。

API31／34上的157原遊戲尚未執行，不能從156.x的SEVL失敗跨版推論。新APK也需重新驗156.0／156.1，不能把0.4.1證據當0.5.0通過。九組、探險長名縮字、OS與長時間條件仍依[驗收矩陣](ANDROID12_15_GAME156_GOAL.md)。

原APK／dump／manifest／PM、截圖與原始receipt只留ignored私人`.tools/lab/`；Git只留本摘要、來源守門與測試。
