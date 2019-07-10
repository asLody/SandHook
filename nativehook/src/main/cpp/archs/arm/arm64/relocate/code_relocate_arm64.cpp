//
// Created by swift on 2019/5/12.
//

#include "code_relocate_arm64.h"
#include "decoder.h"
#include "lock.h"

using namespace SandHook::Decoder;
using namespace SandHook::RegistersA64;
using namespace SandHook::AsmA64;
using namespace SandHook::Utils;

#define __ assemblerA64->

#define IMPL_RELOCATE(X) void CodeRelocateA64::relocate_##X (INST_A64(X)* inst, void* toPc) throw(ErrorCodeException)

#define CASE(X) \
case ENUM_VALUE(InstCodeA64, InstCodeA64::X): \
relocate_##X(reinterpret_cast<INST_A64(X)*>(instruction), toPc); \
break;

CodeRelocateA64::CodeRelocateA64(AssemblerA64 &assembler) : CodeRelocate(assembler.code_container) {
    assemblerA64 = &assembler;
}

bool CodeRelocateA64::Visit(BaseUnit *unit, void *pc) {
    Relocate(reinterpret_cast<BaseInst *>(unit), __ GetPC());
    cur_offset += unit->Size();
    if (unit->RefCount() == 0) {
        delete unit;
    }
    return true;
}

void* CodeRelocateA64::Relocate(void *startPc, Addr len, void *toPc = nullptr) throw(ErrorCodeException) {
    AutoLock autoLock(relocate_lock);
    start_addr = reinterpret_cast<Addr>(startPc);
    length = len;
    cur_offset = 0;
    __ AllocBufferFirst(static_cast<U32>(len * 8));
    void* cur_pc = __ GetPC();
    if (toPc == nullptr) {
        Disassembler::Get()->Disassemble(startPc, len, *this, true);
    } else {
        //TODO
    }
    return cur_pc;
}

void* CodeRelocateA64::Relocate(BaseInst *instruction, void *toPc) throw(ErrorCodeException) {
    void* cur_pc = __ GetPC();

    //insert later AddBind labels
    __ Emit(GetLaterBindLabel(cur_offset));

    if (!instruction->PcRelate()) {
        __ Emit(instruction);
        instruction->Ref();
        return cur_pc;
    }
    switch (instruction->InstCode()) {
        CASE(B_BL)
        CASE(B_COND)
        CASE(TBZ_TBNZ)
        CASE(CBZ_CBNZ)
        CASE(LDR_LIT)
        CASE(ADR_ADRP)
        default:
            __ Emit(instruction);
            instruction->Ref();
    }
    return cur_pc;
}

IMPL_RELOCATE(B_BL) {

    if (InRelocateRange(inst->offset, sizeof(InstA64))) {
        inst->Ref();
        inst->BindLabel(GetLaterBindLabel(inst->offset + cur_offset));
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    Addr target_addr = inst->GetImmPCOffsetTarget();

    if (inst->op == inst->BL) {
        Addr lr = reinterpret_cast<Addr>(toPc);
        lr += 4 * 4; // MovWide * 4;
        lr += 4 * 4; // MovWide * 4;
        lr += 4; // Br
        __ Mov(LR, lr);
    }
    __ Mov(IP1, target_addr);
    __ Br(IP1);
}

IMPL_RELOCATE(B_COND) {

    if (InRelocateRange(inst->offset, sizeof(InstA64))) {
        inst->Ref();
        inst->BindLabel(GetLaterBindLabel(inst->offset + cur_offset));
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    Addr target_addr = inst->GetImmPCOffsetTarget();

    Label *true_label = new Label();
    Label *false_label = new Label();

    __ B(inst->condition, true_label);
    __ B(false_label);

    __ Emit(true_label);
    __ Mov(IP1, target_addr);
    __ Br(IP1);

    __ Emit(false_label);
}

IMPL_RELOCATE(TBZ_TBNZ) {

    if (InRelocateRange(inst->offset, sizeof(InstA64))) {
        inst->Ref();
        inst->BindLabel(GetLaterBindLabel(inst->offset + cur_offset));
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    Addr target_addr = inst->GetImmPCOffsetTarget();

    Label *true_label = new Label();
    Label *false_label = new Label();

    if (inst->op == INST_A64(TBZ_TBNZ)::TBNZ) {
        __ Tbnz(*inst->rt, inst->bit, true_label);
    } else {
        __ Tbz(*inst->rt, inst->bit, true_label);
    }
    __ B(false_label);

    __ Emit(true_label);
    __ Mov(IP1, target_addr);
    __ Br(IP1);

    __ Emit(false_label);
}

IMPL_RELOCATE(CBZ_CBNZ) {

    if (InRelocateRange(inst->offset, sizeof(InstA64))) {
        inst->Ref();
        inst->BindLabel(GetLaterBindLabel(inst->offset + cur_offset));
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    Addr target_addr = inst->GetImmPCOffsetTarget();

    Label *true_label = new Label();
    Label *false_label = new Label();

    if (inst->op == INST_A64(CBZ_CBNZ)::CBNZ) {
        __ Cbnz(*inst->rt, true_label);
    } else {
        __ Cbz(*inst->rt, true_label);
    }

    __ B(false_label);

    __ Emit(true_label);
    __ Mov(IP1, target_addr);
    __ Br(IP1);

    __ Emit(false_label);
}

IMPL_RELOCATE(LDR_LIT) {
    Addr target_addr = inst->GetImmPCOffsetTarget();
    XRegister* rtX = XReg(inst->rt->Code());
    WRegister* rtW = WReg(inst->rt->Code());

    if (InRelocateRange(inst->offset, sizeof(Addr))) {
        inst->Ref();
        inst->BindLabel(GetLaterBindLabel(inst->offset + cur_offset));
        __ Emit(reinterpret_cast<BaseInst*>(inst));
        return;
    }

    switch (inst->op) {
        case INST_A64(LDR_LIT)::LDR_X:
            __ Mov(*rtX, target_addr);
            __ Ldr(*rtX, MemOperand(rtX, 0, Offset));
            break;
        case INST_A64(LDR_LIT)::LDR_W:
            __ Mov(*rtX, target_addr);
            __ Ldr(*rtW, MemOperand(rtX, 0, Offset));
            break;
        case INST_A64(LDR_LIT)::LDR_SW:
            __ Mov(*rtX, target_addr);
            __ Ldrsw(*rtX, MemOperand(rtX, 0, Offset));
            break;
        case INST_A64(LDR_LIT)::LDR_PRFM:
            __ Push(X0);
            __ Mov(X0, target_addr);
            __ Ldrsw(X0, MemOperand(rtX, 0, Offset));
            __ Pop(X0);
            break;
    }
}

IMPL_RELOCATE(ADR_ADRP) {
    __ Mov(*inst->rd, inst->GetImmPCOffsetTarget());
}
