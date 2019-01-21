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
#include "../casts/art/art.h"

namespace SandHook {

    #define MMAP_PAGE_SIZE 4 * 1024

    using namespace art;


    class HookTrampoline {
    public:

        HookTrampoline() = default;

        Trampoline* replacement = nullptr;
        Trampoline* inlineJump = nullptr;
        Trampoline* inlineSecondory = nullptr;
        Trampoline* callOrigin = nullptr;
    };

    class TrampolineManager {
    public:
        TrampolineManager() = default;
        void init(Size quickCompileOffset) {
            this->quickCompileOffset = quickCompileOffset;

        }
        Code allocExecuteSpace(Size size);

        HookTrampoline* installReplacementTrampoline(mirror::ArtMethod* originMethod, mirror::ArtMethod* hookMethod, mirror::ArtMethod* backupMethod);
        HookTrampoline* installInlineTrampoline(mirror::ArtMethod* originMethod, mirror::ArtMethod* hookMethod, mirror::ArtMethod* backupMethod);

        HookTrampoline* getHookTrampoline(mirror::ArtMethod* method) {
            return trampolines[method];
        }

        Code getEntryCode(mirror::ArtMethod* method) {
            Size* pEntryAddress = reinterpret_cast<Size*>(method + quickCompileOffset);
            Code entryCode = reinterpret_cast<Code>(*pEntryAddress);
        }


    private:

        Size quickCompileOffset;

        std::map<mirror::ArtMethod*,HookTrampoline*> trampolines = {};
        std::list<Code> executeSpaceList = std::list<Code>();
        std::mutex allocSpaceLock;
        std::mutex installLock;
        Size executePageOffset = 0;
    };

}

#endif //SANDHOOK_TRAMPOLINE_MANAGER_H
