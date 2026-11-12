package dev.ody.seedpreview;

/** Closed, read-only probe result. Namespace detail stays local; upload schema is unchanged. */
final class RootDiagnostic {
    final String root,bridge,selinux;
    final boolean namespaceMismatch;
    RootDiagnostic(String root,String bridge,String selinux,boolean mismatch){
        this.root=root;this.bridge=bridge;this.selinux=selinux;namespaceMismatch=mismatch;
    }
    static RootDiagnostic failure(String root){return new RootDiagnostic(root,"unknown","unknown",false);}
    static RootDiagnostic parse(int exit,boolean complete,String text,boolean api32){
        if(!complete||text==null||text.length()>2048)return failure("error");
        // Exit 6 alone is ambiguous. Require the exact marker from our UID0-checked prefix.
        if(api32&&exit==6&&text.equals("ROOT_NAMESPACE_MISMATCH\n"))
            return new RootDiagnostic("denied_or_incompatible","unknown","unknown",true);
        if(exit!=0)return failure("denied_or_incompatible");
        String[] lines=text.split("\n",-1);
        if(lines.length!=4||!lines[0].equals("LENS_DIAG_ROOT\t0")
                ||!lines[1].startsWith("LENS_DIAG_BRIDGE\t")||!lines[2].startsWith("LENS_DIAG_SELINUX\t")
                ||!lines[3].isEmpty())return failure("error");
        String bridge=lines[1].substring(17),selinux=lines[2].substring(18).toLowerCase(java.util.Locale.US);
        return new RootDiagnostic("granted",bridge.isEmpty()?"none":DiagnosticReport.oneOf(bridge,"libnb.so","libndk_translation.so")?bridge:"other",
                DiagnosticReport.valid("selinux",selinux)?selinux:"unknown",false);
    }
}
