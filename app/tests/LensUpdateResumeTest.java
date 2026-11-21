package dev.ody.seedpreview;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLStreamHandler;
import java.nio.file.Files;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Queue;

/** Exercises the real downloader against bounded responses and a real partial file. */
public final class LensUpdateResumeTest {
    private static final byte[] APK={1,2,3,4,5,6};
    private static final byte[] PREFIX={1,2,3}, TAIL={4,5,6};
    private static final Queue<Response> responses=new ArrayDeque<>();
    private static final List<String> requests=new ArrayList<>();
    private static final List<Long> progress=new ArrayList<>();
    private static int checks;
    private static final class Response {
        final int status;
        final String range;
        final byte[] bytes;
        final long length;
        Response(int status,String range,byte[] bytes,long length) {
            this.status=status;
            this.range=range;
            this.bytes=bytes;
            this.length=length;
        }
    }
    private static final class Transport extends HttpURLConnection {
        private final Response response;
        Transport(URL url) {
            super(url);
            response=responses.remove();
        }
        public void connect() {}
        public void disconnect() {}
        public boolean usingProxy() { return false; }
        public int getResponseCode() {
            requests.add(getRequestProperty("Range"));
            return response.status;
        }
        public long getContentLengthLong() { return response.length; }
        public String getHeaderField(String name) {
            if(name.equals("Content-Range"))return response.range;
            return name.equals("Location")?"https://release-assets.githubusercontent.com/test":null;
        }
        public InputStream getInputStream(){return new ByteArrayInputStream(response.bytes);}
    }
    private static void check(boolean condition) {
        checks++;
        if(!condition)throw new AssertionError("resume check "+checks+" requests="+requests);
    }
    private static void feed(Response... next) {
        responses.clear();
        requests.clear();
        progress.clear();
        responses.addAll(Arrays.asList(next));
    }
    private static LensUpdateInfo target(File part)throws Exception {
        Files.write(part.toPath(),APK);
        String hash=LensUpdateClient.digest(part);
        LensUpdateClient.removePartial(part);
        return new LensUpdateInfo("schema=1\npackageName="+LensUpdateInfo.PACKAGE+"\nversionName=0.16.0\nversionCode=58\n"
                +"apkUrl=https://github.com/"+LensUpdateInfo.REPO+"/releases/download/seed-lens-v0.16.0/pikmin-seed-lens-0.16.0.apk\n"
                +"size=6\nminSdk=28\nsha256="+hash+"\nsummary=test\ncompatibility=test\n");
    }
    private static void download(LensUpdateInfo info,File part)throws Exception {
        LensUpdateClient.downloadResuming(info,part,new LensUpdateClient.Observer(){
            public boolean cancelled(){return false;}
            public void progress(long received,long total){progress.add(received);}
        });
    }
    private static void fails(LensUpdateInfo info,File part)throws Exception {
        try { download(info,part);throw new AssertionError("corrupt or short response accepted"); }
        catch(IOException expected){check(responses.isEmpty());}
    }
    public static void main(String[] args)throws Exception {
        URL.setURLStreamHandlerFactory(protocol -> protocol.equals("https")?new URLStreamHandler(){
            protected URLConnection openConnection(URL url){return new Transport(url);}
        }:null);
        File part=Files.createTempFile("lens-resume-",".part").toFile();
        try {
            LensUpdateInfo info=target(part);
            feed(new Response(200,null,PREFIX,6));
            fails(info,part);
            check(Arrays.equals(PREFIX,Files.readAllBytes(part.toPath())));
            feed(new Response(302,null,new byte[0],0), new Response(206,"bytes 3-5/6",TAIL,3));
            download(info,part);
            check(requests.equals(Arrays.asList("bytes=3-","bytes=3-")));
            check(progress.equals(Arrays.asList(3L,6L))&&Arrays.equals(APK,Files.readAllBytes(part.toPath())));
            feed();
            download(info,part);
            check(requests.isEmpty());
            for(Response invalid:new Response[]{new Response(200,null,APK,6),
                    new Response(416,null,new byte[0],0),new Response(206,"bytes 2-5/6",TAIL,3),
                    new Response(206,"bytes 3-5/7",TAIL,3),new Response(206,"bytes 3-5/6",TAIL,2)}) {
                Files.write(part.toPath(),PREFIX);
                feed(invalid,new Response(200,null,APK,6));
                download(info,part);
                check(requests.equals(Arrays.asList("bytes=3-",null)));
                check(Arrays.equals(APK,Files.readAllBytes(part.toPath())));
            }
            Files.write(part.toPath(),new byte[]{0,0,0});
            feed(new Response(206,"bytes 3-5/6",TAIL,3));
            fails(info,part);
            check(!part.exists());
            Files.write(part.toPath(),PREFIX);
            feed(new Response(206,"bytes 3-5/6",new byte[]{4,5,6,7},-1));
            fails(info,part);
            check(!part.exists());
            Files.write(part.toPath(),PREFIX);
            feed();
            try {
                LensUpdateClient.downloadResuming(info,part,new LensUpdateClient.Observer(){
                    public boolean cancelled(){return true;}
                    public void progress(long received,long total){}
                });
                throw new AssertionError("cancel ignored");
            } catch(java.io.InterruptedIOException expected) {check(requests.isEmpty());}
        } finally {
            Files.deleteIfExists(part.toPath());
        }
        System.out.println("PASS UpdateResume: "+checks+" bounded range, fallback, partial retention, digest and cancellation assertions");
    }
}
