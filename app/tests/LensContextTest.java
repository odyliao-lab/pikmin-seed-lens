package dev.ody.seedpreview;
public final class LensContextTest {
    public static void main(String[] args){
        String boot="11111111-2222-3333-4444-555555555555",valid="123\t456\t"+boot+"\n";int checks=0;
        LensContext c=LensContext.parse(valid);if(c==null||!c.record().equals(valid)||!c.same(LensContext.parse(valid)))throw new AssertionError();checks++;
        for(String bad:new String[]{"",null,valid.trim(),valid+"\n",valid.replace("\n","\r\n"),valid.replace("123","1"),valid.replace("123","-3"),valid.replace("123","00123"),valid.replace("123","2147483648"),valid.replace("456","9223372036854775808"),valid.replace(boot,"broken"),valid.replace(boot,"11111111-2222-3333-4444-55555555555g"),valid.replace(boot,boot+";id"),valid.replace("\t456\t","\t456\tmore\t")}){if(LensContext.parse(bad)!=null)throw new AssertionError("unsafe context");checks++;}
        if(c.same(LensContext.parse("123\t456\t11111111-2222-3333-4444-555555555556\n")))throw new AssertionError("boot reuse");checks++;
        if(!c.shellGuard().contains("[ \"$b\" = "+boot+" ]")||!c.shellGuard().contains("[ \"$p\" = 123 ]"))throw new AssertionError("binding lost");checks++;
        System.out.println("LensContextTest: "+checks+" boot/process binding checks passed");
    }
}
