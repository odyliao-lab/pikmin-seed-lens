package dev.ody.seedlensprobe;

import android.app.Instrumentation;
import android.os.Bundle;
import android.os.Process;
import android.os.SystemClock;
import java.io.File;
import java.io.FileOutputStream;
import java.nio.charset.StandardCharsets;

/** Self-target instrumentation: no Activity, network, game access or root request. */
public final class ProbeInstrumentation extends Instrumentation {
    private static native long fixture(long input);
    private File receipt;
    private static volatile long callTicks;
    private void append(String value) throws Exception {
        try (FileOutputStream f = new FileOutputStream(receipt, true)) {
            f.write((value + "\n").getBytes(StandardCharsets.UTF_8));
            f.getFD().sync();
        }
    }
    @Override public void onCreate(Bundle args) { super.onCreate(args); start(); }
    @Override public void onStart() {
        Bundle result = new Bundle();
        try {
            if (!"dev.ody.seedlensprobe".equals(getTargetContext().getPackageName()))
                throw new IllegalStateException("wrong self target");
            receipt = new File(getTargetContext().getFilesDir(),
                "baseline-" + Process.myPid() + "-" + SystemClock.elapsedRealtime() + ".tsv");
            if (!receipt.createNewFile()) throw new IllegalStateException("receipt exists");
            append("START\t" + Process.myPid() + "\t" + android.os.Build.VERSION.SDK_INT);
            append("LIBDIR\t" + getTargetContext().getApplicationInfo().nativeLibraryDir);
            System.loadLibrary("fixture");
            for (long x : new long[]{0, 1, 0x123456789L, -1}) {
                if (fixture(x) != (x ^ 0x534545444C454E53L))
                    throw new IllegalStateException("ARM64 fixture return mismatch");
            }
            append("FIXTURE_PASS\t4");
            // Disposable user-space thread for scratch-stack / XSTATE research.
            // It is not a mechanism available in the game and is never installed
            // there. Its bounded lifetime prevents a stranded busy worker.
            Thread callThread = new Thread(() -> {
                final long until = System.nanoTime() + 30000000000L;
                // Checking the clock on every increment can spend nearly all
                // translated execution in clock_gettime on API31. Give this
                // disposable fixture a real, bounded batch of user-space work;
                // never relax the native orig_rax/user-context admission rule.
                while (System.nanoTime() < until)
                    for (int spin=0;spin<65536;spin++) callTicks++;
            }, "SeedLensProbe");
            callThread.start();
            // Keep our own process alive for a bounded namespace probe. The ARM64
            // canary is deliberately NOT loaded here; it must be loaded by the
            // candidate host backend to establish independent evidence.
            long deadline = SystemClock.elapsedRealtime() + 90000;
            int ticks = 0;
            while (SystemClock.elapsedRealtime() < deadline) {
                Thread.sleep(1000);
                if (++ticks % 10 == 0) append("ALIVE\t" + ticks);
            }
            callThread.join(1000);
            if (callThread.isAlive()) throw new IllegalStateException("call thread not finished");
            append("CALL_THREAD_FINISHED\t" + callTicks);
            append("FINISH\tPASS");
            result.putString("probe", "baseline-pass-not-namespace-validation");
            finish(-1, result);
        } catch (Throwable error) {
            // Only type, never arbitrary runtime messages or account data.
            try { append("FAIL\t" + error.getClass().getSimpleName()); } catch (Exception ignored) { }
            result.putString("probe", "failed-" + error.getClass().getSimpleName());
            finish(1, result);
        }
    }
}
