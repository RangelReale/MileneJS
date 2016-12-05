#pragma once

#include <duktape.h>

#include <typeinfo>
#include <typeindex>
#include <string>

namespace miljs {
namespace detail {

namespace PrototypeRegistry {
	using TypeID = std::reference_wrapper<const std::type_info>;

	static const char* _prototypes = "milenejs_prototypes";
	static const char* _prototype_name_prop = "\xFF" "__milenejs_prototype_name";

	namespace detail {
		static inline void _push_prototypes_table(duk_context *state) {
			duk_push_heap_stash(state);
			duk_push_string(state, _prototypes);
			duk_get_prop(state, -2);
			duk_remove(state, -2); // stash
		}

		static inline void _push_typeinfo(duk_context *ctx, TypeID type) {
			duk_push_pointer(ctx, const_cast<std::type_info*>(&type.get()));
		}

		static inline void _push_prototype(duk_context *state, TypeID type) {
			detail::_push_prototypes_table(state); // obj
			detail::_push_typeinfo(state, type); // key
			duk_get_prop(state, -2);
			duk_remove(state, -2);
		}
	}

	static inline void Create(duk_context *ctx)
	{
		duk_push_heap_stash(ctx);
		// create prototypes
		duk_push_object(ctx);
		duk_put_prop_string(ctx, -2, _prototypes);
		duk_pop(ctx); // stash
	}

	/**
	 * Creates a new prototype for the type
	 */
	static inline void PushNewPrototype(duk_context *ctx, TypeID type, const std::string &name)
	{
		// create the object that will be the prototype itself
		duk_push_object(ctx);

		// put name in hidden obj[_prototype_name_prop]
		duk_push_lstring(ctx, name.c_str(), name.size());
		duk_put_prop_string(ctx, -2, _prototype_name_prop);

		// register TypeID -> prototype object in "prototypes"
		detail::_push_prototypes_table(ctx); // obj
		detail::_push_typeinfo(ctx, type); // key
		duk_dup(ctx, -3); // val
		duk_put_prop(ctx, -3);
		duk_pop(ctx);
	}

	/**
	 * Sets the prototype for the object at -1, based on the type
	 */
	static inline void SetPrototype(duk_context *ctx, TypeID type)
	{
		// gets the prototype from prototypes[type]
		detail::_push_prototype(ctx, type);

		// sets the prototype of the passed object
		duk_set_prototype(ctx, -2);
	}

	static inline bool IsRegisteredType(duk_context *ctx, TypeID type)
	{
		detail::_push_prototypes_table(ctx); // obj
		detail::_push_typeinfo(ctx, type); // key
		bool registered = duk_has_prop(ctx, -2) != 0;
		duk_pop_2(ctx);
		return registered;
	}

	static inline std::string GetTypeName(duk_context *ctx, TypeID type)
	{
		std::string name("unregistered type");

		detail::_push_prototype(ctx, type);
		if (duk_is_object(ctx, -1)) {
			duk_get_prop_string(ctx, -1, _prototype_name_prop);
			size_t len = 0;
			char const * str = duk_get_lstring(ctx, -1, &len);
			name.assign(str, len);
			duk_pop(ctx);
		}

		return name;
	}

	static inline std::string GetTypeName(duk_context *ctx, duk_idx_t index)
	{
		std::string name("unregistered type");

		duk_get_prototype(ctx, index);
		if (duk_is_object(ctx, -1)) {
			duk_get_prop_string(ctx, -1, _prototype_name_prop);
			if (duk_is_string(ctx, -1))
			{
				size_t len = 0;
				char const * str = duk_get_lstring(ctx, -1, &len);
				name.assign(str, len);
			}
			duk_pop(ctx);
		}
		duk_pop(ctx);
		return name;
	}

	static inline bool IsType(duk_context *ctx, TypeID type, duk_idx_t index)
	{
		bool equal = false;

		duk_get_prototype(ctx, index);
		if (duk_is_object(ctx, -1))
		{
			detail::_push_prototype(ctx, type);
			equal = duk_equals(ctx, -1, -2) != 0;
			duk_pop(ctx);
		}
		duk_pop(ctx);

		return equal;
	}

}

} }