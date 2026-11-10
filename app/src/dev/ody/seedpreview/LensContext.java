package dev.ody.seedpreview;

/** A UI observation never crosses a boot, PID or process-start boundary. */
final class LensContext {
    final GameSession game;
    final String boot;
    private LensContext(GameSession game,String boot){this.game=game;this.boot=boot;}
    static LensContext parse(String value){
        if(value==null||!value.endsWith("\n")||value.indexOf('\r')>=0)return null;
        String[] f=value.substring(0,value.length()-1).split("\t",-1);
        if(f.length!=3||!f[2].matches("[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}"))return null;
        GameSession game=GameSession.parse(f[0]+"\t"+f[1]+"\n");
        return game==null?null:new LensContext(game,f[2]);
    }
    String record(){return game.pid+"\t"+game.start+"\t"+boot+"\n";}
    boolean same(LensContext other){return other!=null&&boot.equals(other.boot)&&game.same(other.game);}
    String shellGuard(){return game.shellGuard()+"[ \"$b\" = "+boot+" ] || { echo GAME_BOOT_CHANGED; exit 6; }\n";}
}
