//
// Created by swift on 2019/5/12.
//

#ifndef SANDHOOK_NH_INST_ARM32_H
#define SANDHOOK_NH_INST_ARM32_H

namespace SandHook {

    namespace Asm {
//        class INST_A64(STR_IMM) : public InstructionA64<STRUCT_A64(STR_IMM)> {
//        public:
//        A64_STR_IMM();
//
//        A64_STR_IMM(STRUCT_A64(STR_IMM) *inst);
//
//        A64_STR_IMM(RegisterA64 &rt, const MemOperand &operand);
//
//        A64_STR_IMM(Condition condition, RegisterA64 &rt, const MemOperand &operand);
//
//        DEFINE_IS_EXT(STR_IMM, TEST_INST_FIELD(opcode, OPCODE_A64(STR_IMM)) && TEST_INST_FIELD(unkown1_0, 0) && TEST_INST_FIELD(unkown2_0, 0))
//
//        inline U32 instCode() override {
//        return STR_x;
//    }
//
//    void decode(STRUCT_A64(STR_IMM) *inst) override;
//
//    void assembler() override;
//
//    AddrMode getAddrMode() {
//        return operand.addr_mode;
//    }
//
//    private:
//    AddrMode decodeAddrMode();
//
//    public:
//    Condition condition = Condition::al;
//    RegisterA64* rt;
//    MemOperand operand = MemOperand(nullptr);
//    private:
//    bool wback;
//    U32 imm32;
//    bool add;
//    bool index;
//};
    }
}

#endif //SANDHOOK_NH_INST_ARM32_H
