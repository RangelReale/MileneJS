#pragma once

#include "BaseFun.h"
#include "detail/properties.h"

namespace seljs2 {

template <typename T, typename... Args>
class Ctor : public BaseFun {
private:
    using _ctor_type = std::function<void(duk_context *, Args...)>;
    _ctor_type _ctor;

public:
    Ctor(duk_context *ctx,
         const std::string &prototype_name)
         : _ctor([prototype_name](duk_context *ctx, Args... args) {
			void *addr = static_cast<void*>(new T(args...));
			detail::Properties::obj_push_with_ptr(ctx, addr);
			// prototype will be inherited automatically
           }) {
		detail::Properties::function_push_with_ptr(ctx, &detail::_js_dispatcher, DUK_VARARGS, (void *)static_cast<BaseFun *>(this));
		duk_put_prop_string(ctx, -2, "\xFF" "constructor"); // set an internal constructor property
    }

    int Apply(duk_context *ctx) {
        std::tuple<Args...> args = detail::_get_args<Args...>(ctx);
        auto pack = std::tuple_cat(std::make_tuple(ctx), args);
        detail::_lift(_ctor, pack);
        // The constructor will leave a single userdata entry on the stack
        return 1;
    }
};

template <typename T>
class CtorNull : public BaseFun {
private:

public:
	CtorNull(duk_context *ctx,
		const std::string &prototype_name)
	{
	}

	int Apply(duk_context *ctx) {
		return 0;
	}
};
}