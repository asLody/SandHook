//
// Created by swift on 2019/5/6.
//

#include "inst_arm64.h"

using namespace SandHook::Asm;


//PC Rel Inst


A64_INST_PC_REL::A64_INST_PC_REL() {}

A64_INST_PC_REL::A64_INST_PC_REL(aarch64_pcrel_insts *inst) : InstructionA64(inst) {}

int A64_INST_PC_REL::getImmPCRel() {
    U32 hi = static_cast<U32>(get()->immhi);
    U32 lo = get()->immlo;
    U32 offset = (hi << IMM_LO_W) | lo;
    int width = IMM_HI_W + IMM_LO_W;
    return ExtractSignedBitfield32(width - 1, 0, offset);
}

ADDR A64_INST_PC_REL::getImmPCOffset() {
    return static_cast<ADDR>(getImmBranch() * size());
}

ADDR A64_INST_PC_REL::getImmPCOffsetTarget() {
    return reinterpret_cast<ADDR>(getImmPCOffset() + (ADDR) getPC());
}


//ADR ADRP

A64_ADR_ADRP::A64_ADR_ADRP() {}

A64_ADR_ADRP::A64_ADR_ADRP(aarch64_pcrel_insts *inst) : A64_INST_PC_REL(inst) {}


ADDR A64_ADR_ADRP::getImmPCOffset() {
    ADDR offset = static_cast<ADDR>(getImmPCRel());
    if (isADRP()) {
        offset *= PAGE_SIZE;
    }
    return offset;
}

ADDR A64_ADR_ADRP::getImmPCOffsetTarget() {
    Instruction* base = AlignDown(this, PAGE_SIZE);
    return reinterpret_cast<ADDR>(getImmPCOffset() + (ADDR) base);
}

int A64_ADR_ADRP::getImm()  {
    return getImmPCRel();
}

