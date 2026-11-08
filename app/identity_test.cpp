#include "identity_policy.h"
#include <string>
#undef NDEBUG
#include <cassert>
int main(){
    using seed_root::game_command;
    std::string main="com.nianticlabs.pikmin";
    assert(!game_command(main));assert(!game_command(""));
    assert(game_command(main+std::string(1,0)));assert(game_command(main+std::string(78,0)));
    assert(!game_command(main+":service"+char(0)));assert(!game_command(main+".fake"+char(0)));
    assert(!game_command(main+char(0)+"other"));assert(!game_command(main+char(0)+" "));
    assert(!game_command("prefix"+main+char(0)));
}
