package dev.ody.seedpreview;
import android.content.Context;import java.io.*;import java.util.*;import java.util.concurrent.*;
final class LensUpdater {
 static final ExecutorService worker=Executors.newSingleThreadExecutor();static final Prefs values=new Prefs();
 static long version=56;static int verifies;static boolean reject;static final LensUpdateInfo info=new LensUpdateInfo();
 static Prefs prefs(Context c){return values;}static long installed(Context c){return version;}static boolean busy(){return LensUpdateInstaller.preparing;}
 static LensUpdateInfo latest(Context c){return info;}static File apk(Context c){return new File(c.getFilesDir(),"lens-update.apk");}
 static void verify(Context c,File f,LensUpdateInfo i)throws Exception{verifies++;if(reject)throw new IOException("verification refused");}
 static class Prefs {
  final Map<String,Object> map=new HashMap<>();boolean fail;
  int getInt(String k,int f){return (Integer)map.getOrDefault(k,f);}long getLong(String k,long f){return (Long)map.getOrDefault(k,f);}
  Editor edit(){return new Editor();}
  class Editor {
   final Map<String,Object> put=new HashMap<>();final Set<String> remove=new HashSet<>();
   Editor putInt(String k,int v){put.put(k,v);return this;}Editor putLong(String k,long v){put.put(k,v);return this;}Editor putString(String k,String v){put.put(k,v);return this;}
   Editor remove(String k){remove.add(k);return this;}boolean commit(){if(fail)return false;remove.forEach(map::remove);map.putAll(put);return true;}
  }
 }
}
