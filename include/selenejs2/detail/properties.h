#pragma once

#include <duktape.h>

namespace seljs2 {
namespace detail {

namespace Properties {

	// push a function with an associated void*, leaves function on the stack
	static inline duk_idx_t function_push_with_ptr(duk_context *ctx, duk_c_function func, duk_idx_t nargs, void *ptr)
	{
		duk_idx_t ret = duk_push_c_function(ctx, func, nargs);
		// push object into hidden property
		duk_push_pointer(ctx, ptr);
		// put into function property
		duk_put_prop_string(ctx, -2, "\xFF" "_func_obj");
		return ret;
	}

	// return ptr of function at index
	static inline void* function_get_ptr(duk_context *ctx, duk_idx_t index)
	{
		duk_get_prop_string(ctx, index, "\xFF" "_func_obj");
		void* ptr = duk_get_pointer(ctx, -1);
		duk_pop(ctx);
		return ptr;
	}

	// put an associated void* to object at index, leaves object on the stack
	static inline void obj_put_ptr(duk_context *ctx, duk_idx_t index, void *ptr)
	{
		duk_idx_t oidx = duk_normalize_index(ctx, index);
		duk_push_pointer(ctx, ptr);
		duk_put_prop_string(ctx, oidx, "\xFF" "_obj");
	}

	// return ptr of object at index
	static inline void* obj_get_ptr(duk_context *ctx, duk_idx_t index)
	{
		duk_get_prop_string(ctx, index, "\xFF" "_obj");
		void* ptr = duk_get_pointer(ctx, -1);
		duk_pop(ctx);
		return ptr;
	}

	// return ptr of object at index
	inline void *obj_require_ptr(duk_context *ctx, duk_idx_t index)
	{
		duk_get_prop_string(ctx, index, "\xFF" "_obj");
		void* ptr = duk_require_pointer(ctx, -1);
		duk_pop(ctx);
		return ptr;
	}

}

} }