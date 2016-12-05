#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include <milenejs.h>

using namespace miljs;

int main(int argc, char* const argv[])
{
	int result = Catch::Session().run(argc, argv);
	if (result > 0)
	{
		std::cout << "PRESS ANY KEY TO CONTINUE";
		std::cin.ignore();
	}
	return result;
}

TEST_CASE("Eval script in context", "[context]") {

	const char* test_script = R"JS(
	var info = {version: 1, description: 'test'};
	)JS";

	const char* test_script_bug = R"JS(
	var info = {NOTVALID=1, ERROR=2};
	)JS";

	Context ctx;
	REQUIRE_NOTHROW(ctx(test_script));
	REQUIRE_THROWS_AS(ctx(test_script_bug), MileneJSException);

}
