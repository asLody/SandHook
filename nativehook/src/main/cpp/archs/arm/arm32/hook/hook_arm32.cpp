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
void *InlineHookArm32Android::Hook(void *origin, void *replace) {
    AutoLock lock(hookLock);

    void* origin_code;
    if (IsThumbCode((Addr) origin)) {
        origin_code = GetThumbCodeAddress(origin);
    } else {
        LOGE("hook %d error!, only support thumb2 now!", origin);
        return nullptr;
    }

    bool changeMode = IsThumbCode((Addr) origin) != IsThumbCode((Addr) replace);

    void* backup = nullptr;
    AssemblerA32 assembler_backup(backup_buffer);

    StaticCodeBuffer inline_buffer = StaticCodeBuffer(reinterpret_cast<Addr>(origin_code));
    AssemblerA32 assembler_inline(&inline_buffer);
    CodeContainer* code_container_inline = &assembler_inline.code_container;

    //build inline trampoline
#define __ assembler_inline.
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
    CodeRelocateA32 relocate = CodeRelocateA32(assembler_backup);
    backup = relocate.Relocate(origin, code_container_inline->size(), nullptr);
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
    AutoLock lock(hookLock);

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
    backup = relocate.Relocate(origin, change_mode ? (4 * 2 + 2) : (4 * 2), nullptr);
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
    void* trampoline = backup_buffer->copy((void*)BP_SHELLCODE, SHELLCODE_LEN(BP_SHELLCODE));

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
