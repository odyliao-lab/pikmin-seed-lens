package android.app;
import android.os.Bundle;
public class ActivityOptions {
 public static final int MODE_BACKGROUND_ACTIVITY_START_ALLOWED=1;private final Bundle value=new Bundle();
 public static ActivityOptions makeBasic(){return new ActivityOptions();}
 public ActivityOptions setPendingIntentCreatorBackgroundActivityStartMode(int mode){value.creatorMode=mode;return this;}
 public Bundle toBundle(){return value;}
}
