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
/*  TLS.CPP:
 *
 *  Encapsulates TLS access for maximum performance. 
 *
 */

#include "stdafx.h"

#include "unsafe.h"
#include "tls.h"
#include "contract.h"
#include "corerror.h"
#include "ex.h"
#include "clrnt.h"
#include "clrhost.h"

#ifdef _DEBUG


static DWORD gSaveIdx;
LPVOID __stdcall GenericTlsGetValue()
{
    WRAPPER_CONTRACT;

    return UnsafeTlsGetValue(gSaveIdx);
}


// Since mscoree doesn't have access to the EE executable heap, it doesn't get the optimized getters either
#ifndef SELF_NO_HOST

VOID ExerciseTlsStuff()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    // Exercise the TLS stub generator for as many indices as we can.
    // Ideally, we'd like to test:
    //
    //      0  (boundary case)
    //      31 (boundary case for Win95)
    //      32 (boundary case for Win95)
    //      63 (boundary case for WinNT 5)
    //      64 (boundary case for WinNT 5)
    //
    // Since we can't choose what index we get, we'll just
    // do as many as we can.
    DWORD tls[128];
    int i;

    for (i = 0; i < 128; i++) {
        tls[i] = ((DWORD)(-1));
    }

    for (i = 0; i < 128; i++) {
        tls[i] = UnsafeTlsAlloc();

        if (tls[i] == ((DWORD)(-1))) {
            break;
        }

        LPVOID data = (LPVOID)(DWORD_PTR)(0x12345678+i*8);
        UnsafeTlsSetValue(tls[i], data);
        gSaveIdx = tls[i];
        POPTIMIZEDTLSGETTER pGetter1 = MakeOptimizedTlsGetter(tls[i], NULL);
        if (!pGetter1) 
            continue;

        LPVOID dataRetrieved = pGetter1();

        _ASSERTE(data == dataRetrieved);

        FreeOptimizedTlsGetter(pGetter1);
    }


    for (i = 0; i < 128; i++) {
        if (tls[i] != ((DWORD)(-1))) {
            UnsafeTlsFree(tls[i]);
        }
    }
}

#endif  // SELF_NO_HOST
#endif // _DEBUG


// Since non-hosted dlls (eg mscoree) doesn't have access to the EE executable heap, it doesn't get the optimized getters either
#ifdef SELF_NO_HOST
TLSACCESSMODE GetTLSAccessMode(DWORD tlsIndex)
{
    // Static contracts because this is used by contract infrastructure
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    return TLSACCESS_GENERIC;
}

POPTIMIZEDTLSGETTER MakeOptimizedTlsGetter(DWORD tlsIndex, LPVOID pBuffer)
{
    // Static contracts because this is used by contract infrastructure
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    
    return (POPTIMIZEDTLSGETTER) NULL;
}

VOID FreeOptimizedTlsGetter(POPTIMIZEDTLSGETTER pOptimizedTlsGetter)
{
    // Static contracts because this is used by contract infrastructure
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
}

#else  // !SELF_NO_HOST

//---------------------------------------------------------------------------
// Win95 and WinNT store the TLS in different places relative to the
// fs:[0]. This api reveals which. Can also return TLSACCESS_GENERIC if
// no info is available about the Thread location (you have to use the TlsGetValue
// api.) This is intended for use by stub generators that want to inline TLS
// access.
//---------------------------------------------------------------------------
TLSACCESSMODE GetTLSAccessMode(DWORD tlsIndex)
{
    // Static contracts because this is used by contract infrastructure
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    TLSACCESSMODE tlsAccessMode = TLSACCESS_GENERIC;

    //
    // If the app verifier is enabled, TLS indices
    // are faked to help detect invalid handle use.
    //
    if (tlsIndex >= (TLS_MINIMUM_AVAILABLE + TLS_EXPANSION_SLOTS))
    {
        tlsAccessMode = TLSACCESS_GENERIC;
    }
    else if (DbgRandomOnExe(.05))
    {
        tlsAccessMode = TLSACCESS_GENERIC;
    }
#ifdef _X86_
    else if (RunningOnWin95())
    {
        tlsAccessMode = TLSACCESS_W95;
    }
#endif // _X86_
    else if (tlsIndex < TLS_MINIMUM_AVAILABLE)
    {
        tlsAccessMode = TLSACCESS_WNT;
    }
    else
    {
        // Expansion slots are lazily created at the first call to
        // TlsGetValue on a thread, and the code we generate
        // assumes that the expansion slots will exist.
        //
        tlsAccessMode = TLSACCESS_GENERIC;//TLSACCESS_WNT_HIGH;
    }

#ifdef _DEBUG
    {
        static BOOL firstTime = TRUE;
        if (firstTime) {
            firstTime = FALSE;
            ExerciseTlsStuff();
        }
    }
#endif

    return tlsAccessMode;
}


