//
// Created by swift on 2019/5/10.
//

#pragma once

#include "unit.h"
#include <list>

namespace SandHook {
    namespace Asm {

        class LabelBinder {
        public:
            virtual void OnLabelApply(Addr pc) = 0;
        };

        class Label : public BaseUnit {
        public:

            Label() {}

            Label(void *pc) : BaseUnit(pc) {}

            enum UnitTypeDef UnitType() override {
                return UnitTypeDef::UnitLabel;
            }

            INLINE U32 Size() override {
                return 0;
            }

            INLINE void AddBind(LabelBinder *binder) {
                binders.push_back(binder);
            }

            INLINE void RemoveBind(LabelBinder *binder) {
                binders.push_back(binder);
            }

            INLINE void BindLabel() {
                std::list<LabelBinder*>::iterator binder;
                for(binder = binders.begin();binder != binders.end(); ++binder) {
                    (*binder)->OnLabelApply(GetVPC());
                }
            }

        private:
            std::list<LabelBinder*> binders = std::list<LabelBinder*>();
        };

    }
}