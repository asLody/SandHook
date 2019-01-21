//
// Created by swift on 2019/1/20.
//

#ifndef SANDHOOK_TRAMPOLINE_MANAGER_H
#define SANDHOOK_TRAMPOLINE_MANAGER_H

#include "map"
#include "list"
#include "trampoline.cpp"
#include "../utils/lock.h"
#include <sys/mman.h>

namespace SandHook {

    #define MMAP_PAGE_SIZE 4 * 1024


    class HookTrampoline {
    public:

        HookTrampoline() = default;

        Trampoline* replacement;
        Trampoline* inlineJump;
        Trampoline* inlineSecondory;
        Trampoline* callOrigin;
    };

    class TrampolineManager {
    public:
        TrampolineManager() = default;
        void init();
        Code allocExecuteSpace(Size size);

        HookTrampoline* installReplacementTrampoline(void* originMethod, void* hookMethod, void* backupMethod);
        HookTrampoline* installInlineTrampoline(void* originMethod, void* hookMethod, void* backupMethod);

        HookTrampoline* getHookTrampoline(void* method) {
            return trampolines[method];
        }


    private:
        std::map<void*,HookTrampoline*> trampolines = {};
        std::list<Code> executeSpaceList = std::list<Code>();
        std::mutex allocSpaceLock;
        std::mutex installLock;
        Size executePageOffset = 0;
    };

}

#endif //SANDHOOK_TRAMPOLINE_MANAGER_H
