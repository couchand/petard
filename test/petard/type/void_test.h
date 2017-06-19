#include <cxxtest/TestSuite.h>

#include "type.h"

class VoidTypeTest : public CxxTest::TestSuite
{
public:
    void testVoidToString(void)
    {
        VoidTypeHandle voidType;
        TS_ASSERT_EQUALS(voidType.toString(), "void");
    }

    void testVoidIsVoidType(void)
    {
        VoidTypeHandle voidType;
        TS_ASSERT(voidType.isVoidType());
    }

    void testVoidIsNotIntType(void)
    {
        VoidTypeHandle voidType;
        TS_ASSERT(!voidType.isIntType())
    }

    void testVoidIsNotFloatType(void)
    {
        VoidTypeHandle voidType;
        TS_ASSERT(!voidType.isFloatType())
    }

    void testVoidIsNotPointerType(void)
    {
        VoidTypeHandle voidType;
        TS_ASSERT(!voidType.isPointerType())
    }

    void testVoidIsNotFunctionType(void)
    {
        VoidTypeHandle voidType;
        TS_ASSERT(!voidType.isFunctionType())
    }

    void testVoidIsNotVectorType(void)
    {
        VoidTypeHandle voidType;
        TS_ASSERT(!voidType.isVectorType())
    }

    void testVoidIsNotArrayType(void)
    {
        VoidTypeHandle voidType;
        TS_ASSERT(!voidType.isArrayType())
    }

    void testVoidIsNotStructType(void)
    {
        VoidTypeHandle voidType;
        TS_ASSERT(!voidType.isStructType())
    }

    void testVoidIsCompatibleWithVoid(void)
    {
        VoidTypeHandle voidType, anotherVoid;
        TS_ASSERT(voidType.isCompatibleWith(&anotherVoid));
    }

    void testVoidIsNotCompatibleWithOtherTypes(void)
    {
        VoidTypeHandle voidType;
        IntTypeHandle intType(32);
        FloatTypeHandle floatType(64);
        PointerTypeHandle pointerType(std::make_shared<VoidTypeHandle>());
        ArrayTypeHandle arrayType(1, std::make_shared<VoidTypeHandle>());
        VectorTypeHandle vectorType(1, std::make_shared<VoidTypeHandle>());
        TS_ASSERT(!voidType.isCompatibleWith(&intType));
        TS_ASSERT(!voidType.isCompatibleWith(&floatType));
        TS_ASSERT(!voidType.isCompatibleWith(&pointerType));
        TS_ASSERT(!voidType.isCompatibleWith(&arrayType));
        TS_ASSERT(!voidType.isCompatibleWith(&vectorType));
    }

    void testVoidGetLLVMType(void)
    {
        llvm::LLVMContext context;
        VoidTypeHandle voidType;
        TS_ASSERT(voidType.getLLVMType(context)->isVoidTy());
    }
};
