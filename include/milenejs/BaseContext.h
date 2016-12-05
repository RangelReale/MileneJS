#pragma once

#include "ResourceHandler.h"
#include "ExceptionTypes.h"
#include "detail/primitives.h"
#include "Registry.h"

#include <duktape.h>

#include <memory>

namespace miljs {

class BaseContext 
{
public:
	virtual ~BaseContext() {}
	virtual operator duk_context*() const = 0;

	duk_context *ctx() const { return operator duk_context*(); }
	virtual Registry &registry() = 0;

	void operator()(const char *code) const
	{
		ResetStackOnScopeExit savedStack(ctx());
		duk_int_t status = duk_peval_string(ctx(), code);
		if (status != 0) {
			ThrowError(ctx(), -1);
		}
		duk_pop(ctx());
	}

	void load(const char *filename) const
	{
		ResetStackOnScopeExit savedStack(ctx());
		duk_int_t status = duk_peval_file(ctx(), filename);
		if (status != 0) {
			 ThrowError(ctx(), -1);
		}
		duk_pop(ctx());
	}

	void forceGC() const 
	{
		duk_gc(ctx(), 0);
	}
private:
	std::unique_ptr<Registry> _registry;
};

}
