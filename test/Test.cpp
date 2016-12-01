#include <selenejs2.h>

#include <iostream>

using namespace seljs2;

int main() {

	{
		Context ctx;
		ctx("var x = 30; var y = 'maria'; var h = {a: 777};");

		ctx.set("x", 31);

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

		std::cout << "---" << std::endl;

		x.set("a", 14);
		std::cout << x["a"].get<int>() << std::endl;

		ctx["h"].setRef("b", 999);
		std::cout << ctx["h"]["b"].get<int>() << std::endl;

		ctx.setRef("newobj", Type::OBJECT).set("i", 12);
		std::cout << ctx["newobj"]["i"].get<int>() << std::endl;

		Ref arr = ctx.setRef("newarr", Type::ARRAY);
		arr.setRef(0, 1024); 
		arr.setRef(1, 2048);
		std::cout << ctx["newarr"][1].get<int>() << std::endl;
	}

	std::cout << "PRESS ANY KEY TO CONTINUE";
	std::cin.ignore();
	return 0;
}
