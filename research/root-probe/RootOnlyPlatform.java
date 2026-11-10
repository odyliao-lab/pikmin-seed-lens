package dev.ody.seedlensrootprobe;

/** Explicit self-only Root reads, not NativeBridge/PM-loader admission. */
final class RootOnlyPlatform {
    private RootOnlyPlatform() {}
    static boolean api34(String expectedApi,int actualApi,String primaryAbi,boolean process64,
            String kernel,String packagedAbi,int processMachine,boolean explicitScope,
            boolean loadRequested) {
        return explicitScope&&!loadRequested&&"34".equals(expectedApi)&&actualApi==34&&
            process64&&"x86_64".equals(primaryAbi)&&"x86_64".equals(packagedAbi)&&
            "x86_64".equals(kernel)&&processMachine==ProcessElf.X86_64;
    }
}
