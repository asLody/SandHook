//
// Created by SwiftGan on 2019/1/17.
//

#ifndef SANDHOOK_BASE_H
#define SANDHOOK_BASE_H

#define FUNCTION_START(x) \
.text; \
.align 4; \
.global x; \
x: \

#define FUNCTION_END(x) .size x, .-x

#endif //SANDHOOK_BASE_H
