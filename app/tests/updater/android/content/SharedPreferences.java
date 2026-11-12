package android.content;
public interface SharedPreferences {
 String getString(String k,String fallback);long getLong(String k,long fallback);int getInt(String k,int fallback);boolean getBoolean(String k,boolean fallback);
 Editor edit();interface Editor{Editor putString(String k,String v);Editor putLong(String k,long v);Editor putInt(String k,int v);Editor putBoolean(String k,boolean v);boolean commit();void apply();}
}
