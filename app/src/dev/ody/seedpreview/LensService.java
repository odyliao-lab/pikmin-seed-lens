package dev.ody.seedpreview;

import android.app.*;
import android.content.*;
import android.content.pm.ServiceInfo;
import android.os.*;

/** APK-owned foreground watcher; never starts from boot or uses another package. */
public final class LensService extends Service {
    final Handler handler=new Handler(Looper.getMainLooper());
    final Runnable poll=new Runnable(){public void run(){
        LensController c=LensController.get(LensService.this);
        if(!c.armed()||c.groupsKnown()&&!c.anyGroup()){stopSelf();return;}
        c.monitor();handler.postDelayed(this,4000);
    }};
    static void start(Context context){try{context.startForegroundService(new Intent(context,LensService.class));}
        catch(RuntimeException failure){LensController.get(context).reason="顯示等待服務無法啟動；請回 App 重新核對。";}}
    @Override public void onCreate(){
        super.onCreate();NotificationManager manager=getSystemService(NotificationManager.class);
        manager.createNotificationChannel(new NotificationChannel("lens-display","遊戲資訊顯示",NotificationManager.IMPORTANCE_LOW));
        PendingIntent open=PendingIntent.getActivity(this,0,new Intent(this,MainActivity.class),PendingIntent.FLAG_IMMUTABLE|PendingIntent.FLAG_UPDATE_CURRENT);
        Notification note=new Notification.Builder(this,"lens-display").setSmallIcon(android.R.drawable.ic_menu_view)
            .setContentTitle("皮克敏花苗透視鏡").setContentText("已啟用；關閉全部開關即可停止。")
            .setContentIntent(open).setOngoing(true).setVisibility(Notification.VISIBILITY_SECRET).build();
        if(Build.VERSION.SDK_INT>=34)startForeground(71,note,ServiceInfo.FOREGROUND_SERVICE_TYPE_SPECIAL_USE);else startForeground(71,note);
    }
    @Override public int onStartCommand(Intent intent,int flags,int id){handler.removeCallbacks(poll);handler.post(poll);return START_NOT_STICKY;}
    @Override public void onDestroy(){handler.removeCallbacks(poll);super.onDestroy();}
    @Override public IBinder onBind(Intent intent){return null;}
}
