//
// Created by swift on 2019/5/10.
//

#include <assembler.h>
#include <platform.h>

using namespace SandHook::Assembler;
using namespace SandHook::Asm;

void CodeContainer::append(Unit<Base> *unit) {
    units.push_back(unit);
    switch (unit->unitType()) {
        case UnitLabel:
            labels.push_back((Label*)unit);
            break;
        default:
            curPc += unit->size();
    }
    unit->setVPC(curPc);
}

void CodeContainer::commit() {
    std::list<Unit<Base>*>::iterator unit;
    U32 bufferSize = static_cast<U32>(curPc - startPc);
    void* bufferStart = codeBuffer->getBuffer(bufferSize);
    Addr pcNow = reinterpret_cast<Addr>(bufferStart);
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
    std::list<Label*>::iterator label;
    for(label = labels.begin();label != labels.end(); ++label) {
        (*label)->bindLabel();
    }
    flushCache(reinterpret_cast<Addr>(bufferStart), pcNow - reinterpret_cast<Addr>(bufferStart));
}
