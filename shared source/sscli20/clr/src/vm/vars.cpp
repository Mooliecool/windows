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
//
// vars.cpp - Global Var definitions
//

#include "common.h"
#include "vars.hpp"
#include "cordbpriv.h"
#include "eeprofinterfaces.h"


#ifndef DACCESS_COMPILE

//
// Default install library
//
const WCHAR g_pwBaseLibrary[]     = L"mscorlib.dll";
const WCHAR g_pwBaseLibraryName[] = L"mscorlib";
const char g_psBaseLibrary[]      = "mscorlib.dll";
const char g_psBaseLibraryName[]  = "mscorlib";
const char g_psBaseLibrarySatelliteAssemblyName[]  = "mscorlib.resources";


//---------------------------------------------------------
// IMPORTANT:
// If you add a global variable you must clean it up in 
// CoUninitializeEE and set it back to NULL:


HINSTANCE            g_pMSCorEE;
volatile LONG        g_TrapReturningThreads;
#ifdef _DEBUG
// next two variables are used to enforce an ASSERT in Thread::DbgFindThread
// that does not allow g_TrapReturningThreads to creep up unchecked.
volatile LONG        g_trtChgStamp = 0;
volatile LONG        g_trtChgInFlight = 0;

char *               g_ExceptionFile;   // Source of the last thrown exception (COMPLUSThrow())
DWORD                g_ExceptionLine;   // ... ditto ...
void *               g_ExceptionEIP;    // Managed EIP of the last guy to call JITThrow.

#endif

#endif // #ifndef DACCESS_COMPILE
GPTR_IMPL(IdDispenser,      g_pThinLockThreadIdDispenser);

IBCLogger            g_IBCLogger;
EEClass *            g_pNullableEEClass;    // EEClass of the Nullable<T> type

// For [<I1, etc. up to and including [Object
GARY_IMPL(PTR_ArrayTypeDesc, g_pPredefinedArrayTypes, ELEMENT_TYPE_MAX);

GPTR_IMPL(EEConfig, g_pConfig);     // configuration data (from the registry)

GPTR_IMPL(MethodTable,      g_pObjectClass);
GPTR_IMPL(MethodTable,      g_pHiddenMethodTableClass);
GPTR_IMPL(MethodTable,      g_pStringClass);
GPTR_IMPL(MethodTable,      g_pArrayClass);
GPTR_IMPL(MethodTable,      g_pExceptionClass);
GPTR_IMPL(MethodTable,      g_pThreadAbortExceptionClass);
GPTR_IMPL(MethodTable,      g_pOutOfMemoryExceptionClass);
GPTR_IMPL(MethodTable,      g_pStackOverflowExceptionClass);
GPTR_IMPL(MethodTable,      g_pExecutionEngineExceptionClass);
GPTR_IMPL(MethodTable,      g_pDelegateClass);
GPTR_IMPL(MethodTable,      g_pMultiDelegateClass);
GPTR_IMPL(MethodTable,      g_pValueTypeClass);
GPTR_IMPL(MethodTable,      g_pEnumClass);
GPTR_IMPL(MethodTable,      g_pSharedStaticsClass);
GPTR_IMPL(MethodTable,      g_pThreadClass);
GPTR_IMPL(MethodTable,      g_pCriticalFinalizerObjectClass);
GPTR_IMPL(MethodTable,      g_pAsyncFileStream_AsyncResultClass);
GPTR_IMPL(MethodTable,      g_pFreeObjectMethodTable);
GPTR_IMPL(MethodTable,      g_pOverlappedDataClass);

GPTR_IMPL(MethodDesc,       g_pPrepareConstrainedRegionsMethod);
GPTR_IMPL(MethodDesc,       g_pPrepareConstrainedRegionsNoOpMethod);
GPTR_IMPL(MethodDesc,       g_pExecuteBackoutCodeHelperMethod);

GPTR_IMPL(MethodDesc,       g_pObjectCtorMD);
GPTR_IMPL(MethodDesc,       g_pObjectFinalizerMD);

// Global SyncBlock cache
GPTR_IMPL(SyncTableEntry,g_pSyncTable);

GPTR_IMPL (GCHeap,           g_pGCHeap);
#ifdef STRESS_LOG
GPTR_IMPL_INIT(StressLog, g_pStressLog, &StressLog::theLog);
#endif



#ifndef DACCESS_COMPILE

//                                          determines whether the verifier throws an exception when something fails
bool                g_fVerifierOff;

OBJECTHANDLE         g_pPreallocatedOutOfMemoryException;
OBJECTHANDLE         g_pPreallocatedStackOverflowException;
OBJECTHANDLE         g_pPreallocatedExecutionEngineException;
OBJECTHANDLE         g_pPreallocatedRudeThreadAbortException;
OBJECTHANDLE         g_pPreallocatedThreadAbortException;
OBJECTHANDLE         g_pPreallocatedSentinelObject;

