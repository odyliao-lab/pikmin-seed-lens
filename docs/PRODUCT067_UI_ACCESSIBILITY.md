# 0.6.7：版面及輔助操作分項

2026-11-11，僅新index5／SeedLens-Isolated-A15，created1794181954477010，
實際API35／x86_64／libnb.so；同boot400309d5…5a35、原簽章0.6.7/code25。
本文件是介面分項，不增加Android12／14或其他遊戲資格。

## 版面實際回歸

先核對157程序16590/start404069的control OFF、ready/control0及全部42筆歷史SHA，
再正常關閉遊戲，只在產品App前景測試；不操作探險／物品。

- 暖色紙張首頁可正常顯示透明皮克敏／放大鏡插畫、狀態卡及三個分頁。
- 同APK暫以density540／font_scale2測320dp；首頁與工具內容垂直捲動，底部分頁可見。
- 真實保留的RESTART_REQUIRED提示完整換行，可捲至具名稱的disabled Switch，
  開關仍為76×56dp；沒有水平溢出，沒有將歷史屏障改成成功來取畫面。
- 結束後還原density280／font_scale1、auto rotation1/user0/free，外觀回到測試前夜間森林。

064／065的行內同意、橫向與鍵盤證據分別保留，不冒充每個場景均在067重跑。

## TalkBack的實際範圍與未通過項目

隔離機原已安裝Google TalkBack17.0.1.926549743，未新增APK或產品依賴。
測前secure設定為enabled_accessibility_services=null、accessibility_enabled=0、
touch_exploration_enabled=0。只暫開該服務，dumpsys實際列Bound且沒有Crashed service，
touch exploration其後為true；實際畫面出現產品文字焦點框。

節點核對可見原生Switch、名稱「花苗名稱透視開關」與checked=false/enabled=false，
目前分頁另有名稱；這是節點語意檢查，不是朗讀證據。

本輪UIAutomator dump干擾TalkBack後首次教學重新出現；擷取時有既存的dump程序
exit139，但XML成功產出。這些過程不記作TalkBack操作成功，也不是產品崩潰。
改用畫面及系統鍵盤輸入後，尚未取得完整TalkBack逐項導覽／啟用開關及實際語音證據。
Google[官方鍵盤說明](https://support.google.com/accessibility/android/answer/6110948?hl=en)
區分Default與Enhanced keymap；試送Alt／Action組合仍不足以證明服務接收及操作結果。
一般ADB tap直接切換分頁也不算觸控探索／TalkBack雙擊成功。

三個secure值已逐項還原並讀回完全相同，服務Bound／Enabled均空；沒有授予其他權限。
完整TalkBack導覽及朗讀仍列待驗，不能僅因啟動服務或有焦點框就記通過。

## 保留樣本與其他限制

使用者本輪回覆「目前先保留現有樣本」。保留三筆短名未領回花苗，不派遣、不領取，
不更換樣本帳號；探險長名／整張離屏仍未通過。067的156.x回歸及Android12／14另驗。
原始截圖、XML、系統狀態及APK只放ignored私人證據目錄，不提交Git。
