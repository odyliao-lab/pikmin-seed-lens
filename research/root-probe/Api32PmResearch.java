package dev.ody.seedlensrootprobe;

/** Explicit disposable-App experiment only; never the default PM backend. */
final class Api32PmResearch {
    private Api32PmResearch() {}
    static boolean allowed(String scope,String expectedApi,int actualApi,String primaryAbi,
            boolean process64,String kernel,String packagedAbi,int processMachine,boolean loadRequested){
        return "api32_pm_research".equals(scope)&&loadRequested&&
            RootOnlyPlatform.api32(expectedApi,actualApi,primaryAbi,process64,kernel,packagedAbi,
                processMachine,true,false);
    }
}
