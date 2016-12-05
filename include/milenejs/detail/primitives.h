#pragma once

#include "value.h"

#include <duktape.h>

#include <cstdint>
#include <string>

namespace miljs {
namespace detail {

template <typename T>
struct is_primitive {
    static constexpr bool value = false;
};
template <>
struct is_primitive<int> {
    static constexpr bool value = true;
};
template <>
struct is_primitive<unsigned int> {
    static constexpr bool value = true;
};
template <>
struct is_primitive<bool> {
    static constexpr bool value = true;
};
template <>
struct is_primitive<duk_double_t> {
    static constexpr bool value = true;
};
template <>
struct is_primitive<std::string> {
    static constexpr bool value = true;
};

template<typename T>
using decay_primitive =
    typename std::conditional<
        is_primitive<typename std::decay<T>::type>::value,
        typename std::decay<T>::type,
        T
    >::type;

/*
template <typename T>
struct _valuer
{
	static bool is(duk_context *ctx, duk_idx_t index);
	static T require(duk_context *ctx, duk_idx_t index);
	static T get(duk_context *ctx, duk_idx_t index);
	static T to(duk_context *ctx, duk_idx_t index);
	static void push(duk_context *ctx, const T& value);
};
*/

template<>
struct value_t<bool>
{
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_boolean(ctx, index) != 0); }
	static bool require(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_require_boolean(ctx, index) != 0); }
	static bool get(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_get_boolean(ctx, index) != 0); }
	static bool to(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_to_boolean(ctx, index) != 0); }
	static void push(duk_context *ctx, const bool& value) { duk_push_boolean(ctx, value); }
};

template<>
struct value_t<const char *> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_string(ctx, index) != 0); }
	static const char* require(duk_context *ctx, duk_idx_t index) { return duk_require_string(ctx, index); }
	static const char* get(duk_context *ctx, duk_idx_t index) { return duk_get_string(ctx, index); }
	static const char* to(duk_context *ctx, duk_idx_t index) { return duk_safe_to_string(ctx, index); }
	static void push(duk_context *ctx, const char* const& value) { duk_push_string(ctx, value); }
};

template<>
struct value_t<uint8_t> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
	static uint8_t require(duk_context *ctx, duk_idx_t index) { return static_cast<uint8_t>(duk_require_uint(ctx, index)); }
	static uint8_t get(duk_context *ctx, duk_idx_t index) { return static_cast<uint8_t>(duk_get_uint(ctx, index)); }
	static uint8_t to(duk_context *ctx, duk_idx_t index) { return static_cast<uint8_t>(duk_to_uint(ctx, index)); }
	static void push(duk_context *ctx, const uint8_t& value) { duk_push_uint(ctx, value); }
};

template<>
struct value_t<uint16_t> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
	static uint16_t require(duk_context *ctx, duk_idx_t index) { return static_cast<uint16_t>(duk_require_uint(ctx, index)); }
	static uint16_t get(duk_context *ctx, duk_idx_t index) { return static_cast<uint16_t>(duk_get_uint(ctx, index)); }
	static uint16_t to(duk_context *ctx, duk_idx_t index) { return static_cast<uint16_t>(duk_to_uint(ctx, index)); }
	static void push(duk_context *ctx, const uint16_t& value) { duk_push_uint(ctx, value); }
};

template<>
struct value_t<uint32_t> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
	static uint32_t require(duk_context *ctx, duk_idx_t index) { return static_cast<uint32_t>(duk_require_uint(ctx, index)); }
	static uint32_t get(duk_context *ctx, duk_idx_t index) { return static_cast<uint32_t>(duk_get_uint(ctx, index)); }
	static uint32_t to(duk_context *ctx, duk_idx_t index) { return static_cast<uint32_t>(duk_to_uint(ctx, index)); }
	static void push(duk_context *ctx, const uint32_t& value) { duk_push_uint(ctx, value); }
};

/*
template<>
struct value_t<uint64_t> {
static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
static uint64_t require(duk_context *ctx, duk_idx_t index) { return static_cast<uint64_t>(duk_require_uint(ctx, index)); }
static uint64_t get(duk_context *ctx, duk_idx_t index) { return static_cast<uint64_t>(duk_get_uint(ctx, index)); }
static uint64_t to(duk_context *ctx, duk_idx_t index) { return static_cast<uint64_t>(duk_to_uint(ctx, index)); }
static void push(duk_context *ctx, const uint64_t& value) { duk_push_uint(ctx, value); }
};
*/

