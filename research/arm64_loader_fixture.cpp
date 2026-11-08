#include <cstdint>
// Disposable loader fixture only: no JNI, game, IO, network or thread access.
static unsigned constructed=0;
__attribute__((constructor)) static void initialize(){constructed=1;}
extern "C" __attribute__((visibility("default"))) uintptr_t SeedArm64Fixture(uintptr_t input){
    return constructed==1 ? (input^uintptr_t(0x53454544)) : 0;
}
