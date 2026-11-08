package dev.ody.seedpreview;
public final class CompatibilityTest {
    static int checks;
    static void expect(boolean value){checks++;if(!value)throw new AssertionError("case "+checks);}
    public static void main(String[] args){
        for(int api=28;api<=35;api++)expect(Compatibility.blockReason(api,"arm64-v8a","156.1",1793076318L)==null);
        for(int api:new int[]{27,36})expect(Compatibility.blockReason(api,"arm64-v8a","156.1",1793076318L)!=null);
        for(String abi:new String[]{"x86_64","x86","armeabi-v7a",null})expect(Compatibility.blockReason(35,abi,"156.1",1793076318L)!=null);
        for(String v:new String[]{"156","156.0","156.2",null})expect(Compatibility.blockReason(35,"arm64-v8a",v,1793076318L)!=null);
        expect(Compatibility.blockReason(35,"arm64-v8a","156.1",1793076317L)!=null);
        System.out.println("CompatibilityTest: "+checks+" passed; no device certification");
    }
}
