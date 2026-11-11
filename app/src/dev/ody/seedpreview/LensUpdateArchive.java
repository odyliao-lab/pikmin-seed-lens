package dev.ody.seedpreview;
import java.io.*;
import java.security.MessageDigest;
import java.util.*;
import java.util.zip.*;
import java.nio.charset.StandardCharsets;
/** Portable checks of the single-APK native payload contract, not signature verification. */
final class LensUpdateArchive {
 static String zipSha(ZipFile zip,String name)throws Exception{ZipEntry e=zip.getEntry(name);if(e==null||e.getSize()<1||e.getSize()>LensUpdateInfo.MAX_APK)throw new IOException("native missing");
  MessageDigest digest=MessageDigest.getInstance("SHA-256");long total=0;try(InputStream in=zip.getInputStream(e)){byte[] b=new byte[32768];int n;while((n=in.read(b))!=-1){total+=n;if(total>e.getSize())throw new IOException("entry overflow");digest.update(b,0,n);}}if(total!=e.getSize())throw new IOException("entry truncated");return LensUpdateClient.hex(digest.digest());}
 static void inspect(File file)throws Exception{
  try(ZipFile zip=new ZipFile(file)){Set<String> seen=new HashSet<>();long total=0;Enumeration<? extends ZipEntry> entries=zip.entries();while(entries.hasMoreElements()){ZipEntry e=entries.nextElement();if(!seen.add(e.getName())||e.getSize()<0||(total+=e.getSize())>200L*1024*1024||seen.size()>4096)throw new IOException("ZIP shape");}
   String[][] pairs={{"seed-loader-arm64-v8a.sha256","lib/arm64-v8a/libseed_loader.so"},{"seed-loader-x86_64.sha256","lib/x86_64/libseed_loader.so"},{"seed-bridge-x86_64.sha256","lib/x86_64/libseed_bridge.so"},{"seed-bridge-renderer.sha256","assets/libseed_display.so"}};
   for(String[] pair:pairs){ZipEntry e=zip.getEntry("assets/"+pair[0]);if(e==null||e.getSize()<64||e.getSize()>65)throw new IOException("pin missing");String pin;try(InputStream in=zip.getInputStream(e)){byte[] b=new byte[66];int n=in.read(b);pin=InstalledNative.digest(new String(b,0,Math.max(n,0),StandardCharsets.US_ASCII));}if(pin==null||!pin.equals(zipSha(zip,pair[1])))throw new IOException("native pair");}
   if(!zipSha(zip,"assets/libseed_display.so").equals(zipSha(zip,"lib/arm64-v8a/libseed_display.so")))throw new IOException("renderer pair");
  }
 }
 public static void main(String[] args)throws Exception{inspect(new File(args[0]));System.out.println("Single-APK archive pairs verified; not installation or game acceptance");}
}
