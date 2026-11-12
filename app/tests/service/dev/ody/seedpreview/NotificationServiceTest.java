package dev.ody.seedpreview;
import android.app.*;import android.os.Handler;import android.content.Intent;
public final class NotificationServiceTest {
 static int checks;static void check(boolean ok){checks++;if(!ok)throw new AssertionError("notification case "+checks);}
 public static void main(String[] args)throws Exception{
  LensController c=AllOffControllerTest.make(AllOffControllerTest.known());
  var singleton=LensController.class.getDeclaredField("instance");singleton.setAccessible(true);singleton.set(null,c);
  LensService service=new LensService();
  try{
   c.setGps(true);AllOffControllerTest.settle(c);c.tick();AllOffControllerTest.settle(c);
   service.onCreate();service.onStartCommand(null,0,1);
   check(service.manager.last.text.contains("等待遊戲"));
   Notification.Action off=service.manager.last.actions.get(1);
   check(off.title.equals("關閉全部")&&off.intent.service&&off.intent.intent.target==LensService.class);
   check((off.intent.flags&PendingIntent.FLAG_IMMUTABLE)!=0);
   int posts=service.manager.posts;service.publish(c.notificationText());check(posts==service.manager.posts);
   // No Activity: a real async report must be read and published after tick's
   // monitor starts. The former tick-before-read order starved this forever.
   c.setGroup(0,true);AllOffControllerTest.settle(c);
   c.operations.game=LensContext.parse("123\t456\t"+DisplayGroupsTest.BOOT+"\n");
   c.operations.reportOutput=DisplayGroupsTest.rows(System.currentTimeMillis());
   int reads=c.operations.reportCalls;service.poll.run();AllOffControllerTest.settle(c);
   check(c.operations.reportCalls==reads+1);
   check(service.manager.last.text.equals("已套用至已回報畫面"));
   c.operations.reportOutput=DisplayGroupsTest.rows(System.currentTimeMillis()-10000);
   service.poll.run();AllOffControllerTest.settle(c);
   check(service.manager.last.text.equals("等待遊戲畫面回報"));
   c.operations.game=null;service.poll.run();AllOffControllerTest.settle(c);
   check(service.manager.last.text.contains("等待遊戲"));
   c.reason="private coordinate or raw exception";service.publish(c.notificationText());check(!service.manager.last.text.contains("private"));c.reason="";
   c.operations.fail=true;service.onStartCommand(off.intent.intent,0,2);
   check(c.pendingOff()&&!c.anyGroup()&&!c.armed());
   check(service.manager.last.text.contains("尚未確認還原"));
   AllOffControllerTest.settle(c);check(c.pendingOff()&&c.operations.offCalls==1&&c.operations.enableCalls==0);
   // Notification permission failure must not undo a saved OFF or throw out of the service.
   service.manager.denied=true;service.publish("another status");check(c.pendingOff());service.manager.denied=false;
   c.operations.fail=false;c.refreshGroups();AllOffControllerTest.settle(c);
   check(!c.pendingOff()&&!c.anyGroup()&&c.operations.enableCalls==0);
   service.poll.run();check(service.stopped>0);
   service.onDestroy();check(service.removed==1&&!Handler.delayed.contains(service.poll));
   System.out.println("NotificationServiceTest: "+checks+" actual service/controller host checks; not Android notification acceptance");
  }finally{Handler.tasks.clear();Handler.delayed.clear();c.worker.shutdownNow();singleton.set(null,null);var dir=c.operations.context.getFilesDir();for(var f:dir.listFiles())f.delete();dir.delete();}
 }
}
