#pragma once

#include "BaseState.h"
#include "refs.h"
#include "value.h"
#include "ResourceHandler.h"

namespace seljs2 {
class Ref {
private:
	BaseState *_state;
	bool _isref;
	int _ref;
public:
	Ref(BaseState *state, bool isref = true) : _state(state), _isref(isref) {
		if (_isref)
			_ref = duv_ref(*_state);
	}
	~Ref() {
		if (_isref)
			duv_unref(*_state, _ref);
	}

	bool isRef() const {
		return _isref;
	}

	void push() const {
		if (_isref)
			duv_push_ref(*_state, _ref);
	}

	template<typename T>
	T get() const {
		ResetStackOnScopeExit r(*_state);
		push();
		return detail::_get<T>(*_state, -1);
	}
};
}