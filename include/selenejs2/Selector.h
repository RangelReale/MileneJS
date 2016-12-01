#pragma once

#include "BaseState.h"
#include "Ref.h"

namespace seljs2 {
class Selector {
private:
	BaseState *_ctx;
	Ref _ref;

public:
	Selector(BaseState *ctx, Ref ref) : _ctx(ctx), _ref(ref) {}
};

}