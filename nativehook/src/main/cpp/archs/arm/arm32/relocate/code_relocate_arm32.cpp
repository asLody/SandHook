//
// Created by swift on 2019/5/23.
//

#include "code_relocate_arm32.h"
#include "decoder.h"
#include "lock.h"

using namespace SandHook::RegistersA32;
using namespace SandHook::AsmA32;
using namespace SandHook::Utils;
using namespace SandHook::Decoder;

#define __ assemblerA32->

#define IMPL_RELOCATE(T, X) void CodeRelocateA32::relocate_##T##_##X (INST_##T(X)* inst, void* toPc) throw(ErrorCodeException)

#define CASE(T, X) \
case ENUM_VALUE(InstCode##T, InstCode##T::X): \
relocate_##T##_##X(reinterpret_cast<INST_##T(X)*>(instruction), toPc); \
break;

CodeRelocateA32::CodeRelocateA32(AssemblerA32 &assembler) : CodeRelocate(assembler.codeContainer) {
    this->assemblerA32 = &assembler;
}

bool CodeRelocateA32::visit(Unit<Base> *unit, void *pc) {
    relocate(reinterpret_cast<Instruction<Base> *>(unit), __ getPC());
    curOffset += unit->size();
    return true;
}

void* CodeRelocateA32::relocate(void *startPc, Addr len, void *toPc = nullptr) throw(ErrorCodeException) {
    AutoLock autoLock(relocateLock);
    startAddr = reinterpret_cast<Addr>(startPc);
    if (isThumbCode(startAddr)) {
        startAddr = reinterpret_cast<Addr>(getThumbCodeAddress(startPc));
    }
    length = len;
    curOffset = 0;
    __ allocBufferFirst(static_cast<U32>(len * 8));
    void* curPc = __ getPC();
    if (toPc == nullptr) {
        Disassembler::get()->decode(startPc, len, *this);
    } else {
        //TODO
    }
    return curPc;
}

void* CodeRelocateA32::relocate(Instruction<Base> *instruction, void *toPc) throw(ErrorCodeException) {
    void* curPc = __ getPC();

    //insert later bind labels
    __ Emit(getLaterBindLabel(curOffset));

    if (!instruction->pcRelate()) {
        __ Emit(instruction);
        return curPc;
    }
    switch (instruction->instCode()) {
        CASE(T16, B)
        default:
            __ Emit(instruction);
    }
    return curPc;
}


IMPL_RELOCATE(T16, B_COND) {

}

IMPL_RELOCATE(T16, B) {

    Addr targetAddr = inst->getImmPCOffsetTarget();

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(InstT16))) {
        __ B(getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        return;
    }

    Label* target_label = new Label();
    __ Ldr(PC, target_label);
    __ Emit(target_label);
    __ Emit((Addr) getThumbPC(reinterpret_cast<void *>(targetAddr)));

}

IMPL_RELOCATE(T16, BX_BLX) {

}

IMPL_RELOCATE(T16, CBZ_CBNZ) {

}

IMPL_RELOCATE(T16, LDR_LIT) {

}

IMPL_RELOCATE(T16, ADR) {

}

IMPL_RELOCATE(T32, B32) {

}

IMPL_RELOCATE(T32, LDR_LIT) {

}