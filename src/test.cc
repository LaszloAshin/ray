#include <catch2/catch_test_macros.hpp>

#include "MyLib.h"

TEST_CASE("mysnprintOne", "zero") {
	char buf[2] = { 'A', 'B'};
	int n = mysnprintOne(buf, sizeof(buf), 0);
	REQUIRE(n == 1);
	REQUIRE(buf[0] == '0');
	REQUIRE(buf[1] == 0);
}
