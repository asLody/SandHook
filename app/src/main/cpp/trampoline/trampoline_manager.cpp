//
// Created by swift on 2019/1/20.
//
#include "trampoline_manager.h"

namespace SandHook {

    void TrampolineManager::init() {

    }

    Code TrampolineManager::allocExecuteSpace(Size size) {
        if (size > MMAP_PAGE_SIZE)
            return 0;
        AutoLock autoLock(allocSpaceLock);
        void* mmapRes;
        Code exeSpace = 0;
        if (executeSpaceList.size() == 0) {
            goto label_alloc_new_space;
        } else if (executePageOffset + size > MMAP_PAGE_SIZE) {
            goto label_alloc_new_space;
        } else {
            exeSpace = executeSpaceList.back();
            Code retSpace = exeSpace + executePageOffset;
            executePageOffset += size;
            return retSpace;
        }
    label_alloc_new_space:
        mmapRes = mmap(NULL, MMAP_PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
                             MAP_ANON | MAP_PRIVATE, -1, 0);
        if (mmapRes == MAP_FAILED) {
            return 0;
        }
        exeSpace = static_cast<Code>(mmapRes);
        executeSpaceList.push_back(exeSpace);
        executePageOffset = size;
        return exeSpace;
    }

    HookTrampoline* TrampolineManager::installReplacementTrampoline(void *originMethod,
                                                                    void *hookMethod,
                                                                    void *backupMethod) {
        if (trampolines.count(originMethod) != 0)
            return getHookTrampoline(originMethod);
        HookTrampoline* hookTrampoline = new HookTrampoline();
    }

    HookTrampoline* TrampolineManager::installInlineTrampoline(void *originMethod, void *hookMethod,
                                                        void *backupMethod) {
        if (trampolines.count(originMethod) != 0)
            return getHookTrampoline(originMethod);
        HookTrampoline* hookTrampoline = new HookTrampoline();

        DirectJumpTrampoline* directJumpTrampoline = new DirectJumpTrampoline();


    }

}
