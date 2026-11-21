package dev.ody.seedpreview;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageInstaller;
import android.os.Bundle;
public final class LensUpdateInstallActivity extends Activity {
 @Override public void onCreate(Bundle b){super.onCreate(b);handle(getIntent());}
 @Override public void onNewIntent(Intent i){super.onNewIntent(i);setIntent(i);handle(i);}
 void handle(Intent i){
  if(i==null){finish();return;}
  int expected=i.getIntExtra("expected_session",-1),received=i.getIntExtra(PackageInstaller.EXTRA_SESSION_ID,-2);
  if(expected<0||expected!=received||expected!=LensUpdateInstaller.session(this)){finish();return;}
  int status=i.getIntExtra(PackageInstaller.EXTRA_STATUS,PackageInstaller.STATUS_FAILURE);
  if(status==PackageInstaller.STATUS_PENDING_USER_ACTION){
   if(!LensUpdateInstaller.requestConfirmation(this,expected)){finish();return;}
   Intent confirm=i.getParcelableExtra(Intent.EXTRA_INTENT);
   if(confirm!=null)try{startActivity(confirm);finish();return;}catch(Exception ignored){}
   // A missing/unlaunchable confirmation must not leave an uncancellable freeze.
   // Keep the session pending if abandonment itself fails so the user can retry cancel.
   try{LensUpdateInstaller.cancel(this,expected,"無法開啟 Android 安裝確認。可重試已驗證的 APK。");}catch(Exception ignored){}
   startActivity(new Intent(this,DiagnosticsActivity.class).addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP));finish();return;
  }
  try{if(!LensUpdateInstaller.finish(this,expected,status)){finish();return;}}catch(Exception ignored){}
  startActivity(new Intent(this,DiagnosticsActivity.class).addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP));finish();
 }
}
