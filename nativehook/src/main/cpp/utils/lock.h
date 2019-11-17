//
// Created by swift on 2019/5/11.
//

#pragma once

#include <mutex>

namespace SandHook {
    namespace Utils {


        class AutoLock {
        public:
            inline AutoLock(std::mutex& mutex) : mLock(mutex)  { mLock.lock(); }
            inline AutoLock(std::mutex* mutex) : mLock(*mutex) { mLock.lock(); }
            inline ~AutoLock() { mLock.unlock(); }
        private:
            std::mutex& mLock;
        };

    }
}