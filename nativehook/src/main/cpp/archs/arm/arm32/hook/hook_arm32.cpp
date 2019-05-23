//
// Created by swift on 2019/5/23.
//

#include "code_relocate_arm32.h"
#include "hook_arm32.h"
#include "code_buffer.h"
#include "lock.h"

using namespace SandHook::Hook;
using namespace SandHook::Decoder;
using namespace SandHook::Asm;
using namespace SandHook::Assembler;
using namespace SandHook::Utils;

#include "assembler_arm32.h"
using namespace SandHook::RegistersA32;
void *InlineHookArm32Android::inlineHook(void *origin, void *replace) {
    AutoLock lock(hookLock);

    void* originCode = origin;
    if (isThumbCode(reinterpret_cast<Addr>(origin))) {
        originCode = getThumbCodeAddress(origin);
    }

    if (isThumbCode(reinterpret_cast<Addr>(replace))) {
        replace = getThumbPC(replace);
    }

    void* backup = nullptr;
    AssemblerA32 assemblerBackup(backupBuffer);

    StaticCodeBuffer inlineBuffer = StaticCodeBuffer(reinterpret_cast<Addr>(originCode));
    AssemblerA32 assemblerInline(&inlineBuffer);
    CodeContainer* codeContainerInline = &assemblerInline.codeContainer;

    //build inline trampoline
#define __ assemblerInline.
    Label* target_addr_label = new Label();
    __ Ldr(PC, target_addr_label);
    __ Emit(target_addr_label);
    __ Emit(reinterpret_cast<Addr>(replace));
#undef __

    //build backup method
    CodeRelocateA32 relocate = CodeRelocateA32(assemblerBackup);
    backup = relocate.relocate(origin, codeContainerInline->size(), nullptr);
#define __ assemblerBackup.
    Label* origin_addr_label = new Label();
    __ Ldr(PC, origin_addr_label);
    __ Emit(origin_addr_label);
    __ Emit((Addr) getThumbPC(reinterpret_cast<void *>(reinterpret_cast<Addr>(originCode) + relocate.curOffset)));
    __ finish();
#undef __

    //commit inline trampoline
    assemblerInline.finish();
    return getThumbPC(backup);
}