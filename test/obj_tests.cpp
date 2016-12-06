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
	ctx.global().SetClassBare<Foo>("Foo", "double_add", &Foo::DoubleAdd);
	ctx.global().SetClassObj("foo_instance", foo_instance);

	CHECK(ctx["foo_instance"].call("double_add", 3).get<int>() == 8);
}

TEST_CASE("Register obj member variable", "[obj]") {
	Context ctx;

	Foo foo_instance(1);
	ctx.global().SetClassBare<Foo>("Foo", "x", &Foo::x);
	ctx.global().SetClassObj("foo_instance", foo_instance);
	ctx["foo_instance"].call("set_x", 3);

	CHECK(ctx["foo_instance"].call("x").get<int>() == 3);
}

TEST_CASE("Register obj to array", "[obj]") {
	Context ctx;

	Foo foo1(1);
	Foo foo2(2);
	Foo foo3(3);

	ctx.global().SetClassBare<Foo>("Foo", "get_x", &Foo::GetX);

	auto foos = ctx.setRef("foos", Type::ARRAY);
	foos.SetClassObj(0, foo1);
	foos.SetClassObj(1, foo2);
	foos.SetClassObj(2, foo3);
	const int answer = foos[0].call("get_x").get<int>() +
		foos[1].call("get_x").get<int>() +
		foos[2].call("get_x").get<int>();
	CHECK(answer == 6);
}
