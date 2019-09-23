//
// Created by swift on 2019/5/10.
//
#pragma once

#include <log.h>
#include "exception"


namespace SandHook {
    namespace Asm {

        constexpr int ERROR_DEFAULT = 0;
        constexpr int ERROR_SWITCH_TO_ARM32 = 1;

        class ErrorCodeException : public std::exception {
        public:

            ErrorCodeException(const char *what) : code_(ERROR_DEFAULT), what_(what) {
                LOGE("ErrorCodeException: %s", what);
            }

            ErrorCodeException(const int code, const char *what) : code_(code), what_(what) {
                LOGE("ErrorCodeException: %s", what);
            }

            const char *what() const noexcept override {
                return what_;
            }

            const int Code() const {
                return code_;
            }

        private:
            const int code_;
            const char *what_;
        };

    }
}