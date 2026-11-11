package dev.ody.seedpreview;

public final class LensStatusTest {
    static int checks;
    static final String B="11111111-2222-3333-4444-555555555555";
    static final String H="LENS_SESSION\t4321\t9876\t"+B+"\n";
    static final String R="v1\t100000\t4321\t157.0\tready\t1\t5\t3\t42\t7\t9876\n";
    static final String ON="ON_COMMITTED_REQUIRES_UI_ACCEPTANCE\n";
    static final String OFF="OFF_COMMITTED_REQUIRES_UI_RESTORE\n";
    static final LensContext S=LensContext.parse("4321\t9876\t"+B+"\n");
    static LensStatus parse(LensStatus.Action a,int exit,String text,boolean complete){return LensStatus.parse(a,exit,text,true,complete,100000,"157.0",1793360009L,S);}
    static void expect(LensStatus.Phase want,LensStatus got){++checks;if(got.phase!=want)throw new AssertionError("status case "+checks+": "+got.phase);}
    public static void main(String[] args){
        LensStatus.Action read=LensStatus.Action.READ,on=LensStatus.Action.ENABLE,off=LensStatus.Action.OFF;
        expect(LensStatus.Phase.UNVERIFIED,LensStatus.unverified());
        expect(LensStatus.Phase.ON,parse(read,0,H+R,true));
        expect(LensStatus.Phase.WAITING,parse(read,0,H+R.replace("ready","waiting-cards"),true));
        expect(LensStatus.Phase.OFF,parse(read,0,H+R.replace("\t1\t5","\t0\t5"),true));
        expect(LensStatus.Phase.WAITING,parse(on,0,H+ON+R,true));
        expect(LensStatus.Phase.WAITING,parse(on,0,H+ON+R.replace("\t1\t5","\t0\t5"),true));
        expect(LensStatus.Phase.OFF,parse(off,0,H+OFF,true));
        expect(LensStatus.Phase.UNVERIFIED,parse(read,0,H+"NO_FRESH_SAME_PID_START_STATUS\n",true));
        if(!S.same(parse(read,0,H+"NO_FRESH_SAME_PID_START_STATUS\n",true).session))throw new AssertionError("new-process observation lost");++checks;
        expect(LensStatus.Phase.REFUSED,parse(on,13,"Permission denied\n",true));
        expect(LensStatus.Phase.REFUSED,parse(on,6,H+"BLOCKED_STAGE_6\n",true));
        expect(LensStatus.Phase.RESTART_REQUIRED,parse(on,26,H+"OTHER_RENDERER_REQUIRES_GAME_RESTART\nBLOCKED_STAGE_26\n",true));
        expect(LensStatus.Phase.UNCERTAIN,parse(on,4,H+"BLOCKED_STAGE_4\n",true));
        expect(LensStatus.Phase.UNCERTAIN,parse(on,6,H.replace("9876","9877")+"BLOCKED_STAGE_6\n",true));
        expect(LensStatus.Phase.REFUSED,parse(on,28,H+"NATIVEBRIDGE_HOST_RETURNED_NOT_READY\nNO_FRESH_RENDERER_LEFT_OFF\nBLOCKED_STAGE_28\n",true));
        for(int exit:new int[]{-1,137,139,143}){
            LensStatus killed=parse(on,exit,H+"Killed\n",true);
            expect(LensStatus.Phase.UNCERTAIN,killed);
            if(!killed.blocksEnable()||!S.same(killed.session))throw new AssertionError("abnormal exit lost retry barrier");++checks;
        }
        expect(LensStatus.Phase.UNCERTAIN,parse(on,137,"Killed\n",true));
        expect(LensStatus.Phase.UNCERTAIN,parse(on,137,H+ON+R,true));
        expect(LensStatus.Phase.UNCERTAIN,parse(on,6,H+ON+R+"BLOCKED_STAGE_6\n",true));
        expect(LensStatus.Phase.UNCERTAIN,parse(on,73,H+"Unexpected exit\n",true));
        expect(LensStatus.Phase.REFUSED,parse(read,139,H+"Segmentation fault\n",true));
        for(int exit:new int[]{9,11,40,41,44,45})expect(LensStatus.Phase.RESTART_REQUIRED,parse(on,exit,H+"BLOCKED_STAGE_"+exit+"\n",true));
        for(int exit:new int[]{27,42,43})expect(LensStatus.Phase.UNKNOWN,parse(on,exit,H+"Missing final marker\n",true));
        for(String marker:new String[]{"NATIVEBRIDGE_REFUSED_LEFT_OFF_NO_RETRY","ARM64_REFUSED_LEFT_OFF_NO_RETRY"}){
            expect(LensStatus.Phase.RESTART_REQUIRED,parse(on,44,H+marker+"\n",true));
            expect(LensStatus.Phase.RESTART_REQUIRED,parse(on,0,H+marker+"\n"+ON+R,true));
            expect(LensStatus.Phase.RESTART_REQUIRED,parse(off,0,H+marker+"\n"+OFF,true));
        }
        if(LensStatus.savedBarrierPhase("RESTART_REQUIRED",false)!=LensStatus.Phase.RESTART_REQUIRED||
            LensStatus.savedBarrierPhase("UNCERTAIN",false)!=LensStatus.Phase.UNCERTAIN||
            LensStatus.savedBarrierPhase("UNKNOWN",true)!=LensStatus.Phase.UNKNOWN||
            LensStatus.savedBarrierPhase(null,true)!=LensStatus.Phase.UNCERTAIN||
            LensStatus.savedBarrierPhase("ON",false)!=LensStatus.Phase.UNKNOWN)throw new AssertionError("barrier preference migration");++checks;
        expect(LensStatus.Phase.UNCERTAIN,parse(on,0,H+R,true));
        expect(LensStatus.Phase.UNCERTAIN,parse(on,0,H+ON+R,false));
        expect(LensStatus.Phase.UNVERIFIED,parse(read,0,H+R,false));
        expect(LensStatus.Phase.UNVERIFIED,LensStatus.parse(on,0,H+ON+R,false,true,100000,"157.0",1793360009L,S));
        for(String marker:new String[]{"UNKNOWN_ATTEMPT_PRESERVED","UNKNOWN_PRESERVED_EXACT_GAME_STOPPED_REOPEN_REQUIRED","UNKNOWN_CONTAINMENT_UNCONFIRMED_DO_NOT_RETRY"}){
            expect(LensStatus.Phase.UNKNOWN,parse(on,42,H+marker+"\n",true));
            expect(LensStatus.Phase.UNKNOWN,parse(off,0,H+marker+"\n"+OFF,true));
        }
        for(String wrong:new String[]{R.replace("100000","94999"),R.replace("100000","102001"),R.replace("4321","4322"),R.replace("9876","9877"),R.replace("157.0","157.1"),R.replace("\t1\t5","\t2\t5"),R.replace("\t5\t3","\t-1\t3"),R.replace("\t42\t7","\t4294967296\t7"),R.replace("\tready\t","\tready-now\t"),R+R,R.replace("\n","\r\n"),R.substring(0,R.length()-1)})
            expect(LensStatus.Phase.UNVERIFIED,parse(read,0,H+wrong,true));
        expect(LensStatus.Phase.UNVERIFIED,parse(read,0,H+H+R,true));
        expect(LensStatus.Phase.UNVERIFIED,parse(read,0,R,true));
        expect(LensStatus.Phase.UNVERIFIED,parse(read,0,H+ON+R,true));
        expect(LensStatus.Phase.UNVERIFIED,parse(off,0,OFF,true));
        expect(LensStatus.Phase.UNVERIFIED,parse(off,0,H+OFF+"extra\n",true));
        expect(LensStatus.Phase.REFUSED,parse(off,1,H+OFF,true));
        expect(LensStatus.Phase.UNCERTAIN,parse(on,0,H+"prefix"+ON+R,true));
        expect(LensStatus.Phase.UNCERTAIN,parse(on,0,H+ON+ON+R,true));
        expect(LensStatus.Phase.UNCERTAIN,LensStatus.parse(on,0,H+ON+R,true,true,100000,"157.0",1793360010L,S));
        expect(LensStatus.Phase.UNCERTAIN,LensStatus.parse(on,0,H+ON+R,true,true,100000,"157.0",1793360009L,LensContext.parse("4321\t9999\t"+B+"\n")));
        expect(LensStatus.Phase.UNVERIFIED,parse(read,0,H.replace(B,"broken")+R,true));
        expect(LensStatus.Phase.UNCERTAIN,parse(on,0,H.replace(B,"11111111-2222-3333-4444-555555555556")+ON+R,true));
        for(LensStatus.Phase barrier:new LensStatus.Phase[]{LensStatus.Phase.UNKNOWN,LensStatus.Phase.UNCERTAIN,LensStatus.Phase.RESTART_REQUIRED}){
            if(!new LensStatus(barrier,S,100000,false).blocksEnable())throw new AssertionError("retry barrier not blocking");++checks;
            expect(barrier,LensStatus.retainingBarrier(parse(off,0,H+OFF,true),S,barrier));
            expect(barrier,LensStatus.retainingBarrier(parse(read,0,H+R,true),S,barrier));
            expect(barrier,LensStatus.retainingBarrier(LensStatus.unverified(),S,barrier));
            LensContext next=LensContext.parse("4321\t9876\t11111111-2222-3333-4444-555555555556\n");
            expect(LensStatus.Phase.UNVERIFIED,LensStatus.retainingBarrier(new LensStatus(LensStatus.Phase.UNVERIFIED,next,100000,false),S,barrier));
            expect(LensStatus.Phase.UNVERIFIED,LensStatus.retainingBarrier(new LensStatus(LensStatus.Phase.UNVERIFIED,LensContext.parse("4322\t9877\t"+B+"\n"),100000,false),S,barrier));
        }
        for(LensStatus value:new LensStatus[]{parse(off,0,H+OFF,true),parse(read,0,H+R,true),LensStatus.unverified()}){
            LensStatus blocked=LensStatus.retainingBarrier(value,null,LensStatus.Phase.UNKNOWN,true);
            expect(LensStatus.Phase.UNCERTAIN,blocked);
            if(!blocked.blocksEnable()||blocked.controlOn)throw new AssertionError("unreadable stored scope lost barrier");++checks;
        }
        System.out.println("LensStatusTest: "+checks+" protocol/presentation cases passed; not device acceptance");
    }
}
