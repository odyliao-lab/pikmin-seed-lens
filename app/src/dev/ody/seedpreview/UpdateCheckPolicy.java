package dev.ody.seedpreview;

/** Only schedules metadata checks when the App is opened. No background scheduler/download. */
final class UpdateCheckPolicy {
    static long delay(int failures){return failures==1?300_000L:failures==2?1_800_000L:86_400_000L;}
    static boolean due(boolean enabled,long now,long attempt,long success,int failures){
        if(!enabled||now<=0)return false;
        if(attempt<=0||now<attempt)return true;
        // Missing failure history on upgrade retains the old daily schedule.
        long wait=failures==0&&success>0&&success>=attempt?86_400_000L:delay(failures);
        return now-attempt>=wait;
    }
    static int failed(int count){return Math.min(3,Math.min(3,Math.max(0,count))+1);}
    static String status(long remote,long installed,int minSdk,int sdk,String version){
        if(remote<installed)return "本機版本較新；已發布版本為 "+version;
        if(remote==installed)return "目前沒有較新的發布版";
        return minSdk>sdk?"新版需要較新的 Android":"發現新版 "+version;
    }
    static String retry(int failures){return failures==1?"5 分鐘":failures==2?"30 分鐘":"24 小時";}
}
