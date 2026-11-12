package dev.ody.seedpreview;
import android.content.*;import java.util.*;import java.util.concurrent.*;import java.nio.file.*;
public final class UpdateRetryTest {
 static int checks;static void check(boolean ok){checks++;if(!ok)throw new AssertionError("updater case "+checks);}
 static final class Prefs implements SharedPreferences {
  final Map<String,Object> data=new HashMap<>();boolean fail;
  public String getString(String k,String f){return (String)data.getOrDefault(k,f);}public long getLong(String k,long f){return (Long)data.getOrDefault(k,f);}
  public int getInt(String k,int f){return (Integer)data.getOrDefault(k,f);}public boolean getBoolean(String k,boolean f){return (Boolean)data.getOrDefault(k,f);}
  public Editor edit(){return new Editor(){final Map<String,Object> next=new HashMap<>();
   public Editor putString(String k,String v){next.put(k,v);return this;}public Editor putLong(String k,long v){next.put(k,v);return this;}public Editor putInt(String k,int v){next.put(k,v);return this;}public Editor putBoolean(String k,boolean v){next.put(k,v);return this;}
   public boolean commit(){if(fail)return false;data.putAll(next);return true;}public void apply(){commit();}};}
 }
 static Context context(Prefs p){return new Context(){public SharedPreferences getSharedPreferences(String n,int m){return p;}};}
 static void settle()throws Exception{LensUpdater.worker.submit(()->{}).get(3,TimeUnit.SECONDS);check(!LensUpdater.checking);}
 public static void main(String[] args)throws Exception{
  Prefs p=new Prefs();Context c=context(p);String valid=Files.readString(Paths.get(args[0]));LensUpdateClient.raw=valid;
  try{
   check(!LensUpdater.check(c,false,null)&&LensUpdateClient.calls==0); // default OFF
   check(LensUpdater.check(c,true,null));settle();check(LensUpdater.latest(c)!=null&&LensUpdater.checkTime(c,"last_success")>0);
   p.data.put("automatic",true);check(!LensUpdater.check(c,false,null));
   long success=LensUpdater.checkTime(c,"last_success");LensUpdateClient.fail=true;
   for(int i=1;i<=3;i++){check(LensUpdater.check(c,true,null));settle();check(LensUpdater.failures(c)==i);check(!LensUpdater.check(c,false,null));}
   check(LensUpdater.checkTime(c,"last_success")==success&&p.getString("manifest","").equals(valid));
   check(LensUpdater.status(c).contains("24 小時")&&LensUpdater.status(c).contains("快取"));
   // Durable failure history also controls a new Context / reopening, with no job scheduled.
   Context reopened=context(p);check(!LensUpdater.check(reopened,false,null));
   p.data.put("last_attempt",System.currentTimeMillis()-86_400_001L);LensUpdateClient.fail=false;
   check(LensUpdater.check(reopened,false,null));settle();check(LensUpdater.failures(c)==0);
   // Wrong identity metadata is a failure and cannot replace the previous successful cache.
   LensUpdateClient.raw=valid.replace("dev.ody.seedpreview","other.package");check(LensUpdater.check(c,true,null));settle();check(LensUpdater.failures(c)==1&&p.getString("manifest","").equals(valid));
   int requests=LensUpdateClient.calls;p.fail=true;check(!LensUpdater.check(c,true,null));check(LensUpdateClient.calls==requests&&LensUpdater.status(c).contains("尚未連線"));p.fail=false;
   // Single-flight and failed result persistence, exercising the actual asynchronous worker.
   LensUpdateClient.raw=valid;LensUpdateClient.entered=new CountDownLatch(1);LensUpdateClient.release=new CountDownLatch(1);
   check(LensUpdater.check(c,true,null));check(LensUpdateClient.entered.await(1,TimeUnit.SECONDS));check(!LensUpdater.check(c,true,null));
   p.fail=true;LensUpdateClient.release.countDown();settle();check(LensUpdater.status(c).contains("無法保存"));p.fail=false;
   LensUpdateClient.entered=null;LensUpdateClient.release=null;p.data.put("automatic",false);requests=LensUpdateClient.calls;
   check(!LensUpdater.check(c,false,null)&&requests==LensUpdateClient.calls);check(LensUpdater.check(c,true,null));settle();check(LensUpdater.failures(c)==0&&LensUpdater.checkProblem.isEmpty());
   LensUpdateInstaller.pending=true;check(!LensUpdater.check(c,true,null));LensUpdateInstaller.pending=false;
   System.out.println("UpdateRetryTest: "+checks+" actual updater/persistent preference checks; HTTP is a test double");
  }finally{LensUpdater.worker.shutdownNow();}
 }
}
