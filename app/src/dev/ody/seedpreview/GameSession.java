package dev.ody.seedpreview;

/** Strict shell-result boundary. Never turn arbitrary root output into shell arguments. */
final class GameSession {
    final int pid;
    final long start;
    private GameSession(int pid,long start){this.pid=pid;this.start=start;}
    static GameSession parse(String text){
        if(text==null||!text.matches("[1-9][0-9]{0,9}\\t[1-9][0-9]{0,18}\\n?"))return null;
        String[] fields=text.trim().split("\\t",-1);
        try{
            int pid=Integer.parseInt(fields[0]);long start=Long.parseLong(fields[1]);
            return pid>1&&start>0?new GameSession(pid,start):null;
        }catch(NumberFormatException e){return null;}
    }
    boolean same(GameSession other){return other!=null&&pid==other.pid&&start==other.start;}
    String shellGuard(){return "[ \"$p\" = "+pid+" ] && [ \"$t\" = "+start+" ] || { echo GAME_SESSION_CHANGED; exit 6; }\n";}
}
