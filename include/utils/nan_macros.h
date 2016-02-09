// nan helper macros

#ifndef NAN_MACROS_H
#define NAN_MACROS_H

#define EXPECT_PARAM(functionName, paramIdx, paramType, paramName) \
if ((unsigned)info.Length() <= paramIdx || !Nan::New(paramType::prototype)->HasInstance(info[paramIdx])) \
{ \
    return Nan::ThrowError(functionName " expects " paramName); \
}

#endif
