//
// Created by 甘尧 on 2019/1/12.
//
#include "../includes/ICast.h"
#include "art/art_6_0.h"

namespace SandHook {


    class CastEntryPointQuickCompiled : public IMember<art::mirror::ArtMethod, void*> {
    protected:
        Size calOffset(art::mirror::ArtMethod p) override {
            return getParentSize() - BYTE_POINT;
        }
    };


    class CastAccessFlag : public IMember<art::mirror::ArtMethod, uint32_t> {
    protected:
        Size calOffset(art::mirror::ArtMethod p) override {
            int offset = findOffset(&p, getParentSize(), sizeof(uint32_t), 524313);
            if (offset < 0)
                throw 0;
            else
                return static_cast<size_t>(offset);
        }
    };




    class CastArtMethod {
    public:
        static Size size;
        static IMember<art::mirror::ArtMethod, void*>* entryPointQuickCompiled;
        static IMember<art::mirror::ArtMethod, uint32_t>* accessFlag;

        static void init(JNIEnv *env) {
            //init ArtMethodSize
            jclass sizeTestClass = env->FindClass("com/swift/sandhook/ArtMethodSizeTest");
            Size artMethod1 = (Size) env->GetStaticMethodID(sizeTestClass, "method1", "()V");
            Size artMethod2 = (Size) env->GetStaticMethodID(sizeTestClass, "method2", "()V");
            size = artMethod2 - artMethod1;

            art::mirror::ArtMethod m = *reinterpret_cast<art::mirror::ArtMethod *>(artMethod1);

            //init Members
            entryPointQuickCompiled = new CastEntryPointQuickCompiled();
            entryPointQuickCompiled->init(m, size);

            accessFlag = new CastAccessFlag();
            accessFlag->init(m, size);

        }
    };

    Size CastArtMethod::size = 0;
    IMember<art::mirror::ArtMethod, void*>* CastArtMethod::entryPointQuickCompiled = nullptr;
    IMember<art::mirror::ArtMethod, uint32_t>* CastArtMethod::accessFlag = nullptr;

}


