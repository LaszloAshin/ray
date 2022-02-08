#include <catch2/catch_test_macros.hpp>

#include "MyLib.h"

#include <cstring>

namespace {

struct TestVector {
	int size;
	int input;
	int result;
	const char* expected;
};

static const TestVector tests[] = {
	{ 2, 0, 1, "0" },
	{ 2, 1, 1, "1" },
	{ 3, -1, 2, "-1" },
	{ 4, 123, 3, "123" },
	{ 11, 2147483647, 10, "2147483647" },
	{ 12, -2147483647, 11, "-2147483647" },
};

} // anonymous namespace

TEST_CASE("mysnprintOne", "") {
	for (const auto& t : tests) {
		char buf[64];
		memset(buf, 0x55, sizeof(buf));
		int n = mysnprintOne(buf, t.size, t.input);
		REQUIRE(n == t.result);
		REQUIRE(!strcmp(buf, t.expected));
	}
}
