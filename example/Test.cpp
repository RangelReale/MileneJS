#include <milenejs.h>
#include <milenejs/detail/PrototypeRegistry.h>

#include <iostream>

using namespace miljs;

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
	~T1() {}

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

int testfunc(int a, int b)
{
	return a + b;
}

void testPrototypeRegistry()
{
	Context ctx;
	ctx.global().SetClass<T1>("T1", "setV1", &T1::setV1, "getV1", &T1::getV1);

	ctx.global().SetClassCustom<T2, void, void>("T2", "setV1", &T2::setV1, "getV1", &T2::getV1);

	ctx("var t1 = new T1();");
	ctx("var t1v1 = t1.getV1();");
	ctx("t1.setV1(9167); var t1v1new = t1.getV1();");

	std::cout << ctx["t1v1"].get<int>() << std::endl;
	std::cout << ctx["t1v1new"].get<int>() << std::endl;

	T1 vt1;
	vt1.setV1(4444);
	ctx.global().SetClassObj("vt1", vt1);

	ctx("vt1.setV1(667231); var vt1data = vt1.getV1();");
	std::cout << ctx["vt1data"].get<int>() << std::endl;

	T2 t2;
	ctx.global().SetClassObj("t2", t2);
	ctx("var t2v1 = t2.getV1();");
	std::cout << ctx["t2v1"].get<int>() << std::endl;

	//ctx("var t2 = new T2();");

	ctx.global().Register("testfunc", &testfunc);
	ctx("var tf = testfunc(10, 20);");
	std::cout << ctx["tf"].get<int>() << std::endl;

	ctx("function testcalc(a, b) { return a+b+10; };");
	std::cout << ctx.global()["testcalc"](10, 30).get<int>() << std::endl;

	std::cout << ctx["t1"].call("getV1").get<int>() << std::endl;

	std::cout << ctx["t1"].callMethod(ctx["t1"], "getV1").get<int>() << std::endl;

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

class ExampleException : public std::exception {
	std::string _message;
public:
	explicit ExampleException(const std::string &message)
		: _message(message) {}
	char const * what() const noexcept override {
		return _message.c_str();
	}
};

void te_throw()
{
	throw ExampleException("test throw");
}

void te_wrongparam(int v)
{
	std::cout << v << std::endl;
}

void testError()
{
	Context ctx;

	//ctx.load("non_existent_file.js");

	/*
	ctx(R"(
		function syntax_error() {
		1 2 3 4
	}
	)");
	*/

	//ctx["undefined_function"]();

	ctx.global().Register("te_throw", &te_throw);
	ctx["te_throw"]();

	//ctx.global().Register("te_wrongparam", &te_wrongparam);
	//ctx["te_wrongparam"]("string");
}

int main() {

	try {
		//test_base();
		//testPrototypeRegistry();
		testError();
	}
	catch (MileneJSException &e) {
		std::cout << e.what() << std::endl;
		std::cout << e.stack() << std::endl;
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	std::cout << "PRESS ANY KEY TO CONTINUE";
	std::cin.ignore();
	return 0;
}
