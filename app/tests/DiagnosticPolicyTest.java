package dev.ody.seedpreview;
import java.util.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
public final class DiagnosticPolicyTest {
 static int checks;static void check(boolean b){checks++;if(!b)throw new AssertionError("case "+checks);}
 static void rejected(Runnable run){boolean rejected=false;try{run.run();}catch(IllegalArgumentException e){rejected=true;}check(rejected);}
 static Map<String,String> sample(){Map<String,String> f=DiagnosticReport.emptyFacts();f.put("app_version","0.11.0");f.put("app_code","41");f.put("app_signer",DiagnosticReport.SIGNER);f.put("android_api","35");f.put("android_release","15");f.put("abi","x86_64");f.put("root","granted");f.put("bridge","libnb.so");f.put("selinux","permissive");f.put("game_version","157.0");f.put("game_code","1793360009");f.put("native_files","match");f.put("last_exit","none");return f;}
 static String advice(Map<String,String> f){return String.join("\n",DiagnosisPolicy.advice(f));}
 public static void main(String[] args)throws Exception{
  Map<String,String> f=sample();DiagnosticReport report=new DiagnosticReport("11111111-1111-4111-8111-111111111111",f,Arrays.asList("ENABLE_BEGIN","ENABLE_ON"),"cannot_enable");
  check(!report.json.contains("account"));check(report.json.contains("\"root\":\"granted\""));
  check(!advice(f).contains("未精確配對"));check(!advice(f).contains("Android／API 尚未支援"));
  String success="LENS_DIAG_ROOT\t0\nLENS_DIAG_BRIDGE\tlibnb.so\nLENS_DIAG_SELINUX\tEnforcing\n";
  RootDiagnostic parsed=RootDiagnostic.parse(0,true,success,true);
  check(parsed.root.equals("granted")&&parsed.bridge.equals("libnb.so")&&parsed.selinux.equals("enforcing")&&!parsed.namespaceMismatch);
  RootDiagnostic mismatch=RootDiagnostic.parse(6,true,"ROOT_NAMESPACE_MISMATCH\n",true);
  check(mismatch.namespaceMismatch&&mismatch.root.equals("denied_or_incompatible"));
  check(!RootDiagnostic.parse(6,true,"",true).namespaceMismatch);
  check(!RootDiagnostic.parse(6,true,"ROOT_NAMESPACE_MISMATCH\n",false).namespaceMismatch);
  check(!RootDiagnostic.parse(6,false,"ROOT_NAMESPACE_MISMATCH\n",true).namespaceMismatch);
  check(!RootDiagnostic.parse(1,true,"ROOT_NAMESPACE_MISMATCH\n",true).namespaceMismatch);
  check(!RootDiagnostic.parse(6,true,"extra\nROOT_NAMESPACE_MISMATCH\n",true).namespaceMismatch);
  check(RootDiagnostic.parse(0,false,success,true).root.equals("error"));
  check(RootDiagnostic.parse(0,true,success+success,true).root.equals("error"));
  check(RootDiagnostic.parse(1,true,success,true).root.equals("denied_or_incompatible"));
  check(RootDiagnostic.parse(0,true,success.replace("ROOT\t0","ROOT\t2000"),true).root.equals("error"));
  Map<String,String> mismatchFacts=sample();mismatchFacts.put("root",mismatch.root);
  String specific=String.join("\n",DiagnosisPolicy.advice(mismatchFacts,true));
  check(specific.contains("Global Namespace")&&!specific.contains("未取得 Root："));
  check(!advice(mismatchFacts).contains("Global Namespace"));
  DiagnosticReport compatibleReport=new DiagnosticReport(mismatchFacts,Collections.emptyList(),"root_problem");
  check(!compatibleReport.json.contains("namespace")&&!compatibleReport.json.contains("ROOT_NAMESPACE"));
  String checklist=FirstUseCheck.summary(f,true);check(checklist.contains("命名空間不符"));
  f.put("last_phase","ON");check(FirstUseCheck.summary(f,false).contains("依即時狀態確認"));
  Map<String,String> fresh=DiagnosticReport.emptyFacts();check(FirstUseCheck.summary(fresh,false).contains("檢查 Root"));
  for(String api:new String[]{"31","34","30"}){Map<String,String> next=sample();next.put("android_api",api);check(advice(next).contains("Android／API 尚未支援"));}
  Map<String,String> phone=sample();phone.put("abi","arm64-v8a");check(advice(phone).contains("不能保證此手機可用"));
  for(String root:new String[]{"unavailable","denied_or_incompatible","timeout","not_checked","error"}){Map<String,String> next=sample();next.put("root",root);check(!advice(next).equals(advice(f)));check(!advice(next).contains("系統未 Root"));}
  Map<String,String> game=sample();game.put("game_code","1793360008");check(advice(game).contains("未精確配對"));
  Map<String,String> missing=sample();missing.put("game_version","missing");check(advice(missing).contains("未找到 Pikmin Bloom"));
  Map<String,String> unknown=sample();unknown.put("last_phase","UNKNOWN");check(advice(unknown).contains("不要刪除紀錄"));
  Map<String,String> leak=sample();leak.put("account","private@example.invalid");rejected(()->new DiagnosticReport(leak,Collections.emptyList(),"other"));
  for(String value:new String[]{"private@example.invalid","Bearer token","/data/user/0/player","127.0.0.1","grant\nsecret"}){Map<String,String> next=sample();next.put("root",value);rejected(()->new DiagnosticReport(next,Collections.emptyList(),"other"));}
  rejected(()->new DiagnosticReport(f,Arrays.asList("RAW_COMMAND_OUTPUT"),"other"));
  rejected(()->new DiagnosticReport(f,Collections.emptyList(),"free text"));
  rejected(()->new DiagnosticReport("../outside",f,Collections.emptyList(),"other"));
  for(String endpoint:new String[]{"","http://reports.example.org/v1/reports","https://u:p@reports.example.org/v1/reports","https://localhost/v1/reports","https://127.0.0.1/v1/reports","https://reports.example.org/v1/reports?token=x","https://reports.example.org/other","https://reports.example.org:8443/v1/reports"})check(!DiagnosticEndpoint.allowed(endpoint));
  check(!DiagnosticEndpoint.allowed("https://reports.example.org/v1/reports"));check(DiagnosticEndpoint.allowed(DiagnosticEndpoint.URL));check(DiagnosticEndpoint.configured());
  check(DiagnosticReport.receipt("{\"status\":\"accepted\",\"report_id\":\""+report.id+"\"}",report.id));
  check(DiagnosticReport.receipt("{ \"report_id\": \""+report.id+"\", \"status\": \"accepted\" }",report.id));
  for(String body:new String[]{"{}","{\"status\":\"accepted\",\"report_id\":\"wrong\"}","{\"status\":\"accepted\",\"report_id\":\""+report.id+"\",\"extra\":1}","{\"status\":\"rejected\",\"status\":\"accepted\",\"report_id\":\""+report.id+"\"}"})check(!DiagnosticReport.receipt(body,report.id));
  try{DiagnosticUploader.send(report,false);throw new AssertionError("upload without consent");}catch(java.io.IOException expected){check(true);}
  check(!DiagnosticEndpoint.allowed(DiagnosticEndpoint.URL+"?next=evil"));
  if(args.length>0)Files.write(Paths.get(args[0]),report.json.getBytes(StandardCharsets.UTF_8));
  System.out.println("DiagnosticPolicyTest: "+checks+" privacy/advice checks; fixture contains synthetic data only");
 }
}
