//
// Created by swift on 2019/5/16.
//

#include "inst_t16.h"
#include "arm32_base.h"
#include "register_list_arm32.h"

#define SET_BASE_OPCODE(X) Get()->opcode_base = OPCODE_T16(X)

#define SET_OPCODE(X) Get()->opcode = OPCODE_T16(X)
#define SET_OPCODE_MULTI(X, INDEX) Get()->opcode##INDEX = OPCODE_T16(X##_##INDEX)

using namespace SandHook::AsmA32;
using namespace SandHook::RegistersA32;


//Unknow

T16_UNKNOW::T16_UNKNOW(void *inst) : InstructionT16(inst) {
}

//B

T16_B::T16_B(void *inst) : T16_INST_PC_REL(inst) {
}

T16_B::T16_B(Off offset) : offset(offset) {}

T16_B::T16_B(Label *label) {
    BindLabel(label);
}

Off T16_B::GetImmPCOffset() {
    return DECODE_OFFSET(11, 1);
}

void T16_B::Disassemble() {
    offset = GetImmPCOffset();
}

void T16_B::Assemble() {
    SET_OPCODE(B);
    DECODE_OFFSET(11, 1);
}

void T16_B::OnOffsetApply(Off offset) {
    this->offset = offset;
    DECODE_OFFSET(11, 1);
}



//B Cond

T16_B_COND::T16_B_COND(void *inst) : T16_INST_PC_REL(inst) {
}

T16_B_COND::T16_B_COND(Condition condition, Off offset) : condition(condition), offset(offset) {}

T16_B_COND::T16_B_COND(Condition condition, Label *label) {
    BindLabel(label);
}

void T16_B_COND::Disassemble() {
    DECODE_COND;
    offset = GetImmPCOffset();
}

void T16_B_COND::Assemble() {
    SET_OPCODE(B_COND);
    ENCODE_COND;
    ENCODE_OFFSET(8, 1);
}

void T16_B_COND::OnOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(8, 1);
}

Off T16_B_COND::GetImmPCOffset() {
    return DECODE_OFFSET(8, 1);
}


//BX BLX

T16_BX_BLX::T16_BX_BLX(void *inst) : T16_INST_PC_REL(inst) {}

T16_BX_BLX::T16_BX_BLX(T16_BX_BLX::OP op, RegisterA32 &rm) : op(op), rm(&rm) {}

void T16_BX_BLX::Disassemble() {
    DECODE_OP;
    DECODE_RM(Reg);
}

void T16_BX_BLX::Assemble() {
    SET_OPCODE_MULTI(BX_BLX, 1);
    SET_OPCODE_MULTI(BX_BLX, 2);
    ENCODE_OP;
    ENCODE_RM;
}


//CBZ CBNZ
T16_CBZ_CBNZ::T16_CBZ_CBNZ(void *inst) : T16_INST_PC_REL(inst) {}

T16_CBZ_CBNZ::T16_CBZ_CBNZ(T16_CBZ_CBNZ::OP op, Off offset, RegisterA32 &rn) : op(op), offset(offset),
                                                                              rn(&rn) {}
                                                                              
T16_CBZ_CBNZ::T16_CBZ_CBNZ(T16_CBZ_CBNZ::OP op, Label* label, RegisterA32 &rn) : op(op), rn(&rn) {
    BindLabel(label);
}

Off T16_CBZ_CBNZ::GetImmPCOffset() {
    return COMBINE(Get()->i, Get()->imm5, 5) << 2;
}

void T16_CBZ_CBNZ::Disassemble() {
    offset = GetImmPCOffset();
    DECODE_RN(Reg);
    DECODE_OP;
}

void T16_CBZ_CBNZ::Assemble() {
    SET_OPCODE_MULTI(CBZ_CBNZ, 1);
    SET_OPCODE_MULTI(CBZ_CBNZ, 2);
    SET_OPCODE_MULTI(CBZ_CBNZ, 3);
    ENCODE_OP;
    ENCODE_RN;
    ENCODE_OFFSET(5, 2);
}

void T16_CBZ_CBNZ::OnOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(5, 2);
}


//LDR_LIT
T16_LDR_LIT::T16_LDR_LIT(void *inst) : T16_INST_PC_REL(inst) {}

T16_LDR_LIT::T16_LDR_LIT(Off offset, RegisterA32 &rt) : offset(offset), rt(&rt) {}

Off T16_LDR_LIT::GetImmPCOffset() {
    return Get()->imm8 << 2;
}

Addr T16_LDR_LIT::GetImmPCOffsetTarget() {
    return ALIGN((Addr) GetPC() + offset, 4);
}

void T16_LDR_LIT::OnOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(8, 2);
}

void T16_LDR_LIT::Disassemble() {
    DECODE_RT(Reg);
    offset = GetImmPCOffset();
}

void T16_LDR_LIT::Assemble() {
    SET_OPCODE(LDR_LIT);
    ENCODE_RT;
    ENCODE_OFFSET(8, 2);
}


//ADD IMM RD = RN
T16_ADD_IMM_RDN::T16_ADD_IMM_RDN(void *inst) : InstructionT16(inst) {}


T16_ADD_IMM_RDN::T16_ADD_IMM_RDN(RegisterA32 *rdn, U8 imm8) : rdn(rdn), imm8(imm8) {}


void T16_ADD_IMM_RDN::Disassemble() {
    rdn = Reg(Get()->rdn);
    imm8 = Get()->imm8;
}

void T16_ADD_IMM_RDN::Assemble() {
    SET_OPCODE(ADD_IMM_RDN);
    Get()->imm8 = imm8;
    Get()->rdn = rdn->Code();
}




//ADR
T16_ADR::T16_ADR(void *inst) : T16_INST_PC_REL(inst) {}

