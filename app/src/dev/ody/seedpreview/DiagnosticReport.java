package dev.ody.seedpreview;
import java.util.*;
import java.nio.charset.StandardCharsets;
/** Closed schema. Raw exceptions, command output and identifiers never enter a report. */
final class DiagnosticReport {
 static final int MAX_BYTES=16384;
 static final String SIGNER="9080c5d8c9a138a3a8eb42ec65755cf2abd64b881a7bf398feabe3547b934a7e";
 static final String[] KEYS={"app_version","app_code","app_signer","android_api","android_release","abi","manufacturer","game_version","game_code","root","bridge","selinux","native_files","last_phase","last_exit"};
 static final String[] ISSUES={"app_wont_open","root_problem","cannot_enable","text_unchanged","restore_problem","other"};
 static final String[] EVENTS={"ENABLE_BEGIN","ENABLE_ON","ENABLE_WAITING","ENABLE_REFUSED","ENABLE_UNKNOWN","ENABLE_UNCERTAIN","ENABLE_RESTART_REQUIRED","ENABLE_UNSUPPORTED","ENABLE_UNVERIFIED","ENABLE_OFF","GROUP_SAVED","ALL_OFF_BEGIN","ALL_OFF_SAVED","ALL_OFF_FAILED","LOADER_EXIT_0","LOADER_EXIT_1","LOADER_EXIT_2","LOADER_EXIT_3","LOADER_EXIT_4","LOADER_EXIT_6","LOADER_EXIT_22","LOADER_EXIT_23","LOADER_EXIT_24","LOADER_EXIT_25","LOADER_EXIT_26","LOADER_EXIT_27","LOADER_EXIT_28","LOADER_EXIT_42","LOADER_EXIT_43","LOADER_QUERY_FAILED"};
 static boolean oneOf(String value,String... options){for(String option:options)if(option.equals(value))return true;return false;}
 static boolean valid(String key,String value){
  if(value==null||value.length()>80)return false;if(value.equals("unknown"))return true;
  if(oneOf(key,"app_version","game_version","android_release"))return value.matches("[0-9]{1,4}(\\.[0-9]{1,4}){0,3}")||key.equals("game_version")&&value.equals("missing");
  if(oneOf(key,"app_code","game_code","android_api"))return value.matches("[0-9]{1,10}");
  if(key.equals("app_signer"))return value.matches("[a-f0-9]{64}");
  if(key.equals("abi"))return oneOf(value,"arm64-v8a","x86_64","armeabi-v7a","x86","other");
  if(key.equals("manufacturer"))return oneOf(value,"google","samsung","xiaomi","oneplus","oppo","vivo","sony","asus","motorola","huawei","other");
  if(key.equals("root"))return oneOf(value,"not_checked","granted","unavailable","denied_or_incompatible","timeout","error");
  if(key.equals("bridge"))return oneOf(value,"libnb.so","libndk_translation.so","none","other");
  if(key.equals("selinux"))return oneOf(value,"enforcing","permissive","disabled");
  if(key.equals("native_files"))return oneOf(value,"match","mismatch","unreadable");
  if(key.equals("last_phase"))return oneOf(value,"UNVERIFIED","WAITING","ON","OFF","REFUSED","UNKNOWN","UNCERTAIN","RESTART_REQUIRED","UNSUPPORTED");
  if(key.equals("last_exit"))return oneOf(value,"none","java_crash","native_crash","anr","low_memory","user_or_system","other","unavailable");
  return false;
 }
 static boolean receipt(String body,String id){
  if(body==null||body.length()>512)return false;
  String status="\"status\"\\s*:\\s*\"accepted\"",identity="\"report_id\"\\s*:\\s*\""+java.util.regex.Pattern.quote(id)+"\"";
  return body.matches("\\s*\\{\\s*(?:"+status+"\\s*,\\s*"+identity+"|"+identity+"\\s*,\\s*"+status+")\\s*\\}\\s*");
 }
 static Map<String,String> emptyFacts(){Map<String,String> facts=new LinkedHashMap<>();for(String key:KEYS)facts.put(key,"unknown");facts.put("root","not_checked");return facts;}
 static String quoted(String value){return "\""+value+"\"";} // All values first pass the closed alphabet above.
 final String id,json;
 DiagnosticReport(Map<String,String> facts,List<String> events,String issue){this(UUID.randomUUID().toString(),facts,events,issue);}
 DiagnosticReport(String id,Map<String,String> facts,List<String> events,String issue){
  if(!id.matches("[a-f0-9]{8}-[a-f0-9]{4}-4[a-f0-9]{3}-[89ab][a-f0-9]{3}-[a-f0-9]{12}")||!oneOf(issue,ISSUES)||facts.size()!=KEYS.length||events.size()>16)throw new IllegalArgumentException("invalid report");
  StringBuilder out=new StringBuilder("{\n  \"schema\":1,\n  \"report_id\":").append(quoted(id)).append(",\n  \"issue\":").append(quoted(issue)).append(",\n  \"facts\":{\n");
  for(int i=0;i<KEYS.length;i++){String key=KEYS[i],value=facts.get(key);if(!valid(key,value))throw new IllegalArgumentException("invalid fact");out.append("    ").append(quoted(key)).append(':').append(quoted(value)).append(i+1==KEYS.length?"\n":",\n");}
  out.append("  },\n  \"events\":[");for(int i=0;i<events.size();i++){if(!oneOf(events.get(i),EVENTS))throw new IllegalArgumentException("invalid event");if(i>0)out.append(',');out.append(quoted(events.get(i)));}out.append("]\n}\n");
  this.id=id;json=out.toString();if(json.getBytes(StandardCharsets.UTF_8).length>MAX_BYTES)throw new IllegalArgumentException("oversized report");
 }
}
