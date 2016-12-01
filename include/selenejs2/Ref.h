#pragma once

#include "BaseContext.h"
#include "ResourceHandler.h"
#include "ExceptionTypes.h"
#include "detail/refs.h"
#include "detail/value.h"

namespace seljs2 {

class Ref 
{
public:
	Ref(BaseContext *ctx) :
		_ctx(ctx)
	{

	}

	virtual ~Ref() {}

	virtual void push() const
	{
		duk_push_undefined(*_ctx);
	}

	template<typename T>
	T get() const {
		ResetStackOnScopeExit r(*_ctx);
		push();
		return detail::_get<T>(*_ctx, -1);
	}

	virtual Ref operator[](const std::string& name) const {
		throw NotImplementedError();
	}

	BaseContext *ctx() const { return _ctx; }
private:
	BaseContext *_ctx;
};
}