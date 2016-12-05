#pragma once

#include <duktape.h>

#include <exception>

namespace miljs {
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
	static inline void obj_put_ptr(duk_context *ctx, duk_idx_t index, void *ptr, bool owns = false)
	{
		duk_idx_t oidx = duk_normalize_index(ctx, index);
		duk_push_pointer(ctx, ptr);
		duk_put_prop_string(ctx, oidx, "\xFF" "_obj");
		if (owns)
		{
			duk_push_true(ctx);
			duk_put_prop_string(ctx, oidx, "\xFF" "_obj_owns");
		}
	}

	// return ptr of object at index
	static inline void* obj_get_ptr(duk_context *ctx, duk_idx_t index, bool onlyowned = false)
	{
		duk_idx_t oidx = duk_normalize_index(ctx, index);
		duk_get_prop_string(ctx, oidx, "\xFF" "_obj");
		void* ptr = duk_get_pointer(ctx, -1);
		duk_pop(ctx);
		if (onlyowned)
		{
			duk_get_prop_string(ctx, oidx, "\xFF" "_obj_owns");
			bool isowned = duk_is_boolean(ctx, -1) && duk_get_boolean(ctx, -1) != 0;
			duk_pop(ctx);

			if (!isowned)
				return false;
		}
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

	struct error_exception
	{
		std::exception_ptr exception;
	};

	// error finalizer
	inline duk_ret_t error_finalizer(duk_context *ctx)
	{
		duk_get_prop_string(ctx, 0, "\xFF" "_exception");
		if (duk_is_pointer(ctx, -1))
		{
			error_exception *ee = static_cast<error_exception *>(duk_get_pointer(ctx, -1));
			delete ee;
			duk_del_prop_string(ctx, 0, "\xFF" "_exception");
		}
		duk_pop(ctx);

		return 0;
	}

	// store exception into error object
	inline void error_put_exception(duk_context *ctx, duk_idx_t index, std::exception_ptr e)
	{
		duk_idx_t eidx = duk_normalize_index(ctx, index);

		error_exception* ee = new error_exception{ e };
		duk_push_pointer(ctx, static_cast<void*>(ee));
		duk_put_prop_string(ctx, eidx, "\xFF" "_exception");
		duk_push_c_function(ctx, &error_finalizer, 1);
		duk_set_finalizer(ctx, eidx);
	}

	// get exception from error object
	inline void error_throw_exception(duk_context *ctx, duk_idx_t index)
	{
		duk_get_prop_string(ctx, index, "\xFF" "_exception");
		if (duk_is_pointer(ctx, -1))
		{
			error_exception *ee = static_cast<error_exception *>(duk_get_pointer(ctx, -1));
			std::rethrow_exception(ee->exception);
		}
		duk_pop(ctx);
	}

}

} }