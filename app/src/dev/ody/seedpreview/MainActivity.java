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

/** Forest notebook presentation; operation ownership is process scoped. */
public final class MainActivity extends Activity implements LensController.Observer {
    LensController controller;ForestStyle style;LinearLayout page,confirmation;ScrollView scroll;
    TextView stateTitle,stateDetail,note,diagnostics;Switch feature;CheckBox map,contain;
    Button submit;final ArrayList<Button> operationButtons=new ArrayList<>();
    LensContext confirmed;int generation,tab;boolean suppress,resumed,confirmationOpen;
    String notice="";Bitmap mascot;
    @Override public void onCreate(Bundle saved){super.onCreate(saved);controller=LensController.get(this);tab=saved==null?0:saved.getInt("tab",0);if(tab<0||tab>2)tab=0;build();}
    @Override protected void onResume(){super.onResume();resumed=true;controller.attach(this);LensStatus old=controller.state;if(!controller.busy()&&old.session!=null&&!old.blocksEnable()&&System.currentTimeMillis()-old.observedAt>5000)controller.execute(LensStatus.Action.READ,null,false,null);}
    @Override protected void onPause(){resumed=false;clearConfirmation();controller.detach(this);super.onPause();}
    @Override protected void onSaveInstanceState(Bundle out){out.putInt("tab",tab);super.onSaveInstanceState(out);}
    void clearConfirmation(){generation++;confirmed=null;suppress=true;if(map!=null)map.setChecked(false);if(contain!=null)contain.setChecked(false);suppress=false;}
    void build(){
        String appearance=controller.preferences.getString("appearance","system");boolean dark=appearance.equals("dark")||(appearance.equals("system")&&(getResources().getConfiguration().uiMode&Configuration.UI_MODE_NIGHT_MASK)==Configuration.UI_MODE_NIGHT_YES);
        style=new ForestStyle(this,dark);operationButtons.clear();feature=null;map=null;contain=null;submit=null;confirmation=null;note=null;diagnostics=null;
        LinearLayout root=style.column();root.setBackgroundColor(style.paper);
        if(Build.VERSION.SDK_INT>=30)getWindow().setDecorFitsSystemWindows(false);
        getWindow().setStatusBarColor(Color.TRANSPARENT);getWindow().setNavigationBarColor(style.surface);getWindow().getDecorView().setSystemUiVisibility(dark?0:View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR|View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR);
        root.setOnApplyWindowInsetsListener((v,insets)->{if(Build.VERSION.SDK_INT>=30){Insets bars=insets.getInsets(WindowInsets.Type.systemBars());v.setPadding(bars.left,bars.top,bars.right,bars.bottom);}else v.setPadding(insets.getSystemWindowInsetLeft(),insets.getSystemWindowInsetTop(),insets.getSystemWindowInsetRight(),insets.getSystemWindowInsetBottom());return insets;});
        scroll=new ScrollView(this);scroll.setFillViewport(true);page=style.column();page.setPadding(style.dp(20),style.dp(18),style.dp(20),style.dp(20));scroll.addView(page,new ScrollView.LayoutParams(-1,-2));root.addView(scroll,new LinearLayout.LayoutParams(-1,0,1));
        style.add(page,style.text("皮克敏花苗透視鏡  ·  森林手帳",13,style.muted,true),10);if(tab==0)homeHeader();else style.add(page,style.heading(tab==1?"隨手工具":"我的手帳"),16);
        LinearLayout state=style.card();stateTitle=style.text("",20,style.ink,true);stateDetail=style.text("",14,style.muted,false);stateDetail.setAccessibilityLiveRegion(View.ACCESSIBILITY_LIVE_REGION_POLITE);style.add(state,stateTitle,6);style.add(state,stateDetail,0);style.add(page,state,16);
        if(tab==0)home();else if(tab==1)tools();else settings(appearance);
        LinearLayout nav=new LinearLayout(this);nav.setPadding(style.dp(8),style.dp(6),style.dp(8),style.dp(6));nav.setBackgroundColor(style.surface);String[] names={"首頁","工具","設定"};
        for(int i=0;i<3;i++){
            final int target=i;Button b=style.button(names[i],false);b.setMinHeight(style.dp(64));b.setTextSize(12);b.setPadding(style.dp(4),style.dp(6),style.dp(4),style.dp(6));b.setBackground(style.panel(i==tab?style.paper:style.surface,14,false));b.setStateListAnimator(null);b.setElevation(0);b.setCompoundDrawablesWithIntrinsicBounds(null,style.glyph(i,i==tab?style.green:style.muted),null,null);b.setCompoundDrawablePadding(style.dp(3));b.setSelected(i==tab);b.setContentDescription(names[i]+(i==tab?"，目前分頁":""));
            b.setOnClickListener(v->{if(tab!=target){clearConfirmation();confirmationOpen=false;notice="";tab=target;build();}});nav.addView(b,new LinearLayout.LayoutParams(0,-2,1));
        }
        root.addView(nav,new LinearLayout.LayoutParams(-1,-2));setContentView(root);root.requestApplyInsets();changed();
    }
    void homeHeader(){
        style.add(page,style.heading("看看花苗，\n藏了什麼小驚喜。"),4);style.add(page,style.text("讓名字更清楚，發現每一株的裝飾。",14,style.muted,false),4);
        ImageView image=new ImageView(this);image.setScaleType(ImageView.ScaleType.FIT_CENTER);image.setImportantForAccessibility(View.IMPORTANT_FOR_ACCESSIBILITY_NO);
        try{
            if(mascot==null)mascot=OfflineArtwork.mascot(this);
            image.setImageBitmap(mascot);
            page.addView(image,new LinearLayout.LayoutParams(-1,style.dp(190)));
        }catch(Exception error){
            android.util.Log.w("SeedLensArtwork","Offline mascot could not be decoded",error);
            style.add(page,style.text("森林夥伴插畫暫時無法顯示。",12,style.muted,false),16);
        }
    }
    Button action(LinearLayout box,String title,boolean primary,Runnable run){Button b=style.button(title,primary);b.setOnClickListener(v->run.run());style.add(box,b,10);operationButtons.add(b);return b;}
    void home(){
        action(page,"前往工具",true,()->{clearConfirmation();tab=1;confirmationOpen=false;build();});action(page,"只讀核對狀態",false,()->controller.execute(LensStatus.Action.READ,null,false,null));action(page,"關閉並還原顯示",false,()->{clearConfirmation();controller.execute(LensStatus.Action.OFF,null,false,null);});
        LinearLayout card=style.card();style.add(card,style.text("一個小工具，一起慢慢發現。",17,style.ink,true),8);style.add(card,style.text("在工具頁直接開關花苗名稱透視。啟用前會核對當次遊戲程序，關閉直接送出還原要求。",14,style.muted,false),0);style.add(page,card,0);
    }
    void tools(){
        ToolCatalog.Tool tool=ToolCatalog.SEED_NAMES;
        LinearLayout card=style.card();LinearLayout row=new LinearLayout(this);row.setGravity(Gravity.CENTER_VERTICAL);LinearLayout words=style.column();style.add(words,style.text(tool.title,18,style.ink,true),5);style.add(words,style.text(tool.summary,14,style.muted,false),0);row.addView(words,new LinearLayout.LayoutParams(0,-2,1));
        feature=new Switch(this);feature.setTag(tool.id);feature.setShowText(false);feature.setMinHeight(style.dp(56));feature.setMinWidth(style.dp(72));feature.setContentDescription(tool.title+"開關");feature.setThumbTintList(new android.content.res.ColorStateList(new int[][]{new int[]{android.R.attr.state_checked},new int[]{}},new int[]{style.surface,style.muted}));feature.setTrackTintList(new android.content.res.ColorStateList(new int[][]{new int[]{android.R.attr.state_checked},new int[]{}},new int[]{style.green,style.muted}));row.addView(feature,new LinearLayout.LayoutParams(style.dp(76),-2));style.add(card,row,12);style.add(card,style.text("開啟前會在這裡確認；關閉直接送出。",12,style.muted,false),12);
        confirmation=style.column();confirmation.setPadding(0,style.dp(8),0,0);style.add(confirmation,style.text("啟用前，確認這一次",16,style.ink,true),5);map=style.check("我已確認遊戲登入主畫面，其他操作已暫停");contain=style.check("若載入逾時或結果不明，允許停止這次遊戲程序；保留記錄、不自動重送。");style.add(confirmation,map,4);style.add(confirmation,contain,6);note=style.text(notice,13,style.muted,false);style.add(confirmation,note,8);
        submit=action(confirmation,"回到遊戲並啟用",true,()->submit());action(confirmation,"取消這次確認",false,()->{clearConfirmation();confirmationOpen=false;notice="";changed();});style.add(card,confirmation,0);style.add(page,card,16);action(page,"只讀核對狀態",false,()->controller.execute(LensStatus.Action.READ,null,false,null));style.add(page,style.text("目前只列出已實作的功能。切換分頁、離開 App 或遊戲程序改變，都需要重新確認。",13,style.muted,false),0);
        feature.setOnCheckedChangeListener((button,checked)->{
            if(suppress||controller.busy())return;
            if(!checked&&controller.state.controlOn){clearConfirmation();confirmationOpen=false;controller.execute(LensStatus.Action.OFF,null,false,null);}
            else if(checked){clearConfirmation();confirmationOpen=true;notice="勾選主畫面確認後，才會核對本次程序。";changed();scroll.post(()->scroll.smoothScrollTo(0,card.getTop()));}
            else{clearConfirmation();confirmationOpen=false;changed();}
        });
        map.setOnCheckedChangeListener((button,checked)->{
            if(suppress)return;confirmed=null;final int current=++generation;
            if(checked)controller.confirm((found,blocked)->{
                if(current!=generation||!resumed||map==null||!map.isChecked())return;
                if(found==null||blocked){clearConfirmation();notice=blocked?"這次程序仍有未確認紀錄，請先手動重開遊戲。":"找不到唯一遊戲程序，請先手動進入主畫面。";}
                else{confirmed=found;notice="本次程序已核對；送出前會再比對。";}changed();
            });changed();
        });contain.setOnCheckedChangeListener((button,checked)->{if(!suppress)changed();});
    }
    void submit(){
        if(controller.busy()||confirmed==null||!map.isChecked()||!contain.isChecked())return;LensContext consent=confirmed;clearConfirmation();confirmationOpen=false;notice="";
        controller.execute(LensStatus.Action.ENABLE,consent,true,()->{
            FutureTask<Boolean> task=new FutureTask<>(()->{if(!resumed||isFinishing()||isDestroyed()||!hasWindowFocus())return false;Intent intent=getPackageManager().getLaunchIntentForPackage("com.nianticlabs.pikmin");if(intent==null)return false;try{startActivity(intent);return true;}catch(RuntimeException error){return false;}});runOnUiThread(task);try{return task.get(5,TimeUnit.SECONDS);}finally{task.cancel(false);}
        });
    }
    void settings(String appearance){
        LinearLayout card=style.card();style.add(card,style.text("手帳外觀",18,style.ink,true),8);RadioGroup group=new RadioGroup(this);String[] values={"system","light","dark"},labels={"跟隨系統","暖色紙張","夜間森林"};
        for(int i=0;i<values.length;i++){RadioButton r=new RadioButton(this);r.setId(600+i);r.setText(labels[i]);r.setTextColor(style.ink);r.setMinHeight(style.dp(52));r.setButtonTintList(android.content.res.ColorStateList.valueOf(style.green));group.addView(r);if(appearance.equals(values[i]))r.setChecked(true);}
        group.setOnCheckedChangeListener((g,id)->{if(id>=600&&id<603){clearConfirmation();controller.preferences.edit().putString("appearance",values[id-600]).apply();build();}});style.add(card,group,0);style.add(page,card,16);
        String version="未知";try{android.content.pm.PackageInfo p=getPackageManager().getPackageInfo(getPackageName(),0);version=p.versionName+" / "+p.getLongVersionCode();}catch(Exception ignored){}
        LinearLayout about=style.card();style.add(about,style.text("關於這本手帳",18,style.ink,true),8);style.add(about,style.text("版本 "+version+"（驗證原型）\n僅顯示名稱／縮字，不修改遊戲物品。\n單一 APK＋既有 Root，不依賴控制中心、LSPosed 或額外顯示模組。",14,style.muted,false),12);style.add(about,style.text("Android "+Build.VERSION.RELEASE+" / API "+Build.VERSION.SDK_INT+"\nABI "+Build.SUPPORTED_ABIS[0]+"\n精確設定檔：156.0、156.1、157.0\nAndroid12／14 x64 的遊戲驗收尚未通過；安裝成功不代表相容。",13,style.muted,false),0);style.add(page,about,16);
        LinearLayout diag=style.card();Button show=style.button("查看進階診斷",false);diagnostics=style.text(controller.raw,12,style.muted,false);diagnostics.setTextIsSelectable(true);diagnostics.setVisibility(View.GONE);show.setOnClickListener(v->{diagnostics.setVisibility(diagnostics.getVisibility()==View.GONE?View.VISIBLE:View.GONE);show.setText(diagnostics.getVisibility()==View.GONE?"查看進階診斷":"收起進階診斷");});style.add(diag,show,8);style.add(diag,diagnostics,0);style.add(page,diag,0);
    }
    @Override public void changed(){
        if(stateTitle==null)return;LensStatus s=controller.state;boolean busy=controller.busy();String unavailable=controller.operations.availability();String title,detail;int color=style.ink;
        if(busy){title="正在處理，請等候";detail=controller.progress;}
        else if(s.phase==LensStatus.Phase.UNKNOWN||s.phase==LensStatus.Phase.UNCERTAIN){title=s.phase==LensStatus.Phase.UNKNOWN?"載入結果不明":"結果尚未確認";detail="這不代表顯示已關閉。已鎖住重新啟用；請先手動重開遊戲，再只讀核對。原紀錄會保留。";color=style.error;}
        else if(s.phase==LensStatus.Phase.RESTART_REQUIRED){title="這次載入已停止";detail="原紀錄會保留。若要再次啟用，請先重開遊戲再核對；詳細原因可查看進階診斷。";color=style.gold;}
        else if(unavailable!=null){title="這個組合尚未支援";detail=unavailable;}
        else if(s.phase==LensStatus.Phase.ON){title="已啟用";detail="本次程序已有新的顯示回報。兩個遊戲頁面的效果仍請以實際畫面確認。";color=style.green;}
        else if(s.phase==LensStatus.Phase.WAITING){title="等待花苗卡片";detail="啟用要求已送出。請開啟花苗或未領回探險列表，回來後只讀核對。";color=style.gold;}
        else if(s.phase==LensStatus.Phase.OFF){title="顯示已關閉";detail="關閉要求已確認。請回到遊戲確認原名與字型還原；程式庫會留到遊戲退出。";}
        else if(s.phase==LensStatus.Phase.REFUSED){title="這次操作未完成";detail="請核對 Root 與遊戲主畫面，必要時查看進階診斷。不會自動重送載入。";}
        else{title="尚未核對";detail="準備好後，先核對當次遊戲狀態。尚未核對不代表已關閉。";}
        if(!controller.reason.isEmpty())detail=controller.reason;stateTitle.setText(title);stateTitle.setTextColor(color);stateDetail.setText(detail);for(Button b:operationButtons){b.setEnabled(!busy);b.setAlpha(busy?0.55f:1f);}
        if(feature!=null){suppress=true;feature.setChecked(s.controlOn);feature.setEnabled(!busy&&unavailable==null&&!s.blocksEnable());feature.setAlpha(feature.isEnabled()?1f:0.45f);if(Build.VERSION.SDK_INT>=30)feature.setStateDescription(title);suppress=false;}
        if(confirmation!=null){confirmation.setVisibility(confirmationOpen?View.VISIBLE:View.GONE);map.setEnabled(!busy);contain.setEnabled(!busy);note.setText(notice);submit.setEnabled(!busy&&unavailable==null&&confirmed!=null&&map.isChecked()&&contain.isChecked()&&!controller.blockedFor(confirmed));submit.setAlpha(submit.isEnabled()?1f:0.55f);}if(diagnostics!=null)diagnostics.setText(controller.raw);
    }
}
