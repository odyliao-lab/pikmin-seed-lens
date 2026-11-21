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
    /** The caller owns and binds the partial file to this exact release before entering. */
    static void downloadResuming(LensUpdateInfo info, File part, Observer observer) throws Exception {
        long deadline=System.nanoTime()+300_000_000_000L;
        long offset=part.length();
        if(offset>info.size) {
            removePartial(part);
            offset=0;
        }
        try {
            remainingTimeout(deadline,observer,System::nanoTime);
            if(offset<info.size && !transfer(info,part,observer,deadline,offset)) {
                // A full response, stale range or unsupported range. Retry at
                // zero within the original deadline, never append that body.
                transfer(info,part,observer,deadline,0);
            }
            remainingTimeout(deadline,observer,System::nanoTime);
            if(part.length()!=info.size || !info.sha256.equals(digest(part)))
                throw new ProtocolException("下載檔案校驗失敗，請重新下載");
        } catch(ProtocolException invalid) {
            removePartial(part);
            throw invalid;
        }
    }
    private static boolean transfer(LensUpdateInfo info, File part, Observer observer, long deadline, long offset) throws Exception {
        try(FileOutputStream out=new FileOutputStream(part,offset>0)) {
            try {
                if(observer!=null)observer.progress(offset,info.size);
                return readResponse(info.url,out,info.size,info.size,observer,deadline,System::nanoTime,offset);
            } finally {
                out.getFD().sync();
            }
        }
    }
    static void removePartial(File part) throws IOException {
        if(part.exists()&&!part.delete())throw new IOException("無法清除舊的下載片段");
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
        readResponse(address,out,limit,expected,observer,clock.getAsLong()+budget*1_000_000,clock,0);
    }
    private static boolean readResponse(String address, OutputStream out, long limit, long expected,
            Observer observer, long deadline, LongSupplier clock, long offset) throws Exception {
        for(int redirect=0;redirect<=5;redirect++) {
            int timeout=remainingTimeout(deadline,observer,clock);
            if(!LensUpdateInfo.allowedRedirect(address))throw new IOException("下載轉址來源不允許");
            HttpURLConnection c=(HttpURLConnection)new URL(address).openConnection();
            c.setInstanceFollowRedirects(false); c.setConnectTimeout(timeout);c.setReadTimeout(timeout);
            c.setRequestProperty("User-Agent","Pikmin-Seed-Lens-Updater");c.setRequestProperty("Accept-Encoding","identity");
            if(offset>0)c.setRequestProperty("Range","bytes="+offset+"-");
            c.setUseCaches(false);
            try {
                int status=c.getResponseCode();
                c.setReadTimeout(remainingTimeout(deadline,observer,clock));
                if(status==301||status==302||status==303||status==307||status==308) {
                    String location=c.getHeaderField("Location");if(location==null)throw new IOException("更新轉址無效");
                    address=new URL(new URL(address),location).toString();continue;
                }
                long length=c.getContentLengthLong();
                if(offset>0 && (status==200 || status==416 || (status==206 &&
                        !validRange(c.getHeaderField("Content-Range"),length,offset,expected))))return false;
                if(status!=(offset>0?206:200))throw new IOException(status==404?"尚無可用的正式更新":status==403||status==429?"更新服務暫時限制請求，稍後再試":"更新服務回應 "+status);
                String encoding=c.getHeaderField("Content-Encoding");
                if(encoding!=null&&!"identity".equalsIgnoreCase(encoding))
                    throw new ProtocolException("更新檔案編碼不符");
                if(length>limit-offset)throw new ProtocolException("下載大小超出限制");
                if(expected>=0 && length>=0 && length!=expected-offset)
                    throw new ProtocolException("下載大小與版本資料不符");
                long received=offset;
                try(InputStream in=c.getInputStream()) {
                    byte[] buffer=new byte[32768];int n;
                    while(true) {
                        c.setReadTimeout(remainingTimeout(deadline,observer,clock));
                        n=in.read(buffer);
                        remainingTimeout(deadline,observer,clock);
                        if(n==-1)break;
                        received+=n;if(received>limit)throw new ProtocolException("下載大小超出限制");
                        out.write(buffer,0,n);if(observer!=null)observer.progress(received,expected);
                    }
                }
                remainingTimeout(deadline,observer,clock);
                if(expected>=0 && received!=expected)throw new IOException("下載尚未完整");
                return true;
            } finally {c.disconnect();}
        }
        throw new IOException("更新轉址次數過多");
    }
    private static boolean validRange(String range,long length,long offset,long expected) {
        return ("bytes "+offset+"-"+(expected-1)+"/"+expected).equals(range)
                && (length<0||length==expected-offset);
    }
    private static int remainingTimeout(long deadline,Observer observer,LongSupplier clock) throws IOException {
        if(Thread.currentThread().isInterrupted() || (observer!=null && observer.cancelled()))throw new InterruptedIOException("已取消下載");
        long remaining=deadline-clock.getAsLong();
        if(remaining<=0)throw new SocketTimeoutException("更新連線逾時");
        return (int)Math.min(10000,1+(remaining-1)/1_000_000);
    }
}
