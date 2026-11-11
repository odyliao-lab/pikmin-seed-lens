package dev.ody.seedpreview;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageInstaller;
import android.os.Bundle;
public final class LensUpdateInstallActivity extends Activity {
 @Override public void onCreate(Bundle b){super.onCreate(b);handle(getIntent());}
 @Override public void onNewIntent(Intent i){super.onNewIntent(i);setIntent(i);handle(i);}
 void handle(Intent i){
  int expected=i.getIntExtra("expected_session",-1),received=i.getIntExtra(PackageInstaller.EXTRA_SESSION_ID,-2);
  if(expected<0||expected!=received||expected!=LensUpdateInstaller.session(this)){finish();return;}
  int status=i.getIntExtra(PackageInstaller.EXTRA_STATUS,PackageInstaller.STATUS_FAILURE);
  if(status==PackageInstaller.STATUS_PENDING_USER_ACTION){Intent confirm=i.getParcelableExtra(Intent.EXTRA_INTENT);if(confirm!=null)try{startActivity(confirm);}catch(Exception ignored){}finish();return;}
  try{LensUpdateInstaller.clear(this,status==PackageInstaller.STATUS_SUCCESS?"更新安裝完成；請正常重開遊戲。":"安裝已取消或失敗，原版本與設定保留。");}catch(Exception ignored){}
  startActivity(new Intent(this,DiagnosticsActivity.class).addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP));finish();
 }
}
