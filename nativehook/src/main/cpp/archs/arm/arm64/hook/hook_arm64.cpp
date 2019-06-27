//
// Created by swift on 2019/5/23.
//

#include "hook_arm64.h"
#include "code_buffer.h"
#include "lock.h"

using namespace SandHook::Hook;
using namespace SandHook::Decoder;
using namespace SandHook::Asm;
using namespace SandHook::Assembler;
using namespace SandHook::Utils;

#include "assembler_arm64.h"
#include "code_relocate_arm64.h"
using namespace SandHook::RegistersA64;
void *InlineHookArm64Android::Hook(void *origin, void *replace) {
    AutoLock lock(hookLock);

    void* backup = nullptr;
    AssemblerA64 assemblerBackup(backupBuffer);

    StaticCodeBuffer inlineBuffer = StaticCodeBuffer(reinterpret_cast<Addr>(origin));
    AssemblerA64 assemblerInline(&inlineBuffer);
    CodeContainer* codeContainerInline = &assemblerInline.codeContainer;

    //build inline trampoline
#define __ assemblerInline.
    Label* target_addr_label = new Label();
    __ Ldr(IP1, target_addr_label);
    __ Br(IP1);
    __ Emit(target_addr_label);
    __ Emit((Addr) replace);
#undef __

    //build backup method
    CodeRelocateA64 relocate = CodeRelocateA64(assemblerBackup);
    backup = relocate.relocate(origin, codeContainerInline->size(), nullptr);
#define __ assemblerBackup.
    Label* origin_addr_label = new Label();
    __ Ldr(IP1, origin_addr_label);
    __ Br(IP1);
    __ Emit(origin_addr_label);
    __ Emit((Addr) origin + codeContainerInline->size());
    __ Finish();
#undef __

    //commit inline trampoline
    assemblerInline.Finish();
    return backup;
}

bool InlineHookArm64Android::BreakPoint(void *point, void (*callback)(REG regs[])) {
    AutoLock lock(hookLock);

    void* backup = nullptr;
    AssemblerA64 assemblerBackup(backupBuffer);
    AssemblerA64 assemblerTrampoline(backupBuffer);

    StaticCodeBuffer inlineBuffer = StaticCodeBuffer(reinterpret_cast<Addr>(point));
    AssemblerA64 assemblerInline(&inlineBuffer);


    //build backup inst
    CodeRelocateA64 relocate = CodeRelocateA64(assemblerBackup);
    backup = relocate.relocate(point, 4 * 4, nullptr);
#define __ assemblerBackup.
    Label* origin_addr_label = new Label();
    __ Ldr(IP1, origin_addr_label);
    __ Br(IP1);
    __ Emit(origin_addr_label);
    __ Emit((Addr) point + 4 * 4);
    __ Finish();
#undef __


    //build shell code
#define __ assemblerTrampoline.
    //backup NZCV
    __ Sub(SP, Operand(&SP, 0x20));

    __ Str(X0, MemOperand(&SP, 0x10));
    __ Mrs(NZCV, X0);
    __ Str(X30, MemOperand(&SP));
    __ Add(X30, Operand(&SP, 0x20));
    __ Str(X30, MemOperand(&SP, 0x8));
    __ Ldr(X0, MemOperand(&SP, 0x10));

    //backup X0 - X29
    U8 douRegCount = 30 / 2;
    __ Sub(SP, Operand(&SP, 0xf0));
    for (int i = 0; i < douRegCount; ++i) {
        __ Stp(*XReg(2 * i), *XReg(2 * i + 1), MemOperand(&SP, i * 16));
    }

    __ Mov(X0, SP);
    __ Mov(IP1, (Addr) callback);
    __ Blr(IP1);
    __ Ldr(X0, MemOperand(&SP, 0x100));
    __ Msr(NZCV, X0);

    //restore X0 - X29
    for (int i = 0; i < douRegCount; ++i) {
        __ Ldp(*XReg(2 * i), *XReg(2 * i + 1), MemOperand(&SP, i * 16));
    }

    __ Add(SP, Operand(&SP, 0xf0));

    __ Ldr(X30, MemOperand(&SP, (Off) 0));
    __ Add(SP, Operand(&SP, 0x20));

    //jump to origin
    __ Mov(IP1, (Addr) backup);
    __ Br(IP1);

    __ Finish();
#undef __


    void* secondTrampoline = assemblerTrampoline.GetStartPC();
    //build inline trampoline
#define __ assemblerInline.
    Label* target_addr_label = new Label();
    __ Ldr(IP1, target_addr_label);
    __ Br(IP1);
    __ Emit(target_addr_label);
    __ Emit((Addr) secondTrampoline);
    __ Finish();
#undef __

    return true;
}
