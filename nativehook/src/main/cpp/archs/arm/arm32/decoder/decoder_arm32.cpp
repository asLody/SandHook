//
// Created by swift on 2019/5/23.
//

#include "inst_t16.h"
#include "inst_t32.h"
#include "decoder_arm32.h"

using namespace SandHook::Decoder;
using namespace SandHook::AsmA32;

#define CASE(T, X) \
if (IS_OPCODE_##T(*reinterpret_cast<Inst##T *>(pc), X)) { \
STRUCT_##T(X) *s = reinterpret_cast<STRUCT_##T(X) *>(pc); \
unit = reinterpret_cast<Unit<Base> *>(new INST_##T(X)(s)); \
goto label_matched; \
}

#define CASE_A32(X) CASE(A32, X)
#define CASE_T16(X) CASE(T16, X)
#define CASE_T32(X) CASE(T32, X)


Arm32Decoder* Arm32Decoder::instant = new Arm32Decoder();

void Arm32Decoder::decode(void *codeStart, Addr codeLen, InstVisitor &visitor) {
    bool thumb = isThumbCode(reinterpret_cast<Addr>(codeStart));
    if (thumb) {
        codeStart = getThumbCodeAddress(codeStart);
    }
    void *pc = codeStart;
    Addr endAddr = (Addr) codeStart + codeLen;
    Unit<Base>* unit = nullptr;
    while((Addr) pc < endAddr) {
        bool thumb32 = isThumb32(*reinterpret_cast<InstT16*>(pc));
        if (thumb && thumb32) {
            CASE_T32(B32)
            CASE_T32(LDR_LIT)
            CASE_T32(LDR_IMM)
            CASE_T32(LDR_UIMM)
            CASE_T32(MOV_MOVT_IMM)
            if (unit == nullptr) {
                unit = reinterpret_cast<Unit<Base> *>(new INST_T32(UNKNOW)(*reinterpret_cast<STRUCT_T32(UNKNOW) *>(pc)));
            }
        } else if (thumb) {
            CASE_T16(B)
            CASE_T16(B_COND)
            CASE_T16(BX_BLX)
            CASE_T16(CBZ_CBNZ)
            CASE_T16(LDR_LIT)
            CASE_T16(ADD_IMM_RDN)
            CASE_T16(ADR)
            CASE_T16(ADD_REG)
            CASE_T16(CMP_REG)
            CASE_T16(CMP_REG_EXT)
            CASE_T16(MOV_REG)
            CASE_T16(POP)
            CASE_T16(PUSH)
            if (unit == nullptr) {
                unit = reinterpret_cast<Unit<Base> *>(new INST_T16(UNKNOW)(*reinterpret_cast<STRUCT_T16(UNKNOW) *>(pc)));
            }
        } else {
            //TODO
            unit = reinterpret_cast<Unit<Base> *>(new INST_T32(UNKNOW)(*reinterpret_cast<STRUCT_T32(UNKNOW) *>(pc)));
        }

        label_matched:
        if (!visitor.visit(unit, pc)) {
            break;
        }
        pc = reinterpret_cast<InstA64 *>((Addr)pc + unit->size());
        unit = nullptr;
    }
}

