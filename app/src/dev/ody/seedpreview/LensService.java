package dev.ody.seedpreview;

import android.app.*;
import android.content.*;
import android.content.pm.ServiceInfo;
import android.os.*;

/** APK-owned foreground watcher; never starts from boot or uses another package. */
public final class LensService extends Service {
    static final String CLOSE_ALL="dev.ody.seedpreview.CLOSE_ALL";
    NotificationManager manager;String lastText;
    final Handler handler=new Handler(Looper.getMainLooper());
    final Runnable poll=new Runnable(){public void run(){
        LensController c=LensController.get(LensService.this);
        if(!c.needsWatch()){stopSelf();return;}
        c.tick();c.refreshReports();publish(c.notificationText());handler.postDelayed(this,4000);
    }};
    static void start(Context context){try{context.startForegroundService(new Intent(context,LensService.class));}
        catch(RuntimeException failure){LensController.get(context).reason="顯示等待服務無法啟動；請回 App 重新核對。";}}
    @Override public void onCreate(){
        super.onCreate();manager=getSystemService(NotificationManager.class);
        manager.createNotificationChannel(new NotificationChannel("lens-display","遊戲資訊顯示",NotificationManager.IMPORTANCE_LOW));
        Notification note=notification("正在核對目前狀態…");
        if(Build.VERSION.SDK_INT>=34)startForeground(71,note,ServiceInfo.FOREGROUND_SERVICE_TYPE_SPECIAL_USE);else startForeground(71,note);
    }
    Notification notification(String text){
        PendingIntent open=PendingIntent.getActivity(this,0,new Intent(this,MainActivity.class),PendingIntent.FLAG_IMMUTABLE|PendingIntent.FLAG_UPDATE_CURRENT);
        PendingIntent off=PendingIntent.getService(this,71,new Intent(this,LensService.class).setAction(CLOSE_ALL),PendingIntent.FLAG_IMMUTABLE|PendingIntent.FLAG_UPDATE_CURRENT);
        return new Notification.Builder(this,"lens-display").setSmallIcon(android.R.drawable.ic_menu_view)
            .setContentTitle("皮克敏花苗透視鏡").setContentText(text).setStyle(new Notification.BigTextStyle().bigText(text))
            .setContentIntent(open).setOnlyAlertOnce(true).setOngoing(true).setVisibility(Notification.VISIBILITY_SECRET)
            .addAction(new Notification.Action.Builder(null,"開啟透視鏡",open).build())
            .addAction(new Notification.Action.Builder(null,"關閉全部",off).build()).build();
    }
    void publish(String text){
        if(text.equals(lastText))return;
        try{manager.notify(71,notification(text));lastText=text;}catch(RuntimeException denied){/* App controls remain available. */}
    }
    @Override public int onStartCommand(Intent intent,int flags,int id){
        LensController c=LensController.get(this);
        if(intent!=null&&CLOSE_ALL.equals(intent.getAction()))c.allOff();
        publish(c.notificationText());handler.removeCallbacks(poll);handler.post(poll);return START_NOT_STICKY;
    }
    @Override public void onDestroy(){handler.removeCallbacks(poll);stopForeground(STOP_FOREGROUND_REMOVE);super.onDestroy();}
    @Override public IBinder onBind(Intent intent){return null;}
}
