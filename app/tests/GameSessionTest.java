package dev.ody.seedpreview;

public final class GameSessionTest {
    static int checks;
    static void check(boolean value){checks++;if(!value)throw new AssertionError("session case "+checks);}
    public static void main(String[] args){
        GameSession s=GameSession.parse("20468\t5003555\n");check(s!=null);
        check(s.same(GameSession.parse("20468\t5003555")));
        check(!s.same(GameSession.parse("20469\t5003555")));
        check(!s.same(GameSession.parse("20468\t5003556")));
        check(!s.same(null));
        for(String bad:new String[]{null,"","1\t2","0\t2","2\t0","02\t3","2\t03","-2\t3","2\t-3",
            "2147483648\t3","2\t9223372036854775808","2 3\t4","2\t3\n4\t5","2\t3\n\n",
            "2\t3;id","2\t$(id)","warning\n2\t3"," 2\t3","2\t3 ","2\t3\r\n","2\t3\t4"})check(GameSession.parse(bad)==null);
        check(s.shellGuard().contains("20468")&&s.shellGuard().contains("5003555"));
        System.out.println("Game session checks: "+checks);
    }
}
