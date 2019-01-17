//
// Created by SwiftGan on 2019/1/17.
//

#ifndef SANDHOOK_BASE_H
#define SANDHOOK_BASE_H

#define FUNCTION_START(x) .global x; x:
#define FUNCTION_END(x) . - x

#endif //SANDHOOK_BASE_H
