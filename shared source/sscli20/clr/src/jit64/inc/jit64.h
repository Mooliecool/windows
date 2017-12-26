/*++

Copyright (c) 2005 Microsoft Corporation

Module Name: jit64.h

Abstract:

    Declarations and defines for things that are common
    to the C++ and the C files used by all binaries
    built here (mscorjit, mcorpjt and peverify)

Note:
    This file is included by both C and C++ source files

--*/

#ifndef _JIT64_H_
#define _JIT64_H_


// ---------------------- HRESULT value definitions -----------------
//
// HRESULT definitions
//
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Internal JIT exceptions.

#define FACILITY_JIT64		0x64	// This is a made up facility code

#define JIT64_FATAL_ERROR       1L	// Some fatal error occurred
#define JIT64_READER_ERROR      2L	// An error occurred in the reader
#define JIT64_NOMEM_ERROR       3L	// An out of memory error occurred in the JIT64

#define JIT64_FATALEXCEPTION_CODE    (0xE0000000 | FACILITY_JIT64 << 16 | JIT64_FATAL_ERROR)
#define JIT64_READEREXCEPTION_CODE   (0xE0000000 | FACILITY_JIT64 << 16 | JIT64_READER_ERROR)
#define JIT64_NOMEMEXCEPTION_CODE    (0xE0000000 | FACILITY_JIT64 << 16 | JIT64_NOMEM_ERROR)

//===========================================================================

// Function: __JITfilter
// 
//  Filter to detect/handle internal JIT exceptions. 
//  Returns EXCEPTION_EXECUTE_HANDLER for JIT64 exceptions, 
//  and EXCEPTION_CONTINUE_SEARCH for all others.
//
#ifdef __cplusplus
extern "C"
#endif
int __JITfilter(int exceptCode, void *exceptInfo);

// Global environment config variables (set by GetConfigString).
// These are defined/set in jit.cpp.

#ifdef __cplusplus
extern "C" {
#endif

extern UINT EnvConfigCseOn;
#ifndef NDEBUG
extern UINT EnvConfigCseBinarySearch;
extern UINT EnvConfigCseMax;
extern UINT EnvConfigCopyPropMax;
extern UINT EnvConfigDeadCodeMax;
extern UINT EnvConfigCseStats;
#endif // !NDEBUG
extern UINT EnvConfigPInvokeInline;
extern UINT EnvConfigPInvokeCalliOpt;
extern UINT EnvConfigNewGCCalc;
extern UINT EnvConfigTurnOffDebugInfo;
extern WCHAR* EnvConfigJitName;
extern UINT EnvConfigTurnOnIBC;

extern BOOL HaveEnvConfigCseOn;
extern BOOL HaveEnvConfigCseStats;
#ifndef NDEBUG
extern BOOL HaveEnvConfigCseBinarySearch;
extern BOOL HaveEnvConfigCseMax;
extern BOOL HaveEnvConfigCopyPropMax;
extern BOOL HaveEnvConfigDeadCodeMax;
#endif // !NDEBUG
extern BOOL HaveEnvConfigPInvokeInline;
extern BOOL HaveEnvConfigPInvokeCalliOpt;
extern BOOL HaveEnvConfigNewGCCalc;
extern BOOL HaveEnvConfigTurnOffDebugInfo;
extern BOOL HaveEnvConfigJitName;
extern BOOL HaveEnvConfigTurnOnIBC;

} // extern "C" 

#endif // _JIT64_H_
