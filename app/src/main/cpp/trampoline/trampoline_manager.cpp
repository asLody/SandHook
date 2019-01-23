//
// Created by swift on 2019/1/20.
//
#include "trampoline_manager.h"

namespace SandHook {

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

    HookTrampoline* TrampolineManager::installReplacementTrampoline(mirror::ArtMethod *originMethod,
                                                                    mirror::ArtMethod *hookMethod,
                                                                    mirror::ArtMethod *backupMethod) {
        AutoLock autoLock(installLock);

        if (trampolines.count(originMethod) != 0)
            return getHookTrampoline(originMethod);
        HookTrampoline* hookTrampoline = new HookTrampoline();
        ReplacementHookTrampoline* replacementHookTrampoline = nullptr;
        Code replacementHookTrampolineSpace;

        replacementHookTrampoline = new ReplacementHookTrampoline();
        replacementHookTrampoline->init();
        replacementHookTrampolineSpace = allocExecuteSpace(replacementHookTrampoline->getCodeLen());
        if (replacementHookTrampolineSpace == 0)
            goto label_error;
        replacementHookTrampoline->setExecuteSpace(replacementHookTrampolineSpace);
        replacementHookTrampoline->setEntryCodeOffset(quickCompileOffset);
        replacementHookTrampoline->setHookMethod(reinterpret_cast<Code>(hookMethod));
        hookTrampoline->replacement = replacementHookTrampoline;

        trampolines[originMethod] = hookTrampoline;
        return hookTrampoline;

    label_error:
        delete hookTrampoline;
        delete replacementHookTrampoline;
        return nullptr;
    }

    HookTrampoline* TrampolineManager::installInlineTrampoline(mirror::ArtMethod *originMethod,
                                                               mirror::ArtMethod *hookMethod,
                                                               mirror::ArtMethod *backupMethod) {

        AutoLock autoLock(installLock);

        if (trampolines.count(originMethod) != 0)
            return getHookTrampoline(originMethod);
        HookTrampoline* hookTrampoline = new HookTrampoline();
        InlineHookTrampoline* inlineHookTrampoline = nullptr;
        DirectJumpTrampoline* directJumpTrampoline = nullptr;
        CallOriginTrampoline* callOriginTrampoline = nullptr;
        Code inlineHookTrampolineSpace;
        Code callOriginTrampolineSpace;
        Code originEntry;

        //生成二段跳板
        inlineHookTrampoline = new InlineHookTrampoline();
        checkThumbCode(inlineHookTrampoline, getEntryCode(originMethod));
        inlineHookTrampoline->init();
        inlineHookTrampolineSpace = allocExecuteSpace(inlineHookTrampoline->getCodeLen());
        if (inlineHookTrampolineSpace == 0)
            goto label_error;
        inlineHookTrampoline->setExecuteSpace(inlineHookTrampolineSpace);
        inlineHookTrampoline->setOriginMethod(reinterpret_cast<Code>(originMethod));
        inlineHookTrampoline->setHookMethod(reinterpret_cast<Code>(hookMethod));
        inlineHookTrampoline->setEntryCodeOffset(quickCompileOffset);
        inlineHookTrampoline->setOriginCode(getEntryCode(originMethod));
        hookTrampoline->inlineSecondory = inlineHookTrampoline;

        //注入 EntryCode
        directJumpTrampoline = new DirectJumpTrampoline();
        checkThumbCode(directJumpTrampoline, getEntryCode(originMethod));
        directJumpTrampoline->init();
        originEntry = getEntryCode(originMethod);
        if (!memUnprotect(reinterpret_cast<Size>(originEntry), directJumpTrampoline->getCodeLen())) {
            goto label_error;
        }

        if (directJumpTrampoline->isThumCode()) {
            directJumpTrampoline->setExecuteSpace(directJumpTrampoline->getThumbCodeAddress(originEntry));
            directJumpTrampoline->setJumpTarget(directJumpTrampoline->getThumbCodePcAddress(inlineHookTrampoline->getCode()));
        } else {
            directJumpTrampoline->setExecuteSpace(originEntry);
            directJumpTrampoline->setJumpTarget(inlineHookTrampoline->getCode());
        }
        hookTrampoline->inlineJump = directJumpTrampoline;

        //备份原始方法
        if (backupMethod != nullptr) {
            callOriginTrampoline = new CallOriginTrampoline();
            checkThumbCode(callOriginTrampoline, getEntryCode(originMethod));
            callOriginTrampoline->init();
            callOriginTrampolineSpace = allocExecuteSpace(callOriginTrampoline->getCodeLen());
            if (callOriginTrampolineSpace == 0)
                goto label_error;
            callOriginTrampoline->setExecuteSpace(callOriginTrampolineSpace);
            callOriginTrampoline->setOriginMethod(reinterpret_cast<Code>(originMethod));
            Code originCode = nullptr;
            if (callOriginTrampoline->isThumCode()) {
                originCode = callOriginTrampoline->getThumbCodePcAddress(inlineHookTrampoline->getCallOriginCode());
            } else {
                originCode = inlineHookTrampoline->getCallOriginCode();
            }
            callOriginTrampoline->setOriginCode(originCode);
            hookTrampoline->callOrigin = callOriginTrampoline;
        }

        trampolines[originMethod] = hookTrampoline;
        return hookTrampoline;

    label_error:
        delete hookTrampoline;
        if (inlineHookTrampoline != nullptr) {
            delete inlineHookTrampoline;
        }
        if (directJumpTrampoline != nullptr) {
            delete directJumpTrampoline;
        }
        if (callOriginTrampoline != nullptr) {
            delete callOriginTrampoline;
        }
        return nullptr;
    }

}
