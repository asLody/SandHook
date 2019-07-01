//
// Created by swift on 2019/5/23.
//

#include "code_relocate_arm32.h"
#include "decoder.h"
#include "lock.h"

using namespace SandHook::RegistersA32;
using namespace SandHook::AsmA32;
using namespace SandHook::Utils;
using namespace SandHook::Decoder;

#define __ assemblerA32->

#define IMPL_RELOCATE(T, X) void CodeRelocateA32::relocate_##T##_##X (INST_##T(X)* inst, void* toPc) throw(ErrorCodeException)

#define CASE(T, X) \
case ENUM_VALUE(InstCode##T, InstCode##T::X): \
relocate_##T##_##X(reinterpret_cast<INST_##T(X)*>(instruction), toPc); \
break;

CodeRelocateA32::CodeRelocateA32(AssemblerA32 &assembler) : CodeRelocate(assembler.codeContainer) {
    this->assemblerA32 = &assembler;
}

bool CodeRelocateA32::Visit(BaseUnit *unit, void *pc) {
    relocate(reinterpret_cast<BaseInst *>(unit), __ getPC());
    curOffset += unit->Size();
    if (unit->RefCount() == 0) {
        delete unit;
    }
    return true;
}

void* CodeRelocateA32::relocate(void *startPc, Addr len, void *toPc = nullptr) throw(ErrorCodeException) {
    AutoLock autoLock(relocateLock);
    startAddr = reinterpret_cast<Addr>(startPc);
    if (IsThumbCode(startAddr)) {
        startAddr = reinterpret_cast<Addr>(GetThumbCodeAddress(startPc));
    }
    length = len;
    curOffset = 0;
    __ allocBufferFirst(static_cast<U32>(len * 8));
    void* curPc = __ getPC();
    if (toPc == nullptr) {
        Disassembler::get()->Disassembler(startPc, len, *this, true);
    } else {
        //TODO
    }
    return curPc;
}

void* CodeRelocateA32::relocate(BaseInst *instruction, void *toPc) throw(ErrorCodeException) {
    void* curPc = __ getPC();

    //insert later AddBind labels
    __ Emit(getLaterBindLabel(curOffset));

    if (!instruction->PcRelate()) {
        __ Emit(instruction);
        instruction->Ref();
        return curPc;
    }

    if (instruction->InstType() == thumb32) {
        switch (instruction->InstCode()) {
            CASE(T32, B32)
            CASE(T32, LDR_LIT)
            default:
                __ Emit(instruction);
                instruction->Ref();
        }
    } else if (instruction->InstType() == thumb16) {
        switch (instruction->InstCode()) {
            CASE(T16, B)
            CASE(T16, B_COND)
            CASE(T16, BX_BLX)
            CASE(T16, CBZ_CBNZ)
            CASE(T16, ADR)
            CASE(T16, LDR_LIT)
            CASE(T16, ADD_REG_RDN)
            default:
                __ Emit(instruction);
                instruction->Ref();
        }
    } else {
        __ Emit(instruction);
        instruction->Ref();
    }
    return curPc;
}


IMPL_RELOCATE(T16, B_COND) {

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(InstT16))) {
        __ B(inst->condition, getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        return;
    }

    Addr targetAddr = inst->GetImmPCOffsetTarget();

    if (inst->condition == al) {
        Label* target_label = new Label;
        ALIGN_FOR_LDR
        __ Ldr(PC, target_label);
        __ Emit(target_label);
        __ Emit(targetAddr);
    } else {
        Label* true_label = new Label();
        Label* false_label = new Label();
        Label* target_label = new Label;
        __ B(inst->condition, true_label);
        __ B(false_label);
        __ Emit(true_label);
        ALIGN_FOR_LDR
        __ Ldr(PC, target_label);
        __ Emit(target_label);
        __ Emit(targetAddr);
        __ Emit(false_label);
    }

}

