//
// Created by swift on 2019/5/6.
//

#include "inst_arm64.h"
#include "decoder_arm64.h"

using namespace SandHook::Decoder;
using namespace SandHook::AsmA64;

#define CASE(X) \
if (IS_OPCODE_A64(*pc, X)) { \
STRUCT_A64(X) *s = reinterpret_cast<STRUCT_A64(X) *>(pc); \
unit = reinterpret_cast<Unit<Base> *>(new INST_A64(X)(*s)); \
goto label_matched; \
}

void Arm64Decoder::decode(void *codeStart, Addr codeLen, InstVisitor &visitor) {
    InstA64 *pc = reinterpret_cast<InstA64 *>(codeStart);
    Addr endAddr = (Addr) codeStart + codeLen;
    Unit<Base>* unit = nullptr;
    while((Addr) pc < endAddr) {
        CASE(MOV_WIDE)
        CASE(MOV_REG)
        CASE(ADR_ADRP)
        CASE(LDR_LIT)
        CASE(LDR_IMM)
        CASE(LDR_UIMM)
        CASE(LDRSW_IMM)
        CASE(LDRSW_UIMM)
        CASE(STR_UIMM)
        CASE(STR_IMM)
        CASE(B_BL)
        CASE(B_COND)
        CASE(BR_BLR_RET)
        CASE(CBZ_CBNZ)
        CASE(TBZ_TBNZ)
        CASE(SUB_EXT_REG)
        CASE(SVC)
        CASE(EXCEPTION_GEN)
        label_matched:
        if (unit == nullptr) {
            unit = reinterpret_cast<Unit<Base> *>(new A64_UNKNOW(*reinterpret_cast<STRUCT_A64(UNKNOW) *>(pc)));
        }
        if (!visitor.visit(unit, pc)) {
            break;
        }
        pc = reinterpret_cast<InstA64 *>((Addr)pc + unit->size());
        unit = nullptr;
    }
}
