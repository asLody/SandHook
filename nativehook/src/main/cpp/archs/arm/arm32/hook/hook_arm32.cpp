//
// Created by swift on 2019/5/23.
//

#include <log.h>
#include "code_relocate_arm32.h"
#include "hook_arm32.h"
#include "code_buffer.h"
#include "lock.h"
#include "shellcode_arm.h"

using namespace SandHook::Hook;
using namespace SandHook::Decoder;
using namespace SandHook::Asm;
using namespace SandHook::Assembler;
using namespace SandHook::Utils;

#include "assembler_arm32.h"
using namespace SandHook::RegistersA32;
void *InlineHookArm32Android::InlineHook(void *origin, void *replace) {
    AutoLock lock(hookLock);

    void* originCode;
    if (isThumbCode((Addr)origin)) {
        originCode = getThumbCodeAddress(origin);
    } else {
        LOGE("hook %d error!, only support thumb2 now!", origin);
        return nullptr;
    }

    bool changeMode = isThumbCode((Addr) origin) != isThumbCode((Addr) replace);

    void* backup = nullptr;
    AssemblerA32 assemblerBackup(backupBuffer);

    StaticCodeBuffer inlineBuffer = StaticCodeBuffer(reinterpret_cast<Addr>(originCode));
    AssemblerA32 assemblerInline(&inlineBuffer);
    CodeContainer* codeContainerInline = &assemblerInline.codeContainer;

    //build inline trampoline
#define __ assemblerInline.
    if (!changeMode) {
        Label *target_addr_label = new Label();
        ALIGN_FOR_LDR
        __ Ldr(PC, target_addr_label);
        __ Emit(target_addr_label);
        __ Emit((Addr) replace);
    } else {
        //to arm mode
        __ Mov(IP, (Addr) replace);
        __ Bx(IP);
    }
#undef __

    //build backup method
    CodeRelocateA32 relocate = CodeRelocateA32(assemblerBackup);
    backup = relocate.relocate(origin, codeContainerInline->size(), nullptr);
#define __ assemblerBackup.
    Label* origin_addr_label = new Label();
    ALIGN_FOR_LDR
    __ Ldr(PC, origin_addr_label);
    __ Emit(origin_addr_label);
    __ Emit((Addr) getThumbPC(reinterpret_cast<void *>(reinterpret_cast<Addr>(originCode) + relocate.curOffset)));
    __ finish();
#undef __

    //commit inline trampoline
    assemblerInline.finish();
    return getThumbPC(backup);
}


IMPORT_SHELLCODE(BP_SHELLCODE)
IMPORT_LABEL(callback_addr_s, Addr)
IMPORT_LABEL(origin_addr_s, Addr)
bool InlineHookArm32Android::BreakPoint(void *origin, void (*callback)(REG *)) {
    AutoLock lock(hookLock);

    void* originCode;
    if (isThumbCode((Addr)origin)) {
        originCode = getThumbCodeAddress(origin);
    } else {
        LOGE("hook %d error!, only support thumb2 now!", origin);
        return false;
    }

    bool changeMode = isThumbCode((Addr) origin) != isThumbCode((Addr) callback);

    void* backup = nullptr;
    AssemblerA32 assemblerBackup(backupBuffer);

    StaticCodeBuffer inlineBuffer = StaticCodeBuffer(reinterpret_cast<Addr>(originCode));
    AssemblerA32 assemblerInline(&inlineBuffer);

    //build backup method
    CodeRelocateA32 relocate = CodeRelocateA32(assemblerBackup);
    backup = relocate.relocate(origin, changeMode ? (4 * 2 + 2) : (4 * 2), nullptr);
#define __ assemblerBackup.
    Label* origin_addr_label = new Label();
    ALIGN_FOR_LDR
    __ Ldr(PC, origin_addr_label);
    __ Emit(origin_addr_label);
    __ Emit((Addr) getThumbPC(reinterpret_cast<void *>(reinterpret_cast<Addr>(originCode) + relocate.curOffset)));
    __ finish();
#undef __

    //build trampoline
    origin_addr_s = (Addr) getThumbPC(backup);
    callback_addr_s = (Addr) callback;
    void* trampoline = backupBuffer->copy((void*)BP_SHELLCODE, SHELLCODE_LEN(BP_SHELLCODE));

    //build inline trampoline
#define __ assemblerInline.
    if (!changeMode) {
        Label *target_addr_label = new Label();
        ALIGN_FOR_LDR
        __ Ldr(PC, target_addr_label);
        __ Emit(target_addr_label);
        __ Emit((Addr) trampoline);
    } else {
        __ Mov(IP, (Addr) trampoline);
        __ Bx(IP);
    }
    __ finish();
#undef __

    return true;
}
