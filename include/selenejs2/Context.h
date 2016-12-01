#pragma once

#include "BaseContext.h"
#include "Ref.h"
#include "detail/refs.h"

namespace seljs2 {

class RefPushGlobal : public RefPush
{
public:
	void push(BaseContext *ctx) override
	{ 
		duk_push_global_object(ctx->ctx());
	}
};

class Context : public BaseContext
{
public:
	Context() :
		BaseContext(), _ctx(duk_create_heap_default()), _owns_context(true)
	{
		detail::duv_ref_setup(_ctx);
	}

	Context(duk_context *ctx) :
		BaseContext(), _ctx(ctx), _owns_context(false)
	{
		detail::duv_ref_setup(ctx);
	}

	~Context()
	{
		if (_owns_context)
			duk_destroy_heap(_ctx);
		_ctx = nullptr;
	}

	operator duk_context*() const override 
	{
		return _ctx;
	}

	Ref global()
	{
		return Ref(this, std::shared_ptr<RefPush>(new RefPushGlobal));
	}

	Ref operator[](const std::string &name) &
	{
		Ref ret = global();
		return ret[name];

		/*
		ResetStackOnScopeExit r(_ctx);
		duk_push_global_object(_ctx);
		duk_get_prop_string(_ctx, -1, name.c_str());
		return Ref(this);
		*/
	}
private:
	duk_context *_ctx;
	bool _owns_context;
};

}
