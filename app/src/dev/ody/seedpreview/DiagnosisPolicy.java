package dev.ody.seedpreview;
import java.util.*;
/** Advice only. Never changes compatibility admission or invokes a loader. */
final class DiagnosisPolicy {
 static List<String> advice(Map<String,String> f){return advice(f,false);}
 static List<String> advice(Map<String,String> f,boolean namespaceMismatch){
  List<String> out=new ArrayList<>();String api=f.get("android_api"),abi=f.get("abi"),root=f.get("root");
  if(!DiagnosticReport.SIGNER.equals(f.get("app_signer")))out.add("安裝來源待核對：請使用官方發布頁的原簽章 APK，勿解除安裝或清除既有資料。此檢查不代表 APK 內容已完整驗證。");
  if("x86_64".equals(abi)){
   if(!DiagnosticReport.oneOf(api,"32","35"))out.add("此模擬器 Android／API 尚未支援。目前實測為 MuMu 12.0（實際 API32）及 15.0（API35）；Android 12／API31 不能視為 API32。請勿安裝外掛來繞過版本檢查。");
   else if(!DiagnosticReport.oneOf(f.get("bridge"),"unknown","libnb.so"))out.add("ARM 轉譯後端不符：目前 MuMu 使用 libnb.so。請改用已驗證環境，勿手動替換系統轉譯檔。");
  }else if("arm64-v8a".equals(abi))out.add("這是 ARM64 手機：本輪新版只有 MuMu 驗收，不能保證此手機可用。即使取得 Root 仍須另驗平台；不需要另裝顯示模組。");
  else out.add("CPU 架構尚未支援或無法辨識，請提供這份診斷；不要改裝其他架構的 SO。");
  try{int n=Integer.parseInt(api);if(n<28||n>35)out.add("Android 版本超出目前載入範圍，保持原顯示；請先檢查是否有新版透視鏡。");}catch(Exception ignored){out.add("無法確認 Android API，不能推定相容。");}
  if("missing".equals(f.get("game_version")))out.add("未找到 Pikmin Bloom：請先安裝遊戲本體；工作設定檔／分身中的遊戲也可能無法被此 App 讀取。登入由你本人完成。");
  else if(!"157.0".equals(f.get("game_version"))||!"1793360009".equals(f.get("game_code")))out.add("遊戲版本未精確配對：四組資訊目前對應 157.0／1793360009。請先檢查新版透視鏡；不要為此清除遊戲資料或略過版本檢查。");
  if(namespaceMismatch)out.add("已取得 Root，但掛載命名空間不符：若使用 Magisk，開啟設定 → 超級使用者 → 掛載命名空間模式 → 全域命名空間（Global Namespace），再重新啟動 MuMu 並重試。僅適用既有 Magisk，不需另外安裝，也不會自動修改設定。若使用 MuMu 內建 Root，請提供診斷，不套用 Magisk 步驟。");
  else if("not_checked".equals(root))out.add("Root 尚未檢查：按「檢查 Root」後，於你既有的 Root 管理器授權透視鏡。檢查只讀取環境，不載入遊戲。");
  else if("unavailable".equals(root))out.add("找不到可用的 Root 指令：可能尚未 Root，或 Root 管理器對 App 隱藏了指令。一般 Android 不會因安裝本 APK 就自動取得 Root；若已 Root，請核對管理器設定。");
  else if("denied_or_incompatible".equals(root))out.add("未取得 Root：可能被拒絕，或現有 su 不支援此平台所需的呼叫方式。請在 Root 管理器確認透視鏡授權，再手動重新檢查；不自動改用其他載入方式。");
  else if("timeout".equals(root))out.add("Root 檢查逾時：請先處理 Root 授權視窗，再手動重試。逾時不能證明手機沒有 Root。");
  else if(!"granted".equals(root))out.add("Root 檢查未完成，請重新檢查或匯出診斷。");
  if("mismatch".equals(f.get("native_files"))||"unreadable".equals(f.get("native_files")))out.add("App 內建載入檔核對失敗：請從官方來源以相同簽章覆蓋安裝，不清資料、不另外複製 SO。");
  if(DiagnosticReport.oneOf(f.get("last_phase"),"UNKNOWN","UNCERTAIN","RESTART_REQUIRED"))out.add("曾有載入結果未確認：請正常重開遊戲，讓新程序重新核對。不要刪除紀錄或對同一程序重送載入。");
  if(DiagnosticReport.oneOf(f.get("last_exit"),"java_crash","native_crash","anr"))out.add("系統記錄到透視鏡曾閃退／無回應。可預覽並匯出問題回報；此摘要不包含完整堆疊或遊戲資料。");
  out.add("檢查結果只代表基本條件，不等於遊戲顯示成功。可用環境請先到遊戲主畫面，再依工具頁提示啟用；文字未更新時重新開啟列表。");
  return out;
 }
}
