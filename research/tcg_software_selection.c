/* Linux x64 HOST research only. Never package in an Android APK.
 * Select the SDK's existing software-CPU mapper for the explicitly identified
 * disposable ARM64 AVD, whose QEMU core already runs without acceleration.
 * No Android API/property, Root, game, NativeBridge or readiness overrides.
 * libc declarations avoid borrowing an Android sysroot for this Linux ABI.
 */
#if !defined(__linux__) || !defined(__x86_64__) || defined(__ANDROID__)
#error This helper is for the Linux x64 research HOST only.
#endif
_Static_assert(sizeof(void *) == 8 && sizeof(long) == 8, "Linux x64 ABI required");

typedef unsigned long size_type;
extern char *getenv(const char *);
extern long readlink(const char *, char *, size_type);
extern int open(const char *, int, ...);
extern long read(int, void *, size_type);
extern int close(int);
extern long write(int, const void *, size_type);
extern void *dlsym(void *, const char *);
extern void _exit(int) __attribute__((noreturn));

static const char symbol[] = "_ZN7android24GetCurrentCpuAcceleratorEv";
static const char expected_exe[] =
    "/opt/seedlens-lab/sdk/emulator/qemu/linux-x86_64/qemu-system-aarch64-headless";
static const char expected_avd[] = "SeedLens_API31_ARM64_Linux";
static const char marker[] = "seedlens-tcg-mapper-selection-v1";

static int equal(const char *left, const char *right) {
    if (!left || !right) return 0;
    while (*left && *left == *right) { ++left; ++right; }
    return *left == *right;
}

static int approved_software_target(void) {
    if (!equal(getenv("SEEDLENS_TCG_SOFTWARE_ONLY"), marker)) return 0;
    char path[4096];
    const long length = readlink("/proc/self/exe", path, sizeof(path)-1);
    if (length <= 0 || (size_type)length >= sizeof(path)-1) return 0;
    path[length] = 0;
    if (!equal(path, expected_exe)) return 0;
    char arguments[8192];
    const int fd = open("/proc/self/cmdline", 0);
    if (fd < 0) return 0;
    const long count = read(fd, arguments, sizeof(arguments));
    close(fd);
    if (count <= 0 || (size_type)count >= sizeof(arguments) || arguments[count-1]) return 0;
    int avd = 0, software = 0;
    const char *prior = "";
    for (long offset = 0; offset < count;) {
        const char *argument = arguments+offset;
        if (equal(prior, "-avd") && equal(argument, expected_avd)) avd = 1;
        if (equal(argument, "-no-accel") ||
            (equal(prior, "-accel") && equal(argument, "off"))) software = 1;
        prior = argument;
        while (offset < count && arguments[offset]) ++offset;
        ++offset;
    }
    return avd && software;
}

/* Public SDK C++ signature returns the CpuAccelerator enum (int).
 * NONE=0 selects software; it never reports accelerated execution or success.
 */
__attribute__((visibility("default")))
int seedlens_software_cpu(void) __asm__("_ZN7android24GetCurrentCpuAcceleratorEv");
int seedlens_software_cpu(void) {
    typedef int (*original_function)(void);
    original_function original = (original_function)dlsym((void *)-1, symbol);
    if (!original) {
        static const char failure[] = "SEED_TCG_SELECTION_NO_ORIGINAL\n";
        write(2, failure, sizeof(failure)-1);
        _exit(86);
    }
    /* Preserve the real host probe/status initialization. Only the selected
     * research QEMU's mapper decision is forced to its actual software mode. */
    const int detected = original();
    if (!approved_software_target()) return detected;
    static int logged;
    if (!__atomic_exchange_n(&logged, 1, __ATOMIC_RELAXED)) {
        static const char notice[] =
            "SEED_TCG_SOFTWARE_SELECTION_V1 qualified_host=0 android_qualification=0\n";
        write(2, notice, sizeof(notice)-1);
    }
    return 0;
}
