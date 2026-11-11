package dev.ody.seedpreview;

import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.Locale;
import java.util.function.LongSupplier;

/** HTTPS only, bounded reads/deadline, no game/device identifiers or authentication headers. */
final class LensUpdateClient {
    interface Observer { boolean cancelled(); void progress(long received,long total); }
    static String manifest() throws Exception {
        ByteArrayOutputStream out=new ByteArrayOutputStream();
        read(LensUpdateInfo.MANIFEST_URL,out,16384,-1,null,30_000);
        return new String(out.toByteArray(),StandardCharsets.UTF_8);
    }
    static void download(LensUpdateInfo info,File part,Observer observer) throws Exception {
        try(FileOutputStream out=new FileOutputStream(part)) {
            read(info.url,out,info.size,info.size,observer,5*60_000);
            out.getFD().sync();
        } catch(Exception error){part.delete();throw error;}
        if(!info.sha256.equals(digest(part))){part.delete();throw new IOException("下載檔案校驗失敗");}
    }
    static String digest(File file) throws Exception {
        MessageDigest digest=MessageDigest.getInstance("SHA-256");
        try(InputStream in=new FileInputStream(file)){byte[] bytes=new byte[32768];int n;while((n=in.read(bytes))!=-1)digest.update(bytes,0,n);}
        return hex(digest.digest());
    }
    static String hex(byte[] bytes){StringBuilder text=new StringBuilder();for(byte b:bytes)text.append(String.format(Locale.US,"%02x",b&255));return text.toString();}
    private static void read(String address,OutputStream out,long limit,long expected,Observer observer,long budget) throws Exception {
        read(address,out,limit,expected,observer,budget,System::nanoTime);
    }
    // Clock injection keeps deadline regressions deterministic without network access.
    static void read(String address,OutputStream out,long limit,long expected,Observer observer,long budget,LongSupplier clock) throws Exception {
        long deadline=clock.getAsLong()+budget*1_000_000;
        for(int redirect=0;redirect<=5;redirect++) {
            int timeout=remainingTimeout(deadline,observer,clock);
            if(!LensUpdateInfo.allowedRedirect(address))throw new IOException("下載轉址來源不允許");
            HttpURLConnection c=(HttpURLConnection)new URL(address).openConnection();
            c.setInstanceFollowRedirects(false); c.setConnectTimeout(timeout);c.setReadTimeout(timeout);
            c.setRequestProperty("User-Agent","Pikmin-Seed-Lens-Updater");c.setRequestProperty("Accept-Encoding","identity");
            c.setUseCaches(false);
            try {
                int status=c.getResponseCode();
                c.setReadTimeout(remainingTimeout(deadline,observer,clock));
                if(status==301||status==302||status==303||status==307||status==308) {
                    String location=c.getHeaderField("Location");if(location==null)throw new IOException("更新轉址無效");
                    address=new URL(new URL(address),location).toString();continue;
                }
                if(status!=200)throw new IOException(status==404?"尚無可用的正式更新":status==403||status==429?"更新服務暫時限制請求，稍後再試":"更新服務回應 "+status);
                if(c.getContentLengthLong()>limit)throw new IOException("下載大小超出限制");
                if(expected>=0 && c.getContentLengthLong()>=0 && c.getContentLengthLong()!=expected)throw new IOException("下載大小與版本資料不符");
                long received=0;
                try(InputStream in=c.getInputStream()) {
                    byte[] buffer=new byte[32768];int n;
                    while(true) {
                        c.setReadTimeout(remainingTimeout(deadline,observer,clock));
                        n=in.read(buffer);
                        remainingTimeout(deadline,observer,clock);
                        if(n==-1)break;
                        received+=n;if(received>limit)throw new IOException("下載大小超出限制");
                        out.write(buffer,0,n);if(observer!=null)observer.progress(received,expected);
                    }
                }
                remainingTimeout(deadline,observer,clock);
                if(expected>=0 && received!=expected)throw new IOException("下載尚未完整");
                return;
            } finally {c.disconnect();}
        }
        throw new IOException("更新轉址次數過多");
    }
    private static int remainingTimeout(long deadline,Observer observer,LongSupplier clock) throws IOException {
        if(Thread.currentThread().isInterrupted() || (observer!=null && observer.cancelled()))throw new InterruptedIOException("已取消下載");
        long remaining=deadline-clock.getAsLong();
        if(remaining<=0)throw new SocketTimeoutException("更新連線逾時");
        return (int)Math.min(10000,1+(remaining-1)/1_000_000);
    }
}
