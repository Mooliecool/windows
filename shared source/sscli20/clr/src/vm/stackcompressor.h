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

#ifndef __stackcompressor_h__
#define __stackcompressor_h__
#include "common.h"


#include "newcompressedstack.h"
#include "security.h"
#include "appdomainhelper.h"
#include "threads.inl"

#ifndef DACCESS_COMPILE

class StackCompressor
{
       
public:
    static DWORD StackCompressor::GetCSInnerAppDomainAssertCount(COMPRESSEDSTACKREF csRef);
    static DWORD StackCompressor::GetCSInnerAppDomainOverridesCount(COMPRESSEDSTACKREF csRef);
    static void* SetAppDomainStack(Thread* pThread, void* curr);
    static void RestoreAppDomainStack(Thread* pThread, void* appDomainStack);

    static void Destroy(void *stack);
    static OBJECTREF GetCompressedStack( StackCrawlMark* stackMark = NULL );
    
    
};
#endif // DACCESS_COMPILE
#endif // __stackcompressor_h__

