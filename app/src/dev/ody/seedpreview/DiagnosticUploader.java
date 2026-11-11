package dev.ody.seedpreview;
import javax.net.ssl.HttpsURLConnection;
import java.net.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
/** Explicit one-shot upload. No background queue, redirects, credentials or retry. */
final class DiagnosticUploader {
 interface ConnectionFactory { HttpsURLConnection open()throws IOException; }
 static final class Failure extends IOException {Failure(String message){super(message);}}
 static String send(DiagnosticReport report,boolean consent)throws Exception{
  return send(report,"",consent);
 }
 static String send(DiagnosticReport report,String email,boolean consent)throws Exception{
  return send(report,email,consent,()->(HttpsURLConnection)new URL(DiagnosticEndpoint.URL).openConnection());
 }
 static String send(DiagnosticReport report,String email,boolean consent,ConnectionFactory connections)throws Exception{
  if(!consent)throw new IOException("尚未同意傳送");
  if(!DiagnosticEndpoint.configured())throw new IOException("上傳服務尚未啟用，請先匯出診斷");
  byte[] body=DiagnosticSubmission.body(report,email,consent);
  HttpsURLConnection c=connections.open();
  c.setInstanceFollowRedirects(false);c.setConnectTimeout(10000);c.setReadTimeout(10000);c.setRequestMethod("POST");c.setDoOutput(true);c.setUseCaches(false);
  c.setRequestProperty("Content-Type","application/json; charset=utf-8");c.setRequestProperty("X-Seed-Lens-Consent","preview-v1");c.setRequestProperty("X-Seed-Lens-Client","android-v1");c.setRequestProperty("User-Agent","SeedLens-Diagnostics/1");c.setFixedLengthStreamingMode(body.length);
  java.util.concurrent.ScheduledExecutorService watchdog=java.util.concurrent.Executors.newSingleThreadScheduledExecutor(r->{Thread t=new Thread(r,"diagnostic-deadline");t.setDaemon(true);return t;});
  java.util.concurrent.ScheduledFuture<?> expiry=watchdog.schedule(()->c.disconnect(),30,java.util.concurrent.TimeUnit.SECONDS);
  long deadline=System.nanoTime()+30_000_000_000L;
  try{try(OutputStream out=c.getOutputStream()){out.write(body);}int status=c.getResponseCode();if(status!=200&&status!=201)throw new Failure(status==429?"提報過於頻繁，請稍後再試。":status==409?"編號與已收內容不同；請使用原 Email 重試或重新產生報告。":status==400?"服務拒絕這份報告格式；請重新檢查環境。":status==503?"接收服務暫時無法使用，請稍後再試。":"服務未接受報告（"+status+"）。");
   ByteArrayOutputStream out=new ByteArrayOutputStream();try(InputStream in=c.getInputStream()){int ch;while(true){long remaining=deadline-System.nanoTime();if(remaining<=0)throw new java.net.SocketTimeoutException();c.setReadTimeout((int)Math.min(10000,1+(remaining-1)/1_000_000));ch=in.read();if(ch==-1)break;if(out.size()==512)throw new IOException("回應過大");out.write(ch);}}
   if(!DiagnosticReport.receipt(new String(out.toByteArray(),StandardCharsets.UTF_8),report.id))throw new Failure("無法核對回報收據，傳送結果未知。");return report.id;
  }finally{expiry.cancel(false);watchdog.shutdownNow();c.disconnect();}
 }
}
