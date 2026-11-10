package dev.ody.seedlensrootprobe;

/** Research-only admission. Kernel and primary ABI distinguish full ARM from translation. */
final class ProbePlatform {
    private ProbePlatform() {}
    static String backend(String expectedApi,int actualApi,String primaryAbi,boolean process64,String kernel){
        if(!process64||!Integer.toString(actualApi).equals(expectedApi))return "";
        if("arm64-v8a".equals(primaryAbi)&&"aarch64".equals(kernel)&&(actualApi==31||actualApi==34))return "arm64";
        if("x86_64".equals(primaryAbi)&&"x86_64".equals(kernel)&&(actualApi==31||actualApi==35))return "x64";
        return "";
    }
}
