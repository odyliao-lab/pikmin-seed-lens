package dev.ody.seedlensrootprobe;

public final class Api32PmResearchTest {
    public static void main(String[] args){
        int checks=0;
        for(String scope:new String[]{null,"","api32_root_only","api32_pm_research","api32_pm_research "})
        for(String expected:new String[]{null,"","31","32","32","032","35"})
        for(int api:new int[]{31,32,34,35})
        for(boolean load:new boolean[]{false,true}){
            boolean want="api32_pm_research".equals(scope)&&"32".equals(expected)&&api==32&&load;
            if(Api32PmResearch.allowed(scope,expected,api,"x86_64",true,"aarch64","x86_64",62,load)!=want)
                throw new AssertionError("API32 explicit PM scope mismatch");
            ++checks;
        }
        for(String abi:new String[]{null,"x86_64","arm64-v8a"})
        for(String packaged:new String[]{null,"x86_64","arm64-v8a"})
        for(String kernel:new String[]{null,"x86_64","aarch64"})
        for(int machine:new int[]{0,3,62,183})
        for(boolean process64:new boolean[]{false,true}){
            boolean want="x86_64".equals(abi)&&"x86_64".equals(packaged)&&"aarch64".equals(kernel)&&machine==62&&process64;
            if(Api32PmResearch.allowed("api32_pm_research","32",32,abi,process64,kernel,packaged,machine,true)!=want)
                throw new AssertionError("API32 PM ABI mismatch");
            ++checks;
        }
        System.out.println("API32_PM_SCOPE_PASS "+checks+" checks; HOST ONLY");
    }
}
