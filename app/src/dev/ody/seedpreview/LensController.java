package dev.ody.seedpreview;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Looper;
import java.lang.ref.WeakReference;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

/** Process-owned gate survives Activity changes; consents belong to the view. */
final class LensController {
    interface Observer { void changed(); }
    interface Confirmation { void finished(LensContext context,boolean blocked); }
    private static LensController instance;
    static synchronized LensController get(Context context){if(instance==null)instance=new LensController(context);return instance;}
    final LensOperations operations;
    final SharedPreferences preferences;
    final ExecutorService worker=Executors.newSingleThreadExecutor();
    final Handler main=new Handler(Looper.getMainLooper());
    final AtomicBoolean active=new AtomicBoolean();
    final AtomicBoolean reportReading=new AtomicBoolean();
    private WeakReference<Observer> observer=new WeakReference<>(null);
    LensStatus state=LensStatus.unverified();
    String raw="尚無操作紀錄。",progress="",reason="";
    LensContext barrier;
    boolean unscopedBarrier;
    final Boolean[] groupOn=new Boolean[4];
    Boolean gpsOn;String gpsRaw,gpsText="尚未核對";
    final String[] groupReport={"尚未核對","尚未核對","尚未核對","尚未核對"};
    LensContext reportContext;
    String reportRaw;
    final DisplayGroups.Report[] effects=new DisplayGroups.Report[4];
    boolean reportReadFailed,contextChecked,closeQueued;
    // App-owned intentions survive unavailable Root/game and are never replaced by reports.
    final boolean[] intentPresent=new boolean[5];
    volatile long intentRevision;
    boolean settingsDirty=true;
    long syncedRevision=-1,syncRetryAt;
    String settingsError="",eligibility="";
    LensContext autoAttempt;
    LensStatus.Phase barrierPhase=LensStatus.Phase.UNKNOWN;
    private LensController(Context context){
        operations=new LensOperations(context);preferences=context.getApplicationContext().getSharedPreferences("lens-interface",Context.MODE_PRIVATE);
        for(int i=0;i<5;i++){
            intentPresent[i]=preferences.contains(intentKey(i));
            if(intentPresent[i])try{wanted(i,preferences.getBoolean(intentKey(i),false));}catch(ClassCastException invalid){wanted(i,null);}
        }
        boolean current=preferences.contains("retry-barrier-v2"),legacyUncertain=false;
        try{legacyUncertain=preferences.getBoolean("ui-uncertain",false);}catch(ClassCastException ignored){}
        barrier=LensContext.preference(storedText("retry-barrier-v2"),current?null:storedText("retry-barrier"));
        barrierPhase=LensStatus.savedBarrierPhase(storedText("retry-barrier-phase"),legacyUncertain);
        if(barrier!=null){
            state=new LensStatus(barrierPhase,barrier,0,false);
            if(!current)persistBarrier();
        }else if(current||preferences.contains("retry-barrier")||preferences.contains("retry-barrier-phase")||legacyUncertain){
            unscopedBarrier=true;barrierPhase=LensStatus.Phase.UNCERTAIN;
            state=new LensStatus(barrierPhase,null,0,false);
        }
    }
    private String storedText(String key){try{return preferences.getString(key,null);}catch(ClassCastException malformed){return null;}}
    private void persistBarrier(){
        if(barrier==null)return;
        preferences.edit().putString("retry-barrier-v2",barrier.preferenceRecord()).putString("retry-barrier-phase",barrierPhase.name()).putBoolean("ui-uncertain",barrierPhase!=LensStatus.Phase.UNKNOWN).apply();
    }
    private void bindUnscopedBarrier(LensContext current){
        if(unscopedBarrier&&current!=null){
            barrier=current;barrierPhase=LensStatus.Phase.UNCERTAIN;unscopedBarrier=false;
            state=new LensStatus(barrierPhase,current,System.currentTimeMillis(),false);persistBarrier();
        }
        state=LensStatus.forCurrentSession(state,current);
    }
    void attach(Observer value){observer=new WeakReference<>(value);notifyView();}
    void detach(Observer value){if(observer.get()==value)observer.clear();}
    private void notifyView(){Observer value=observer.get();if(value!=null)value.changed();}
    boolean busy(){return active.get();}
    boolean groupsKnown(){for(Boolean value:groupOn)if(value==null)return false;return gpsOn!=null;}
    boolean anyGroup(){for(Boolean value:groupOn)if(Boolean.TRUE.equals(value))return true;return Boolean.TRUE.equals(gpsOn);}
    boolean armed(){try{return preferences.getBoolean("display-auto-arm",false);}catch(ClassCastException e){return false;}}
    boolean pendingOff(){try{return preferences.getBoolean("all-off-pending",false);}catch(ClassCastException invalid){return true;}}
    static String intentKey(int i){return "feature-intent-v1-"+i;}
    Boolean wanted(int i){return i<4?groupOn[i]:gpsOn;}
    void wanted(int i,Boolean value){if(i<4)groupOn[i]=value;else gpsOn=value;}
    boolean hasIntent(){for(boolean value:intentPresent)if(value)return true;return false;}
    boolean needsWatch(){return pendingOff()||anyGroup()&&armed()||settingsDirty&&hasIntent();}
    boolean arm(){
        if(!preferences.edit().putBoolean("display-auto-arm",true).commit())return false;
        ++intentRevision;settingsDirty=true;syncRetryAt=0;wake();kick();return true;
    }
    void setGroup(int group,boolean enabled){if(group>=0&&group<4)saveIntent(group,enabled);}
    void setGps(boolean enabled){saveIntent(4,enabled);}
    private void saveIntent(int i,boolean enabled){
        if(!preferences.edit().putBoolean(intentKey(i),enabled).commit()){
            reason="選擇未保存，請重試。";notifyView();return;
        }
        wanted(i,enabled);intentPresent[i]=true;++intentRevision;
        settingsDirty=true;settingsError="";syncRetryAt=0;reason="";
        if(i==4)gpsRaw=null;
        DiagnosticStore.record(operations.context,"GROUP_SAVED");
        notifyView();wake();kick();
    }
    private void wake(){
        if(needsWatch())LensService.start(operations.context);
        else operations.context.stopService(new android.content.Intent(operations.context,LensService.class));
    }
    void tick(){
        String current=String.valueOf(operations.extensionAvailability())+"|"+String.valueOf(operations.gpsAvailability())+"|"+armed();
        if(!current.equals(eligibility)){eligibility=current;++intentRevision;settingsDirty=true;syncRetryAt=0;}
        if(pendingOff()||settingsDirty){if(System.currentTimeMillis()>=syncRetryAt)kick();return;}
        if(anyGroup()&&armed())monitor();
    }
    private void kick(){
        if(active.get())return;
        if(pendingOff()){resumeAllOff();return;}
        if(settingsDirty&&System.currentTimeMillis()>=syncRetryAt)syncSettings();
    }
    void refreshGroups(){syncRetryAt=0;settingsDirty=true;kick();}
    private void syncSettings(){
        if(!active.compareAndSet(false,true))return;
        final long revision=intentRevision;
        final Boolean[] intentions={groupOn[0],groupOn[1],groupOn[2],groupOn[3],gpsOn};
        final boolean[] present=intentPresent.clone();
        final boolean consent=armed();
        progress="正在核對套用條件…";notifyView();
        worker.execute(()->{
            Boolean[] imported=new Boolean[5];String error="";
            String displayBlock=operations.extensionAvailability(),gpsBlock=operations.gpsAvailability();
            for(int i=0;i<5;i++){
                if(intentRevision!=revision)break; // Do not fan out an obsolete queued ON.
                if(intentions[i]==null&&present[i]){error="設定無法讀取，請重新選擇開關。";continue;}
                Boolean target=intentions[i]==null?null:intentions[i]&&consent&&(i==4?gpsBlock:displayBlock)==null;
                try{
                    LensOperations.Reply r=i==4?operations.gpsCopy(target):operations.displayGroup(i,target);
                    Boolean saved=i==4?GpsCopy.setting(r.exit,r.complete,r.output):DisplayGroups.setting(i,r.exit,r.complete,r.output);
                    if(saved==null||target!=null&&!target.equals(saved)){error="等待 Root 授權或設定同步；點此重試。";break;}
                    if(intentions[i]==null)imported[i]=saved;
                }catch(Exception unavailable){error="等待 Root 授權或設定同步；點此重試。";break;}
            }
            final String problem=error;
            main.post(()->{
                active.set(false);progress="";
                if(revision!=intentRevision||pendingOff()){kick();return;}
                boolean migration=false;android.content.SharedPreferences.Editor edit=preferences.edit();
                for(int i=0;i<5;i++)if(!intentPresent[i]&&imported[i]!=null){edit.putBoolean(intentKey(i),imported[i]);migration=true;}
                if(migration){
                    if(edit.commit()){
                        for(int i=0;i<5;i++)if(!intentPresent[i]&&imported[i]!=null){wanted(i,imported[i]);intentPresent[i]=true;}
                        ++intentRevision;settingsDirty=true;
                    }else{settingsError="舊設定尚未保存，請重試。";syncRetryAt=System.currentTimeMillis()+15000;notifyView();wake();return;}
                }
                settingsError=problem;settingsDirty=migration||!problem.isEmpty()||!groupsKnown();
                if(!settingsDirty)syncedRevision=intentRevision;
                syncRetryAt=problem.isEmpty()?0:System.currentTimeMillis()+15000;
                updateReports(System.currentTimeMillis());notifyView();wake();
                if(migration&&problem.isEmpty())kick();else refreshReports();
            });
        });
    }
    void allOff(){
        android.content.SharedPreferences.Editor edit=preferences.edit().putBoolean("all-off-pending",true).putBoolean("display-auto-arm",false);
        for(int i=0;i<5;i++)edit.putBoolean(intentKey(i),false);
        if(!edit.commit()){reason="關閉意願未保存，尚未關閉全部；請重試。";notifyView();return;}
        for(int i=0;i<5;i++){wanted(i,false);intentPresent[i]=true;}
        ++intentRevision;settingsDirty=true;syncRetryAt=0;settingsError="";reason="";gpsRaw=null;reportRaw=null;
        DiagnosticStore.record(operations.context,"ALL_OFF_BEGIN");notifyView();wake();kick();
    }
    private void resumeAllOff(){
        if(!active.compareAndSet(false,true))return;
        closeQueued=true;progress="正在同步全部關閉…";notifyView();
        worker.execute(()->{
            boolean committed=false;
            try{LensOperations.Reply r=operations.allOff();committed=DisplayGroups.allOffSaved(r.exit,r.complete,r.output);}catch(Exception ignored){}
            final boolean saved=committed;
            main.post(()->{
                boolean complete=saved&&preferences.edit().putBoolean("all-off-pending",false).commit();
                DiagnosticStore.record(operations.context,complete?"ALL_OFF_SAVED":"ALL_OFF_FAILED");
                // Later user selections survive this older OFF acknowledgement.
                settingsError=complete?"":"已保存關閉，等待 Root 確認；點此重試。";
                syncRetryAt=complete?0:System.currentTimeMillis()+15000;
                settingsDirty=true;closeQueued=false;active.set(false);progress="";notifyView();wake();
                if(complete)kick();
            });
        });
    }
    private String intentStatus(int i){
        Boolean on=wanted(i);if(on==null)return intentPresent[i]?"設定無法讀取，可重新選擇":"舊設定待核對，可直接選擇";
        String prefix=on?"已開啟，":"已關閉，";
        if(pendingOff())return prefix+"等待完成關閉同步";
        if(!settingsError.isEmpty())return prefix+settingsError;
        String unavailable=i==4?operations.gpsAvailability():operations.extensionAvailability();
        if(on&&unavailable!=null)return prefix+"暫不套用："+unavailable;
        if(on&&!armed())return prefix+"等待首次套用確認";
        if(on&&reportReadFailed)return prefix+"等待 Root 或遊戲狀態核對";
        if(settingsDirty||syncedRevision!=intentRevision)return prefix+"等待同步";
        if(on&&blockedFor(reportContext)&&!gameAbsent())return prefix+"需要正常重開遊戲";
        if(on&&gameAbsent())return prefix+"等待遊戲";
        return null;
    }
    void updateReports(long now){
        gpsText=intentStatus(4);if(gpsText==null)gpsText=GpsCopy.report(gpsOn,reportContext,gpsRaw,now);
        for(int i=0;i<4;i++){
            effects[i]=groupOn[i]==null?null:DisplayGroups.checkedReport(i,groupOn[i],reportContext,reportRaw,now);
            String waiting=intentStatus(i);groupReport[i]=waiting!=null?waiting:effects[i]==null?"設定未確認":effects[i].text;
        }
    }
    boolean gameAbsent(){return contextChecked&&!reportReadFailed&&reportContext==null;}
    String overview(){
        updateReports(System.currentTimeMillis());
        if(pendingOff())return "關閉意願已保存，等待同步";
        if(!settingsError.isEmpty())return "選擇已保存，等待 Root／設定同步";
        if(anyGroup()){
            if(reportReadFailed)return "已開啟，等待 Root／遊戲核對";
            if(operations.extensionAvailability()!=null)return "已開啟，等待相容環境";
            if(!armed())return "已開啟，等待首次確認";
            if(blockedFor(reportContext)&&!gameAbsent())return "已開啟，需要正常重開遊戲";
            if(settingsDirty)return "選擇已保存，等待套用";
            if(gameAbsent())return "已開啟，等待遊戲";
            boolean display=false;for(Boolean value:groupOn)display|=Boolean.TRUE.equals(value);
            if(!display&&Boolean.TRUE.equals(gpsOn))return gpsText;
        }
        return DisplayOverview.text(groupOn,effects,reportContext!=null,false,false,"",reportReadFailed?"read-failed":"",null,gameAbsent()?LensStatus.Phase.UNVERIFIED:state.phase);
    }
    void refreshReports(){
        // Passive reads share the serial worker without disabling user switches.
        // Only mutations own active; this read never changes saved intentions.
        if(settingsDirty||!groupsKnown()||operations.extensionAvailability()!=null||active.get()||!reportReading.compareAndSet(false,true))return;
        worker.execute(()->{
            LensContext current=null;String raw=null,gps=null;boolean failed=false;
            try{current=operations.readContext();if(current!=null){LensOperations.Reply r=operations.displayReports(current);if(r.exit==0&&r.complete){
                String header=r.output.startsWith("LENS_SESSION\t")&&r.output.indexOf('\n')>=0?r.output.substring(0,r.output.indexOf('\n')+1):"";
                StringBuilder display=new StringBuilder();for(String line:r.output.split("\n")){if(line.startsWith("LENS_GPS_REPORT\t")){if(gps!=null)throw new IllegalStateException("duplicate GPS report");gps=header+line.substring("LENS_GPS_REPORT\t".length())+"\n";}else display.append(line).append('\n');}raw=display.toString();
            }else failed=true;}}catch(Exception ignored){failed=true;raw=null;gps=null;}
            final String gpsResult=gps;
            final LensContext owner=current;final String text=raw;final boolean failure=failed;
            main.post(()->{reportContext=owner;reportRaw=text;gpsRaw=gpsResult;reportReadFailed=failure;contextChecked=true;bindUnscopedBarrier(owner);updateReports(System.currentTimeMillis());reportReading.set(false);notifyView();});
        });
    }
    void monitor(){
        if(pendingOff()||settingsDirty||!armed()||!groupsKnown()||!anyGroup()||!active.compareAndSet(false,true))return;
        worker.execute(()->{
            LensContext found=null;boolean front=false;String unsupported=operations.extensionAvailability();String error="";
            try{if(unsupported==null){found=operations.readContext();front=found!=null&&operations.gameForeground();}}catch(Exception denied){error="等待 Root 或遊戲程序核對；將自動重試。";}
            final String failure=error;
            final LensContext current=found;final boolean foreground=front;
            main.post(()->{
                active.set(false);
                if(pendingOff()||settingsDirty||!armed()||!anyGroup()){kick();return;}
                if(!failure.isEmpty()){reason=failure;reportReadFailed=true;contextChecked=false;notifyView();return;}
                if(unsupported!=null){reason=unsupported;notifyView();return;}
                if(reason.startsWith("等待 Root 或遊戲程序核對"))reason="";
                if(current==null){
                    reportContext=null;reportRaw=null;gpsRaw=null;contextChecked=true;reportReadFailed=false;
                    // Do not erase the last load failure when that game exits.
                    if(state.phase!=LensStatus.Phase.REFUSED&&state.phase!=LensStatus.Phase.UNVERIFIED)
                        reason="";
                    else if(reason.isEmpty())reason="";
                    notifyView();return;
                }
                bindUnscopedBarrier(current);
                if(blockedFor(current)){reason="結果不明，請正常重開遊戲；不重送載入。";notifyView();return;}
                reportContext=current;contextChecked=true;reportReadFailed=false;
                boolean runnable=false;for(Boolean on:groupOn)runnable|=Boolean.TRUE.equals(on);
                runnable|=Boolean.TRUE.equals(gpsOn)&&operations.gpsAvailability()==null;
                if(!runnable||!foreground||autoAttempt!=null&&autoAttempt.same(current)){notifyView();return;}
                reportContext=current;reportRaw=null;reason="";
                execute(LensStatus.Action.ENABLE,current,true,()->operations.gameForeground());
            });
        });
    }
    boolean blockedFor(LensContext context){return unscopedBarrier||barrier!=null&&(context==null||barrier.same(context));}
    void confirm(Confirmation callback){
        if(!active.compareAndSet(false,true)){callback.finished(null,true);return;}
        reason="";progress="正在核對當次遊戲程序…";notifyView();
        worker.execute(()->{
            LensContext found=null;try{found=operations.readContext();}catch(Exception ignored){}
            final LensContext current=found;
            main.post(()->{
                active.set(false);progress="";
                bindUnscopedBarrier(current);boolean blocked=blockedFor(current);
                if(current!=null&&barrier!=null&&!barrier.same(current)&&state.blocksEnable())state=new LensStatus(LensStatus.Phase.UNVERIFIED,current,System.currentTimeMillis(),false);
                notifyView();callback.finished(current,blocked);
            });
        });
    }
    void execute(LensStatus.Action action,LensContext consent,boolean failStop,LensOperations.Foreground foreground){
        if(action==LensStatus.Action.ENABLE&&(pendingOff()||settingsDirty||!anyGroup()||!armed()||consent==null||!failStop||blockedFor(consent))){
            reason="尚未取得本次啟用所需的確認。";notifyView();return;
        }
        if(!active.compareAndSet(false,true)){reason="前一操作尚未結束，請等候。";notifyView();return;}
        final UpdateInterlock.Lease updateLease=action==LensStatus.Action.ENABLE?UpdateInterlock.beginLoad(operations.context.getFilesDir(),LensUpdater.installed(operations.context)):null;
        if(action==LensStatus.Action.ENABLE&&updateLease==null){reason="更新準備中或前一操作尚未結束，請先完成／取消更新。";active.set(false);notifyView();return;}
        reason="";progress=action==LensStatus.Action.ENABLE?"正在回到遊戲並準備顯示…":action==LensStatus.Action.OFF?"正在關閉顯示…":"只讀核對中…";notifyView();
        if(action==LensStatus.Action.ENABLE){autoAttempt=consent;DiagnosticStore.record(operations.context,"ENABLE_BEGIN");}
        worker.execute(()->{
            LensOperations.Result result;
            try{result=operations.execute(action,consent,foreground,()->main.post(()->{progress="原操作仍未返回，已鎖住重試，請等候。";notifyView();}));}
            finally{if(updateLease!=null)updateLease.close();}
            long now=System.currentTimeMillis();
            LensStatus parsed=result.blocked!=null?new LensStatus(LensStatus.Phase.UNSUPPORTED,null,now,false):
                LensStatus.parse(action,result.reply.exit,result.reply.output,result.reply.started,result.reply.complete,now,result.version,result.code,consent);
            main.post(()->{
                raw=result.blocked!=null?result.blocked:result.reply.output;
                reason=result.blocked==null?"":result.blocked;
                if(result.blocked==null&&(parsed.phase==LensStatus.Phase.REFUSED||parsed.phase==LensStatus.Phase.UNVERIFIED))
                    reason=DisplayGroups.failure(result.reply.exit,result.reply.output);
                bindUnscopedBarrier(parsed.session);
                if(parsed.phase==LensStatus.Phase.UNKNOWN||parsed.phase==LensStatus.Phase.UNCERTAIN||parsed.phase==LensStatus.Phase.RESTART_REQUIRED){
                    if(parsed.session!=null){
                        barrier=parsed.session;barrierPhase=parsed.phase;
                        unscopedBarrier=false;persistBarrier();
                    }
                }
                // OFF and read errors never erase a barrier for the same
                // process. A fresh boot/PID/start is required to proceed.
                state=LensStatus.retainingBarrier(parsed,barrier,barrierPhase,unscopedBarrier);
                if(action==LensStatus.Action.ENABLE){
                    DiagnosticStore.record(operations.context,"ENABLE_"+state.phase.name());
                    if(result.reply!=null)DiagnosticStore.record(operations.context,result.reply.exit<0?"LOADER_QUERY_FAILED":"LOADER_EXIT_"+result.reply.exit);
                }
                progress="";active.set(false);notifyView();kick();refreshReports();
            });
        });
    }
}
