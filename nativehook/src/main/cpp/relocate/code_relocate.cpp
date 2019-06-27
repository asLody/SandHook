//
// Created by swift on 2019/5/10.
//

#include "code_relocate.h"

using namespace SandHook::Asm;

//in range of Copy
bool CodeRelocate::inRelocateRange(Off targetOffset, Addr targetLen) {
    Off startP = curOffset + targetOffset;
    Off endP = startP + targetLen;
    return startP >= 0 && endP <= length;
}

Label *CodeRelocate::getLaterBindLabel(Addr offset) {
    Label* label_per_unit = nullptr;
    std::map<Addr,Label*>::iterator it = laterBindlabels->find(offset);
    if (it != laterBindlabels->end()) {
        label_per_unit = it->second;
    }
    if (label_per_unit == nullptr) {
        label_per_unit = new Label();
        laterBindlabels->insert(std::map<Addr, Label*>::value_type(offset, label_per_unit));
    }
    return label_per_unit;
}