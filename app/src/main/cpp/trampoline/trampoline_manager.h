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
#include <unistd.h>

namespace SandHook {

    #define MMAP_PAGE_SIZE sysconf(_SC_PAGESIZE)

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

        bool memUnprotect(Size addr, Size len) {
            long pagesize = sysconf(_SC_PAGESIZE);
            unsigned alignment = (unsigned)((unsigned long long)addr % pagesize);
            int i = mprotect((void *) (addr - alignment), (size_t) (alignment + len),
                             PROT_READ | PROT_WRITE | PROT_EXEC);
            if (i == -1) {
                return false;
            }
            return true;
        }

        Code getEntryCode(void* method) {
            Code entryCode = *reinterpret_cast<Code*>((Size)method + quickCompileOffset);
            return entryCode;
        }

        bool isThumbCode(Size codeAddr) {
            return (codeAddr & (Size)1) == (Size)1;
        }

        void checkThumbCode(Trampoline* trampoline, Code code) {
            #if defined(__arm__)
            trampoline->setThumb(isThumbCode(reinterpret_cast<Size>(code)));
            #endif
        }


    private:

        Size quickCompileOffset;

        std::map<mirror::ArtMethod*,HookTrampoline*> trampolines;
        std::list<Code> executeSpaceList = std::list<Code>();
        std::mutex allocSpaceLock;
        std::mutex installLock;
        Size executePageOffset = 0;
    };

}

#endif //SANDHOOK_TRAMPOLINE_MANAGER_H
