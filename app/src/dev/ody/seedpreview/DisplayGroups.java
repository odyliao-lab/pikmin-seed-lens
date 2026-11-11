package dev.ody.seedpreview;

/** Standalone group contract; four switches, no partial-enable setting. */
final class DisplayGroups {
    static final String[] TITLES={"花苗資訊","皮克敏資訊","探險資訊","精華／花瓣對照"};
    static final String[] DETAILS={
        "顏色與裝飾、已收／未收、持有／育苗／待種與未知原因；育苗詳細頁步數列顯示種類及剩餘步數。",
        "成年主清單顯示目前花態、可收瓣、任務與已取得裝飾的短副標；原完整名稱保留於詳細頁。",
        "一般白盒對象與裝飾、探險狀態及遊戲剩餘時間；原地點可於詳細頁查看，原生隱藏列不強制顯示。",
        "選精華畫面保留原精華數，補對應花瓣、容量與剩餘空間。"
    };
    static final String[][] MEMBERS={
        {"seed-preview","seed-collection","planter-display"},
        {"adult-state-preview","adult-decoration-preview"},
        {"gift-preview","expedition-display"},{"nectar-stock-preview"}
    };
    static final int[] FIRST={0,3,5,7};
    static String failure(int exit,String output){
        if(output!=null&&output.contains("BLOCKED_STAGE_6"))return "遊戲版本或檔案核對不符，保持原顯示。";
        if(output!=null&&output.contains("BLOCKED_STAGE_22"))return "目前平台的載入後端不支援，保持原顯示。";
        if(output!=null&&output.contains("BLOCKED_STAGE_23"))return "App 安裝檔核對失敗，請重新安裝原簽章 APK。";
        if(output!=null&&output.contains("CONFLICTING_MODULE"))return "發現其他載入模組，已停止；請先確認隔離環境。";
        if(exit==26)return "設定檔無效、無法讀取或同步未完成；保留原件，點此重試。";
        if(exit==4)return "前一操作尚未結束，請稍候重試。";
        if(exit==1||exit==255||exit<0)return "Root 授權或安裝檔核對未通過，請允許此 App 的 Root 後重新核對。";
        return "操作未完成（"+exit+"）；請核對 Root 與遊戲，不自動重送載入。";
    }
    static Boolean setting(int group,int exit,boolean complete,String output){
        if(group<0||group>3||exit!=0||!complete||output==null)return null;
        if(output.equals("LENS_GROUP\t"+group+"\ton\n"))return true;
        if(output.equals("LENS_GROUP\t"+group+"\toff\n"))return false;
        return null;
    }
    static String report(int group,boolean enabled,LensContext context,String raw,long now){
        if(context==null||raw==null||raw.indexOf('\r')>=0||raw.length()>8192||!raw.endsWith("\n"))return "等待遊戲資料";
        String[] lines=raw.split("\n");
        if(lines.length!=10||!lines[0].startsWith("LENS_SESSION\t"))return "回報未完整核對";
        LensContext observed=LensContext.parse(lines[0].substring(13)+"\n");
        if(observed==null||!observed.same(context)||!lines[9].equals("LENS_REPORTS_END\t"+context.game.pid+"\t"+context.game.start))return "程序已變更，等待重新綁定";
        boolean waiting=false;
        for(int i=FIRST[group];i<FIRST[group]+MEMBERS[group].length;i++){
            if(lines[i+1].equals("LENS_MEMBER_MISSING\t"+i)){waiting=true;continue;}
            String prefix="LENS_MEMBER\t"+i+"\t";
            if(!lines[i+1].startsWith(prefix))return "回報契約不符";
            String[] f=lines[i+1].substring(prefix.length()).split("\t",-1);
            if(f.length!=11||!f[0].equals("v1")||!f[2].equals(""+context.game.pid)||!f[3].equals("157.0")||!f[10].equals(""+context.game.start))return "回報版本或程序不符";
            try{long stamp=Long.parseLong(f[1]);if(!f[1].matches("[0-9]{1,19}")||stamp<0||stamp<now-5000||stamp>now+2000)return "等待新鮮回報";
                for(int n=5;n<=9;n++)if(!f[n].matches("[0-9]{1,10}")||Long.parseLong(f[n])>(n==5?1L:4294967295L))return "回報欄位無效";
            }catch(NumberFormatException e){return "回報數值無效";}
            if(!f[4].equals("ready")&&!f[4].equals("waiting-cards"))return "資料未就緒："+f[4];
            if(!f[5].equals(enabled?"1":"0"))return enabled?"等待整組套用":"等待原顯示還原";
            waiting|=f[4].equals("waiting-cards");
        }
        return waiting?(enabled?"已啟用，等待遊戲卡片":"已關閉，等待回報"):enabled?"已啟用":"已關閉";
    }
}
