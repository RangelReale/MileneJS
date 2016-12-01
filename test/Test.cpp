#include <selenejs2.h>

#include "selenejs2/primitive.h"
#include "selenejs2/Ref.h"

#include <iostream>

using namespace seljs2;

int main() {
	State s;
	detail::_push(s, 14);
	detail::_push(s, 98.2);
	detail::_push(s, "teste");

	std::cout << detail::_pop<std::string>(s) << std::endl;
	std::cout << detail::_pop<double>(s) << std::endl;
	std::cout << detail::_pop<int>(s) << std::endl;

	/****************/

	detail::_push(s, 918);
	Ref r(&s);
	std::cout << "stack amount: " << duk_get_top(s) << std::endl;
	std::cout << r.get<int>() << std::endl;

	std::cout << "PRESS ANY KEY TO CONTINUE";
	std::cin.ignore();
	return 0;
}
