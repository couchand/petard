#include <cxxtest/TestSuite.h>

#include "type.h"

class PointerTypeTest : public CxxTest::TestSuite
{
public:
    void testPointerToString(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT_EQUALS(pointerType.toString(), "i32*");
    }

    void testPointerIsNotVoidType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!pointerType.isVoidType());
    }

    void testPointerIsNotIntType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!pointerType.isIntType())
    }

    void testPointerIsNotFloatType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!pointerType.isFloatType())
    }

    void testPointerIsPointerType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(pointerType.isPointerType())
    }

    void testPointerIsNotFunctionType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!pointerType.isFunctionType())
    }

    void testPointerIsNotVectorType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!pointerType.isVectorType())
    }

    void testPointerIsNotArrayType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!pointerType.isArrayType())
    }

    void testPointerIsNotStructType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(!pointerType.isStructType())
    }

    void testPointerIsCompatibleWithPointerToSameType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32)), anotherPointer(std::make_shared<IntTypeHandle>(32));
        TS_ASSERT(pointerType.isCompatibleWith(&anotherPointer));
    }

    void testPointerIsNotCompatibleWithPointerToDifferentType(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32)), differentPointer(std::make_shared<FloatTypeHandle>(64));
        TS_ASSERT(!pointerType.isCompatibleWith(&differentPointer));
    }

    void testPointerIsNotCompatibleWithOtherTypes(void)
    {
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));
        VoidTypeHandle voidType;
        IntTypeHandle intType(32);
        FloatTypeHandle floatType(64);
        TS_ASSERT(!pointerType.isCompatibleWith(&voidType));
        TS_ASSERT(!pointerType.isCompatibleWith(&intType));
        TS_ASSERT(!pointerType.isCompatibleWith(&floatType));
    }

    void testPointerGetLLVMType(void)
    {
        llvm::LLVMContext context;
        PointerTypeHandle pointerType(std::make_shared<IntTypeHandle>(32));

        auto ty = pointerType.getLLVMType(context);

        TS_ASSERT(ty->isPointerTy());
        auto ptrTy = static_cast<llvm::PointerType *>(ty);
        TS_ASSERT(ptrTy->getElementType()->isIntegerTy(32));
    }
};
