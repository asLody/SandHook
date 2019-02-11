//
// Created by swift on 2019/2/3.
//

#include "../includes/cast_art_method.h"
#include "../includes/utils.h"

extern int SDK_INT;

namespace SandHook {

    class CastDexCacheResolvedMethods : public ArrayMember<art::mirror::ArtMethod, void *> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod *p) override {
            if (SDK_INT >= ANDROID_P)
                return getParentSize() + 1;
            int offset = 0;
            Size addr = getAddressFromJava(jniEnv, "com/swift/sandhook/SandHookMethodResolver",
                                           "resolvedMethodsAddress");
            if (addr != 0) {
                offset = findOffset(p, getParentSize(), 2, addr);
                if (offset >= 0) {
                    return static_cast<Size>(offset);
                }
            }
            if (SDK_INT == ANDROID_M) {
                return 4;
            } else if (SDK_INT >= ANDROID_L && SDK_INT <= ANDROID_L2) {
                return 4 * 3;
            }
            return getParentSize() + 1;
        }

    public:
        Size arrayStart(mirror::ArtMethod *parent) override {
            void *p = IMember<mirror::ArtMethod, void *>::get(parent);
            if (SDK_INT <= ANDROID_M) {
                return reinterpret_cast<Size>(p) + 4 * 3;
            } else {
                return reinterpret_cast<Size>(p);
            }
        }

    };

    class CastEntryPointFormInterpreter : public IMember<art::mirror::ArtMethod, void *> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod *p) override {
            if (SDK_INT >= ANDROID_L2 && SDK_INT <= ANDROID_M)
                return getParentSize() - 3 * BYTE_POINT;
            else if (SDK_INT <= ANDROID_L)
                return getParentSize() - 4 * 8 - 4 * 4;
            else
                return getParentSize() + 1;
        }
    };

    class CastEntryPointQuickCompiled : public IMember<art::mirror::ArtMethod, void *> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod *p) override {
            if (SDK_INT >= ANDROID_M) {
                return getParentSize() - BYTE_POINT;
            } else {
                return getParentSize() - 4 - 2 * BYTE_POINT;
            }
        }
    };

    class CastEntryPointFromJni : public IMember<art::mirror::ArtMethod, void *> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod *p) override {
            if (SDK_INT >= ANDROID_L2 && SDK_INT <= ANDROID_N) {
                return getParentSize() - 2 * BYTE_POINT;
            } else {
                return getParentSize() - 8 * 2 - 4 * 4;
            }
        }
    };


    class CastAccessFlag : public IMember<art::mirror::ArtMethod, uint32_t> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod *p) override {
            uint32_t accessFlag = getIntFromJava(jniEnv, "com/swift/sandhook/SandHook",
                                                 "testAccessFlag");
            if (accessFlag == 0) {
                accessFlag = 524313;
            }
            int offset = findOffset(p, getParentSize(), 2, accessFlag);
            if (offset < 0) {
                if (SDK_INT == ANDROID_L2) {
                    return 20;
                } else if (SDK_INT == ANDROID_L) {
                    return 56;
                } else {
                    return getParentSize() + 1;
                }
            } else {
                return static_cast<size_t>(offset);
            }
        }
    };


    class CastDexMethodIndex : public IMember<art::mirror::ArtMethod, uint32_t> {
    protected:
        Size calOffset(JNIEnv *jniEnv, art::mirror::ArtMethod *p) override {
            if (SDK_INT >= ANDROID_P)
                return getParentSize() + 1;
            int offset = 0;
            jint index = getIntFromJava(jniEnv, "com/swift/sandhook/SandHookMethodResolver",
                                        "dexMethodIndex");
            if (index != 0) {
                offset = findOffset(p, getParentSize(), 2, static_cast<uint32_t>(index));
                if (offset >= 0) {
                    return static_cast<Size>(offset);
                }
            }
            return getParentSize() + 1;
        }
    };


    void CastArtMethod::init(JNIEnv *env) {
        //init ArtMethodSize
        jclass sizeTestClass = env->FindClass("com/swift/sandhook/ArtMethodSizeTest");
        Size artMethod1 = (Size) env->GetStaticMethodID(sizeTestClass, "method1", "()V");
        Size artMethod2 = (Size) env->GetStaticMethodID(sizeTestClass, "method2", "()V");

        size = artMethod2 - artMethod1;

        art::mirror::ArtMethod *m1 = reinterpret_cast<art::mirror::ArtMethod *>(artMethod1);
        art::mirror::ArtMethod *m2 = reinterpret_cast<art::mirror::ArtMethod *>(artMethod2);

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
        art::mirror::ArtMethod *neverCall = reinterpret_cast<art::mirror::ArtMethod *>(env->GetMethodID(
                neverCallTestClass, "neverCall", "()V"));
        quickToInterpreterBridge = entryPointQuickCompiled->get(neverCall);

        art::mirror::ArtMethod *neverCallStatic = reinterpret_cast<art::mirror::ArtMethod *>(env->GetStaticMethodID(
                neverCallTestClass, "neverCallStatic", "()V"));
        staticResolveStub = entryPointQuickCompiled->get(neverCallStatic);

        art::mirror::ArtMethod *neverCallNative = reinterpret_cast<art::mirror::ArtMethod *>(env->GetMethodID(
                neverCallTestClass, "neverCallNative", "()V"));
        genericJniStub = entryPointQuickCompiled->get(neverCallNative);

    }

    void CastArtMethod::copy(art::mirror::ArtMethod *from, art::mirror::ArtMethod *to) {
        memcpy(to, from, size);
    }

    Size CastArtMethod::size = 0;
    IMember<art::mirror::ArtMethod, void *> *CastArtMethod::entryPointQuickCompiled = nullptr;
    IMember<art::mirror::ArtMethod, void *> *CastArtMethod::entryPointFormInterpreter = nullptr;
    ArrayMember<art::mirror::ArtMethod, void *> *CastArtMethod::dexCacheResolvedMethods = nullptr;
    IMember<art::mirror::ArtMethod, uint32_t> *CastArtMethod::dexMethodIndex = nullptr;
    IMember<art::mirror::ArtMethod, uint32_t> *CastArtMethod::accessFlag = nullptr;
    void *CastArtMethod::quickToInterpreterBridge = nullptr;
    void *CastArtMethod::genericJniStub = nullptr;
    void *CastArtMethod::staticResolveStub = nullptr;

}