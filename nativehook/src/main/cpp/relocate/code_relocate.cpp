//
// Created by swift on 2019/5/10.
//

#include "code_relocate.h"

using namespace SandHook::Asm;

//in range of Copy
bool CodeRelocate::InRelocateRange(Off targetOffset, Addr targetLen) {
    Off startP = cur_offset + targetOffset;
    Off endP = startP + targetLen;
    return startP >= 0 && endP <= length;
}

Label *CodeRelocate::GetLaterBindLabel(Addr offset) {
    Label* label_per_unit = nullptr;
    std::map<Addr,Label*>::iterator it = later_bind_labels->find(offset);
    if (it != later_bind_labels->end()) {
        label_per_unit = it->second;
    }
    if (label_per_unit == nullptr) {
        label_per_unit = new Label();
        later_bind_labels->insert(std::map<Addr, Label*>::value_type(offset, label_per_unit));
    }
    return label_per_unit;
}