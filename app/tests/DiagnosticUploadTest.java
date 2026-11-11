package dev.ody.seedpreview;
import javax.net.ssl.HttpsURLConnection;
import java.net.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.security.cert.Certificate;
import java.util.*;
/** Synthetic reports and instrumented transport only; never contacts production in CI. */
public final class DiagnosticUploadTest {
 static int checks;static void check(boolean ok){checks++;if(!ok)throw new AssertionError("upload case "+checks);}
 static void reject(Runnable r){boolean bad=false;try{r.run();}catch(IllegalArgumentException e){bad=true;}check(bad);}
 static DiagnosticReport report(){return new DiagnosticReport("00000000-0000-4000-8000-000000000012",DiagnosticReport.emptyFacts(),Collections.emptyList(),"other");}
 static class Connection extends HttpsURLConnection {
  ByteArrayOutputStream sent=new ByteArrayOutputStream();String response;int code=201;boolean disconnected,broken;int reads;
  Connection(String response)throws Exception{super(new URL(DiagnosticEndpoint.URL));this.response=response;}
  public String getCipherSuite(){return "test-only";}public Certificate[] getLocalCertificates(){return null;}public Certificate[] getServerCertificates(){return null;}
  public void connect(){}public boolean usingProxy(){return false;}public void disconnect(){disconnected=true;}
  public OutputStream getOutputStream()throws IOException{if(broken)throw new IOException("synthetic network error");return sent;}
  public int getResponseCode(){return code;}public InputStream getInputStream(){reads++;return new ByteArrayInputStream(response.getBytes(StandardCharsets.UTF_8));}
 }
 static String receipt(DiagnosticReport r){return "{\"status\":\"accepted\",\"report_id\":\""+r.id+"\"}";}
 public static void main(String[] args)throws Exception{
  DiagnosticReport r=report();
  check(DiagnosticSubmission.email(null).isEmpty());check(DiagnosticSubmission.email("  ").isEmpty());check(DiagnosticSubmission.email(" fix+app@example.invalid ").equals("fix+app@example.invalid"));
  for(String email:new String[]{"invalid","x@y",".x@example.invalid","x.@example.invalid","x..y@example.invalid","x@example.invalid\r\nBcc:a@evil.invalid","x\"@example.invalid","x\\@example.invalid","a b@example.invalid",String.join("",Collections.nCopies(65,"a"))+"@example.invalid"})reject(()->DiagnosticSubmission.email(email));
  reject(()->DiagnosticSubmission.body(r,"",false));
  String body=new String(DiagnosticSubmission.body(r,"fix@example.invalid",true),StandardCharsets.UTF_8);check(body.contains("\"report\":"+r.json));check(body.contains("\"email\":\"fix@example.invalid\""));check(!r.json.contains("fix@example.invalid"));
  check(new String(DiagnosticSubmission.body(r,"",true),StandardCharsets.UTF_8).endsWith("\"email\":\"\"}"));
  final int[] attempts={0};
  try{DiagnosticUploader.send(r,"",false,()->{attempts[0]++;throw new IOException();});throw new AssertionError();}catch(IOException expected){check(attempts[0]==0);}
  try{DiagnosticUploader.send(r,"bad",true,()->{attempts[0]++;throw new IOException();});throw new AssertionError();}catch(IllegalArgumentException expected){check(attempts[0]==0);}
  Connection good=new Connection(receipt(r));check(DiagnosticUploader.send(r,"fix@example.invalid",true,()->{attempts[0]++;return good;}).equals(r.id));
  check(attempts[0]==1&&good.disconnected&&!good.getInstanceFollowRedirects());check(good.getRequestMethod().equals("POST"));check(good.getRequestProperty("X-Seed-Lens-Client").equals("android-v1"));check(good.getRequestProperty("X-Seed-Lens-Consent").equals("preview-v1"));check(good.sent.toString("UTF-8").equals(body));
  for(int status:new int[]{302,400,403,409,429,503}){Connection failed=new Connection(receipt(r));failed.code=status;int[] opens={0};try{DiagnosticUploader.send(r,"",true,()->{opens[0]++;return failed;});throw new AssertionError();}catch(DiagnosticUploader.Failure expected){check(opens[0]==1&&failed.disconnected&&failed.reads==0);}}
  for(String response:new String[]{"{}",receipt(r).replace(r.id,"wrong"),receipt(r).replace("}",",\"extra\":true}"),String.join("",Collections.nCopies(513,"x"))}){Connection failed=new Connection(response);try{DiagnosticUploader.send(r,"",true,()->failed);throw new AssertionError();}catch(IOException expected){check(failed.disconnected);}}
  Connection failed=new Connection("");failed.broken=true;try{DiagnosticUploader.send(r,"",true,()->failed);throw new AssertionError();}catch(IOException expected){check(failed.disconnected);}
  System.out.println("DiagnosticUploadTest: "+checks+" envelope, consent, privacy, receipt and no-retry checks");
 }
}
