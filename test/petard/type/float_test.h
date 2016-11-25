#include <cxxtest/TestSuite.h>

#include "type.h"

class FloatTypeTest : public CxxTest::TestSuite
{
public:
		void testFloatToString(void)
		{
				FloatTypeHandle floatType(32);
				TS_ASSERT_EQUALS(floatType.toString(), "float");

				FloatTypeHandle doubleType(64);
				TS_ASSERT_EQUALS(doubleType.toString(), "double");
		}

		void testFloatIsNotVoidType(void)
		{
				FloatTypeHandle floatType(32);
				TS_ASSERT(!floatType.isVoidType());
		}

		void testFloatIsNotIntType(void)
		{
				FloatTypeHandle floatType(32);
				TS_ASSERT(!floatType.isIntType())
		}

		void testFloatIsFloatType(void)
		{
				FloatTypeHandle floatType(32);
				TS_ASSERT(floatType.isFloatType())
		}

		void testFloatIsNotPointerType(void)
		{
				FloatTypeHandle floatType(32);
				TS_ASSERT(!floatType.isPointerType())
		}

		void testFloatIsNotFunctionType(void)
		{
				FloatTypeHandle floatType(32);
				TS_ASSERT(!floatType.isFunctionType())
		}

		void testFloatIsNotVectorType(void)
		{
				FloatTypeHandle floatType(32);
				TS_ASSERT(!floatType.isVectorType())
		}

		void testFloatIsNotArrayType(void)
		{
				FloatTypeHandle floatType(32);
				TS_ASSERT(!floatType.isArrayType())
		}

		void testFloatIsNotStructType(void)
		{
				FloatTypeHandle floatType(32);
				TS_ASSERT(!floatType.isStructType())
		}

		void testFloatIsCompatibleWithFloatOfSameWidth(void)
		{
				FloatTypeHandle floatType(32), anotherFloat(32);
				TS_ASSERT(floatType.isCompatibleWith(&anotherFloat));
		}

		void testFloatIsNotCompatibleWithFloatOfDifferentWidth(void)
		{
				FloatTypeHandle floatType(32), largerFloat(64);
				TS_ASSERT(!floatType.isCompatibleWith(&largerFloat));
		}

		void testFloatIsNotCompatibleWithOtherTypes(void)
		{
				FloatTypeHandle floatType(32);
				VoidTypeHandle voidType;
				IntTypeHandle intType(32);
				TS_ASSERT(!floatType.isCompatibleWith(&voidType));
				TS_ASSERT(!floatType.isCompatibleWith(&intType));
		}

		void testFloatGetLLVMType(void)
		{
				TS_SKIP("TODO: LLVM tests");
		}
};
