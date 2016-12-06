#include "catch.hpp"
#include "util.h"
#include <milenejs.h>
#include <string>
#include <exception>


using namespace miljs;

TEST_CASE("Catch exception", "[exception]") {
	Context ctx;
	ctx.load("../test/test_exceptions.js");
	ctx.global().Register("throw_logic_error",
		[]() {throw std::logic_error("Message from C++."); });

	std::string msg = ctx["call_protected"]("throw_logic_error").get<std::string>();
	bool ok = ctx["ok"].get<bool>();
	CHECK(!ok);
	CHECK(msg.find("Message from C++.") != std::string::npos);
}

TEST_CASE("Catch unknown exception", "[exception]") {
	Context ctx;
	ctx.load("../test/test_exceptions.js");
	ctx.global().Register("throw_int",
		[]() {throw 0; });

	std::string msg = ctx["call_protected"]("throw_int").get<std::string>();
	bool ok = ctx["ok"].get<bool>();
	CHECK(!ok);
	CHECK(msg.find("invalid c++ exception") != std::string::npos);
}

TEST_CASE("Catch JS exception", "[exception]") {
	Context ctx;
	const char* expected = "Message from JS.";
	ctx.load("../test/test_exceptions.js");
	CHECKMILEXCEPTION_STR(ctx["raise"]("Message from JS."), expected);
}

TEST_CASE("Catch correct exception", "[exception]") {
	Context ctx;
	ctx.global().Register("throw_logic_error",
		[]() {throw std::logic_error("Arbitrary message."); });

	try {
		ctx["throw_logic_error"]();
	}
	catch (std::logic_error &e) {
		CHECK(std::string(e.what()).find("Arbitrary message.") != std::string::npos);
	}
}
