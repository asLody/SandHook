//
// Created by swift on 2019/5/11.
//

#ifndef SANDHOOK_NH_LOCK_H
#define SANDHOOK_NH_LOCK_H

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

#endif //SANDHOOK_NH_LOCK_H
