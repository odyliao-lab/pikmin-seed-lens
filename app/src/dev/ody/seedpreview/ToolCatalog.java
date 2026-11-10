package dev.ody.seedpreview;

/** Stable descriptors for implemented tools only. Each tool owns its controller. */
final class ToolCatalog {
    static final class Tool {
        final String id,title,summary;
        Tool(String id,String title,String summary){this.id=id;this.title=title;this.summary=summary;}
    }
    static final Tool SEED_NAMES=new Tool("seed_names","花苗名稱透視","看見花苗與未領回探險的裝飾名稱，長名字自動縮小。");
    private ToolCatalog(){}
}
