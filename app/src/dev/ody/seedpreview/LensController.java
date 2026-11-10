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
    LensStatus.Phase barrierPhase=LensStatus.Phase.UNKNOWN;
    private LensController(Context context){
        operations=new LensOperations(context);preferences=context.getApplicationContext().getSharedPreferences("lens-interface",Context.MODE_PRIVATE);
        barrier=LensContext.parse(preferences.getString("retry-barrier",""));
        if(preferences.getBoolean("ui-uncertain",false))barrierPhase=LensStatus.Phase.UNCERTAIN;
        if(barrier!=null)state=new LensStatus(barrierPhase,barrier,0,false);
    }
    void attach(Observer value){observer=new WeakReference<>(value);notifyView();}
    void detach(Observer value){if(observer.get()==value)observer.clear();}
    private void notifyView(){Observer value=observer.get();if(value!=null)value.changed();}
    boolean busy(){return active.get();}
    boolean blockedFor(LensContext context){return barrier!=null&&(context==null||barrier.same(context));}
    void confirm(Confirmation callback){
        if(!active.compareAndSet(false,true)){callback.finished(null,true);return;}
        reason="";progress="正在核對當次遊戲程序…";notifyView();
        worker.execute(()->{
            LensContext found=null;try{found=operations.readContext();}catch(Exception ignored){}
            final LensContext current=found;
            main.post(()->{
                active.set(false);progress="";
                boolean blocked=blockedFor(current);
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
                if(parsed.phase==LensStatus.Phase.UNKNOWN||parsed.phase==LensStatus.Phase.UNCERTAIN){
                    if(parsed.session!=null){
                        barrier=parsed.session;barrierPhase=parsed.phase;
                        preferences.edit().putString("retry-barrier",barrier.record()).putBoolean("ui-uncertain",barrierPhase==LensStatus.Phase.UNCERTAIN).apply();
                    }
                }
                // OFF and read errors never erase a barrier for the same
                // process. A fresh boot/PID/start is required to proceed.
                state=LensStatus.retainingBarrier(parsed,barrier,barrierPhase);
                progress="";active.set(false);notifyView();
            });
        });
    }
}
