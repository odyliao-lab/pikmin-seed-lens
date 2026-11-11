package dev.ody.seedpreview;
import android.content.Context;
import android.util.AtomicFile;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
/** A small enum-only local ring; never stores raw commands, names, IDs or diagnostics. */
final class DiagnosticStore {
 static AtomicFile file(Context c){return new AtomicFile(new File(c.getFilesDir(),"diagnostic-events.txt"));}
 static List<String> events(Context c){
  try(InputStream in=file(c).openRead()){ByteArrayOutputStream out=new ByteArrayOutputStream();byte[] b=new byte[512];int n;while((n=in.read(b))!=-1){if(out.size()+n>2048)return new ArrayList<>();out.write(b,0,n);}List<String> list=new ArrayList<>();for(String s:new String(out.toByteArray(),StandardCharsets.US_ASCII).split("\n")){if(s.isEmpty())continue;if(!DiagnosticReport.oneOf(s,DiagnosticReport.EVENTS)||list.size()==16)return new ArrayList<>();list.add(s);}return list;}catch(Exception e){return new ArrayList<>();}
 }
 static synchronized void record(Context c,String event){
  if(!DiagnosticReport.oneOf(event,DiagnosticReport.EVENTS))return;
  AtomicFile f=file(c);FileOutputStream out=null;
  try{List<String> list=events(c);list.add(event);while(list.size()>16)list.remove(0);out=f.startWrite();for(String s:list)out.write((s+"\n").getBytes(StandardCharsets.US_ASCII));f.finishWrite(out);}catch(Exception ignored){if(out!=null)f.failWrite(out);}
 }
 static String lastPhase(Context c){List<String> events=events(c);for(int i=events.size()-1;i>=0;i--){String s=events.get(i);if(s.startsWith("ENABLE_")&&!s.equals("ENABLE_BEGIN"))return s.substring(7);}return "unknown";}
}
