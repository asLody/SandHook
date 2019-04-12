//
// Created by SwiftGan on 2019/4/12.
//

#include <syscall.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "../includes/native_hook.h"
#include "../includes/arch.h"
#include "../includes/log.h"


extern int SDK_INT;

int inline getArrayItemCount(char *const array[]) {
    int i;
    for (i = 0; array[i]; ++i);
    return i;
}

bool isSandHooker(char *const args[]) {
    int orig_arg_count = getArrayItemCount(args);

    for (int i = 0; i < orig_arg_count; i++) {
        if (SDK_INT >= ANDROID_N && strstr(args[i], "SandHooker")) {
            LOGE("skip dex2oat hooker!");
            return true;
        }
    }

    return false;
}

char **build_new_env(char *const envp[]) {
    char *provided_ld_preload = NULL;
    int provided_ld_preload_index = -1;
    int orig_envp_count = getArrayItemCount(envp);

    for (int i = 0; i < orig_envp_count; i++) {
        if (strstr(envp[i], "compiler-filter")) {
            provided_ld_preload = envp[i];
            provided_ld_preload_index = i;
        }
    }
    char ld_preload[40];
    if (provided_ld_preload) {
        sprintf(ld_preload, "--compiler-filter=%s", "speed");
    }

    int new_envp_count = orig_envp_count;
    if (SDK_INT >= ANDROID_M) {
        new_envp_count = orig_envp_count + 1;
    }
    char **new_envp = (char **) malloc(new_envp_count * sizeof(char *));
    int cur = 0;
    for (int i = 0; i < orig_envp_count; ++i) {
        if (i != provided_ld_preload_index) {
            new_envp[cur++] = envp[i];
        } else {
            new_envp[i] = ld_preload;
        }
    }

    if (new_envp_count != orig_envp_count) {
        new_envp[new_envp_count - 1] = (char *) (SDK_INT > ANDROID_N2 ? "--inline-max-code-units=0" : "--inline-depth-limit=0");
    }

    return new_envp;
}

extern "C" int fake_execve_disable_inline(const char *pathname, char *argv[], char *const envp[]) {
    if (strstr(pathname, "dex2oat")) {
        if (SDK_INT >= ANDROID_N && isSandHooker(argv)) {
            LOGE("skip dex2oat!");
            return -1;
        }
        char **new_envp = build_new_env(envp);
        LOGE("dex2oat by disable inline!");
        int ret = static_cast<int>(syscall(__NR_execve, pathname, argv, new_envp));
        free(new_envp);
        return ret;
    }
    int ret = static_cast<int>(syscall(__NR_execve, pathname, argv, envp));
    return ret;
}

extern "C" int fake_execve_disable_oat(const char *pathname, char *argv[], char *const envp[]) {
    if (strstr(pathname, "dex2oat")) {
        LOGE("skip dex2oat!");
        return -1;
    }
    return static_cast<int>(syscall(__NR_execve, pathname, argv, envp));
}

namespace SandHook {

    volatile bool hasHookedDex2oat = false;
    
    bool NativeHook::hookDex2oat(bool disableDex2oat) {
        if (hasHookedDex2oat)
            return false;

        hasHookedDex2oat = true;
        return nativeHookNoBackup(reinterpret_cast<void *>(execve),
                           reinterpret_cast<void *>(disableDex2oat ? fake_execve_disable_oat : fake_execve_disable_inline));
    }
    
}
