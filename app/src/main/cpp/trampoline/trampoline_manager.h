//
// Created by swift on 2019/1/20.
//

#ifndef SANDHOOK_TRAMPOLINE_MANAGER_H
#define SANDHOOK_TRAMPOLINE_MANAGER_H

#include "map"
#include "list"
#include "trampoline.cpp"

namespace SandHook {

    class TrampolineManager {
    public:
        TrampolineManager() {
            trampolines = {};
        }
    private:
        std::map<void*,Trampoline*> trampolines;
    };

}

#endif //SANDHOOK_TRAMPOLINE_MANAGER_H
