package dev.ody.seedpreview;
final class LensUpdateClient {
 static String raw;static int calls;static boolean fail;static java.util.concurrent.CountDownLatch entered,release;
 interface Observer{boolean cancelled();void progress(long n,long total);}
 static String manifest()throws Exception{calls++;if(entered!=null){entered.countDown();if(!release.await(3,java.util.concurrent.TimeUnit.SECONDS))throw new AssertionError("HTTP test blocked");}if(fail)throw new java.io.IOException("network");return raw;}
 static void removePartial(java.io.File f)throws java.io.IOException{java.nio.file.Files.deleteIfExists(f.toPath());}
 static int downloads;static long offset;static boolean cancelDownload;
 static void downloadResuming(LensUpdateInfo i,java.io.File f,Observer o)throws java.io.IOException{
  downloads++;offset=f.length();
  java.nio.file.Files.write(f.toPath(),new byte[]{1,2,3},java.nio.file.StandardOpenOption.CREATE,java.nio.file.StandardOpenOption.APPEND);
  if(cancelDownload)LensUpdater.cancel=true;
  throw new java.io.IOException("synthetic interrupted transfer");
 }
 static String digest(java.io.File f){throw new AssertionError("no APK digest during metadata check");}
}
