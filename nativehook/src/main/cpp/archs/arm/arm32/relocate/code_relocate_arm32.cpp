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

bool CodeRelocateA32::visit(Unit<Base> *unit, void *pc) {
    relocate(reinterpret_cast<Instruction<Base> *>(unit), __ getPC());
    curOffset += unit->size();
    if (unit->refcount() == 0) {
        delete unit;
    }
    return true;
}

void* CodeRelocateA32::relocate(void *startPc, Addr len, void *toPc = nullptr) throw(ErrorCodeException) {
    AutoLock autoLock(relocateLock);
    startAddr = reinterpret_cast<Addr>(startPc);
    if (isThumbCode(startAddr)) {
        startAddr = reinterpret_cast<Addr>(getThumbCodeAddress(startPc));
    }
    length = len;
    curOffset = 0;
    __ allocBufferFirst(static_cast<U32>(len * 8));
    void* curPc = __ getPC();
    if (toPc == nullptr) {
        Disassembler::get()->decode(startPc, len, *this, true);
    } else {
        //TODO
    }
    return curPc;
}

void* CodeRelocateA32::relocate(Instruction<Base> *instruction, void *toPc) throw(ErrorCodeException) {
    void* curPc = __ getPC();

    //insert later bind labels
    __ Emit(getLaterBindLabel(curOffset));

    if (!instruction->pcRelate()) {
        __ Emit(instruction);
        instruction->ref();
        return curPc;
    }

    if (instruction->instType() == thumb32) {
        switch (instruction->instCode()) {
            CASE(T32, B32)
            CASE(T32, LDR_LIT)
            default:
                __ Emit(instruction);
                instruction->ref();
        }
    } else if (instruction->instType() == thumb16) {
        switch (instruction->instCode()) {
            CASE(T16, B)
            CASE(T16, B_COND)
            CASE(T16, BX_BLX)
            CASE(T16, CBZ_CBNZ)
            CASE(T16, ADR)
            CASE(T16, LDR_LIT)
            CASE(T16, ADD_REG_RDN)
            default:
                __ Emit(instruction);
                instruction->ref();
        }
    } else {
        __ Emit(instruction);
        instruction->ref();
    }
    return curPc;
}


IMPL_RELOCATE(T16, B_COND) {

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(InstT16))) {
        __ B(inst->condition, getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        return;
    }

    Addr targetAddr = inst->getImmPCOffsetTarget();

    if (inst->condition == al) {
        Label* target_label = new Label;
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

    Addr targetAddr = inst->getImmPCOffsetTarget();

    Label* target_label = new Label();
    __ Ldr(PC, target_label);
    __ Emit(target_label);
    __ Emit((Addr) getThumbPC(reinterpret_cast<void *>(targetAddr)));

}

IMPL_RELOCATE(T16, BX_BLX) {
    __ Emit(reinterpret_cast<Instruction<Base>*>(inst));
    inst->ref();
}

IMPL_RELOCATE(T16, CBZ_CBNZ) {

    inst->ref();

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(InstT16))) {
        inst->bindLabel(*getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<Instruction<Base>*>(inst));
        return;
    }

    Addr targetAddr = inst->getImmPCOffsetTarget();

    Label* true_label = new Label;
    Label* false_label = new Label;
    Label* target_label = new Label();

    inst->bindLabel(*true_label);
    __ Emit(reinterpret_cast<Instruction<Base>*>(inst));
    __ B(false_label);
    __ Emit(true_label);
    __ Ldr(PC, target_label);
    __ Emit(target_label);
    __ Emit((Addr) getThumbPC(reinterpret_cast<void *>(targetAddr)));
    __ Emit(false_label);

}

IMPL_RELOCATE(T16, LDR_LIT) {

    if (inRelocateRange(CODE_OFFSET(inst), inst->rt->getWide())) {
        inst->ref();
        inst->bindLabel(*getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<Instruction<Base>*>(inst));
        return;
    }

    Addr targetAddr = inst->getImmPCOffsetTarget();

    __ Mov(*inst->rt, targetAddr);
    __ Ldr(*inst->rt, MemOperand(inst->rt, 0));
}

IMPL_RELOCATE(T16, ADR) {

    if (inRelocateRange(CODE_OFFSET(inst), inst->rd->getWide())) {
        inst->ref();
        inst->bindLabel(*getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<Instruction<Base>*>(inst));
        return;
    }

    Addr targetAddr = inst->getImmPCOffsetTarget();

    __ Mov(*inst->rd, targetAddr);

}

IMPL_RELOCATE(T16, ADD_REG_RDN) {

    if (*inst->rm != PC) {
        inst->ref();
        __ Emit(reinterpret_cast<Instruction<Base>*>(inst));
        return;
    }

    RegisterA32& tmpReg = *inst->rdn != R0 ? R0 : R1;

    __ Push(tmpReg);
    __ Mov(tmpReg,(Addr)inst->getPC());
    __ Add(*inst->rdn, *inst->rdn, tmpReg);
    __ Pop(tmpReg);

}

IMPL_RELOCATE(T32, B32) {

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(InstT16))) {
        inst->ref();
        inst->bindLabel(*getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<Instruction<Base>*>(inst));
        return;
    }

    Addr targetAddr = inst->getImmPCOffsetTarget();


    if (inst->x == T32_B32::thumb) {
        //Thumb mode
        if (inst->op == T32_B32::BL) {
            Addr lr = reinterpret_cast<Addr>(toPc);
            lr += 2 * 2; // 2级流水线
            lr += 2 * 4; // Mov + Movt 长度
            lr += 4; // Ldr Lit 长度
            lr += 4; // targetAddr 长度
            __ Mov(LR, lr);
        }
        targetAddr = reinterpret_cast<Addr>(getThumbPC(reinterpret_cast<void *>(targetAddr)));
        Label* target_label = new Label();
        __ Ldr(PC, target_label);
        __ Emit(target_label);
        __ Emit(reinterpret_cast<Addr>(targetAddr));
    } else {
        //to A32 mode
        __ Mov(IP, targetAddr);
        if (inst->op == T32_B32::BL) {
            __ Blx(IP);
        } else {
            __ Bx(IP);
        }
    }

}

IMPL_RELOCATE(T32, LDR_LIT) {

    if (inRelocateRange(CODE_OFFSET(inst), sizeof(Addr))) {
        inst->ref();
        inst->bindLabel(*getLaterBindLabel(CODE_OFFSET(inst) + curOffset));
        __ Emit(reinterpret_cast<Instruction<Base>*>(inst));
        return;
    }

    Addr targetAddr = inst->getImmPCOffsetTarget();

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
            inst->ref();
            __ Emit(reinterpret_cast<Instruction<Base>*>(inst));
    }

}