//
// Created by swift on 2019/5/10.
//

#include "exception"

#ifndef SANDHOOK_NH_EXCEPTION_H
#define SANDHOOK_NH_EXCEPTION_H

namespace SandHook {
    namespace Asm {

        class ErrorCodeException : public std::exception {
        public:

            ErrorCodeException(const char *what_) : what_(what_) {}

            const char *what() const noexcept override {
                return what_;
            }

        private:
            const char *what_;
        };

    }
}

#endif //SANDHOOK_NH_EXCEPTION_H
