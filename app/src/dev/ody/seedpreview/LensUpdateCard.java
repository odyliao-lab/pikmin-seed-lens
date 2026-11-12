package dev.ody.seedpreview;
import android.app.*;
import android.content.*;
import android.net.Uri;
import android.provider.Settings;
import android.widget.*;
final class LensUpdateCard {
 final Activity activity;final TextView status,summary;final Button check,download,install,cancel;final CheckBox automatic;
 LensUpdateCard(Activity a,ForestStyle style,LinearLayout page){
  activity=a;LinearLayout card=style.card();style.add(card,style.text("版本更新",20,style.ink,true),8);
  status=style.text("",14,style.muted,false);style.add(card,status,8);
  automatic=style.check("開啟 App 時自動檢查更新");automatic.setChecked(LensUpdater.automatic(a));automatic.setOnCheckedChangeListener((b,on)->{LensUpdater.prefs(a).edit().putBoolean("automatic",on).apply();if(on)LensUpdater.check(a,false,null);});style.add(card,automatic,0);
  style.add(card,style.text("成功後每日檢查；失敗依 5 分鐘／30 分鐘／24 小時退避。不自動下載或傳送診斷。",12,style.muted,false),8);
  check=style.button("檢查新版本",false);check.setOnClickListener(v->{LensUpdater.check(a,true,null);render();});style.add(card,check,8);
  summary=style.text("",13,style.muted,false);style.add(card,summary,8);
  download=style.button("下載新版",true);download.setOnClickListener(v->new AlertDialog.Builder(a).setTitle("下載新版透視鏡？").setMessage("只下載官方透視鏡 APK，核對 SHA256、套件、版本、原簽章及內建載入檔；不使用 Root 安裝。下載後仍需你確認。").setNegativeButton("取消",null).setPositiveButton("下載",(d,w)->{LensUpdater.download(a);render();}).show());style.add(card,download,8);
  install=style.button("確認安裝",true);install.setOnClickListener(v->{
   if(!a.getPackageManager().canRequestPackageInstalls()){a.startActivity(new Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES,Uri.parse("package:"+a.getPackageName())));return;}
   new AlertDialog.Builder(a).setTitle("安裝新版透視鏡？").setMessage("保留原簽章與設定。請先結束目前操作；準備安裝時會阻擋新的載入。接著由 Android 要求確認，更新後請正常重開遊戲。").setNegativeButton("取消",null).setPositiveButton("交給 Android 安裝",(d,w)->LensUpdateInstaller.begin(a)).show();});style.add(card,install,8);
  cancel=style.button("取消下載／安裝",false);cancel.setOnClickListener(v->{if(LensUpdater.downloading){LensUpdater.cancel=true;return;}new AlertDialog.Builder(a).setTitle("取消本次更新？").setMessage("原版本與設定保留。").setNegativeButton("返回",null).setPositiveButton("取消更新",(d,w)->{try{LensUpdateInstaller.cancel(a);}catch(Exception e){status.setText("取消尚未確認，請稍後再試。");}}).show();});style.add(card,cancel,0);style.add(page,card,12);render();
 }
 void render(){
  status.setText("目前 "+LensUpdater.installedVersion(activity)+"（"+LensUpdater.installed(activity)+"）"+"\n"+LensUpdater.status(activity));boolean busy=LensUpdater.busy(),pending=LensUpdateInstaller.pending(activity);LensUpdateInfo info=LensUpdater.latest(activity);
  check.setEnabled(!busy&&!pending);download.setEnabled(!busy&&!pending&&info!=null&&info.newerThan(LensUpdater.installed(activity))&&info.minSdk<=android.os.Build.VERSION.SDK_INT);install.setEnabled(!busy&&!pending&&LensUpdater.ready(activity));cancel.setEnabled(LensUpdater.downloading||pending);
  for(Button b:new Button[]{check,download,install,cancel})b.setAlpha(b.isEnabled()?1f:0.45f);
  summary.setText("上次成功檢查："+LensUpdater.checkedAt(activity)+"\n"+(info==null?"尚無快取；更新來源僅限透視鏡官方索引。":"上次成功取得的版本資料（快取）："+info.version+"\n"+info.summary+"\n"+info.compatibility));
 }
}
