package dev.ody.seedlensrootprobe;

/** Research-only admission; process ELF must agree with the pinned runner ABI. */
final class ProbePlatform {
    private ProbePlatform() {}
    static String backend(String expectedApi,int actualApi,String primaryAbi,boolean process64,String kernel,String packagedAbi,int processMachine){
        if(!process64||primaryAbi==null||!primaryAbi.equals(packagedAbi)||!Integer.toString(actualApi).equals(expectedApi))return "";
        if("arm64-v8a".equals(primaryAbi)&&processMachine==ProcessElf.AARCH64&&"aarch64".equals(kernel)&&(actualApi==31||actualApi==34))return "arm64";
        // MuMu API35 reports uname=aarch64 while its executable and PM ABI are x64.
        // That label alone never admits an ARM process or an additional API.
        boolean x64Kernel="x86_64".equals(kernel)||(actualApi==35&&"aarch64".equals(kernel));
        if("x86_64".equals(primaryAbi)&&processMachine==ProcessElf.X86_64&&x64Kernel&&(actualApi==31||actualApi==35))return "x64";
        return "";
    }
}
