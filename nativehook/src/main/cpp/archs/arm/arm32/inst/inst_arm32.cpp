//
// Created by swift on 2019/5/12.
//

#include "inst_arm32.h"

//// STR IMM
//A64_STR_IMM::A64_STR_IMM() {}
//
//A64_STR_IMM::A64_STR_IMM(STRUCT_A64(STR_IMM) *inst) : InstructionA64(inst) {
//        DisAssembler(inst);
//}
//
//A64_STR_IMM::A64_STR_IMM(RegisterA64 &rt, const MemOperand &operand) : rt(&rt), operand(operand) {}
//
//A64_STR_IMM::A64_STR_IMM(Condition condition, RegisterA64 &rt, const MemOperand &operand)
//        : condition(condition), rt(&rt), operand(operand) {}
//
//AddrMode A64_STR_IMM::decodeAddrMode() {
//    if (Get()->P == 1 && Get()->W == 0) {
//        return Offset;
//    } else if (Get()->P == 0 && Get()->W == 0) {
//        return PostIndex;
//    } else if (Get()->P == 1 && Get()->W == 1) {
//        return PreIndex;
//    } else {
//        valid = false;
//        return NonAddrMode;
//    }
//}
//
//void A64_STR_IMM::DisAssembler(STRUCT_A64(STR_IMM) *inst) {
//    imm32 = zeroExtend32(12, inst->imm12);
//    condition = Condition(inst->cond);
//    operand.addr_mode_ = decodeAddrMode();
//    index = inst->P == 1;
//    wback = inst->P == 1 || inst->W == 0;
//    add = inst->U == 0;
//    rt = XReg(static_cast<U8>(inst->rt));
//    operand.base_ = XReg(static_cast<U8>(inst->rn));
//    operand.offset_ = add ? imm32 : -imm32;
//}
//
//
//void A64_STR_IMM::Assemble() {
//    INST_DCHECK(condition, Condition::nv)
//
//}