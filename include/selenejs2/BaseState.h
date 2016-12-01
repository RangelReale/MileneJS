#pragma once

#include <duktape.h>

#include "refs.h"

namespace seljs2 {
class BaseState {
private:
	duk_context *_l;
	bool _l_owner;

public:
	BaseState() : _l(nullptr), _l_owner(true) {
		_l = duk_create_heap_default();
		if (_l == nullptr) throw 0;
		duv_ref_setup(_l);
	}
	BaseState(duk_context *l) : _l(l), _l_owner(false) {
		duv_ref_setup(_l);
	}
	virtual ~BaseState() {
		if (_l != nullptr && _l_owner) {
			//ForceGC();
			duk_destroy_heap(_l);
		}
		_l = nullptr;
	}

	// implicit conversion operator
	operator duk_context*() {
		return _l;
	}

	virtual void push() = 0;
};
}
