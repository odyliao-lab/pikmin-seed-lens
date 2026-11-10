package dev.ody.seedlensrootprobe;

public final class ProbePlatformTest {
    private static int checks;
    private static void check(String want,String expected,int actual,String abi,boolean process64,String kernel){
        if(!want.equals(ProbePlatform.backend(expected,actual,abi,process64,kernel,abi,
                "arm64-v8a".equals(abi)?ProcessElf.AARCH64:ProcessElf.X86_64)))throw new AssertionError("Research platform refusal failed");
        ++checks;
    }
    public static void main(String[] args){
        check("arm64","31",31,"arm64-v8a",true,"aarch64");
        check("arm64","34",34,"arm64-v8a",true,"aarch64");
        check("x64","31",31,"x86_64",true,"x86_64");
        check("x64","35",35,"x86_64",true,"x86_64");
        for(int api:new int[]{28,30,31,32,33,34,35,36}){
            check("",Integer.toString(api),api,"arm64-v8a",true,"x86_64");
            check(api==35?"x64":"",Integer.toString(api),api,"x86_64",true,"aarch64");
            check("",Integer.toString(api),api,"arm64-v8a",false,"aarch64");
        }
        check("","34",34,"x86_64",true,"x86_64");
        check("","35",35,"arm64-v8a",true,"aarch64");
        check("","32",32,"arm64-v8a",true,"aarch64");
        check("","31",34,"arm64-v8a",true,"aarch64");
        for(String expected:new String[]{null,"","034","34 ","+34","31","35"})check("",expected,34,"arm64-v8a",true,"aarch64");
        check("","34",34,null,true,"aarch64");
        check("","34",34,"arm64-v8a",true,null);
        for(String[] mismatch:new String[][]{{"31","x86_64","x86_64","arm64-v8a"},{"34","arm64-v8a","aarch64","x86_64"},{"34","arm64-v8a","aarch64",null}}){
            if(!ProbePlatform.backend(mismatch[0],Integer.parseInt(mismatch[0]),mismatch[1],true,mismatch[2],mismatch[3],"arm64-v8a".equals(mismatch[1])?ProcessElf.AARCH64:ProcessElf.X86_64).isEmpty())throw new AssertionError("Packaged ABI mismatch admitted");
            ++checks;
        }
        // Same uname label, different real process executables: never classify
        // the known MuMu API35 x64 process as native ARM.
        for(int machine:new int[]{0,3,40,-1,ProcessElf.X86_64,ProcessElf.AARCH64}){
            String arm=ProbePlatform.backend("34",34,"arm64-v8a",true,"aarch64","arm64-v8a",machine);
            String x64=ProbePlatform.backend("35",35,"x86_64",true,"aarch64","x86_64",machine);
            if(!arm.equals(machine==ProcessElf.AARCH64?"arm64":"")||
               !x64.equals(machine==ProcessElf.X86_64?"x64":""))throw new AssertionError("Process ELF disagreement admitted");
            checks+=2;
        }
        for(int api:new int[]{28,30,31,32,33,34,35,36}){
            if(!ProbePlatform.backend(Integer.toString(api),api,"x86_64",true,"aarch64","x86_64",ProcessElf.AARCH64).isEmpty())throw new AssertionError("Translated ARM executable admitted as x64");
            if(!ProbePlatform.backend(Integer.toString(api),api,"arm64-v8a",true,"aarch64","arm64-v8a",ProcessElf.X86_64).isEmpty())throw new AssertionError("Host x64 executable admitted as ARM");
            checks+=2;
        }
        for(String kernel:new String[]{null,"","arm64","aarch64 ","x86_64 "}){
            if(!ProbePlatform.backend("35",35,"x86_64",true,kernel,"x86_64",ProcessElf.X86_64).isEmpty())throw new AssertionError("Unknown kernel label admitted");
            ++checks;
        }
        System.out.println("ROOT_PROBE_PLATFORM_PASS "+checks+" checks; HOST ONLY");
    }
}
