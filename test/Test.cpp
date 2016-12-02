#include <selenejs2.h>
#include <selenejs2/detail/PrototypeRegistry.h>

#include <iostream>

using namespace seljs2;

void test_base()
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

	Ref obj = ctx.setRef("newobj", Type::OBJECT);
	obj.set("i", 12);
	obj.set("vv", 7762);
	std::cout << ctx["newobj"]["i"].get<int>() << std::endl;
	for (auto key : obj.keyList()) {
		std::cout << key << " == " << obj[key].cast<std::string>() << std::endl;
	}

	Ref arr = ctx.setRef("newarr", Type::ARRAY);
	arr.setRef(0, 1024);
	arr.setRef(1, 2048);
	std::cout << ctx["newarr"][1].get<int>() << std::endl;
	for (auto key : arr.keyList()) {
		std::cout << key << " == " << arr[key].cast<std::string>() << std::endl;
	}
}

class T1 {
public:
	T1() : _v1(76) {}

	void setV1(int v1) { _v1 = v1; }
	int getV1() { return _v1; }
private:
	int _v1;
	std::string _v2;
};

class T2 {
public:
	T2() : _v1(917) {}

	void setV1(int v1) { _v1 = v1; }
	int getV1() { return _v1; }
private:
	int _v1;
	std::string _v2;
};
class T3 {};

void testPrototypeRegistry()
{
	Context ctx;
	ctx.global().SetClass<T1>("T1", "setV1", &T1::setV1, "getV1", &T1::getV1);

	ctx.global().SetClassCustom<T2, void, void>("T2", "setV1", &T2::setV1, "getV1", &T2::getV1);

	ctx("var t1 = new T1();");
	//ctx("var t2 = new T2();");
	ctx("var t1v1 = t1.getV1();");
	ctx("t1.setV1(9167); var t1v1new = t1.getV1();");

	std::cout << ctx["t1v1"].get<int>() << std::endl;
	std::cout << ctx["t1v1new"].get<int>() << std::endl;

	/*
	detail::PrototypeRegistry::Create(ctx);
	detail::PrototypeRegistry::PushNewPrototype(ctx, typeid(T1), "T1");
	detail::PrototypeRegistry::PushNewPrototype(ctx, typeid(T2), "T2");

	duk_push_object(ctx);
	detail::PrototypeRegistry::SetPrototype(ctx, typeid(T1));

	duk_push_object(ctx);
	detail::PrototypeRegistry::SetPrototype(ctx, typeid(T2));

	duk_push_object(ctx);
	detail::PrototypeRegistry::SetPrototype(ctx, typeid(T3));

	std::cout << detail::PrototypeRegistry::GetTypeName(ctx, -3) << std::endl;
	std::cout << detail::PrototypeRegistry::GetTypeName(ctx, -2) << std::endl;
	std::cout << detail::PrototypeRegistry::GetTypeName(ctx, -1) << std::endl;
	*/
}

int main() {

	try {
		//test_base();
		testPrototypeRegistry();
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	std::cout << "PRESS ANY KEY TO CONTINUE";
	std::cin.ignore();
	return 0;
}
