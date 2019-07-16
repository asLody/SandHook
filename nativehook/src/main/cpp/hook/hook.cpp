//
// Created by swift on 2019/5/14.
//
#include "hook.h"
#include "lock.h"

#if defined(__arm__)
#include "hook_arm32.h"
#elif defined(__aarch64__)
#include "hook_arm64.h"
#endif

using namespace SandHook::Hook;
using namespace SandHook::Utils;

CodeBuffer* InlineHook::backup_buffer = new AndroidCodeBuffer();

#if defined(__arm__)
InlineHook* InlineHook::instance = new InlineHookArm32Android();
#elif defined(__aarch64__)
InlineHook* InlineHook::instance = new InlineHookArm64Android();
#endif

void InterruptHandler(int signum, siginfo_t* siginfo, void* uc) {
    if (signum != SIGILL)
        return;
    sigcontext &context = reinterpret_cast<ucontext_t *>(uc)->uc_mcontext;
    InlineHook::instance->ExceptionHandler(signum, &context);
}

bool InlineHook::InitForSingleInstHook() {
    AutoLock lock(hook_lock);
    if (inited)
        return true;
    struct sigaction sig{};
    sigemptyset(&sig.sa_mask);
    // Notice: remove this flag if needed.
    sig.sa_flags = SA_SIGINFO;
    sig.sa_sigaction = InterruptHandler;
    if (sigaction(SIGILL, &sig, nullptr) != -1) {
        inited = true;
    }
    return inited;
}