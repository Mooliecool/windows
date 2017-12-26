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
#ifndef DEBUGGER_COMMON_H
#define DEBUGGER_COMMON_H

#ifdef _X86_
#define PTR_TO_CORDB_ADDRESS(_ptr) (CORDB_ADDRESS)(ULONG)(_ptr)
#else
#define PTR_TO_CORDB_ADDRESS(_ptr) (CORDB_ADDRESS)(ULONG_PTR)(_ptr)
#endif //_X86_

#define CORDB_ADDRESS_TO_PTR(_cordb_addr) ((LPVOID)(SIZE_T)(_cordb_addr))


void GetPidDecoratedName(__out_z __out_ecount(cBufSizeInChars) WCHAR * pBuf,
                         int cBufSizeInChars,
                         const WCHAR * pPrefix,
                         DWORD pid);


//
// This macro is used in CORDbgCopyThreadContext().
//
// CORDbgCopyThreadContext() does an intelligent copy
// from pSrc to pDst, respecting the ContextFlags of both contexts.
//
#define CopyContextChunk(_t, _f, _end, _flag)                                  \
{                                                                              \
    LOG((LF_CORDB, LL_INFO1000000,                                             \
         "CP::CTC: copying " #_flag  ":" FMT_ADDR "<---" FMT_ADDR "(%d)\n",    \
         DBG_ADDR(_t), DBG_ADDR(_f), ((UINT_PTR)(_end) - (UINT_PTR)_t)));      \
    memcpy((_t), (_f), ((UINT_PTR)(_end) - (UINT_PTR)(_t)));                     \
}

//
// CORDbgCopyThreadContext() does an intelligent copy from pSrc to pDst,
// respecting the ContextFlags of both contexts.
//
struct DebuggerREGDISPLAY;

extern void CORDbgCopyThreadContext(CONTEXT* pDst, CONTEXT* pSrc);
extern void CORDbgSetDebuggerREGDISPLAYFromContext(DebuggerREGDISPLAY *pDRD,
                                                   CONTEXT* pContext);


#define PSR_RI 41
#define PSR_SS 40
#define RtlIa64IncrementIP(CurrentSlot, Psr, Ip)    \
                                                    \
   switch ((CurrentSlot) & 0x3) {                   \
                                                    \
      /* Advance psr.ri based on current slot number */ \
                                                    \
      case 0:                                       \
         Psr = ((Psr) & ~(3ULL << PSR_RI)) | (1ULL << PSR_RI);     \
         break;                                     \
                                                    \
      case 1:                                       \
         Psr = ((Psr) & ~(3ULL << PSR_RI)) | (2ULL << PSR_RI);     \
         break;                                     \
                                                    \
      case 2:                                       \
         Psr = ((Psr) & ~(3ULL << PSR_RI));         \
         Ip = (Ip) + 16;        /*  Wrap slot number -- need to incr IP */  \
         break;                                     \
                                                    \
      default:                                      \
         break;                                     \
   }

/* ------------------------------------------------------------------------- *
 * Constant declarations
 * ------------------------------------------------------------------------- */

enum
{
    NULL_THREAD_ID = -1,
    NULL_PROCESS_ID = -1
};

/* ------------------------------------------------------------------------- *
 * Macros
 * ------------------------------------------------------------------------- */

//
// CANNOT USE IsBad*Ptr() methods here.  They are *banned* APIs because of various
// reasons (see http://winweb/wincet/bannedapis.htm).
//

#define VALIDATE_POINTER_TO_OBJECT(ptr, type)                                \
if ((ptr) == NULL)                                                           \
{                                                                            \
    return E_INVALIDARG;                                                     \
}

#define VALIDATE_POINTER_TO_OBJECT_OR_NULL(ptr, type)

//
// CANNOT USE IsBad*Ptr() methods here.  They are *banned* APIs because of various
// reasons (see http://winweb/wincet/bannedapis.htm).
//
#define VALIDATE_POINTER_TO_OBJECT_ARRAY(ptr, type, cElt, fRead, fWrite)     \
if ((ptr) == NULL)                                                           \
{                                                                            \
    return E_INVALIDARG;                                                     \
}

#define VALIDATE_POINTER_TO_OBJECT_ARRAY_OR_NULL(ptr, type,cElt,fRead,fWrite)

/* ------------------------------------------------------------------------- *
 * Function Prototypes
 * ------------------------------------------------------------------------- */




// We only want to use ReadProcessMemory if we're truely out of process
#define ReadProcessMemoryI(hProcess,lpBaseAddress,lpBuffer,nSize,lpNumberOfBytesRead) \
    ReadProcessMemory(hProcess,lpBaseAddress,lpBuffer,nSize,lpNumberOfBytesRead)

// Linear search through an array of NativeVarInfos, to find
// the variable of index dwIndex, valid at the given ip.
//
// returns CORDBG_E_IL_VAR_NOT_AVAILABLE if the variable isn't
// valid at the given ip.
//
// This should be inlined
HRESULT FindNativeInfoInILVariableArray(DWORD dwIndex,
                                        SIZE_T ip,
                                        ICorJitInfo::NativeVarInfo **ppNativeInfo,
                                        unsigned int nativeInfoCount,
                                        ICorJitInfo::NativeVarInfo *nativeInfo);


#define VALIDATE_HEAP
//HeapValidate(GetProcessHeap(), 0, NULL);

//  struct DebuggerILToNativeMap:   Holds the IL to Native offset map
//  Great pains are taken to ensure that this each entry corresponds to the
//  first IL instruction in a source line.  It isn't actually a mapping
//  of _every_ IL instruction in a method, just those for source lines.
//  SIZE_T ilOffset:  IL offset of a source line.
//  SIZE_T nativeStartOffset:  Offset within the method where the native
//      instructions corresponding to the IL offset begin.
//  SIZE_T nativeEndOffset:  Offset within the method where the native
//      instructions corresponding to the IL offset end.
//
//  Note: any changes to this struct need to be reflected in
//  COR_DEBUG_IL_TO_NATIVE_MAP in CorDebug.idl. These structs must
//  match exactly.
//
struct DebuggerILToNativeMap
{
    ULONG ilOffset;
    ULONG nativeStartOffset;
    ULONG nativeEndOffset;
    ICorDebugInfo::SourceTypes source;
};

void ExportILToNativeMap(ULONG32 cMap,
             COR_DEBUG_IL_TO_NATIVE_MAP mapExt[],
             struct DebuggerILToNativeMap mapInt[],
             SIZE_T sizeOfCode);





#endif //DEBUGGER_COMMON_H