template<>
struct value_t<int8_t> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
	static int8_t require(duk_context *ctx, duk_idx_t index) { return static_cast<int8_t>(duk_require_int(ctx, index)); }
	static int8_t get(duk_context *ctx, duk_idx_t index) { return static_cast<int8_t>(duk_get_int(ctx, index)); }
	static int8_t to(duk_context *ctx, duk_idx_t index) { return static_cast<int8_t>(duk_to_int(ctx, index)); }
	static void push(duk_context *ctx, const int8_t& value) { duk_push_int(ctx, value); }
};

template<>
struct value_t<int16_t> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
	static int16_t require(duk_context *ctx, duk_idx_t index) { return static_cast<int16_t>(duk_require_int(ctx, index)); }
	static int16_t get(duk_context *ctx, duk_idx_t index) { return static_cast<int16_t>(duk_get_int(ctx, index)); }
	static int16_t to(duk_context *ctx, duk_idx_t index) { return static_cast<int16_t>(duk_to_int(ctx, index)); }
	static void push(duk_context *ctx, const int16_t& value) { duk_push_int(ctx, value); }
};

template<>
struct value_t<int32_t> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
	static int32_t require(duk_context *ctx, duk_idx_t index) { return static_cast<int32_t>(duk_require_int(ctx, index)); }
	static int32_t get(duk_context *ctx, duk_idx_t index) { return static_cast<int32_t>(duk_get_int(ctx, index)); }
	static int32_t to(duk_context *ctx, duk_idx_t index) { return static_cast<int32_t>(duk_to_int(ctx, index)); }
	static void push(duk_context *ctx, const int32_t& value) { duk_push_int(ctx, value); }
};

/*
template<>
struct value_t<int64_t> {
static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
static int64_t require(duk_context *ctx, duk_idx_t index) { return static_cast<int64_t>(duk_require_int(ctx, index)); }
static int64_t get(duk_context *ctx, duk_idx_t index) { return static_cast<int64_t>(duk_get_int(ctx, index)); }
static int64_t to(duk_context *ctx, duk_idx_t index) { return static_cast<int64_t>(duk_to_int(ctx, index)); }
static void push(duk_context *ctx, const int64_t& value) { duk_push_int(ctx, value); }
};
*/

template<>
struct value_t<float> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
	static float require(duk_context *ctx, duk_idx_t index) { return static_cast<float>(duk_require_number(ctx, index)); }
	static float get(duk_context *ctx, duk_idx_t index) { return static_cast<float>(duk_get_number(ctx, index)); }
	static float to(duk_context *ctx, duk_idx_t index) { return static_cast<float>(duk_to_number(ctx, index)); }
	static void push(duk_context *ctx, const float& value) { duk_push_number(ctx, value); }
};

template<>
struct value_t<double> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(ctx, index) != 0); }
	static double require(duk_context *ctx, duk_idx_t index) { return static_cast<double>(duk_require_number(ctx, index)); }
	static double get(duk_context *ctx, duk_idx_t index) { return static_cast<double>(duk_get_number(ctx, index)); }
	static double to(duk_context *ctx, duk_idx_t index) { return static_cast<double>(duk_to_number(ctx, index)); }
	static void push(duk_context *ctx, const double& value) { duk_push_number(ctx, value); }
};

/*
template<>
struct value_t<void*> {
static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_pointer(ctx, index) != 0); }
static void* require(duk_context *ctx, duk_idx_t index) { return (duk_require_pointer(ctx, index)); }
static void* get(duk_context *ctx, duk_idx_t index) { return (duk_get_pointer(ctx, index)); }
static void to(duk_context *ctx, duk_idx_t index) { (duk_to_pointer(ctx, index)); }
static void push(duk_context *ctx, const (void*)& value) { duk_push_pointer(ctx, value); }
};
*/

template<>
struct value_t<std::string> {
	static bool is(duk_context *ctx, duk_idx_t index) { return static_cast<bool>(duk_is_string(ctx, index) != 0); }
	static std::string require(duk_context *ctx, duk_idx_t index) { return std::string(duk_require_string(ctx, index)); }
	static std::string get(duk_context *ctx, duk_idx_t index) {
		duk_size_t len;
		const char* ret = duk_get_lstring(ctx, index, &len);
		if (!ret)
			return std::string();
		return std::string(ret, len);
	}
	static std::string to(duk_context *ctx, duk_idx_t index) { return std::string(duk_safe_to_string(ctx, index)); }
	static void push(duk_context *ctx, const std::string& value) { duk_push_string(ctx, value.c_str()); }
};

}
}