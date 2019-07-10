//
// Created by swift on 2019/5/10.
//

#include <assembler.h>
#include <platform.h>

using namespace SandHook::Assembler;
using namespace SandHook::Asm;

CodeContainer::CodeContainer(CodeBuffer *codeBuffer) : code_buffer(codeBuffer) {}

void CodeContainer::SetCodeBuffer(CodeBuffer *codeBuffer) {
    this->code_buffer = codeBuffer;
}

void CodeContainer::Append(BaseUnit *unit) {
    units.push_back(unit);
    unit->SetVPC(cur_pc);
    switch (unit->UnitType()) {
        case UnitLabel:
            labels.push_back((Label*)unit);
            break;
        default:
            cur_pc += unit->Size();
    }
}

void CodeContainer::Commit() {
    U32 bufferSize = static_cast<U32>(cur_pc - start_pc);
    void* bufferStart;
    if (start_pc > 0) {
        bufferStart = reinterpret_cast<void *>(start_pc);
        code_buffer->ResetLastBufferSize(bufferSize);
    } else {
        bufferStart = code_buffer->GetBuffer(bufferSize);
    }
    Addr pcNow = reinterpret_cast<Addr>(bufferStart);

    //commit to code buffer & Assembler inst
    std::list<BaseUnit*>::iterator unit;
    for(unit = units.begin();unit != units.end(); ++unit) {
        if ((*unit)->UnitType() == UnitData) {
            (*unit)->Move(reinterpret_cast<Base*>(pcNow));
        } else if ((*unit)->UnitType() != UnitLabel) {
            (*unit)->Set(reinterpret_cast<Base*>(pcNow));
        }
        if ((*unit)->UnitType() == UnitInst) {
            reinterpret_cast<BaseInst*>(*unit)->Assemble();
        }
        pcNow += (*unit)->Size();
    }

    //AddBind labels
    std::list<Label*>::iterator label;
    for(label = labels.begin();label != labels.end(); ++label) {
        (*label)->BindLabel();
    }

    //flush I cache
    FlushCache(reinterpret_cast<Addr>(bufferStart), pcNow - reinterpret_cast<Addr>(bufferStart));

    //Set pc
    start_pc = reinterpret_cast<Addr>(bufferStart);
    cur_pc = pcNow;

}

void CodeContainer::AllocBufferFirst(U32 size) {
    start_pc = reinterpret_cast<Addr>(code_buffer->GetBuffer(size));
    cur_pc = start_pc;
}

CodeContainer::~CodeContainer() {
    std::list<BaseUnit*>::iterator unit;
    for(unit = units.begin();unit != units.end(); ++unit) {
        delete (*unit);
    }
}

Addr CodeContainer::Size() {
    return cur_pc - start_pc;
}
