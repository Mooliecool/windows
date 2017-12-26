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

/*++

Module Name: jit.h

Abstract:

    This file contains functionality needed for the JIT DLL.

--*/

#ifndef _JIT_H_
#define _JIT_H_

#include "jit64.h"

// Assuming:  there is only a single Jit instance, an instance of the 
// JitContext class holds all of the compilation specific data.

class Jit; // Advance declaration
typedef struct CI_TAG CI, COMPILER_INSTANCE, *PCI, *PCOMPILER_INSTANCE;

class JitContext 
{
public:
    // Data
    Jit*            jitter;   //the fjit we are being used by
    ICorJitInfo*    jitInfo;  //interface to EE, passed in when comp starts
    DWORD           flags;          //compilation directives
    CORINFO_METHOD_INFO*methodInfo;     //see corjit.h
    unsigned int    methodAttributes;//see corjit.h


    CORINFO_EE_INFO eeInfo;

    // Methods
    JitContext(ICorJitInfo* comp);
    ~JitContext();

    /* get and initialize a compilation context to use for compiling */
    static JitContext* GetContext(
        Jit*           jitter,
        ICorJitInfo*       comp,
        CORINFO_METHOD_INFO* methInfo,
        DWORD           dwFlags
        );

    /* return a compilation context to the free list */
    void ReleaseContext();

    /* make sure the list of available compilation contexts is */
    /* initialized at startup */
    static BOOL Init();

    /* release all of the compilation contexts at shutdown */
    static void Terminate();

    /* compute the size of an argument based on machine chip */
    unsigned int computeArgSize(CorInfoType argType, 
        CORINFO_ARG_LIST_HANDLE argSig, CORINFO_SIG_INFO* sig);

    /* compress the gc info into gcHdrInfo and answer the size in bytes */
    unsigned int compressGCHdrInfo();

    /* grow a bool[] array by allocating a new one and copying the old */
    /* values into it, return the size of the new array */
    static unsigned growBooleans(bool** bools, unsigned bools_len, 
        unsigned new_bools_size);

    /* grow an unsigned char[] array by allocating a new one and copying */
    /* the old values into it, return the size of the new array */
    static unsigned growBuffer(BYTE** chars, unsigned chars_len, 
        unsigned new_chars_size);

    unsigned writeInstr(BYTE *src, unsigned size);

    void resetContextState(); // resets all state info so the method can be rejitted 

private:
};

class Jit: public ICorJitCompiler {
public:

    Jit();
    ~Jit();

    /* the jitting function */
    CorJitResult __stdcall compileMethod (
        ICorJitInfo*            comp,               /* IN */
        CORINFO_METHOD_INFO*    info,               /* IN */
        unsigned                flags,              /* IN */
        BYTE **                 nativeEntry,        /* OUT */
        ULONG *                 nativeSizeOfCode    /* OUT */
        );

    void __stdcall clearCache();
    BOOL __stdcall isCacheCleanupRequired();

    static BOOL Init(unsigned int cache_len);
    static void Terminate();

    CorJitResult jitCompile(
        JitContext*     jitData,
        BYTE **         entryAddress,
        unsigned *      codeSize              /* IN/OUT */
        );
private:
};

#endif // _JIT_H_
