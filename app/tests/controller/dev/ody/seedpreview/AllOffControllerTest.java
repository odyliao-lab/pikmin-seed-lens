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
  do {Handler.one();Thread.sleep(2);}while((c.busy()||!Handler.tasks.isEmpty())&&System.nanoTime()<until);
  check(!c.busy()&&Handler.tasks.isEmpty());
 }
 public static void main(String[] args)throws Exception{
  Prefs prefs=new Prefs();prefs.values.put("display-auto-arm",true);LensController c=make(prefs),restarted=null;
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
   System.out.println("AllOffControllerTest: "+checks+" real controller queue/persistence checks with host Android/root doubles; not device acceptance");
  }finally{c.worker.shutdownNow();if(restarted!=null)restarted.worker.shutdownNow();}
 }
}
