package dev.ody.seedpreview;

import android.app.*;
import android.content.*;
import android.content.res.Configuration;
import android.graphics.*;
import android.os.*;
import android.view.*;
import android.widget.*;

/** Compact tool list; controls stay direct, help stays inline, actions stay visible. */
public final class MainActivity extends Activity implements LensController.Observer {
    LensController controller;ForestStyle style;TextView message,status,closeAll;
    final Handler updates=new Handler(Looper.getMainLooper());
    final Runnable poll=new Runnable(){public void run(){changed();controller.tick();controller.refreshReports();updates.postDelayed(this,3000);}};
    final Switch[] switches=new Switch[4];
    final TextView[] reports=new TextView[4];
    Switch gpsSwitch;TextView gpsReport;
    boolean suppress;
    @Override public void onCreate(Bundle saved){super.onCreate(saved);controller=LensController.get(this);build();if(saved==null)offerFirstCheck();}
    @Override protected void onResume(){super.onResume();controller.attach(this);try{startService(new Intent(this,LensUpdateCheckService.class));}catch(RuntimeException ignored){}controller.refreshGroups();updates.postDelayed(poll,3000);}
    @Override protected void onPause(){updates.removeCallbacks(poll);controller.detach(this);super.onPause();}
    void build(){
        boolean dark=(getResources().getConfiguration().uiMode&Configuration.UI_MODE_NIGHT_MASK)==Configuration.UI_MODE_NIGHT_YES;
        style=new ForestStyle(this,dark);LinearLayout root=style.column();root.setBackgroundColor(style.paper);
        if(Build.VERSION.SDK_INT>=30)getWindow().setDecorFitsSystemWindows(false);
        getWindow().setStatusBarColor(Color.TRANSPARENT);getWindow().setNavigationBarColor(style.paper);
        getWindow().getDecorView().setSystemUiVisibility(dark?0:View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR|View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR);
        root.setOnApplyWindowInsetsListener((v,insets)->{
            if(Build.VERSION.SDK_INT>=30){Insets bars=insets.getInsets(WindowInsets.Type.systemBars());v.setPadding(bars.left,bars.top,bars.right,bars.bottom);}
            else v.setPadding(insets.getSystemWindowInsetLeft(),insets.getSystemWindowInsetTop(),insets.getSystemWindowInsetRight(),insets.getSystemWindowInsetBottom());return insets;
        });
        LinearLayout header=style.column();header.setPadding(style.dp(16),style.dp(8),style.dp(16),style.dp(8));root.addView(header,new LinearLayout.LayoutParams(-1,-2));
        LensTabs.add(this,style,header,false,true);
        LinearLayout brand=new LinearLayout(this);brand.setGravity(Gravity.CENTER_VERTICAL);
        ImageView image=new ImageView(this);image.setScaleType(ImageView.ScaleType.FIT_CENTER);image.setImportantForAccessibility(View.IMPORTANT_FOR_ACCESSIBILITY_NO);
        try{image.setImageBitmap(OfflineArtwork.mascot(this));brand.addView(image,new LinearLayout.LayoutParams(style.dp(48),style.dp(48)));}catch(Exception ignored){}
        LinearLayout identity=style.column();identity.setPadding(style.dp(10),0,0,0);brand.addView(identity,new LinearLayout.LayoutParams(0,-2,1));
        style.add(identity,style.text("皮克敏花苗透視鏡",18,style.ink,true),0);
        status=style.text("正在核對…",12,style.green,false);style.add(identity,status,0);style.add(header,brand,0);
        ScrollView scroll=new ScrollView(this);scroll.setClipToPadding(false);root.addView(scroll,new LinearLayout.LayoutParams(-1,0,1));
        LinearLayout page=style.column();page.setPadding(style.dp(16),0,style.dp(16),style.dp(8));scroll.addView(page);
        LinearLayout list=style.column();list.setBackground(style.panel(style.surface,16,true));style.add(page,list,8);
        for(int i=0;i<5;i++){
            final int selected=i;boolean gps=i==4;String title=gps?"複製探險座標":DisplayGroups.TITLES[i];
            String detailText=gps?"開啟探險花苗／水果詳情，左右切換停穩後複製目前項目座標。不移動定位、不派遣或領取；四組資訊顯示全關也能使用。未知或隱藏時保留原剪貼簿，座標不會上傳。":DisplayGroups.DETAILS[i];
            if(i>0){View divider=new View(this);divider.setBackgroundColor(style.line);LinearLayout.LayoutParams line=new LinearLayout.LayoutParams(-1,style.dp(1));line.setMargins(style.dp(12),0,style.dp(12),0);list.addView(divider,line);}
            LinearLayout section=style.column(),row=new LinearLayout(this);row.setGravity(Gravity.CENTER_VERTICAL);row.setPadding(style.dp(12),style.dp(6),style.dp(8),style.dp(6));
            LinearLayout label=style.column();row.addView(label,new LinearLayout.LayoutParams(0,-2,1));style.add(label,style.text(title,16,style.ink,true),0);
            TextView report=style.text("尚未核對",12,style.muted,false);style.add(label,report,0);
            Button help=new Button(this);help.setText("ⓘ");help.setAllCaps(false);help.setTextSize(20);help.setTextColor(style.green);help.setPadding(0,0,0,0);help.setMinWidth(0);help.setMinimumWidth(0);help.setMinHeight(style.dp(48));
            help.setContentDescription(title+"說明");help.setBackground(new android.graphics.drawable.RippleDrawable(android.content.res.ColorStateList.valueOf(style.dark?0x33FFFFFF:0x182B6045),style.panel(style.surface,12,false),null));row.addView(help,new LinearLayout.LayoutParams(style.dp(48),-2));
            Switch toggle=new Switch(this);toggle.setShowText(false);toggle.setMinHeight(style.dp(48));toggle.setContentDescription(title+"開關");
            toggle.setThumbTintList(new android.content.res.ColorStateList(new int[][]{new int[]{android.R.attr.state_checked},new int[]{}},new int[]{style.surface,style.muted}));
            toggle.setTrackTintList(new android.content.res.ColorStateList(new int[][]{new int[]{android.R.attr.state_checked},new int[]{}},new int[]{style.green,style.muted}));row.addView(toggle,new LinearLayout.LayoutParams(style.dp(64),-2));style.add(section,row,0);
            TextView detail=style.text(detailText,13,style.muted,false);detail.setPadding(style.dp(12),0,style.dp(12),style.dp(12));detail.setVisibility(View.GONE);style.add(section,detail,0);style.add(list,section,0);
            help.setOnClickListener(v->{boolean visible=detail.getVisibility()==View.VISIBLE;detail.setVisibility(visible?View.GONE:View.VISIBLE);help.setText(visible?"ⓘ":"×");help.setContentDescription((visible?"展開":"收合")+title+"說明");});
            if(gps){gpsSwitch=toggle;gpsReport=report;}else{switches[i]=toggle;reports[i]=report;}
            toggle.setOnCheckedChangeListener((button,on)->{if(suppress)return;if(selected==4)controller.setGps(on);else controller.setGroup(selected,on);if(on&&!controller.armed()&&Boolean.TRUE.equals(selected==4?controller.gpsOn:controller.groupOn[selected]))consent(()->{});});
        }
        message=style.text("",12,style.muted,false);message.setPadding(style.dp(4),0,style.dp(4),0);message.setOnClickListener(v->{if(controller.anyGroup()&&!controller.armed())consent(()->{});else controller.refreshGroups();});style.add(page,message,0);
        LinearLayout footer=style.column();footer.setPadding(style.dp(16),style.dp(8),style.dp(16),style.dp(6));root.addView(footer,new LinearLayout.LayoutParams(-1,-2));
        LinearLayout actions=new LinearLayout(this);
        Button off=action("關閉全部",false);closeAll=off;off.setOnClickListener(v->controller.allOff());LinearLayout.LayoutParams offSize=new LinearLayout.LayoutParams(0,-2,1);offSize.rightMargin=style.dp(8);actions.addView(off,offSize);
        Button launch=action("開啟遊戲 ↗",true);actions.addView(launch,new LinearLayout.LayoutParams(0,-2,1));
        launch.setOnClickListener(v->{Runnable open=()->{if(controller.anyGroup()&&controller.armed())LensService.start(this);Intent game=getPackageManager().getLaunchIntentForPackage("com.nianticlabs.pikmin");if(game!=null)startActivity(game);};if(controller.anyGroup()&&!controller.armed())consent(open);else open.run();});style.add(footer,actions,4);
        String version="未知";try{version=getPackageManager().getPackageInfo(getPackageName(),0).versionName;}catch(Exception ignored){}
        TextView versionLabel=style.text("v"+version+"  ·  開發者 odyliao-lab",11,style.muted,false);versionLabel.setGravity(Gravity.CENTER);style.add(footer,versionLabel,0);
        setContentView(root);root.requestApplyInsets();changed();
    }
    void offerFirstCheck(){
        android.content.SharedPreferences p=getSharedPreferences("lens-first-use",0);
        if(p.contains("offered"))return;
        if(!p.edit().putBoolean("offered",true).commit())return;
        if(controller.hasIntent()||controller.armed())return; // Existing users keep their normal entry.
        new AlertDialog.Builder(this).setTitle("第一次使用透視鏡")
            .setMessage("建議先檢查 Root 與遊戲版本，再選擇功能並開啟遊戲。檢查不會安裝套件或修改系統；也可稍後從「診斷」頁進入。")
            .setNegativeButton("稍後再說",null).setPositiveButton("檢查環境",(d,w)->startActivity(new Intent(this,DiagnosticsActivity.class))).show();
    }
    Button action(String text,boolean primary){Button button=style.button(text,primary);button.setMinHeight(style.dp(48));button.setMinimumHeight(style.dp(48));button.setPadding(style.dp(12),style.dp(6),style.dp(12),style.dp(6));return button;}
    String rowStatus(Boolean on,String report,String unavailable){
        if(on==null)return report;
        if(!controller.settingsError.isEmpty()||controller.pendingOff()||controller.reportReadFailed)return on?"已開啟，等待同步":"已關閉，等待同步";
        if(on&&unavailable!=null)return "已開啟，暫不可用";
        return report;
    }
    void consent(Runnable after){
        new AlertDialog.Builder(this).setTitle("允許自動套用")
            .setMessage("開關選擇已保存。條件符合後會自動套用資訊或複製探險座標，不移動定位。載入結果不明或逾時時，允許停止當次遊戲並保留紀錄，不自動重送；下次正常開啟的新程序會重新核對。")
            .setNegativeButton("取消",(d,w)->changed()).setPositiveButton("允許",(d,w)->{
                if(controller.arm())after.run();else{controller.reason="啟用意願未保存，請重試。";changed();}
            }).show();
    }
    @Override public void changed(){
        if(message==null)return;String unavailable=controller.operations.extensionAvailability();
        status.setText(controller.overview());closeAll.setEnabled(true);closeAll.setAlpha(closeAll.isEnabled()?1f:0.45f);
        suppress=true;
        for(int i=0;i<4;i++){
            Boolean value=controller.groupOn[i];switches[i].setChecked(Boolean.TRUE.equals(value));
            // Preferences remain editable while Root, game or an older operation waits.
            switches[i].setEnabled(true);
            switches[i].setAlpha(switches[i].isEnabled()?1f:0.45f);
            reports[i].setText(rowStatus(value,controller.groupReport[i],unavailable));
        }
        gpsSwitch.setChecked(Boolean.TRUE.equals(controller.gpsOn));
        gpsSwitch.setEnabled(true);
        gpsSwitch.setAlpha(gpsSwitch.isEnabled()?1f:0.45f);gpsReport.setText(rowStatus(controller.gpsOn,controller.gpsText,controller.operations.gpsAvailability()));
        suppress=false;
        String gpsUnavailable=Boolean.TRUE.equals(controller.gpsOn)?controller.operations.gpsAvailability():null;
        String text=!controller.reason.isEmpty()?controller.reason:!controller.settingsError.isEmpty()?controller.settingsError:unavailable!=null?unavailable:gpsUnavailable!=null?gpsUnavailable:controller.reportReadFailed?"無法核對遊戲回報，請檢查 Root 後點此重試。":controller.anyGroup()&&!controller.armed()?"選擇已保存；點此完成首次套用確認。":"";
        if(!controller.gameAbsent()&&(controller.state.phase==LensStatus.Phase.UNKNOWN||controller.state.phase==LensStatus.Phase.UNCERTAIN))
            text="結果不明，請正常重開遊戲；保留紀錄，不重送載入。";
        else if(!controller.gameAbsent()&&controller.state.phase==LensStatus.Phase.RESTART_REQUIRED)
            text="遊戲仍有舊版顯示或載入未完成，請正常重開遊戲後重新綁定。";
        message.setVisibility(text.isEmpty()?View.GONE:View.VISIBLE);message.setText(text);message.setTextColor(controller.state.blocksEnable()&&!controller.gameAbsent()?style.error:style.muted);
    }
}
