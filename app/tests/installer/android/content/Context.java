package android.content;
import android.content.pm.PackageManager;import java.io.File;
public class Context {
 public static File directory;public static final PackageManager pm=new PackageManager();
 public Context getApplicationContext(){return this;}public File getFilesDir(){return directory;}
 public PackageManager getPackageManager(){return pm;}
}
