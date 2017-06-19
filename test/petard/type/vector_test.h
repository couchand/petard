#include <cxxtest/TestSuite.h>

#include "type.h"

class VectorTypeTest : public CxxTest::TestSuite
{
public:
    void testVectorToString(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT_EQUALS(vectorType.toString(), "<1 x i32>");
    }

    void testVectorIsNotVoidType(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!vectorType.isVoidType());
    }

    void testVectorIsNotIntType(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!vectorType.isIntType())
    }

    void testVectorIsNotFloatType(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!vectorType.isFloatType())
    }

    void testVectorIsNotPointerType(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!vectorType.isPointerType())
    }

    void testVectorIsNotFunctionType(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!vectorType.isFunctionType())
    }

    void testVectorIsVectorType(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(vectorType.isVectorType())
    }

    void testVectorIsNotArrayType(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!vectorType.isArrayType())
    }

    void testVectorIsNotStructType(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!vectorType.isStructType())
    }

    void testVectorIsCompatibleWithVectorOfSameTypeAndSize(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32)), anotherVector(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(vectorType.isCompatibleWith(&anotherVector));
    }

    void testVectorIsNotCompatibleWithVectorfDifferentSize(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32)), differentVector(2, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!vectorType.isCompatibleWith(&differentVector));
    }

    void testVectorIsNotCompatibleWithVectorfDifferentType(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32)), differentVector(1, std::make_shared<FloatTypeHandle>(64));
        TS_ASSERT(!vectorType.isCompatibleWith(&differentVector));
    }

    void testVectorIsNotCompatibleWithOtherTypes(void)
    {
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));
        VoidTypeHandle voidType;
        IntTypeHandle intType(32);
        FloatTypeHandle floatType(64);
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        ArrayTypeHandle arrayType(1, std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!vectorType.isCompatibleWith(&voidType));
        TS_ASSERT(!vectorType.isCompatibleWith(&intType));
        TS_ASSERT(!vectorType.isCompatibleWith(&floatType));
        TS_ASSERT(!vectorType.isCompatibleWith(&pointerType));
        TS_ASSERT(!vectorType.isCompatibleWith(&arrayType));
    }

    void testVectorGetLLVMType(void)
    {
        llvm::LLVMContext context;
        VectorTypeHandle vectorType(1, std::make_shared<IntTypeHandle>(32));

        auto ty = vectorType.getLLVMType(context);

        TS_ASSERT(ty->isVectorTy());
        auto vecTy = static_cast<llvm::VectorType *>(ty);
        TS_ASSERT(vecTy->getNumElements() == 1);
        TS_ASSERT(vecTy->getElementType()->isIntegerTy(32));
    }
};
