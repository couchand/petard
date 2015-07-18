// codegen

#include <nan.h>
using node::ObjectWrap;

using namespace v8;

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "types.h"
#include "function_builder.h"
#include "codegen.h"

class TypeWrapper : public ObjectWrap
{
    TypeWrapper(TypeHandle *t)
    : Type(t) {}

    static NAN_METHOD(New)
    {
        NanScope();

        if (!args.IsConstructCall() || args.Length() == 0 || !args[0]->IsExternal())
        {
            return NanThrowError("Cannot instantiate type directly, use factory");
        }

        Handle<External> handle = Handle<External>::Cast(args[0]);
        TypeHandle *t = static_cast<TypeHandle *>(handle->Value());
        TypeWrapper *instance = new TypeWrapper(t);

        instance->Wrap(args.This());

        NanReturnValue(args.This());
    }

    static NAN_METHOD(ToString)
    {
        NanScope();

        TypeWrapper *wrapper = ObjectWrap::Unwrap<TypeWrapper>(args.This());

        std::string name = wrapper->Type->toString();

        NanReturnValue(NanNew(name));
    }

    static Handle<Value> wrapType(TypeHandle *type)
    {
        NanEscapableScope();

        Handle<Value> argv[1] = { NanNew<External>((void *)type) };

        return NanEscapeScope(NanNew(constructor)->NewInstance(1, argv));
    }

public:
    TypeHandle *Type;

    static Persistent<FunctionTemplate> prototype;
    static Persistent<Function> constructor;

    static void Init()
    {
        Local<FunctionTemplate> tmpl = NanNew<FunctionTemplate>(TypeWrapper::New);

        tmpl->SetClassName(NanNew<String>("Type"));
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);
        NODE_SET_PROTOTYPE_METHOD(tmpl, "toString", TypeWrapper::ToString);

        NanAssignPersistent(prototype, tmpl);
        NanAssignPersistent(constructor, tmpl->GetFunction());
    }

    static NAN_METHOD(GetVoidTy)
    {
        NanScope();
        NanReturnValue(wrapType(new VoidTypeHandle()));
    }

    static NAN_METHOD(GetFunctionTy)
    {
        NanScope();

        TypeHandle *returns;
        std::vector<TypeHandle *> takes;

        if (args.Length() == 0)
        {
            returns = new VoidTypeHandle();
        }
        else
        {
            Local<Object> handle = args[0]->ToObject();

            if (!NanHasInstance(prototype, handle))
            {
                return NanThrowError("Argument must be a type specifier");
            }

            TypeWrapper *wrapper = ObjectWrap::Unwrap<TypeWrapper>(handle);
            returns = wrapper->Type;
        }

        for (unsigned i = 1, e = args.Length(); i < e; i += 1)
        {
            Local<Object> handle = args[i]->ToObject();

            if (!NanHasInstance(prototype, handle))
            {
                return NanThrowError("Argument must be a type specifier");
            }

            TypeWrapper *wrapper = ObjectWrap::Unwrap<TypeWrapper>(handle);
            takes.push_back(wrapper->Type);
        }

        NanReturnValue(wrapType(new FunctionTypeHandle(returns, takes)));
    }
};

class FunctionBuilderWrapper : public ObjectWrap
{
    explicit FunctionBuilderWrapper(FunctionBuilder *builder)
    : Builder(builder) {}

    static NAN_METHOD(New)
    {
        NanScope();

        if (!args.IsConstructCall() || args.Length() == 0 || !args[0]->IsExternal())
        {
            return NanThrowError("Cannot instantiate type directly, use factory");
        }

        Handle<External> handle = Handle<External>::Cast(args[0]);
        FunctionBuilder *fb = static_cast<FunctionBuilder *>(handle->Value());
        FunctionBuilderWrapper *instance = new FunctionBuilderWrapper(fb);

        instance->Wrap(args.This());

        NanReturnValue(args.This());
    }

public:
    FunctionBuilder *Builder;

    static Persistent<FunctionTemplate> prototype;
    static Persistent<Function> constructor;

    static void Init()
    {
        NanScope();

        Local<FunctionTemplate> tmpl = NanNew<FunctionTemplate>(FunctionBuilderWrapper::New);

        tmpl->SetClassName(NanNew("FunctionBuilder"));
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);

