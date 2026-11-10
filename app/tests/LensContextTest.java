package dev.ody.seedpreview;
public final class LensContextTest {
    public static void main(String[] args)throws Exception{
        String boot="11111111-2222-3333-4444-555555555555",valid="123\t456\t"+boot+"\n";int checks=0;
        LensContext c=LensContext.parse(valid);if(c==null||!c.record().equals(valid)||!c.same(LensContext.parse(valid)))throw new AssertionError();checks++;
        for(String bad:new String[]{"",null,valid.trim(),valid+"\n",valid.replace("\n","\r\n"),valid.replace("123","1"),valid.replace("123","-3"),valid.replace("123","00123"),valid.replace("123","2147483648"),valid.replace("456","9223372036854775808"),valid.replace(boot,"broken"),valid.replace(boot,"11111111-2222-3333-4444-55555555555g"),valid.replace(boot,boot+";id"),valid.replace("\t456\t","\t456\tmore\t")}){if(LensContext.parse(bad)!=null)throw new AssertionError("unsafe context");checks++;}
        if(c.same(LensContext.parse("123\t456\t11111111-2222-3333-4444-555555555556\n")))throw new AssertionError("boot reuse");checks++;
        if(!c.shellGuard().contains("[ \"$b\" = "+boot+" ]")||!c.shellGuard().contains("[ \"$p\" = 123 ]"))throw new AssertionError("binding lost");checks++;
        String pref=c.preferenceRecord();
        if(pref.indexOf('\n')>=0||pref.indexOf('\t')>=0||!c.same(LensContext.preference(pref,null)))throw new AssertionError("preference roundtrip");checks++;
        if(!c.same(LensContext.preference(null,valid)))throw new AssertionError("legacy plain record");checks++;
        String xml="<map><string name='retry-barrier'>123&#9;456&#9;"+boot+"&#10;    </string></map>";
        String legacy=javax.xml.parsers.DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(new java.io.ByteArrayInputStream(xml.getBytes(java.nio.charset.StandardCharsets.UTF_8))).getElementsByTagName("string").item(0).getTextContent();
        if(LensContext.parse(legacy)!=null)throw new AssertionError("runtime parser relaxed for XML");checks++;
        if(!c.same(LensContext.preference(null,legacy)))throw new AssertionError("measured Android XML migration");checks++;
        for(String bad:new String[]{"",pref+" ",pref+"\n",pref.replace("v2:","v3:"),pref.replace(":123:",":1:"),pref.replace(":123:",":00123:"),pref+":extra",pref.replace(boot,"broken")}){
            if(LensContext.preference(bad,valid)!=null)throw new AssertionError("invalid current preference fell back");checks++;
        }
        for(String bad:new String[]{valid+"  ",valid+"        ",valid+"\n    ",valid.replace("123","00123")+"    "}){
            if(LensContext.preference(null,bad)!=null)throw new AssertionError("unsafe legacy migration");checks++;
        }
        if(LensContext.preference(null,null)!=null)throw new AssertionError("absent preference");checks++;
        System.out.println("LensContextTest: "+checks+" boot/process binding checks passed");
    }
}
