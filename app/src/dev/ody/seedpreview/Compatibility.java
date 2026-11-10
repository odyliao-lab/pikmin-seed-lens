package dev.ody.seedpreview;

/** Admission is not certification. Exact game profile, independent platform backend. */
public final class Compatibility {
    // Checked-in x64 default stays API35-only. The unsigned candidate
    // build generates a private copy from the same switch as the native backend.
    static final boolean API31_CANDIDATE=false;
    static final boolean API34_CANDIDATE=false;
    private Compatibility() {}
    public static String blockReason(int api,String abi,String version,long code) {
        if(!(("156.1".equals(version)&&code==1793076318L)||("156.0".equals(version)&&code==1792142832L)
                ||("157.0".equals(version)&&code==1793360009L)))
            return "遊戲沒有精確配對的設定檔，保留原顯示；不會嘗試舊位址。";
        if(api<28||api>35)return "Android 版本尚未納入原型範圍，保留原顯示。";
        if("x86_64".equals(abi))return api==35||(API31_CANDIDATE&&api==31)||(API34_CANDIDATE&&api==34)?null:"此 Android 的 NativeBridge 尚未納入候選範圍，保留原顯示。";
        if(!"arm64-v8a".equals(abi))return "此 ABI 尚未支援，保留原顯示。";
        return null; // Candidate ARM64 backend, NOT a device acceptance claim.
    }
}
