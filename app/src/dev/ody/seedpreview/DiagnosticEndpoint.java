package dev.ody.seedpreview;
import java.net.URI;
/** Owner-deployed, verified receiver. No user-supplied endpoints or redirects. */
final class DiagnosticEndpoint {
 static final String URL="https://seed-lens-reports.ody-liao.chatgpt.site/api/submit";
 static final String ORIGIN="https://seed-lens-reports.ody-liao.chatgpt.site";
 static boolean allowed(String value){try{URI u=new URI(value);return URL.equals(value)&&"https".equals(u.getScheme())&&"seed-lens-reports.ody-liao.chatgpt.site".equals(u.getHost())&&u.getUserInfo()==null&&u.getPort()==-1&&u.getQuery()==null&&u.getFragment()==null&&"/api/submit".equals(u.getRawPath());}catch(Exception e){return false;}}
 static boolean configured(){return allowed(URL);}
}
