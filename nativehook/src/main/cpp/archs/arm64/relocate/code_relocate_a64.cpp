//
// Created by swift on 2019/5/12.
//

#include "code_relocate_a64.h"

#define __ assemblerA64->

#define IMPL_RELOCATE(X) void CodeRelocateA64::relocate_##X (INST_A64(X)* inst, void* toPc) throw(ErrorCodeException)

#define CASE(X) \
case InstCodeA64::X: \
relocate_##X(reinterpret_cast<INST_A64(X)*>(instruction), toPc); \
break;

CodeRelocateA64::CodeRelocateA64(AssemblerA64 &assembler) : CodeRelocate(assembler.codeContainer) {
    this->assemblerA64 = &assembler;
}

void CodeRelocateA64::relocate(void *startPc, void *toPc, Addr len) throw(ErrorCodeException) {

}

void CodeRelocateA64::relocate(Instruction<Base> *instruction, void *toPc) throw(ErrorCodeException) {
    if (!instruction->pcRelate()) {
        __ Emit(instruction);
        return;
    }
    switch (instruction->instCode()) {
        CASE(B_BL)
        CASE(B_COND)
        CASE(TBZ_TBNZ)
        CASE(CBZ_CBNZ)
        CASE(LDR_LIT)
        CASE(ADR_ADRP)
        default:
            __ Emit(instruction);
    }
}

IMPL_RELOCATE(B_BL) {
    Addr targetAddr = inst->getImmPCOffsetTarget();
    if (inst->op == inst->BL) {
        __ Mov(LR, targetAddr + inst->size());
    }
    __ Mov(IP1, targetAddr);
    __ Br(IP1);
}

IMPL_RELOCATE(B_COND) {

    Addr targetAddr = inst->getImmPCOffsetTarget();

    Label *true_label = new Label();
    Label *false_label = new Label();

    __ B(inst->condition, true_label);
    __ B(false_label);

    __ Emit(true_label);
    __ Mov(IP1, targetAddr);
    __ Br(IP1);

    __ Emit(false_label);
}

IMPL_RELOCATE(TBZ_TBNZ) {

    Addr targetAddr = inst->getImmPCOffsetTarget();

    Label *true_label = new Label();
    Label *false_label = new Label();

    if (inst->op == INST_A64(TBZ_TBNZ)::TBNZ) {
        __ Tbnz(*inst->rt, inst->bit, true_label);
    } else {
        __ Tbz(*inst->rt, inst->bit, true_label);
    }
    __ B(false_label);

    __ Emit(true_label);
    __ Mov(IP1, targetAddr);
    __ Br(IP1);

    __ Emit(false_label);
}

IMPL_RELOCATE(CBZ_CBNZ) {
    Addr targetAddr = inst->getImmPCOffsetTarget();

    Label *true_label = new Label();
    Label *false_label = new Label();

    if (inst->op == INST_A64(CBZ_CBNZ)::CBNZ) {
        __ Cbnz(*inst->rt, true_label);
    } else {
        __ Cbz(*inst->rt, true_label);
    }

    __ B(false_label);

    __ Emit(true_label);
    __ Mov(IP1, targetAddr);
    __ Br(IP1);

    __ Emit(false_label);
}

IMPL_RELOCATE(LDR_LIT) {

}

IMPL_RELOCATE(ADR_ADRP) {
    __ Mov(*inst->rd, inst->getImmPCOffsetTarget());
}
