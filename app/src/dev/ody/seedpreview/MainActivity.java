package dev.ody.seedpreview;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.*;
import android.os.*;
import android.view.*;
import android.widget.*;
import java.util.*;
import java.util.concurrent.*;

/** Single-page controls; native operation ownership stays in LensController. */
public final class MainActivity extends Activity implements LensController.Observer {
    LensController controller;
    ForestStyle style;
    LinearLayout confirmation;
    TextView stateTitle,stateDetail,note;
    Switch feature;
    CheckBox map,contain;
    Button submit,off;
    final ArrayList<Button> operationButtons=new ArrayList<>();
    LensContext confirmed;
    int generation;
    boolean suppress,resumed,confirmationOpen;
    String notice="";

    @Override public void onCreate(Bundle saved){super.onCreate(saved);controller=LensController.get(this);build();}
    @Override protected void onResume(){
        super.onResume();resumed=true;controller.attach(this);
        LensStatus old=controller.state;
        if(!controller.busy()&&old.session!=null&&!old.blocksEnable()&&System.currentTimeMillis()-old.observedAt>5000)
            controller.execute(LensStatus.Action.READ,null,false,null);
    }
    @Override protected void onPause(){
        resumed=false;clearConfirmation();confirmationOpen=false;controller.detach(this);super.onPause();
    }
    void clearConfirmation(){
        generation++;confirmed=null;suppress=true;
        if(map!=null)map.setChecked(false);if(contain!=null)contain.setChecked(false);suppress=false;
    }
    void build(){
        boolean dark=(getResources().getConfiguration().uiMode&Configuration.UI_MODE_NIGHT_MASK)==Configuration.UI_MODE_NIGHT_YES;
        style=new ForestStyle(this,dark);
        LinearLayout root=style.column();root.setBackgroundColor(style.paper);
        if(Build.VERSION.SDK_INT>=30)getWindow().setDecorFitsSystemWindows(false);
        getWindow().setStatusBarColor(Color.TRANSPARENT);getWindow().setNavigationBarColor(style.paper);
        getWindow().getDecorView().setSystemUiVisibility(dark?0:View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR|View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR);
        root.setOnApplyWindowInsetsListener((v,insets)->{
            if(Build.VERSION.SDK_INT>=30){Insets bars=insets.getInsets(WindowInsets.Type.systemBars());v.setPadding(bars.left,bars.top,bars.right,bars.bottom);}
            else v.setPadding(insets.getSystemWindowInsetLeft(),insets.getSystemWindowInsetTop(),insets.getSystemWindowInsetRight(),insets.getSystemWindowInsetBottom());
            return insets;
        });
        ScrollView scroll=new ScrollView(this);scroll.setFillViewport(true);
        LinearLayout page=style.column();page.setPadding(style.dp(20),style.dp(24),style.dp(20),style.dp(20));
        scroll.addView(page,new ScrollView.LayoutParams(-1,-2));root.addView(scroll,new LinearLayout.LayoutParams(-1,-1));
        ImageView image=new ImageView(this);image.setScaleType(ImageView.ScaleType.FIT_CENTER);
        image.setImportantForAccessibility(View.IMPORTANT_FOR_ACCESSIBILITY_NO);
        try{image.setImageBitmap(OfflineArtwork.mascot(this));page.addView(image,new LinearLayout.LayoutParams(-1,style.dp(136)));}
        catch(Exception error){android.util.Log.w("SeedLensArtwork","Offline mascot could not be decoded",error);}
        TextView title=style.text("皮克敏花苗透視鏡",23,style.ink,true);title.setGravity(Gravity.CENTER);style.add(page,title,24);
        LinearLayout card=style.card();
        LinearLayout row=new LinearLayout(this);row.setGravity(Gravity.CENTER_VERTICAL);
        TextView name=style.text(ToolCatalog.SEED_NAMES.title,18,style.ink,true);
        row.addView(name,new LinearLayout.LayoutParams(0,-2,1));
        feature=new Switch(this);feature.setShowText(false);feature.setMinHeight(style.dp(56));feature.setMinWidth(style.dp(72));
        feature.setContentDescription("花苗名稱透視開關");
        feature.setThumbTintList(new android.content.res.ColorStateList(new int[][]{new int[]{android.R.attr.state_checked},new int[]{}},new int[]{style.surface,style.muted}));
        feature.setTrackTintList(new android.content.res.ColorStateList(new int[][]{new int[]{android.R.attr.state_checked},new int[]{}},new int[]{style.green,style.muted}));
        row.addView(feature,new LinearLayout.LayoutParams(style.dp(76),-2));style.add(card,row,2);
        style.add(card,style.text("顯示花苗裝飾名稱，長字自動縮小。",14,style.muted,false),16);
        stateTitle=style.text("",16,style.ink,true);stateDetail=style.text("",13,style.muted,false);
        style.add(card,stateTitle,4);style.add(card,stateDetail,12);
        confirmation=style.column();
        map=style.check("遊戲已在主畫面，其他操作已暫停");
        contain=style.check("結果不明或逾時時，允許停止本次遊戲；不自動重送。");
        style.add(confirmation,map,2);style.add(confirmation,contain,4);
        note=style.text("",13,style.muted,false);style.add(confirmation,note,6);
        submit=action(confirmation,"啟用",true,()->submit());
        action(confirmation,"取消",false,()->{clearConfirmation();confirmationOpen=false;notice="";changed();});
        style.add(card,confirmation,0);
        action(card,"核對狀態",false,()->{clearConfirmation();confirmationOpen=false;controller.execute(LensStatus.Action.READ,null,false,null);});
        off=action(card,"關閉顯示",false,()->{clearConfirmation();confirmationOpen=false;controller.execute(LensStatus.Action.OFF,null,false,null);});
        style.add(page,card,20);
        String version="未知";
        try{android.content.pm.PackageInfo info=getPackageManager().getPackageInfo(getPackageName(),0);version=info.versionName;}
        catch(Exception ignored){}
        TextView footer=style.text("v"+version+"  ·  開發者 odyliao-lab",12,style.muted,false);
        footer.setGravity(Gravity.CENTER);style.add(page,footer,0);
        feature.setOnCheckedChangeListener((button,checked)->{
            if(suppress||controller.busy())return;
            if(!checked&&controller.state.controlOn){clearConfirmation();confirmationOpen=false;controller.execute(LensStatus.Action.OFF,null,false,null);}
            else{clearConfirmation();confirmationOpen=checked;notice="";changed();}
        });
        map.setOnCheckedChangeListener((button,checked)->{
            if(suppress)return;confirmed=null;final int current=++generation;
            if(checked)controller.confirm((found,blocked)->{
                if(current!=generation||!resumed||!map.isChecked())return;
                if(found==null||blocked){clearConfirmation();notice=blocked?"請重開遊戲後再核對。":"請先開啟遊戲主畫面。";}
                else{confirmed=found;notice="本次遊戲已核對。";}changed();
            });changed();
        });
        contain.setOnCheckedChangeListener((button,checked)->{if(!suppress)changed();});
        setContentView(root);root.requestApplyInsets();changed();
    }
    Button action(LinearLayout box,String text,boolean primary,Runnable run){
        Button button=style.button(text,primary);button.setStateListAnimator(null);button.setElevation(0);
        button.setOnClickListener(v->run.run());style.add(box,button,6);operationButtons.add(button);return button;
    }
    void submit(){
        if(controller.busy()||confirmed==null||!map.isChecked()||!contain.isChecked())return;
        LensContext consent=confirmed;clearConfirmation();confirmationOpen=false;notice="";
        controller.execute(LensStatus.Action.ENABLE,consent,true,()->{
            FutureTask<Boolean> task=new FutureTask<>(()->{
                if(!resumed||isFinishing()||isDestroyed()||!hasWindowFocus())return false;
                Intent intent=getPackageManager().getLaunchIntentForPackage("com.nianticlabs.pikmin");
                if(intent==null)return false;
                try{startActivity(intent);return true;}catch(RuntimeException error){return false;}
            });
            runOnUiThread(task);try{return task.get(5,TimeUnit.SECONDS);}finally{task.cancel(false);}
        });
    }
    @Override public void changed(){
        if(stateTitle==null)return;
        LensStatus s=controller.state;boolean busy=controller.busy();String unavailable=controller.operations.availability();
        String title,detail="";int color=style.ink;
        if(busy){title="處理中…";detail=controller.progress;}
        else if(s.phase==LensStatus.Phase.UNKNOWN||s.phase==LensStatus.Phase.UNCERTAIN){
            title="結果不明，請勿重試";color=style.error;
            detail=controller.unscopedBarrier?"請先核對，再重開遊戲後重新核對。紀錄保留。":"不代表已關閉。請重開遊戲後核對；紀錄保留。";
        }
        else if(s.phase==LensStatus.Phase.RESTART_REQUIRED){title="請重新核對";detail="先核對目前程序；若仍受限，請重開遊戲再核對。";color=style.gold;}
        else if(unavailable!=null){title="暫不支援";detail=unavailable;color=style.error;}
        else if(s.phase==LensStatus.Phase.ON){title="已啟用";color=style.green;}
        else if(s.phase==LensStatus.Phase.WAITING){title="等待花苗列表";detail="請開啟花苗或探險列表。";color=style.gold;}
        else if(s.phase==LensStatus.Phase.OFF){title="已關閉";}
        else if(s.phase==LensStatus.Phase.REFUSED){title="操作未完成";detail="請檢查 Root 與遊戲主畫面。不自動重送。";color=style.error;}
        else{title="尚未核對";detail="請先開啟遊戲主畫面，再啟用。";}
        if(!controller.reason.isEmpty()&&!s.blocksEnable()&&!busy)detail=controller.reason;
        stateTitle.setText(title);stateTitle.setTextColor(color);stateDetail.setText(detail);
        stateDetail.setVisibility(detail.isEmpty()?View.GONE:View.VISIBLE);
        for(Button button:operationButtons){button.setEnabled(!busy);button.setAlpha(busy?0.55f:1f);}
        suppress=true;feature.setChecked(s.controlOn);feature.setEnabled(!busy&&unavailable==null&&!s.blocksEnable());
        feature.setAlpha(feature.isEnabled()?1f:0.45f);if(Build.VERSION.SDK_INT>=30)feature.setStateDescription(title);suppress=false;
        off.setVisibility(s.controlOn||s.phase==LensStatus.Phase.OFF?View.GONE:View.VISIBLE);
        confirmation.setVisibility(confirmationOpen?View.VISIBLE:View.GONE);map.setEnabled(!busy);contain.setEnabled(!busy);
        note.setText(notice);note.setVisibility(notice.isEmpty()?View.GONE:View.VISIBLE);
        submit.setEnabled(!busy&&unavailable==null&&confirmed!=null&&map.isChecked()&&contain.isChecked()&&!controller.blockedFor(confirmed));
        submit.setAlpha(submit.isEnabled()?1f:0.55f);
    }
}
