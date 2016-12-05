#pragma once

#include "BaseFun.h"
#include "detail/primitives.h"
#include <string>
#include "detail/properties.h"
//#include "metatable.h"

namespace miljs {
template <int N, typename Ret, typename... Args>
class Fun : public BaseFun {
private:
    using _fun_type = std::function<Ret(detail::decay_primitive<Args>...)>;
    _fun_type _fun;

public:
    Fun(duk_context *&ctx,
        _fun_type fun) : _fun(fun) {
		detail::Properties::function_push_with_ptr(ctx, &detail::_js_dispatcher, DUK_VARARGS, (void *)static_cast<BaseFun *>(this));
    }

    // Each application of a function receives a new Lua context so
    // this argument is necessary.
    int Apply(duk_context *ctx) override {
        std::tuple<detail::decay_primitive<Args>...> args =
            detail::_get_args<detail::decay_primitive<Args>...>(ctx);
        detail::_push(ctx, detail::_lift(_fun, args));
        return N;
    }

};

template <typename... Args>
class Fun<0, void, Args...> : public BaseFun {
private:
    using _fun_type = std::function<void(detail::decay_primitive<Args>...)>;
    _fun_type _fun;

public:
    Fun(duk_context *&ctx,
        _fun_type fun) : _fun(fun) {
		detail::Properties::function_push_with_ptr(ctx, &detail::_js_dispatcher, DUK_VARARGS, (void *)static_cast<BaseFun *>(this));
	}

    // Each application of a function receives a new Lua context so
    // this argument is necessary.
    int Apply(duk_context *ctx) {
        std::tuple<detail::decay_primitive<Args>...> args =
            detail::_get_args<detail::decay_primitive<Args>...>(ctx);
        detail::_lift(_fun, args);
        return 0;
    }
};
}
