#pragma once

#include "detail/PrototypeRegistry.h"
#include "detail/properties.h"

namespace seljs2 {
struct BaseClassObj {
    virtual ~BaseClassObj() {}
};
template <typename T>
class ClassObj : public BaseClassObj {
private:

public:
    ClassObj(duk_context *ctx, T *t) {
		duk_push_object(ctx);
		detail::Properties::obj_put_ptr(ctx, -1, t);
		detail::PrototypeRegistry::SetPrototype(ctx, typeid(T));
    }
};
}
