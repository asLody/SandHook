//
// Created by swift on 2019/5/10.
//

#ifndef SANDHOOK_NH_UNIT_H
#define SANDHOOK_NH_UNIT_H

#include <malloc.h>
#include "../includes/base.h"

namespace SandHook {
    namespace Asm {

        template <typename Raw>
        class Unit {
        public:

            Unit() {
                if (unitType() != Void) {
                    raw = reinterpret_cast<Raw *>(malloc(size()));
                    memset(raw, 0, size());
                    auto_alloc = true;
                }
            }

            Unit<Raw>(Raw *raw) : raw(raw) {}

            Unit<Raw>(Raw raw) {
                Unit();
                *this->raw = raw;
            }

            virtual void* getPC() {
                return auto_alloc ? nullptr : raw;
            }

            inline Raw* get() const {
                return raw;
            }

            inline void set(Raw raw) const {
                *this->raw = raw;
            }

            inline void copy(void* dest) {
                memcpy(dest, getPC(), size());
            }

            virtual UnitType unitType() {
                return UnitType::Unkown;
            };

            virtual U32 size() {
                return sizeof(Raw);
            }

            virtual ~Unit() {
                if (auto_alloc) {
                    free(raw);
                }
            }

        private:
            Raw* raw;
            bool auto_alloc = false;
        };

    }
}

#endif //SANDHOOK_NH_UNIT_H