CertificateCache *g_pCertificateCache = NULL;

// 
//
// Global System Info
//
SYSTEM_INFO g_SystemInfo;

// support for Event Tracing for Windows (ETW)
CEtwTracer * g_pEtwTracer = NULL;

#endif // #ifndef DACCESS_COMPILE

// support for IPCManager 
GPTR_IMPL(IPCWriterInterface, g_pIPCManagerInterface);


#ifdef DEBUGGING_SUPPORTED
//
// Support for the COM+ Debugger.
//
GPTR_IMPL(DebugInterface,     g_pDebugInterface);
GPTR_IMPL(EEDbgInterfaceImpl, g_pEEDbgInterfaceImpl);
GVAL_IMPL(DWORD,              g_CORDebuggerControlFlags);
GPTR_IMPL(DebugInfoManager,   g_pDebugInfoStore);

#endif // DEBUGGING_SUPPORTED

#ifndef DACCESS_COMPILE

#ifdef PROFILING_SUPPORTED
// Profiling support
HINSTANCE           g_pDebuggerDll = NULL;
ProfilerStatus      g_profStatus = profNone;
ProfControlBlock    g_profControlBlock;
#endif // PROFILING_SUPPORTED


// Global default for Concurrent GC. The default is value is 1
int g_IGCconcurrent = 1;

int g_IGCHoardVM = 0;

//
// Global state variable indicating if the EE is in its init phase.
//
bool g_fEEInit = false;

//
// Global state variables indicating which stage of shutdown we are in
//

#endif // #ifndef DACCESS_COMPILE

GVAL_IMPL(bool, g_fProcessDetach);


#ifndef DACCESS_COMPILE

DWORD g_fEEShutDown = 0;
BOOL g_fForbidEnterEE = false;
bool g_fFinalizerRunOnShutDown = false;
bool g_fManagedAttach = false;
bool g_fNoExceptions = false;

DWORD g_FinalizerWaiterStatus = 0;

const WCHAR g_pwzClickOnceEnv_FullName[] = L"__COR_COMMAND_LINE_APP_FULL_NAME__";
const WCHAR g_pwzClickOnceEnv_Manifest[] = L"__COR_COMMAND_LINE_MANIFEST__";
const WCHAR g_pwzClickOnceEnv_Parameter[] = L"__COR_COMMAND_LINE_PARAMETER__";

DWORD g_dwGlobalSharePolicy = AppDomain::SHARE_POLICY_UNSPECIFIED;

//
// Do we own the lifetime of the process, ie. is it an EXE?
//
bool g_fWeControlLifetime = false;

#ifdef _DEBUG
// The following should only be used for assertions.  (Famous last words).
bool dbg_fDrasticShutdown = false;
#endif
bool g_fInControlC = false;

//
// Cached command line file provided by the host.
//
LPWSTR g_pCachedCommandLine = NULL;
LPWSTR g_pCachedModuleFileName = 0;

// host configuration file. If set, it is added to every AppDomain (fusion context)
LPCWSTR g_pszHostConfigFile = NULL;
SIZE_T  g_dwHostConfigFile = 0;

// AppDomainManager assembly and type names provided as environment variables.
LPWSTR g_wszAppDomainManagerAsm = NULL;
LPWSTR g_wszAppDomainManagerType = NULL;
bool g_fDomainManagerInitialized = false;

//
// Hardcoded Meta-Sig
//

//
// Helper enum with metasig lengths
//
// iterate over the metasig recursing into the complex types
#define DEFINE_METASIG(body)            body,
#define METASIG_ATOM(x)                 + 1
#define METASIG_RECURSE                 1
#define SM(varname, args, retval)       gsigl_SM_ ## varname = 1 + 1 retval args
#define IM(varname, args, retval)       gsigl_IM_ ## varname = 1 + 1 retval args
#define Fld(varname, val)               gsigl_Fld_ ## varname = 1 val
enum _gsigl {
#include "metasig.h"
};

//
// Helper enum with metasig argcount
//
// iterate over the metasig without recursing into the complex types
#define DEFINE_METASIG(body)            body,
#define METASIG_ATOM(x)                 + 1
#define METASIG_RECURSE                 0
#define SM(varname, args, retval)       gsigc_SM_ ## varname = 0 args
#define IM(varname, args, retval)       gsigc_IM_ ## varname = 0 args
#define Fld(varname, val)               gsigc_Fld_ ## varname = 0
enum _gsigc {
#include "metasig.h"
};


//
// The actual array with the hardcoded metasig:
//
// There are 3 variations of the macros for Fields, Static Methods and Instance Methods.
//
// Each of them has 2 flavors: one for the fully baked signatures, and the other 
// for the signatures with unresolved type references
//
// The signatures with unresolved type references are marked with negative size, 
// and the pointer to them is non-const because of it will be overwritten with
// the pointer to the resolved signature at runtime.
//

