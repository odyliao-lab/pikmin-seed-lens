package android.content;
public class Context {
 public static final int MODE_PRIVATE=0;
 public SharedPreferences prefs;public int stops;
 private java.io.File directory;
 public java.io.File getFilesDir(){try{if(directory==null)directory=java.nio.file.Files.createTempDirectory("seedlens-controller-").toFile();return directory;}catch(java.io.IOException e){throw new RuntimeException(e);}}
 public Context getApplicationContext(){return this;}
 public SharedPreferences getSharedPreferences(String name,int mode){return prefs;}
 public boolean stopService(Intent intent){stops++;return true;}
 public void startForegroundService(Intent intent){}
}
