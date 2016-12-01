#pragma once

#include "BaseState.h"
#include "Selector.h"
#include "ResourceHandler.h"
#include "Ref.h"

namespace seljs2 {
class State : public BaseState {
public:
	State() : BaseState() {}
	State(duk_context *ctx) : BaseState(ctx) {}

	void push() {
		duk_push_global_object(*this);
	}

	Selector operator[](const char *name) {
		ResetStackOnScopeExit s(*this);
		push();
		duk_get_prop_string(*this, -1, name);
		return Selector(this, Ref(this));
	}

};
}
