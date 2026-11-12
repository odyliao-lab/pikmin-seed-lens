package android.app;
import android.content.Context;import java.util.*;
public class Notification {
 public static final int VISIBILITY_SECRET=-1;
 public String text;public final List<Action> actions=new ArrayList<>();
 public static class BigTextStyle{public BigTextStyle bigText(String s){return this;}}
 public static class Builder {
  final Notification n=new Notification();public Builder(Context c,String ch){}
  public Builder setSmallIcon(int i){return this;}public Builder setContentTitle(String s){return this;}
  public Builder setContentText(String s){n.text=s;return this;}public Builder setStyle(BigTextStyle s){return this;}
  public Builder setContentIntent(PendingIntent i){return this;}public Builder setOnlyAlertOnce(boolean b){return this;}
  public Builder setOngoing(boolean b){return this;}public Builder setVisibility(int i){return this;}
  public Builder addAction(Action a){n.actions.add(a);return this;}public Notification build(){return n;}
 }
 public static class Action {
  public String title;public PendingIntent intent;
  public static class Builder{final Action a=new Action();public Builder(Object icon,String title,PendingIntent intent){a.title=title;a.intent=intent;}public Action build(){return a;}}
 }
}
