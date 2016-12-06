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

TEST_CASE("Return unregistered type", "[interop]") {
	using namespace test_lifetime;
	Context ctx;
	ctx.global().Register("return_value", &return_value);
	int const instanceCountBeforeCreation = InstanceCounter::instances;
	CHECK_THROWS_AS(ctx("globalValue = return_value();"), CopyUnregisteredType);
}

TEST_CASE("Value parameter", "[interop]") {
	using namespace test_lifetime;
	Context ctx;
	ctx.global().SetClass<InstanceCounter>("MyClass");
	ctx("function acceptValue(value) { valCopy = value; }");
	int const instanceCountBeforeCreation = InstanceCounter::instances;
	
	ctx["acceptValue"](InstanceCounter{});

	CHECK(InstanceCounter::instances == instanceCountBeforeCreation + 1);
}

TEST_CASE("Wrong value parameter", "[interop]") {
	using namespace test_lifetime;
	Context ctx;
	ctx.global().SetClass<InstanceCounter>("MyClass");
	ctx("function acceptValue(value) { valCopy = value; }");
	int const instanceCountBeforeCreation = InstanceCounter::instances;

	try {
		ctx["acceptValue"](Special{});
		CHECK(false); // shouldn't reach here
	}
	catch (CopyUnregisteredType & e)
	{
		CHECK(e.getType().get() == typeid(Special));
	}
}

TEST_CASE("Value parameter keeps type info", "[interop]") {
	using namespace test_lifetime;
	Context ctx;
	ctx.global().SetClass<Special>("MyClass");
	ctx("function acceptValue(value) { valCopy = value; }");
	ctx["acceptValue"](Special{});

	Special *foo = ctx["valCopy"].get<Special*>();

	CHECK(foo != nullptr);
}

TEST_CASE("Callback with value", "[interop]") {
	using namespace test_lifetime;
	Context ctx;
	ctx.global().SetClass<InstanceCounter>("MyClass");
	ctx("val = new MyClass();");

	std::unique_ptr<InstanceCounter> copy;
	ctx.global().Register("accept", [&copy](InstanceCounter counter) {
		copy.reset(new InstanceCounter(std::move(counter)));
	});

	int const instanceCountBeforeCall = InstanceCounter::instances;
	ctx("accept(val);");

	CHECK(InstanceCounter::instances == instanceCountBeforeCall + 1);
}

TEST_CASE("nullptr to null", "[interop]") {
	Context ctx;
	ctx.global().Register("getNullptr", []() -> void* {
		return nullptr;
	});
	ctx("x = getNullptr();");
	ctx("result = x == null;");
	CHECK(ctx["result"].get<bool>() == true);
}

TEST_CASE("Get primitive by value", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	CHECK(ctx["global1"].get<int>() == 5);
}

/*
TEST_CASE("Get primitive by const ref", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	CHECK(ctx["global1"].get<const int &>() == 5);
}

TEST_CASE("Get primitive by rvalue ref", "[interop]") {
	Context ctx;
	ctx.load("../test/test.js");
	CHECK(ctx["global1"].get<int &&>() == 5);
}
*/

TEST_CASE("Call with primitive by value", "[interop]") {
	Context ctx;
	bool success = false;
	auto const accept_int_by_value = [&success](int x) {success = x == 5; };
	ctx.global().Register("test", accept_int_by_value);
	ctx["test"](5);
	CHECK(success == true);
}

TEST_CASE("Call with primitive by const ref", "[interop]") {
	Context ctx;
	bool success = false;
	auto const accept_int_by_const_ref =
		[&success](const int & x) {success = x == 5; };
	ctx.global().Register("test", accept_int_by_const_ref);
	ctx["test"](5);
	CHECK(success == true);
}

TEST_CASE("Call with primitive by rvalue ref", "[interop]") {
	Context ctx;
	bool success = false;
	auto const accept_int_by_rvalue_ref =
		[&success](int && x) {success = x == 5; };
	ctx.global().Register("test", accept_int_by_rvalue_ref);
	ctx["test"](5);
	CHECK(success == true);
}
