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
unit = reinterpret_cast<BaseUnit*>(new INST_##T(X)(pc)); \
goto label_matched; \
}

#define CASE_A32(X) CASE(A32, X)
#define CASE_T16(X) CASE(T16, X)
#define CASE_T32(X) CASE(T32, X)


Arm32Decoder* Arm32Decoder::instant = new Arm32Decoder();

void Arm32Decoder::Disassemble(void *codeStart, Addr codeLen, InstVisitor &visitor,
                               bool onlyPcRelInst) {
    bool thumb = IsThumbCode(reinterpret_cast<Addr>(codeStart));
    if (thumb) {
        codeStart = GetThumbCodeAddress(codeStart);
    }
    void *pc = codeStart;
    Addr endAddr = (Addr) codeStart + codeLen;
    BaseUnit *unit = nullptr;
    while((Addr) pc < endAddr) {
        bool thumb32 = IsThumb32(*reinterpret_cast<InstT16*>(pc));
        if (thumb && thumb32) {
            CASE_T32(SUB_IMM)
            CASE_T32(B32)
            CASE_T32(LDR_LIT)
            if (!onlyPcRelInst) {
                CASE_T32(LDR_IMM)
                CASE_T32(LDR_UIMM)
                CASE_T32(MOV_MOVT_IMM)
            }
            if (unit == nullptr) {
                unit = reinterpret_cast<BaseUnit*>(new INST_T32(UNKNOW)(pc));
            }
        } else if (thumb) {
            CASE_T16(B)
            CASE_T16(B_COND)
            CASE_T16(BX_BLX)
            CASE_T16(CBZ_CBNZ)
            CASE_T16(LDR_LIT)
            CASE_T16(ADR)
            CASE_T16(ADD_REG_RDN)
            if (!onlyPcRelInst) {
                CASE_T16(ADD_REG)
                CASE_T16(CMP_REG)
                CASE_T16(ADD_IMM_RDN)
                CASE_T16(CMP_REG_EXT)
                CASE_T16(MOV_REG)
                CASE_T16(POP)
                CASE_T16(PUSH)
            }
            if (unit == nullptr) {
                unit = reinterpret_cast<BaseUnit*>(new INST_T16(UNKNOW)(pc));
            }
        } else {
            //TODO arm32 support
            unit = reinterpret_cast<BaseUnit*>(new INST_T32(UNKNOW)(pc));
        }

        label_matched:
        reinterpret_cast<BaseInst*>(unit)->Disassemble();
        if (!visitor.Visit(unit, pc)) {
            break;
        }
        pc = reinterpret_cast<void*>((Addr)pc + unit->Size());
        unit = nullptr;
    }
}

