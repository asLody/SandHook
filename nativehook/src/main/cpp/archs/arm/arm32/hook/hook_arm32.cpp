//
// Created by swift on 2019/5/23.
//

#include <log.h>
#include <cstdlib>
#include <cassert>
#include <signal.h>
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
void *InlineHookArm32Android::Hook(void *origin, void *replace) {
    AutoLock lock(hook_lock);

    void* origin_code;
    if (IsThumbCode((Addr) origin)) {
        origin_code = GetThumbCodeAddress(origin);
    } else {
        LOGE("hook %d error!, only support thumb2 now!", origin);
        return nullptr;
    }

    bool change_mode = IsThumbCode((Addr) origin) != IsThumbCode((Addr) replace);

    void* backup = nullptr;
    AssemblerA32 assembler_backup(backup_buffer);

    StaticCodeBuffer inline_buffer = StaticCodeBuffer(reinterpret_cast<Addr>(origin_code));
    AssemblerA32 assembler_inline(&inline_buffer);
    CodeContainer* code_container_inline = &assembler_inline.code_container;

    //build inline trampoline
#define __ assembler_inline.
    if (!change_mode) {
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
    CodeRelocateA32 relocate = CodeRelocateA32(assembler_backup);
    try {
        backup = relocate.Relocate(origin, code_container_inline->Size(), nullptr);
    } catch (ErrorCodeException e) {
        return nullptr;
    }
#define __ assembler_backup.
    Label* origin_addr_label = new Label();
    ALIGN_FOR_LDR
    __ Ldr(PC, origin_addr_label);
    __ Emit(origin_addr_label);
    __ Emit((Addr) GetThumbPC(reinterpret_cast<void *>(reinterpret_cast<Addr>(origin_code) + relocate.cur_offset)));
    __ Finish();
#undef __

    //commit inline trampoline
    assembler_inline.Finish();
    return GetThumbPC(backup);
}


IMPORT_SHELLCODE(BP_SHELLCODE)
IMPORT_LABEL(callback_addr_s, Addr)
IMPORT_LABEL(origin_addr_s, Addr)
bool InlineHookArm32Android::BreakPoint(void *origin, void (*callback)(REG *)) {
    if (origin == nullptr || callback == nullptr)
        return false;
    AutoLock lock(hook_lock);

    void* origin_code;
    if (IsThumbCode((Addr) origin)) {
        origin_code = GetThumbCodeAddress(origin);
    } else {
        LOGE("hook %d error!, only support thumb2 now!", origin);
        return false;
    }

    bool change_mode = IsThumbCode((Addr) origin) != IsThumbCode((Addr) callback);

    void* backup = nullptr;
    AssemblerA32 assembler_backup(backup_buffer);

    StaticCodeBuffer inline_buffer = StaticCodeBuffer(reinterpret_cast<Addr>(origin_code));
    AssemblerA32 assembler_inline(&inline_buffer);

    //build backup method
    CodeRelocateA32 relocate = CodeRelocateA32(assembler_backup);
    try {
        backup = relocate.Relocate(origin, change_mode ? (4 * 2 + 2) : (4 * 2), nullptr);
    } catch (ErrorCodeException e) {
        return nullptr;
    }
#define __ assembler_backup.
    Label* origin_addr_label = new Label();
    ALIGN_FOR_LDR
    __ Ldr(PC, origin_addr_label);
    __ Emit(origin_addr_label);
    __ Emit((Addr) GetThumbPC(reinterpret_cast<void *>(reinterpret_cast<Addr>(origin_code) + relocate.cur_offset)));
    __ Finish();
#undef __

    //build trampoline
    origin_addr_s = (Addr) GetThumbPC(backup);
    callback_addr_s = (Addr) callback;
    void* trampoline = backup_buffer->Copy((void*)BP_SHELLCODE, SHELLCODE_LEN(BP_SHELLCODE));

    //build inline trampoline
#define __ assembler_inline.
    if (!change_mode) {
        Label *target_addr_label = new Label();
        ALIGN_FOR_LDR
        __ Ldr(PC, target_addr_label);
        __ Emit(target_addr_label);
        __ Emit((Addr) trampoline);
    } else {
        __ Mov(IP, (Addr) trampoline);
        __ Bx(IP);
    }
    __ Finish();
#undef __

    return true;
}

void *InlineHookArm32Android::SingleInstHook(void *origin, void *replace) {
    if (origin == nullptr || replace == nullptr)
        return nullptr;
    if (!InitForSingleInstHook()) {
        return nullptr;
    }
    AutoLock lock(hook_lock);

    void* origin_code;
    if (IsThumbCode((Addr) origin)) {
        origin_code = GetThumbCodeAddress(origin);
    } else {
        LOGE("hook %d error!, only support thumb2 now!", origin);
        return nullptr;
    }

    void* backup = nullptr;
    AssemblerA32 assembler_backup(backup_buffer);

    StaticCodeBuffer inline_buffer = StaticCodeBuffer(reinterpret_cast<Addr>(origin_code));
    AssemblerA32 assembler_inline(&inline_buffer);
    CodeContainer* code_container_inline = &assembler_inline.code_container;

    //build inline trampoline
#define __ assembler_inline.
    __ Hvc(static_cast<U16>(hook_infos.size()));
#undef __

    //build backup method
    bool rec_to_thumb = true;
    CodeRelocateA32 relocate = CodeRelocateA32(assembler_backup);
    try {
        backup = relocate.Relocate(origin, code_container_inline->Size(), nullptr);
    } catch (ErrorCodeException e) {
        if (e.Code() == ERROR_SWITCH_TO_ARM32) {
            //to arm32 mode
            rec_to_thumb = false;
            assembler_backup.AllocBufferFirst(4 * 4);
            backup = assembler_backup.GetPC();
        } else {
            return nullptr;
        }
    }
    Addr rec_addr = reinterpret_cast<Addr>(origin_code);
    if (rec_to_thumb) {
        rec_addr += relocate.cur_offset;
        rec_addr = reinterpret_cast<Addr>(GetThumbPC(reinterpret_cast<void *>(rec_addr)));
    } else {
        rec_addr += 4;
    }
#define __ assembler_backup.
    Label *origin_addr_label = new Label();
    ALIGN_FOR_LDR
    __ Ldr(PC, origin_addr_label);
    __ Emit(origin_addr_label);
    __ Emit(rec_addr);
    __ Finish();
#undef __

    hook_infos.push_back({false, nullptr, origin, replace, GetThumbPC(backup)});

    //commit inline trampoline
    assembler_inline.Finish();
    return GetThumbPC(backup);
}

bool InlineHookArm32Android::SingleBreakPoint(void *point, BreakCallback callback, void *data) {
    if (point == nullptr || callback == nullptr)
        return false;
    if (!InitForSingleInstHook())
        return false;
    AutoLock lock(hook_lock);

    bool is_a32 = false;
    void* origin_code;
    if (IsThumbCode((Addr) point)) {
        origin_code = GetThumbCodeAddress(point);
    } else {
        origin_code = point;
        is_a32 = true;
    }

    void* backup = nullptr;
    AssemblerA32 assembler_backup(backup_buffer);

    StaticCodeBuffer inline_buffer = StaticCodeBuffer(reinterpret_cast<Addr>(origin_code));
    AssemblerA32 assembler_inline(&inline_buffer);
    CodeContainer* code_container_inline = &assembler_inline.code_container;

    //build inline trampoline
#define __ assembler_inline.
    __ Hvc(static_cast<U16>(hook_infos.size()));
#undef __

    //build backup method
    CodeRelocateA32 relocate = CodeRelocateA32(assembler_backup);
    if (!is_a32) {
        try {
            backup = relocate.Relocate(point, code_container_inline->Size(), nullptr);
        } catch (ErrorCodeException e) {
            return nullptr;
        }
    } else {
        // a32 emit directly temp
        assembler_backup.AllocBufferFirst(4 * 8);
        backup = assembler_backup.GetPC();
        assembler_backup.Emit(*reinterpret_cast<U32*>(origin_code));
    }
#define __ assembler_backup.
    Label* origin_addr_label = new Label();
    ALIGN_FOR_LDR
    __ Ldr(PC, origin_addr_label);
    __ Emit(origin_addr_label);
    __ Emit((Addr) GetThumbPC(reinterpret_cast<void *>(reinterpret_cast<Addr>(origin_code) + relocate.cur_offset)));
    __ Finish();
#undef __

    hook_infos.push_back({true, data, point, (void*)callback, GetThumbPC(backup)});

    //commit inline trampoline
    assembler_inline.Finish();
    return true;
}

bool InlineHookArm32Android::ExceptionHandler(int num, sigcontext *context) {
    InstT32 *code = reinterpret_cast<InstT32*>(context->arm_pc);
    if (!IS_OPCODE_T32(*code, HVC))
        return false;
    INST_T32(HVC) hvc(code);
    hvc.Disassemble();
    if (hvc.imme >= hook_infos.size())
        return false;
    HookInfo &hook_info = hook_infos[hvc.imme];
    if (!hook_info.is_break_point) {
        context->arm_pc = reinterpret_cast<U32>(hook_info.replace);
    } else {
        BreakCallback callback = reinterpret_cast<BreakCallback>(hook_info.replace);
        if (callback(context, hook_info.user_data)) {
            context->arm_pc = reinterpret_cast<U32>(hook_info.backup);
        } else {
            context->arm_pc += 4;
        }
    }
    return true;
}
