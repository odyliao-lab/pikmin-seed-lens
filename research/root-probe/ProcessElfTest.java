package dev.ody.seedlensrootprobe;

import java.io.ByteArrayInputStream;
import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

public final class ProcessElfTest {
    private static int checks;
    private static byte[] header(int machine,int type){
        byte[] h=new byte[64];h[0]=0x7f;h[1]='E';h[2]='L';h[3]='F';h[4]=2;h[5]=1;h[6]=1;
        h[16]=(byte)type;h[18]=(byte)machine;h[19]=(byte)(machine>>8);h[20]=1;h[52]=64;return h;
    }
    private static void check(int expected,byte[] bytes)throws IOException{
        if(ProcessElf.machine(new ByteArrayInputStream(bytes))!=expected)throw new AssertionError("ELF process identity mismatch");
        ++checks;
    }
    public static void main(String[] args)throws Exception{
        for(int machine:new int[]{ProcessElf.X86_64,ProcessElf.AARCH64})for(int type:new int[]{2,3})check(machine,header(machine,type));
        byte[] valid=header(ProcessElf.X86_64,3);
        for(int length=0;length<64;++length)check(ProcessElf.UNKNOWN,Arrays.copyOf(valid,length));
        for(int offset:new int[]{0,1,2,3,4,5,6,16,17,18,19,20,21,22,23,52,53}){
            byte[] bad=valid.clone();bad[offset]=(byte)(valid[offset]==0?1:0);check(ProcessElf.UNKNOWN,bad);
        }
        for(int machine:new int[]{0,3,40,243,65535})check(ProcessElf.UNKNOWN,header(machine,3));
        InputStream shortReads=new FilterInputStream(new ByteArrayInputStream(valid)){
            @Override public int read(byte[] b,int offset,int length)throws IOException{return super.read(b,offset,Math.min(length,1));}
        };
        if(ProcessElf.machine(shortReads)!=ProcessElf.X86_64)throw new AssertionError("Short reads lost process identity");++checks;
        InputStream zeroOnce=new FilterInputStream(new ByteArrayInputStream(header(ProcessElf.AARCH64,3))){
            boolean first=true;
            @Override public int read(byte[] b,int offset,int length)throws IOException{if(first){first=false;return 0;}return super.read(b,offset,length);}
        };
        if(ProcessElf.machine(zeroOnce)!=ProcessElf.AARCH64)throw new AssertionError("Zero-length read fallback failed");++checks;
        ByteArrayInputStream bounded=new ByteArrayInputStream(Arrays.copyOf(valid,512));
        if(ProcessElf.machine(bounded)!=ProcessElf.X86_64||bounded.available()!=448)throw new AssertionError("Read exceeded fixed ELF header");++checks;
        try{
            ProcessElf.machine(new InputStream(){@Override public int read()throws IOException{throw new IOException("fixture");}});
            throw new AssertionError("I/O failure must not become architecture evidence");
        }catch(IOException expected){++checks;}
        System.out.println("ROOT_PROBE_PROCESS_ELF_PASS "+checks+" checks; HOST ONLY");
    }
}
