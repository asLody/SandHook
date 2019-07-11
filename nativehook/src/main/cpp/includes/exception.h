//
// Created by swift on 2019/5/10.
//
#pragma once

#include "exception"


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