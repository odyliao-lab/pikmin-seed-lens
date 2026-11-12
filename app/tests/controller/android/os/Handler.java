package android.os;
import java.util.concurrent.ConcurrentLinkedQueue;
public class Handler {
 public static final ConcurrentLinkedQueue<Runnable> tasks=new ConcurrentLinkedQueue<>();
 public Handler(Looper loop){}
 public boolean post(Runnable r){tasks.add(r);return true;}
 public static final ConcurrentLinkedQueue<Runnable> delayed=new ConcurrentLinkedQueue<>();
 public boolean postDelayed(Runnable r,long delay){delayed.add(r);return true;}
 public void removeCallbacks(Runnable r){tasks.removeIf(v->v==r);delayed.removeIf(v->v==r);}
 public static void one(){Runnable r=tasks.poll();if(r!=null)r.run();}
}
