package dev.ody.seedpreview;
import android.app.*;
import android.content.*;
import android.content.pm.*;
import android.os.Build;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.*;
import java.util.concurrent.TimeUnit;
/** Independent of the main controller. No loader execution, gameplay reads or network. */
final class DiagnosticCollector {
 static String sha(InputStream in)throws Exception{MessageDigest d=MessageDigest.getInstance("SHA-256");byte[] b=new byte[32768];int n;while((n=in.read(b))!=-1)d.update(b,0,n);StringBuilder out=new StringBuilder();for(byte x:d.digest())out.append(String.format(Locale.US,"%02x",x&255));return out.toString();}
 static String publicValue(String key,String value){return DiagnosticReport.valid(key,value)?value:"unknown";}
 static String abi(){String a=Build.SUPPORTED_ABIS.length==0?"other":Build.SUPPORTED_ABIS[0];return DiagnosticReport.oneOf(a,"arm64-v8a","x86_64","armeabi-v7a","x86")?a:"other";}
 static Map<String,String> basic(Context c){
  Map<String,String> f=DiagnosticReport.emptyFacts();f.put("android_api",""+Build.VERSION.SDK_INT);f.put("android_release",publicValue("android_release",Build.VERSION.RELEASE));f.put("abi",abi());
  String maker=Build.MANUFACTURER.toLowerCase(Locale.US);f.put("manufacturer",DiagnosticReport.valid("manufacturer",maker)?maker:"other");
  try{PackageInfo app=c.getPackageManager().getPackageInfo(c.getPackageName(),PackageManager.GET_SIGNING_CERTIFICATES);f.put("app_version",publicValue("app_version",app.versionName));f.put("app_code",publicValue("app_code",""+app.getLongVersionCode()));
   if(app.signingInfo!=null&&!app.signingInfo.hasMultipleSigners()&&app.signingInfo.getApkContentsSigners().length==1)f.put("app_signer",sha(new ByteArrayInputStream(app.signingInfo.getApkContentsSigners()[0].toByteArray())));
  }catch(Exception ignored){}
  try{PackageInfo game=c.getPackageManager().getPackageInfo("com.nianticlabs.pikmin",0);f.put("game_version",publicValue("game_version",game.versionName));f.put("game_code",publicValue("game_code",""+game.getLongVersionCode()));}catch(PackageManager.NameNotFoundException e){f.put("game_version","missing");}catch(Exception ignored){}
  f.put("native_files",nativeFiles(c));f.put("last_phase",DiagnosticStore.lastPhase(c));f.put("last_exit",exit(c));return f;
 }
 static String nativeFiles(Context c){
  try{String a=abi();String[] names="x86_64".equals(a)?new String[]{"libseed_loader.so","libseed_bridge.so"}:new String[]{"libseed_loader.so","libseed_display.so"};
   for(String name:names){File file=new File(c.getApplicationInfo().nativeLibraryDir,name);if(!InstalledNative.allowed(file.getCanonicalPath(),a,name))return "mismatch";
    String expected;
    if(name.equals("libseed_display.so")){try(InputStream in=c.getAssets().open(name)){expected=sha(in);}}
    else{String asset=name.equals("libseed_loader.so")?"seed-loader-"+a+".sha256":"seed-bridge-x86_64.sha256";try(InputStream in=c.getAssets().open(asset)){byte[] b=new byte[66];int n=in.read(b);expected=InstalledNative.digest(new String(b,0,Math.max(0,n),StandardCharsets.US_ASCII));}}
    try(InputStream in=new FileInputStream(file)){if(expected==null||!expected.equals(sha(in)))return "mismatch";}
   }return "match";
  }catch(Exception ignored){return "unreadable";}
 }
 static String exit(Context c){
  if(Build.VERSION.SDK_INT<30)return "unavailable";
  try{ActivityManager am=c.getSystemService(ActivityManager.class);for(ApplicationExitInfo info:am.getHistoricalProcessExitReasons(c.getPackageName(),0,8)){
    if(!c.getPackageName().equals(info.getProcessName()))continue;
    switch(info.getReason()){case ApplicationExitInfo.REASON_CRASH:return "java_crash";case ApplicationExitInfo.REASON_CRASH_NATIVE:return "native_crash";case ApplicationExitInfo.REASON_ANR:return "anr";case ApplicationExitInfo.REASON_LOW_MEMORY:return "low_memory";case ApplicationExitInfo.REASON_USER_REQUESTED:case ApplicationExitInfo.REASON_USER_STOPPED:return "user_or_system";default:return "other";}
   }return "none";
  }catch(Exception ignored){return "unavailable";}
 }
 static RootDiagnostic root(){
  Process p=null;ByteArrayOutputStream bytes=new ByteArrayOutputStream();final boolean[] complete={true};
  try{
   String script=RootCommand.prefix(Build.VERSION.SDK_INT,abi())+"[ $(id -u) = 0 ] || exit 1\nprintf 'LENS_DIAG_ROOT\\t0\\n'\nprintf 'LENS_DIAG_BRIDGE\\t'; getprop ro.dalvik.vm.native.bridge\nprintf 'LENS_DIAG_SELINUX\\t'; getenforce\n";
   p=new ProcessBuilder(RootCommand.arguments(Build.VERSION.SDK_INT,abi(),script)).redirectErrorStream(true).start();p.getOutputStream().close();final Process child=p;
   Thread reader=new Thread(()->{try(InputStream in=child.getInputStream()){byte[] b=new byte[256];int n;while((n=in.read(b))!=-1)synchronized(bytes){if(bytes.size()+n>2048){complete[0]=false;}else bytes.write(b,0,n);}}catch(Exception e){complete[0]=false;}},"LensDiagnosticRoot");reader.setDaemon(true);reader.start();
   if(!p.waitFor(20,TimeUnit.SECONDS)){p.destroyForcibly();return RootDiagnostic.failure("timeout");}
   reader.join(1000);String text;synchronized(bytes){text=bytes.toString("UTF-8");}
   return RootDiagnostic.parse(p.exitValue(),!reader.isAlive()&&complete[0],text,RootCommand.nativeApi32(Build.VERSION.SDK_INT,abi()));
  }catch(IOException e){return RootDiagnostic.failure(p==null?"unavailable":"error");}catch(Exception e){return RootDiagnostic.failure("error");}finally{if(p!=null&&p.isAlive())p.destroyForcibly();}
 }
}
