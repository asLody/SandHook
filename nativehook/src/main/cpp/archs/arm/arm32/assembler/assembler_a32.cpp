//
// Created by swift on 2019/5/22.
//

#include "assembler_a32.h"

using namespace SandHook::Assembler;
using namespace SandHook::RegistersA32;
using namespace SandHook::AsmA32;

AssemblerA32::AssemblerA32(CodeBuffer* codeBuffer) {
    codeContainer.setCodeBuffer(codeBuffer);
}

void *AssemblerA32::getPC() {
    return reinterpret_cast<void *>(codeContainer.curPc);
}

void *AssemblerA32::getStartPC() {
    return reinterpret_cast<void *>(codeContainer.startPc);
}

void AssemblerA32::allocBufferFirst(U32 size) {
    codeContainer.allocBufferFirst(size);
}

void *AssemblerA32::finish() {
    codeContainer.commit();
    return reinterpret_cast<void *>(codeContainer.startPc);
}

void AssemblerA32::Emit(U32 data32) {
    Emit(reinterpret_cast<Unit<Base>*>(new Data32(data32)));
}

void AssemblerA32::Emit(Unit<Base> *unit) {
    codeContainer.append(unit);
}

void AssemblerA32::Mov(RegisterA32 &rd, U16 imm16) {
    Emit(reinterpret_cast<Unit<Base>*>(new INST_T32(MOV_MOVT_IMM)(INST_T32(MOV_MOVT_IMM)::MOV, &rd, imm16)));
}

void AssemblerA32::Movt(RegisterA32 &rd, U16 imm16) {
    Emit(reinterpret_cast<Unit<Base>*>(new INST_T32(MOV_MOVT_IMM)(INST_T32(MOV_MOVT_IMM)::MOVT, &rd, imm16)));
}

void AssemblerA32::Mov(RegisterA32 &rd, U32 imm32) {
    U16 immH = BITS16H(imm32);
    U16 immL = BITS16L(imm32);
    Mov(rd, immL);
    Movt(rd, immH);
}
