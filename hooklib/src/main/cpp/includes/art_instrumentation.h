//
// Created by SwiftGan on 2019/2/28.
//

#ifndef SANDHOOK_ART_INSTRUMENTATION_H
#define SANDHOOK_ART_INSTRUMENTATION_H

#include "art_method.h"

namespace art {
    namespace instrumentation {
        class Instrumentation {
        public:
            bool deOptimizeMethod(mirror::ArtMethod* method);
        };
    }
}

#endif //SANDHOOK_ART_INSTRUMENTATION_H
