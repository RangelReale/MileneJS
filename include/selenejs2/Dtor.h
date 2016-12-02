#pragma once

#include "BaseFun.h"
#include "detail/properties.h"

namespace seljs2 {

template <typename T>
class Dtor : public BaseFun {
private:
    std::string _metatable_name;
public:
    Dtor(duk_context *ctx,
         const std::string &metatable_name)
        : _metatable_name(metatable_name) {
		detail::Properties::function_push_with_ptr(ctx, &detail::_js_dispatcher, DUK_VARARGS, (void *)static_cast<BaseFun *>(this));
		duk_set_finalizer(ctx, -2);
    }

    int Apply(duk_context *ctx) {
		T *t = (T *)detail::Properties::obj_get_ptr(ctx, 0);
		if (t)
			t->~T();
        return 0;
    }
};

template <typename T>
class DtorNull : public BaseFun {
private:
public:
	DtorNull(duk_context *ctx,
		const std::string &prototype_name)
	{
	}

	int Apply(duk_context *ctx) {
		return 0;
	}
};
}
