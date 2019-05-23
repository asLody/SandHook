//
// Created by swift on 2019/5/14.
//

#include "hook.h"

#include "code_buffer.h"
#include "lock.h"

using namespace SandHook::Hook;
using namespace SandHook::Decoder;
using namespace SandHook::Asm;
using namespace SandHook::Assembler;
using namespace SandHook::Utils;



#include "assembler_arm64.h"
#include "code_relocate_arm64.h"
#include "code_relocate_arm64.h"
using namespace SandHook::RegistersA64;
AndroidCodeBuffer* backupBuffer = new AndroidCodeBuffer();
void *InlineHookArm64Android::inlineHook(void *origin, void *replace) {
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
    __ Emit(reinterpret_cast<Addr>(replace));
#undef __

    //build backup method
    CodeRelocateA64 relocate = CodeRelocateA64(assemblerBackup);
    backup = relocate.relocate(origin, codeContainerInline->size(), nullptr);
#define __ assemblerBackup.
    Label* origin_addr_label = new Label();
    __ Ldr(IP1, origin_addr_label);
    __ Br(IP1);
    __ Emit(origin_addr_label);
    __ Emit(reinterpret_cast<Addr>(origin) + codeContainerInline->size());
    __ finish();
#undef __

    //commit inline trampoline
    assemblerInline.finish();
    return backup;
}
