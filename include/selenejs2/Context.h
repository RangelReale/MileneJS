#pragma once

#include "BaseContext.h"
#include "RefVal.h"
#include "detail/refs.h"

namespace seljs2 {

class Context : public BaseContext, public RefVal
{
public:
	Context() :
		BaseContext(), RefVal(this, false), _ctx(duk_create_heap_default()), _owns_context(true)
	{
		detail::duv_ref_setup(_ctx);
	}

	Context(duk_context *ctx) :
		BaseContext(), RefVal(this, false), _ctx(ctx), _owns_context(false)
	{
		detail::duv_ref_setup(ctx);
	}

	~Context()
	{
		if (_owns_context)
			duk_destroy_heap(_ctx);
		_ctx = nullptr;
	}

	void push() const override
	{
		duk_push_global_object(*this);
	}

	operator duk_context*() const override 
	{
		return _ctx;
	}
private:
	duk_context *_ctx;
	bool _owns_context;
};

}
