package android.content.pm;
import android.app.PendingIntent;import java.io.*;import java.util.*;
public class PackageInstaller {
 public static final String EXTRA_SESSION_ID="android.content.pm.extra.SESSION_ID",EXTRA_STATUS="android.content.pm.extra.STATUS";
 public static final int STATUS_PENDING_USER_ACTION=-1,STATUS_SUCCESS=0,STATUS_FAILURE=1,
 STATUS_FAILURE_BLOCKED=2,STATUS_FAILURE_ABORTED=3,STATUS_FAILURE_INVALID=4,
 STATUS_FAILURE_CONFLICT=5,STATUS_FAILURE_STORAGE=6,STATUS_FAILURE_INCOMPATIBLE=7;
 public int next=700,created,abandoned;public boolean abandonFails,commitFails;public PendingIntent committed;public SessionParams params;
 public final Set<Integer> sessions=new HashSet<>();
 public int createSession(SessionParams p){params=p;created++;sessions.add(++next);return next;}
 public Session openSession(int id){return new Session(this);}
 public Object getSessionInfo(int id){return sessions.contains(id)?new Object():null;}
 public void abandonSession(int id){if(abandonFails)throw new IllegalStateException("abandon unavailable");sessions.remove(id);abandoned++;}
 public static class SessionParams {
  public static final int MODE_FULL_INSTALL=1,USER_ACTION_REQUIRED=1;public String pkg;public long size;public int userAction;
  public SessionParams(int mode){}public void setAppPackageName(String p){pkg=p;}public void setSize(long s){size=s;}public void setRequireUserAction(int a){userAction=a;}
 }
 public static class Session implements AutoCloseable {
  final PackageInstaller owner;public Session(PackageInstaller p){owner=p;}
  public OutputStream openWrite(String n,long offset,long size){return new ByteArrayOutputStream();}
  public void fsync(OutputStream out){}public void commit(PendingIntent callback){owner.committed=callback;if(owner.commitFails)throw new IllegalStateException("commit outcome unknown");}public void close(){}
 }
}
