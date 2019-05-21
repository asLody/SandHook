//
// Created by swift on 2019/5/22.
//

#ifndef SANDHOOK_ASSEMBLER_A32_H
#define SANDHOOK_ASSEMBLER_A32_H

#include "assembler.h"
#include "register_a32.h"
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


        public:
            CodeContainer codeContainer = CodeContainer(nullptr);
        };
    }
}

#endif //SANDHOOK_ASSEMBLER_A32_H
