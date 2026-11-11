package dev.ody.seedpreview;

/** Presentation only: saved intent is never treated as a game acknowledgement. */
final class DisplayOverview {
    static String text(Boolean[] wanted,DisplayGroups.Report[] reports,boolean gamePresent,
            boolean saving,boolean pendingOff,String progress,String problem,String unavailable,LensStatus.Phase phase){
        if(pendingOff)return saving?"正在關閉全部…":"全部關閉尚未完成，請重試";
        if(phase==LensStatus.Phase.UNKNOWN||phase==LensStatus.Phase.UNCERTAIN||phase==LensStatus.Phase.RESTART_REQUIRED)
            return "需要重開遊戲";
        if(!problem.isEmpty())return "核對未通過";
        if(unavailable!=null)return "目前環境不支援";
        if(saving&&!progress.isEmpty())return "正在處理…";
        boolean any=false,applied=false,restored=true;
        for(int i=0;i<4;i++){
            if(wanted[i]==null)return "設定尚未確認";
            any|=wanted[i];
            applied|=wanted[i]&&reports[i]!=null&&reports[i].effect==DisplayGroups.Effect.APPLIED;
            restored&=reports[i]!=null&&reports[i].effect==DisplayGroups.Effect.RESTORED;
        }
        if(!any&&!gamePresent)return "已關閉設定，遊戲未開啟";
        if(!any)return restored?"已確認關閉":"已關閉設定，等待還原確認";
        if(!gamePresent)return "等待遊戲";
        return applied?"已套用至已回報畫面":"等待遊戲畫面回報";
    }
}
