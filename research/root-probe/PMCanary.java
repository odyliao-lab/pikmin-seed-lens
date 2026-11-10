package dev.ody.seedlensrootprobe;

import android.content.Context;
import android.os.Bundle;
import android.os.Process;
import android.os.SystemClock;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/** Normal App -> unchanged PATH su -M -> its own PM-installed self-probe PIE.
 * No general Root program/command argument, game target or automatic retry. */
final class PMCanary {
    private static String sha(File path)throws Exception{
        MessageDigest digest=MessageDigest.getInstance("SHA-256");
        try(InputStream input=new FileInputStream(path)){
            byte[] bytes=new byte[65536];int n;
            while((n=input.read(bytes))!=-1)digest.update(bytes,0,n);
        }
        StringBuilder value=new StringBuilder();
        for(byte b:digest.digest())value.append(String.format("%02x",b&255));
        return value.toString();
    }
    static int run(Context context,Bundle args)throws Exception{
        String pid=args.getString("load_pid"),start=args.getString("load_start"),host=args.getString("load_host");
        String timeout=args.getString("timeout_test","false");
        int api=android.os.Build.VERSION.SDK_INT;
        String abi=android.os.Build.SUPPORTED_ABIS.length==0?"":android.os.Build.SUPPORTED_ABIS[0];
        String backend=ProbePlatform.backend(args.getString("expected_api"),api,abi,Process.is64Bit(),android.system.Os.uname().machine);
        boolean arm=backend.equals("arm64");
        if(backend.isEmpty()||Process.myUid()<10000||
           !"dev.ody.seedlensrootprobe".equals(context.getPackageName())||
           pid==null||!pid.matches("[1-9][0-9]{0,9}")||start==null||!start.matches("[1-9][0-9]{0,19}")||
           host==null||!host.matches("/[A-Za-z0-9_+=~/.-]+")||
           !(timeout.equals("false")||timeout.equals("true"))||(arm&&!timeout.equals("false")))throw new IllegalArgumentException();
        File runner=new File(context.getApplicationInfo().nativeLibraryDir,"libseed_probe_runner.so");
        if(!runner.getPath().equals(runner.getCanonicalPath())||!runner.canExecute()||
           !runner.getPath().matches("/data/app/[A-Za-z0-9_+=~/.-]+/dev\\.ody\\.seedlensrootprobe-[A-Za-z0-9_+=~.-]+/lib/"+(arm?"arm64":"x86_64")+"/libseed_probe_runner\\.so"))
            throw new IllegalStateException("PM executable path mismatch");
        String expected;
        try(InputStream input=context.getAssets().open("runner.sha256")){
            byte[] data=new byte[66];int n=input.read(data);
            if(n!=65||input.read()!=-1)throw new IllegalStateException();
            expected=new String(data,0,65,StandardCharsets.US_ASCII);
        }
        if(!expected.matches("[0-9a-f]{64}\\n")||!sha(runner).equals(expected.trim()))throw new IllegalStateException("PM executable digest mismatch");
        File receipt=new File(context.getFilesDir(),"pm-"+Process.myPid()+"-"+SystemClock.elapsedRealtime()+".tsv");
        if(!receipt.createNewFile())throw new IllegalStateException();
        // All fields are restricted above; no quotes, spaces or shell expansion.
        String command=runner.getPath()+" "+pid+" "+start+" "+(arm?host+" "+api:api+" "+host)+(timeout.equals("true")?" --timeout-test":"");
        try(FileOutputStream output=new FileOutputStream(receipt,true)){
            output.write(("APP_UID\t"+Process.myUid()+"\nRUNNER_SHA\t"+expected+"TARGET\t"+pid+"\t"+start+"\nONE_COMMAND_ONLY\n").getBytes(StandardCharsets.UTF_8));
            output.getFD().sync();
            java.lang.Process child=new ProcessBuilder("su","-M","-c",command).redirectErrorStream(true).start();
            AtomicBoolean failed=new AtomicBoolean(false);
            ByteArrayOutputStream retained=new ByteArrayOutputStream();
            Thread reader=new Thread(()->{
                try(InputStream input=child.getInputStream()){
                    byte[] bytes=new byte[1024];int n;
                    while((n=input.read(bytes))!=-1)synchronized(retained){
                        if(retained.size()+n<=32768)retained.write(bytes,0,n);else failed.set(true);
                    }
                }catch(Exception error){failed.set(true);}
            });reader.start();
            if(!child.waitFor(90,TimeUnit.SECONDS)){
                output.write("STILL_RUNNING_NO_KILL_NO_RETRY\n".getBytes(StandardCharsets.US_ASCII));output.getFD().sync();
                child.waitFor(); // Never kill an in-flight loader on App timeout.
            }
            reader.join(2000);
            synchronized(retained){retained.writeTo(output);}
            output.write(("\nLOADER_EXIT\t"+child.exitValue()+"\nRETURNED_NOT_READY_OR_GAME_ACCEPTANCE\n").getBytes(StandardCharsets.US_ASCII));output.getFD().sync();
            if(reader.isAlive()||failed.get())throw new IOException("Incomplete loader output; no retry");
            return child.exitValue();
        }
    }
}
