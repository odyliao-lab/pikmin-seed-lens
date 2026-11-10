package dev.ody.seedpreview;

/** Presentation only. No shell calls, native writes, retry or support inference. */
final class LensStatus {
    enum Action { READ, ENABLE, OFF }
    enum Phase { UNVERIFIED, WAITING, ON, OFF, REFUSED, UNKNOWN, UNCERTAIN, RESTART_REQUIRED, UNSUPPORTED }
    final Phase phase;
    final LensContext session;
    final long observedAt;
    final boolean controlOn;
    LensStatus(Phase phase,LensContext session,long at,boolean on){
        this.phase=phase;this.session=session;this.observedAt=at;this.controlOn=on;
    }
    static LensStatus unverified(){return new LensStatus(Phase.UNVERIFIED,null,0,false);}
    boolean blocksEnable(){return phase==Phase.UNKNOWN||phase==Phase.UNCERTAIN||phase==Phase.RESTART_REQUIRED||phase==Phase.UNSUPPORTED;}
    static Phase savedBarrierPhase(String value,boolean legacyUncertain){
        if(Phase.RESTART_REQUIRED.name().equals(value))return Phase.RESTART_REQUIRED;
        if(Phase.UNCERTAIN.name().equals(value))return Phase.UNCERTAIN;
        if(Phase.UNKNOWN.name().equals(value))return Phase.UNKNOWN;
        return legacyUncertain?Phase.UNCERTAIN:Phase.UNKNOWN;
    }
    static LensStatus retainingBarrier(LensStatus value,LensContext barrier,Phase phase){
        if(barrier!=null&&(value.session==null||barrier.same(value.session)))
            return new LensStatus(phase,barrier,value.observedAt,false);
        return value;
    }
    static String sessionHeader(){return "LENS_SESSION\t";}
    private static boolean unknownLine(String line){
        return line.equals("UNKNOWN_ATTEMPT_PRESERVED")||
            line.equals("UNKNOWN_PRESERVED_EXACT_GAME_STOPPED_REOPEN_REQUIRED")||
            line.equals("UNKNOWN_CONTAINMENT_UNCONFIRMED_DO_NOT_RETRY");
    }
    private static boolean noRetryLine(String line){
        return line.equals("NATIVEBRIDGE_REFUSED_LEFT_OFF_NO_RETRY")||line.equals("ARM64_REFUSED_LEFT_OFF_NO_RETRY")||
            line.equals("OTHER_RENDERER_REQUIRES_GAME_RESTART");
    }
    private static boolean safeNativeRefusal(int exit,String[] lines,LensContext session,LensContext submitted){
        if(session==null||submitted==null||!submitted.same(session))return false;
        switch(exit){case 2:case 3:case 6:case 22:case 23:case 24:case 25:case 28:break;default:return false;}
        int blocked=0;
        for(int i=1;i<lines.length-1;i++){
            String line=lines[i];
            if(line.equals("BLOCKED_STAGE_"+exit)){blocked++;continue;}
            if((exit==25||exit==28)&&(line.equals("NATIVEBRIDGE_HOST_RETURNED_NOT_READY")||line.equals("ARM64_RETURNED_REQUIRES_STATUS")))continue;
            if(exit==22&&line.equals("EXPLICIT_FAIL_STOP_CONSENT_REQUIRED"))continue;
            if(exit==26&&line.equals("OTHER_RENDERER_REQUIRES_GAME_RESTART"))continue;
            if(exit==28&&line.equals("NO_FRESH_RENDERER_LEFT_OFF"))continue;
            return false;
        }
        return blocked==1;
    }
    private static long decimal(String value){
        if(value==null||!value.matches("[0-9]{1,19}"))return -1;
        try{return Long.parseLong(value);}catch(NumberFormatException e){return -1;}
    }
    private static boolean renderer(String row,LensContext session,long now,String version,long code){
        if(session==null||!Compatibility.exactProfile(version,code)||row.length()>512)return false;
        String[] f=row.split("\t",-1);
        if(f.length!=11||!f[0].equals("v1")||!f[2].equals(Integer.toString(session.game.pid))||
            !f[3].equals(version)||!f[10].equals(Long.toString(session.game.start))||
            !(f[4].equals("ready")||f[4].equals("waiting-cards")))return false;
        long stamp=decimal(f[1]);
        if(stamp<0||now<0||stamp>now+2000||stamp<now-5000)return false;
        for(int i=5;i<=9;i++){
            long value=decimal(f[i]);
            if(value<0||value>(i==5?1:4294967295L))return false;
        }
        return true;
    }
    static LensStatus parse(Action action,int exit,String output,boolean started,boolean complete,
            long now,String version,long code,LensContext submitted){
        if(output==null)output="";
        String[] lines=output.split("\n",-1);
        LensContext session=lines.length>1&&lines[0].startsWith(sessionHeader())?
            LensContext.parse(lines[0].substring(sessionHeader().length())+"\n"):null;
        boolean nativeUnknown=false,noRetry=false;
        for(String line:lines){nativeUnknown|=unknownLine(line);noRetry|=noRetryLine(line);}
        LensContext attempted=action==Action.ENABLE&&submitted!=null?submitted:session;
        if(nativeUnknown&&(session!=null||submitted!=null))
            return new LensStatus(Phase.UNKNOWN,action==Action.ENABLE&&submitted!=null?submitted:session!=null?session:submitted,now,false);
        if(noRetry&&attempted!=null)return new LensStatus(Phase.RESTART_REQUIRED,attempted,now,false);
        if(!started)return unverified();
        if(!complete||output.length()>16384||output.indexOf('\r')>=0||!output.endsWith("\n"))
            return new LensStatus(action==Action.ENABLE?Phase.UNCERTAIN:Phase.UNVERIFIED,submitted,now,false);
        if(exit!=0){
            if(action==Action.ENABLE&&submitted!=null){
                if(session!=null){
                    if(exit==27||exit==42||exit==43)return new LensStatus(Phase.UNKNOWN,submitted,now,false);
                    if(exit==9||exit==11||exit==26||exit==40||exit==41||exit==44||exit==45)
                        return new LensStatus(Phase.RESTART_REQUIRED,submitted,now,false);
                    if(!safeNativeRefusal(exit,lines,session,submitted))return new LensStatus(Phase.UNCERTAIN,submitted,now,false);
                }else if(exit<0||exit>=128)return new LensStatus(Phase.UNCERTAIN,submitted,now,false);
            }
            return new LensStatus(Phase.REFUSED,session,now,false);
        }
        int headers=0,commitsOn=0,commitsOff=0,rows=0;String row=null;
        for(String line:lines){
            if(line.startsWith(sessionHeader()))headers++;
            if(line.equals("ON_COMMITTED_REQUIRES_UI_ACCEPTANCE"))commitsOn++;
            if(line.equals("OFF_COMMITTED_REQUIRES_UI_RESTORE"))commitsOff++;
            if(line.startsWith("v1\t")){rows++;row=line;}
        }
        if(headers!=1||session==null||(action==Action.ENABLE&&(submitted==null||!submitted.same(session))))
            return new LensStatus(action==Action.ENABLE?Phase.UNCERTAIN:Phase.UNVERIFIED,submitted,now,false);
        if(action==Action.OFF){
            if(commitsOff==1&&commitsOn==0&&rows==0&&lines.length==3)
                return new LensStatus(Phase.OFF,session,now,false);
            return unverified();
        }
        boolean valid=rows==1&&renderer(row,session,now,version,code);
        if(action==Action.ENABLE){
            // The emitted status precedes the native ON commit. Never promote
            // this acknowledgment to a verified rendered ON state.
            if(commitsOn==1&&commitsOff==0&&valid)
                return new LensStatus(Phase.WAITING,session,now,true);
            return new LensStatus(Phase.UNCERTAIN,session,now,false);
        }
        if(commitsOn!=0||commitsOff!=0||!valid||lines.length!=3)return new LensStatus(Phase.UNVERIFIED,session,now,false);
        String[] f=row.split("\t",-1);
        if(f[5].equals("0"))return new LensStatus(Phase.OFF,session,now,false);
        return new LensStatus(f[4].equals("ready")?Phase.ON:Phase.WAITING,session,now,true);
    }
}
