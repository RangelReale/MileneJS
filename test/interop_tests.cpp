#include "catch.hpp"
#include "util.h"
#include "common/lifetime.h"
#include <milenejs.h>
#include <string>

using namespace miljs;

int my_add(int a, int b) {
	return a + b;
}

void no_return() {
}

TEST_CASE("Function no args", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	CHECK_NOTHROW(ctx["foo"]());
}

TEST_CASE("Add", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	CHECK(ctx["add"](5, 2).get<int>() == 7);
}

TEST_CASE("Call field", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	CHECK(ctx["mytable"]["foo"]().get<int>() == 4);
}

TEST_CASE("Call C function", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	ctx.global().Register("cadd", std::function<int(int, int)>(my_add));
	CHECK(ctx["cadd"](3, 6).get<int>() == 9);
}

TEST_CASE("Call C function from JS", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	ctx.global().Register("cadd", std::function<int(int, int)>(my_add));
	CHECK(ctx["execute"]().get<int>() == 11);
}

TEST_CASE("No return", "[interop]") {
	Context ctx;
	ctx.global().Register("no_return", &no_return);
	CHECK_NOTHROW(ctx["no_return"]());
}

TEST_CASE("Call std function", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	std::function<int(int, int)> mult = [](int x, int y) { return x * y; };
	ctx.global().Register("cmultiply", mult);
	CHECK(ctx["cmultiply"](5, 6).get<int>() == 30);
}

TEST_CASE("Call lambda", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	ctx.global().Register("cmultiply", [](int x, int y) { return x * y; });
	CHECK(ctx["cmultiply"](5, 6).get<int>() == 30);
}

TEST_CASE("Call normal C function", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	ctx.global().Register("cadd", &my_add);
	CHECK(ctx["cadd"](4, 20).get<int>() == 24);
}

TEST_CASE("Call normal C function many times", "[interop]") {
	// Ensures there isn't any strange overflow problem or lingering
	// state
	Context ctx;
	ctx.load("../test/test.js");
	ctx.global().Register("cadd", &my_add);
	bool result = true;
	for (int i = 0; i < 25; ++i) {
		const int answer = ctx["cadd"](4, 20).get<int>();
		result = result && (answer == 24);
	}
	CHECK(result == true);
}

TEST_CASE("Call functor", "[interop]") {
	Context ctx;
	struct the_answer {
		int answer = 42;
		int operator()() {
			return answer;
		}
	};
	the_answer functor;
	ctx.load("../test/test.js");
	ctx.global().Register("c_the_answer", std::function<int()>(functor));
	CHECK(ctx["c_the_answer"]().get<int>() == 42);
}

TEST_CASE("Embedded nulls", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	const std::string result = ctx["embedded_nulls"]().get<std::string>();
	CHECK(result.size() == 4);
}

struct Special {
	int foo = 3;
};

static Special special;

Special* return_special_pointer() { return &special; }

TEST_CASE("Pointer return", "[interop]") {
	Context ctx;
	ctx.global().Register("return_special_pointer", &return_special_pointer);
	CHECK(ctx["return_special_pointer"]().get<Special*>() == &special);
}

Special& return_special_reference() { return special; }

TEST_CASE("Reference return", "[interop]") {
	Context ctx;
	ctx.global().Register("return_special_reference", &return_special_reference);
	Special &ref = ctx["return_special_reference"]().get<Special&>();
	CHECK(&ref == &special);
}

test_lifetime::InstanceCounter return_value() { return {}; }

TEST_CASE("Return value", "[interop]") {
	using namespace test_lifetime;
	Context ctx;
	ctx.global().SetClass<InstanceCounter>("MyClass");
	ctx.global().Register("return_value", &return_value);
	int const instanceCountBeforeCreation = InstanceCounter::instances;
	ctx("globalValue = return_value();");
	CHECK(InstanceCounter::instances == instanceCountBeforeCreation + 1);
}
