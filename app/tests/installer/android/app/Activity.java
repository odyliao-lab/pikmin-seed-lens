package android.app;
import android.content.*;import android.os.Bundle;import java.util.*;
public class Activity extends Context {
 public Intent intent;public boolean finished;public Intent reject;public final List<Intent> started=new ArrayList<>();
 public void onCreate(Bundle b){}public void onNewIntent(Intent i){}public Intent getIntent(){return intent;}public void setIntent(Intent i){intent=i;}
 public void finish(){finished=true;}public void startActivity(Intent i){if(i==reject)throw new IllegalStateException("confirmation unavailable");started.add(i);}
}
