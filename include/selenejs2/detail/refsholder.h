#pragma once

#include "../BaseContext.h"
#include "refs.h"

#include <iostream>

namespace seljs2 {
namespace detail {

class refsholder
{
public:
	refsholder(BaseContext *ctx) :
		_ctx(ctx)
	{
		_ref = duv_ref(*_ctx);
		//std::cout << "@ REF " << _ref << std::endl;
	}

	~refsholder()
	{
		//std::cout << "# UREF " << _ref << std::endl;
		duv_unref(*_ctx, _ref);
	}

	int ref() const 
	{
		return _ref;
	}
private:
	BaseContext *_ctx;
	int _ref;
};

} }