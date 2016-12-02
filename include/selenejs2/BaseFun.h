#pragma once

//#include "function.h"
#include <exception>
// #include "ExceptionHandler.h"
#include <functional>
#include "detail/primitives.h"
#include <tuple>
#include "detail/properties.h"
#include "detail/value.h"

namespace seljs2 {
struct BaseFun {
    virtual ~BaseFun() {}
    virtual int Apply(duk_context *ctx) = 0;
};

namespace detail {

inline duk_ret_t _js_dispatcher(duk_context *ctx) {
	duk_push_this(ctx);
	
	// get function ptr
	duk_push_current_function(ctx);
    BaseFun *fun = (BaseFun *)detail::Properties::function_get_ptr(ctx, -1);
	duk_pop(ctx); // pop function

    //_js_check_get raiseParameterConversionError = nullptr;
    const char * wrong_meta_table = nullptr;
    int erroneousParameterIndex = 0;
    try {
        return fun->Apply(ctx);
    /*} catch (GetParameterFromJSTypeError & e) {
        raiseParameterConversionError = e.checked_get;
        erroneousParameterIndex = e.index;
    } catch (GetUserdataParameterFromJSTypeError & e) {
        wrong_meta_table = duk_push_lstring(
            l, e.metatable_name.c_str(), e.metatable_name.length());
        erroneousParameterIndex = e.index;*/
    } catch (std::exception &e) {
		if (duk_is_error(ctx, -1) == 0) {
			duk_push_error_object(ctx, DUK_ERR_ERROR, e.what());
		}
		//store_current_exception(ctx, e.what());
		duk_throw(ctx);
    } catch (...) {
		if (duk_is_error(ctx, -1) == 0)
			throw;
		duk_throw(ctx);
	}
	/*
    if(raiseParameterConversionError) {
        raiseParameterConversionError(l, erroneousParameterIndex);
    }
    else if(wrong_meta_table) {
		// TODO: don't know if this is right
        duvL_checkudata(l, erroneousParameterIndex, wrong_meta_table);
    }
	*/
	return DUK_RET_ERROR;
}

template <typename Ret, typename... Args, std::size_t... N>
inline Ret _lift(std::function<Ret(Args...)> fun,
                 std::tuple<Args...> args,
                 _indices<N...>) {
    return fun(std::get<N>(args)...);
}

template <typename Ret, typename... Args>
inline Ret _lift(std::function<Ret(Args...)> fun,
                 std::tuple<Args...> args) {
    return _lift(fun, args, typename _indices_builder<sizeof...(Args)>::type());
}

template <typename... T, std::size_t... N>
inline std::tuple<T...> _get_args(duk_context *state, _indices<N...>) {
    return std::tuple<T...>{_check_get(detail::_id<T>{}, state, N)...};
}

template <typename... T>
inline std::tuple<T...> _get_args(duk_context *state) {
    constexpr std::size_t num_args = sizeof...(T);
    return _get_args<T...>(state, typename _indices_builder<num_args>::type());
}
}
}
