package dev.ody.seedpreview;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.os.Build;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/** Same guarded native operations; no UI preference can authorize an operation. */
final class LensOperations {
    interface Foreground { boolean showGame() throws Exception; }
    static final String CONTEXT_SCRIPT="set -eu\n[ $(id -u) = 0 ]\np=$(pidof com.nianticlabs.pikmin); case \"$p\" in ''|*[!0-9]*) exit 3;; esac\nt=$(sed 's/.*) //' /proc/$p/stat | awk '{print $20}')\nb=$(cat /proc/sys/kernel/random/boot_id)\nprintf '%s\\t%s\\t%s\\n' \"$p\" \"$t\" \"$b\"\n";
    static final class Reply {
        final int exit;final String output;final boolean started,complete;
        Reply(int exit,String output,boolean started,boolean complete){this.exit=exit;this.output=output;this.started=started;this.complete=complete;}
    }
    static final class Result {
        final Reply reply;final String version,blocked;final long code;
        Result(Reply reply,String version,long code,String blocked){this.reply=reply;this.version=version;this.code=code;this.blocked=blocked;}
    }
    final Context context;
    LensOperations(Context context){this.context=context.getApplicationContext();}
    PackageInfo gameInfo()throws Exception{return context.getPackageManager().getPackageInfo("com.nianticlabs.pikmin",0);}
    String availability(){
        try{
            if(installed("dev.ody.pikmincontrol")||installed("dev.ody.pikmincontrol.mumu"))return "此環境已有控制中心，為避免雙重掛接，保留原顯示。";
            PackageInfo game=gameInfo();
            return Compatibility.blockReason(Build.VERSION.SDK_INT,Build.SUPPORTED_ABIS[0],game.versionName,game.getLongVersionCode());
        }catch(Exception e){return "尚未找到遊戲，請先安裝並由你本人完成登入。";}
    }
    LensContext readContext()throws Exception{
        Reply reply=shell(CONTEXT_SCRIPT,true,null);
        return reply.exit==0&&reply.complete?LensContext.parse(reply.output):null;
    }
    Result execute(LensStatus.Action action,LensContext consent,Foreground foreground,Runnable pending){
        String version=null;long code=0;
        try{
            PackageInfo game=gameInfo();version=game.versionName;code=game.getLongVersionCode();
            if(installed("dev.ody.pikmincontrol")||installed("dev.ody.pikmincontrol.mumu"))
                return new Result(null,version,code,"此環境已有控制中心，為避免雙重掛接，保留原顯示。");
            if(action==LensStatus.Action.ENABLE){
                String blocked=Compatibility.blockReason(Build.VERSION.SDK_INT,Build.SUPPORTED_ABIS[0],version,code);
                if(blocked!=null)return new Result(null,version,code,blocked);
                if(consent==null||!consent.same(readContext()))throw new IOException("session changed");
                if(foreground==null||!foreground.showGame())throw new IOException("foreground unavailable");
                Thread.sleep(2000);
            }
            return new Result(perform(action,consent,pending),version,code,null);
        }catch(Exception e){return new Result(new Reply(-1,"未完成："+e.getClass().getSimpleName()+"\n",false,false),version,code,null);}
    }
    private Reply shell(String script,boolean readOnly,Runnable pending)throws Exception{
        File file=File.createTempFile("operation-",".sh",context.getFilesDir());
        java.lang.Process process=null;boolean ended=false;boolean interrupted=false;
        try{
            try(FileOutputStream out=new FileOutputStream(file)){out.write(script.getBytes(StandardCharsets.UTF_8));}
            process=new ProcessBuilder("su","-M","-c","sh "+file.getAbsolutePath()).redirectErrorStream(true).start();
            final java.lang.Process child=process;ByteArrayOutputStream bytes=new ByteArrayOutputStream();
            AtomicBoolean incomplete=new AtomicBoolean(false);
            Thread reader=new Thread(()->{
                try(InputStream in=child.getInputStream()){
                    byte[] buffer=new byte[1024];int n;
                    while((n=in.read(buffer))!=-1)synchronized(bytes){
                        int keep=Math.min(n,Math.max(0,16384-bytes.size()));
                        if(keep<n)incomplete.set(true);if(keep>0)bytes.write(buffer,0,keep);
                    }
                }catch(IOException error){incomplete.set(true);}
            },"SeedLensReply");reader.setDaemon(true);reader.start();
            try{ended=process.waitFor(readOnly?10:90,TimeUnit.SECONDS);}catch(InterruptedException e){interrupted=true;}
            if(!ended&&readOnly){process.destroyForcibly();incomplete.set(true);}
            if(!ended&&!readOnly&&pending!=null){try{pending.run();}catch(RuntimeException ignored){}}
            // Once a native command has started, keep the operation gate until
            // it exits. Activity changes/host timeout never kill or replay it.
            while(!ended){try{process.waitFor();ended=true;}catch(InterruptedException e){interrupted=true;}}
            try{reader.join(2000);}catch(InterruptedException e){interrupted=true;incomplete.set(true);}
            synchronized(bytes){return new Reply(process.exitValue(),bytes.toString("UTF-8"),true,!reader.isAlive()&&!incomplete.get());}
        }catch(Exception error){
            if(process==null)throw error;
            if(readOnly&&!ended)process.destroyForcibly();
            while(!ended){try{process.waitFor();ended=true;}catch(InterruptedException e){interrupted=true;}}
            return new Reply(process.exitValue(),"操作輸出未完整取得："+error.getClass().getSimpleName()+"\n",true,false);
        }finally{
            if(process==null||ended)file.delete();
            if(interrupted)Thread.currentThread().interrupt();
        }
    }
    private boolean installed(String name){try{context.getPackageManager().getPackageInfo(name,0);return true;}catch(Exception e){return false;}}
    private String hashAsset(String name)throws Exception{
        MessageDigest digest=MessageDigest.getInstance("SHA-256");
        try(InputStream in=context.getAssets().open(name)){byte[] bytes=new byte[65536];int n;while((n=in.read(bytes))!=-1)digest.update(bytes,0,n);}
        StringBuilder value=new StringBuilder();for(byte b:digest.digest())value.append(String.format("%02x",b&255));return value.toString();
    }
    private String digestAsset(String name)throws Exception{
        try(InputStream in=context.getAssets().open(name)){
            ByteArrayOutputStream out=new ByteArrayOutputStream();int c;
            while((c=in.read())!=-1){if(out.size()>=65)throw new IOException("invalid digest");out.write(c);}
            String value=InstalledNative.digest(out.toString("US-ASCII"));
            if(value==null)throw new IOException("invalid digest");return value;
        }
    }
    private Reply perform(LensStatus.Action action,LensContext consent,Runnable pending)throws Exception{
        boolean on=action==LensStatus.Action.ENABLE,read=action==LensStatus.Action.READ;
        if(on&&consent==null)throw new IOException("missing confirmation");
        String abi=Build.SUPPORTED_ABIS[0];
        String loader=new File(context.getApplicationInfo().nativeLibraryDir,"libseed_loader.so").getCanonicalPath();
        boolean bridge="x86_64".equals(abi);String payloadName=bridge?"libseed_bridge.so":"libseed_display.so";
        String payload=new File(context.getApplicationInfo().nativeLibraryDir,payloadName).getCanonicalPath();
        if(!InstalledNative.allowed(loader,abi,"libseed_loader.so")||(on&&!InstalledNative.allowed(payload,abi,payloadName)))throw new IOException("invalid installed path");
        StringBuilder s=new StringBuilder("set -eu\n[ $(id -u) = 0 ]\n");
        s.append("[ $(sha256sum "+loader+" | cut -d' ' -f1) = "+digestAsset("seed-loader-"+abi+".sha256")+" ]\n");
        if(on){
            s.append("for m in pikmin_control_core pcc_seed_preview_canary; do [ ! -d /data/adb/modules/$m ] || { echo CONFLICTING_MODULE; exit 4; }; done\n");
            String sha=bridge?digestAsset("seed-bridge-x86_64.sha256"):hashAsset("libseed_display.so");
            s.append("[ $(sha256sum "+payload+" | cut -d' ' -f1) = "+sha+" ]\n");
        }
        s.append("p=$(pidof com.nianticlabs.pikmin); case \"$p\" in ''|*[!0-9]*) echo GAME_NOT_UNIQUE; exit 3;; esac\n")
            .append("t=$(sed 's/.*) //' /proc/$p/stat | awk '{print $20}')\nb=$(cat /proc/sys/kernel/random/boot_id)\n");
        if(on)s.append(consent.shellGuard());
        s.append("printf 'LENS_SESSION\\t%s\\t%s\\t%s\\n' \"$p\" \"$t\" \"$b\"\n");
        s.append("exec ").append(loader).append(read?" --status ":on?" --enable-contained ":" --off ").append("\"$p\" \"$t\"");
        if(on)s.append(" ").append(payload);s.append("\n");
        return shell(s.toString(),false,pending);
    }
}
