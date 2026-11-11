/* Linux HOST-only isolated unit fixture; never an Android/AVD qualification.
 * The fake /proc reader exercises the production guard without launching QEMU.
 */
#define SEEDLENS_API34_HOST_RESEARCH 1
#define getenv fixture_getenv
#define readlink fixture_readlink
#define open fixture_open
#define read fixture_read
#define close fixture_close
#define write fixture_write
#define dlsym fixture_dlsym
#define _exit fixture_exit
#include "tcg_software_selection.c"
#undef getenv
#undef readlink
#undef open
#undef read
#undef close
#undef write
#undef dlsym
#undef _exit

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#if defined(SEEDLENS_API34_AOSP_HOST_RESEARCH)
#define TEST_VALID_AVD "SeedLens_API34_AOSP_ARM64_Linux"
#define TEST_VALID_PORT "5590"
#if defined(SEEDLENS_API34_AOSP_MULTI_RESEARCH)
#define TEST_EXPECTED_MARKER "seedlens-api34-aosp-tcg-multi-mapper-v1"
#elif defined(SEEDLENS_API34_AOSP_ONE_CORE_RESEARCH)
#define TEST_EXPECTED_MARKER "seedlens-api34-aosp-tcg-onecore-mapper-v1"
#else
#define TEST_EXPECTED_MARKER "seedlens-api34-aosp-tcg-single-mapper-v1"
#endif
#else
#define TEST_VALID_AVD "SeedLens_API34_ARM64_Linux"
#define TEST_VALID_PORT "5588"
#if defined(SEEDLENS_API34_TCG_SINGLE_RESEARCH)
#define TEST_EXPECTED_MARKER "seedlens-api34-tcg-single-mapper-v1"
#else
#define TEST_EXPECTED_MARKER "seedlens-api34-tcg-mapper-selection-v1"
#endif
#endif

#if defined(SEEDLENS_API34_AOSP_MULTI_RESEARCH)
#define TEST_TCG_MODE "tcg,thread=multi"
#define TEST_INVALID_TCG_MODE "tcg,thread=single"
#define TEST_CORES "2"
#define TEST_WRONG_CORES "1"
#else
#define TEST_TCG_MODE "tcg,thread=single"
#define TEST_INVALID_TCG_MODE "tcg,thread=multi"
#define TEST_CORES "1"
#define TEST_WRONG_CORES "2"
#endif

static const char *fixture_marker = TEST_EXPECTED_MARKER;
static const char *fixture_exe =
    "/opt/seedlens-lab/sdk/emulator/qemu/linux-x86_64/qemu-system-aarch64-headless";
static char fixture_argv[8192];
static long fixture_argc_bytes;
static int fixture_original_calls, fixture_has_original = 1;

char *fixture_getenv(const char *name) {
    return !strcmp(name, "SEEDLENS_TCG_SOFTWARE_ONLY") ? (char *)fixture_marker : NULL;
}
long fixture_readlink(const char *path, char *buffer, size_type size) {
    if (strcmp(path, "/proc/self/exe")) return -1;
    const size_t length = strlen(fixture_exe);
    if (length > size) return -1;
    memcpy(buffer, fixture_exe, length);
    return (long)length;
}
int fixture_open(const char *path, int flags, ...) {
    return !strcmp(path, "/proc/self/cmdline") && flags == 0 ? 7 : -1;
}
long fixture_read(int fd, void *buffer, size_type size) {
    if (fd != 7 || (size_type)fixture_argc_bytes > size) return -1;
    memcpy(buffer, fixture_argv, (size_t)fixture_argc_bytes);
    return fixture_argc_bytes;
}
int fixture_close(int fd) { return fd == 7 ? 0 : -1; }
long fixture_write(int fd, const void *buffer, size_type size) {
    (void)buffer;
    return fd == 2 ? (long)size : -1;
}
static int original_probe(void) { ++fixture_original_calls; return 1; }
void *fixture_dlsym(void *handle, const char *name) {
    return fixture_has_original && handle == (void *)-1 &&
           !strcmp(name, "_ZN7android24GetCurrentCpuAcceleratorEv") ?
           (void *)original_probe : NULL;
}
void fixture_exit(int code) { _Exit(code); }

static int run_case(const char *name, const char *const *argv, int expected) {
    fixture_argc_bytes = 0;
    for (const char *const *item = argv; *item; ++item) {
        const size_t length = strlen(*item) + 1;
        if (fixture_argc_bytes + (long)length >= (long)sizeof(fixture_argv)) abort();
        memcpy(fixture_argv + fixture_argc_bytes, *item, length);
        fixture_argc_bytes += (long)length;
    }
    const int before = fixture_original_calls;
    const int result = seedlens_software_cpu();
    if (result != expected || fixture_original_calls != before + 1) {
        fprintf(stderr, "HOST_GUARD_FAIL %s result=%d calls=%d\n",
                name, result, fixture_original_calls - before);
        return 1;
    }
    printf("HOST_GUARD_PASS %s\n", name);
    return 0;
}

