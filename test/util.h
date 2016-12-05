#pragma once

#include <milenejs.h>

#include <string>

#define CHECKMILEXCEPTION_STR(expr, expected)	\
	try {	\
		expr;	\
	}	\
	catch (MileneJSException &e) { \
		CHECK(std::string(e.what()).find(expected) != std::string::npos); \
	}
