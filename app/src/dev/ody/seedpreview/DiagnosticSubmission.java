package dev.ody.seedpreview;
import java.nio.charset.StandardCharsets;
/** Contact is a separate consented field, never part of the diagnostic/export. */
final class DiagnosticSubmission {
 static final int MAX_BYTES=18432;
 static String email(String input){
  if(input==null)return "";String value=input.trim();if(value.isEmpty())return "";
  int at=value.indexOf('@');
  if(value.length()>254||at<1||at>64||!value.matches("[A-Za-z0-9.!#$%&'*+/=?^_`{|}~-]+@[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?(?:\\.[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?)+")||value.startsWith(".")||value.contains("..")||value.charAt(at-1)=='.')throw new IllegalArgumentException("Email 格式不正確，請更正或留空");
  return value;
 }
 static byte[] body(DiagnosticReport report,String input,boolean consent){
  if(!consent)throw new IllegalArgumentException("consent required");
  String contact=email(input);
  if(report.json.getBytes(StandardCharsets.UTF_8).length>DiagnosticReport.MAX_BYTES)throw new IllegalArgumentException("report size");
  // Accepted mailbox alphabet excludes quotes, backslashes and control characters.
  byte[] body=("{\"report\":"+report.json+",\"email\":\""+contact+"\"}").getBytes(StandardCharsets.UTF_8);
  if(body.length>MAX_BYTES)throw new IllegalArgumentException("submission size");return body;
 }
}