T16_ADR::T16_ADR(RegisterA32 &rd, Off offset) : rd(&rd), offset(offset) {}

T16_ADR::T16_ADR(RegisterA32 &rd, Label *label) : rd(&rd) {
    BindLabel(label);
}


Off T16_ADR::GetImmPCOffset() {
    return COMBINE(Get()->imm8, 0 ,2);
}

Addr T16_ADR::GetImmPCOffsetTarget() {
    return RoundDown((Addr) GetPC() + offset, 4);
}

void T16_ADR::OnOffsetApply(Off offset) {
    this->offset = offset;
    Get()->imm8 = (U32)offset >> 2;
}

void T16_ADR::Disassemble() {
    offset = GetImmPCOffset();
    DECODE_RD(Reg);
}


void T16_ADR::Assemble() {
    SET_OPCODE(ADR);
    ENCODE_RD;
    Get()->imm8 = (U32)offset >> 2;
}


T16_CMP_REG::T16_CMP_REG(void *inst) : InstructionT16(inst) {}

T16_CMP_REG::T16_CMP_REG(RegisterA32 &rm, RegisterA32 &rn) : rm(&rm), rn(&rn) {}

void T16_CMP_REG::Disassemble() {
    DECODE_RM(Reg);
    DECODE_RN(Reg);
}

void T16_CMP_REG::Assemble() {
    SET_BASE_OPCODE(DATA_PROC);
    SET_OPCODE(CMP_REG);
    ENCODE_RM;
    ENCODE_RN;
}


//MOV REG
T16_MOV_REG::T16_MOV_REG(void *inst) : InstructionT16(inst) {}

T16_MOV_REG::T16_MOV_REG(RegisterA32 &rd, RegisterA32 &rm) : rm(&rm), rd(&rd) {}

void T16_MOV_REG::Disassemble() {
    DECODE_RM(Reg);
    rd = Reg(static_cast<U8>(COMBINE(Get()->D, Get()->rd, 3)));
}

void T16_MOV_REG::Assemble() {
    SET_BASE_OPCODE(SPDIABE);
    SET_OPCODE(MOV_REG);
    ENCODE_RM;
    Get()->rd = BITS(rd->Code(), 0, 2);
    Get()->D = BIT(rd->Code(), 3);
}

bool T16_MOV_REG::PcRelate() {
    return rd == &PC || rm == &PC;
}




T16_ADD_REG::T16_ADD_REG(void *inst) : InstructionT16(inst) {}

T16_ADD_REG::T16_ADD_REG(RegisterA32 *rd, RegisterA32 *rn, RegisterA32 *rm) : rd(rd), rn(rn),
                                                                              rm(rm) {}

void T16_ADD_REG::Disassemble() {
    DECODE_RD(Reg);
    DECODE_RN(Reg);
    DECODE_RM(Reg);
}

void T16_ADD_REG::Assemble() {
    SET_OPCODE(ADD_REG);
    INST_ASSERT(rd->Code() > 7);
    INST_ASSERT(rn->Code() > 7);
    INST_ASSERT(rm->Code() > 7);
    ENCODE_RD;
    ENCODE_RN;
    ENCODE_RM;
}



T16_CMP_REG_EXT::T16_CMP_REG_EXT(void *inst) : InstructionT16(inst) {}

T16_CMP_REG_EXT::T16_CMP_REG_EXT(RegisterA32 &rn, RegisterA32 &rm) : rn(&rn), rm(&rm) {}

void T16_CMP_REG_EXT::Disassemble() {
    rn = Reg(COMBINE(Get()->N, Get()->rn, 3));
    DECODE_RM(Reg);
}

void T16_CMP_REG_EXT::Assemble() {
    SET_OPCODE(CMP_REG_EXT);
    ENCODE_RM;
    Get()->rn = BITS(rn->Code(), 0, 2);
    Get()->N = BIT(rn->Code(), 3);
}


//POP
T16_POP::T16_POP(void *inst) : InstructionT16(inst) {}

T16_POP::T16_POP(const RegisterList &registerList) : registerList(registerList) {}

void T16_POP::Disassemble() {
    registerList.SetList(COMBINE(Get()->P << 7, Get()->regs, 8));
}

void T16_POP::Assemble() {
    SET_OPCODE(POP);
    U16 regs = registerList.GetList();
    Get()->regs = BITS(regs, 0, 7);
    Get()->P = BIT(regs, 15);
}


//PUSH
T16_PUSH::T16_PUSH(void *inst) : InstructionT16(inst) {}

T16_PUSH::T16_PUSH(const RegisterList &registerList) : registerList(registerList) {}

void T16_PUSH::Disassemble() {
    registerList.SetList(COMBINE(Get()->M << 6, Get()->regs, 8));
}

void T16_PUSH::Assemble() {
    SET_OPCODE(PUSH);
    U16 regs = registerList.GetList();
    Get()->regs = BITS(regs, 0, 7);
    Get()->M = BIT(regs, 14);
}


// Add reg_ rdn T2
T16_ADD_REG_RDN::T16_ADD_REG_RDN(void *inst) : InstructionT16(inst) {}

T16_ADD_REG_RDN::T16_ADD_REG_RDN(RegisterA32 &rdn, RegisterA32 &rm) : rdn(&rdn), rm(&rm) {}

bool T16_ADD_REG_RDN::PcRelate() {
    return *rm == PC;
}

void T16_ADD_REG_RDN::Disassemble() {
    DECODE_RM(Reg);
    rdn = Reg(Get()->rdn);
}

void T16_ADD_REG_RDN::Assemble() {
    SET_OPCODE(ADD_REG_RDN);
    ENCODE_RM;
    Get()->rdn = rdn->Code();
}
