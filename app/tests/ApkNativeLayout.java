package dev.ody.seedpreview;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.security.MessageDigest;
import java.util.*;
import java.util.zip.*;

/** Host build check only. No Android execution, signing or device acceptance. */
public final class ApkNativeLayout {
    static final Set<String> REQUIRED=new HashSet<>(Arrays.asList(
        "assets/libseed_display.so", "lib/arm64-v8a/libseed_display.so",
        "lib/arm64-v8a/libseed_loader.so", "lib/x86_64/libseed_loader.so", "lib/x86_64/libseed_bridge.so"));
    static long u32(byte[] b,int i){return (b[i]&255L)|((b[i+1]&255L)<<8)|((b[i+2]&255L)<<16)|((b[i+3]&255L)<<24);}
    static int u16(byte[] b,int i){return (b[i]&255)|((b[i+1]&255)<<8);}
    static void require(boolean ok,String reason)throws IOException{if(!ok)throw new IOException(reason);}
    static byte[] read(RandomAccessFile f,long at,int length)throws IOException{
        require(at>=0&&length>=0&&at<=f.length()-length,"ZIP range");
        byte[] bytes=new byte[length];f.seek(at);f.readFully(bytes);return bytes;
    }
    static void layout(Path path,Set<String> required)throws IOException{
        try(RandomAccessFile f=new RandomAccessFile(path.toFile(),"r")){
            long size=f.length();require(size>=22&&size<=512L*1024*1024,"APK size");
            int count=(int)Math.min(size,65557);byte[] tail=read(f,size-count,count);int eoc=-1;
            for(int i=0;i+22<=count;i++)if(u32(tail,i)==0x06054b50L&&i+22+u16(tail,i+20)==count){
                require(eoc==-1,"Ambiguous ZIP end");eoc=i;
            }
            require(eoc>=0,"Missing ZIP end");
            int entries=u16(tail,eoc+10);long cdSize=u32(tail,eoc+12),cdAt=u32(tail,eoc+16);
            require(u16(tail,eoc+4)==0&&u16(tail,eoc+6)==0&&entries>0&&entries<=4096
                &&u16(tail,eoc+8)==entries&&cdSize<=4*1024*1024&&cdAt+cdSize==size-count+eoc,"Unsupported ZIP directory");
            byte[] cd=read(f,cdAt,(int)cdSize);int pos=0;Set<String> seen=new HashSet<>(),found=new HashSet<>();
            for(int index=0;index<entries;index++){
                require(pos<=cd.length-46&&u32(cd,pos)==0x02014b50L,"ZIP central header");
                int n=u16(cd,pos+28),extra=u16(cd,pos+30),comment=u16(cd,pos+32),total=46+n+extra+comment;
                require(n>0&&total<=cd.length-pos&&u16(cd,pos+34)==0,"ZIP central length/disk");
                String name=new String(cd,pos+46,n,StandardCharsets.UTF_8);
                require(seen.add(name),"Duplicate APK entry");
                if(name.endsWith(".so")){
                    int flags=u16(cd,pos+8);long length=u32(cd,pos+24),local=u32(cd,pos+42);
                    require((flags&~0x800)==0&&u16(cd,pos+10)==ZipEntry.STORED&&length>0&&length<=32*1024*1024
                        &&u32(cd,pos+20)==length&&local<cdAt,"Compressed/unsupported native entry: "+name);
                    byte[] h=read(f,local,30);int localExtra=u16(h,28);
                    require(u32(h,0)==0x04034b50L&&u16(h,6)==flags&&u16(h,8)==0&&u32(h,14)==u32(cd,pos+16)
                        &&u32(h,18)==length&&u32(h,22)==length&&u16(h,26)==n,"Native local header mismatch");
                    require(Arrays.equals(read(f,local+30,n),Arrays.copyOfRange(cd,pos+46,pos+46+n)),"Native local name mismatch");
                    long data=local+30+n+localExtra;
                    require(data%16384==0&&data+length<=cdAt,"Native entry not 16KB aligned: "+name);
                    found.add(name);
                }
                pos+=total;
            }
            require(pos==cd.length&&found.containsAll(required),"Missing required native entry");
        }
    }
    static String digest(ZipFile zip,String name)throws Exception{
        ZipEntry entry=zip.getEntry(name);require(entry!=null,"Missing digest source");
        MessageDigest sha=MessageDigest.getInstance("SHA-256");
        try(InputStream in=zip.getInputStream(entry)){byte[] bytes=new byte[65536];int n;while((n=in.read(bytes))!=-1)sha.update(bytes,0,n);}
        StringBuilder result=new StringBuilder();for(byte b:sha.digest())result.append(String.format("%02x",b&255));return result.toString();
    }
    public static void main(String[] args)throws Exception{
        require(args.length==1,"Pass exactly one APK");Path apk=Paths.get(args[0]);layout(apk,REQUIRED);
        try(ZipFile zip=new ZipFile(apk.toFile())){
            require(digest(zip,"assets/libseed_display.so").equals(digest(zip,"lib/arm64-v8a/libseed_display.so")),"Renderer copies differ");
            checkDigest(zip,"assets/seed-bridge-renderer.sha256","assets/libseed_display.so");
            checkDigest(zip,"assets/seed-bridge-x86_64.sha256","lib/x86_64/libseed_bridge.so");
            for(String abi:Arrays.asList("arm64-v8a","x86_64")){
                ZipEntry entry=zip.getEntry("assets/seed-loader-"+abi+".sha256");
                require(entry!=null&&entry.getSize()==65,"Loader digest asset size");
                String expected;try(InputStream in=zip.getInputStream(entry)){expected=new String(in.readNBytes(66),StandardCharsets.US_ASCII);}
                require(expected.matches("[0-9a-f]{64}\\n")&&digest(zip,"lib/"+abi+"/libseed_loader.so").equals(expected.trim()),"Loader digest mismatch");
            }
        }
        System.out.println("APK_NATIVE_LAYOUT_AND_DIGEST_PASS (not device acceptance)");
    }
    static void checkDigest(ZipFile zip,String receipt,String payload)throws Exception{
        ZipEntry entry=zip.getEntry(receipt);require(entry!=null&&entry.getSize()==65,"Missing/invalid paired digest");
        String expected;try(InputStream in=zip.getInputStream(entry)){expected=new String(in.readNBytes(66),StandardCharsets.US_ASCII);}
        require(expected.matches("[0-9a-f]{64}\\n")&&digest(zip,payload).equals(expected.trim()),"Bridge/renderer build pairing mismatch");
    }
}
