package android.app;
import android.content.*;
public class PendingIntent {
 public static final int FLAG_IMMUTABLE=1,FLAG_UPDATE_CURRENT=2;
 public final Intent intent;public final int flags;public final boolean service;
 PendingIntent(Intent i,int f,boolean s){intent=i;flags=f;service=s;}
 public static PendingIntent getActivity(Context c,int id,Intent i,int f){return new PendingIntent(i,f,false);}
 public static PendingIntent getService(Context c,int id,Intent i,int f){return new PendingIntent(i,f,true);}
}
