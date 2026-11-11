package dev.ody.seedpreview;
import java.util.Arrays;
public final class RootCommandTest {
    public static void main(String[] args){int checks=0;
        for(int api:new int[]{28,31,32,33,34,35,36})for(String abi:new String[]{null,"x86_64","arm64-v8a","x86_64 "}){
            boolean native32=api==32&&"x86_64".equals(abi);
            String[] expected=native32?new String[]{"su","-c","pinned-command"}:new String[]{"su","-M","-c","pinned-command"};
            if(!Arrays.equals(expected,RootCommand.arguments(api,abi,"pinned-command")))throw new AssertionError("Root command selection changed");
            if(native32!=RootCommand.prefix(api,abi).contains("ROOT_NAMESPACE_MISMATCH"))throw new AssertionError("Root namespace guard mismatch");checks+=2;
        }
        if(!RootCommand.prefix(32,"x86_64").contains("[ -n \"$a\" ]"))throw new AssertionError("Root namespace guard must quote empty values");
        checks++;
        System.out.println("RootCommandTest: "+checks+" passed; HOST ONLY");
    }
}
