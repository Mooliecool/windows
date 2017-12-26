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

#include "rotor_pal.h"
#include "rotor_palrt.h" // for GetLastError, HRESULT_FROM_WIN32
#include "syms.h" // for LIB_mscorwks
#include "sscoree_int.h" // For SetupLib

// These are interop runtime helpers. They are implemented directly in the shim for two reasons:
// 1. They has to live outside mscorwks.dll for some implementation of exception handling.
// 2. These helpers should better be fast. The shim forwarding is unnecessary overhead.

extern "C" INT32 __stdcall ND_RU1(VOID *psrc, INT32 ofs)
{
    return (INT32) *( (UINT8*)(ofs + (BYTE*)psrc) );
}

extern "C" INT32 __stdcall ND_RI2(VOID *psrc, INT32 ofs)
{
   return GET_UNALIGNED_16(ofs + (BYTE*)psrc); // (INT32) *( (INT16*)(ofs + (BYTE*)psrc) );
}

extern "C" INT32 __stdcall ND_RI4(VOID *psrc, INT32 ofs)
{
    return GET_UNALIGNED_32(ofs + (BYTE*)psrc); // (INT32) *( (INT32*)(ofs + (BYTE*)psrc) );
}

extern "C" INT64 __stdcall ND_RI8(VOID *psrc, INT32 ofs)
{
    return GET_UNALIGNED_64(ofs + (BYTE*)psrc); // (INT64) *( (INT64*)(ofs + (BYTE*)psrc) );
}

extern "C" VOID __stdcall ND_WU1(VOID *psrc, INT32 ofs, UINT8 val)
{
    *( (UINT8*)(ofs + (BYTE*)psrc) ) = val;
}


extern "C" VOID __stdcall ND_WI2(VOID *psrc, INT32 ofs, INT16 val)
{

    SET_UNALIGNED_16(ofs + (BYTE*)psrc, val); // *( (INT16*)(ofs + (BYTE*)psrc) ) = val;
}


extern "C" VOID __stdcall ND_WI4(VOID *psrc, INT32 ofs, INT32 val)
{
    SET_UNALIGNED_32(ofs + (BYTE*)psrc, val); // *( (INT32*)(ofs + (BYTE*)psrc) ) = val;
}


extern "C" VOID __stdcall ND_WI8(VOID *psrc, INT32 ofs, INT64 val)
{
    SET_UNALIGNED_64(ofs + (BYTE*)psrc, val); // *( (INT64*)(ofs + (BYTE*)psrc) ) = val;
}

extern "C" VOID __stdcall ND_CopyObjSrc(LPBYTE source, int ofs, LPBYTE dst, int cb)
{
    CopyMemory(dst, source + ofs, cb);
}

extern "C" VOID __stdcall ND_CopyObjDst(LPBYTE source, LPBYTE dst, int ofs, int cb)
{
    CopyMemory(dst + ofs, source, cb);
}

static
HMODULE GetLibrary(ShimmedLib LibIndex);

extern "C"
STDAPI GetRealProcAddress(LPCSTR pwszProcName, VOID** ppv)
{
    if(!ppv) 
    { 
        return E_POINTER; 
    } 

    HMODULE hLib = GetLibrary(LIB_mscorwks);
    if(hLib == NULL)
    {
        return HRESULT_FROM_GetLastError();
    }

    *ppv = (void*) GetProcAddress(hLib,pwszProcName);
    if(*ppv == NULL)
    {
        return HRESULT_FROM_GetLastError();
    }
    return S_OK;
}

extern "C" 
STDAPI CreateCordbObject(int iDebuggerVersion, IUnknown ** ppCordb);

extern "C"
STDAPI CreateDebuggingInterfaceFromVersion(
    int iDebuggerVersion, 
    LPCWSTR szDebuggeeVersion, 
    IUnknown ** ppCordb)
{
    return CreateCordbObject(iDebuggerVersion, ppCordb);
}

static
HMODULE GetLibrary(ShimmedLib LibIndex)
{
    // the rest of the accesses to are not protected by locks
    // so I assume that locking is not needed here
    if(g_Libs[LibIndex].Handle == NULL)
    {
        g_Libs[LibIndex].Handle = SetupLib(LibIndex);
    }
    return g_Libs[LibIndex].Handle;
}
