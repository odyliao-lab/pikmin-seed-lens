package dev.ody.seedpreview;
public final class CompatibilityTest {
    static int checks;
    static void expect(boolean value){checks++;if(!value)throw new AssertionError("case "+checks);}
    public static void main(String[] args){
        if(args.length>0)expect(args.length==1&&args[0].equals(Boolean.toString(Compatibility.API31_CANDIDATE)));
        for(int api=28;api<=35;api++)expect(Compatibility.blockReason(api,"arm64-v8a","156.1",1793076318L)==null);
        for(int api:new int[]{27,36})expect(Compatibility.blockReason(api,"arm64-v8a","156.1",1793076318L)!=null);
        expect(Compatibility.blockReason(35,"x86_64","156.1",1793076318L)==null);
        for(int api:new int[]{28,32,34,36})expect(Compatibility.blockReason(api,"x86_64","156.1",1793076318L)!=null);
        expect((Compatibility.blockReason(31,"x86_64","156.1",1793076318L)==null)==Compatibility.API31_CANDIDATE);
        expect((Compatibility.blockReason(31,"x86_64","156.0",1792142832L)==null)==Compatibility.API31_CANDIDATE);
        expect(Compatibility.blockReason(31,"x86_64","156.1",1793076317L)!=null);
        expect(Compatibility.blockReason(31,"x86_64","156.0",1792142833L)!=null);
        expect(Compatibility.blockReason(31,"x86_64","156.1",1792142832L)!=null);
        expect(Compatibility.blockReason(31,"x86_64","156.2",1793076318L)!=null);
        expect(Compatibility.blockReason(32,"x86_64","156.0",1792142832L)!=null);
        for(String abi:new String[]{"x86","armeabi-v7a",null})expect(Compatibility.blockReason(35,abi,"156.1",1793076318L)!=null);
        for(String v:new String[]{"156","156.0","156.2",null})expect(Compatibility.blockReason(35,"arm64-v8a",v,1793076318L)!=null);
        expect(Compatibility.blockReason(35,"arm64-v8a","156.1",1793076317L)!=null);
        expect(Compatibility.blockReason(34,"arm64-v8a","156.0",1792142832L)==null);
        expect(Compatibility.blockReason(35,"x86_64","156.0",1792142832L)==null);
        expect(Compatibility.blockReason(35,"arm64-v8a","156.1",1792142832L)!=null);
        expect(Compatibility.blockReason(35,"arm64-v8a","156.0",1792142831L)!=null);
        System.out.println("CompatibilityTest: "+checks+" passed; API31 candidate="+Compatibility.API31_CANDIDATE+"; no device certification");
    }
}
