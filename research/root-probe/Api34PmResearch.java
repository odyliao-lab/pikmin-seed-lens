package dev.ody.seedlensrootprobe;

/** Explicit disposable-App experiment only; never the default PM backend. */
final class Api34PmResearch {
    private Api34PmResearch() {}
    static boolean allowed(String scope,String expectedApi,int actualApi,String primaryAbi,
            boolean process64,String kernel,String packagedAbi,int processMachine,boolean loadRequested){
        return "api34_pm_research".equals(scope)&&loadRequested&&
            RootOnlyPlatform.api34(expectedApi,actualApi,primaryAbi,process64,kernel,packagedAbi,
                processMachine,true,false);
    }
}
