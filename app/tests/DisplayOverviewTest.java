package dev.ody.seedpreview;
public final class DisplayOverviewTest {
    static int checks;
    static void check(boolean value){checks++;if(!value)throw new AssertionError("case "+checks);}
    static String text(Boolean[] on,DisplayGroups.Report[] reports,boolean game,boolean pending,String problem,LensStatus.Phase phase){
        return DisplayOverview.text(on,reports,game,false,pending,"",problem,null,phase);
    }
    public static void main(String[] args){
        Boolean[] on={true,false,false,false},off={false,false,false,false};
        DisplayGroups.Report[] reports=new DisplayGroups.Report[4];
        check(text(on,reports,false,false,"",LensStatus.Phase.ON).equals("等待遊戲"));
        check(text(on,reports,true,false,"",LensStatus.Phase.ON).contains("等待"));
        long now=10000;LensContext owner=LensContext.parse("123\t456\t"+DisplayGroupsTest.BOOT+"\n");
        String raw=DisplayGroupsTest.rows(now);
        reports[0]=DisplayGroups.checkedReport(0,true,owner,raw,now);
        check(text(on,reports,true,false,"",LensStatus.Phase.ON).startsWith("已套用"));
        reports[0]=DisplayGroups.checkedReport(0,true,owner,raw,now+5001);
        check(!text(on,reports,true,false,"",LensStatus.Phase.ON).startsWith("已套用"));
        reports[0]=DisplayGroups.checkedReport(0,true,owner,raw.replace("ready","waiting-cards"),now);
        check(!text(on,reports,true,false,"",LensStatus.Phase.ON).startsWith("已套用"));
        check(text(off,reports,false,false,"",LensStatus.Phase.UNVERIFIED).equals("已關閉設定，遊戲未開啟"));
        check(text(off,reports,true,false,"",LensStatus.Phase.OFF).contains("等待還原"));
        for(int i=0;i<4;i++)reports[i]=DisplayGroups.checkedReport(i,false,owner,raw.replace("ready\t1","ready\t0"),now);
        check(text(off,reports,true,false,"",LensStatus.Phase.OFF).equals("已確認關閉"));
        reports[3]=DisplayGroups.checkedReport(3,false,owner,raw,now);
        check(text(off,reports,true,false,"",LensStatus.Phase.OFF).contains("等待還原"));
        reports[0]=DisplayGroups.checkedReport(0,true,owner,raw.replace("LENS_MEMBER\t2\tv1\t10000\t123\t157.0\tready","LENS_MEMBER\t2\tv1\t10000\t123\t157.0\twaiting-cards"),now);
        check(reports[0].effect==DisplayGroups.Effect.APPLIED&&reports[0].text.contains("其餘待回報"));
        reports[0]=DisplayGroups.checkedReport(0,true,owner,raw.replace("ready","waiting-cards").replace("\t3\t0","\t0\t0"),now);
        check(reports[0].effect==DisplayGroups.Effect.WAITING);
        check(text(off,reports,false,true,"",LensStatus.Phase.OFF).contains("尚未完成"));
        check(text(off,reports,true,false,"",LensStatus.Phase.UNKNOWN).equals("需要重開遊戲"));
        check(text(off,reports,true,false,"Root",LensStatus.Phase.OFF).equals("核對未通過"));
        check(DisplayOverview.text(on,reports,true,false,false,"","","version",LensStatus.Phase.ON).contains("不支援"));
        check(DisplayOverview.text(on,reports,true,true,false,"saving","",null,LensStatus.Phase.ON).contains("正在處理"));
        check(DisplayGroups.checkedReport(0,true,owner,raw.replace("\t3\t0","\t0\t0"),now).effect==DisplayGroups.Effect.WAITING);
        String result="LENS_GROUP\t0\toff\nLENS_GROUP\t1\toff\nLENS_GROUP\t2\toff\nLENS_GROUP\t3\toff\nLENS_ALL_OFF_COMMITTED\n";
        check(DisplayGroups.allOffSaved(0,true,result));
        for(String bad:new String[]{"",result.replace("off","on"),result.replace("LENS_ALL_OFF_COMMITTED\n",""),result+"extra\n",result.replace("\n","\r\n")})check(!DisplayGroups.allOffSaved(0,true,bad));
        check(!DisplayGroups.allOffSaved(0,false,result));check(!DisplayGroups.allOffSaved(26,true,result));
        System.out.println("DisplayOverviewTest: "+checks+" status and all-OFF acknowledgement checks; not game acceptance");
    }
}
