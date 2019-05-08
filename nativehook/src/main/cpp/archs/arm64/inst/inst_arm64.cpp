//
// Created by swift on 2019/5/6.
//

#include "inst_arm64.h"
#include "../register/register_a64.h"

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

A64_ADR_ADRP::A64_ADR_ADRP(aarch64_pcrel_insts *inst) : A64_INST_PC_REL(inst) {
    decode(inst);
}


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

A64_ADR_ADRP::A64_ADR_ADRP(A64_ADR_ADRP::OP op, RegisterA64 *rd, int imme) : op(op), rd(rd),
                                                                             imme(imme) {
    assembler();
}

void A64_ADR_ADRP::decode(aarch64_pcrel_insts *decode) {

}

void A64_ADR_ADRP::assembler() {

}


//Mov Wide

A64_MOV_WIDE::A64_MOV_WIDE() {}

A64_MOV_WIDE::A64_MOV_WIDE(aarch64_mov_wide *inst) : InstructionA64(inst) {
    decode(inst);
}

A64_MOV_WIDE::A64_MOV_WIDE(A64_MOV_WIDE::OP op,RegisterA64* rd, U16 imme, U8 shift)
        : shift(shift), op(op), imme(imme), rd(rd) {
    assembler();
}

void A64_MOV_WIDE::assembler() {
    get()->opcode = MOV_WIDE_OPCODE;
    get()->imm16 = imme;
    get()->hw = static_cast<InstA64>(shift / 16);
    get()->opc = op;
    get()->sf = rd->is64Bit() ? 1 : 0;
    get()->rd = rd->getCode();
}

void A64_MOV_WIDE::decode(aarch64_mov_wide *inst) {
    imme = static_cast<U16>(inst->imm16);
    shift = static_cast<U8>(inst->hw * 16);
    op = static_cast<OP>(inst->opc);
    if (inst->sf == 0) {
        rd = XRegister::get(static_cast<U8>(inst->rd));
    } else {
        rd = WRegister::get(static_cast<U8>(inst->rd));
    }
}



//B BL

A64_B_BL::A64_B_BL() {}

A64_B_BL::A64_B_BL(aarch64_b_bl *inst) : InstructionA64(inst) {}
