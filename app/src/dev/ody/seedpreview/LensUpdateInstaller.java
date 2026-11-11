package dev.ody.seedpreview;
import android.app.PendingIntent;
import android.content.*;
import android.content.pm.PackageInstaller;
import android.os.Build;
import java.io.*;
/** User-confirmed Android installer only; no root pm install or module operations. */
final class LensUpdateInstaller {
 static volatile boolean preparing;
 static int session(Context c){try{return LensUpdater.prefs(c).getInt("session",-1);}catch(Exception e){return -1;}}
 static boolean pending(Context c){
  int id=session(c);if(id<0)return false;
  try{long target=LensUpdater.prefs(c).getLong("install_code",Long.MAX_VALUE);
   if(LensUpdater.installed(c)>=target){clear(c,"更新安裝完成；請正常重開遊戲。");return false;}
   if(c.getPackageManager().getPackageInstaller().getSessionInfo(id)==null){clear(c,"上次安裝已結束，原設定保留。");return false;}
  }catch(Exception e){return true;}return true;
 }
 static void clear(Context c,String result)throws Exception{
  if(!UpdateInterlock.clear(c.getFilesDir()))throw new IOException("interlock");
  if(!LensUpdater.prefs(c).edit().remove("session").remove("install_code").putString("status",result).commit())throw new IOException("save");
 }
 static void cancel(Context c)throws Exception{int id=session(c);if(id>=0)c.getPackageManager().getPackageInstaller().abandonSession(id);clear(c,"已取消更新，原版本與設定保留。");}
 static void begin(Context input){Context c=input.getApplicationContext();synchronized(LensUpdater.class){if(LensUpdater.busy()||pending(c))return;preparing=true;}
  LensUpdater.worker.execute(()->{int id=-1;boolean frozen=false;try{
    if(!c.getPackageManager().canRequestPackageInstalls())throw new IOException("permission");
    LensUpdateInfo info=LensUpdater.latest(c);if(info==null)throw new IOException("manifest");LensUpdater.verify(c,LensUpdater.apk(c),info);
    if(!(frozen=UpdateInterlock.freeze(c.getFilesDir(),info.code)))throw new IOException("active load");
    PackageInstaller installer=c.getPackageManager().getPackageInstaller();PackageInstaller.SessionParams params=new PackageInstaller.SessionParams(PackageInstaller.SessionParams.MODE_FULL_INSTALL);
    params.setAppPackageName(LensUpdateInfo.PACKAGE);params.setSize(info.size);if(Build.VERSION.SDK_INT>=31)params.setRequireUserAction(PackageInstaller.SessionParams.USER_ACTION_REQUIRED);
    id=installer.createSession(params);if(!LensUpdater.prefs(c).edit().putInt("session",id).putLong("install_code",info.code).commit())throw new IOException("save");
    try(PackageInstaller.Session session=installer.openSession(id)){
     try(InputStream in=new FileInputStream(LensUpdater.apk(c));OutputStream out=session.openWrite("base.apk",0,info.size)){byte[] b=new byte[32768];int n;while((n=in.read(b))!=-1)out.write(b,0,n);session.fsync(out);}
     Intent callback=new Intent(c,LensUpdateInstallActivity.class).setAction("dev.ody.seedpreview.INSTALL_RESULT."+id).putExtra("expected_session",id).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TOP);
     PendingIntent result=PendingIntent.getActivity(c,id,callback,PendingIntent.FLAG_UPDATE_CURRENT|(Build.VERSION.SDK_INT>=31?PendingIntent.FLAG_MUTABLE:0));session.commit(result.getIntentSender());
    }
   }catch(Exception e){if(id>=0)try{c.getPackageManager().getPackageInstaller().abandonSession(id);}catch(Exception ignored){}
    try{if(frozen)clear(c,"安裝未完成；請確認沒有顯示操作進行中、並允許此來源安裝 App 後重試。原版本保留。");else LensUpdater.prefs(c).edit().putString("status","暫時無法準備安裝，原版本保留；請稍後重試。").commit();}catch(Exception ignored){}
   }finally{preparing=false;}});
 }
}
