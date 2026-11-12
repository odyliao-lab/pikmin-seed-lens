package dev.ody.seedpreview;
final class LensUpdateClient {
 static String raw;static int calls;static boolean fail;static java.util.concurrent.CountDownLatch entered,release;
 interface Observer{boolean cancelled();void progress(long n,long total);}
 static String manifest()throws Exception{calls++;if(entered!=null){entered.countDown();if(!release.await(3,java.util.concurrent.TimeUnit.SECONDS))throw new AssertionError("HTTP test blocked");}if(fail)throw new java.io.IOException("network");return raw;}
 static void download(LensUpdateInfo i,java.io.File f,Observer o){throw new AssertionError("no automatic download");}
 static String digest(java.io.File f){throw new AssertionError("no APK digest during metadata check");}
}
