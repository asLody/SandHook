//
// Created by swift on 2019/5/10.
//

#ifndef SANDHOOK_NH_LABEL_H
#define SANDHOOK_NH_LABEL_H

#include "unit.h"
#include <list>

namespace SandHook {
    namespace Asm {

        class LabelBinder {
        public:
            virtual void onLabelApply(Addr pc) = 0;
        };

        class Label : public Unit<Base> {
        public:

            Label() {}

            Label(void *pc) : pc(pc) {}

            inline UnitType unitType() override {
                return UnitType::UnitLabel;
            }

            inline U32 size() override {
                return 0;
            }

            inline void setPC(void* pc) {
                this->pc = pc;
            }

            inline void *getPC() override {
                return pc;
            }

            inline void addBinder(LabelBinder* binder) {
                binders.push_back(binder);
            }

            inline void removeBinder(LabelBinder* binder) {
                binders.push_back(binder);
            }

            inline void bindLabel() {
                std::list<LabelBinder*>::iterator binder;
                for(binder = binders.begin();binder != binders.end(); ++binder) {
                    (*binder)->onLabelApply(getVPC());
                }
            }

        private:
            void* pc;
            std::list<LabelBinder*> binders = std::list<LabelBinder*>();
        };

    }
}

#endif //SANDHOOK_NH_LABEL_H
