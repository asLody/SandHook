//
// Created by swift on 2019/5/12.
//

#include "code_relocate_a64.h"

#define __ assemblerA64->

CodeRelocateA64::CodeRelocateA64(AssemblerA64 &assembler) : CodeRelocate(assembler.codeContainer) {
    this->assemblerA64 = &assembler;
}

bool CodeRelocateA64::relocate(Instruction<Base> *instruction, void *toPc) throw(ErrorCodeException) {
    __ finish();
    return false;
}

bool CodeRelocateA64::relocate(void *startPc, void *toPc, Addr len) throw(ErrorCodeException) {
    return false;
}
