package dev.ody.seedlensarmsignalprobe;

import android.app.Instrumentation;
import android.os.Build;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.os.Process;
import android.os.SystemClock;
import android.system.Os;
import java.io.File;
import java.io.FileOutputStream;
import java.nio.charset.StandardCharsets;

/** Normal JNI in this disposable App only; no Root, ptrace, network or game. */
public final class SignalInstrumentation extends Instrumentation {
    private Bundle arguments;
    private static native int runCase(int mode,int expectedApi,int recordFd);
    @Override public void onCreate(Bundle args) { arguments=args; super.onCreate(args); start(); }
    private static void record(FileOutputStream file,String line)throws Exception {
        file.write((line+"\n").getBytes(StandardCharsets.UTF_8)); file.getFD().sync();
    }
    @Override public void onStart() {
        Bundle result=new Bundle();
        try {
            int api=Build.VERSION.SDK_INT;
            String expected=arguments.getString("expected_api"),mode=arguments.getString("mode");
            String expectedMachine=arguments.getString("expected_machine");
            int ownMachine=ProcessElf.currentMachine();
            int selected="baseline".equals(mode)?0:"raise".equals(mode)?1:"udf".equals(mode)?2:
                         "sevl".equals(mode)?3:"pair".equals(mode)?4:
                         "wfe".equals(mode)?5:"alarm".equals(mode)?6:-1;
            if(Process.myUid()<10000||!(api==31||api==34||api==35)||
               !Integer.toString(api).equals(expected)||selected<0||
               (ownMachine!=ProcessElf.X86_64&&ownMachine!=ProcessElf.AARCH64)||
               !Integer.toString(ownMachine).equals(expectedMachine)||
               !getTargetContext().getPackageName().equals("dev.ody.seedlensarmsignalprobe"))
                throw new IllegalArgumentException("Exact own-App research arguments required");
            String stem="signal-"+mode+"-"+Process.myPid()+"-"+SystemClock.elapsedRealtime();
            File text=new File(getTargetContext().getFilesDir(),stem+".tsv");
            File binary=new File(getTargetContext().getFilesDir(),stem+".bin");
            if(!text.createNewFile()||!binary.createNewFile())throw new IllegalStateException();
            Os.chmod(text.getAbsolutePath(),0600); Os.chmod(binary.getAbsolutePath(),0600);
            try(FileOutputStream receipt=new FileOutputStream(text,true);
                ParcelFileDescriptor nativeReceipt=ParcelFileDescriptor.open(binary,
                        ParcelFileDescriptor.MODE_WRITE_ONLY|ParcelFileDescriptor.MODE_APPEND)) {
                record(receipt,"START\t"+Process.myPid()+"\t"+Process.myUid()+"\t"+api+"\t"+mode);
                record(receipt,"OWN_PROCESS_ELF_MACHINE\t"+ownMachine);
                System.loadLibrary("armsignal"); record(receipt,"LIBRARY_LOADED");
                record(receipt,"BEFORE_NATIVE\t"+mode);
                int actual=runCase(selected,api,nativeReceipt.getFd());
                if(actual!=api)throw new IllegalStateException("Native refusal: "+actual);
                record(receipt,"RETURNED\t"+mode+"\tOWN_PROBE_ONLY_NOT_GAME_ACCEPTANCE");
            }
            result.putString("signal_probe",mode+"-returned-own-probe-only"); finish(-1,result);
        } catch(Throwable error) {
            result.putString("signal_probe","failed-"+error.getClass().getSimpleName()); finish(1,result);
        }
    }
}
