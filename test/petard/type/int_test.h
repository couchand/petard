#include <cxxtest/TestSuite.h>

#include "type.h"

class IntTypeTest : public CxxTest::TestSuite
{
public:
    void testIntToString(void)
    {
        IntTypeHandle intType(32);
        TS_ASSERT_EQUALS(intType.toString(), "i32");
    }

    void testIntIsNotVoidType(void)
    {
        IntTypeHandle intType(32);
        TS_ASSERT(!intType.isVoidType());
    }

    void testIntIsIntType(void)
    {
        IntTypeHandle intType(32);
        TS_ASSERT(intType.isIntType())
    }

    void testIntIsNotFloatType(void)
    {
        IntTypeHandle intType(32);
        TS_ASSERT(!intType.isFloatType())
    }

    void testIntIsNotPointerType(void)
    {
        IntTypeHandle intType(32);
        TS_ASSERT(!intType.isPointerType())
    }

    void testIntIsNotFunctionType(void)
    {
        IntTypeHandle intType(32);
        TS_ASSERT(!intType.isFunctionType())
    }

    void testIntIsNotVectorType(void)
    {
        IntTypeHandle intType(32);
        TS_ASSERT(!intType.isVectorType())
    }

    void testIntIsNotArrayType(void)
    {
        IntTypeHandle intType(32);
        TS_ASSERT(!intType.isArrayType())
    }

    void testIntIsNotStructType(void)
    {
        IntTypeHandle intType(32);
        TS_ASSERT(!intType.isStructType())
    }

    void testIntIsCompatibleWithIntOfSameWidth(void)
    {
        IntTypeHandle intType(32), anotherInt(32);
        TS_ASSERT(intType.isCompatibleWith(&anotherInt));
    }

    void testIntIsNotCompatibleWithIntOfDifferentWidth(void)
    {
        IntTypeHandle intType(32), largerInt(64);
        TS_ASSERT(!intType.isCompatibleWith(&largerInt));
    }

    void testIntIsNotCompatibleWithOtherTypes(void)
    {
        IntTypeHandle intType(32);
        VoidTypeHandle voidType;
        FloatTypeHandle floatType(64);
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        ArrayTypeHandle arrayType(1, std::make_shared<IntTypeHandle>(32));
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!intType.isCompatibleWith(&voidType));
        TS_ASSERT(!intType.isCompatibleWith(&floatType));
        TS_ASSERT(!intType.isCompatibleWith(&pointerType));
        TS_ASSERT(!intType.isCompatibleWith(&arrayType));
        TS_ASSERT(!intType.isCompatibleWith(&vectorType));
    }

    void testIntGetLLVMType(void)
    {
        llvm::LLVMContext context;
        IntTypeHandle int32Type(32), int64Type(64);
        TS_ASSERT(int32Type.getLLVMType(context)->isIntegerTy(32));
        TS_ASSERT(int64Type.getLLVMType(context)->isIntegerTy(64));
    }
};
