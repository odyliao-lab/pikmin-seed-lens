package dev.ody.seedpreview;
import java.io.*;
import java.nio.channels.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
/** App-private cross-process exclusion: APK preparation cannot overlap a live load. */
final class UpdateInterlock {
 static final class Lease implements AutoCloseable {
  final RandomAccessFile file;final FileLock lock;
  Lease(RandomAccessFile file,FileLock lock){this.file=file;this.lock=lock;}
  public void close(){try{lock.release();}catch(Exception ignored){}try{file.close();}catch(Exception ignored){}}
 }
 static Lease lock(File directory){RandomAccessFile f=null;try{f=new RandomAccessFile(new File(directory,"update-load.lock"),"rw");FileLock lock=f.getChannel().tryLock();if(lock==null){f.close();return null;}return new Lease(f,lock);}catch(Exception e){if(f!=null)try{f.close();}catch(Exception ignored){}return null;}}
 static Lease beginLoad(File directory,long installed){Lease lease=lock(directory);if(lease==null)return null;try{
   File marker=new File(directory,"update-freeze");if(marker.exists()){
    if(marker.length()>24)throw new IOException();String raw=new String(Files.readAllBytes(marker.toPath()),StandardCharsets.US_ASCII);
    if(!raw.matches("[0-9]{1,10}\\n")||Long.parseLong(raw.trim())>installed)throw new IOException();
    if(!marker.delete())throw new IOException();
   }return lease;
  }catch(Exception e){lease.close();return null;}}
 static boolean freeze(File directory,long target){try(Lease lease=lock(directory)){if(lease==null||target<1)return false;
   File marker=new File(directory,"update-freeze"),temporary=new File(directory,"update-freeze.new");
   try(FileOutputStream out=new FileOutputStream(temporary)){out.write((target+"\n").getBytes(StandardCharsets.US_ASCII));out.getFD().sync();}
   Files.move(temporary.toPath(),marker.toPath(),StandardCopyOption.REPLACE_EXISTING,StandardCopyOption.ATOMIC_MOVE);return true;
  }catch(Exception e){return false;}}
 static boolean clear(File directory){try(Lease lease=lock(directory)){if(lease==null)return false;File marker=new File(directory,"update-freeze");return !marker.exists()||marker.delete();}catch(Exception e){return false;}}
}
