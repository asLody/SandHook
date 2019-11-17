//
// Created by swift on 2019/5/22.
//

#include "assembler_arm32.h"
#include "exception.h"

using namespace SandHook::Assembler;
using namespace SandHook::RegistersA32;
using namespace SandHook::AsmA32;

AssemblerA32::AssemblerA32(CodeBuffer* codeBuffer) {
    code_container.SetCodeBuffer(codeBuffer);
}

void *AssemblerA32::GetPC() {
    return reinterpret_cast<void *>(code_container.cur_pc);
}

void *AssemblerA32::GetStartPC() {
    return reinterpret_cast<void *>(code_container.start_pc);
}

void AssemblerA32::AllocBufferFirst(U32 size) {
    code_container.AllocBufferFirst(size);
}

void *AssemblerA32::Finish() {
    code_container.Commit();
    return reinterpret_cast<void *>(code_container.start_pc);
}

void AssemblerA32::Emit(U32 data32) {
    Emit(reinterpret_cast<BaseUnit*>(new Data32(data32)));
}

void AssemblerA32::Emit(U16 data16) {
    Emit(reinterpret_cast<BaseUnit*>(new Data16(data16)));
}

void AssemblerA32::Emit(BaseUnit *unit) {
    code_container.Append(unit);
}

void AssemblerA32::Mov(RegisterA32 &rd, U16 imm16) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(MOV_MOVT_IMM)(INST_T32(MOV_MOVT_IMM)::MOV, rd, imm16)));
}

void AssemblerA32::Movt(RegisterA32 &rd, U16 imm16) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(MOV_MOVT_IMM)(INST_T32(MOV_MOVT_IMM)::MOVT, rd, imm16)));
}

void AssemblerA32::Mov(RegisterA32 &rd, U32 imm32) {
    U16 immL = BITS16L(imm32);
    U16 immH = BITS16H(imm32);
    Mov(rd, immL);
    Movt(rd, immH);
}

void AssemblerA32::Ldr(RegisterA32 &rt, Off offset) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_LIT)(INST_T32(LDR_LIT)::LDR, INST_T32(LDR_LIT)::UnSign, rt, offset)));
}

void AssemblerA32::Ldr(RegisterA32 &rt, Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_LIT)(INST_T32(LDR_LIT)::LDR, INST_T32(LDR_LIT)::UnSign, rt, label)));
}

void AssemblerA32::Ldrb(RegisterA32 &rt, Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_LIT)(INST_T32(LDR_LIT)::LDRB, INST_T32(LDR_LIT)::UnSign,rt, label)));
}

void AssemblerA32::Ldrh(RegisterA32 &rt, Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_LIT)(INST_T32(LDR_LIT)::LDRH, INST_T32(LDR_LIT)::UnSign,rt, label)));
}

void AssemblerA32::Ldrsb(RegisterA32 &rt, Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_LIT)(INST_T32(LDR_LIT)::LDRB, INST_T32(LDR_LIT)::Sign,rt, label)));
}

void AssemblerA32::Ldrsh(RegisterA32 &rt, Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_LIT)(INST_T32(LDR_LIT)::LDRH, INST_T32(LDR_LIT)::Sign,rt, label)));
}

void AssemblerA32::Ldr(RegisterA32 &rt, const MemOperand &operand) {
    if (operand.addr_mode == Offset && operand.offset >= 0) {
        Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_UIMM)(rt, *operand.rn, operand.addr_mode)));
    } else {
        Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_IMM)(INST_T32(LDR_IMM)::LDR, rt, operand)));
    }
}

void AssemblerA32::Ldrb(RegisterA32 &rt, const MemOperand &operand) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_IMM)(INST_T32(LDR_IMM)::LDRB, rt, operand)));
}

void AssemblerA32::Ldrh(RegisterA32 &rt, const MemOperand &operand) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_IMM)(INST_T32(LDR_IMM)::LDRH, rt, operand)));
}

void AssemblerA32::Ldrsb(RegisterA32 &rt, const MemOperand &operand) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_IMM)(INST_T32(LDR_IMM)::LDRSB, rt, operand)));
}

void AssemblerA32::Ldrsh(RegisterA32 &rt, const MemOperand &operand) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(LDR_IMM)(INST_T32(LDR_IMM)::LDRSH, rt, operand)));
}

void AssemblerA32::B(Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T16(B)(label)));
}

void AssemblerA32::Bl(Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(B32)(INST_T32(B32)::BL, INST_T32(B32)::arm, label)));
}

void AssemblerA32::Blx(Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(B32)(INST_T32(B32)::BL, INST_T32(B32)::thumb, label)));
}

void AssemblerA32::Bx(Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(B32)(INST_T32(B32)::B, INST_T32(B32)::thumb, label)));
}

void AssemblerA32::Mov(RegisterA32 &rd, RegisterA32 &rm) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T16(MOV_REG)(rd, rm)));
}

void AssemblerA32::Bx(RegisterA32 &rm) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T16(BX_BLX)(INST_T16(BX_BLX)::BX, rm)));
}

void AssemblerA32::Blx(RegisterA32 &rm) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T16(BX_BLX)(INST_T16(BX_BLX)::BLX, rm)));
}

void AssemblerA32::B(Condition condition, Label* label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T16(B_COND)(condition, label)));
}

void AssemblerA32::Add(RegisterA32 &rdn, U8 imm8) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T16(ADD_IMM_RDN)(&rdn, imm8)));
}

void AssemblerA32::Add(RegisterA32 &rd, RegisterA32 &rn, RegisterA32 &rm) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T16(ADD_REG)(&rd, &rn, &rm)));
}

void AssemblerA32::Cmp(RegisterA32 &rd, RegisterA32 &rn) {
    if (rd.Code() < 8 && rn.Code() < 8) {
        Emit(reinterpret_cast<BaseUnit*>(new INST_T16(CMP_REG)(rd, rn)));
    } else {
        Emit(reinterpret_cast<BaseUnit*>(new INST_T16(CMP_REG_EXT)(rd, rn)));
    }
}

void AssemblerA32::Pop(RegisterA32 &rt) {
    if (rt.Code() < 8 || rt == PC) {
        Emit(reinterpret_cast<BaseUnit*>(new INST_T16(POP)(RegisterList(rt))));
    } else {
        throw ErrorCodeException("error pop inst");
    }
}

void AssemblerA32::Push(RegisterA32 &rt) {
    if (rt.Code() < 8 || rt == PC) {
        Emit(reinterpret_cast<BaseUnit*>(new INST_T16(PUSH)(RegisterList(rt))));
    } else {
        throw ErrorCodeException("error pop inst");
    }
}

void AssemblerA32::Adr(RegisterA32 &rd, Label *label) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T16(ADR)(rd, label)));
}

void AssemblerA32::Nop16() {
    Mov(IP, IP);
}

void AssemblerA32::Hvc(U16 num) {
    Emit(reinterpret_cast<BaseUnit*>(new INST_T32(HVC)(num)));
}


