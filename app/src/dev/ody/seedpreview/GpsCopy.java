package dev.ody.seedpreview;
/** Standalone settings/status only. Coordinates never cross into App reports. */
final class GpsCopy {
 static Boolean setting(int exit,boolean complete,String raw){if(exit!=0||!complete)return null;if("LENS_GPS\ton\n".equals(raw))return true;if("LENS_GPS\toff\n".equals(raw))return false;return null;}
 static String report(Boolean on,LensContext owner,String raw,long now){
  if(on==null)return "設定未確認";if(owner==null||raw==null||raw.length()>1024||raw.indexOf('\r')>=0||!raw.endsWith("\n"))return on?"已保存啟用，等待探險詳情":"已保存關閉";
  String[] lines=raw.split("\n");if(lines.length!=2||!lines[0].startsWith("LENS_SESSION\t"))return "等待 GPS 新鮮回報";
  LensContext context=LensContext.parse(lines[0].substring(13)+"\n");if(context==null||!context.same(owner))return "程序已變更，等待重新綁定";
  if(lines[1].equals("NO_FRESH_GPS_REPORT"))return on?"已保存啟用，等待載入遊戲":"已保存關閉";
  String[] f=lines[1].split("\t",-1);if(f.length!=11||!f[0].equals("v1")||!f[2].equals(""+owner.game.pid)||!f[3].equals("157.0")||!f[10].equals(""+owner.game.start))return "GPS 回報不符";
  try{
   long time=Long.parseLong(f[1]);if(time<now-5000||time>now+2000)return "等待 GPS 新鮮回報";
   for(int i=5;i<10;i++)if(!f[i].matches("[0-9]{1,10}")||Long.parseLong(f[i])>(i==5?1L:4294967295L))return "GPS 回報欄位無效";
   if(!f[5].equals(on?"1":"0"))return on?"等待 GPS 啟用":"等待 GPS 停用確認";
   if(!on)return "已確認關閉；不改剪貼簿";
   if(!f[4].equals("ready"))return "等待 GPS 相容能力與探險詳情";
   switch(f[8]){case "2":return "本次遊戲已複製 "+f[6]+" 次";case "3":case "5":return "目前座標或剪貼簿未確認；保留原內容";case "4":return "等待單一可見探險詳情";default:return "已啟用；選取停穩後複製";}
  }catch(NumberFormatException invalid){return "GPS 回報數值無效";}
 }
}
