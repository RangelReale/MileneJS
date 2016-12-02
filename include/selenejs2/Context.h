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
		_registry.reset(new Registry(_ctx));
	}

	Context(duk_context *ctx) :
		BaseContext(), _ctx(ctx), _owns_context(false)
	{
		detail::duv_ref_setup(ctx);
		_registry.reset(new Registry(_ctx));
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
	}

	template<typename T>
	void set(const std::string &name, T&& value)
	{
		global().set(name, std::forward<T>(value));
	}

	template<typename T>
	Ref setRef(const std::string &name, T&& value)
	{
		return global().setRef(name, std::forward<T>(value));
	}


	Registry &registry() override
	{
		return *_registry;
	}

private:
	duk_context *_ctx;
	bool _owns_context;
	std::unique_ptr<Registry> _registry;
};

}
