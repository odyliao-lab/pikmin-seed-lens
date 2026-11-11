package dev.ody.seedpreview;

import java.net.URI;
import java.util.HashMap;
import java.util.Map;

/** Strict, bounded public release metadata; no credentials or arbitrary download locations. */
final class LensUpdateInfo {
    static final String PACKAGE = "dev.ody.seedpreview";
    static final String REPO = "odyliao-lab/pikmin-control-center-releases";
    static final String MANIFEST_URL = "https://raw.githubusercontent.com/odyliao-lab/pikmin-seed-lens" + "/main/updates/preview.properties";
    static final long MAX_APK = 100L * 1024 * 1024;
    final String version, url, sha256, summary, compatibility;
    final long code, size;
    final int minSdk;
    LensUpdateInfo(String raw) {
        if(raw==null || raw.length()>16384)throw new IllegalArgumentException("版本資料過大");
        Map<String,String> values=new HashMap<>();
        for(String line:raw.split("\\r?\\n")) {
            if(line.trim().isEmpty() || line.startsWith("#"))continue;
            int split=line.indexOf('=');
            if(split<1)throw new IllegalArgumentException("版本資料格式錯誤");
            String key=line.substring(0,split),value=line.substring(split+1).trim();
            if(values.put(key,value)!=null)throw new IllegalArgumentException("版本欄位重複");
        }
        if(values.size()!=10 || !"1".equals(values.get("schema")) || !PACKAGE.equals(values.get("packageName")))
            throw new IllegalArgumentException("不支援的版本資料");
        version=required(values,"versionName");
        if(!version.matches("[0-9]{1,4}\\.[0-9]{1,4}\\.[0-9]{1,4}"))throw new IllegalArgumentException("僅接受正式版本");
        code=number(values,"versionCode",1,Integer.MAX_VALUE);
        size=number(values,"size",1,MAX_APK);
        minSdk=(int)number(values,"minSdk",28,100);
        sha256=required(values,"sha256");
        if(!sha256.matches("[0-9a-f]{64}"))throw new IllegalArgumentException("SHA-256格式錯誤");
        url=required(values,"apkUrl");
        String expected="https://github.com/"+REPO+"/releases/download/seed-lens-v"+version+"/pikmin-seed-lens-"+version+".apk";
        if(!expected.equals(url))throw new IllegalArgumentException("更新下載來源不符");
        summary=required(values,"summary"); compatibility=required(values,"compatibility");
        if(summary.length()>500 || compatibility.length()>180 || summary.matches("(?s).*[\\p{Cntrl}].*") || compatibility.matches("(?s).*[\\p{Cntrl}].*"))throw new IllegalArgumentException("更新說明過長");
    }
    String releaseUrl(){return "https://github.com/"+REPO+"/releases/tag/seed-lens-v"+version;}
    boolean newerThan(long installed){return code>installed;}
    static boolean autoDue(boolean enabled,long now,long last) {
        return enabled && (last<=0 || now<last || now-last>=86_400_000L);
    }
    static boolean allowedRedirect(String value) {
        try {
            URI u=new URI(value); String host=u.getHost();
            return "https".equals(u.getScheme()) && u.getUserInfo()==null && (u.getPort()==-1 || u.getPort()==443)
                    && u.getFragment()==null && ("github.com".equals(host) || "release-assets.githubusercontent.com".equals(host)
                        || "objects.githubusercontent.com".equals(host) || "raw.githubusercontent.com".equals(host));
        }catch(Exception ignored){return false;}
    }
    private static String required(Map<String,String> m,String key){String v=m.get(key);if(v==null||v.isEmpty())throw new IllegalArgumentException("缺少版本欄位："+key);return v;}
    private static long number(Map<String,String> m,String key,long min,long max){
        String text=required(m,key);if(!text.matches("[0-9]+"))throw new IllegalArgumentException("數值格式錯誤");
        long n=Long.parseLong(text);if(n<min||n>max)throw new IllegalArgumentException("版本数值超出範圍");return n;
    }
}
