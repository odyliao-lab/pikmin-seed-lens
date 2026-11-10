package dev.ody.seedlensrootprobe;

public final class RootOnlyPlatformTest {
    private static int checks;
    private static void check(boolean want,String expected,int actual,String primary,
            boolean process64,String kernel,String packaged,int machine,boolean scope,boolean load){
        if(RootOnlyPlatform.api34(expected,actual,primary,process64,kernel,packaged,machine,scope,load)!=want)
            throw new AssertionError("Self Root-only scope disagreement");
        checks++;
    }
    public static void main(String[] args){
        check(true,"34",34,"x86_64",true,"x86_64","x86_64",62,true,false);
        for(int api:new int[]{28,30,31,32,33,35,36})
            check(false,Integer.toString(api),api,"x86_64",true,"x86_64","x86_64",62,true,false);
        for(String expected:new String[]{null,"","034","34 ","+34","31","35"})
            check(false,expected,34,"x86_64",true,"x86_64","x86_64",62,true,false);
        for(int machine:new int[]{0,3,40,183,-1})
            check(false,"34",34,"x86_64",true,"x86_64","x86_64",machine,true,false);
        for(String abi:new String[]{null,"","arm64-v8a","x86","x86_64 "}){
            check(false,"34",34,abi,true,"x86_64","x86_64",62,true,false);
            check(false,"34",34,"x86_64",true,"x86_64",abi,62,true,false);
        }
        for(String kernel:new String[]{null,"","aarch64","x86_64 "})
            check(false,"34",34,"x86_64",true,kernel,"x86_64",62,true,false);
        check(false,"34",34,"x86_64",false,"x86_64","x86_64",62,true,false);
        check(false,"34",34,"x86_64",true,"x86_64","x86_64",62,false,false);
        check(false,"34",34,"x86_64",true,"x86_64","x86_64",62,true,true);
        // The ordinary loader backend still rejects precisely this platform.
        if(!ProbePlatform.backend("34",34,"x86_64",true,"x86_64","x86_64",62).isEmpty())
            throw new AssertionError("Root-only research opened PM-loader admission");
        checks++;
        System.out.println("ROOT_ONLY_PLATFORM_PASS "+checks+" checks; HOST ONLY");
    }
}
