//
// Created by swift on 2019/5/12.
//

#include "inst_arm32.h"

//// STR IMM
//A64_STR_IMM::A64_STR_IMM() {}
//
//A64_STR_IMM::A64_STR_IMM(STRUCT_A64(STR_IMM) *inst) : InstructionA64(inst) {
//        decode(inst);
//}
//
//A64_STR_IMM::A64_STR_IMM(RegisterA64 &rt, const MemOperand &operand) : rt(&rt), operand(operand) {}
//
//A64_STR_IMM::A64_STR_IMM(Condition condition, RegisterA64 &rt, const MemOperand &operand)
//        : condition(condition), rt(&rt), operand(operand) {}
//
//AddrMode A64_STR_IMM::decodeAddrMode() {
//    if (get()->P == 1 && get()->W == 0) {
//        return Offset;
//    } else if (get()->P == 0 && get()->W == 0) {
//        return PostIndex;
//    } else if (get()->P == 1 && get()->W == 1) {
//        return PreIndex;
//    } else {
//        valid = false;
//        return NonAddrMode;
//    }
//}
//
//void A64_STR_IMM::decode(STRUCT_A64(STR_IMM) *inst) {
//    imm32 = zeroExtend32(12, inst->imm12);
//    condition = Condition(inst->cond);
//    operand.addr_mode = decodeAddrMode();
//    index = inst->P == 1;
//    wback = inst->P == 1 || inst->W == 0;
//    add = inst->U == 0;
//    rt = XReg(static_cast<U8>(inst->rt));
//    operand.base = XReg(static_cast<U8>(inst->rn));
//    operand.offset = add ? imm32 : -imm32;
//}
//
//
//void A64_STR_IMM::assembler() {
//    INST_DCHECK(condition, Condition::nv)
//
//}