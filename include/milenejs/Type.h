#pragma once

#include "ExceptionTypes.h"
#include "detail/value.h"

#include <string>
#include <iostream>
#include <sstream>

namespace miljs {

enum class Type
{
	UNKNOWN,
	NONE,
	UNDEFINED,
	TNULL,
	BOOLEAN,
	NUMBER,
	STRING,
	OBJECT,
	BUFFER,
	POINTER,
	LIGHTFUNC,

	ARRAY,
	FUNCTION
};

inline std::ostream& operator<< (std::ostream &os, Type t)
{
	switch (t)
	{
	case Type::UNKNOWN: return os << "UNKNOWN";
	case Type::NONE: return os << "NONE";
	case Type::UNDEFINED: return os << "UNDEFINED";
	case Type::TNULL: return os << "NULL";
	case Type::BOOLEAN: return os << "BOOLEAN";
	case Type::NUMBER: return os << "NUMBER";
	case Type::STRING: return os << "STRING";
	case Type::OBJECT: return os << "OBJECT";
	case Type::BUFFER: return os << "BUFFER";
	case Type::POINTER: return os << "POINTER";
	case Type::LIGHTFUNC: return os << "LIGHTFUNC";

	case Type::ARRAY: return os << "ARRAY";
	case Type::FUNCTION: return os << "FUNCTION";
	}
	return os << static_cast<std::uint16_t>(t);
}

inline std::string typeToString(Type t)
{
	std::stringstream r;
	r << t;
	return r.str();
}

namespace detail {

template<>
struct value_t<Type> {
	static void push(duk_context *ctx, const Type& value)
	{
		switch (value)
		{
		case Type::UNDEFINED: duk_push_undefined(ctx); break;
		case Type::TNULL: duk_push_null(ctx); break;
		case Type::BOOLEAN: duk_push_boolean(ctx, false); break;
		case Type::NUMBER: duk_push_number(ctx, 0); break;
		case Type::STRING: duk_push_string(ctx, ""); break;
		case Type::OBJECT: duk_push_object(ctx); break;
		case Type::ARRAY: duk_push_array(ctx); break;
		default:
			throw NotSupportedError();
		}
	}
};


}

}