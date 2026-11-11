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
    private WeakReference<Observer> observer=new WeakReference<>(null);
    LensStatus state=LensStatus.unverified();
    String raw="尚無操作紀錄。",progress="",reason="";
    LensContext barrier;
    boolean unscopedBarrier;
    final Boolean[] groupOn=new Boolean[4];
    final String[] groupReport={"尚未核對","尚未核對","尚未核對","尚未核對"};
    LensContext autoAttempt;
    LensStatus.Phase barrierPhase=LensStatus.Phase.UNKNOWN;
    private LensController(Context context){
        operations=new LensOperations(context);preferences=context.getApplicationContext().getSharedPreferences("lens-interface",Context.MODE_PRIVATE);
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
    }
    void attach(Observer value){observer=new WeakReference<>(value);notifyView();}
    void detach(Observer value){if(observer.get()==value)observer.clear();}
    private void notifyView(){Observer value=observer.get();if(value!=null)value.changed();}
    boolean busy(){return active.get();}
    boolean groupsKnown(){for(Boolean value:groupOn)if(value==null)return false;return true;}
    boolean anyGroup(){for(Boolean value:groupOn)if(Boolean.TRUE.equals(value))return true;return false;}
    boolean armed(){try{return preferences.getBoolean("display-auto-arm",false);}catch(ClassCastException e){return false;}}
    boolean arm(){return preferences.edit().putBoolean("display-auto-arm",true).commit();}
    void refreshGroups(){
        if(!active.compareAndSet(false,true))return;
        progress="正在核對 Root 與設定…";notifyView();
        worker.execute(()->{
            Boolean[] found=new Boolean[4];String error="";
            for(int i=0;i<4;i++)try{
                LensOperations.Reply r=operations.displayGroup(i,null);found[i]=DisplayGroups.setting(i,r.exit,r.complete,r.output);
                if(found[i]==null){error="Root 或設定核對失敗（"+r.exit+"）；點此重試，不會當成已關閉。";break;}
            }catch(Exception e){error="Root 或設定讀取失敗；點此重試。";break;}
            final String problem=error;
            main.post(()->{
                System.arraycopy(found,0,groupOn,0,4);reason=problem;progress="";active.set(false);notifyView();
                if(groupsKnown()&&anyGroup()&&armed())LensService.start(operations.context);
                refreshReports();
            });
        });
    }
    void setGroup(int group,boolean enabled){
        if(group<0||group>3||!active.compareAndSet(false,true))return;
        progress="保存整組設定…";reason="";notifyView();
        worker.execute(()->{
            Boolean saved=null;String error="";
            try{LensOperations.Reply r=operations.displayGroup(group,enabled);saved=DisplayGroups.setting(group,r.exit,r.complete,r.output);
                if(saved==null)error="設定未完整同步（"+r.exit+"），點此依已保存意願重試。";
            }catch(Exception e){error="Root 或設定保存失敗；不推定開關已改變。";}
            final Boolean value=saved;final String problem=error;
            main.post(()->{
                groupOn[group]=value;groupReport[group]=value==null?"設定未確認":value?"已啟用，等待遊戲":"等待原顯示還原";
                reason=problem;progress="";active.set(false);notifyView();
                if(value!=null&&anyGroup()&&armed())LensService.start(operations.context);
                if(groupsKnown()&&!anyGroup())operations.context.stopService(new android.content.Intent(operations.context,LensService.class));
                refreshReports();
            });
        });
    }
    void refreshReports(){
        if(!groupsKnown()||!active.compareAndSet(false,true))return;
        worker.execute(()->{
            LensContext current=null;String raw=null;
            try{current=operations.readContext();if(current!=null){LensOperations.Reply r=operations.displayReports(current);if(r.exit==0&&r.complete)raw=r.output;}}catch(Exception ignored){}
            final LensContext owner=current;final String text=raw;final long now=System.currentTimeMillis();
            main.post(()->{for(int i=0;i<4;i++)groupReport[i]=DisplayGroups.report(i,groupOn[i],owner,text,now);active.set(false);notifyView();});
        });
    }
    void monitor(){
        if(!armed()||!groupsKnown()||!anyGroup()||!active.compareAndSet(false,true))return;
        worker.execute(()->{
            LensContext found=null;boolean front=false;String unsupported=operations.extensionAvailability();String error="";
            try{if(unsupported==null){found=operations.readContext();front=found!=null&&operations.gameForeground();}}catch(Exception denied){error="Root 或程序核對失敗，已停止等待；請回 App 重新核對。";}
            final String failure=error;
            final LensContext current=found;final boolean foreground=front;
            main.post(()->{
                active.set(false);
                if(!failure.isEmpty()){reason=failure;operations.context.stopService(new android.content.Intent(operations.context,LensService.class));notifyView();return;}
                if(unsupported!=null){reason=unsupported;notifyView();return;}
                if(current==null){reason="已啟用，等待開啟遊戲。";notifyView();return;}
                bindUnscopedBarrier(current);
                if(blockedFor(current)){reason="結果不明，請正常重開遊戲；不重送載入。";notifyView();return;}
                if(!foreground||autoAttempt!=null&&autoAttempt.same(current))return;
                autoAttempt=current;reason="";
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
        if(action==LensStatus.Action.ENABLE&&(consent==null||!failStop||blockedFor(consent))){
            reason="尚未取得本次啟用所需的確認。";notifyView();return;
        }
        if(!active.compareAndSet(false,true)){reason="前一操作尚未結束，請等候。";notifyView();return;}
        reason="";progress=action==LensStatus.Action.ENABLE?"正在回到遊戲並準備顯示…":action==LensStatus.Action.OFF?"正在關閉顯示…":"只讀核對中…";notifyView();
        worker.execute(()->{
            LensOperations.Result result=operations.execute(action,consent,foreground,()->main.post(()->{progress="原操作仍未返回，已鎖住重試，請等候。";notifyView();}));
            long now=System.currentTimeMillis();
            LensStatus parsed=result.blocked!=null?new LensStatus(LensStatus.Phase.UNSUPPORTED,null,now,false):
                LensStatus.parse(action,result.reply.exit,result.reply.output,result.reply.started,result.reply.complete,now,result.version,result.code,consent);
            main.post(()->{
                raw=result.blocked!=null?result.blocked:result.reply.output;
                reason=result.blocked==null?"":result.blocked;
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
                progress="";active.set(false);notifyView();
            });
        });
    }
}
