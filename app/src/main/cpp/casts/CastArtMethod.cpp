//
// Created by 甘尧 on 2019/1/12.
//
#include "../includes/ICast.h"

namespace art {
    namespace mirror {
        class Object {
        };

        class Class : public Object {
        };

        class ArtMethod {
        public:
            virtual void SetAccessFlags(uint32_t new_access_flags);
            virtual uint32_t GetAccessFlags();
        };
    }
}

namespace SandHook {

    class CastArtMethod : ICast<art::mirror::ArtMethod> {
    public:
        CastArtMethod(const art::mirror::ArtMethod &t) : ICast(t){};



    };

    class CastEntryPointQuickCompiled : IMember<art::mirror::ArtMethod, void*> {
    public:
        CastEntryPointQuickCompiled(const art::mirror::ArtMethod &p, Size size) : IMember(p, size) {}

    protected:
    private:
        Size calOffset() {
            return 0;
        }
    };

}
