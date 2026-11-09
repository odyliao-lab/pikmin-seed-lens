package dev.ody.seedpreview;

import android.app.*;
import android.os.*;
import android.widget.*;
import android.content.pm.PackageInfo;
import android.content.Intent;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;

/** Explicit post-login prototype. No silent startup, game launch, inventory or scanner writes. */
public final class MainActivity extends Activity {
    static final AtomicBoolean operationActive=new AtomicBoolean();
    final ExecutorService worker=Executors.newSingleThreadExecutor();
    TextView status;Button enable,disable,refresh;CheckBox map,contain;
    GameSession confirmedGame;
    int confirmationGeneration;
    static final String READ_GAME="set -eu\n[ $(id -u) = 0 ]\np=$(pidof com.nianticlabs.pikmin); case \"$p\" in ''|*[!0-9]*) exit 3;; esac\nt=$(sed 's/.*) //' /proc/$p/stat | awk '{print $20}')\nprintf '%s\\t%s\\n' \"$p\" \"$t\"\n";
    public void onCreate(Bundle b){super.onCreate(b);
        LinearLayout box=new LinearLayout(this);box.setOrientation(1);box.setPadding(32,32,32,32);
        String version;
        try{version=getPackageManager().getPackageInfo(getPackageName(),0).versionName;}
        catch(android.content.pm.PackageManager.NameNotFoundException e){version="版本未知";}
        TextView title=new TextView(this);title.setText("皮克敏花苗透視鏡 "+version+"（驗證原型）");title.setTextSize(23);box.addView(title);
        TextView help=new TextView(this);help.setText("僅顯示名稱／縮字，不改物品。只需 Root，不安裝額外模組。\n精確設定檔：156.1；156.0仍待實機驗證。其他版本不套用。\n登入主畫面後再確認；每次啟用綁定當下程序，離開此頁即重新確認。\n關閉會還原已綁定卡片，載入的程式留到遊戲退出。請勿與控制中心預覽並用。");box.addView(help);
        map=new CheckBox(this);map.setText("我已確認遊戲登入主畫面，其他操作已暫停");box.addView(map);
        contain=new CheckBox(this);contain.setText("若載入逾時或結果不明，允許停止這次遊戲程序；保留記錄、不自動重送，需重新開啟遊戲。");
        box.addView(contain);
        enable=new Button(this);enable.setText("回到遊戲並啟用花苗顯示");box.addView(enable);
        disable=new Button(this);disable.setText("關閉並還原顯示");box.addView(disable);
        refresh=new Button(this);refresh.setText("重新核對狀態");box.addView(refresh);
        status=new TextView(this);status.setTextIsSelectable(true);box.addView(status);setContentView(box);
        map.setOnCheckedChangeListener((button,checked)->{confirmedGame=null;confirmationGeneration++;if(checked)confirmCurrentGame();});
        enable.setOnClickListener(v->{if(!map.isChecked()||confirmedGame==null){status.setText("請重新勾選主畫面確認，並等候程序核對完成。");return;}
            if(!contain.isChecked()){status.setText("請先確認載入失敗時的停止處理；未同意不會載入。");return;}run(true,false);});
        disable.setOnClickListener(v->run(false,false));refresh.setOnClickListener(v->run(false,true));
    }
    @Override protected void onPause(){super.onPause();clearConfirmation();}
    void clearConfirmation(){confirmationGeneration++;confirmedGame=null;if(map!=null)map.setChecked(false);if(contain!=null)contain.setChecked(false);}
    void buttons(boolean active){enable.setEnabled(active);disable.setEnabled(active);refresh.setEnabled(active);map.setEnabled(active);contain.setEnabled(active);}
    void confirmCurrentGame(){
        if(!operationActive.compareAndSet(false,true)){clearConfirmation();status.setText("前一操作尚未結束，請稍後重新確認。");return;}
        final int generation=confirmationGeneration;buttons(false);status.setText("核對目前已開啟的遊戲程序…");
        worker.execute(()->{
            GameSession session=null;try{session=GameSession.parse(shell(READ_GAME,true));}catch(Exception ignored){}
            final GameSession result=session;
            runOnUiThread(()->{operationActive.set(false);buttons(true);
                if(generation!=confirmationGeneration||!map.isChecked())return;
                confirmedGame=result;
                if(result==null){clearConfirmation();status.setText("找不到唯一遊戲程序。請先手動進入遊戲主畫面，再回來確認；不會自動開啟遊戲。");}
                else status.setText("已核對本次遊戲程序。啟用前會再比對；若程序改變，需重新確認。");
            });
        });
    }
    void run(boolean on,boolean onlyRead){
        if(!operationActive.compareAndSet(false,true)){status.setText("前一操作尚未結束，請勿重試；不會再次載入。");return;}
        final GameSession consent=confirmedGame;clearConfirmation();buttons(false);status.setText("核對中…");
        worker.execute(()->{String answer;try{
            if(on&&!onlyRead){
                if(consent==null||!consent.same(GameSession.parse(shell(READ_GAME,true))))throw new IOException("session changed");
                FutureTask<Boolean> foreground=new FutureTask<>(()->{
                    if(isFinishing()||isDestroyed())return false;
                    Intent game=getPackageManager().getLaunchIntentForPackage("com.nianticlabs.pikmin");
                    if(game==null)return false;
                    try{startActivity(game);return true;}catch(RuntimeException e){return false;}
                });
                runOnUiThread(foreground);
                try{if(!foreground.get(5,TimeUnit.SECONDS))throw new IOException("foreground unavailable");}
                finally{foreground.cancel(false);}
                Thread.sleep(2000);
            }
            answer=perform(on,onlyRead,consent);
        }catch(Exception e){answer="未完成："+e.getClass().getSimpleName()+"；請重新確認主畫面，不會自動重送載入。";}
            operationActive.set(false);
            final String a=answer;runOnUiThread(()->{status.setText(a);buttons(true);});});}
    String shell(String script)throws Exception {return shell(script,false);}
    String shell(String script,boolean readOnly)throws Exception {
        File f=File.createTempFile("operation-",".sh",getFilesDir());try(FileOutputStream out=new FileOutputStream(f)){out.write(script.getBytes(StandardCharsets.UTF_8));}
        java.lang.Process p=new ProcessBuilder("su","-M","-c","sh "+f.getAbsolutePath()).redirectErrorStream(true).start();
        // Drain concurrently; bound retained output. Do NOT kill a live injector on host timeout.
        ByteArrayOutputStream bytes=new ByteArrayOutputStream();Thread t=new Thread(()->{try(InputStream in=p.getInputStream()){byte[] buf=new byte[1024];int n;while((n=in.read(buf))>=0){synchronized(bytes){if(bytes.size()<16384)bytes.write(buf,0,Math.min(n,16384-bytes.size()));}}}catch(IOException ignored){}});t.start();
        if(!p.waitFor(readOnly?10:90,TimeUnit.SECONDS)){
            if(readOnly){p.destroyForcibly();t.join(2000);f.delete();throw new IOException("read-only identity timeout");}
            runOnUiThread(()->status.setText("操作仍未返回；已鎖住重試，等待原程序結束。不會強殺載入程序。"));
            p.waitFor(); // Keep the single-operation gate held while the tracer exists.
        }
        f.delete(); // Only our completed, unique script; never an in-flight script.
        t.join(2000);synchronized(bytes){String s=bytes.toString("UTF-8");return p.exitValue()==0?s:"拒絕／失敗 ("+p.exitValue()+")\n"+s;}
    }
    boolean installed(String name){try{getPackageManager().getPackageInfo(name,0);return true;}catch(Exception e){return false;}}
    String hashAsset(String name)throws Exception{MessageDigest d=MessageDigest.getInstance("SHA-256");try(InputStream in=getAssets().open(name)){byte[] b=new byte[65536];int n;while((n=in.read(b))!=-1)d.update(b,0,n);}StringBuilder s=new StringBuilder();for(byte b:d.digest())s.append(String.format("%02x",b&255));return s.toString();}
    String loaderDigest(String abi)throws Exception{
        try(InputStream in=getAssets().open("seed-loader-"+abi+".sha256")){
            ByteArrayOutputStream out=new ByteArrayOutputStream();int c;
            while((c=in.read())!=-1){if(out.size()>=65)throw new IOException("invalid digest");out.write(c);}
            String value=InstalledNative.digest(out.toString("US-ASCII"));
            if(value==null)throw new IOException("invalid digest");return value;
        }
    }
    String perform(boolean on,boolean onlyRead,GameSession consent)throws Exception{
        if(on&&!onlyRead&&consent==null)throw new IOException("missing session confirmation");
        if(installed("dev.ody.pikmincontrol")||installed("dev.ody.pikmincontrol.mumu"))return "已安裝控制中心，為避免雙重掛接，本原型不接管。";
        String pid="p=$(pidof com.nianticlabs.pikmin); case \"$p\" in ''|*[!0-9]*) echo GAME_NOT_UNIQUE; exit 3;; esac\n";
        String abi=Build.SUPPORTED_ABIS[0];
        if(on&&!onlyRead){
            PackageInfo game=getPackageManager().getPackageInfo("com.nianticlabs.pikmin",0);
            String blocked=Compatibility.blockReason(Build.VERSION.SDK_INT,abi,game.versionName,game.getLongVersionCode());
            if(blocked!=null)return blocked;
        }
        String loader=new File(getApplicationInfo().nativeLibraryDir,"libseed_loader.so").getCanonicalPath();
        boolean bridge="x86_64".equals(abi);String payloadName=bridge?"libseed_bridge.so":"libseed_display.so";
        String payload=new File(getApplicationInfo().nativeLibraryDir,payloadName).getCanonicalPath();
        if(!InstalledNative.allowed(loader,abi,"libseed_loader.so")||(on&&!onlyRead&&!InstalledNative.allowed(payload,abi,payloadName)))return "安裝原生程式庫路徑不符，不會載入。";
        StringBuilder s=new StringBuilder("set -eu\n[ $(id -u) = 0 ]\n");
        s.append("[ $(sha256sum "+loader+" | cut -d' ' -f1) = "+loaderDigest(abi)+" ]\n");
        if(on&&!onlyRead){
            s.append("for m in pikmin_control_core pcc_seed_preview_canary; do [ ! -d /data/adb/modules/$m ] || { echo CONFLICTING_MODULE; exit 4; }; done\n");
            String payloadSha=bridge?pairedBridgeDigest():hashAsset("libseed_display.so");
            s.append("[ $(sha256sum "+payload+" | cut -d' ' -f1) = "+payloadSha+" ]\n");
        }
        s.append(pid).append("t=$(sed 's/.*) //' /proc/$p/stat | awk '{print $20}')\n");
        if(on&&!onlyRead)s.append(consent.shellGuard());
        s.append("exec ").append(loader).append(onlyRead?" --status ":on?" --enable-contained ":" --off ")
            .append("\"$p\" \"$t\"");
        if(on&&!onlyRead)s.append(" ").append(payload);
        s.append("\n");
        return shell(s.toString());
    }
    String pairedBridgeDigest()throws Exception{
        try(InputStream in=getAssets().open("seed-bridge-x86_64.sha256")){
            byte[] bytes=new byte[66];int length=0,n;while(length<bytes.length&&(n=in.read(bytes,length,bytes.length-length))>0)length+=n;
            String value=InstalledNative.digest(new String(bytes,0,length,StandardCharsets.US_ASCII));
            if(value==null)throw new IOException("invalid host digest");return value;
        }
    }
}
