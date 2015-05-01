#include <cxxtest/TestSuite.h>
#include "../src/typename.h"

class TypeTestSuite : public CxxTest::TestSuite {
public:
	void testTypeName() {
		auto typeName = TypeName::make("Int");
		TS_ASSERT_EQUALS(typeName->name(), "Int");

		auto arrayType = TypeName::make("Int[]");
		TS_ASSERT_EQUALS(arrayType->name(), "Int[]");
		TS_ASSERT_EQUALS(arrayType->isArray(), true);
		TS_ASSERT_EQUALS(arrayType->elementTypeName()->name(), "Int");
	}
};