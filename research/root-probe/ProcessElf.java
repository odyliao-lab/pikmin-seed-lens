package dev.ody.seedlensrootprobe;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

/** Research-only process architecture evidence, not a general ELF validator. */
final class ProcessElf {
    static final int UNKNOWN=0, X86_64=62, AARCH64=183;
    private ProcessElf() {}
    static int currentMachine() throws IOException {
        // Read this Java process's kernel-selected executable, never a caller path.
        try(InputStream input=new FileInputStream("/proc/self/exe")) {
            return machine(input);
        }
    }
    static int machine(InputStream input) throws IOException {
        byte[] h=new byte[64];int used=0;
        while(used<h.length) {
            int count=input.read(h,used,h.length-used);
            if(count<0)return UNKNOWN;
            if(count==0) {
                int value=input.read();if(value<0)return UNKNOWN;
                h[used++]=(byte)value;
            } else used+=count;
        }
        if(h[0]!=0x7f||h[1]!='E'||h[2]!='L'||h[3]!='F'||
           h[4]!=2||h[5]!=1||h[6]!=1||
           h[20]!=1||h[21]!=0||h[22]!=0||h[23]!=0||u16(h,52)!=64)return UNKNOWN;
        int type=u16(h,16),machine=u16(h,18);
        if(type!=2&&type!=3)return UNKNOWN;
        return machine==X86_64||machine==AARCH64?machine:UNKNOWN;
    }
    private static int u16(byte[] h,int at) {
        return (h[at]&255)|((h[at+1]&255)<<8);
    }
}
