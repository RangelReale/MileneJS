#pragma once

#include "traits.h"

#include <duktape.h>

#include <type_traits>

namespace seljs2 {

namespace detail {

template<typename T>
struct value_t;

template <typename T>
using value = value_t<typename std::decay<T>::type>;

/*
template<typename T>
T _get(_id<T> id, duk_context *ctx, duk_idx_t index) {
	return value<T>::_get(ctx, index);
}

template <typename T>
T _pop(_id<T> id, duk_context *ctx) {
	T ret = ::seljs::detail::_get(id, ctx, -1);
	duk_pop(l);
	return ret;
}

template <typename T>
void _push(duk_context *ctx, T&& t) {
	value<T>::push(ctx, std::forward<T>(t));
}
*/

template<typename T>
T _get(duk_context *ctx, duk_idx_t index) {
	return value<T>::get(ctx, index);
}

template <typename T>
T _pop(duk_context *ctx) {
	T ret = ::seljs2::detail::_get<T>(ctx, -1);
	duk_pop(ctx);
	return ret;
}

template <typename T>
void _push(duk_context *ctx, T&& t) {
	value<T>::push(ctx, std::forward<T>(t));
}

}
}