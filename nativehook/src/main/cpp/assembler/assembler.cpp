//
// Created by swift on 2019/5/10.
//

#include <assembler.h>
#include <platform.h>

using namespace SandHook::Assembler;
using namespace SandHook::Asm;

CodeContainer::CodeContainer(CodeBuffer *codeBuffer) : codeBuffer(codeBuffer) {}

void CodeContainer::setCodeBuffer(CodeBuffer *codeBuffer) {
    this->codeBuffer = codeBuffer;
}

void CodeContainer::append(Unit<Base> *unit) {
    units.push_back(unit);
    unit->SetVPC(curPc);
    switch (unit->UnitType()) {
        case UnitLabel:
            labels.push_back((Label*)unit);
            break;
        default:
            curPc += unit->Size();
    }
}

void CodeContainer::commit() {
    U32 bufferSize = static_cast<U32>(curPc - startPc);
    void* bufferStart;
    if (startPc > 0) {
        bufferStart = reinterpret_cast<void *>(startPc);
        codeBuffer->ResetLastBufferSize(bufferSize);
    } else {
        bufferStart = codeBuffer->GetBuffer(bufferSize);
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
    startPc = reinterpret_cast<Addr>(bufferStart);
    curPc = pcNow;

}

void CodeContainer::allocBufferFirst(U32 size) {
    startPc = reinterpret_cast<Addr>(codeBuffer->GetBuffer(size));
    curPc = startPc;
}

CodeContainer::~CodeContainer() {
    std::list<Unit<Base>*>::iterator unit;
    for(unit = units.begin();unit != units.end(); ++unit) {
        delete (*unit);
    }
}

Addr CodeContainer::size() {
    return curPc - startPc;
}
