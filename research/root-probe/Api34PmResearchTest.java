package dev.ody.seedlensrootprobe;

public final class Api34PmResearchTest {
    public static void main(String[] args){
        int checks=0;
        for(String scope:new String[]{null,"","api34_root_only","api34_pm_research","api34_pm_research "})
        for(String expected:new String[]{null,"","31","32","34","034","35"})
        for(int api:new int[]{31,32,34,35})
        for(boolean load:new boolean[]{false,true}){
            boolean want="api34_pm_research".equals(scope)&&"34".equals(expected)&&api==34&&load;
            if(Api34PmResearch.allowed(scope,expected,api,"x86_64",true,"x86_64","x86_64",62,load)!=want)
                throw new AssertionError("API34 explicit PM scope mismatch");
            ++checks;
        }
        for(String abi:new String[]{null,"x86_64","arm64-v8a"})
        for(String packaged:new String[]{null,"x86_64","arm64-v8a"})
        for(String kernel:new String[]{null,"x86_64","aarch64"})
        for(int machine:new int[]{0,3,62,183})
        for(boolean process64:new boolean[]{false,true}){
            boolean want="x86_64".equals(abi)&&"x86_64".equals(packaged)&&"x86_64".equals(kernel)&&machine==62&&process64;
            if(Api34PmResearch.allowed("api34_pm_research","34",34,abi,process64,kernel,packaged,machine,true)!=want)
                throw new AssertionError("API34 PM ABI mismatch");
            ++checks;
        }
        System.out.println("API34_PM_SCOPE_PASS "+checks+" checks; HOST ONLY");
    }
}
