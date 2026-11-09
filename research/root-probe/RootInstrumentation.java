package dev.ody.seedlensrootprobe;
import android.app.Instrumentation;
import android.os.Bundle;
import android.os.Process;
import android.os.SystemClock;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/** Self-only normal App Root capability test. No network, game or file writes by root. */
public final class RootInstrumentation extends Instrumentation {
    private Bundle arguments;
    @Override public void onCreate(Bundle args){arguments=args;super.onCreate(args);start();}
    @Override public void onStart(){
        Bundle result=new Bundle();File receipt=null;
        try{
            String expectedBoot=arguments==null?null:arguments.getString("expected_boot");
            String expectedApi=arguments==null?null:arguments.getString("expected_api");
            if(!"dev.ody.seedlensrootprobe".equals(getTargetContext().getPackageName())||Process.myUid()<10000||
               expectedBoot==null||!expectedBoot.matches("[0-9a-f-]{36}")||
               !("31".equals(expectedApi)||"35".equals(expectedApi))||
               android.os.Build.VERSION.SDK_INT!=Integer.parseInt(expectedApi))throw new IllegalStateException();
            receipt=new File(getTargetContext().getFilesDir(),"root-"+Process.myPid()+"-"+SystemClock.elapsedRealtime()+".tsv");
            if(!receipt.createNewFile())throw new IllegalStateException();
            StringBuilder output=new StringBuilder();
            AtomicBoolean readFailed=new AtomicBoolean(false);
            java.lang.Process child=new ProcessBuilder("su","-M","-c",
                "id -u; id; readlink /proc/self/ns/mnt; readlink /proc/1/ns/mnt; cat /proc/sys/kernel/random/boot_id")
                .redirectErrorStream(true).start();
            Thread reader=new Thread(()->{
                try(InputStream input=child.getInputStream()){
                    byte[] bytes=new byte[1024];int n;
                    while((n=input.read(bytes))!=-1){synchronized(output){
                        if(output.length()+n>16384)throw new IllegalStateException();
                        output.append(new String(bytes,0,n,StandardCharsets.UTF_8));
                    }}
                }catch(Exception error){readFailed.set(true);}
            });reader.start();
            boolean ended=child.waitFor(10,TimeUnit.SECONDS);
            if(!ended){child.destroyForcibly();child.waitFor(1,TimeUnit.SECONDS);}
            reader.join(1000);
            String text; synchronized(output){text=output.toString();}
            boolean root=ended&&!reader.isAlive()&&!readFailed.get()&&child.exitValue()==0&&text.startsWith("0\n")&&
                text.contains("uid=0(root)")&&text.contains(expectedBoot)&&text.contains("mnt:[");
            try(FileOutputStream file=new FileOutputStream(receipt,true)){
                String report="APP_UID\t"+Process.myUid()+"\nAPI\t"+android.os.Build.VERSION.SDK_INT+
                    "\nSU_M_EXIT\t"+(ended?child.exitValue():-1)+"\n"+text+"\n"+
                    (root?"NORMAL_APP_SU_M_PASS_NOT_GAME_ACCEPTANCE":"ROOT_CAPABILITY_NOT_PROVEN")+"\n";
                file.write(report.getBytes(StandardCharsets.UTF_8));file.getFD().sync();
            }
            result.putString("root_probe",root?"normal-app-su-M-pass-not-game":"not-proven");finish(root?-1:1,result);
        }catch(Throwable error){
            if(receipt!=null)try(FileOutputStream file=new FileOutputStream(receipt,true)){
                file.write(("FAIL\t"+error.getClass().getSimpleName()+"\n").getBytes(StandardCharsets.UTF_8));file.getFD().sync();
            }catch(Exception ignored){}
            result.putString("root_probe","failed-"+error.getClass().getSimpleName());finish(1,result);
        }
    }
}
