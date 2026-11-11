package dev.ody.seedpreview;
import android.content.Context;
final class LensOperations {
 interface Foreground {boolean showGame()throws Exception;}
 static final class Reply {final int exit;final String output;final boolean started,complete;
  Reply(int e,String s,boolean start,boolean c){exit=e;output=s;started=start;complete=c;}}
 static final class Result {Reply reply;String version,blocked;long code;}
 final Context context;volatile int offCalls,readCalls,enableCalls;volatile boolean fail;
 LensOperations(Context c){context=c;}
 String extensionAvailability(){return null;}
 Reply allOff(){offCalls++;return new Reply(fail?26:0,fail?"incomplete":
  "LENS_GROUP\t0\toff\nLENS_GROUP\t1\toff\nLENS_GROUP\t2\toff\nLENS_GROUP\t3\toff\nLENS_ALL_OFF_COMMITTED\n",true,true);}
 Reply displayGroup(int g,Boolean enabled){readCalls++;throw new AssertionError("pending OFF must not read/import ON");}
 LensContext readContext(){return null;}
 Reply displayReports(LensContext c){throw new AssertionError("no live game");}
 boolean gameForeground(){return false;}
 Result execute(LensStatus.Action a,LensContext c,Foreground f,Runnable p){enableCalls++;throw new AssertionError("must not load while pending OFF");}
}
