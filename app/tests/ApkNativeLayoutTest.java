package dev.ody.seedpreview;
import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.zip.*;
public final class ApkNativeLayoutTest {
    static final String NAME="assets/libseed_display.so";
    static byte[] fixture(boolean aligned,boolean stored)throws Exception{
        ByteArrayOutputStream out=new ByteArrayOutputStream();
        try(ZipOutputStream zip=new ZipOutputStream(out)){
            ZipEntry e=new ZipEntry(NAME);byte[] body={1,2,3};CRC32 crc=new CRC32();crc.update(body);
            if(stored){e.setMethod(ZipEntry.STORED);e.setSize(body.length);e.setCompressedSize(body.length);e.setCrc(crc.getValue());}
            // ZIP local header + ASCII name + one well-formed padding extra.
            if(aligned){int count=16384-30-NAME.length();byte[] extra=new byte[count];extra[0]=(byte)255;extra[1]=(byte)255;extra[2]=(byte)(count-4);extra[3]=(byte)((count-4)>>8);e.setExtra(extra);}
            zip.putNextEntry(e);zip.write(body);zip.closeEntry();
        }return out.toByteArray();
    }
    static void check(byte[] bytes,boolean pass)throws Exception{
        Path file=Files.createTempFile("seed-layout-fixture-",".zip");
        try{Files.write(file,bytes);boolean accepted=true;try{ApkNativeLayout.layout(file,Collections.singleton(NAME));}catch(IOException e){accepted=false;}
            if(accepted!=pass)throw new AssertionError("Unexpected layout decision");
        }finally{Files.delete(file);}
    }
    public static void main(String[] args)throws Exception{
        byte[] valid=fixture(true,true);check(valid,true);
        check(fixture(false,true),false);check(fixture(true,false),false);check(new byte[22],false);
        byte[] bad=valid.clone();bad[8]=8;check(bad,false);
        bad=valid.clone();bad[30]='x';check(bad,false);
        bad=valid.clone();bad[18]=9;check(bad,false);
        bad=Arrays.copyOf(valid,valid.length-1);check(bad,false);
        System.out.println("APK_LAYOUT_TEST_PASS 8");
        String sha="039058c6f2c0cb492c533b0a4d14ef77cc0f78abccced5287d84a1a2011cfb81";
        paired(sha+"\n",true);paired("0".repeat(64)+"\n",false);
        paired(sha+"\r\n",false);paired(null,false);
        System.out.println("APK_PAIRED_DIGEST_TEST_PASS 4");
    }
    static void paired(String receipt,boolean pass)throws Exception{
        Path file=Files.createTempFile("seed-pair-fixture-",".zip");
        try{
            try(ZipOutputStream out=new ZipOutputStream(Files.newOutputStream(file))){
                out.putNextEntry(new ZipEntry(NAME));out.write(new byte[]{1,2,3});out.closeEntry();
                if(receipt!=null){out.putNextEntry(new ZipEntry("paired.sha256"));out.write(receipt.getBytes(java.nio.charset.StandardCharsets.US_ASCII));out.closeEntry();}
            }
            boolean accepted=true;try(ZipFile zip=new ZipFile(file.toFile())){try{ApkNativeLayout.checkDigest(zip,"paired.sha256",NAME);}catch(IOException e){accepted=false;}}
            if(accepted!=pass)throw new AssertionError("Unexpected pairing decision");
        }finally{Files.delete(file);}
    }
}