int main(void) {
    const char *valid[] = {"qemu", "-avd", TEST_VALID_AVD,
                           "-port", TEST_VALID_PORT, "-no-accel",
#if defined(SEEDLENS_API34_AOSP_ONE_CORE_RESEARCH) || defined(SEEDLENS_API34_AOSP_MULTI_RESEARCH)
                           "-cores", TEST_CORES,
#endif
#if defined(SEEDLENS_API34_TCG_SINGLE_RESEARCH) || defined(SEEDLENS_API34_AOSP_MULTI_RESEARCH)
                           "-qemu", "-accel", TEST_TCG_MODE,
#endif
#if defined(SEEDLENS_API34_AOSP_ONE_CORE_RESEARCH) || defined(SEEDLENS_API34_AOSP_MULTI_RESEARCH)
                           "-smp", TEST_CORES,
#endif
                           NULL};
    int failed = run_case("exact-new-target", valid, 0);
    fixture_marker = "seedlens-tcg-mapper-selection-v1";
    failed += run_case("old-marker-refused", valid, 1);
    fixture_marker = NULL;
    failed += run_case("missing-marker-refused", valid, 1);
    fixture_marker = TEST_EXPECTED_MARKER;
    fixture_exe = "/usr/bin/true";
    failed += run_case("wrong-executable-refused", valid, 1);
    fixture_exe = expected_exe;
    const char *wrong_avd[] = {"qemu", "-avd", "SeedLens_API31_ARM64_Linux", "-port", TEST_VALID_PORT, "-no-accel", NULL};
    failed += run_case("old-avd-refused", wrong_avd, 1);
    const char *wrong_port[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", "5586", "-no-accel", NULL};
    failed += run_case("wrong-port-refused", wrong_port, 1);
    const char *duplicate_avd[] = {"qemu", "-avd", TEST_VALID_AVD, "-avd", "other", "-port", TEST_VALID_PORT, "-no-accel", NULL};
    failed += run_case("duplicate-avd-refused", duplicate_avd, 1);
    const char *duplicate_port[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, "-port", "5589", "-no-accel", NULL};
    failed += run_case("duplicate-port-refused", duplicate_port, 1);
    const char *no_software[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, NULL};
    failed += run_case("missing-software-refused", no_software, 1);
    const char *duplicate_software[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, "-no-accel", "-no-accel", NULL};
    failed += run_case("duplicate-software-refused", duplicate_software, 1);
    const char *conflicting_accel[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, "-no-accel", "-accel", "kvm", NULL};
    failed += run_case("conflicting-accelerator-refused", conflicting_accel, 1);
    const char *enable_kvm[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, "-no-accel", "-enable-kvm", NULL};
    failed += run_case("enable-kvm-refused", enable_kvm, 1);
#if defined(SEEDLENS_API34_TCG_SINGLE_RESEARCH) || defined(SEEDLENS_API34_AOSP_MULTI_RESEARCH)
    fixture_marker = "seedlens-api34-tcg-mapper-selection-v1";
    failed += run_case("default-api34-marker-refused", valid, 1);
    fixture_marker = TEST_EXPECTED_MARKER;
    const char *modes[] = {TEST_INVALID_TCG_MODE, "tcg", "off", "kvm", "tcg,thread=single,invalid=1"};
    for (unsigned int index=0; index<sizeof(modes)/sizeof(modes[0]); ++index) {
        const char *wrong_mode[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, "-no-accel", "-qemu", "-accel", modes[index], NULL};
        failed += run_case("non-exact-scheduling-refused", wrong_mode, 1);
    }
    const char *no_mode[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, "-no-accel", NULL};
    failed += run_case("missing-single-mode-refused", no_mode, 1);
    const char *front_mode[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, "-no-accel", "-accel", "tcg,thread=single", "-qemu", NULL};
    failed += run_case("frontend-accelerator-refused", front_mode, 1);
    const char *duplicate_qemu[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, "-no-accel", "-qemu", "-accel", "tcg,thread=single", "-qemu", NULL};
    failed += run_case("duplicate-qemu-refused", duplicate_qemu, 1);
    const char *duplicate_accel[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", TEST_VALID_PORT, "-no-accel", "-qemu", "-accel", "tcg,thread=single", "-accel", "tcg,thread=single", NULL};
    failed += run_case("duplicate-accelerator-refused", duplicate_accel, 1);
#if defined(SEEDLENS_API34_AOSP_HOST_RESEARCH)
    fixture_marker = "seedlens-api34-tcg-single-mapper-v1";
    failed += run_case("google-api-single-marker-refused", valid, 1);
    fixture_marker = TEST_EXPECTED_MARKER;
    const char *google_avd[] = {"qemu", "-avd", "SeedLens_API34_ARM64_Linux", "-port", TEST_VALID_PORT, "-no-accel", "-qemu", "-accel", "tcg,thread=single", NULL};
    failed += run_case("google-api-avd-refused", google_avd, 1);
    const char *google_port[] = {"qemu", "-avd", TEST_VALID_AVD, "-port", "5588", "-no-accel", "-qemu", "-accel", "tcg,thread=single", NULL};
    failed += run_case("google-api-port-refused", google_port, 1);
#if defined(SEEDLENS_API34_AOSP_ONE_CORE_RESEARCH) || defined(SEEDLENS_API34_AOSP_MULTI_RESEARCH)
    fixture_marker = "seedlens-api34-aosp-tcg-single-mapper-v1";
    failed += run_case("two-core-aosp-marker-refused", valid, 1);
    fixture_marker = TEST_EXPECTED_MARKER;
    for (unsigned int scenario=0; scenario<8; ++scenario) {
        const char *copy[32]; size_t count=0;
        while (valid[count]) { copy[count]=valid[count]; ++count; }
        copy[count]=NULL;
        size_t front=0, back=0;
        for (size_t i=0; i<count; ++i) {
            if (!strcmp(copy[i],"-cores")) front=i;
            if (!strcmp(copy[i],"-smp")) back=i;
        }
        if (scenario==0 || scenario==2) {
            size_t at=scenario==0?front:back;
            memmove(copy+at,copy+at+2,(count-at-1)*sizeof(*copy));
        } else if (scenario==1) copy[front+1]=TEST_WRONG_CORES;
        else if (scenario==3) copy[back+1]=TEST_WRONG_CORES;
        else if (scenario==4) copy[back+1]="1,cores=2";
        else if (scenario==5 || scenario==6) {
            copy[count++]=scenario==5?"-cores":"-smp";
            copy[count++]=TEST_CORES; copy[count]=NULL;
        } else {
            copy[front]="-ignored";
            copy[count++]="-cores";copy[count++]=TEST_CORES;copy[count]=NULL;
        }
        failed += run_case("ambiguous-wrong-or-missing-core-count-refused",copy,1);
    }
#endif
#endif
#endif
#if defined(SEEDLENS_API34_AOSP_MULTI_RESEARCH)
    /* Preserve valid CPU counts while varying scheduling, so refusal cannot
       accidentally be explained by an unrelated missing-count predicate. */
    const char *bad_modes[] = {"tcg,thread=single", "tcg", "off", "kvm", "tcg,thread=multi,invalid=1"};
    for (size_t i=0; i<sizeof(bad_modes)/sizeof(bad_modes[0]); ++i) {
        const char *copy[32]; size_t n=0;
        while (valid[n]) { copy[n]=valid[n]; ++n; } copy[n]=NULL;
        for (size_t j=0; j<n; ++j) if (!strcmp(copy[j],"-accel")) copy[j+1]=bad_modes[i];
        failed += run_case("wrong-scheduling-with-valid-cpu-counts",copy,1);
    }
    fixture_marker="seedlens-api34-aosp-tcg-onecore-mapper-v1";
    failed += run_case("onecore-marker-refused",valid,1);
    fixture_marker=TEST_EXPECTED_MARKER;
    for (unsigned i=0;i<2;++i) {
        const char *copy[32];size_t n=0;
        while(valid[n]){copy[n]=valid[n];++n;}
        copy[n++]=i?"-accel":"-qemu";
        if(i)copy[n++]=TEST_TCG_MODE;
        copy[n]=NULL;
        failed += run_case("duplicate-backend-with-valid-cpu-counts",copy,1);
    }
#endif
    const pid_t child = fork();
    if (child == 0) { fixture_has_original = 0; seedlens_software_cpu(); _Exit(99); }
    int status = 0;
    if (child < 0 || waitpid(child, &status, 0) != child ||
        !WIFEXITED(status) || WEXITSTATUS(status) != 86) ++failed;
    else puts("HOST_GUARD_PASS missing-original-exit86");
    printf("HOST_GUARD_SUMMARY cases=%d failed=%d android_qualification=0\n",
#if defined(SEEDLENS_API34_AOSP_MULTI_RESEARCH)
           43,
#elif defined(SEEDLENS_API34_AOSP_ONE_CORE_RESEARCH)
           35,
#elif defined(SEEDLENS_API34_AOSP_HOST_RESEARCH)
           26,
#elif defined(SEEDLENS_API34_TCG_SINGLE_RESEARCH)
           23,
#else
           13,
#endif
           failed);
    return failed ? 1 : 0;
}
