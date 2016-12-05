#pragma once

#include "traits.h"

#include <duktape.h>

#include <type_traits>

namespace miljs {
namespace detail {

template<typename T>
struct value_t;

template <typename T>
using value = value_t<typename std::decay<T>::type>;

template<typename T>
T _get(_id<T> id, duk_context *ctx, duk_idx_t index) {
	return value<T>::get(ctx, index);
}

template<typename T>
T _check_get(_id<T> id, duk_context *ctx, duk_idx_t index) {
	return value<T>::require(ctx, index);
}

template<typename T>
T _cast(_id<T> id, duk_context *ctx, duk_idx_t index) {
	ResetStackOnScopeExit r(ctx);
	duk_dup(ctx, index);
	return value<T>::to(ctx, -1);
}

template <typename T>
T _pop(_id<T> id, duk_context *ctx) {
	T ret = ::miljs::detail::_get<T>(ctx, -1);
	duk_pop(ctx);
	return ret;
}

template <typename T>
void _push(duk_context *ctx, T&& t) {
	value<T>::push(ctx, std::forward<T>(t));
}

inline void _push_n(duk_context *) {}

template <typename T, typename... Rest>
inline void _push_n(duk_context *ctx, T &&value, Rest&&... rest) {
	_push(ctx, std::forward<T>(value));
	_push_n(ctx, std::forward<Rest>(rest)...);
}

template <typename... T, std::size_t... N>
inline void _push_dispatcher(duk_context *ctx,
	const std::tuple<T...> &values,
	_indices<N...>) {
	_push_n(ctx, std::get<N>(values)...);
}

inline void _push(duk_context *, std::tuple<>) {}

template <typename... T>
inline void _push(duk_context *ctx, const std::tuple<T...> &values) {
	constexpr int num_values = sizeof...(T);
	_push_dispatcher(ctx, values,
		typename _indices_builder<num_values>::type());
}

template <typename... T>
inline void _push(duk_context *ctx, std::tuple<T...> &&values) {
	_push(ctx, const_cast<const std::tuple<T...> &>(values));
}

} }