//---------------------------------------------------------------------------
// Creates a platform-optimized version of TlsGetValue compiled
// for a particular index. Can return NULL.
//---------------------------------------------------------------------------
// AMD64 NOTE: a target for the optimized getter can be passed in, this is 
//             useful so that code can avoid an indirect call for the GetThread
//             and GetAppDomain calls for instance. If NULL is passed then
//             we will allocate from the executeable heap.
POPTIMIZEDTLSGETTER MakeOptimizedTlsGetter(DWORD tlsIndex, LPVOID pBuffer)
{
    // Static contracts because this is used by contract infrastructure
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    
    LPBYTE pCode = NULL;

    switch (GetTLSAccessMode(tlsIndex)) 
    {

#ifdef _X86_
        case TLSACCESS_WNT:
            pCode = (BYTE*) ClrHeapAlloc(ClrGetProcessExecutableHeap(), 0, 7);
            if (pCode) {
                *((WORD*)  (pCode + 0)) = 0xa164;       //  mov  eax, fs:[IMM32]
                *((DWORD*) (pCode + 2)) = WINNT_TLS_OFFSET + tlsIndex * sizeof(void*);
                *((BYTE*)  (pCode + 6)) = 0xc3;         //  retn
            }
            break;

        case TLSACCESS_WNT_HIGH:
            _ASSERTE(tlsIndex > 63);
            
            pCode = (BYTE*) ClrHeapAlloc(ClrGetProcessExecutableHeap(), 0, 14);
            if (pCode) {
                *((WORD*)  (pCode + 0)) = 0xa164;       //  mov  eax, fs:[f94]
                *((DWORD*) (pCode + 2)) = WINNT5_TLSEXPANSIONPTR_OFFSET;
            
                if ((tlsIndex - 64) < 32) {
                    *((WORD*)  (pCode + 6))  = 0x408b;   //  mov eax, [eax+IMM8]
                    *((BYTE*)  (pCode + 8))  = (BYTE)((tlsIndex - 64) * sizeof(void*));
                    *((BYTE*)  (pCode + 9)) = 0xc3;     //  retn
                } else {
                    *((WORD*)  (pCode + 6))  = 0x808b;   //  mov eax, [eax+IMM32]
                    *((DWORD*) (pCode + 8))  = (tlsIndex - 64) * sizeof(void*);
                    *((BYTE*)  (pCode + 12)) = 0xc3;     //  retn
                }
            }
            break;
            
        case TLSACCESS_W95:
            pCode = (BYTE*) ClrHeapAlloc(ClrGetProcessExecutableHeap(), 0, 14);
            if (pCode) {
                *((WORD*)  (pCode + 0)) = 0xa164;       //  mov  eax, fs:[2c]
                *((DWORD*) (pCode + 2)) = WIN95_TLSPTR_OFFSET;
            
                if (tlsIndex < 32) {
                    *((WORD*)  (pCode + 6))  = 0x408b;   //  mov eax, [eax+IMM8]
                    *((BYTE*)  (pCode + 8))  = (BYTE)(tlsIndex << 2);
                    *((BYTE*)  (pCode + 9)) = 0xc3;     //  retn
                } else {
                    *((WORD*)  (pCode + 6))  = 0x808b;   //  mov eax, [eax+IMM32]
                    *((DWORD*) (pCode + 8))  = tlsIndex << 2;
                    *((BYTE*)  (pCode + 12)) = 0xc3;     //  retn
                }
            }
            break;
#endif // _X86_




    }
    
    return (POPTIMIZEDTLSGETTER)pCode;
}


//---------------------------------------------------------------------------
// Frees a function created by MakeOptimizedTlsGetter().
//---------------------------------------------------------------------------
VOID FreeOptimizedTlsGetter(POPTIMIZEDTLSGETTER pOptimizedTlsGetter)
{
    // Static contracts because this is used by contract infrastructure
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    BYTE* pGetter = (BYTE*)pOptimizedTlsGetter;
    ClrHeapFree(ClrGetProcessExecutableHeap(), 0, pGetter);
}

#endif  // !SELF_NO_HOST
