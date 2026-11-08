package dev.ody.seedlensarmprobe;
import android.app.Instrumentation;
import android.os.Bundle;
import android.os.Process;
import java.io.File;
import java.io.FileOutputStream;
import java.nio.charset.StandardCharsets;
public final class ProbeInstrumentation extends Instrumentation {
    private static native long fixture(long value);
    private static native void spin();
    private File receipt;
    private void append(String value)throws Exception{
        try(FileOutputStream file=new FileOutputStream(receipt,true)){
            file.write((value+"\n").getBytes(StandardCharsets.UTF_8));file.getFD().sync();
        }
    }
    @Override public void onCreate(Bundle args){super.onCreate(args);start();}
    @Override public void onStart(){
        Bundle result=new Bundle();
        try{
            if(!"dev.ody.seedlensarmprobe".equals(getTargetContext().getPackageName()))throw new IllegalStateException();
            receipt=new File(getTargetContext().getFilesDir(),"baseline-"+Process.myPid()+".tsv");
            if(!receipt.createNewFile())throw new IllegalStateException();
            System.loadLibrary("fixture");
            if(fixture(123)!=(123L^0x53454544L))throw new IllegalStateException();
            append("BASELINE_JNI_PASS");
            Thread thread=new Thread(ProbeInstrumentation::spin,"SeedLensProbe");thread.start();
            for(int i=0;i<45;++i){Thread.sleep(1000);if(i%10==0)append("ALIVE\t"+i);}
            thread.join(1000);
            if(thread.isAlive()||fixture(456)!=(456L^0x53454544L))throw new IllegalStateException();
            append("ORIGINAL_THREAD_RETURNED_JNI_PASS");
            result.putString("probe","baseline-and-resume-pass-not-game");finish(-1,result);
        }catch(Throwable error){
            try{append("FAIL\t"+error.getClass().getSimpleName());}catch(Exception ignored){}
            result.putString("probe","failed");finish(1,result);
        }
    }
}
