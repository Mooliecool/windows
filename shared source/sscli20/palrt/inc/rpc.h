// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
// ===========================================================================
// File: rpc.h
// 
// =========================================================================== 
// dummy rpc.h for PAL

#ifndef __RPC_H__
#define __RPC_H__

#include "rotor_palrt.h"

#define __RPC_STUB
#define __RPC_USER
#define __RPC_FAR

#define MIDL_INTERFACE(x)   struct
#define DECLSPEC_UUID(x)

#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8) EXTERN_C const IID itf

interface IRpcStubBuffer;
interface IRpcChannelBuffer;

typedef void* PRPC_MESSAGE;
typedef void* RPC_IF_HANDLE;

#endif // __RPC_H__
