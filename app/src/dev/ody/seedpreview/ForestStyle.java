package dev.ody.seedpreview;
import android.content.Context;
import android.content.res.ColorStateList;
import android.graphics.*;
import android.graphics.drawable.*;
import android.view.View;
import android.widget.*;

/** Offline forest-paper tokens and simple code-native navigation glyphs. */
final class ForestStyle {
    final Context context;final boolean dark;
    final int paper,surface,ink,muted,green,onGreen,line,gold,error;
    ForestStyle(Context context,boolean dark){
        this.context=context;this.dark=dark;
        paper=c(dark?"#15231C":"#F5F3E8");surface=c(dark?"#203328":"#FFFDF6");ink=c(dark?"#EDF1E5":"#233C2E");muted=c(dark?"#BBCCBE":"#59695C");
        green=c(dark?"#A6CEAC":"#2B6045");onGreen=c(dark?"#172C20":"#FFFFFF");line=c(dark?"#47604C":"#D6DECD");gold=c(dark?"#E2BD75":"#9A6C29");error=c(dark?"#F3B0A2":"#963E31");
    }
    static int c(String value){return Color.parseColor(value);}
    int dp(float value){return Math.round(value*context.getResources().getDisplayMetrics().density);}
    GradientDrawable panel(int color,int radius,boolean border){GradientDrawable d=new GradientDrawable();d.setColor(color);d.setCornerRadius(dp(radius));if(border)d.setStroke(dp(1),line);return d;}
    LinearLayout column(){LinearLayout box=new LinearLayout(context);box.setOrientation(LinearLayout.VERTICAL);return box;}
    LinearLayout card(){LinearLayout box=column();box.setPadding(dp(18),dp(18),dp(18),dp(18));box.setBackground(panel(surface,20,true));box.setElevation(dp(1));return box;}
    TextView text(String value,int size,int color,boolean bold){TextView v=new TextView(context);v.setText(value);v.setTextSize(size);v.setTextColor(color);v.setLineSpacing(dp(3),1);if(bold)v.setTypeface(Typeface.DEFAULT,Typeface.BOLD);return v;}
    TextView heading(String value){TextView v=text(value,28,ink,false);v.setTypeface(Typeface.create("serif",Typeface.BOLD));return v;}
    Button button(String value,boolean primary){
        Button b=new Button(context);b.setText(value);b.setTextSize(15);b.setAllCaps(false);b.setMinHeight(dp(52));b.setMinimumWidth(0);b.setPadding(dp(14),dp(12),dp(14),dp(12));b.setTextColor(primary?onGreen:green);
        b.setBackground(new RippleDrawable(ColorStateList.valueOf(dark?0x33FFFFFF:0x182B6045),panel(primary?green:surface,14,!primary),null));return b;
    }
    CheckBox check(String value){CheckBox b=new CheckBox(context);b.setText(value);b.setTextSize(14);b.setTextColor(ink);b.setMinHeight(dp(56));b.setPadding(0,dp(8),0,dp(8));b.setButtonTintList(ColorStateList.valueOf(green));return b;}
    void add(LinearLayout parent,View child,int after){LinearLayout.LayoutParams p=new LinearLayout.LayoutParams(-1,-2);p.bottomMargin=dp(after);parent.addView(child,p);}
    Drawable glyph(int kind,int color){return new Glyph(kind,color,dp(24));}
    static final class Glyph extends Drawable {
        final int kind,size;final Paint paint=new Paint(3);
        Glyph(int kind,int color,int size){this.kind=kind;this.size=size;paint.setColor(color);paint.setStyle(Paint.Style.STROKE);paint.setStrokeWidth(1.8f);paint.setStrokeCap(Paint.Cap.ROUND);paint.setStrokeJoin(Paint.Join.ROUND);}
        public int getIntrinsicWidth(){return size;}public int getIntrinsicHeight(){return size;}
        public void draw(Canvas canvas){
            canvas.save();canvas.translate(getBounds().left,getBounds().top);canvas.scale(getBounds().width()/24f,getBounds().height()/24f);Path p=new Path();
            if(kind==0){p.moveTo(3,11);p.lineTo(12,3);p.lineTo(21,11);p.moveTo(6,9);p.lineTo(6,21);p.lineTo(18,21);p.lineTo(18,9);p.moveTo(10,21);p.lineTo(10,14);p.lineTo(14,14);p.lineTo(14,21);canvas.drawPath(p,paint);}
            else if(kind==1){canvas.drawCircle(10,10,6.5f,paint);canvas.drawLine(15,15,21,21,paint);}
            else{canvas.drawCircle(12,12,4,paint);for(int i=0;i<8;i++){double a=i*Math.PI/4;canvas.drawLine(12+(float)Math.cos(a)*8,12+(float)Math.sin(a)*8,12+(float)Math.cos(a)*10,12+(float)Math.sin(a)*10,paint);}canvas.drawCircle(12,12,8,paint);}
            canvas.restore();
        }
        public void setAlpha(int alpha){paint.setAlpha(alpha);}public void setColorFilter(ColorFilter filter){paint.setColorFilter(filter);}public int getOpacity(){return PixelFormat.TRANSLUCENT;}
    }
}
