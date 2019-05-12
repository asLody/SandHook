//
// Created by swift on 2019/5/11.
//

#ifndef SANDHOOK_NH_ASSEMBLER_A64_H
#define SANDHOOK_NH_ASSEMBLER_A64_H

#include <assembler.h>
#include "register_a64.h"
#include "inst_arm64.h"

namespace SandHook {
    namespace Assembler {

        class AssemblerA64 {

        public:
            AssemblerA64(CodeBuffer* codeBuffer);

            void allocBufferFirst(U32 size);
            void* getStartPC();
            void* getPC();
            void* finish();


            void Emit(Unit<Base>* unit);


            void MoveWide(RegisterA64& rd, INST_A64(MOV_WIDE)::OP op, U64 imme, INST_A64(MOV_WIDE)::Shift shift);

            void Movz(RegisterA64& rd, U64 imme, INST_A64(MOV_WIDE)::Shift shift);
            void Movk(RegisterA64& rd, U64 imme, INST_A64(MOV_WIDE)::Shift shift);
            void Movn(RegisterA64& rd, U64 imme, INST_A64(MOV_WIDE)::Shift shift);

            void Mov(WRegister& rd, U32 imme);
            void Mov(XRegister& rd, U64 imme);

            void Br(XRegister& rn);

            void B(Off offset);
            void B(Label* label);

            void Bl(Off offset);
            void Bl(Label* label);

            void B(Condition condition, Off offset);
            void B(Condition condition, Label* label);

            void Tbz(RegisterA64 &rt, U32 bit, Off offset);
            void Tbz(RegisterA64 &rt, U32 bit, Label* label);

            void Tbnz(RegisterA64 &rt, U32 bit, Off offset);
            void Tbnz(RegisterA64 &rt, U32 bit, Label* label);

            void Cbz(RegisterA64 &rt, Off offset);
            void Cbz(RegisterA64 &rt, Label* label);

            void Cbnz(RegisterA64 &rt, Off offset);
            void Cbnz(RegisterA64 &rt, Label* label);


        public:
            CodeContainer codeContainer = CodeContainer(nullptr);
        };

    }
}

#endif //SANDHOOK_NH_ASSEMBLER_A64_H
