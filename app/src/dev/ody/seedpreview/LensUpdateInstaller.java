package dev.ody.seedpreview;
import android.app.PendingIntent;
import android.app.ActivityOptions;
import android.content.*;
import android.content.pm.PackageInstaller;
import android.os.Build;
import android.os.Bundle;
import java.io.*;
/** User-confirmed Android installer only; no root pm install or module operations. */
final class LensUpdateInstaller {
 static volatile boolean preparing;
 private static int missingSession=-1;
 private static long firstMissingAt;
 private static final long CALLBACK_GRACE_MS=8_000;
 static int session(Context c){try{return LensUpdater.prefs(c).getInt("session",-1);}catch(Exception e){return -1;}}
 static synchronized boolean pending(Context c){
  int id=session(c);if(id<0)return false;
  try{
   long target=LensUpdater.prefs(c).getLong("install_code",Long.MAX_VALUE);
   if(LensUpdater.installed(c)>=target){
    clear(c,"更新安裝完成，請正常重開遊戲。");return false;
   }
   if(c.getPackageManager().getPackageInstaller().getSessionInfo(id)!=null){
    missingSession=-1;return true;
   }
   // A terminal result may arrive after Android removes the session.
   long now=android.os.SystemClock.elapsedRealtime();
   if(missingSession!=id||now<firstMissingAt){missingSession=id;firstMissingAt=now;}
   if(now-firstMissingAt<CALLBACK_GRACE_MS)return true;
   clear(c,"Android 未完成安裝，未收到具體原因。可重試已驗證的 APK。");return false;
  }catch(Exception e){return true;}
 }
 static String pendingStatus(Context c){
  return LensUpdater.prefs(c).getBoolean("install_needs_action",false)
       ?"請完成 Android 安裝確認。":"Android 正在處理安裝。";
 }
 static synchronized boolean requestConfirmation(Context c,int id){
  if(id<0||session(c)!=id)return false;
  LensUpdater.prefs(c).edit().putBoolean("install_needs_action",true).commit();return true;
 }
 private static void clear(Context c,String result)throws IOException{
  if(!UpdateInterlock.clear(c.getFilesDir()))throw new IOException("interlock");
  if(!LensUpdater.prefs(c).edit().remove("session").remove("install_code").remove("install_needs_action")
      .putString("status",result).commit())throw new IOException("save");
  missingSession=-1;
 }
 static synchronized boolean finish(Context c,int id,int status)throws IOException{
  if(id<0||session(c)!=id)return false;
  clear(c,resultMessage(status));return true;
 }
 // Keep stable system categories. OEM messages may include private paths.
 private static String resultMessage(int status){
  switch(status){
   case PackageInstaller.STATUS_SUCCESS:return "更新安裝完成，請正常重開遊戲。";
   case PackageInstaller.STATUS_FAILURE_ABORTED:return "Android 已中止安裝。可重試已驗證的 APK。";
   case PackageInstaller.STATUS_FAILURE_BLOCKED:return "Android 或裝置政策阻擋安裝。請核對系統提示。";
   case PackageInstaller.STATUS_FAILURE_STORAGE:return "裝置儲存空間不足。";
   case PackageInstaller.STATUS_FAILURE_CONFLICT:return "Android 回報套件衝突。";
   case PackageInstaller.STATUS_FAILURE_INVALID:return "Android 回報 APK 無效。";
   case PackageInstaller.STATUS_FAILURE_INCOMPATIBLE:return "Android 回報這個 APK 與裝置不相容。";
   default:return "Android 未完成安裝，狀態 "+status+"。可重試已驗證的 APK。";
  }
 }
 static synchronized boolean cancel(Context c,int id,String result)throws IOException{
  if(id<0||session(c)!=id)return false;
  c.getPackageManager().getPackageInstaller().abandonSession(id);
  clear(c,result);return true;
 }
 static void cancel(Context c)throws IOException{
  cancel(c,session(c),"已取消安裝，已驗證的 APK 可供重試。");
 }
 static PendingIntent callback(Context c,int id){
  Intent intent=new Intent(c,LensUpdateInstallActivity.class).setAction("dev.ody.seedpreview.INSTALL_RESULT."+id).putExtra("expected_session",id).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TOP);
  // Only this explicit, session-bound system installer callback receives the opt-in.
  // target35 no longer delegates the creator's visible-Activity privilege by default.
  Bundle options=null;
  if(Build.VERSION.SDK_INT>=34)options=ActivityOptions.makeBasic().setPendingIntentCreatorBackgroundActivityStartMode(ActivityOptions.MODE_BACKGROUND_ACTIVITY_START_ALLOWED).toBundle();
  return PendingIntent.getActivity(c,id,intent,PendingIntent.FLAG_UPDATE_CURRENT|(Build.VERSION.SDK_INT>=31?PendingIntent.FLAG_MUTABLE:0),options);
 }
 static void begin(Context input){
  Context c=input.getApplicationContext();
  synchronized(LensUpdater.class){
   if(LensUpdater.busy()||pending(c))return;
   preparing=true;
  }
  LensUpdater.worker.execute(()->{
   try{prepare(c);}finally{preparing=false;}
  });
 }
 private static void prepare(Context c){
  int id=-1;
  boolean frozen=false,tracked=false;
  PackageInstaller installer=null;
  try{
   if(!c.getPackageManager().canRequestPackageInstalls())throw new IOException("permission");
   LensUpdateInfo info=LensUpdater.latest(c);if(info==null)throw new IOException("manifest");
   try{LensUpdater.verify(c,LensUpdater.apk(c),info);}
   catch(Exception invalid){
    LensUpdater.prefs(c).edit().remove("download_sha").putString("status","快取的 APK 無法驗證，請重新下載。").commit();
    return;
   }
   if(!(frozen=UpdateInterlock.freeze(c.getFilesDir(),info.code)))throw new IOException("active load");
   installer=c.getPackageManager().getPackageInstaller();
   PackageInstaller.SessionParams params=new PackageInstaller.SessionParams(PackageInstaller.SessionParams.MODE_FULL_INSTALL);
   params.setAppPackageName(LensUpdateInfo.PACKAGE);params.setSize(info.size);
   if(Build.VERSION.SDK_INT>=31)params.setRequireUserAction(PackageInstaller.SessionParams.USER_ACTION_REQUIRED);
   synchronized(LensUpdateInstaller.class){
    id=installer.createSession(params);
    if(!LensUpdater.prefs(c).edit().putInt("session",id).putLong("install_code",info.code)
        .remove("install_needs_action").commit())throw new IOException("save");
    tracked=true;
   }
   try(PackageInstaller.Session session=installer.openSession(id)){
    try(InputStream in=new FileInputStream(LensUpdater.apk(c));OutputStream out=session.openWrite("base.apk",0,info.size)){
     byte[] bytes=new byte[32768];int n;while((n=in.read(bytes))!=-1)out.write(bytes,0,n);session.fsync(out);
    }
    session.commit(callback(c,id).getIntentSender());
   }
  }catch(Exception failure){
   synchronized(LensUpdateInstaller.class){
    // A submission error can arrive after Android accepted the commit.
    // Preserve the interlock if abandonment fails for a tracked session.
    if(id>=0){
     if(tracked&&session(c)!=id)return; // The terminal callback already handled it.
     try{installer.abandonSession(id);}
     catch(Exception ambiguous){if(tracked)return;}
    }
    String status="無法準備安裝，請確認透視鏡目前沒有載入作業，並允許此來源安裝 App。";
    try{if(frozen)clear(c,status);else LensUpdater.prefs(c).edit().putString("status",status).commit();}
    catch(IOException ignored){}
   }
  }
 }
}
