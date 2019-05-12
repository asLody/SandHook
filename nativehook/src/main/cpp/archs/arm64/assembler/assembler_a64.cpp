//
// Created by swift on 2019/5/11.
//

#include "assembler_a64.h"

SandHook::Assembler::AssemblerA64::AssemblerA64(CodeBuffer* codeBuffer) {
    codeContainer.setCodeBuffer(codeBuffer);
}

void *SandHook::Assembler::AssemblerA64::finish() {
    codeContainer.commit();
    return reinterpret_cast<void *>(codeContainer.startPc);
}

void
SandHook::Assembler::AssemblerA64::MoveWide(RegisterA64 &rd, INST_A64(MOV_WIDE)::OP op, U64 imme,
                                            INST_A64(MOV_WIDE)::Shift shift) {
    codeContainer.append(reinterpret_cast<Unit<Base> *>(new INST_A64(MOV_WIDE)(op, &rd, imme, shift)));
}

void SandHook::Assembler::AssemblerA64::Mov(WRegister &rd, U32 imme) {
    const U16 h0 = BITS16L(imme);
    const U16 h1 = BITS16H(imme);
    Movz(rd, h0, INST_A64(MOV_WIDE)::Shift0);
    Movk(rd, h1, INST_A64(MOV_WIDE)::Shift1);
}

void SandHook::Assembler::AssemblerA64::Mov(XRegister &rd, U64 imme) {
    const U32 wl = BITS32L(imme);
    const U32 wh = BITS32H(imme);
    const U16 h0 = BITS16L(wl);
    const U16 h1 = BITS16H(wl);
    const U16 h2 = BITS16L(wh);
    const U16 h3 = BITS16H(wh);
    Movz(rd, h0, INST_A64(MOV_WIDE)::Shift0);
    Movk(rd, h1, INST_A64(MOV_WIDE)::Shift1);
    Movk(rd, h2, INST_A64(MOV_WIDE)::Shift2);
    Movk(rd, h3, INST_A64(MOV_WIDE)::Shift3);
}

void SandHook::Assembler::AssemblerA64::Movz(RegisterA64 &rd, U64 imme,
                                             INST_A64(MOV_WIDE)::Shift shift) {
    MoveWide(rd, INST_A64(MOV_WIDE)::MOV_WideOp_Z, imme, shift);
}

void SandHook::Assembler::AssemblerA64::Movk(RegisterA64 &rd, U64 imme,
                                             INST_A64(MOV_WIDE)::Shift shift) {
    MoveWide(rd, INST_A64(MOV_WIDE)::MOV_WideOp_K, imme, shift);
}

void SandHook::Assembler::AssemblerA64::Movn(RegisterA64 &rd, U64 imme,
                                             INST_A64(MOV_WIDE)::Shift shift) {
    MoveWide(rd, INST_A64(MOV_WIDE)::MOV_WideOp_N, imme, shift);
}

void SandHook::Assembler::AssemblerA64::Emit(Unit<Base> &unit) {
    codeContainer.append(&unit);
}

void *SandHook::Assembler::AssemblerA64::getPC() {
    return reinterpret_cast<void *>(codeContainer.curPc);
}

void *SandHook::Assembler::AssemblerA64::getStartPC() {
    return reinterpret_cast<void *>(codeContainer.startPc);
}