IMPL_RELOCATE(T16, B) {

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(InstT16))) {
        __ B(getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        return;
    }

    Addr targetAddr = inst->GetImmPCOffsetTarget();

    Label* target_label = new Label();
    ALIGN_FOR_LDR
    __ Ldr(PC, target_label);
    __ Emit(target_label);
    __ Emit((Addr) GetThumbPC(reinterpret_cast<void *>(targetAddr)));

}

IMPL_RELOCATE(T16, BX_BLX) {
    __ Emit(reinterpret_cast<BaseInst*>(inst));
    inst->Ref();
}

IMPL_RELOCATE(T16, CBZ_CBNZ) {

    inst->Ref();

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(InstT16))) {
        inst->BindLabel(getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    Addr targetAddr = inst->GetImmPCOffsetTarget();

    Label* true_label = new Label;
    Label* false_label = new Label;
    Label* target_label = new Label();

    inst->BindLabel(true_label);
    __ Emit(reinterpret_cast<BaseInst*>(inst));
    __ B(false_label);
    __ Emit(true_label);
    ALIGN_FOR_LDR
    __ Ldr(PC, target_label);
    __ Emit(target_label);
    __ Emit((Addr) GetThumbPC(reinterpret_cast<void *>(targetAddr)));
    __ Emit(false_label);

}

IMPL_RELOCATE(T16, LDR_LIT) {

    if (inRelocateRange(CODE_OFFSET(inst), inst->rt->Wide())) {
        inst->Ref();
        inst->BindLabel(getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    Addr targetAddr = inst->GetImmPCOffsetTarget();

    __ Mov(*inst->rt, targetAddr);
    __ Ldr(*inst->rt, MemOperand(inst->rt, 0));
}

IMPL_RELOCATE(T16, ADR) {

    if (inRelocateRange(CODE_OFFSET(inst), inst->rd->Wide())) {
        inst->Ref();
        inst->BindLabel(getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    Addr targetAddr = inst->GetImmPCOffsetTarget();

    __ Mov(*inst->rd, targetAddr);

}

IMPL_RELOCATE(T16, ADD_REG_RDN) {

    if (*inst->rm != PC) {
        inst->Ref();
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    RegisterA32& tmpReg = *inst->rdn != R0 ? R0 : R1;

    __ Push(tmpReg);
    __ Mov(tmpReg,(Addr)inst->GetPC());
    __ Add(*inst->rdn, *inst->rdn, tmpReg);
    __ Pop(tmpReg);

}

IMPL_RELOCATE(T32, B32) {

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(InstT16))) {
        inst->Ref();
        inst->BindLabel(getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<BaseInst *>(inst));
        return;
    }

    Addr targetAddr = inst->GetImmPCOffsetTarget();


    if (inst->x == T32_B32::thumb) {
        //Thumb mode
        targetAddr = reinterpret_cast<Addr>(GetThumbPC(reinterpret_cast<void *>(targetAddr)));
    }
    __ Mov(IP, targetAddr);
    if (inst->op == T32_B32::BL) {
        __ Blx(IP);
    } else {
        __ Bx(IP);
    }

}

IMPL_RELOCATE(T32, LDR_LIT) {

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(Addr))) {
        inst->Ref();
        inst->BindLabel(getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    Addr targetAddr = inst->GetImmPCOffsetTarget();

    __ Mov(*inst->rt, targetAddr);
    switch (inst->op) {
        case T32_LDR_LIT::LDR:
            __ Ldr(*inst->rt, MemOperand(inst->rt, 0));
            break;
        case T32_LDR_LIT::LDRB:
            if (inst->s == T32_LDR_LIT::UnSign) {
                __ Ldrb(*inst->rt, MemOperand(inst->rt, 0));
            } else {
                __ Ldrsb(*inst->rt, MemOperand(inst->rt, 0));
            }
            break;
        case T32_LDR_LIT::LDRH:
            if (inst->s == T32_LDR_LIT::UnSign) {
                __ Ldrh(*inst->rt, MemOperand(inst->rt, 0));
            } else {
                __ Ldrsh(*inst->rt, MemOperand(inst->rt, 0));
            }
            break;
        default:
            inst->Ref();
            __ Emit(reinterpret_cast<BaseInst*>(inst));
    }

}