package dev.ody.seedpreview;
import android.app.*;
import android.content.*;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.*;
import android.view.*;
import android.widget.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.*;
/** A separate lightweight process also reachable from the launcher long-press shortcut. */
public final class DiagnosticsActivity extends Activity {
 ForestStyle style;LinearLayout results;TextView notice;Button basic,root,preview,retry;
 Map<String,String> facts;boolean busy,namespaceMismatch;String exportJson;
 final ExecutorService worker=Executors.newSingleThreadExecutor();final Handler handler=new Handler(Looper.getMainLooper());
 final Runnable updatePoll=new Runnable(){public void run(){if(updates!=null)try{updates.render();}catch(RuntimeException ignored){}handler.postDelayed(this,1000);}};
 LensUpdateCard updates;
 @Override public void onCreate(Bundle saved){super.onCreate(saved);if(saved!=null)exportJson=saved.getString("export-json");build();scan(false);}
 void build(){
  boolean dark=(getResources().getConfiguration().uiMode&Configuration.UI_MODE_NIGHT_MASK)==Configuration.UI_MODE_NIGHT_YES;style=new ForestStyle(this,dark);
  getWindow().setStatusBarColor(style.paper);getWindow().setNavigationBarColor(style.paper);getWindow().getDecorView().setSystemUiVisibility(dark?0:View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR|View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR);
  ScrollView scroll=new ScrollView(this);scroll.setFillViewport(true);scroll.setBackgroundColor(style.paper);
  if(Build.VERSION.SDK_INT>=30)getWindow().setDecorFitsSystemWindows(false);
  scroll.setOnApplyWindowInsetsListener((v,insets)->{
   if(Build.VERSION.SDK_INT>=30){android.graphics.Insets bars=insets.getInsets(WindowInsets.Type.systemBars());v.setPadding(bars.left,bars.top,bars.right,bars.bottom);}
   else v.setPadding(insets.getSystemWindowInsetLeft(),insets.getSystemWindowInsetTop(),insets.getSystemWindowInsetRight(),insets.getSystemWindowInsetBottom());return insets;
  });LinearLayout page=style.column();page.setPadding(style.dp(20),style.dp(18),style.dp(20),style.dp(20));scroll.addView(page);setContentView(scroll);
  LensTabs.add(this,style,page,true);style.add(page,style.text("自我診斷",24,style.ink,true),12);
  style.add(page,style.text("先檢查基本條件。診斷不啟用遊戲功能、不修改系統。",14,style.muted,false),12);
  notice=style.text("讀取環境中…",14,style.muted,false);notice.setTextIsSelectable(true);style.add(page,notice,12);
  retry=style.button("重試此份回報",false);retry.setVisibility(View.GONE);style.add(page,retry,8);
  basic=style.button("重新檢查環境",false);basic.setOnClickListener(v->scan(false));style.add(page,basic,8);
  root=style.button("檢查 Root",true);root.setOnClickListener(v->new AlertDialog.Builder(this).setTitle("檢查 Root")
   .setMessage("將呼叫既有 su，只讀取 Root、掛載命名空間、轉譯後端與 SELinux。可能出現授權視窗；不安裝套件、不載入遊戲。")
   .setNegativeButton("取消",null).setPositiveButton("開始檢查",(d,w)->scan(true)).show());style.add(page,root,12);
  Button tools=style.button("回工具頁選擇功能／開啟遊戲",false);tools.setOnClickListener(v->{startActivity(new Intent(this,MainActivity.class).addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP|Intent.FLAG_ACTIVITY_SINGLE_TOP));finish();});style.add(page,tools,8);
  results=style.column();style.add(page,results,14);
  preview=style.button("預覽問題回報",false);preview.setOnClickListener(v->chooseIssue());style.add(page,preview,8);
  style.add(page,style.text("只提供版本、平台、Root 結果與透視鏡錯誤分類；不含帳號、位置、截圖、完整日誌或記憶體。未同意不傳送。",12,style.muted,false),14);
  try{updates=new LensUpdateCard(this,style,page);}catch(RuntimeException e){style.add(page,style.text("更新設定暫時無法讀取；其他診斷仍可使用。",13,style.error,false),8);}
  style.add(page,style.text("主畫面打不開時，可長按桌面圖示選「診斷與回報」。",12,style.muted,false),10);
 }
 @Override protected void onResume(){super.onResume();handler.post(updatePoll);if(facts!=null&&!busy)scan(false);}
 @Override protected void onPause(){handler.removeCallbacks(updatePoll);super.onPause();}
 @Override protected void onDestroy(){worker.shutdown();super.onDestroy();}
 @Override protected void onSaveInstanceState(Bundle b){if(exportJson!=null)b.putString("export-json",exportJson);super.onSaveInstanceState(b);}
 void scan(boolean checkRoot){
  if(busy)return;busy=true;buttons();notice.setText(checkRoot?"等待 Root 授權，最多 20 秒…":"正在檢查版本與 App 內建檔案…");
  worker.execute(()->{Map<String,String> found=DiagnosticCollector.basic(this);
   final RootDiagnostic result=checkRoot?DiagnosticCollector.root():RootDiagnostic.failure("not_checked");
   found.put("root",result.root);found.put("bridge",result.bridge);found.put("selinux",result.selinux);
   final Map<String,String> value=found;runOnUiThread(()->{if(isDestroyed())return;facts=value;namespaceMismatch=result.namespaceMismatch;busy=false;buttons();render();});});
 }
 void buttons(){basic.setEnabled(!busy);root.setEnabled(!busy);preview.setEnabled(!busy&&facts!=null);for(Button b:new Button[]{basic,root,preview})b.setAlpha(b.isEnabled()?1f:0.45f);}
 void render(){
  results.removeAllViews();notice.setText("Android "+facts.get("android_release")+"／API"+facts.get("android_api")+" · "+facts.get("abi")+"\n遊戲 "+facts.get("game_version")+"／"+facts.get("game_code")+"\nRoot："+rootLabel(facts.get("root")));
  style.add(results,style.text(FirstUseCheck.summary(facts,namespaceMismatch),14,style.ink,false),10);
  for(String text:DiagnosisPolicy.advice(facts,namespaceMismatch)){LinearLayout card=style.card();style.add(card,style.text(text,14,style.ink,false),0);style.add(results,card,10);}
 }
 String rootLabel(String s){if(namespaceMismatch)return "已取得 UID0，掛載命名空間不符";if("granted".equals(s))return "已取得（不代表遊戲可載入）";if("not_checked".equals(s))return "尚未檢查";if("unavailable".equals(s))return "找不到指令";if("timeout".equals(s))return "逾時，結果未確認";return "未取得／無法確認";}
 void chooseIssue(){
  String[] labels={"App 無法開啟／閃退","Root 授權問題","無法啟用功能","遊戲文字沒有變更","關閉後未還原","其他"};
  new AlertDialog.Builder(this).setTitle("遇到什麼問題？").setItems(labels,(d,which)->{
   try{showReport(new DiagnosticReport(new LinkedHashMap<>(facts),DiagnosticStore.events(this),DiagnosticReport.ISSUES[which]));}
   catch(Exception e){notice.setText("報告未通過資料範圍檢查，請重新檢查環境。");}
  }).show();
 }
 void showReport(DiagnosticReport report){
  showReport(report,"");
 }
 void showReport(DiagnosticReport report,String previousEmail){
  LinearLayout box=style.column();box.setBackgroundColor(style.paper);box.setPadding(style.dp(18),style.dp(8),style.dp(18),0);
  style.add(box,style.text("以下是實際提供的全部資料。本機編號不是送出成功收據。",13,style.muted,false),8);
  TextView body=style.text(report.json,11,style.ink,false);body.setTypeface(Typeface.MONOSPACE);body.setTextIsSelectable(true);ScrollView view=new ScrollView(this);view.addView(body);box.addView(view,new LinearLayout.LayoutParams(-1,style.dp(160)));
  style.add(box,style.text("Email（選填）",13,style.ink,true),8);
  EditText email=new EditText(this);email.setTextColor(style.ink);email.setHintTextColor(style.muted);email.setHint("you@example.com");email.setSingleLine(true);email.setInputType(android.text.InputType.TYPE_CLASS_TEXT|android.text.InputType.TYPE_TEXT_VARIATION_EMAIL_ADDRESS);email.setFilters(new android.text.InputFilter[]{new android.text.InputFilter.LengthFilter(254)});email.setText(previousEmail);style.add(box,email,0);
  style.add(box,style.text("僅用於問題修正後通知；不填也能提報，不會用於行銷。Email 只隨上傳提供，不放入匯出 JSON。",12,style.muted,false),2);
  CheckBox consent=style.check("我已檢視並同意提供診斷；上傳時另附選填 Email");style.add(box,consent,0);
  email.addTextChangedListener(new android.text.TextWatcher(){public void beforeTextChanged(CharSequence s,int start,int count,int after){}public void onTextChanged(CharSequence s,int start,int before,int count){consent.setChecked(false);}public void afterTextChanged(android.text.Editable s){}});
  style.add(box,style.text("接收端：seed-lens-reports.ody-liao.chatgpt.site\n免登入。資料保存 29 天，逾期不供查詢，於服務處理請求時清除。尚未提供自動通知。",12,style.muted,false),4);
  ScrollView container=new ScrollView(this);container.addView(box);
  AlertDialog dialog=new AlertDialog.Builder(this).setTitle("預覽問題回報").setView(container).setNegativeButton("取消",null).setNeutralButton("同意並匯出",null).setPositiveButton("同意並上傳",null).create();
  dialog.setOnShowListener(v->{Button export=dialog.getButton(-3),upload=dialog.getButton(-1);export.setEnabled(false);upload.setEnabled(false);
   consent.setOnCheckedChangeListener((b,checked)->{export.setEnabled(checked);upload.setEnabled(checked&&DiagnosticEndpoint.configured());});
   export.setOnClickListener(b->{if(!consent.isChecked())return;exportJson=report.json;Intent intent=new Intent(Intent.ACTION_CREATE_DOCUMENT).addCategory(Intent.CATEGORY_OPENABLE).setType("application/json").putExtra(Intent.EXTRA_TITLE,"seed-lens-report-"+report.id+".json");
    try{startActivityForResult(intent,81);dialog.dismiss();}catch(ActivityNotFoundException e){notice.setText("找不到系統檔案選擇器，尚未匯出或傳送。");}});
   upload.setOnClickListener(b->{if(!consent.isChecked()||busy||!DiagnosticEndpoint.configured())return;
    final String contact;try{contact=DiagnosticSubmission.email(email.getText().toString());}catch(IllegalArgumentException e){email.setError("Email 格式不正確，請更正或留空");email.requestFocus();return;}
    busy=true;buttons();retry.setVisibility(View.GONE);dialog.dismiss();notice.setText("傳送中，請保留本機編號："+report.id);
    worker.execute(()->{String result;boolean accepted=false;try{result="已收到報告，回報編號："+DiagnosticUploader.send(report,contact,true);accepted=true;}catch(Exception e){result=(e instanceof DiagnosticUploader.Failure?e.getMessage():"尚未取得成功收據，傳送結果未知。")+"\n不會自動重送。請保留本機編號："+report.id;}final String text=result;final boolean ok=accepted;runOnUiThread(()->{if(isDestroyed())return;busy=false;buttons();notice.setText(text);retry.setVisibility(ok?View.GONE:View.VISIBLE);retry.setOnClickListener(v2->{if(!busy)showReport(report,contact);});});});
   });});dialog.show();
 }
 @Override protected void onActivityResult(int request,int result,Intent data){super.onActivityResult(request,result,data);
  if(request!=81)return;final String body=exportJson;exportJson=null;if(result!=RESULT_OK||data==null||data.getData()==null||body==null){notice.setText("已取消匯出，沒有傳送。");return;}
  final Uri uri=data.getData();if(!"content".equals(uri.getScheme())||body.getBytes(StandardCharsets.UTF_8).length>DiagnosticReport.MAX_BYTES){notice.setText("匯出目的地無效，沒有傳送。");return;}
  busy=true;buttons();worker.execute(()->{boolean ok=false;try(OutputStream out=getContentResolver().openOutputStream(uri,"w")){if(out!=null){out.write(body.getBytes(StandardCharsets.UTF_8));ok=true;}}catch(Exception ignored){}final boolean saved=ok;runOnUiThread(()->{if(isDestroyed())return;busy=false;buttons();notice.setText(saved?"已匯出；沒有上傳，請由你選擇私人管道提供。":"匯出未完成；沒有上傳。");});});
 }
}
