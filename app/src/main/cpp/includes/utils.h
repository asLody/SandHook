//
// Created by 甘尧 on 2019/1/13.
//

#ifndef SANDHOOK_UTILS_H
#define SANDHOOK_UTILS_H


#include <cwchar>

template<typename T>
int findOffset(void *start, size_t len,size_t step,T value) {

    if (NULL == start) {
        return -1;
    }

    for (int i = 0; i <= len; i += step) {
        T current_value = *reinterpret_cast<T*>((size_t)start + i);
        if (value == current_value) {
            return i;
        }
    }
    return -1;
}

#endif //SANDHOOK_UTILS_H
