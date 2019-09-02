//
// Created by swift on 2019/5/23.
//

#include <cstdlib>
#include <cassert>
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
    AutoLock lock(hook_lock);

    void* backup = nullptr;
    AssemblerA64 assembler_backup(backup_buffer);

    StaticCodeBuffer inline_buffer = StaticCodeBuffer(reinterpret_cast<Addr>(origin));
    AssemblerA64 assembler_inline(&inline_buffer);
    CodeContainer* code_container_inline = &assembler_inline.code_container;

    //build inline trampoline
#define __ assembler_inline.
    Label* target_addr_label = new Label();
    __ Ldr(IP1, target_addr_label);
    __ Br(IP1);
    __ Emit(target_addr_label);
    __ Emit((Addr) replace);
#undef __

    //build backup method
    CodeRelocateA64 relocate = CodeRelocateA64(assembler_backup);
    backup = relocate.Relocate(origin, code_container_inline->Size(), nullptr);
#define __ assembler_backup.
    Label* origin_addr_label = new Label();
    __ Ldr(IP1, origin_addr_label);
    __ Br(IP1);
    __ Emit(origin_addr_label);
    __ Emit((Addr) origin + code_container_inline->Size());
    __ Finish();
#undef __

    //commit inline trampoline
    assembler_inline.Finish();
    return backup;
}

bool InlineHookArm64Android::BreakPoint(void *point, void (*callback)(REG regs[])) {
    if (point == nullptr || callback == nullptr)
        return false;
    AutoLock lock(hook_lock);

    void* backup = nullptr;
    AssemblerA64 assembler_backup(backup_buffer);
    AssemblerA64 assembler_trampoline(backup_buffer);

    StaticCodeBuffer inline_buffer = StaticCodeBuffer(reinterpret_cast<Addr>(point));
    AssemblerA64 assembler_inline(&inline_buffer);


    //build backup inst
    CodeRelocateA64 relocate = CodeRelocateA64(assembler_backup);
    backup = relocate.Relocate(point, 4 * 4, nullptr);
#define __ assembler_backup.
    Label* origin_addr_label = new Label();
    __ Ldr(IP1, origin_addr_label);
    __ Br(IP1);
    __ Emit(origin_addr_label);
    __ Emit((Addr) point + 4 * 4);
    __ Finish();
#undef __


    //build shell code
#define __ assembler_trampoline.
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


    void* second_trampoline = assembler_trampoline.GetStartPC();
    //build inline trampoline
#define __ assembler_inline.
    Label* target_addr_label = new Label();
    __ Ldr(IP1, target_addr_label);
    __ Br(IP1);
    __ Emit(target_addr_label);
    __ Emit((Addr) second_trampoline);
    __ Finish();
#undef __

    return true;
}


void *InlineHookArm64Android::SingleInstHook(void *origin, void *replace) {
    if (origin == nullptr || replace == nullptr)
        return nullptr;
    if (!InitForSingleInstHook())
        return nullptr;
    AutoLock lock(hook_lock);
    void* backup = nullptr;
    AssemblerA64 assembler_backup(backup_buffer);

    StaticCodeBuffer inline_buffer = StaticCodeBuffer(reinterpret_cast<Addr>(origin));
    AssemblerA64 assembler_inline(&inline_buffer);
    CodeContainer* code_container_inline = &assembler_inline.code_container;

    //build inline trampoline
#define __ assembler_inline.
    __ Hvc(static_cast<U16>(hook_infos.size()));
#undef __

    //build backup method
    CodeRelocateA64 relocate = CodeRelocateA64(assembler_backup);
    backup = relocate.Relocate(origin, code_container_inline->Size(), nullptr);
#define __ assembler_backup.
    Label* origin_addr_label = new Label();
    __ Ldr(IP1, origin_addr_label);
    __ Br(IP1);
    __ Emit(origin_addr_label);
    __ Emit((Addr) origin + code_container_inline->Size());
    __ Finish();
#undef __

    hook_infos.push_back({false, nullptr, origin, replace, backup});

    //commit inline trampoline
    assembler_inline.Finish();
    return backup;
}

bool InlineHookArm64Android::SingleBreakPoint(void *point, BreakCallback callback, void *data) {
    if (point == nullptr || callback == nullptr)
        return false;
    if (!InitForSingleInstHook())
        return false;
    AutoLock lock(hook_lock);
    void* backup = nullptr;
    AssemblerA64 assembler_backup(backup_buffer);

    StaticCodeBuffer inline_buffer = StaticCodeBuffer(reinterpret_cast<Addr>(point));
    AssemblerA64 assembler_inline(&inline_buffer);
    CodeContainer* code_container_inline = &assembler_inline.code_container;

    //build inline trampoline
#define __ assembler_inline.
    __ Hvc(static_cast<U16>(hook_infos.size()));
#undef __

    //build backup method
    CodeRelocateA64 relocate = CodeRelocateA64(assembler_backup);
    backup = relocate.Relocate(point, code_container_inline->Size(), nullptr);
#define __ assembler_backup.
    Label* origin_addr_label = new Label();
    __ Ldr(IP1, origin_addr_label);
    __ Br(IP1);
    __ Emit(origin_addr_label);
    __ Emit((Addr) point + code_container_inline->Size());
    __ Finish();
#undef __

    hook_infos.push_back({true, data, point, (void*)callback, backup});

    //commit inline trampoline
    assembler_inline.Finish();
    return true;
}

bool InlineHookArm64Android::ExceptionHandler(int num, sigcontext *context) {
    InstA64 *code = reinterpret_cast<InstA64*>(context->pc);
    if (!IS_OPCODE_A64(*code, EXCEPTION_GEN))
        return false;
    INST_A64(EXCEPTION_GEN) hvc(code);
    hvc.Disassemble();
    if (hvc.imme >= hook_infos.size())
        return false;
    HookInfo &hook_info = hook_infos[hvc.imme];
    if (!hook_info.is_break_point) {
        context->pc = reinterpret_cast<U64>(hook_info.replace);
    } else {
        BreakCallback callback = reinterpret_cast<BreakCallback>(hook_info.replace);
        if (callback(context, hook_info.user_data)) {
            context->pc = reinterpret_cast<U64>(hook_info.backup);
        } else {
            context->pc += 4;
        }
    }
    return true;
}
