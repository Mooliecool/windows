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

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _FN_TABLE_ACCESS_H
#define _FN_TABLE_ACCESS_H




struct FakeEEJitManager
{
    LPVOID      __VFN_table;
    LPVOID      m_jit;
    LPVOID      m_next;
    DWORD       m_CodeType;
    DWORD       m_IsDefaultCodeMan;
    LPVOID      m_runtimeSupport;
    LPVOID      m_JITCompiler;
    LPVOID      m_pCodeHeap;
    LPVOID      m_pCodeHeapCritSec;
};

struct FakeHeapList
{
    FakeHeapList*       hpNext;
    LPVOID              pHeap;          // changed type from LoaderHeap*
    DWORD_PTR           startAddress;   // changed from PBYTE
    DWORD_PTR           endAddress;     // changed from PBYTE
    volatile PBYTE      changeStart;
    volatile PBYTE      changeEnd;
    DWORD_PTR           mapBase;        // changed from PBYTE
    DWORD_PTR           pHdrMap;        // changed from DWORD*
    size_t              maxCodeHeapSize;
    DWORD               cBlocks;
    DWORD               bFull;
};

#ifdef USE_INDIRECT_CODEHEADER
typedef struct _FakeHpRealCodeHdr
{
    LPVOID              phdrJitEHInfo;  // changed from EE_ILEXCEPTION*
    LPVOID              phdrJitGCInfo;  // changed from BYTE*
    LPVOID              hdrMDesc;       // changed from MethodDesc*
} FakeRealCodeHeader;

typedef struct _FakeHpCodeHdr
{
    LPVOID              pRealCodeHeader;
} FakeCodeHeader;

#else   // USE_INDIRECT_CODEHEADER
typedef struct _FakeHpCodeHdr
{
    LPVOID              phdrJitEHInfo;  // changed from EE_ILEXCEPTION*
    LPVOID              phdrJitGCInfo;  // changed from BYTE*
    LPVOID              hdrMDesc;       // changed from MethodDesc*
} FakeCodeHeader;
#endif  // USE_INDIRECT_CODEHEADER



enum FakeEEDynamicFunctionTableType
{
    FAKEDYNFNTABLE_JIT = 0,
    FAKEDYNFNTABLE_STUB = 1,
};



BOOL WINAPI             DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID pReserved);
//NTSTATUS                OutOfProcessFindHeader(HANDLE hProcess, DWORD_PTR pMapIn, DWORD_PTR addr, DWORD_PTR &codeHead);
extern "C" NTSTATUS     OutOfProcessFunctionTableCallback(IN HANDLE hProcess, IN PVOID TableAddress, OUT PULONG pnEntries, OUT PRUNTIME_FUNCTION* ppFunctions);


#endif //_FN_TABLE_ACCESS_H
