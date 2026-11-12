package dev.ody.seedpreview;
import android.app.*;import android.content.*;import android.content.pm.*;import android.os.Build;import java.nio.file.*;import java.util.concurrent.*;
public final class InstallerCallbackTest {
 static int checks;static void check(boolean ok){checks++;if(!ok)throw new AssertionError("installer case "+checks);}
 static void settle()throws Exception{LensUpdater.worker.submit(()->{}).get(3,TimeUnit.SECONDS);check(!LensUpdateInstaller.preparing);}
 static Intent result(int expected,int received,int status){return new Intent().putExtra("expected_session",expected).putExtra(PackageInstaller.EXTRA_SESSION_ID,received).putExtra(PackageInstaller.EXTRA_STATUS,status);}
 public static void main(String[] args)throws Exception{
  Context c=new Context();Path dir=Files.createTempDirectory("lens-installer-test");Context.directory=dir.toFile();Files.write(LensUpdater.apk(c).toPath(),new byte[]{1,2,3});PackageInstaller pm=Context.pm.installer;
  try{
   for(int api:new int[]{28,31,32,34,35}){
    Build.VERSION.SDK_INT=api;PendingIntent p=LensUpdateInstaller.callback(c,731);
    check(p.intent.target==LensUpdateInstallActivity.class&&p.request==731&&p.intent.getIntExtra("expected_session",-1)==731);
    check(p.intent.action.equals("dev.ody.seedpreview.INSTALL_RESULT.731"));
    check((p.flags&PendingIntent.FLAG_UPDATE_CURRENT)!=0&&((p.flags&PendingIntent.FLAG_MUTABLE)!=0)==(api>=31));
    check(api>=34?p.options!=null&&p.options.creatorMode==ActivityOptions.MODE_BACKGROUND_ACTIVITY_START_ALLOWED:p.options==null);
   }
   Build.VERSION.SDK_INT=35;
   LensUpdateInstaller.begin(c);settle();int id=LensUpdateInstaller.session(c);
   check(id>0&&pm.committed!=null&&pm.committed.options.creatorMode==1&&LensUpdater.verifies==1);
   check(pm.params.userAction==PackageInstaller.SessionParams.USER_ACTION_REQUIRED&&pm.params.pkg.equals("dev.ody.seedpreview"));
   check(Files.exists(dir.resolve("update-freeze"))&&LensUpdateInstaller.pending(c));
   int created=pm.created;LensUpdateInstaller.begin(c);settle();check(pm.created==created);
   LensUpdateInstallActivity stale=new LensUpdateInstallActivity();stale.handle(result(id,id+1,-1));check(stale.finished&&stale.started.isEmpty()&&LensUpdateInstaller.session(c)==id);
   LensUpdateInstallActivity confirm=new LensUpdateInstallActivity();Intent system=new Intent();confirm.handle(result(id,id,-1).putExtra(Intent.EXTRA_INTENT,system));check(confirm.finished&&confirm.started.get(0)==system&&LensUpdateInstaller.pending(c));
   LensUpdateInstallActivity cancelled=new LensUpdateInstallActivity();cancelled.handle(result(id,id,PackageInstaller.STATUS_FAILURE));check(!LensUpdateInstaller.pending(c)&&!Files.exists(dir.resolve("update-freeze")));
   LensUpdateInstaller.begin(c);settle();id=LensUpdateInstaller.session(c);check(pm.created==created+1);
   LensUpdateInstallActivity missing=new LensUpdateInstallActivity();missing.handle(result(id,id,-1));check(!LensUpdateInstaller.pending(c)&&!Files.exists(dir.resolve("update-freeze"))&&pm.abandoned==1);
   check(LensUpdater.values.map.get("status").toString().contains("無法開啟 Android"));
   LensUpdateInstaller.begin(c);settle();id=LensUpdateInstaller.session(c);LensUpdateInstallActivity blocked=new LensUpdateInstallActivity();blocked.reject=system;blocked.handle(result(id,id,-1).putExtra(Intent.EXTRA_INTENT,system));check(!LensUpdateInstaller.pending(c)&&pm.abandoned==2);
   LensUpdateInstaller.begin(c);settle();id=LensUpdateInstaller.session(c);pm.abandonFails=true;new LensUpdateInstallActivity().handle(result(id,id,-1));check(LensUpdateInstaller.pending(c)&&Files.exists(dir.resolve("update-freeze")));pm.abandonFails=false;LensUpdateInstaller.cancel(c);check(!LensUpdateInstaller.pending(c));
   created=pm.created;LensUpdater.reject=true;LensUpdateInstaller.begin(c);settle();check(pm.created==created&&!Files.exists(dir.resolve("update-freeze")));LensUpdater.reject=false;
   Context.pm.allowed=false;LensUpdateInstaller.begin(c);settle();check(pm.created==created);Context.pm.allowed=true;
   try(UpdateInterlock.Lease lease=UpdateInterlock.beginLoad(dir.toFile(),56)){check(lease!=null);LensUpdateInstaller.begin(c);settle();check(pm.created==created);}
   LensUpdateInstaller.begin(c);settle();LensUpdater.version=57;check(!LensUpdateInstaller.pending(c)&&!Files.exists(dir.resolve("update-freeze"))&&!LensUpdater.values.map.containsKey("session"));
   System.out.println("InstallerCallbackTest: "+checks+" actual installer/callback checks; Android is a test double, device UI remains required");
  }finally{LensUpdater.worker.shutdownNow();try(var paths=Files.walk(dir)){for(Path p:paths.sorted(java.util.Comparator.reverseOrder()).toList())Files.delete(p);}}
 }
}
