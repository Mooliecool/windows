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


/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
//@@MIDL_FILE_HEADING(  )

#ifdef _MSC_VER
#pragma warning( disable: 4049 )  /* more than 64k source lines */
#endif


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ICorProfilerCallback,0x176FBED1,0xA55C,0x4796,0x98,0xCA,0xA9,0xDA,0x0E,0xF8,0x83,0xE7);


MIDL_DEFINE_GUID(IID, IID_ICorProfilerCallback2,0x8A8CC829,0xCCF2,0x49fe,0xBB,0xAE,0x0F,0x02,0x22,0x28,0x07,0x1A);


MIDL_DEFINE_GUID(IID, IID_ICorProfilerInfo,0x28B5557D,0x3F3F,0x48b4,0x90,0xB2,0x5F,0x9E,0xEA,0x2F,0x6C,0x48);


MIDL_DEFINE_GUID(IID, IID_ICorProfilerInfo2,0xCC0935CD,0xA518,0x487d,0xB0,0xBB,0xA9,0x32,0x14,0xE6,0x54,0x78);


MIDL_DEFINE_GUID(IID, IID_ICorProfilerObjectEnum,0x2C6269BD,0x2D13,0x4321,0xAE,0x12,0x66,0x86,0x36,0x5F,0xD6,0xAF);


MIDL_DEFINE_GUID(IID, IID_IMethodMalloc,0xA0EFB28B,0x6EE2,0x4d7b,0xB9,0x83,0xA7,0x5E,0xF7,0xBE,0xED,0xB8);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



