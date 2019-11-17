//
// Created by swift on 2019/5/6.
//

#include "inst_arm64.h"
#include "decoder_arm64.h"

using namespace SandHook::Decoder;
using namespace SandHook::AsmA64;

#define CASE(X) \
if (IS_OPCODE_A64(*pc, X)) { \
unit = reinterpret_cast<BaseUnit*>(new INST_A64(X)(pc)); \
goto label_matched; \
}

Arm64Decoder* Arm64Decoder::instant = new Arm64Decoder();

void Arm64Decoder::Disassemble(void *codeStart, Addr codeLen, InstVisitor &visitor,
                               bool onlyPcRelInst) {
    InstA64 *pc = reinterpret_cast<InstA64 *>(codeStart);
    Addr end_addr = (Addr) codeStart + codeLen;
    BaseUnit* unit = nullptr;
    while((Addr) pc < end_addr) {
        // pc relate insts
        CASE(B_BL)
        CASE(B_COND)
        CASE(CBZ_CBNZ)
        CASE(TBZ_TBNZ)
        CASE(LDR_LIT)
        CASE(ADR_ADRP)
        if (onlyPcRelInst)
            goto label_matched;
        CASE(BR_BLR_RET)
        CASE(EXCEPTION_GEN)
        CASE(MSR_MRS)
        CASE(MOV_WIDE)
        CASE(MOV_REG)
        CASE(LDR_IMM)
        CASE(LDR_UIMM)
        CASE(LDRSW_IMM)
        CASE(LDRSW_UIMM)
        CASE(STR_UIMM)
        CASE(STR_IMM)
        CASE(SUB_EXT_REG)
        CASE(STP_LDP)
        CASE(ADD_SUB_IMM)

        label_matched:
        if (unit == nullptr) {
            unit = reinterpret_cast<BaseUnit*>(new INST_A64(UNKNOW)(pc));
        }
        reinterpret_cast<BaseInst*>(unit)->Disassemble();
        if (!visitor.Visit(unit, pc)) {
            break;
        }
        pc = reinterpret_cast<InstA64 *>((Addr)pc + unit->Size());
        unit = nullptr;
    }
}
