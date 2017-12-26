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

Module Name: jit.cpp

Abstract:

    This file contains the code for the JIT DLL, including the DLL
    entry point.

--*/

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#ifdef _MSC_VER
#pragma hdrstop
#endif

#define  _DECL_DLLMAIN
#include "process.h"

#ifndef _DEBUG
#include "clrhost.h"
#endif

#include "config.h"
#include "jit.h"

//----------------------------------------------------------------------
// Global Variables
//----------------------------------------------------------------------

// The one and only JITTER ever returned
static Jit* ILJitter = 0;

// Where to send the logging mesages
ICorJitInfo* logCallback = 0;

// Either an available JitContext or NULL.
// We are assuming that on average we will finish this jit before 
// another starts up. If that proves to be untrue, we'll just allocate 
// new JitContext's as necessary. We delete the extra ones in 
// JitContext::ReleaseContext()

JitContext* next_JitContext;
HINSTANCE g_hInst = NULL;

//----------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------

/*++

    Function: DllMain
    
    Entry point for the JIT DLL.

--*/

extern "C"
BOOL WINAPI
DllMain(HANDLE hInstance, DWORD dwReason, LPVOID pvReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // do any initialization here
        g_hInst = (HINSTANCE)hInstance;

        unsigned int cache_len = 100*4096;

        //allocate the code cache
        if (!Jit::Init(cache_len)) 
        {
            _ASSERTE(0);
        }
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        // do any finalization here

        // free the code cache
        Jit::Terminate();
    }
    return TRUE;
}
/*++

    Function: GetModuleInst

    Returns the HInstance of this module.

--*/

HINSTANCE GetModuleInst()
{
    return g_hInst;
}

/*++

    Function: getJit

    External interface to obtain an instance of the JIT. 

    Issue-REVIEW-2001/04/03-MarkLe ICorJitCompiler as COM object.
    This comment is from the x86 jit sources: "really the ICorJitCompiler 
    should be done as a COM object, this is just something to get us going". 

    Returns an ICorJitCompiler*.

--*/

ICorJitCompiler* __stdcall getJit()
{
    if (ILJitter == 0)
    {
        // no need to check for out of memory, since caller checks for return value of NULL
        ILJitter = new/*(JitBuff)*/ Jit();
        _ASSERTE(ILJitter != NULL);
    }
    return(ILJitter);
}

/*++

    Function: Jit::compileMethod

    The main JIT entrypoint.

--*/


extern void Fe2(COMPILER_INSTANCE *ciPtr);

CorJitResult __stdcall 
Jit::compileMethod (
    ICorJitInfo*           compHnd,            /* IN */
    CORINFO_METHOD_INFO*   info,               /* IN */
    unsigned               flags,              /* IN */
    BYTE **                entryAddress,       /* OUT */
    ULONG *                nativeSizeOfCode    /* OUT */
    )
{

    CorJitResult jitResult;
    JitContext* jitData;
    unsigned char*  entryPoint;
    unsigned int codeSize;
    unsigned int actualCodeSize;

    // Initialize defaults.
    jitResult = CORJIT_INTERNALERROR;
    jitData = NULL;
    entryPoint = NULL;


    // Setup a Jit context.
    jitData = JitContext::GetContext(this, compHnd, info, flags);

    BOOL jitRetry;  // this is set to false unless we get an exception
    // because of underestimation of code buffer size

    // the following loop is expected to execute only once, except when we
    // underestimate the size of the code buffer, in which case, we try again
    // with a larger codeSize

    do
    {
        // Call the actual Jit compiler and check the results.
        codeSize = 0;
        actualCodeSize = codeSize;
        
        jitResult = jitCompile(jitData, &entryPoint, &actualCodeSize);
        jitRetry = false;
    } while (jitRetry);

    *entryAddress = (BYTE*)entryPoint;
    *nativeSizeOfCode = actualCodeSize;

    return jitResult;
}

/*++

    Function: Jit::clearCache

    Notification from the runtime that any caches should be cleaned up.
    
--*/
void __stdcall
Jit::clearCache()
{
    return;
}

/*++

    Function: Jit::isCacheCleanupRequired

    Notify runtime if we have something to clean up
    
--*/
BOOL __stdcall
Jit::isCacheCleanupRequired()
{
    return FALSE;
}

/*++

    Function: Jit::jitCompile

    Compile the method. If successful, return number of bytes 
    jitted, else return 0.

--*/

