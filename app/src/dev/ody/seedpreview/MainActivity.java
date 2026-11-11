package dev.ody.seedpreview;

import android.app.*;
import android.content.*;
import android.content.res.Configuration;
import android.graphics.*;
import android.os.*;
import android.view.*;
import android.widget.*;

/** Four direct group switches on one page. Descriptions start collapsed. */
public final class MainActivity extends Activity implements LensController.Observer {
    LensController controller;ForestStyle style;TextView message;
    final Switch[] switches=new Switch[4];
    final TextView[] reports=new TextView[4];
    boolean suppress;
    @Override public void onCreate(Bundle saved){super.onCreate(saved);controller=LensController.get(this);build();}
    @Override protected void onResume(){super.onResume();controller.attach(this);controller.refreshGroups();}
    @Override protected void onPause(){controller.detach(this);super.onPause();}
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
        ScrollView scroll=new ScrollView(this);scroll.setFillViewport(true);LinearLayout page=style.column();page.setPadding(style.dp(20),style.dp(18),style.dp(20),style.dp(18));
        root.addView(scroll,new LinearLayout.LayoutParams(-1,-1));scroll.addView(page);
        ImageView image=new ImageView(this);image.setScaleType(ImageView.ScaleType.FIT_CENTER);image.setImportantForAccessibility(View.IMPORTANT_FOR_ACCESSIBILITY_NO);
        try{image.setImageBitmap(OfflineArtwork.mascot(this));page.addView(image,new LinearLayout.LayoutParams(-1,style.dp(96)));}catch(Exception ignored){}
        TextView title=style.text("皮克敏花苗透視鏡",22,style.ink,true);title.setGravity(Gravity.CENTER);style.add(page,title,14);
        for(int group=0;group<4;group++){
            final int selected=group;LinearLayout card=style.card(),row=new LinearLayout(this);row.setGravity(Gravity.CENTER_VERTICAL);
            row.addView(style.text(DisplayGroups.TITLES[group],18,style.ink,true),new LinearLayout.LayoutParams(0,-2,1));
            Switch toggle=new Switch(this);toggle.setShowText(false);toggle.setMinHeight(style.dp(48));toggle.setContentDescription(DisplayGroups.TITLES[group]+"開關");
            toggle.setThumbTintList(new android.content.res.ColorStateList(new int[][]{new int[]{android.R.attr.state_checked},new int[]{}},new int[]{style.surface,style.muted}));
            toggle.setTrackTintList(new android.content.res.ColorStateList(new int[][]{new int[]{android.R.attr.state_checked},new int[]{}},new int[]{style.green,style.muted}));
            switches[group]=toggle;row.addView(toggle,new LinearLayout.LayoutParams(style.dp(76),-2));style.add(card,row,0);
            reports[group]=style.text("尚未核對",12,style.muted,false);style.add(card,reports[group],0);
            TextView detail=style.text(DisplayGroups.DETAILS[group],13,style.muted,false);detail.setVisibility(View.GONE);
            TextView expand=style.text("說明 ▸",12,style.green,false);expand.setPadding(0,style.dp(8),0,style.dp(8));
            expand.setOnClickListener(v->{boolean visible=detail.getVisibility()==View.VISIBLE;detail.setVisibility(visible?View.GONE:View.VISIBLE);expand.setText(visible?"說明 ▸":"說明 ▾");});
            style.add(card,expand,0);style.add(card,detail,0);style.add(page,card,10);
            toggle.setOnCheckedChangeListener((button,on)->{
                if(suppress)return;
                // A periodic read can acquire the gate after the last redraw.
                // Restore the durable value if this click could not be saved.
                if(controller.busy()){changed();return;}
                if(on&&!controller.armed()){
                    changed();consent(()->controller.setGroup(selected,true));
                }else controller.setGroup(selected,on);
            });
        }
        message=style.text("",13,style.muted,false);message.setOnClickListener(v->{if(!controller.busy())controller.refreshGroups();});style.add(page,message,12);
        TextView launch=style.text("開啟遊戲 ↗",15,style.green,true);launch.setGravity(Gravity.CENTER);launch.setPadding(0,style.dp(10),0,style.dp(10));
        launch.setOnClickListener(v->{
            Runnable open=()->{if(controller.anyGroup()&&controller.armed())LensService.start(this);Intent game=getPackageManager().getLaunchIntentForPackage("com.nianticlabs.pikmin");if(game!=null)startActivity(game);};
            if(controller.anyGroup()&&!controller.armed())consent(open);else open.run();
        });style.add(page,launch,12);
        String version="未知";try{version=getPackageManager().getPackageInfo(getPackageName(),0).versionName;}catch(Exception ignored){}
        TextView footer=style.text("v"+version+"  ·  開發者 odyliao-lab",12,style.muted,false);footer.setGravity(Gravity.CENTER);style.add(page,footer,0);
        setContentView(root);root.requestApplyInsets();changed();
    }
    void consent(Runnable after){
        new AlertDialog.Builder(this).setTitle("啟用遊戲資訊")
            .setMessage("僅修改顯示。載入結果不明或逾時時，允許停止當次遊戲並保留紀錄，不自動重送；下次正常開啟的新程序會重新核對。")
            .setNegativeButton("取消",(d,w)->changed()).setPositiveButton("啟用",(d,w)->{
                if(controller.arm())after.run();else{controller.reason="啟用意願未保存，請重試。";changed();}
            }).show();
    }
    @Override public void changed(){
        if(message==null)return;boolean busy=controller.busy();String unavailable=controller.operations.extensionAvailability();
        suppress=true;
        for(int i=0;i<4;i++){
            Boolean value=controller.groupOn[i];switches[i].setChecked(Boolean.TRUE.equals(value));
            // OFF remains available even if the installed game becomes unsupported.
            switches[i].setEnabled(!busy&&value!=null&&(Boolean.TRUE.equals(value)||unavailable==null));
            switches[i].setAlpha(switches[i].isEnabled()?1f:0.45f);
            reports[i].setText(value==null?"設定未確認":controller.groupReport[i]);
        }
        suppress=false;String text=busy?controller.progress:!controller.reason.isEmpty()?controller.reason:unavailable!=null?unavailable:"啟用後開啟遊戲即可使用。";
        if(controller.state.phase==LensStatus.Phase.UNKNOWN||controller.state.phase==LensStatus.Phase.UNCERTAIN)
            text="結果不明，請正常重開遊戲；保留紀錄，不重送載入。";
        else if(controller.state.phase==LensStatus.Phase.RESTART_REQUIRED)
            text="遊戲仍有舊版顯示或載入未完成，請正常重開遊戲後重新綁定。";
        message.setText(text);message.setTextColor(controller.state.blocksEnable()?style.error:style.muted);
    }
}
