#pragma once

#include "value.h"
#include "PrototypeRegistry.h"
#include "properties.h"

#include <duktape.h>

#include <string>

namespace miljs {
namespace detail {

template <typename T>
struct value_t<T*>
{
	static T* get(duk_context *ctx, duk_idx_t index) {
		if (PrototypeRegistry::IsType(ctx, typeid(T), index)) {
			T* ret = (T*)Properties::obj_get_ptr(ctx, index);
			return ret;
		}
		return nullptr;
	}

	static T* require(duk_context *ctx, duk_idx_t index) {
		PrototypeRegistry::CheckType(ctx, typeid(T), index);
		T* ret = (T*)Properties::obj_get_ptr(ctx, index);
		return ret;
	}

	static void push(duk_context *ctx, T* t) {
		if (t == nullptr) {
			duk_push_null(ctx);
		}
		else {
			duk_push_object(ctx);
			Properties::obj_put_ptr(ctx, -1, t);
			PrototypeRegistry::SetPrototype(ctx, typeid(T));
		}
	}

};

template <typename T>
struct value_t
{
	static T& get(duk_context *ctx, duk_idx_t index) {
		if (!PrototypeRegistry::IsType(ctx, typeid(T), index)) {
			throw TypeError{
				PrototypeRegistry::GetTypeName(ctx, typeid(T)),
				PrototypeRegistry::GetTypeName(ctx, index)
			};
		}

		T* ptr = (T*)Properties::obj_get_ptr(ctx, index);
		if (ptr == nullptr) {
			throw TypeError{ PrototypeRegistry::GetTypeName(ctx, typeid(T)) };
		}
		return *ptr;
	}

	static T& require(duk_context *ctx, duk_idx_t index) {
		return get(ctx, index);
	}

	static void push(duk_context *ctx, T& t) {
		duk_push_object(ctx);
		Properties::obj_put_ptr(ctx, -1, &t);
		PrototypeRegistry::SetPrototype(ctx, typeid(T));
	}

	static void push(duk_context *ctx, T&& t) {
		if (!PrototypeRegistry::IsRegisteredType(ctx, typeid(T))) {
			throw CopyUnregisteredType(typeid(t));
		}
		duk_push_object(ctx);
		T* newT = new T(std::forward<T>(t));
		Properties::obj_put_ptr(ctx, -1, newT, true);
		PrototypeRegistry::SetPrototype(ctx, typeid(T));
	}
};

}
}