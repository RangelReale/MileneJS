#include "catch.hpp"
#include <milenejs.h>
#include <string>
#include <vector>

using namespace miljs;

struct Foo {
	int x;
	const int y;
	Foo(int x_) : x(x_), y(3) {}
	int GetX() { return x; }
	int DoubleAdd(int y) {
		return 2 * (x + y);
	}
	void SetX(int x_) {
		x = x_;
	}
};

TEST_CASE("Register obj", "[obj]") {
	Context ctx;

	Foo foo_instance(1);
	ctx.global().SetObj("foo_instance", foo_instance, "double_add", &Foo::DoubleAdd);

	CHECK(ctx["foo_instance"].call("double_add", 3).get<int>() == 8);
}
