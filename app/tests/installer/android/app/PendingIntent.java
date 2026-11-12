package android.app;
import android.content.*;import android.os.Bundle;
public class PendingIntent {
 public static final int FLAG_UPDATE_CURRENT=0x08000000,FLAG_MUTABLE=0x02000000;
 public int request,flags;public Intent intent;public Bundle options;
 public static PendingIntent getActivity(Context c,int r,Intent i,int f,Bundle b){PendingIntent p=new PendingIntent();p.request=r;p.intent=i;p.flags=f;p.options=b;return p;}
 public PendingIntent getIntentSender(){return this;}
}
