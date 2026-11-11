#include "probe_platform.h"
using seed_probe::Abi;
using seed_probe::admit;
// Compile-time checks run on the host compiler for both Android targets. These
// validate selection only, never pidfd/ptrace/JNI/NativeBridge device behavior.
static_assert(admit("31", 31, Abi::Arm64));
static_assert(admit("34", 34, Abi::Arm64));
static_assert(admit("31", 31, Abi::X64));
static_assert(admit("35", 35, Abi::X64));
static_assert(!admit("31", 32, Abi::Arm64));
static_assert(!admit("31", 32, Abi::X64));
static_assert(!admit("34", 31, Abi::Arm64));
static_assert(!admit("35", 31, Abi::X64));
static_assert(!admit("35", 35, Abi::Arm64));
static_assert(!admit("34", 34, Abi::X64));
static_assert(!admit("32", 32, Abi::Arm64));
static_assert(!admit("32", 32, Abi::X64));
static_assert(!admit("", 31, Abi::Arm64));
static_assert(!admit("031", 31, Abi::Arm64));
static_assert(!admit("31junk", 31, Abi::X64));
static_assert(!admit("31\n", 31, Abi::X64));
static_assert(!admit("+31", 31, Abi::X64));
static_assert(!admit(" 31", 31, Abi::Arm64));
static_assert(!admit("31", -1, Abi::Arm64));
static_assert(!admit("31", 0, Abi::X64));
static_assert(!admit("31", 36, Abi::X64));
static_assert(!admit("31", 31, static_cast<Abi>(99)));
int main() { return 0; }
