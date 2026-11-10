package dev.ody.seedlensrootprobe;

public final class ProbePlatformTest {
    private static int checks;
    private static void check(String want,String expected,int actual,String abi,boolean process64,String kernel){
        if(!want.equals(ProbePlatform.backend(expected,actual,abi,process64,kernel)))throw new AssertionError("Research platform refusal failed");
        ++checks;
    }
    public static void main(String[] args){
        check("arm64","31",31,"arm64-v8a",true,"aarch64");
        check("arm64","34",34,"arm64-v8a",true,"aarch64");
        check("x64","31",31,"x86_64",true,"x86_64");
        check("x64","35",35,"x86_64",true,"x86_64");
        for(int api:new int[]{28,30,31,32,33,34,35,36}){
            check("",Integer.toString(api),api,"arm64-v8a",true,"x86_64");
            check("",Integer.toString(api),api,"x86_64",true,"aarch64");
            check("",Integer.toString(api),api,"arm64-v8a",false,"aarch64");
        }
        check("","34",34,"x86_64",true,"x86_64");
        check("","35",35,"arm64-v8a",true,"aarch64");
        check("","32",32,"arm64-v8a",true,"aarch64");
        check("","31",34,"arm64-v8a",true,"aarch64");
        for(String expected:new String[]{null,"","034","34 ","+34","31","35"})check("",expected,34,"arm64-v8a",true,"aarch64");
        check("","34",34,null,true,"aarch64");
        check("","34",34,"arm64-v8a",true,null);
        System.out.println("ROOT_PROBE_PLATFORM_PASS "+checks+" checks; HOST ONLY");
    }
}
