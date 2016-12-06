#include "catch.hpp"
#include "util.h"
#include <milenejs.h>
#include <string>

using namespace miljs;

TEST_CASE("Load error", "[error]") {
	Context ctx;
	const char* expected = "no sourcecode";
	CHECKMILEXCEPTION_STR(ctx.load("../test/non_exist.js"), expected);
}

TEST_CASE("Load syntax error", "[error]") {
	Context ctx;
	const char* expected = "unterminated statement";
	CHECKMILEXCEPTION_STR(ctx.load("../test/test_syntax_error.js"), expected)
}

TEST_CASE("Do syntax error", "[error]") {
	Context ctx;
	const char* expected = "unterminated statement";
	CHECKMILEXCEPTION_STR(ctx(R"(
function syntax_error() {
	1 2 3 4
}
		)"), expected)
}

TEST_CASE("Call undefined function", "[error]") {
	Context ctx;
	const char* expected = "undefined not callable";
	ctx.load("../test/test_error.js");
	CHECKMILEXCEPTION_STR(ctx["undefined_function"](), expected);
}

TEST_CASE("Call undefined function 2", "[error]") {
	Context ctx;
	const char* expected = "identifier 'err_func2' undefined";
	ctx.load("../test/test_error.js");
	CHECKMILEXCEPTION_STR(ctx["err_func1"](1, 2), expected);
}

TEST_CASE("Call stack overflow", "[error]") {
	Context ctx;
	const char* expected = "callstack limit";
	ctx.load("../test/test_error.js");
	CHECKMILEXCEPTION_STR(ctx["do_overflow"](), expected);
}

TEST_CASE("Parameter conversion error", "[error]") {
	Context ctx;
	const char * expected =
		"number required, found 'not a number'";
	std::string largeStringToPreventSSO(50, 'x');
	ctx.global().Register("accept_string_int_string", [](std::string, int, std::string) {});

	CHECKMILEXCEPTION_STR(ctx["accept_string_int_string"](
		largeStringToPreventSSO,
		"not a number",
		largeStringToPreventSSO), expected);
}
