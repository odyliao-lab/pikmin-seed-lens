package dev.ody.seedpreview;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
/** Visible-App-triggered metadata check only. No boot, download, root or diagnostic upload. */
public final class LensUpdateCheckService extends Service {
 @Override public int onStartCommand(Intent i,int flags,int id){if(!LensUpdater.check(this,false,()->stopSelf(id)))stopSelf(id);return START_NOT_STICKY;}
 @Override public IBinder onBind(Intent i){return null;}
}
