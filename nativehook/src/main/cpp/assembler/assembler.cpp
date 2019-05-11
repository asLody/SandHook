//
// Created by swift on 2019/5/10.
//

#include <assembler.h>

using namespace SandHook::Assembler;
using namespace SandHook::Asm;

void CodeContainer::append(Unit<Base> *unit) {
    Label* l = dynamic_cast<Label *>(unit);
    units.push_back(unit);
    switch (unit->unitType()) {
        case UnitLabel:
            labels.push_back((Label*)unit);
            break;
        default:
            curPc += unit->size();
    }
    if (unit->unitType() == UnitData) {
        unit->move((Base*)codeBuffer->getBuffer(unit));
    } else if (unit->unitType() != UnitLabel) {
        unit->set((Base*)codeBuffer->getBuffer(unit));
    }
}

void CodeContainer::commit() {
    std::list<Label*>::iterator label;
    for(label = labels.begin();label != labels.end(); ++label) {
        (*label)->bindLabel();
    }
}

void *CodeBuffer::getBuffer(Unit<Base> *unit) {
    return nullptr;
}
