//
// Created by 甘尧 on 2019/2/23.
//

#ifndef SANDHOOK_ART_COMPILER_OPTIONS_H
#define SANDHOOK_ART_COMPILER_OPTIONS_H

#include "arch.h"

namespace art {
    class CompilerOptions {
    public:
        void* compiler_filter_;
        Size huge_method_threshold_;
        Size large_method_threshold_;
        Size small_method_threshold_;
        Size tiny_method_threshold_;
        Size num_dex_methods_threshold_;
        Size inline_depth_limit_;
        Size inline_max_code_units_;
    };
}

#endif //SANDHOOK_ART_COMPILER_OPTIONS_H
