package dev.ody.seedpreview;
import android.content.*;import android.os.Handler;import java.util.*;import java.util.concurrent.*;
/** Actual controller with durable preference / Root doubles; no device qualification. */
public final class AllOffControllerTest {
 static int checks;static void check(boolean v){checks++;if(!v)throw new AssertionError("case "+checks);}
 static final class Prefs implements SharedPreferences {
  final Map<String,Object> values=new HashMap<>();boolean fail;
  public boolean contains(String k){return values.containsKey(k);}
  public String getString(String k,String f){return values.containsKey(k)?(String)values.get(k):f;}
  public boolean getBoolean(String k,boolean f){return values.containsKey(k)?(Boolean)values.get(k):f;}
  public Editor edit(){return new Editor(){final Map<String,Object> changes=new HashMap<>();
   public Editor putString(String k,String v){changes.put(k,v);return this;}
   public Editor putBoolean(String k,boolean v){changes.put(k,v);return this;}
   public boolean commit(){if(fail)return false;values.putAll(changes);return true;}public void apply(){commit();}};}
 }
 static final List<LensController> controllers=new ArrayList<>();
 static LensController make(Prefs p)throws Exception{Context ctx=new Context();ctx.prefs=p;var ctor=LensController.class.getDeclaredConstructor(Context.class);ctor.setAccessible(true);LensController c=ctor.newInstance(ctx);controllers.add(c);return c;}
 static Prefs known(){Prefs p=new Prefs();for(int i=0;i<5;i++)p.values.put(LensController.intentKey(i),false);p.values.put("display-auto-arm",true);return p;}
 static void settle(LensController c)throws Exception{long until=System.nanoTime()+3_000_000_000L;do{Handler.one();Thread.sleep(2);}while((c.busy()||c.reportReading.get()||!Handler.tasks.isEmpty())&&System.nanoTime()<until);check(!c.busy()&&!c.reportReading.get()&&Handler.tasks.isEmpty());}
 static void blockedRead(LensController c){c.operations.settingEntered=new CountDownLatch(1);c.operations.settingRelease=new CountDownLatch(1);}
 public static void main(String[] args)throws Exception{
  try{
   // Root denied: intent persists immediately and survives process recreation.
   Prefs p=known();LensController c=make(p);c.operations.fail=true;c.setGroup(0,true);
   check(Boolean.TRUE.equals(c.groupOn[0])&&p.getBoolean(LensController.intentKey(0),false));settle(c);
   check(Boolean.TRUE.equals(c.groupOn[0])&&c.groupReport[0].contains("Root")&&c.operations.enableCalls==0);
   c.setGps(true);check(Boolean.TRUE.equals(c.gpsOn));settle(c);
   LensController restart=make(p);check(Boolean.TRUE.equals(restart.groupOn[0])&&Boolean.TRUE.equals(restart.gpsOn));
   c.operations.fail=false;c.refreshGroups();settle(c);c.tick();settle(c);
   check(Boolean.TRUE.equals(c.operations.saved[0])&&c.overview().contains("等待遊戲")&&c.operations.enableCalls==0);
   c.operations.contextFail=true;c.monitor();settle(c);c.updateReports(System.currentTimeMillis());
   check(c.groupOn[0]&&c.groupReport[0].contains("Root")&&c.operations.enableCalls==0);
   c.operations.contextFail=false;c.monitor();settle(c);check(c.reason.isEmpty()&&c.overview().contains("等待遊戲"));
   // Unsupported Android/game: retain ON, keep effective native OFF, recover when eligible.
   LensController unsupported=make(known());unsupported.operations.unavailable="Android version not qualified";
   unsupported.setGroup(1,true);unsupported.setGps(true);settle(unsupported);
   check(Boolean.TRUE.equals(unsupported.groupOn[1])&&Boolean.TRUE.equals(unsupported.gpsOn));
   check(!unsupported.operations.saved[1]&&!unsupported.operations.saved[4]&&unsupported.groupReport[1].contains("不套用"));
   unsupported.operations.unavailable=null;unsupported.tick();settle(unsupported);
   check(unsupported.operations.saved[1]&&unsupported.operations.saved[4]);
   // GPS has its own qualification, independent from the four display groups.
   unsupported.operations.gpsUnavailable="GPS platform unavailable";unsupported.tick();settle(unsupported);
   check(unsupported.operations.saved[1]&&!unsupported.operations.saved[4]&&unsupported.gpsOn&&unsupported.gpsText.contains("GPS platform"));
   // Latest selection wins while a native setting write is already in flight.
   LensController rapid=make(known());blockedRead(rapid);rapid.setGroup(0,true);
   check(rapid.operations.settingEntered.await(1,TimeUnit.SECONDS));rapid.setGroup(0,false);rapid.setGps(true);rapid.setGps(false);
   check(!rapid.groupOn[0]&&!rapid.gpsOn);rapid.operations.settingRelease.countDown();settle(rapid);
   check(!rapid.operations.saved[0]&&!rapid.operations.saved[4]&&!rapid.anyGroup());
   // An old all-OFF acknowledgement cannot erase a later explicit ON.
   LensController off=make(known());blockedRead(off);off.setGroup(0,true);check(off.operations.settingEntered.await(1,TimeUnit.SECONDS));
   LensContext owner=LensContext.parse("123\t456\t"+DisplayGroupsTest.BOOT+"\n");off.barrier=owner;off.barrierPhase=LensStatus.Phase.UNKNOWN;
   off.allOff();check(off.pendingOff()&&!off.anyGroup()&&!off.armed());off.setGroup(2,true);check(off.groupOn[2]);check(off.arm());
   off.operations.settingRelease.countDown();settle(off);
   check(!off.pendingOff()&&off.groupOn[2]&&off.operations.saved[2]&&!off.operations.saved[0]);
   check(off.operations.writes.indexOf("ALL_OFF")<off.operations.writes.lastIndexOf("2:true"));
   check(off.barrier==owner&&off.barrierPhase==LensStatus.Phase.UNKNOWN);
   // Failed OFF remains a durable native fence; user can still choose ON locally.
   LensController failed=make(known());failed.operations.fail=true;failed.allOff();settle(failed);
   check(failed.pendingOff()&&!failed.anyGroup());failed.setGps(true);check(failed.gpsOn);settle(failed);check(failed.pendingOff()&&failed.operations.enableCalls==0);
   failed.operations.fail=false;failed.refreshGroups();settle(failed);check(!failed.pendingOff()&&failed.gpsOn&&!failed.operations.saved[4]);
   // OFF on disk must never import a stale native ON after App recreation.
   Prefs disk=known();LensController persisted=make(disk);Arrays.fill(persisted.operations.saved,true);persisted.refreshGroups();settle(persisted);
   check(!persisted.anyGroup());for(Boolean value:persisted.operations.saved)check(!value);
   // Migration only imports absent local values; invalid native values stay unknown.
   Prefs legacy=new Prefs();legacy.values.put("display-auto-arm",true);LensController migration=make(legacy);migration.operations.saved[0]=true;migration.operations.saved[3]=null;
   migration.refreshGroups();settle(migration);check(migration.groupOn[0]&&migration.groupOn[3]==null);
   check(!legacy.contains(LensController.intentKey(3)));migration.setGroup(0,false);settle(migration);check(!migration.groupOn[0]);
   // Invalid App preference is neither OFF nor replaced by a native value.
   Prefs corrupt=known();corrupt.values.put(LensController.intentKey(2),"invalid");LensController invalid=make(corrupt);invalid.refreshGroups();settle(invalid);check(invalid.groupOn[2]==null);
   invalid.setGroup(2,true);settle(invalid);check(invalid.groupOn[2]&&invalid.operations.saved[2]);
   // Failed local storage cannot claim a saved preference or issue a new Root write.
   Prefs broken=known();LensController storage=make(broken);broken.fail=true;storage.setGps(true);check(!storage.gpsOn&&storage.operations.readCalls==0&&storage.reason.contains("未保存"));
   // Saving ON before first consent does not enable the native member.
   Prefs noConsent=known();noConsent.values.put("display-auto-arm",false);LensController consent=make(noConsent);consent.setGps(true);settle(consent);
   check(consent.gpsOn&&!consent.operations.saved[4]&&consent.gpsText.contains("首次"));check(consent.arm());settle(consent);check(consent.operations.saved[4]);
   // Monitor only loads after settings sync and a fresh eligible foreground process.
   consent.operations.game=owner;consent.operations.front=true;consent.operations.allowEnable=true;
   consent.monitor();settle(consent);check(consent.operations.enableCalls==1&&consent.autoAttempt.same(owner));
   consent.monitor();settle(consent);check(consent.operations.enableCalls==1);
   consent.barrier=owner;consent.autoAttempt=null;consent.monitor();settle(consent);check(consent.operations.enableCalls==1);
   // Existing update interlock remains authoritative despite saved ON.
   LensController update=make(known());update.setGps(true);settle(update);update.operations.game=owner;update.operations.front=true;update.operations.allowEnable=true;
   java.io.File dir=update.operations.context.getFilesDir();check(UpdateInterlock.freeze(dir,41));update.monitor();settle(update);check(update.operations.enableCalls==0);check(UpdateInterlock.clear(dir));update.monitor();settle(update);check(update.operations.enableCalls==1);
   // Passive reads cannot block storing or replace the newest preference.
   LensController report=make(known());report.refreshGroups();settle(report);report.operations.game=owner;report.operations.reportEntered=new CountDownLatch(1);report.operations.reportRelease=new CountDownLatch(1);
   report.refreshReports();check(report.operations.reportEntered.await(1,TimeUnit.SECONDS));report.setGps(true);check(report.gpsOn);report.setGps(false);check(!report.gpsOn);report.operations.reportRelease.countDown();settle(report);check(!report.operations.saved[4]);
   System.out.println("AllOffControllerTest: "+checks+" real intent/migration/queue/OFF/guard checks; not device acceptance");
  }finally{for(LensController c:controllers){c.worker.shutdownNow();java.io.File dir=c.operations.context.getFilesDir();for(java.io.File file:dir.listFiles())file.delete();dir.delete();}}
 }
}
