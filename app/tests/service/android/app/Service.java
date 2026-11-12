package android.app;
import android.content.*;import android.os.IBinder;
public class Service extends Context {
 public static final int START_NOT_STICKY=2,STOP_FOREGROUND_REMOVE=1;
 public final NotificationManager manager=new NotificationManager();public int stopped,removed;
 public <T>T getSystemService(Class<T> c){return c.cast(manager);}
 public void onCreate(){}public void onDestroy(){}public int onStartCommand(Intent i,int f,int id){return 2;}
 public IBinder onBind(Intent i){return null;}
 public void startForeground(int id,Notification n){manager.notify(id,n);}
 public void startForeground(int id,Notification n,int type){startForeground(id,n);}
 public void stopSelf(){stopped++;}public void stopForeground(int mode){removed++;}
}
