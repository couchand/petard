// codegen

#include <nan.h>
using Nan::ObjectWrap;

using namespace v8;

#include "type_wrapper.h"
#include "value_wrapper.h"
#include "builder_wrapper.h"
#include "function_builder_wrapper.h"
#include "code_unit_wrapper.h"

static NAN_MODULE_INIT(Init)
{
    TypeWrapper::Init(target);
    ValueWrapper::Init(target);
    CodeUnitWrapper::Init(target);
    BuilderWrapper::Init(target);
    FunctionBuilderWrapper::Init(target);

    Local<FunctionTemplate> getVoidTy = Nan::New<FunctionTemplate>(TypeWrapper::GetVoidTy);
    Nan::Set(target, Nan::New("getVoidTy").ToLocalChecked(), Nan::GetFunction(getVoidTy).ToLocalChecked());

    Local<FunctionTemplate> getFunctionTy = Nan::New<FunctionTemplate>(TypeWrapper::GetFunctionTy);
    Nan::Set(target, Nan::New("getFunctionTy").ToLocalChecked(), Nan::GetFunction(getFunctionTy).ToLocalChecked());

    Local<FunctionTemplate> getIntTy = Nan::New<FunctionTemplate>(TypeWrapper::GetIntTy);
    Nan::Set(target, Nan::New("getIntTy").ToLocalChecked(), Nan::GetFunction(getIntTy).ToLocalChecked());

    Local<FunctionTemplate> getPointerTy = Nan::New<FunctionTemplate>(TypeWrapper::GetPointerTy);
    Nan::Set(target, Nan::New("getPointerTy").ToLocalChecked(), Nan::GetFunction(getPointerTy).ToLocalChecked());

    Local<FunctionTemplate> getArrayTy = Nan::New<FunctionTemplate>(TypeWrapper::GetArrayTy);
    Nan::Set(target, Nan::New("getArrayTy").ToLocalChecked(), Nan::GetFunction(getArrayTy).ToLocalChecked());

    Local<Function> codeUnit = Nan::New(CodeUnitWrapper::constructor());
    Nan::Set(target, Nan::New("CodeUnit").ToLocalChecked(), codeUnit);

    Local<Function> builder = Nan::New(BuilderWrapper::constructor());
    Nan::Set(target, Nan::New("Builder").ToLocalChecked(), builder);

    Local<Function> functionBuilder = Nan::New(FunctionBuilderWrapper::constructor());
    Nan::Set(target, Nan::New("FunctionBuilder").ToLocalChecked(), functionBuilder);

    Local<Function> value = Nan::New(ValueWrapper::constructor());
    Nan::Set(target, Nan::New("Value").ToLocalChecked(), value);

    Local<Function> type = Nan::New(TypeWrapper::constructor());
    Nan::Set(target, Nan::New("Type").ToLocalChecked(), type);
}

NODE_MODULE(petard, Init)
