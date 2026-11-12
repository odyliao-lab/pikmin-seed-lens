package android.content;
import android.content.pm.*;import java.io.File;
public abstract class Context {
 public Context getApplicationContext(){return this;}public abstract SharedPreferences getSharedPreferences(String name,int mode);
 public String getPackageName(){return "dev.ody.seedpreview";}public PackageManager getPackageManager(){return new PackageManager();}
 public File getFilesDir(){throw new AssertionError("metadata must not touch APK files");}
}
