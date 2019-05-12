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
    unit->setVPC(curPc);
    switch (unit->unitType()) {
        case UnitLabel:
            labels.push_back((Label*)unit);
            break;
        default:
            curPc += unit->size();
    }
}

void CodeContainer::commit() {
    U32 bufferSize = static_cast<U32>(curPc - startPc);
    void* bufferStart;
    if (startPc > 0) {
        bufferStart = reinterpret_cast<void *>(startPc);
        codeBuffer->resetLastBufferSize(bufferSize);
    } else {
        bufferStart = codeBuffer->getBuffer(bufferSize);
    }
    Addr pcNow = reinterpret_cast<Addr>(bufferStart);

    //commit to code buffer & assembler inst
    std::list<Unit<Base>*>::iterator unit;
    for(unit = units.begin();unit != units.end(); ++unit) {
        if ((*unit)->unitType() == UnitData) {
            (*unit)->move(reinterpret_cast<Base *>(pcNow));
        } else if ((*unit)->unitType() != UnitLabel) {
            (*unit)->set(reinterpret_cast<Base *>(pcNow));
        }
        if ((*unit)->unitType() == UnitInst) {
            reinterpret_cast<Instruction<Base>*>(*unit)->assembler();
        }
        pcNow += (*unit)->size();
    }

    //bind labels
    std::list<Label*>::iterator label;
    for(label = labels.begin();label != labels.end(); ++label) {
        (*label)->bindLabel();
    }

    //flush I cache
    flushCache(reinterpret_cast<Addr>(bufferStart), pcNow - reinterpret_cast<Addr>(bufferStart));

    //set pc
    startPc = reinterpret_cast<Addr>(bufferStart);
    curPc = pcNow;

}

void CodeContainer::allocBufferFirst(U32 size) {
    startPc = reinterpret_cast<Addr>(codeBuffer->getBuffer(size));
    curPc = startPc;
}
