package android.os;
import java.util.concurrent.ConcurrentLinkedQueue;
public class Handler {
 public static final ConcurrentLinkedQueue<Runnable> tasks=new ConcurrentLinkedQueue<>();
 public Handler(Looper loop){}
 public boolean post(Runnable r){tasks.add(r);return true;}
 public boolean postDelayed(Runnable r,long delay){tasks.add(r);return true;}
 public static void one(){Runnable r=tasks.poll();if(r!=null)r.run();}
}
