#pragma once

#include "ResourceHandler.h"
#include "ExceptionTypes.h"
#include "detail/primitive.h"

#include <duktape.h>

namespace seljs2 {

class BaseContext 
{
public:
	virtual ~BaseContext() {}
	virtual operator duk_context*() const = 0;

	duk_context *ctx() const { return operator duk_context*(); }

	void operator()(const char *code) const
	{
		ResetStackOnScopeExit savedStack(ctx());
		duk_int_t status = duk_peval_string_noresult(ctx(), code);
		if (status != 0) {
			throw SeleneJSException(ErrorMessage(ctx(), -1));
		}
	}

};

}
