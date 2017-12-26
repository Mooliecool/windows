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

#ifndef _IMETA_H
#define _IMETA_H
/*++

Module Name: imeta.h

Abstract:

    APIs used to get information from the VM.

--*/

//----------------------------------------------------------------------
// Typedefs for passing handles between C (UTC) code and C++ code (EE).
// These handles are really pointers, but we can't pass the actual
// type between the EE (C++) and UTC (C), so we typedef these IMETA
// versions to use in the IMeta APIs.
//----------------------------------------------------------------------

#if defined(CC_SKCPP)

// Advance declarations
class JitContext; 
class GcInfoEncoder;
class GcJitAllocator;

// A jit context
typedef JitContext*                 IMETA_JITCONTEXT;

// A generic handle (used for hashing).
typedef CORINFO_METHOD_HANDLE       IMETA_HANDLE;

// GC related handles
typedef GcJitAllocator*             IMETA_GCJIT_ALLOCATOR_HANDLE;

// Pull in some types from ICorDebugInfo
typedef ICorDebugInfo::OffsetMapping OffsetMapping;
typedef ICorDebugInfo::ILVarInfo ILVarInfo;
typedef ICorDebugInfo::NativeVarInfo NativeVarInfo;
typedef ICorDebugInfo::VarLoc VarLoc;
typedef ICorDebugInfo::VarLocType VarLocType;

#else //(CC_SKCPP)

// A jit context
typedef void* IMETA_JITCONTEXT;

// GC related handles
typedef void* IMETA_GCJIT_ALLOCATOR_HANDLE;


// Pull in some types from ICorDebugInfo
typedef void* OffsetMapping;
typedef void* ILVarInfo;
typedef void* NativeVarInfo;
typedef void* VarLoc;

#endif //(CC_SKCPP)

// Structure for correlating bytecode offsets to file names and line numbers.
typedef struct tagCOMLIMEINFO {
    char *filename;
    unsigned int offset;
    unsigned int linenum;
} COMLINEINFO;

typedef struct tagCILMETASCOPE {
    struct IMetaDataImport *Import;
    struct IMetaDataEmit *Emit;
} CILMETASCOPE;

/* These defines must match the set of extra CorTokenTypes added in wvm.h */

#define    mdtJitHelper            0x73000000
#define    mdtInterfaceOffset      0x74000000
#define    mdtCodeOffset           0x75000000
#define    mdtSyncHandle           0x76000000

// new ones ianb added
#define    mdtVarArgsHandle        0x77000000
#define    mdtPInvokeCalliHandle   0x78000000
#define    mdtIBCProfHandle        0x79000000

#define    mdtMBReturnHandle       0x7A000000
#define    mdtGSCookie             0x7B000000

#define    mdtJMCHandle            0x7C000000
#define    mdtCaptureThreadGlobal  0x7D000000

#endif // _IMETA_H
