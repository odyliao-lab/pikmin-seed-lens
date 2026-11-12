package dev.ody.seedpreview;
import java.util.Map;

/** Basic checklist only; never treats historical last_phase or APK installation as live readiness. */
final class FirstUseCheck {
    static String summary(Map<String,String> f,boolean namespaceMismatch){
        String root=f.get("root"),api=f.get("android_api"),abi=f.get("abi");
        String platform="x86_64".equals(abi)&&DiagnosticReport.oneOf(api,"32","35")
                ?"MuMu 已驗證 API 範圍；仍需核對轉譯與載入":"此平台需另外確認相容性";
        String rootText=namespaceMismatch?"已取得 UID0，但掛載命名空間不符"
                :"granted".equals(root)?"本次 Root 檢查通過"
                :"not_checked".equals(root)?"請按下方「檢查 Root」":"Root 尚未確認可用";
        boolean game="157.0".equals(f.get("game_version"))&&"1793360009".equals(f.get("game_code"));
        return "① Root："+rootText+"\n② 環境："+platform
                +"\n③ 遊戲版本："+(game?"157.0／版本代碼符合，載入時另核完整身分":"未找到或尚未精確配對")
                +"\n④ 內建檔案："+("match".equals(f.get("native_files"))?"核對通過":"尚未確認完整")
                +"\n⑤ 實際套用：回工具頁選擇功能、完成同意，再開遊戲；依即時狀態確認。";
    }
}
