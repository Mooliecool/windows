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
#ifndef _FUSION_INC_FUSIONHEAP_H_INCLUDED_
#define _FUSION_INC_FUSIONHEAP_H_INCLUDED_

#include "new.hpp"

#define FUSION_NEW_SINGLETON(_type) new (nothrow) _type
#define FUSION_NEW_ARRAY(_type, _n) new (nothrow) _type[_n]
#define FUSION_DELETE_ARRAY(_ptr) if((_ptr)) delete [] (_ptr)
#define FUSION_DELETE_SINGLETON(_ptr) if((_ptr)) delete (_ptr)

#define NEW(_type) FUSION_NEW_SINGLETON(_type)

EXTERN_C
BOOL
FusionpInitializeHeap(
    HINSTANCE hInstance
    );

EXTERN_C
VOID
FusionpUninitializeHeap();

#endif // _FUSION_INC_FUSIONHEAP_H_INCLUDED_
