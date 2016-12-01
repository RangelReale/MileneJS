#pragma once

#include "BaseContext.h"
#include "ResourceHandler.h"
#include "ExceptionTypes.h"
#include "Type.h"
#include "detail/refs.h"
#include "detail/value.h"
#include "detail/refsholder.h"

#include <iostream>
#include <memory>

namespace seljs2 {

class RefPush
{
public:
	virtual ~RefPush() {}
	virtual void push(BaseContext *ctx) = 0;
};

class Ref 
{
public:
	Ref(BaseContext *ctx, std::shared_ptr<RefPush> refpush = std::shared_ptr<RefPush>()) :
		_ctx(ctx), _refpush(refpush), _ref()
	{
		if (!_refpush)
			_ref.reset(new detail::refsholder(ctx));
	}

	virtual ~Ref() 
	{
	}

	Ref(const Ref& other) :
		_ctx(other._ctx),
		_refpush(other._refpush),
		_ref(other._ref)
	{
	}

	Ref& operator=(const Ref& other)
	{
		_ctx = other._ctx;
		_refpush = other._refpush;
		_ref = other._ref;
	}

	Ref(Ref&& other) :
		_ctx(other._ctx), _refpush(other._refpush), _ref()
	{
		_ref.swap(other._ref);
	}

	Ref& operator=(Ref&& other)
	{
		_ctx = other._ctx;
		_refpush = other._refpush;
		_ref.swap(other._ref);
		return *this;
	}

	Type type() const
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		duk_int_t t = duk_get_type(*_ctx, -1);
		switch (t)
		{
		case DUK_TYPE_NONE: return Type::NONE;
		case DUK_TYPE_UNDEFINED: return Type::UNDEFINED;
		case DUK_TYPE_NULL: return Type::TNULL;
		case DUK_TYPE_BOOLEAN: return Type::BOOLEAN;
		case DUK_TYPE_NUMBER: return Type::NUMBER;
		case DUK_TYPE_STRING: return Type::STRING;
		case DUK_TYPE_BUFFER: return Type::BUFFER;
		case DUK_TYPE_POINTER: return Type::POINTER;
		case DUK_TYPE_LIGHTFUNC: return Type::LIGHTFUNC;
		case DUK_TYPE_OBJECT: {
			if (duk_is_array(*_ctx, -1)) return Type::ARRAY;
			if (duk_is_function(*_ctx, -1)) return Type::FUNCTION;
			return Type::OBJECT;
		};
		}
		return Type::UNKNOWN;
	}

	void push() const
	{
		if (_refpush) 
		{
			_refpush->push(_ctx);
		}
		else
		{
			if (_ref)
				detail::duv_push_ref(ctx()->ctx(), _ref->ref());
			else
				duk_push_undefined(ctx()->ctx());
		}
	}

	template<typename T>
	T get() const 
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		return detail::_get<T>(*_ctx, -1);
	}

	template<typename T>
	void set(const std::string &name, T&& value)
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		detail::_push<T>(*_ctx, std::forward<T>(value));
		duk_put_prop_string(*_ctx, -2, name.c_str());
	}

	template<typename T>
	void set(const int index, T&& value)
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		detail::_push<T>(*_ctx, std::forward<T>(value));
		duk_put_prop_index(*_ctx, -2, index);
	}

	template<typename T>
	Ref setRef(const std::string &name, T&& value)
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		detail::_push<T>(*_ctx, std::forward<T>(value));
		duk_put_prop_string(*_ctx, -2, name.c_str());
		return (*this)[name];
	}

	template<typename T>
	Ref setRef(const int index, T&& value)
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		detail::_push<T>(*_ctx, std::forward<T>(value));
		duk_put_prop_index(*_ctx, -2, index);
		return (*this)[index];
	}

	Ref&& operator[](const std::string& name) && 
	{
		ResetStackOnScopeExit r(ctx()->ctx());
		push();
		duk_get_prop_string(ctx()->ctx(), -1, name.c_str());
		_refpush.reset();
		_ref.reset(new detail::refsholder(_ctx));
		return std::move(*this);
	}

	Ref&& operator[](const int index) &&
	{
		ResetStackOnScopeExit r(ctx()->ctx());
		push();
		duk_get_prop_index(ctx()->ctx(), -1, index);
		_refpush.reset();
		_ref.reset(new detail::refsholder(_ctx));
		return std::move(*this);
	}

	Ref operator[](const std::string &name) &
	{
		ResetStackOnScopeExit r(ctx()->ctx());
		push();
		duk_get_prop_string(ctx()->ctx(), -1, name.c_str());
		return Ref(_ctx);
	}

	Ref operator[](const int index) &
	{
		ResetStackOnScopeExit r(ctx()->ctx());
		push();
		duk_get_prop_index(ctx()->ctx(), -1, index);
		return Ref(_ctx);
	}

	BaseContext *ctx() const { return _ctx; }
private:
	BaseContext *_ctx;
	std::shared_ptr<RefPush> _refpush;
	std::shared_ptr<detail::refsholder> _ref;
};
}