package dev.ody.seedlensarmhintprobe;

import android.app.Instrumentation;
import android.os.Bundle;
import android.os.Build;
import android.os.Process;
import android.os.SystemClock;
import java.io.File;
import java.io.FileOutputStream;
import java.nio.charset.StandardCharsets;

/** Own disposable App only: no Root, network, ptrace, game or signal interception. */
public final class HintInstrumentation extends Instrumentation {
    private Bundle arguments;
    private static native int runHint(int mode,int expectedApi);
    @Override public void onCreate(Bundle args){arguments=args;super.onCreate(args);start();}
    private static void record(FileOutputStream file,String text)throws Exception{
        file.write((text+"\n").getBytes(StandardCharsets.UTF_8));file.getFD().sync();
    }
    @Override public void onStart(){
        Bundle result=new Bundle();
        try{
            int api=Build.VERSION.SDK_INT;
            String mode=arguments.getString("mode"),expected=arguments.getString("expected_api");
            int instruction="baseline".equals(mode)?0:"sevl".equals(mode)?1:"pair".equals(mode)?2:-1;
            if(Process.myUid()<10000||!(api==31||api==34||api==35)||!Integer.toString(api).equals(expected)||instruction<0||
               !getTargetContext().getPackageName().equals("dev.ody.seedlensarmhintprobe"))throw new IllegalArgumentException();
            File receipt=new File(getTargetContext().getFilesDir(),"hint-"+mode+"-"+Process.myPid()+"-"+SystemClock.elapsedRealtime()+".tsv");
            if(!receipt.createNewFile())throw new IllegalStateException();
            try(FileOutputStream file=new FileOutputStream(receipt,true)){
                record(file,"START\t"+Process.myPid()+"\t"+Process.myUid()+"\t"+api+"\t"+mode);
                System.loadLibrary("armhint");
                record(file,"LIBRARY_LOADED");
                if(runHint(0,api)!=api)throw new IllegalStateException("Native API mismatch");
                record(file,"BASELINE_NATIVE_API_PASS");
                record(file,"BEFORE\t"+mode);
                if(runHint(instruction,api)!=api)throw new IllegalStateException("Unexpected native result");
                record(file,"RETURNED\t"+mode+"\tSELF_PROBE_ONLY_NOT_GAME_ACCEPTANCE");
            }
            result.putString("hint_probe",mode+"-returned-not-game-acceptance");finish(-1,result);
        }catch(Throwable error){result.putString("hint_probe","failed-"+error.getClass().getSimpleName());finish(1,result);}
    }
}
