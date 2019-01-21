//
// Created by 甘尧 on 2019/1/12.
//

#ifndef SANDHOOK_CAST_ART_METHOD_H
#define SANDHOOK_CAST_ART_METHOD_H

#include "../includes/cast.h"
#include "../trampoline/trampoline_manager.h"

namespace SandHook {

    class CastDexCacheResolvedMethods : public ArrayMember<art::mirror::ArtMethod> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod p) override {
            if (SDK_INT >= ANDROID_P)
                return getParentSize() + 1;
            int offset = 0;
            Size addr = getAddressFromJava(jniEnv, "com/swift/sandhook/SandHookMethodResolver", "resolvedMethodsAddress");
            if (addr != 0) {
                offset = findOffset(&p, getParentSize(), 2, addr);
                if (offset >= 0) {
                    return static_cast<Size>(offset);
                }
            }
            return getParentSize() + 1;
        }

        Size calElementSize(JNIEnv *jniEnv, art::mirror::ArtMethod p) override {
            return getParentSize();
        }
    };

    class CastEntryPointFormInterpreter : public IMember<art::mirror::ArtMethod, void*> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod p) override {
            if (SDK_INT >= ANDROID_L2 && SDK_INT <= ANDROID_M)
                return getParentSize() - 3 * BYTE_POINT;
            else if (SDK_INT <= ANDROID_L)
                return getParentSize() - 4 * 8 - 4 * 4;
            else
                return getParentSize() + 1;
        }
    };

    class CastEntryPointQuickCompiled : public IMember<art::mirror::ArtMethod, void*> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod p) override {
            if (SDK_INT >= ANDROID_L2) {
                return getParentSize() - BYTE_POINT;
            } else {
                return getParentSize() - 8 * 2 - 4 * 4;
            }
        }
    };

    class CastEntryPointFromJni : public IMember<art::mirror::ArtMethod, void*> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod p) override {
            if (SDK_INT >= ANDROID_L2 && SDK_INT <= ANDROID_N) {
                return getParentSize() - 2 * BYTE_POINT;
            } else {
                return getParentSize() - 8 * 2 - 4 * 4;
            }
        }
    };


    class CastAccessFlag : public IMember<art::mirror::ArtMethod, uint32_t> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod p) override {
            int offset = findOffset(&p, getParentSize(), 2, (uint32_t)524313);
            if (offset < 0)
                return getParentSize() + 1;
            else
                return static_cast<size_t>(offset);
        }
    };


    class CastDexMethodIndex : public IMember<art::mirror::ArtMethod, uint32_t> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod p) override {
            if (SDK_INT >= ANDROID_P)
                return getParentSize() + 1;
            int offset = 0;
            jint index = getIntFromJava(jniEnv, "com/swift/sandhook/SandHookMethodResolver", "dexMethodIndex");
            if (index != 0) {
                offset = findOffset(&p, getParentSize(), 2, static_cast<uint32_t>(index));
                if (offset >= 0) {
                    return static_cast<Size>(offset);
                }
            }
            return getParentSize() + 1;
        }
    };




    class CastArtMethod {
    public:
        static Size size;
        static IMember<art::mirror::ArtMethod, void*>* entryPointQuickCompiled;
        static IMember<art::mirror::ArtMethod, void*>* entryPointFormInterpreter;
        static ArrayMember<art::mirror::ArtMethod>* dexCacheResolvedMethods;
        static IMember<art::mirror::ArtMethod, uint32_t>* dexMethodIndex;
        static IMember<art::mirror::ArtMethod, uint32_t>* accessFlag;
        static Code quickToInterpreterBridge;


        static void init(JNIEnv *env, int sdk) {
            SDK_INT = sdk;
            //init ArtMethodSize
            jclass sizeTestClass = env->FindClass("com/swift/sandhook/ArtMethodSizeTest");
            Size artMethod1 = (Size) env->GetStaticMethodID(sizeTestClass, "method1", "()V");
            Size artMethod2 = (Size) env->GetStaticMethodID(sizeTestClass, "method2", "()V");
            size = artMethod2 - artMethod1;

            art::mirror::ArtMethod m1 = *reinterpret_cast<art::mirror::ArtMethod *>(artMethod1);
            art::mirror::ArtMethod m2 = *reinterpret_cast<art::mirror::ArtMethod *>(artMethod2);

            //init Members
            entryPointQuickCompiled = new CastEntryPointQuickCompiled();
            entryPointQuickCompiled->init(env, m1, size);

            accessFlag = new CastAccessFlag();
            accessFlag->init(env, m1, size);

            entryPointFormInterpreter = new CastEntryPointFormInterpreter();
            entryPointFormInterpreter->init(env, m1, size);

            dexCacheResolvedMethods = new CastDexCacheResolvedMethods();
            dexCacheResolvedMethods->init(env, m1, size);

            dexMethodIndex = new CastDexMethodIndex();
            dexMethodIndex->init(env, m1, size);

            jclass neverCallTestClass = env->FindClass("com/swift/sandhook/ClassNeverCall");
            art::mirror::ArtMethod* neverCall = reinterpret_cast<art::mirror::ArtMethod *>(env->GetMethodID(neverCallTestClass, "neverCall", "()V"));
            quickToInterpreterBridge = reinterpret_cast<Code>(entryPointQuickCompiled->get(*neverCall));

//            //test
//            art::mirror::ArtMethod** mArray = reinterpret_cast<art::mirror::ArtMethod**>(m1.dex_cache_resolved_methods_);
//
//            art::mirror::ArtMethod m1B = *mArray[m1.dex_method_index_];
//            art::mirror::ArtMethod m1C = *mArray[m2.dex_method_index_];

        }

        static void copy(art::mirror::ArtMethod* from, art::mirror::ArtMethod* to) {
            memcpy(to, from, size);
        }

    };

    Size CastArtMethod::size = 0;
    IMember<art::mirror::ArtMethod, void*>* CastArtMethod::entryPointQuickCompiled = nullptr;
    IMember<art::mirror::ArtMethod, void*>* CastArtMethod::entryPointFormInterpreter = nullptr;
    ArrayMember<art::mirror::ArtMethod>* CastArtMethod::dexCacheResolvedMethods = nullptr;
    IMember<art::mirror::ArtMethod, uint32_t>* CastArtMethod::dexMethodIndex = nullptr;
    IMember<art::mirror::ArtMethod, uint32_t>* CastArtMethod::accessFlag = nullptr;
    Code CastArtMethod::quickToInterpreterBridge = nullptr;

}

#endif //SANDHOOK_CAST_ART_METHOD_H


