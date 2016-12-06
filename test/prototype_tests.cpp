#include "catch.hpp"
#include "util.h"
#include <milenejs.h>
#include <string>

using namespace miljs;

struct Qux {
	int baz() { return 4; }
	int qux = 3;
};

static Qux qux;

Qux *GetQuxPtr() { return &qux; }
Qux &GetQuxRef() { return qux; }

TEST_CASE("Prototype registry ptr", "[prototype]") {
	Context ctx;
	ctx.global().Register("get_instance", &GetQuxPtr);
	ctx.global().SetClass<Qux>("Qux", "baz", &Qux::baz);
	ctx.load("../test/test_prototype.js");
	CHECK(ctx["call_method"]().get<int>() == 4);
}

TEST_CASE("Prototype registry ref", "[prototype]") {
	Context ctx;
	ctx.global().Register("get_instance", &GetQuxRef);
	ctx.global().SetClass<Qux>("Qux", "baz", &Qux::baz);
	ctx.load("../test/test_prototype.js");
	CHECK(ctx["call_method"]().get<int>() == 4);
}

TEST_CASE("Prototype ptr member", "[prototype]") {
	Context ctx;
	ctx.global().Register("get_instance", &GetQuxPtr);
	ctx.global().SetClass<Qux>("Qux", "baz", &Qux::baz, "qux", &Qux::qux);
	ctx.load("../test/test_prototype.js");
	CHECK(ctx["access_member"]().get<int>() == 3);
}

TEST_CASE("Prototype ref member", "[prototype]") {
	Context ctx;
	ctx.global().Register("get_instance", &GetQuxRef);
	ctx.global().SetClass<Qux>("Qux", "baz", &Qux::baz, "qux", &Qux::qux);
	ctx.load("../test/test_prototype.js");
	CHECK(ctx["access_member"]().get<int>() == 3);
}
