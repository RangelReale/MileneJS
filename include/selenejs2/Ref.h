#pragma once

#include "BaseContext.h"
#include "ResourceHandler.h"
#include "ExceptionTypes.h"
#include "detail/refs.h"
#include "detail/value.h"
#include "detail/refsholder.h"

#include <iostream>
#include <memory>

namespace seljs2 {

class Ref 
{
public:
	Ref(BaseContext *ctx, bool isref = true) :
		_ctx(ctx), _ref()
	{
		if (isref)
			_ref.reset(new detail::refsholder(ctx));
	}

	virtual ~Ref() 
	{
	}

	Ref(const Ref& other) :
		_ctx(other._ctx),
		_ref(other._ref)
	{
	}

	Ref& operator=(const Ref& other)
	{
		_ctx = other._ctx;
		_ref = other._ref;
	}

	Ref(Ref&& other) :
		_ctx(other._ctx), _ref()
	{
		_ref.swap(other._ref);
	}

	Ref& operator=(Ref&& other)
	{
		_ctx = other._ctx;
		_ref.swap(other._ref);
		return *this;
	}

	void push() const
	{
		if (_ref)
			detail::duv_push_ref(ctx()->ctx(), _ref->ref());
		else
			duk_push_undefined(ctx()->ctx());
	}

	template<typename T>
	T get() const 
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		return detail::_get<T>(*_ctx, -1);
	}

	Ref&& operator[](const std::string& name) && 
	{
		ResetStackOnScopeExit r(ctx()->ctx());
		push();
		duk_get_prop_string(ctx()->ctx(), -1, name.c_str());
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

	BaseContext *ctx() const { return _ctx; }
private:
	BaseContext *_ctx;
	std::shared_ptr<detail::refsholder> _ref;
};
}