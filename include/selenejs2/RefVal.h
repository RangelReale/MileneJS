#pragma once

#include "Ref.h"
#include "detail/refs.h"

namespace seljs2 {

class RefVal : public Ref
{
public:
	RefVal(BaseContext *ctx, bool isref = true) :
		Ref(ctx), _isref(isref)
	{
		if (_isref)
			_ref = detail::duv_ref(ctx->ctx());
	}

	~RefVal()
	{
		if (_isref)
			detail::duv_unref(ctx()->ctx(), _ref);
	}

	RefVal(const RefVal& other) = delete;
	RefVal& operator=(const RefVal & source) = delete;

	RefVal(RefVal&& other) :
		Ref(other.ctx()), _isref(other._isref)
	{
		if (_isref)
			_ref = other._ref;
		other._isref = false;
		other._ref = 0;
	}

	RefVal& operator=(RefVal&& other)
	{
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

	virtual Ref operator[](const std::string& name) const override {
		ResetStackOnScopeExit r(ctx()->ctx());
		push();
		duk_get_prop_string(ctx()->ctx(), -1, name.c_str());
		return std::move(RefVal(ctx()));
	}

private:
	int _ref;
	bool _isref;
};

}