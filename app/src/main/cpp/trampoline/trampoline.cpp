//
// Created by SwiftGan on 2019/1/17.
//
#include "trampoline.h"
#include "stdlib.h"
#include "map"

namespace SandHook {

    class DirectJumpTrampoline : public Trampoline {
    public:

        DirectJumpTrampoline() : Trampoline::Trampoline() {}

        void setJumpTarget(Code target) {
            codeCopy(reinterpret_cast<char *>(&target), OFFSET_JUMP_ADDR_TARGET, BYTE_POINT);
        }

    protected:
        Size codeLength() override {
            return SIZE_DIRECT_JUMP_TRAMPOLINE;
        }

        Code templateCode() override {
            return reinterpret_cast<char*>(DIRECT_JUMP_TRAMPOLINE);
        }
    };

    class ReplacementHookTrampoline : public Trampoline {
    public:

        void setHookMethod(Code hookMethod) {
            codeCopy(reinterpret_cast<char*>(&hookMethod), OFFSET_REPLACEMENT_ADDR_ART_METHOD, BYTE_POINT);
        }

        void setEntryCodeOffset(Size offSet) {
            codeCopy(reinterpret_cast<char*>(&offSet), OFFSET_REPLACEMENT_OFFSET_ENTRY_CODE, BYTE_POINT);
        }

    protected:
        Size codeLength() override {
            return SIZE_REPLACEMENT_HOOK_TRAMPOLINE;
        }

        Code templateCode() override {
            return reinterpret_cast<char*>(REPLACEMENT_HOOK_TRAMPOLINE);
        }
    };

    class InlineHookTrampoline : public Trampoline {
    public:

        void setOriginMethod(Code originMethod) {
            codeCopy(reinterpret_cast<char*>(&originMethod), OFFSET_INLINE_ADDR_ORIGIN_METHOD, BYTE_POINT);
        }

        void setHookMethod(Code hookMethod) {
            codeCopy(reinterpret_cast<char*>(&hookMethod), OFFSET_INLINE_ADDR_HOOK_METHOD, BYTE_POINT);
        }

        void setEntryCodeOffset(Size offSet) {
            codeCopy(reinterpret_cast<char*>(&offSet), OFFSET_INLINE_OFFSET_ENTRY_CODE, BYTE_POINT);
        }

        void setOriginCode(Code originCode) {
            codeCopy(originCode, OFFSET_INLINE_ORIGIN_CODE, SIZE_DIRECT_JUMP_TRAMPOLINE);
        }

    protected:
        Size codeLength() override {
            return SIZE_REPLACEMENT_HOOK_TRAMPOLINE;
        }

        Code templateCode() override {
            return reinterpret_cast<char*>(REPLACEMENT_HOOK_TRAMPOLINE);
        }
    };

}

