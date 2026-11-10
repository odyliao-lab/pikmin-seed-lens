package dev.ody.seedlensarmprobe;
import android.app.Instrumentation;
import android.os.Bundle;
import android.os.Build;
import android.os.Process;
import android.system.Os;
import java.io.File;
import java.io.FileOutputStream;
import java.nio.charset.StandardCharsets;
public final class ProbeInstrumentation extends Instrumentation {
    private static native long fixture(long value);
    private static native int nativeApi();
    private static native void spin();
    private Bundle arguments;
    private File receipt;
    private void append(String value)throws Exception{
        try(FileOutputStream file=new FileOutputStream(receipt,true)){
            file.write((value+"\n").getBytes(StandardCharsets.UTF_8));file.getFD().sync();
        }
    }
    @Override public void onCreate(Bundle args){arguments=args;super.onCreate(args);start();}
    @Override public void onStart(){
        Bundle result=new Bundle();
        try{
            int api=Build.VERSION.SDK_INT;
            String expected=arguments==null?null:arguments.getString("expected_api");
            String machine=Os.uname().machine;
            if(!"dev.ody.seedlensarmprobe".equals(getTargetContext().getPackageName())||Process.myUid()<10000||
               !(api==31||api==34)||!Integer.toString(api).equals(expected)||!Process.is64Bit()||
               Build.SUPPORTED_ABIS.length==0||!"arm64-v8a".equals(Build.SUPPORTED_ABIS[0])||
               !"aarch64".equals(machine))throw new IllegalStateException("Unreviewed API or non-native ARM64 target");
            receipt=new File(getTargetContext().getFilesDir(),"baseline-"+Process.myPid()+".tsv");
            if(!receipt.createNewFile())throw new IllegalStateException();
            append("START\t"+Process.myPid()+"\t"+Process.myUid()+"\t"+api+"\t"+machine);
            System.loadLibrary("fixture");
            if(nativeApi()!=api)throw new IllegalStateException("Native API mismatch");
            if(fixture(123)!=(123L^0x53454544L))throw new IllegalStateException();
            append("BASELINE_JNI_PASS");
            append("RESEARCH_FIXTURE_WINDOW_SECONDS\t120");
            Thread thread=new Thread(ProbeInstrumentation::spin,"SeedLensProbe");thread.start();
            for(int i=0;i<135;++i){Thread.sleep(1000);if(i%10==0)append("ALIVE\t"+i);}
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
