//
// Created by swift on 2019/5/10.
//

#ifndef SANDHOOK_NH_LABEL_H
#define SANDHOOK_NH_LABEL_H

#include "unit.h"
#include <list>

namespace SandHook {
    namespace Asm {

        virtual class LabelBinder {
        public:
            virtual void bindLabel(void* pc) = 0;
        };

        class Label : public Unit<None> {
        public:

            Label() {}

            Label(void *pc) : pc(pc) {}

            inline UnitType unitType() override {
                return UnitType::Label;
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

            inline void bindLabel(void* pc) {
                setPC(pc);
                std::list<LabelBinder*>::iterator binder;
                for(binder = binders.begin();binder != binders.end(); ++binder) {
                    (*binder)->bindLabel(pc);
                }
            }

        private:
            void* pc;
            std::list<LabelBinder*> binders = std::list();
        };

    }
}

#endif //SANDHOOK_NH_LABEL_H
