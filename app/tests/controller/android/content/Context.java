package android.content;
public class Context {
 public static final int MODE_PRIVATE=0;
 public SharedPreferences prefs;public int stops;
 public java.io.File getFilesDir(){return new java.io.File(System.getProperty("java.io.tmpdir"));}
 public Context getApplicationContext(){return this;}
 public SharedPreferences getSharedPreferences(String name,int mode){return prefs;}
 public boolean stopService(Intent intent){stops++;return true;}
}
