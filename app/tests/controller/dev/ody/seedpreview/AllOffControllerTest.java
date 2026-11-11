package dev.ody.seedpreview;
import android.content.*;import android.os.Handler;import java.util.*;
public final class AllOffControllerTest {
 static int checks;static void check(boolean value){checks++;if(!value)throw new AssertionError("case "+checks);}
 static final class Prefs implements SharedPreferences {
  final Map<String,Object> values=new HashMap<>();boolean fail;
  public boolean contains(String k){return values.containsKey(k);}
  public String getString(String k,String f){return values.containsKey(k)?(String)values.get(k):f;}
  public boolean getBoolean(String k,boolean f){return values.containsKey(k)?(Boolean)values.get(k):f;}
  public Editor edit(){return new Editor(){final Map<String,Object> changes=new HashMap<>();
   public Editor putString(String k,String v){changes.put(k,v);return this;}
   public Editor putBoolean(String k,boolean v){changes.put(k,v);return this;}
   public boolean commit(){values.putAll(changes);return !fail;}public void apply(){commit();}};}
 }
 static LensController make(Prefs prefs)throws Exception{
  Context ctx=new Context();ctx.prefs=prefs;
  var constructor=LensController.class.getDeclaredConstructor(Context.class);constructor.setAccessible(true);
  return constructor.newInstance(ctx);
 }
 static void settle(LensController c)throws Exception{
  long until=System.nanoTime()+3_000_000_000L;
  do {Handler.one();Thread.sleep(2);}while((c.busy()||c.reportReading.get()||!Handler.tasks.isEmpty())&&System.nanoTime()<until);
  check(!c.busy()&&!c.reportReading.get()&&Handler.tasks.isEmpty());
 }
 public static void main(String[] args)throws Exception{
  Prefs prefs=new Prefs();prefs.values.put("display-auto-arm",true);LensController c=make(prefs),restarted=null,deferred=null;
  try{
   Arrays.fill(c.groupOn,true);c.active.set(true); // A report read is still in flight.
   LensContext barrier=LensContext.parse("123\t456\t"+DisplayGroupsTest.BOOT+"\n");
   c.barrier=barrier;c.barrierPhase=LensStatus.Phase.UNKNOWN;
   c.allOff();check(c.pendingOff()&&!c.armed());check(c.operations.offCalls==0&&c.closeQueued);
   check(c.active.get()&&c.groupOn[0]); // Saving intent does not claim native OFF.
   c.execute(LensStatus.Action.ENABLE,barrier,true,()->true);check(c.operations.enableCalls==0);
   c.active.set(false);settle(c);
   check(c.operations.offCalls==1&&!c.pendingOff()&&!c.armed());
   for(Boolean on:c.groupOn)check(Boolean.FALSE.equals(on));
   check(c.barrier==barrier&&c.barrierPhase==LensStatus.Phase.UNKNOWN);
   check(LensService.starts==0&&c.operations.context.stops==1);
   c.operations.fail=true;c.allOff();settle(c);check(c.pendingOff());
   for(Boolean on:c.groupOn)check(on==null);
   // Simulate process recreation using the same durable App preferences.
   restarted=make(prefs);restarted.refreshGroups();settle(restarted);
   check(!restarted.pendingOff()&&!restarted.armed());
   check(restarted.operations.offCalls==1&&restarted.operations.readCalls==0);
   for(Boolean on:restarted.groupOn)check(Boolean.FALSE.equals(on));
   prefs.fail=true;int calls=restarted.operations.offCalls;restarted.allOff();
   check(restarted.operations.offCalls==calls&&restarted.reason.contains("未保存"));
   Prefs updatePrefs=new Prefs();updatePrefs.values.put("display-auto-arm",true);deferred=make(updatePrefs);
   Arrays.fill(deferred.groupOn,true);deferred.gpsOn=false;deferred.operations.game=barrier;deferred.operations.front=true;
   java.io.File directory=deferred.operations.context.getFilesDir();check(UpdateInterlock.freeze(directory,41));
   deferred.monitor();settle(deferred);check(deferred.autoAttempt==null&&deferred.operations.enableCalls==0);
   check(UpdateInterlock.clear(directory));deferred.operations.allowEnable=true;
   deferred.monitor();settle(deferred);check(deferred.autoAttempt.same(barrier)&&deferred.operations.enableCalls==1);
   Arrays.fill(deferred.groupOn,false);deferred.gpsOn=true;deferred.autoAttempt=null;
   check(deferred.groupsKnown()&&deferred.anyGroup());deferred.monitor();settle(deferred);check(deferred.operations.enableCalls==2);
   deferred.allOff();settle(deferred);check(Boolean.FALSE.equals(deferred.gpsOn)&&!deferred.anyGroup());
   deferred.operations.reportEntered=new java.util.concurrent.CountDownLatch(1);
   deferred.operations.reportRelease=new java.util.concurrent.CountDownLatch(1);
   deferred.refreshReports();check(deferred.operations.reportEntered.await(1,java.util.concurrent.TimeUnit.SECONDS));
   check(!deferred.busy()&&deferred.reportReading.get());
   deferred.setGps(true);check(deferred.busy()); // User intent queues behind a passive read.
   deferred.operations.reportRelease.countDown();settle(deferred);
   check(Boolean.TRUE.equals(deferred.gpsOn));
   deferred.allOff();settle(deferred);check(Boolean.FALSE.equals(deferred.gpsOn));
   System.out.println("AllOffControllerTest: "+checks+" real controller queue/persistence checks with host Android/root doubles; not device acceptance");
  }finally{c.worker.shutdownNow();if(restarted!=null)restarted.worker.shutdownNow();if(deferred!=null){deferred.worker.shutdownNow();java.io.File dir=deferred.operations.context.getFilesDir();for(java.io.File file:dir.listFiles())file.delete();dir.delete();}}
 }
}