#define DEFINE_METASIG(body)            body
#define DEFINE_METASIG_T(body)          _##body
#define METASIG_ATOM(x)                 x,
#define METASIG_RECURSE                 1

// define gsig_ ## varname before gsige_ ## varname to give a hint to the compiler about the desired layout

#define SM(varname, args, retval) extern const BYTE gsige_SM_ ## varname[];     \
    const HardCodedMetaSig gsig_SM_ ## varname = { gsige_SM_ ## varname };      \
    const BYTE gsige_SM_ ## varname[] = { gsigl_SM_ ## varname,                 \
        IMAGE_CEE_CS_CALLCONV_DEFAULT, gsigc_SM_ ## varname, retval args };

#define IM(varname, args, retval) extern const BYTE gsige_IM_ ## varname[];     \
    const HardCodedMetaSig gsig_IM_ ## varname = { gsige_IM_ ## varname };      \
    const BYTE gsige_IM_ ## varname[] = { gsigl_IM_ ## varname,                 \
        IMAGE_CEE_CS_CALLCONV_HASTHIS, gsigc_IM_ ## varname, retval args };

#define Fld(varname, val) extern const BYTE gsige_Fld_ ## varname[];            \
    const HardCodedMetaSig gsig_Fld_ ## varname = { gsige_Fld_ ## varname };    \
    const BYTE gsige_Fld_ ## varname[] = { gsigl_Fld_ ## varname,               \
        IMAGE_CEE_CS_CALLCONV_FIELD, val };

#define _SM(varname, args, retval) extern const BYTE gsige_SM_ ## varname[];    \
    HardCodedMetaSig gsig_SM_ ## varname = { gsige_SM_ ## varname };            \
    const BYTE gsige_SM_ ## varname[] = { (BYTE) -gsigl_SM_ ## varname,         \
        IMAGE_CEE_CS_CALLCONV_DEFAULT, gsigc_SM_ ## varname, retval args };

#define _IM(varname, args, retval) extern const BYTE gsige_IM_ ## varname[];    \
    HardCodedMetaSig gsig_IM_ ## varname = { gsige_IM_ ## varname };            \
    const BYTE gsige_IM_ ## varname[] = { (BYTE) -gsigl_IM_ ## varname,         \
        IMAGE_CEE_CS_CALLCONV_HASTHIS, gsigc_IM_ ## varname, retval args };

#define _Fld(varname, val) extern const BYTE gsige_Fld_ ## varname[];           \
    HardCodedMetaSig gsig_Fld_ ## varname = { gsige_Fld_ ## varname };          \
    const BYTE gsige_Fld_ ## varname[] = { (BYTE) -gsigl_Fld_ ## varname,       \
        IMAGE_CEE_CS_CALLCONV_FIELD, val };

#include "metasig.h"

#undef _SM
#undef _IM
#undef _Fld



#ifdef _DEBUG

//
// Make sure DEFINE_METASIG is used for signatures that do not reference other types
//
// counts number of type references in the signature and C_ASSERTs that 
// it is zero. An assertion failure results in error C2118: negative subscript.
#define DEFINE_METASIG(body)            body
#define DEFINE_METASIG_T(body)
#define METASIG_BODY(varname, types)    C_ASSERT(types 0 == 0);
#define METASIG_ATOM(x)                 0+
#define METASIG_RECURSE                 1
#define C(x)                            1+
#define g(x)                            1+
#include "metasig.h"

//
// Make sure DEFINE_METASIG_T is used only for signatures that reference 
// other types.
//
// counts number of type references in the signature and C_ASSERTs that 
// it is non zero. An assertion failure results in error C2118: negative subscript.
#define DEFINE_METASIG(body)
#define DEFINE_METASIG_T(body)          body
#define METASIG_BODY(varname, types)    C_ASSERT(types 0 != 0);
#define METASIG_ATOM(x)                 0+
#define METASIG_RECURSE                 1
#define C(x)                            1+
#define g(x)                            1+
#include "metasig.h"

#endif

//
//
// IJW needs the shim HINSTANCE
//
HINSTANCE g_hInstShim = NULL;

char g_Version[] = VER_PRODUCTVERSION_STR;

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void OBJECTHANDLE_EnumMemoryRegions(OBJECTHANDLE handle)
{
    PTR_TADDR ref = PTR_TADDR(handle);
    if (ref.IsValid())
    {
        ref.EnumMem();
        
        PTR_Object obj = PTR_Object(*ref);
        if (obj.IsValid())
        {
            obj->EnumMemoryRegions();
        }
    }
}

void OBJECTREF_EnumMemoryRegions(OBJECTREF ref)
{
    if (ref.IsValid())
    {
        ref->EnumMemoryRegions();
    }
}

#else


#endif // #ifdef DACCESS_COMPILE

extern "C" volatile const GSCookie s_gsCookie = 0;


