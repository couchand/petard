#include <cxxtest/TestSuite.h>

#include "type.h"

class ArrayTypeTest : public CxxTest::TestSuite
{
public:
    void testArrayToString(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT_EQUALS(arrayType.toString(), "[0 x i32]");
    }

    void testArrayIsNotVoidType(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!arrayType.isVoidType());
    }

    void testArrayIsNotIntType(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!arrayType.isIntType())
    }

    void testArrayIsNotFloatType(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!arrayType.isFloatType())
    }

    void testArrayIsNotPointerType(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!arrayType.isPointerType())
    }

    void testArrayIsNotFunctionType(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!arrayType.isFunctionType())
    }

    void testArrayIsNotVectorType(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!arrayType.isVectorType())
    }

    void testArrayIsArrayType(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(arrayType.isArrayType())
    }

    void testArrayIsNotStructType(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!arrayType.isStructType())
    }

    void testArrayIsCompatibleWithArrayOfSameTypeAndSize(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32)), anotherArray(0, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(arrayType.isCompatibleWith(&anotherArray));
    }

    void testArrayIsNotCompatibleWithArrayOfDifferentSize(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32)), differentArray(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!arrayType.isCompatibleWith(&differentArray));
    }

    void testArrayIsNotCompatibleWithArrayOfDifferentType(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32)), differentArray(0, std::make_shared<FloatTypeHandle>(64));
        TS_ASSERT(!arrayType.isCompatibleWith(&differentArray));
    }

    void testArrayIsNotCompatibleWithOtherTypes(void)
    {
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));
        VoidTypeHandle voidType;
        IntTypeHandle intType(32);
        FloatTypeHandle floatType(64);
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!arrayType.isCompatibleWith(&voidType));
        TS_ASSERT(!arrayType.isCompatibleWith(&intType));
        TS_ASSERT(!arrayType.isCompatibleWith(&floatType));
        TS_ASSERT(!arrayType.isCompatibleWith(&pointerType));
        TS_ASSERT(!arrayType.isCompatibleWith(&vectorType));
    }

    void testArrayGetLLVMType(void)
    {
        llvm::LLVMContext context;
        ArrayTypeHandle arrayType(0, std::make_shared<IntTypeHandle>(32));

        auto ty = arrayType.getLLVMType(context);

        TS_ASSERT(ty->isArrayTy());
        auto arrTy = static_cast<llvm::ArrayType *>(ty);
        TS_ASSERT(arrTy->getNumElements() == 0);
        TS_ASSERT(arrTy->getElementType()->isIntegerTy(32));
    }
};