        NanAssignPersistent(prototype, tmpl);
        NanAssignPersistent(constructor, tmpl->GetFunction());
    }
};

class CodeUnitWrapper : public ObjectWrap
{
    explicit CodeUnitWrapper(CodeUnit *unit)
    : Unit(unit) {}

    static NAN_METHOD(New)
    {
        NanScope();

        if (!args.IsConstructCall())
        {
            const int argc = 1;
            Local<Value> argv[argc] = { args[0] };
            Local<Function> cons = NanNew(constructor);
            NanReturnValue(cons->NewInstance(argc, argv));
        }
        else
        {
            if (!args[0]->IsString())
            {
                return NanThrowError("Must provide file name.");
            }

            Local<String> filename = args[0].As<String>();
            String::Utf8Value encoded(filename);

            CodeUnit *unit = new CodeUnit(*encoded);
            CodeUnitWrapper *wrapper = new CodeUnitWrapper(unit);
            wrapper->Wrap(args.This());

            NanReturnValue(args.This());
        }
    }

    void dumpModule()
    {
        Unit->dumpModule();
    }

    static NAN_METHOD(Dump)
    {
        NanScope();

        CodeUnitWrapper *self = ObjectWrap::Unwrap<CodeUnitWrapper>(args.This());

        self->dumpModule();

        NanReturnUndefined();
    }

    static NAN_METHOD(MakeFunction)
    {
        NanEscapableScope();

        CodeUnitWrapper *self = ObjectWrap::Unwrap<CodeUnitWrapper>(args.This());

        if (args.Length() == 0 || !args[0]->IsString())
        {
            return NanThrowError("Must provide function name");
        }

        Local<String> fnname = args[0].As<String>();
        String::Utf8Value encoded(fnname);

        if (args.Length() == 1)
        {
            return NanThrowError("Must provide function type");
        }

        Local<Object> handle = args[1]->ToObject();
        if (!NanHasInstance(TypeWrapper::prototype, handle))
        {
            return NanThrowError("Must provide function type");
        }

        TypeWrapper *wrapper = ObjectWrap::Unwrap<TypeWrapper>(handle);

        FunctionBuilder *builder = self->Unit->MakeFunction(*encoded, wrapper->Type);

        if (!builder)
        {
            return NanThrowError("Unable to create function (is name unique?)");
        }

        Handle<Value> argv[1] = { NanNew<External>((void *)builder) };

        NanReturnValue(NanNew(FunctionBuilderWrapper::constructor)->NewInstance(1, argv));
    }

public:
    CodeUnit *Unit;

    static Persistent<FunctionTemplate> prototype;
    static Persistent<Function> constructor;

    static void Init()
    {
        NanScope();

        Local<FunctionTemplate> tmpl = NanNew<FunctionTemplate>(CodeUnitWrapper::New);

        tmpl->SetClassName(NanNew<String>("CodeUnit"));
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(tmpl, "dump", CodeUnitWrapper::Dump);
        NODE_SET_PROTOTYPE_METHOD(tmpl, "makeFunction", CodeUnitWrapper::MakeFunction);

        NanAssignPersistent(prototype, tmpl);
        NanAssignPersistent(constructor, tmpl->GetFunction());
    }
};

Persistent<FunctionTemplate> FunctionBuilderWrapper::prototype;
Persistent<Function> FunctionBuilderWrapper::constructor;
Persistent<FunctionTemplate> TypeWrapper::prototype;
Persistent<Function> TypeWrapper::constructor;
Persistent<FunctionTemplate> CodeUnitWrapper::prototype;
Persistent<Function> CodeUnitWrapper::constructor;

void Init(Handle<Object> exports, Handle<Object> module)
{
    TypeWrapper::Init();
    CodeUnitWrapper::Init();
    FunctionBuilderWrapper::Init();

    Local<Function> getVoidTy = NanNew<Function>(TypeWrapper::GetVoidTy);
    exports->Set(NanNew<String>("getVoidTy"), getVoidTy);

    Local<Function> getFunctionTy = NanNew<Function>(TypeWrapper::GetFunctionTy);
    exports->Set(NanNew<String>("getFunctionTy"), getFunctionTy);

    Local<Function> codeUnit = NanNew(CodeUnitWrapper::constructor);
    exports->Set(NanNew<String>("CodeUnit"), codeUnit);
}

NODE_MODULE(codegen, Init)
