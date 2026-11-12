package android.app;
public class NotificationManager {
 public static final int IMPORTANCE_LOW=2;public int posts;public boolean denied;public Notification last;
 public void createNotificationChannel(NotificationChannel c){}
 public void notify(int id,Notification n){if(denied)throw new SecurityException();posts++;last=n;}
}
