//
// Created by 甘尧 on 2019/1/13.
//

#ifndef SANDHOOK_UTILS_H
#define SANDHOOK_UTILS_H


#if defined(__MACH__) && defined(__FreeBSD__) && defined(__NetBSD__) && defined(__OpenBSD__)\
    && defined(__DragonFly__)
#define ERROR_SIGNAL SIGBUS
#else
#define ERROR_SIGNAL SIGSEGV
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
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

template<typename T>
int findOffsetWithCB1(void *start, size_t len,size_t step, bool func(int, T)) {

    if (NULL == start) {
        return -1;
    }

    for (int i = 0; i <= len; i += step) {
        T current_value = *reinterpret_cast<T*>((size_t)start + i);
        if (func(i, current_value)) {
            return i;
        }
    }
    return -1;
}

template<typename T>
int findOffsetWithCB2(void *start1, void *start2, size_t len,size_t step, bool func(T,T)) {

    if (NULL == start1 || NULL == start2) {
        return -1;
    }

    for (int i = 0; i <= len; i += step) {
        T v1 = *reinterpret_cast<T*>((size_t)start1 + i);
        T v2 = *reinterpret_cast<T*>((size_t)start2 + i);
        if (func(v1, v2)) {
            return i;
        }
    }

    return -1;
}

static sigjmp_buf badreadjmpbuf;


static void badreadfunc(int signo)
{
    /*write(STDOUT_FILENO, "catch\n", 6);*/
    siglongjmp(badreadjmpbuf, 1);
}


bool isBadReadPtr(void *ptr, int length)
{
    struct sigaction sa, osa;
    bool ret = false;

    /*init new handler struct*/
    sa.sa_handler = badreadfunc;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    /*retrieve old and set new handlers*/
    if(sigaction(ERROR_SIGNAL, &sa, &osa)<0)
        return true;

    if(sigsetjmp(badreadjmpbuf, 1) == 0)
    {
        int i, hi=length/sizeof(int), remain=length%sizeof(int);
        int* pi = static_cast<int *>(ptr);
        char* pc = (char*)ptr + hi;
        for(i=0;i<hi;i++)
        {
            int tmp = *(pi+i);
        }
        for(i=0;i<remain;i++)
        {
            char tmp = *(pc+i);
        }

    }
    else
    {
        ret = true;
    }

    /*restore prevouis signal actions*/
    if(sigaction(ERROR_SIGNAL, &osa, NULL)<0)
        return true;

    return ret;
}


Size getAddressFromJava(JNIEnv* env, char* className, char* fieldName) {
    jclass clazz = env -> FindClass(className);
    if (clazz == NULL){
        printf("find class error !");
        return 0;
    }
    jfieldID id = env -> GetStaticFieldID(clazz, fieldName, "Ljava/lang/Long;");
    if (id == NULL){
        printf("find field error !");
        return 0;
    }
    return env -> GetStaticLongField(clazz, id);
}



#endif //SANDHOOK_UTILS_H
