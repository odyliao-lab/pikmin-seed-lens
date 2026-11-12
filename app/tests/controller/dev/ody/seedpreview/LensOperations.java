package dev.ody.seedpreview;
import android.content.Context;
import java.util.*;
import java.util.concurrent.*;
final class LensOperations {
 interface Foreground {boolean showGame()throws Exception;}
 static final class Reply {final int exit;final String output;final boolean started,complete;
  Reply(int e,String s,boolean start,boolean c){exit=e;output=s;started=start;complete=c;}}
 static final class Result {Reply reply;String version,blocked;long code;}
 final Context context;volatile int offCalls,readCalls,enableCalls;volatile boolean fail,front,allowEnable,contextFail;volatile LensContext game;
 volatile String unavailable,gpsUnavailable,reportOutput="";volatile int reportCalls;
 final Boolean[] saved={false,false,false,false,false};
 final List<String> writes=Collections.synchronizedList(new ArrayList<>());
 volatile CountDownLatch settingEntered,settingRelease,reportEntered,reportRelease;
 LensOperations(Context c){context=c;}
 String extensionAvailability(){return unavailable;}
 String gpsAvailability(){return gpsUnavailable!=null?gpsUnavailable:unavailable;}
 private void waitSetting(){if(settingEntered!=null){settingEntered.countDown();await(settingRelease);}}
 private static void await(CountDownLatch latch){try{if(!latch.await(3,TimeUnit.SECONDS))throw new AssertionError("release timed out");}catch(InterruptedException e){throw new AssertionError(e);}}
 Reply gpsCopy(Boolean enabled){return setting(4,enabled);}
 Reply setting(int i,Boolean enabled){waitSetting();readCalls++;if(fail||saved[i]==null)return new Reply(26,"incomplete",true,true);
  if(enabled!=null){saved[i]=enabled;writes.add(i+":"+enabled);}
  return new Reply(0,i==4?"LENS_GPS\t"+(saved[i]?"on":"off")+"\n":"LENS_GROUP\t"+i+"\t"+(saved[i]?"on":"off")+"\n",true,true);
 }
 Reply allOff(){offCalls++;if(fail)return new Reply(26,"incomplete",true,true);Arrays.fill(saved,false);writes.add("ALL_OFF");return new Reply(0,
  "LENS_GROUP\t0\toff\nLENS_GROUP\t1\toff\nLENS_GROUP\t2\toff\nLENS_GROUP\t3\toff\nLENS_ALL_OFF_COMMITTED\n",true,true);}
 Reply displayGroup(int g,Boolean enabled){return setting(g,enabled);}
 LensContext readContext(){if(fail||contextFail)throw new IllegalStateException("Root denied");return game;}
 Reply displayReports(LensContext c){reportCalls++;if(reportEntered!=null){reportEntered.countDown();await(reportRelease);}return new Reply(0,reportOutput,true,true);}
 boolean gameForeground(){return front;}
 Result execute(LensStatus.Action a,LensContext c,Foreground f,Runnable p){enableCalls++;if(!allowEnable)throw new AssertionError("load forbidden");Result result=new Result();result.blocked="host test refusal";return result;}
}
