package dev.ody.seedpreview;

/** Pure admission checks; filesystem canonicalization/owner checks are separate. */
final class InstalledNative {
    static boolean allowed(String path,String abi,String name){
        String arch="arm64-v8a".equals(abi)?"arm64":"x86_64".equals(abi)?"x86_64":null;
        if(path==null || arch==null || !("libseed_loader.so".equals(name)||"libseed_display.so".equals(name)||"libseed_bridge.so".equals(name)))return false;
        if("libseed_display.so".equals(name)&&!"arm64".equals(arch))return false;
        if("libseed_bridge.so".equals(name)&&!"x86_64".equals(arch))return false;
        return path.matches("/data/app/(~~[A-Za-z0-9_=~+\\-]+/)?dev\\.ody\\.seedpreview-[A-Za-z0-9_=~+\\-]+/lib/"+arch+"/"+name.replace(".","\\."));
    }
    static String digest(String value){
        if(value==null)return null;
        return value.matches("[0-9a-f]{64}\\n?")?value.trim():null;
    }
}
