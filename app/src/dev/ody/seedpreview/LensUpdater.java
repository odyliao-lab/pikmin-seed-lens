package dev.ody.seedpreview;
import android.content.*;
import android.content.pm.*;
import android.os.Build;
import java.io.*;
import java.security.MessageDigest;
import java.util.*;
import java.util.concurrent.*;
import java.util.zip.*;
/** Diagnostic-process updater, separate from root/game operations. */
final class LensUpdater {
 static final ExecutorService worker=Executors.newSingleThreadExecutor();
 static volatile boolean checking,downloading,cancel;static volatile String transfer="",checkProblem="";
 static SharedPreferences prefs(Context c){return c.getSharedPreferences("lens-updates",0);}
 static String installedVersion(Context c){try{return c.getPackageManager().getPackageInfo(c.getPackageName(),0).versionName;}catch(Exception e){return "未知";}}
 static long installed(Context c){try{return c.getPackageManager().getPackageInfo(c.getPackageName(),0).getLongVersionCode();}catch(Exception e){return -1;}}
 static LensUpdateInfo latest(Context c){try{return new LensUpdateInfo(prefs(c).getString("manifest",""));}catch(Exception e){return null;}}
 static File apk(Context c){return new File(c.getFilesDir(),"lens-update.apk");}
 static boolean busy(){return checking||downloading||LensUpdateInstaller.preparing;}
 static boolean automatic(Context c){try{return prefs(c).getBoolean("automatic",false);}catch(Exception e){return false;}}
 static String status(Context c){
  if(checking)return "正在檢查更新…";if(downloading)return transfer;if(LensUpdateInstaller.preparing)return "正在核對並準備安裝…";
  if(LensUpdateInstaller.pending(c))return "等待 Android 安裝確認；可取消本次更新。";
  if(!checkProblem.isEmpty())return checkProblem;
  return prefs(c).getString("status","尚未檢查更新");
 }
 static long checkTime(Context c,String key){try{return Math.max(0,prefs(c).getLong(key,0));}catch(ClassCastException invalid){return 0;}}
 static int failures(Context c){try{return Math.min(3,Math.max(0,prefs(c).getInt("check_failures",0)));}catch(ClassCastException invalid){return 0;}}
 static String checkedAt(Context c){long at=checkTime(c,"last_success");return at==0?"尚無成功檢查時間":new java.text.SimpleDateFormat("MM-dd HH:mm",Locale.TAIWAN).format(new Date(at));}
 static synchronized boolean check(Context input,boolean manual,Runnable done){
  Context c=input.getApplicationContext();long now=System.currentTimeMillis();
  if(busy()||LensUpdateInstaller.pending(c))return false;
  if(!manual&&!UpdateCheckPolicy.due(automatic(c),now,checkTime(c,"last_attempt"),checkTime(c,"last_success"),failures(c)))return false;
  if(!prefs(c).edit().putLong("last_attempt",now).commit()){checkProblem="檢查時間無法保存，尚未連線；請稍後重試。";return false;}
  checking=true;checkProblem="";
  worker.execute(()->{try{String raw=LensUpdateClient.manifest();LensUpdateInfo info=new LensUpdateInfo(raw),previous=latest(c);
    if(previous!=null&&(info.code<previous.code||info.code==previous.code&&!info.sha256.equals(previous.sha256)))throw new IOException("rollback");
    String state=UpdateCheckPolicy.status(info.code,installed(c),info.minSdk,Build.VERSION.SDK_INT,info.version);
    if(!prefs(c).edit().putString("manifest",raw).putString("status",state).putLong("last_success",System.currentTimeMillis()).putInt("check_failures",0).commit())throw new IOException("save");
   }catch(Exception e){int count=UpdateCheckPolicy.failed(failures(c));
    String state="檢查未完成，保留原版本與快取。"+(automatic(c)?"自動檢查將在 "+UpdateCheckPolicy.retry(count)+"後下次開啟 App 時重試；":"自動檢查已關閉；")+"仍可手動檢查。";
    if(!prefs(c).edit().putString("status",state).putInt("check_failures",count).commit())checkProblem="檢查結果無法保存，請稍後重試；原版本保留。";}
   finally{checking=false;if(done!=null)done.run();}});return true;
 }
 static synchronized void download(Context input){
  Context c=input.getApplicationContext();LensUpdateInfo info=latest(c);if(busy()||LensUpdateInstaller.pending(c)||info==null||!info.newerThan(installed(c))||info.minSdk>Build.VERSION.SDK_INT)return;
  downloading=true;cancel=false;transfer="下載準備中…";
  worker.execute(()->{File part=new File(c.getFilesDir(),"lens-update.apk.part");try{
    LensUpdateClient.download(info,part,new LensUpdateClient.Observer(){public boolean cancelled(){return cancel;}public void progress(long n,long total){transfer="下載中 · "+(n*100/total)+"%";}});
    verify(c,part,info);if(cancel)throw new InterruptedIOException();
    if(apk(c).exists()&&!apk(c).delete())throw new IOException("replace");
    if(!part.renameTo(apk(c)))throw new IOException("save");
    if(!prefs(c).edit().putString("download_sha",info.sha256).putString("status","已下載並核對；等待你確認安裝。").commit())throw new IOException("save");
   }catch(Exception e){part.delete();prefs(c).edit().putString("status",cancel?"已取消下載，原版本保留。":"下載或 APK 核對未完成，原版本保留；請重試或從官方發布頁取得 APK。").commit();}
   finally{downloading=false;transfer="";}});
 }
 static boolean ready(Context c){LensUpdateInfo i=latest(c);return i!=null&&i.newerThan(installed(c))&&apk(c).isFile()&&i.sha256.equals(prefs(c).getString("download_sha",""));}
 static void verify(Context c,File file,LensUpdateInfo i)throws Exception{
  if(file.length()!=i.size||!i.sha256.equals(LensUpdateClient.digest(file)))throw new IOException("APK hash");
  PackageManager pm=c.getPackageManager();PackageInfo a=pm.getPackageArchiveInfo(file.getAbsolutePath(),PackageManager.GET_SIGNING_CERTIFICATES),b=pm.getPackageInfo(c.getPackageName(),PackageManager.GET_SIGNING_CERTIFICATES);
  if(a==null||!LensUpdateInfo.PACKAGE.equals(a.packageName)||a.getLongVersionCode()!=i.code||!i.version.equals(a.versionName)||i.code<=b.getLongVersionCode()||a.applicationInfo==null||a.applicationInfo.minSdkVersion>Build.VERSION.SDK_INT||a.signingInfo==null||b.signingInfo==null||a.signingInfo.hasMultipleSigners()||b.signingInfo.hasMultipleSigners()||a.signingInfo.getApkContentsSigners().length!=1||b.signingInfo.getApkContentsSigners().length!=1)throw new IOException("APK identity");
  byte[] next=a.signingInfo.getApkContentsSigners()[0].toByteArray(),current=b.signingInfo.getApkContentsSigners()[0].toByteArray();
  if(!MessageDigest.isEqual(next,current)||!DiagnosticReport.SIGNER.equals(DiagnosticCollector.sha(new ByteArrayInputStream(next))))throw new IOException("APK signer");
  LensUpdateArchive.inspect(file);
 }
}
