package dev.ody.seedpreview;
public final class DisplayGroupsTest {
    static int checks;
    static void check(boolean value){checks++;if(!value)throw new AssertionError("case "+checks);}
    static final String BOOT="baf6009f-3eca-45bf-a05d-086cf5c2d2fb";
    static String rows(long now){
        StringBuilder s=new StringBuilder("LENS_SESSION\t123\t456\t"+BOOT+"\n");
        for(int i=0;i<8;i++)s.append("LENS_MEMBER\t"+i+"\tv1\t"+now+"\t123\t157.0\tready\t1\t3\t0\t0\t0\t456\n");
        return s.append("LENS_REPORTS_END\t123\t456\n").toString();
    }
    public static void main(String[] args){
        check(DisplayGroups.MEMBERS.length==4);
        check(DisplayGroups.MEMBERS[0].length==3&&DisplayGroups.MEMBERS[1].length==2&&DisplayGroups.MEMBERS[2].length==2&&DisplayGroups.MEMBERS[3].length==1);
        check(DisplayGroups.MEMBERS[3][0].equals("nectar-stock-preview"));
        check(DisplayGroups.failure(1,"").contains("Root"));
        check(DisplayGroups.failure(6,"BLOCKED_STAGE_6\n").contains("遊戲版本"));
        check(DisplayGroups.failure(23,"BLOCKED_STAGE_23\n").contains("安裝檔"));
        check(DisplayGroups.failure(22,"BLOCKED_STAGE_22\n").contains("不支援"));
        check(DisplayGroups.failure(26,"").contains("設定檔"));
        for(int g=0;g<4;g++){
            check(Boolean.TRUE.equals(DisplayGroups.setting(g,0,true,"LENS_GROUP\t"+g+"\ton\n")));
            check(Boolean.FALSE.equals(DisplayGroups.setting(g,0,true,"LENS_GROUP\t"+g+"\toff\n")));
            for(String bad:new String[]{"off","","LENS_GROUP\t"+g+"\toff\r\n","LENS_GROUP\t9\toff\n"})check(DisplayGroups.setting(g,0,true,bad)==null);
            check(DisplayGroups.setting(g,1,true,"LENS_GROUP\t"+g+"\toff\n")==null);
        }
        LensContext owner=LensContext.parse("123\t456\t"+BOOT+"\n");long now=10000;
        for(int g=0;g<4;g++){
            check(DisplayGroups.report(g,true,owner,rows(now),now).equals("已啟用"));
            check(!DisplayGroups.report(g,true,owner,rows(now-5001),now).equals("已啟用"));
            check(!DisplayGroups.report(g,true,owner,rows(now).replace("\t456","\t457"),now).equals("已啟用"));
            check(!DisplayGroups.report(g,true,owner,rows(now).replace("157.0","156.1"),now).equals("已啟用"));
            check(!DisplayGroups.report(g,true,owner,rows(now).replace("\t3\t0","\t-1\t0"),now).equals("已啟用"));
            check(DisplayGroups.report(g,false,owner,rows(now),now).equals("等待原顯示還原"));
        }
        check(!DisplayGroups.report(0,true,owner,rows(now).replace("LENS_MEMBER\t2\t","LENS_MEMBER\t1\t"),now).equals("已啟用"));
        check(DisplayGroups.report(3,true,owner,rows(now).replace("ready","waiting-cards"),now).contains("等待"));
        System.out.println("DisplayGroupsTest: "+checks+" host protocol checks; not game acceptance");
    }
}
