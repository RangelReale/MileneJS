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

		//x4 = 92;

		std::cout << ctx["h"]["a"].get<int>() << std::endl;
	}

	std::cout << "PRESS ANY KEY TO CONTINUE";
	std::cin.ignore();
	return 0;
}
