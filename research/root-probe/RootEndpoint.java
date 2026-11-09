package dev.ody.seedpreview;

/** Select before an operation; never retry a failed/uncertain loader elsewhere. */
public final class RootEndpoint {
    private RootEndpoint() {}
    public static String command(int api,String primaryAbi,boolean debugMagiskExecutable){
        if(api==31&&"x86_64".equals(primaryAbi)&&debugMagiskExecutable)return "/debug_ramdisk/su";
        return "su";
    }
}
