#include <selenejs2.h>

#include <iostream>

using namespace seljs2;

int main() {

	{
		Context ctx;
		ctx("var x = 30; var y = 'maria'; var h = {a: 777};");

		std::cout << ctx["x"].get<int>() << std::endl;
		std::cout << ctx["y"].get<std::string>() << std::endl;

		Ref v = ctx["x"];
		std::cout << v.get<int>() << std::endl;

		Ref x = ctx["h"];
		Ref x2 = x["a"];
		std::cout << x2.get<int>() << std::endl;

		Ref x3 = x2;
		std::cout << x3.get<int>() << std::endl;

		Ref x4(x2);
		std::cout << x4.get<int>() << std::endl;

		std::cout << ctx["h"]["a"].get<int>() << std::endl;

		/*
		State s;
		detail::_push(s, 14);
		detail::_push(s, 98.2);
		detail::_push(s, "teste");

		std::cout << detail::_pop<std::string>(s) << std::endl;
		std::cout << detail::_pop<double>(s) << std::endl;
		std::cout << detail::_pop<int>(s) << std::endl;

		detail::_push(s, 918);
		Ref r(&s);
		std::cout << "stack amount: " << duk_get_top(s) << std::endl;
		std::cout << r.get<int>() << std::endl;
		*/
	}

	std::cout << "PRESS ANY KEY TO CONTINUE";
	std::cin.ignore();
	return 0;
}
