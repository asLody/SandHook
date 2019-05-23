//
// Created by swift on 2019/5/22.
//

#ifndef SANDHOOK_ASSEMBLER_A32_H
#define SANDHOOK_ASSEMBLER_A32_H

#include "assembler.h"
#include "register_arm32.h"
#include "inst_t16.h"
#include "inst_t32.h"

using namespace SandHook::AsmA32;

namespace SandHook {
    namespace Assembler {

        class AssemblerA32 {
        public:
            AssemblerA32(CodeBuffer* codeBuffer);

            void allocBufferFirst(U32 size);
            void* getStartPC();
            void* getPC();
            void* finish();

            void Emit(U32 data32);
            void Emit(Unit<Base>* unit);


            void Mov(RegisterA32 &rd, U16 imm16);
            void Movt(RegisterA32 &rd, U16 imm16);

            void Mov(RegisterA32 &rd, U32 imm32);
            void Mov(RegisterA32 &rd, RegisterA32 &rm);

            void Ldr(RegisterA32 &rt, Label* label);
            void Ldrb(RegisterA32 &rt, Label* label);
            void Ldrh(RegisterA32 &rt, Label* label);

            void Ldr(RegisterA32 &rt, const MemOperand& operand);
            void Ldrb(RegisterA32 &rt, const MemOperand& operand);
            void Ldrh(RegisterA32 &rt, const MemOperand& operand);
            void Ldrsb(RegisterA32 &rt, const MemOperand& operand);
            void Ldrsh(RegisterA32 &rt, const MemOperand& operand);

            void B(Label* label);
            void Bl(Label* label);
            void Blx(Label* label);
            void Bx(Label* label);

            void Blx(RegisterA32 &rm);
            void Bx(RegisterA32 &rm);

            void B(Condition condition, Label* label);

            void Add(RegisterA32 &rdn, U8 imm8);
            void Add(RegisterA32 &rd, RegisterA32 &rn, RegisterA32& rm);


            void Cmp(RegisterA32 &rn, RegisterA32 &rm);

            void Pop(RegisterA32& rt);
            void Push(RegisterA32& rt);

        public:
            CodeContainer codeContainer = CodeContainer(nullptr);
        };
    }
}

#endif //SANDHOOK_ASSEMBLER_A32_H
