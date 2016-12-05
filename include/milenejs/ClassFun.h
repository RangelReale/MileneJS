#pragma once

#include "BaseFun.h"
#include <string>
#include "detail/properties.h"

namespace miljs {

template <int N, typename T, typename Ret, typename... Args>
class ClassFun : public BaseFun {
private:
    using _fun_type = std::function<Ret(T*, Args...)>;
    _fun_type _fun;
    std::string _name;

    T *_get(duk_context *ctx) {
		// get obj from "this" bindind
		T *ret = (T *)detail::Properties::obj_get_ptr(ctx, -1);
        return ret;
    }

public:
    ClassFun(duk_context *ctx,
             const std::string &name,
             Ret(*fun)(Args...))
        : ClassFun(ctx, name, _fun_type{fun}) {}

    ClassFun(duk_context *ctx,
             const std::string &name,
             _fun_type fun)
        : _fun(fun), _name(name) {
		detail::Properties::function_push_with_ptr(ctx, &detail::_js_dispatcher, DUK_VARARGS, (void *)static_cast<BaseFun *>(this));
		duk_put_prop_string(ctx, -2, name.c_str());
    }

    int Apply(duk_context *ctx) {
		// "this" is at index -1
        std::tuple<T*> t = std::make_tuple(_get(ctx));
        std::tuple<Args...> args = detail::_get_args<Args...>(ctx);
        std::tuple<T*, Args...> pack = std::tuple_cat(t, args);
        detail::_push(ctx, detail::_lift(_fun, pack));
        return N;
    }
};

template <typename T, typename... Args>
class ClassFun<0, T, void, Args...> : public BaseFun {
private:
    using _fun_type = std::function<void(T*, Args...)>;
    _fun_type _fun;
    std::string _name;

    T *_get(duk_context *ctx) {
		// get obj from "this" binding
		T *ret = (T *)detail::Properties::obj_get_ptr(ctx, -1);
		return ret;
    }

public:
    ClassFun(duk_context *ctx,
             const std::string &name,
             void(*fun)(Args...))
        : ClassFun(ctx, name, _fun_type{fun}) {}

    ClassFun(duk_context *ctx,
             const std::string &name,
             _fun_type fun)
        : _fun(fun), _name(name) {
		detail::Properties::function_push_with_ptr(ctx, &detail::_js_dispatcher, DUK_VARARGS, (void *)static_cast<BaseFun *>(this));
		duk_put_prop_string(ctx, -2, name.c_str());
    }

    int Apply(duk_context *ctx) {
		// "this" is at index -1
		std::tuple<T*> t = std::make_tuple(_get(ctx));
        std::tuple<Args...> args = detail::_get_args<Args...>(ctx);
        std::tuple<T*, Args...> pack = std::tuple_cat(t, args);
        detail::_lift(_fun, pack);
        return 0;
    }
};
}
