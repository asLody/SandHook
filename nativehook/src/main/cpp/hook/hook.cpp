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
    if (!InlineHook::instance->ExceptionHandler(signum, &context)) {
        if (InlineHook::instance->old_sig_act.sa_sigaction) {
            InlineHook::instance->old_sig_act.sa_sigaction(signum, siginfo, uc);
        }
    }
}

bool InlineHook::InitForSingleInstHook() {
    bool do_init = false;
    {
        AutoLock lock(hook_lock);
        if (inited)
            return true;
        struct sigaction sig{};
        sigemptyset(&sig.sa_mask);
        // Notice: remove this flag if needed.
        sig.sa_flags = SA_SIGINFO;
        sig.sa_sigaction = InterruptHandler;
        if (sigaction(SIGILL, &sig, &old_sig_act) != -1) {
            inited = true;
            do_init = true;
        }
    }
    //protect sigaction
    if (do_init) {
        int (*replace)(int, struct sigaction *, struct sigaction *) = [](int sig, struct sigaction *new_sa, struct sigaction *old_sa) -> int {
            if (sig != SIGILL) {
                return InlineHook::instance->sigaction_backup(sig, new_sa, old_sa);
            } else {
                *old_sa = InlineHook::instance->old_sig_act;
                InlineHook::instance->old_sig_act = *new_sa;
                return 0;
            }
        };
        sigaction_backup = reinterpret_cast<SigAct>(SingleInstHook((void*)sigaction, (void*)replace));
    }
    return inited;
}