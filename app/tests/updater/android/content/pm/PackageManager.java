package android.content.pm;
public class PackageManager{public static final int GET_SIGNING_CERTIFICATES=1;public PackageInfo getPackageInfo(String p,int flags){return new PackageInfo();}public PackageInfo getPackageArchiveInfo(String p,int flags){throw new AssertionError("no APK inspection during metadata check");}}
