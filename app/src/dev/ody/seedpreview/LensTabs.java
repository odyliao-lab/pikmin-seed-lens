package dev.ody.seedpreview;
import android.app.Activity;
import android.content.Intent;
import android.view.Gravity;
import android.widget.*;
final class LensTabs {
 static void add(Activity a,ForestStyle style,LinearLayout page,boolean diagnostic){add(a,style,page,diagnostic,false);}
 static void add(Activity a,ForestStyle style,LinearLayout page,boolean diagnostic,boolean compact){
  LinearLayout row=new LinearLayout(a);String[] labels={"工具","診斷"};
  for(int i=0;i<2;i++){final boolean target=i==1;Button b=style.button(labels[i],target==diagnostic);if(compact){b.setMinHeight(style.dp(48));b.setMinimumHeight(style.dp(48));b.setPadding(style.dp(8),style.dp(6),style.dp(8),style.dp(6));}b.setGravity(Gravity.CENTER);row.addView(b,new LinearLayout.LayoutParams(0,-2,1));
   b.setOnClickListener(v->{if(target==diagnostic)return;a.startActivity(new Intent(a,target?DiagnosticsActivity.class:MainActivity.class).addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT));if(diagnostic)a.finish();});}
  style.add(page,row,compact?8:12);
 }
}
