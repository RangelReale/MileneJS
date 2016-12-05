#include "catch.hpp"

#include "util.h"

#include <milenejs.h>

#include <string>

using namespace miljs;

TEST_CASE("Load error", "[error]") {
	Context ctx;
	const char* expected = "no sourcecode";
	try {
		ctx.load("../test/non_exist.js");
	}
	catch (MileneJSException &e) {
		CHECK(std::string(e.what()).find(expected) != std::string::npos);
	}
}

TEST_CASE("Load syntax error", "[error]") {
	Context ctx;
	const char* expected = "unterminated statement";
	try {
		ctx.load("../test/test_syntax_error.js");
	}
	catch (MileneJSException &e) {
		CHECK(std::string(e.what()).find(expected) != std::string::npos);
	}
}

TEST_CASE("Do syntax error", "[error]") {
	Context ctx;
	const char* expected = "unterminated statement";
	try {
		ctx(R"(
function syntax_error() {
	1 2 3 4
}
		)");
	}
	catch (MileneJSException &e) {
		CHECK(std::string(e.what()).find(expected) != std::string::npos);
	}
}

TEST_CASE("Call undefined function", "[error]") {
	Context ctx;
	const char* expected = "undefined not callable";
	try {
		ctx.load("../test/test_error.js");
		ctx["undefined_function"]();
	}
	catch (MileneJSException &e) {
		CHECK(std::string(e.what()).find(expected) != std::string::npos);
	}
}

TEST_CASE("Call undefined function 2", "[error]") {
	Context ctx;
	const char* expected = "identifier 'err_func2' undefined";
	try {
		ctx.load("../test/test_error.js");
		ctx["err_func1"](1, 2);
	}
	catch (MileneJSException &e) {
		CHECK(std::string(e.what()).find(expected) != std::string::npos);
	}
}

TEST_CASE("Call stack overflow", "[error]") {
	Context ctx;
	const char* expected = "callstack limit";
	try {
		ctx.load("../test/test_error.js");
		ctx["do_overflow"]();
	}
	catch (MileneJSException &e) {
		CHECK(std::string(e.what()).find(expected) != std::string::npos);
	}
}

TEST_CASE("Parameter conversion error", "[error]") {
	Context ctx;
	const char * expected =
		"number required, found 'not a number'";
	try {
		std::string largeStringToPreventSSO(50, 'x');
		ctx.global().Register("accept_string_int_string", [](std::string, int, std::string) {});

		ctx["accept_string_int_string"](
			largeStringToPreventSSO,
			"not a number",
			largeStringToPreventSSO);
	}
	catch (MileneJSException &e) {
		CHECK(std::string(e.what()).find(expected) != std::string::npos);
	}
}
