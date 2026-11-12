package dev.ody.seedpreview;
import java.io.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.zip.*;
public final class LensUpdateTest {
 static int checks;interface Work{void run()throws Exception;}
 static void check(boolean b){checks++;if(!b)throw new AssertionError("case "+checks);}
 static void refused(Work work)throws Exception{boolean failed=false;try{work.run();}catch(Exception e){failed=true;}check(failed);}
 static File archive(Path directory,boolean corrupt,boolean missing)throws Exception{
  File file=directory.resolve(UUID.randomUUID()+".apk").toFile();
  String[][] pairs={{"seed-loader-arm64-v8a.sha256","lib/arm64-v8a/libseed_loader.so"},{"seed-loader-x86_64.sha256","lib/x86_64/libseed_loader.so"},{"seed-bridge-x86_64.sha256","lib/x86_64/libseed_bridge.so"},{"seed-bridge-renderer.sha256","assets/libseed_display.so"}};
  byte[] bytes={1,2,3,4};String hash=LensUpdateClient.hex(java.security.MessageDigest.getInstance("SHA-256").digest(bytes));
  try(ZipOutputStream zip=new ZipOutputStream(new FileOutputStream(file))){for(int i=0;i<pairs.length;i++){
    zip.putNextEntry(new ZipEntry("assets/"+pairs[i][0]));zip.write((hash+"\n").getBytes(StandardCharsets.US_ASCII));zip.closeEntry();
    if(missing&&i==1)continue;zip.putNextEntry(new ZipEntry(pairs[i][1]));zip.write(corrupt&&i==1?new byte[]{0}:bytes);zip.closeEntry();
   }zip.putNextEntry(new ZipEntry("lib/arm64-v8a/libseed_display.so"));zip.write(bytes);zip.closeEntry();}return file;
 }
 public static void main(String[] args)throws Exception{
  String raw=new String(Files.readAllBytes(Paths.get(args[0])),StandardCharsets.UTF_8);LensUpdateInfo good=new LensUpdateInfo(raw);
  check(good.code==53&&good.version.equals("0.14.2")&&good.newerThan(52)&&!good.newerThan(53));check(good.releaseUrl().contains("seed-lens-v0.14.2"));
  for(String bad:new String[]{raw+"schema=1\n",raw.replace("dev.ody.seedpreview","dev.ody.pikmincontrol"),raw.replace("seed-lens-v"+good.version,"v"+good.version),raw.replace("pikmin-seed-lens-"+good.version+".apk","other-app.apk"),raw.replace("https://github.com","http://github.com"),raw.replace("minSdk=28","minSdk=1"),raw.replace("versionCode="+good.code,"versionCode=-1"),raw.replace("size="+good.size,"size=999999999999"),raw+"token=secret\n"})refused(()->new LensUpdateInfo(bad));
  check(LensUpdateInfo.autoDue(true,86400001,1));check(!LensUpdateInfo.autoDue(false,86400001,1));check(!LensUpdateInfo.autoDue(true,10,1));
  long attempt=1_000_000;
  for(int count=1;count<=4;count++){
   long wait=count==1?300_000:count==2?1_800_000:86_400_000;
   check(!UpdateCheckPolicy.due(true,attempt+wait-1,attempt,0,count));
   check(UpdateCheckPolicy.due(true,attempt+wait,attempt,0,count));
   check(!UpdateCheckPolicy.due(false,attempt+wait,attempt,0,count));
  }
  check(UpdateCheckPolicy.due(true,attempt,0,0,0));
  check(!UpdateCheckPolicy.due(true,attempt,attempt,0,0));
  check(UpdateCheckPolicy.due(true,attempt-1,attempt,0,1));
  check(!UpdateCheckPolicy.due(true,0,attempt,0,1));
  check(!UpdateCheckPolicy.due(true,attempt+300_000,attempt,attempt,0));
  check(UpdateCheckPolicy.due(true,attempt+86_400_000,attempt,attempt,0));
  check(UpdateCheckPolicy.due(true,attempt+300_000,attempt,attempt,1));
  check(UpdateCheckPolicy.failed(Integer.MAX_VALUE)==3&&UpdateCheckPolicy.failed(-1)==1);
  check(UpdateCheckPolicy.status(52,53,99,35,"old").contains("本機版本較新"));
  check(UpdateCheckPolicy.status(53,53,28,35,"same").contains("沒有較新"));
  check(UpdateCheckPolicy.status(54,53,36,35,"new").contains("較新的 Android"));
  check(UpdateCheckPolicy.status(54,53,28,35,"new").equals("發現新版 new"));
  for(String url:new String[]{"http://github.com/x","https://evil.invalid/x","https://github.com.evil.invalid/x","https://user:pass@github.com/x","https://github.com:8443/x","file:///tmp/a"})check(!LensUpdateInfo.allowedRedirect(url));
  check(LensUpdateInfo.allowedRedirect("https://release-assets.githubusercontent.com/a?signature=temporary"));
  refused(()->LensUpdateClient.read(LensUpdateInfo.MANIFEST_URL,new ByteArrayOutputStream(),100,-1,null,0,()->1L));
  Path dir=Files.createTempDirectory("seedlens-update-test");
  try{
   UpdateInterlock.Lease load=UpdateInterlock.beginLoad(dir.toFile(),40);check(load!=null);check(!UpdateInterlock.freeze(dir.toFile(),41));check(UpdateInterlock.beginLoad(dir.toFile(),40)==null);load.close();
   check(UpdateInterlock.freeze(dir.toFile(),41));check(UpdateInterlock.beginLoad(dir.toFile(),40)==null);
   try(UpdateInterlock.Lease installed=UpdateInterlock.beginLoad(dir.toFile(),41)){check(installed!=null);}check(!Files.exists(dir.resolve("update-freeze")));
   Files.write(dir.resolve("update-freeze"),"bad".getBytes(StandardCharsets.US_ASCII));check(UpdateInterlock.beginLoad(dir.toFile(),41)==null);check(UpdateInterlock.clear(dir.toFile()));
   LensUpdateArchive.inspect(archive(dir,false,false));check(true);refused(()->LensUpdateArchive.inspect(archive(dir,true,false)));refused(()->LensUpdateArchive.inspect(archive(dir,false,true)));
  }finally{try(var files=Files.list(dir)){for(Path p:(Iterable<Path>)files::iterator)Files.delete(p);}Files.delete(dir);}
  System.out.println("LensUpdateTest: "+checks+" product/URL/archive/interlock checks; not Android install acceptance");
 }
}
