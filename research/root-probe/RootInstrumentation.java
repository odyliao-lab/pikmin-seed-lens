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
            String endpoint=arguments==null?null:arguments.getString("su_endpoint");
            String abi=android.os.Build.SUPPORTED_ABIS.length==0?"":android.os.Build.SUPPORTED_ABIS[0];
            int processMachine=ProcessElf.currentMachine();
            String kernel=android.system.Os.uname().machine;
            String packagedAbi=PMCanary.packagedAbi(getTargetContext());
            String scope=arguments.getString("research_scope");
            boolean root32="api32_root_only".equals(scope);
            boolean rootOnly="api34_root_only".equals(scope)||root32;
            boolean pm32="api32_pm_research".equals(scope);
            boolean api32Command=(root32||pm32)&&"path_api32_c".equals(endpoint);
            boolean pm34="api34_pm_research".equals(scope);
            if(scope!=null&&!rootOnly&&!pm34&&!pm32)throw new IllegalArgumentException();
            boolean rootOnlyAllowed=root32?RootOnlyPlatform.api32(expectedApi,android.os.Build.VERSION.SDK_INT,
                abi,Process.is64Bit(),kernel,packagedAbi,processMachine,true,
                arguments.containsKey("load_pid")):RootOnlyPlatform.api34(expectedApi,android.os.Build.VERSION.SDK_INT,
                abi,Process.is64Bit(),kernel,packagedAbi,processMachine,rootOnly,
                arguments.containsKey("load_pid"));
            boolean pm32Allowed=Api32PmResearch.allowed(scope,expectedApi,android.os.Build.VERSION.SDK_INT,abi,Process.is64Bit(),kernel,packagedAbi,processMachine,arguments.containsKey("load_pid"));
            boolean pm34Allowed=Api34PmResearch.allowed(scope,expectedApi,android.os.Build.VERSION.SDK_INT,
                abi,Process.is64Bit(),kernel,packagedAbi,processMachine,arguments.containsKey("load_pid"));
            String su=dev.ody.seedpreview.RootEndpoint.command(android.os.Build.VERSION.SDK_INT,abi,
                new File("/debug_ramdisk/su").canExecute());
            if("debug_ramdisk".equals(endpoint)&&"31".equals(expectedApi))su="/debug_ramdisk/su";
            else if("path".equals(endpoint)||api32Command)su="su";
            else if(endpoint!=null&&!"auto".equals(endpoint))throw new IllegalArgumentException();
            if(!"dev.ody.seedlensrootprobe".equals(getTargetContext().getPackageName())||Process.myUid()<10000||
               expectedBoot==null||!expectedBoot.matches("[0-9a-f-]{36}")||
               (rootOnly?!rootOnlyAllowed:pm34?!pm34Allowed:pm32?!pm32Allowed:ProbePlatform.backend(expectedApi,android.os.Build.VERSION.SDK_INT,
                   abi,Process.is64Bit(),kernel,packagedAbi,processMachine).isEmpty()))throw new IllegalStateException();
            receipt=new File(getTargetContext().getFilesDir(),"root-"+Process.myPid()+"-"+SystemClock.elapsedRealtime()+".tsv");
            if(!receipt.createNewFile())throw new IllegalStateException();
            StringBuilder output=new StringBuilder();
            AtomicBoolean readFailed=new AtomicBoolean(false);
            String query="id -u; id; readlink /proc/self/ns/mnt; readlink /proc/1/ns/mnt; cat /proc/sys/kernel/random/boot_id";
            ProcessBuilder command=api32Command?new ProcessBuilder(su,"-c",query):new ProcessBuilder(su,"-M","-c",query);
            java.lang.Process child=command
                .redirectErrorStream(true).start();
            // A read-only query sends no stdin. Close it so a provider waiting
            // for EOF cannot turn an otherwise bounded query into a false hang.
            child.getOutputStream().close();
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
            String[] lines=text.trim().split("\n");
            boolean root=ended&&!reader.isAlive()&&!readFailed.get()&&child.exitValue()==0&&lines.length==5&&
                lines[0].equals("0")&&lines[1].contains("uid=0(root)")&&
                lines[2].matches("mnt:\\[[0-9]+\\]")&&lines[2].equals(lines[3])&&lines[4].equals(expectedBoot);
            try(FileOutputStream file=new FileOutputStream(receipt,true)){
                String report="APP_UID\t"+Process.myUid()+"\nAPI\t"+android.os.Build.VERSION.SDK_INT+
                    "\nRESEARCH_SCOPE\t"+(root32?"API32_ROOT_ONLY_NO_PM":pm32?"API32_PM_RESEARCH":rootOnly?"API34_ROOT_ONLY_NO_PM":pm34?"API34_PM_RESEARCH":"EXISTING_BACKEND")+
                    "\nPROCESS_ELF_MACHINE\t"+processMachine+"\nKERNEL_UNAME\t"+kernel+"\nPACKAGED_ABI\t"+packagedAbi+
                    "\nSU_ENDPOINT\t"+su+"\nSU_MODE\t"+(api32Command?"API32_NATIVE_C":"M")+
                    "\n"+(api32Command?"SU_C_EXIT":"SU_M_EXIT")+"\t"+(ended?child.exitValue():-1)+"\n"+text+"\n"+
                    (root?(api32Command?"NORMAL_APP_SU_C_PASS_NOT_GAME_ACCEPTANCE":"NORMAL_APP_SU_M_PASS_NOT_GAME_ACCEPTANCE"):"ROOT_CAPABILITY_NOT_PROVEN")+"\n";
                file.write(report.getBytes(StandardCharsets.UTF_8));file.getFD().sync();
            }
            if(root&&arguments.getString("load_pid")!=null){
                int loader=PMCanary.run(getTargetContext(),arguments);
                result.putInt("pm_loader_exit",loader);
                result.putInt("pm_client_pid",Process.myPid());
                result.putInt("pm_client_uid",Process.myUid());
                result.putString("root_probe","normal-app-pm-command-returned-not-game");
                finish(-1,result);return;
            }
            result.putString("root_probe",root?(api32Command?"normal-app-su-c-pass-not-game":"normal-app-su-M-pass-not-game"):"not-proven");finish(root?-1:1,result);
        }catch(Throwable error){
            if(receipt!=null)try(FileOutputStream file=new FileOutputStream(receipt,true)){
                file.write(("FAIL\t"+error.getClass().getSimpleName()+"\n").getBytes(StandardCharsets.UTF_8));file.getFD().sync();
            }catch(Exception ignored){}
            result.putString("root_probe","failed-"+error.getClass().getSimpleName());finish(1,result);
        }
    }
}
