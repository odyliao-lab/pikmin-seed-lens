package android.content;
import java.util.*;
public class Intent {
 public static final int FLAG_ACTIVITY_NEW_TASK=0x10000000,FLAG_ACTIVITY_CLEAR_TOP=0x04000000;
 public static final String EXTRA_INTENT="android.intent.extra.INTENT";
 public Class<?> target;public String action;public int flags;public final Map<String,Object> extras=new HashMap<>();
 public Intent(){}public Intent(Context c,Class<?> t){target=t;}
 public Intent setAction(String a){action=a;return this;}public Intent addFlags(int f){flags|=f;return this;}
 public Intent putExtra(String k,int v){extras.put(k,v);return this;}public Intent putExtra(String k,Intent v){extras.put(k,v);return this;}
 public int getIntExtra(String k,int f){Object v=extras.get(k);return v instanceof Integer?(Integer)v:f;}
 @SuppressWarnings("unchecked") public <T>T getParcelableExtra(String k){return (T)extras.get(k);}
}
