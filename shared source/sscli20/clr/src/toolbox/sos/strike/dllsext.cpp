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
#include "strike.h"
#include "data.h"
#include "util.h"


VOID
DllsName(
    ULONG_PTR addrContaining,
    __out_ecount (MAX_PATH) WCHAR *dllName
    )
{
    dllName[0] = L'\0';
    
    ULONG Index;
    ULONG64 base;
    if (g_ExtSymbols->GetModuleByOffset(addrContaining, 0, &Index, &base) != S_OK)
        return;
    CHAR name[MAX_PATH+1];
    ULONG length;
    if (g_ExtSymbols->GetModuleNames(Index,base,name,MAX_PATH,&length,NULL,0,NULL,NULL,0,NULL) == S_OK)
    {
        MultiByteToWideChar (CP_ACP,0,name,-1,dllName,MAX_PATH);
    }
}

