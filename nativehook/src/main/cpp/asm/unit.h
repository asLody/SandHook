//
// Created by swift on 2019/5/10.
//

#pragma once

#include <malloc.h>
#include <memory>
#include "base.h"

namespace SandHook {
    namespace Asm {

        template <typename S>
        class Unit {
        public:

            Unit() {}

            Unit<S>(S *pc) : pc_(pc) {}

            Unit<S>(void *pc) : pc_(ForceCast<S*>(pc)) {}

            virtual void* GetPC() {
                return pc_;
            }

            virtual Addr GetVPC() {
                return virtual_pc_;
            }

            INLINE void SetVPC(Addr vPos) {
                virtual_pc_ = vPos;
            }

            INLINE S* Get() {
                return pc_;
            }

            INLINE void Set(S raw) {
                *pc_ = raw;
            }

            INLINE void Set(S *raw) {
                pc_ = raw;
            }

            INLINE void Copy(S *dest) {
                *dest = *pc_;
            }

            virtual void Move(S *dest) {
                *dest = *pc_;
                pc_ = dest;
            }

            virtual UnitTypeDef UnitType() {
                return UnitTypeDef::UnitUnknow;
            };

            virtual U32 Size() {
                return sizeof(S);
            }

            INLINE U8 Ref() {
                return ++ref_count_;
            }

            INLINE U8 Release() {
                return --ref_count_;
            }

            INLINE U8 RefCount() {
                return ref_count_;
            }

        protected:
            S* pc_ = nullptr;
            Addr virtual_pc_ = 0;
            U8 ref_count_ = 0;
        };

        using BaseUnit = Unit<Base>;
        using BaseUnitRef = std::shared_ptr<BaseUnit>;

    }
}
