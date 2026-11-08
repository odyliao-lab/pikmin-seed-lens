package dev.ody.seedpreview;

import android.app.*;
import android.os.*;
import android.widget.*;
import android.content.pm.PackageInfo;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;

/** Explicit post-login prototype. No silent startup, game launch, inventory or scanner writes. */
public final class MainActivity extends Activity {
    static final String ROOT="/data/local/tmp/pcc-seed-root-v1";
    static final String CONTROL="/data/local/tmp/pikmin-seed-preview.txt";
    static final AtomicBoolean operationActive=new AtomicBoolean();
    final ExecutorService worker=Executors.newSingleThreadExecutor();
    TextView status;Button enable,disable,refresh;CheckBox map;
    public void onCreate(Bundle b){super.onCreate(b);
        LinearLayout box=new LinearLayout(this);box.setOrientation(1);box.setPadding(32,32,32,32);
        TextView title=new TextView(this);title.setText("花苗顯示 0.1.4（驗證原型）");title.setTextSize(23);box.addView(title);
        TextView help=new TextView(this);help.setText("僅顯示名稱／縮字，不改物品。只需 Root，不安裝額外模組。\n目前遊戲設定檔：156.1。其他版本不套用。\n本原型須在登入主畫面後啟用；遊戲重開後請再次確認。\n關閉會還原已綁定卡片，載入的程式留到遊戲退出。請勿與控制中心預覽並用。");box.addView(help);
        map=new CheckBox(this);map.setText("我已確認遊戲登入主畫面，其他操作已暫停");box.addView(map);
        enable=new Button(this);enable.setText("啟用花苗顯示");box.addView(enable);
        disable=new Button(this);disable.setText("關閉並還原顯示");box.addView(disable);
        refresh=new Button(this);refresh.setText("重新核對狀態");box.addView(refresh);
        status=new TextView(this);status.setTextIsSelectable(true);box.addView(status);setContentView(box);
        enable.setOnClickListener(v->{if(!map.isChecked()){status.setText("先確認主畫面，避免登入期間載入。");return;}run(true,false);});
        disable.setOnClickListener(v->run(false,false));refresh.setOnClickListener(v->run(false,true));
    }
    void run(boolean on,boolean onlyRead){
        if(!operationActive.compareAndSet(false,true)){status.setText("前一操作尚未結束，請勿重試；不會再次載入。");return;}
        enable.setEnabled(false);disable.setEnabled(false);refresh.setEnabled(false);status.setText("核對中…");
        worker.execute(()->{String answer;try{answer=perform(on,onlyRead);}catch(Exception e){answer="未完成："+e.getClass().getSimpleName()+"；不會自動重送載入。";}
            operationActive.set(false);
            final String a=answer;runOnUiThread(()->{status.setText(a);enable.setEnabled(true);disable.setEnabled(true);refresh.setEnabled(true);});});}
    String shell(String script)throws Exception {
        File f=File.createTempFile("operation-",".sh",getFilesDir());try(FileOutputStream out=new FileOutputStream(f)){out.write(script.getBytes(StandardCharsets.UTF_8));}
        java.lang.Process p=new ProcessBuilder("su","-M","-c","sh "+f.getAbsolutePath()).redirectErrorStream(true).start();
        // Drain concurrently; bound retained output. Do NOT kill a live injector on host timeout.
        ByteArrayOutputStream bytes=new ByteArrayOutputStream();Thread t=new Thread(()->{try(InputStream in=p.getInputStream()){byte[] buf=new byte[1024];int n;while((n=in.read(buf))>=0){synchronized(bytes){if(bytes.size()<16384)bytes.write(buf,0,Math.min(n,16384-bytes.size()));}}}catch(IOException ignored){}});t.start();
        if(!p.waitFor(90,TimeUnit.SECONDS)){
            runOnUiThread(()->status.setText("操作仍未返回；已鎖住重試，等待原程序結束。不會強殺載入程序。"));
            p.waitFor(); // Keep the single-operation gate held while the tracer exists.
        }
        f.delete(); // Only our completed, unique script; never an in-flight script.
        t.join(2000);synchronized(bytes){String s=bytes.toString("UTF-8");return p.exitValue()==0?s:"拒絕／失敗 ("+p.exitValue()+")\n"+s;}
    }
    boolean installed(String name){try{getPackageManager().getPackageInfo(name,0);return true;}catch(Exception e){return false;}}
    void extract(String name)throws Exception{File f=new File(getFilesDir(),name);try(InputStream in=getAssets().open(name);FileOutputStream out=new FileOutputStream(f)){byte[] b=new byte[65536];int n;while((n=in.read(b))!=-1)out.write(b,0,n);out.getFD().sync();}}
    String hash(String name)throws Exception{MessageDigest d=MessageDigest.getInstance("SHA-256");try(InputStream in=new FileInputStream(new File(getFilesDir(),name))){byte[] b=new byte[65536];int n;while((n=in.read(b))!=-1)d.update(b,0,n);}StringBuilder s=new StringBuilder();for(byte b:d.digest())s.append(String.format("%02x",b&255));return s.toString();}
    String perform(boolean on,boolean onlyRead)throws Exception{
        if(installed("dev.ody.pikmincontrol")||installed("dev.ody.pikmincontrol.mumu"))return "已安裝控制中心，為避免雙重掛接，本原型不接管。";
        String pid="p=$(pidof com.nianticlabs.pikmin); case \"$p\" in ''|*[!0-9]*) echo GAME_NOT_UNIQUE; exit 3;; esac\n";
        String report="s=/data/user/0/com.nianticlabs.pikmin/files/seed_preview_status.tsv\nif [ -f \"$s\" ]; then awk -v p=\"$p\" -v now=\"$(date +%s)\" 'BEGIN{ok=0} $1==\"v1\" && $3==p && now*1000-$2>=-2000 && now*1000-$2<=5000 {print;ok=1} END{if(!ok)print \"NO_FRESH_SAME_PID_STATUS\"}' \"$s\"; else echo NOT_LOADED; fi\n";
        if(onlyRead)return shell(pid+report);
        if(!on)return shell("set -e\n[ -d "+ROOT+" ] && [ ! -L "+CONTROL+" ]\nprintf 'off\\n' > "+ROOT+"/control.new\nchmod 644 "+ROOT+"/control.new\nmv "+ROOT+"/control.new "+CONTROL+"\nsleep 2\n"+pid+report);
        PackageInfo game=getPackageManager().getPackageInfo("com.nianticlabs.pikmin",0);
        String abi=Build.SUPPORTED_ABIS[0];
        String blocked=Compatibility.blockReason(Build.VERSION.SDK_INT,abi,game.versionName,game.getLongVersionCode());
        if(blocked!=null)return blocked;
        String loader="seed-root-"+abi;extract(loader);extract("libseed_display.so");
        StringBuilder s=new StringBuilder("set -eu\n[ $(id -u) = 0 ]\n");
        s.append("for m in pikmin_control_core pcc_seed_preview_canary; do [ ! -d /data/adb/modules/$m ] || { echo CONFLICTING_MODULE; exit 4; }; done\n");
        s.append("[ ! -L "+ROOT+" ]\nmkdir -p "+ROOT+"\n[ $(stat -c %u "+ROOT+") = 0 ]\nchmod 755 "+ROOT+"\n");
        for(String name:new String[]{loader,"libseed_display.so"}){
            s.append("[ ! -L "+ROOT+"/"+name+" ]\ncp "+getFilesDir()+"/"+name+" "+ROOT+"/"+name+".new\n");
            s.append("[ $(sha256sum "+ROOT+"/"+name+".new | cut -d' ' -f1) = "+hash(name)+" ]\nchmod 755 "+ROOT+"/"+name+".new\nmv "+ROOT+"/"+name+".new "+ROOT+"/"+name+"\n");
        }
        s.append(pid).append("[ ! -L "+CONTROL+" ]\nprintf 'off\\n' > "+ROOT+"/control.new\nchmod 644 "+ROOT+"/control.new\nmv "+ROOT+"/control.new "+CONTROL+"\n");
        s.append("if ! grep -q '/libseed_display.so' /proc/$p/maps; then\nt=$(sed 's/.*) //' /proc/$p/stat | awk '{print $20}')\n"+ROOT+"/"+loader+" \"$p\" \"$t\" > "+ROOT+"/load.log 2>&1 || { code=$?; echo LOAD_FAILED_STAGE_$code; exit 5; }\nfi\n");
        s.append("[ \"$(pidof com.nianticlabs.pikmin)\" = \"$p\" ] || exit 6\nprintf 'on\\n' > "+ROOT+"/control.new\nchmod 644 "+ROOT+"/control.new\nmv "+ROOT+"/control.new "+CONTROL+"\nsleep 4\n").append(report);
        return shell(s.toString());
    }
}
