#pragma once

#include "BaseContext.h"
#include "ResourceHandler.h"
#include "ExceptionTypes.h"
#include "detail/refs.h"
#include "detail/value.h"

#include <iostream>

namespace seljs2 {

class Ref 
{
public:
	Ref(BaseContext *ctx, bool isref = true) :
		_ctx(ctx), _isref(isref)
	{
		if (_isref)
			_ref = detail::duv_ref(ctx->ctx());
	}

	virtual ~Ref() 
	{
		if (_isref)
			detail::duv_unref(ctx()->ctx(), _ref);
	}

	Ref(const Ref& other) :
		_ctx(other._ctx),
		_isref(other._isref)
	{
		if (_isref)
		{
			detail::duv_push_ref(_ctx->ctx(), other._ref);
			_ref = detail::duv_ref(_ctx->ctx());
		}
	}

	Ref& operator=(const Ref& other)
	{
		_ctx = other._ctx;
		_isref = other._isref;
		if (_isref)
		{
			detail::duv_push_ref(_ctx->ctx(), other._ref);
			_ref = detail::duv_ref(_ctx->ctx());
		}
	}

	Ref(Ref&& other) :
		_ctx(other._ctx),
		_isref(other._isref)
	{
		if (_isref)
			_ref = other._ref;
		other._isref = false;
		other._ref = 0;
	}

	Ref& operator=(Ref&& other)
	{
		_ctx = other._ctx;
		_isref = other._isref;
		if (_isref)
			_ref = other._ref;
		other._isref = false;
		other._ref = 0;
		return *this;
	}

	void push() const
	{
		if (_isref)
			detail::duv_push_ref(ctx()->ctx(), _ref);
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
		if (_isref)
			detail::duv_unref(ctx()->ctx(), _ref);
		_isref = true;
		_ref = detail::duv_ref(ctx()->ctx());
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
	int _ref;
	bool _isref;
};
}