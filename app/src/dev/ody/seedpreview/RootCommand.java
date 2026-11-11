package dev.ody.seedpreview;

/** Choose once before a command. Failed or uncertain operations never fall back. */
final class RootCommand {
    private RootCommand() {}
    static boolean nativeApi32(int api,String abi){return api==32&&"x86_64".equals(abi);}
    static String[] arguments(int api,String abi,String command){
        return nativeApi32(api,abi)?new String[]{"su","-c",command}:new String[]{"su","-M","-c",command};
    }
    static String prefix(int api,String abi){
        if(!nativeApi32(api,abi))return "";
        return "set -eu\n[ $(id -u) = 0 ]\na=$(readlink /proc/self/ns/mnt)\nb=$(readlink /proc/1/ns/mnt)\n[ -n \"$a\" ] && [ \"$a\" = \"$b\" ] || { echo ROOT_NAMESPACE_MISMATCH; exit 6; }\n";
    }
}
