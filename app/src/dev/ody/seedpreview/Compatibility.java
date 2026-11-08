package dev.ody.seedpreview;

/** Admission is not certification. Exact game profile, independent platform backend. */
public final class Compatibility {
    private Compatibility() {}
    public static String blockReason(int api,String abi,String version,long code) {
        if(!"156.1".equals(version)||code!=1793076318L)
            return "遊戲沒有精確配對的設定檔，保留原顯示；不會嘗試舊位址。";
        if(api<28||api>35)return "Android 版本尚未納入原型範圍，保留原顯示。";
        if("x86_64".equals(abi))
            return "NativeBridge 後端待修：Virgo 未提供公開載入命名空間，已停用此原型的 x86_64 載入；不會修改遊戲。";
        if(!"arm64-v8a".equals(abi))return "此 ABI 尚未支援，保留原顯示。";
        return null; // Candidate ARM64 backend, NOT a device acceptance claim.
    }
}
