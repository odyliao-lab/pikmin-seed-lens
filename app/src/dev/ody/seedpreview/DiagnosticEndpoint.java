package dev.ody.seedpreview;
import java.net.URI;
/** Empty until the owner deploys and verifies the private receiver. No user-supplied endpoints. */
final class DiagnosticEndpoint {
 static final String URL="";
 static boolean allowed(String value){try{URI u=new URI(value);String h=u.getHost();return "https".equals(u.getScheme())&&h!=null&&h.indexOf('.')>0&&!h.matches("[0-9.]+")&&!h.endsWith(".localhost")&&!h.equals("localhost")&&u.getUserInfo()==null&&(u.getPort()==-1||u.getPort()==443)&&u.getQuery()==null&&u.getFragment()==null&&"/v1/reports".equals(u.getRawPath());}catch(Exception e){return false;}}
 static boolean configured(){return allowed(URL);}
}