CorJitResult 
Jit::jitCompile (
    JitContext*   jit,
    BYTE **       entryAddress,
    unsigned *    codeSize
    )
{
    int nErrors;
    int ReadMSIL(JitContext* jitContext, 
                 BYTE ** entryAddress,
                 unsigned * codeSize);


    nErrors = ReadMSIL(jit, entryAddress, codeSize);


    return (nErrors == 0 ? CORJIT_OK : CORJIT_INTERNALERROR);
}

/*++

    Function: Jit::Jit, Jit::~Jit

    Empty default ctor and dtor.

--*/

Jit::Jit() 
{
}

Jit::~Jit() 
{
}

/*++

    Function: Jit::Init

    Initialize JIT.

--*/

BOOL
Jit::Init(unsigned int cache_len)
{
    if (!JitContext::Init()) 
        return FALSE;

   return TRUE;
}

/*++

    Function: Jit::Terminate

    Terminate JIT.

--*/

void 
Jit::Terminate() 
{
    JitContext::Terminate();
    if (ILJitter)
        ILJitter->~Jit();
    ILJitter = NULL;
}

//----------------------------------------------------------------------
// Functions for JitContext
//----------------------------------------------------------------------

/*++

    Function: NewJitContext

    This is the same as New special cased for JitContext since the caller
    has an SEH __try block which is not allowed by the compiler.

--*/

void 
NewJitContext(JitContext** pNewContext, ICorJitInfo* comp)
{
    if ((*pNewContext = new JitContext(comp)) == NULL) 
        RaiseException(JIT64_NOMEMEXCEPTION_CODE,EXCEPTION_NONCONTINUABLE,0,NULL);
}

/*++

    Function: JitContext::GetContext

    Get and initialize a compilation context to use for compiling.

--*/

JitContext* 
JitContext::GetContext(
    Jit*           jitter,
    ICorJitInfo*   comp,
    CORINFO_METHOD_INFO* methInfo,
    DWORD          dwFlags
    )
{
    JitContext* next;

    next = (JitContext*)InterlockedExchangePointer((LPVOID*)&next_JitContext, NULL);
    BOOL gotException = TRUE;
    PAL_TRY 
    {
        /*if the list was empty, make a new one to use */
        if (!next)
        {
            NewJitContext(&next,comp);
        }
        /* set up this one for use */
        next->jitter     = jitter;
        next->jitInfo    = comp;
        next->methodInfo = methInfo;
        next->flags      = dwFlags;
        comp->getEEInfo(&next->eeInfo);
        gotException = FALSE;
    }
    PAL_FINALLY //(EXCEPTION_EXECUTE_HANDLER)
    {
        // cleanup if we get here because of an exception
        if (gotException && (next != NULL))
        {
            next->ReleaseContext();
            next = NULL;
        }
    }
    PAL_ENDTRY

    return next;
}

/*++

    Function: JitContext::ReleaseContext

    Return a compilation context to the free list.
    Exchange with the next_JitContext and if we get one back, delete the one
    we get back. The assumption is that the steady state case is almost no 
    concurrent jits.

--*/

void
JitContext::ReleaseContext(void)
{
    JitContext* next;

    /* mark this context as not in use */
    jitInfo = NULL;
    methodInfo = NULL;
    jitter = NULL;
    _ASSERTE(this != next_JitContext);

    next = (JitContext*)InterlockedExchangePointer((LPVOID*)&next_JitContext, (LPVOID)this);
//  next = (JitContext*) FastInterlockExchange ((long*) next_JitContext, (long*) this);

    _ASSERTE(this != next);                 // I was not on the free 'list'
    if (next) delete next;
}

/*++

    Function: JitContext::Init

    Return a compilation context to the free list.
    Exchange with the next_JitContext and if we get one back, delete the one
    we get back. The assumption is that the steady state case is almost no 
    concurrent jits.

--*/

BOOL
JitContext::Init(void)
{
    next_JitContext = NULL;
    return TRUE;
}

/*++

    Function: JitContext::Terminate

--*/

void
JitContext::Terminate(void)
{
    if (next_JitContext) delete next_JitContext;
    next_JitContext = NULL;
    return;
}

/*++

    Function: JitContext::JitContext, JitContext::~JitContext
    
--*/

JitContext::JitContext(ICorJitInfo* comp) 
{
}

JitContext::~JitContext() 
{
}
