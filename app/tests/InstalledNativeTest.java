package dev.ody.seedpreview;
public final class InstalledNativeTest {
    static int checks;
    static void expect(boolean value){checks++;if(!value)throw new AssertionError("case "+checks);}
    public static void main(String[] args){
        String root="/data/app/~~Ab_c==/dev.ody.seedpreview-X_1==/lib/";
        expect(InstalledNative.allowed(root+"arm64/libseed_loader.so","arm64-v8a","libseed_loader.so"));
        expect(InstalledNative.allowed(root+"x86_64/libseed_loader.so","x86_64","libseed_loader.so"));
        expect(InstalledNative.allowed(root+"arm64/libseed_display.so","arm64-v8a","libseed_display.so"));
        expect(InstalledNative.allowed(root+"x86_64/libseed_bridge.so","x86_64","libseed_bridge.so"));
        expect(!InstalledNative.allowed(root+"arm64/libseed_bridge.so","arm64-v8a","libseed_bridge.so"));
        expect(InstalledNative.allowed("/data/app/dev.ody.seedpreview-1/lib/arm64/libseed_loader.so","arm64-v8a","libseed_loader.so"));
        for(String path:new String[]{null,"/data/local/tmp/libseed_loader.so",root+"arm64/libseed_loader.so;id",
            root+"arm64/../arm64/libseed_loader.so",root+"arm64/libseed_loader.so\n",root+"arm64//libseed_loader.so",
            root+"x86_64/libseed_loader.so",root+"arm64/libseed_display.so",
            "/data/app/other/dev.ody.seedpreview-X/lib/arm64/libseed_loader.so",
            "/data/app/dev.ody.seedpreview-/lib/arm64/libseed_loader.so",
            "/data/app/dev.ody.seedpreview.fake-X/lib/arm64/libseed_loader.so"})
            expect(!InstalledNative.allowed(path,"arm64-v8a","libseed_loader.so"));
        expect(!InstalledNative.allowed(root+"x86_64/libseed_display.so","x86_64","libseed_display.so"));
        expect(!InstalledNative.allowed(root+"arm64/libseed_loader.so","armeabi-v7a","libseed_loader.so"));
        expect(!InstalledNative.allowed(root+"arm64/libseed_loader.so",null,"libseed_loader.so"));
        String sha=new String(new char[64]).replace('\0','a');
        expect(sha.equals(InstalledNative.digest(sha)));expect(sha.equals(InstalledNative.digest(sha+"\n")));
        for(String value:new String[]{null,"",sha+";id",sha+"\n\n",sha.toUpperCase(),sha.substring(1)," "+sha})expect(InstalledNative.digest(value)==null);
        System.out.println("InstalledNativeTest: "+checks+" passed; NOT PM/SELinux device acceptance");
    }
}
