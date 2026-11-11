package dev.ody.seedpreview;
import javax.net.ssl.HttpsURLConnection;
import java.net.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
/** Explicit one-shot upload. No background queue, redirects, credentials or retry. */
final class DiagnosticUploader {
 static String send(DiagnosticReport report,boolean consent)throws Exception{
  if(!consent)throw new IOException("尚未同意傳送");
  if(!DiagnosticEndpoint.configured())throw new IOException("上傳服務尚未啟用，請先匯出診斷");
  byte[] body=report.json.getBytes(StandardCharsets.UTF_8);if(body.length>DiagnosticReport.MAX_BYTES)throw new IOException("報告過大");
  HttpsURLConnection c=(HttpsURLConnection)new URL(DiagnosticEndpoint.URL).openConnection();
  c.setInstanceFollowRedirects(false);c.setConnectTimeout(10000);c.setReadTimeout(10000);c.setRequestMethod("POST");c.setDoOutput(true);c.setUseCaches(false);
  c.setRequestProperty("Content-Type","application/json; charset=utf-8");c.setRequestProperty("X-Seed-Lens-Consent","preview-v1");c.setFixedLengthStreamingMode(body.length);
  long deadline=System.nanoTime()+30_000_000_000L;
  try{try(OutputStream out=c.getOutputStream()){out.write(body);}int status=c.getResponseCode();if(status!=200&&status!=201)throw new IOException("服務未接受報告（"+status+"），未自動重試");
   ByteArrayOutputStream out=new ByteArrayOutputStream();try(InputStream in=c.getInputStream()){int ch;while(true){long remaining=deadline-System.nanoTime();if(remaining<=0)throw new java.net.SocketTimeoutException();c.setReadTimeout((int)Math.min(10000,1+(remaining-1)/1_000_000));ch=in.read();if(ch==-1)break;if(out.size()==512)throw new IOException("回應過大");out.write(ch);}}
   if(!DiagnosticReport.receipt(new String(out.toByteArray(),StandardCharsets.UTF_8),report.id))throw new IOException("無法核對回報收據，請保留本機編號");return report.id;
  }finally{c.disconnect();}
 }
}
