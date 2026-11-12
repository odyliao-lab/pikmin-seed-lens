package android.content.pm;
public class PackageManager {
 public boolean allowed=true;public final PackageInstaller installer=new PackageInstaller();
 public boolean canRequestPackageInstalls(){return allowed;}public PackageInstaller getPackageInstaller(){return installer;}
}
