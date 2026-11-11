package dev.ody.seedpreview;
public final class GpsCopyTest {
 static int checks;static void check(boolean value){checks++;if(!value)throw new AssertionError("gps case "+checks);}
 public static void main(String[] args){
  String boot="12345678-1234-4234-8234-123456789abc";LensContext c=LensContext.parse("123\t456\t"+boot+"\n");
  check(GpsCopy.setting(0,true,"LENS_GPS\ton\n"));check(!GpsCopy.setting(0,true,"LENS_GPS\toff\n"));
  check(GpsCopy.setting(0,false,"LENS_GPS\toff\n")==null);check(GpsCopy.setting(26,true,"LENS_GPS\toff\n")==null);check(GpsCopy.setting(0,true,"LENS_GPS\toff\nextra")==null);
  String row="LENS_SESSION\t123\t456\t"+boot+"\nv1\t10000\t123\t157.0\tready\t1\t2\t0\t2\t0\t456\n";
  check(GpsCopy.report(true,c,row,10000).contains("已複製 2 次"));check(!GpsCopy.report(true,c,row,20000).contains("已複製"));
  check(!GpsCopy.report(true,c,row.replace("123\t157.0","124\t157.0"),10000).contains("已複製"));
  check(!GpsCopy.report(true,c,row.replace("\t2\t0\t456","\t3\t0\t456"),10000).contains("已複製"));
  check(GpsCopy.report(null,c,row,10000).equals("設定未確認"));
  System.out.println("GpsCopyTest: "+checks+" settings, freshness and no-false-copy checks");
 }
}
