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
    __ finish();
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
    }
}

IMPL_RELOCATE(B_BL) {

}

IMPL_RELOCATE(B_COND) {

}

IMPL_RELOCATE(TBZ_TBNZ) {

}

IMPL_RELOCATE(CBZ_CBNZ) {

}

IMPL_RELOCATE(LDR_LIT) {

}

IMPL_RELOCATE(ADR_ADRP) {

}
