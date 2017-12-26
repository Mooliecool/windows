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
// vars.hpp
//
// Global variables
//
#ifndef _VARS_HPP
#define _VARS_HPP

// This will need ifdefs for non-x86 processors (ia64 is pointer to 128bit instructions)!
#define SLOT    PBYTE
typedef DPTR(SLOT) PTR_SLOT;

typedef LPVOID  DictionaryEntry;

/* Define the implementation dependent size types */

#ifndef _INTPTR_T_DEFINED
typedef int                 intptr_t;
#define _INTPTR_T_DEFINED
#endif

#ifndef _UINTPTR_T_DEFINED
typedef unsigned int        uintptr_t;
#define _UINTPTR_T_DEFINED
#endif

#ifndef _PTRDIFF_T_DEFINED
typedef int                 ptrdiff_t;
#define _PTRDIFF_T_DEFINED
#endif


#ifndef _SIZE_T_DEFINED
typedef unsigned int     size_t;
#define _SIZE_T_DEFINED
#endif


#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#include "util.hpp"
#include <corpriv.h>
#include <cordbpriv.h>
#include "eeprofinterfaces.h"
#include "profilepriv.h"
#include "eehash.h"
#include "certificatecache.h"

class ClassLoader;
class LoaderHeap;
class GCHeap;
class Object;
class StringObject;
class TransparentProxyObject;
class ArrayClass;
class MethodTable;
class MethodDesc;
class SyncBlockCache;
class SyncTableEntry;
class ThreadStore;
class IPCWriterInterface;
class CEtwTracer;
class DebugInterface;
class DebugInfoManager;
class EEDbgInterfaceImpl;
class EECodeManager;
class Crst;
class BBSweep;


//
// object handles are opaque types that track object pointers
//
#ifndef DACCESS_COMPILE

struct OBJECTHANDLE__
{
    void* unused;
};
typedef struct OBJECTHANDLE__* OBJECTHANDLE;

#else

typedef TADDR OBJECTHANDLE;

#endif

//
// _UNCHECKED_OBJECTREF is for code that can't deal with DEBUG OBJECTREFs
//
typedef PTR_Object _UNCHECKED_OBJECTREF;
typedef DPTR(PTR_Object) PTR_UNCHECKED_OBJECTREF;

#if defined(_DEBUG_IMPL) // && !defined(_IA64_)
#define USE_CHECKED_OBJECTREFS
#else
#undef  USE_CHECKED_OBJECTREFS
#endif

#ifndef DEFINE_OBJECTREF
#define DEFINE_OBJECTREF
#ifdef USE_CHECKED_OBJECTREFS
class OBJECTREF;
#else
typedef PTR_Object OBJECTREF;
#endif
#endif

#ifdef DACCESS_COMPILE
void OBJECTHANDLE_EnumMemoryRegions(OBJECTHANDLE handle);
void OBJECTREF_EnumMemoryRegions(OBJECTREF ref);
#endif


#ifdef USE_CHECKED_OBJECTREFS


//=========================================================================
// In the retail build, OBJECTREF is typedef'd to "Object*".
// In the debug build, we use operator overloading to detect
// common programming mistakes that create GC holes. The critical
// rules are:
//
//   1. Your thread must have disabled preemptive GC before
//      reading or writing any OBJECTREF. When preemptive GC is enabled,
//      another other thread can suspend you at any time and
//      move or discard objects.
//   2. You must guard your OBJECTREF's using a root pointer across
//      any code that might trigger a GC.
//
// Each of the overloads validate that:
//
//   1. Preemptive GC is currently disabled
//   2. The object looks consistent (checked by comparing the
//      object's methodtable pointer with that of the class.)
//
// Limitations:
//    - Can't say
//
//          if (or) {}
//
//      must say
//
//          if (or != NULL) {}
//
//
//=========================================================================
class OBJECTREF {
    private:
        // Holds the real object pointer.
        // The union gives us better debugger pretty printing
    union {
        Object *m_asObj;
        class StringObject* m_asString;
        class ArrayBase* m_asArray;
        class PtrArray* m_asPtrArray;
        class DelegateObject* m_asDelegate;
        class TransparentProxyObject* m_asTP;

        class ReflectClassBaseObject* m_asReflectClass;
        class CompressedStackObject* m_asCompressedStack;
        class SecurityContextObject* m_asSecurityContext;
        class ExecutionContextObject* m_asExecutionContext;
        class AppDomainBaseObject* m_asAppDomainBase;
        class PermissionSetObject* m_asPermissionSetObject;
    };

    public:
        //-------------------------------------------------------------
        // Default constructor, for non-initializing declarations:
        //
        //      OBJECTREF or;
        //-------------------------------------------------------------
        OBJECTREF();

        //-------------------------------------------------------------
        // Copy constructor, for passing OBJECTREF's as function arguments.
        //-------------------------------------------------------------
        OBJECTREF(const OBJECTREF & objref);

        //-------------------------------------------------------------
        // To allow NULL to be used as an OBJECTREF.
        //-------------------------------------------------------------
        OBJECTREF(TADDR nul);

        //-------------------------------------------------------------
        // Test against NULL.
        //-------------------------------------------------------------
        int operator!() const;

        //-------------------------------------------------------------
        // Compare two OBJECTREF's.
        //-------------------------------------------------------------
        int operator==(const OBJECTREF &objref) const;

        //-------------------------------------------------------------
        // Compare two OBJECTREF's.
        //-------------------------------------------------------------
        int operator!=(const OBJECTREF &objref) const;

        //-------------------------------------------------------------
        // Forward method calls.
        //-------------------------------------------------------------
        Object* operator->();
        const Object* operator->() const;

        //-------------------------------------------------------------
        // Assignment. We don't validate the destination so as not
        // to break the sequence:
        //
        //      OBJECTREF or;
        //      or = ...;
        //-------------------------------------------------------------
        OBJECTREF& operator=(const OBJECTREF &objref);
        OBJECTREF& operator=(TADDR nul);

            // allow explict casts
        explicit OBJECTREF(Object *pObject);
};

//-------------------------------------------------------------
//  template class REF for different types of REF class to be used
//  in the debug mode
//  Template type should be a class that extends Object
//-------------------------------------------------------------



template <class T>
class REF : public OBJECTREF
{
    public:

        //-------------------------------------------------------------
        // Default constructor, for non-initializing declarations:
        //
        //      OBJECTREF or;
        //-------------------------------------------------------------
      REF() :OBJECTREF ()
        {
            LEAF_CONTRACT;
            // no op
        }

        //-------------------------------------------------------------
        // Copy constructor, for passing OBJECTREF's as function arguments.
        //-------------------------------------------------------------
      explicit REF(const OBJECTREF& objref) : OBJECTREF(objref)
        {
            LEAF_CONTRACT;
            //no op
        }


        //-------------------------------------------------------------
        // To allow NULL to be used as an OBJECTREF.
        //-------------------------------------------------------------
      REF(TADDR nul) : OBJECTREF (nul)
        {
            LEAF_CONTRACT;
            // no op
        }

      explicit REF(T* pObject) : OBJECTREF(pObject)
        {
            LEAF_CONTRACT;
            // no op
        }

        //-------------------------------------------------------------
        // Forward method calls.
        //-------------------------------------------------------------
        T* operator->()
        {
            LEAF_CONTRACT;
            return (T *)OBJECTREF::operator->();
        }

        const T* operator->() const
        {
            LEAF_CONTRACT;
            return (const T *)OBJECTREF::operator->();
        }

        //-------------------------------------------------------------
        // Assignment. We don't validate the destination so as not
        // to break the sequence:
        //
        //      OBJECTREF or;
        //      or = ...;
        //-------------------------------------------------------------
        REF<T> &operator=(OBJECTREF &objref)
        {
            LEAF_CONTRACT;
            return (REF<T>&)OBJECTREF::operator=(objref);
        }

};

#define VALIDATEOBJECTREF(objref) ((objref)->Validate())

#define ObjectToOBJECTREF(obj)     (OBJECTREF(obj))
#define OBJECTREFToObject(objref)  ((objref).operator-> ())
#define ObjectToSTRINGREF(obj)     (STRINGREF(obj))
#define STRINGREFToObject(objref)  (*( (StringObject**) &(objref) ))
#define ObjectToSTRINGBUFFERREF(obj)    (STRINGBUFFERREF(obj))
#define STRINGBUFFERREFToObject(objref) (*( (StringBufferObject**) &(objref) ))

#else   // _DEBUG_IMPL

#define VALIDATEOBJECTREF(objref)

#define ObjectToOBJECTREF(obj)    ((PTR_Object) (obj))
#define OBJECTREFToObject(objref) ((PTR_Object) (objref))
#define ObjectToSTRINGREF(obj)    ((PTR_StringObject) (obj))
#define STRINGREFToObject(objref) ((PTR_StringObject) (objref))
#define ObjectToSTRINGBUFFERREF(obj)    ((Ptr_StringBufferObject) (obj))
#define STRINGBUFFERREFToObject(objref) ((Ptr_StringBufferObject) (objref))

#endif // _DEBUG_IMPL


#define MAX_CLASSNAME_LENGTH    1024
#define MAX_NAMESPACE_LENGTH    1024

class EEConfig;
class ClassLoaderList;
class Module;
class ArrayTypeDesc;

#define EXTERN extern

// For [<I1, etc. up to and including [Object
GARY_DECL(PTR_ArrayTypeDesc, g_pPredefinedArrayTypes, ELEMENT_TYPE_MAX);
EXTERN HINSTANCE            g_pMSCorEE;
EXTERN volatile LONG        g_TrapReturningThreads;
EXTERN BBSweep              g_BBSweep;
EXTERN IBCLogger            g_IBCLogger;
EXTERN EEClass *            g_pNullableEEClass;

#ifdef _DEBUG
// next two variables are used to enforce an ASSERT in Thread::DbgFindThread
// that does not allow g_TrapReturningThreads to creep up unchecked.
EXTERN volatile LONG        g_trtChgStamp;
EXTERN volatile LONG        g_trtChgInFlight;
EXTERN char *               g_ExceptionFile;
EXTERN DWORD                g_ExceptionLine;
EXTERN void *               g_ExceptionEIP;
#endif
GPTR_DECL(EEConfig,         g_pConfig);             // configuration data (from the registry)
GPTR_DECL(MethodTable,      g_pObjectClass);
GPTR_DECL(MethodTable,      g_pHiddenMethodTableClass);
GPTR_DECL(MethodTable,      g_pStringClass);
GPTR_DECL(MethodTable,      g_pArrayClass);
GPTR_DECL(MethodTable,      g_pExceptionClass);
GPTR_DECL(MethodTable,      g_pThreadAbortExceptionClass);
GPTR_DECL(MethodTable,      g_pOutOfMemoryExceptionClass);
GPTR_DECL(MethodTable,      g_pStackOverflowExceptionClass);
GPTR_DECL(MethodTable,      g_pExecutionEngineExceptionClass);
GPTR_DECL(MethodTable,      g_pThreadAbortExceptionClass);
GPTR_DECL(MethodTable,      g_pDelegateClass);
GPTR_DECL(MethodTable,      g_pMultiDelegateClass);
GPTR_DECL(MethodTable,      g_pFreeObjectMethodTable);
GPTR_DECL(MethodTable,      g_pValueTypeClass);
GPTR_DECL(MethodTable,      g_pEnumClass);
GPTR_DECL(MethodTable,      g_pThreadClass);
GPTR_DECL(MethodTable,      g_pCriticalFinalizerObjectClass);
GPTR_DECL(MethodTable,      g_pAsyncFileStream_AsyncResultClass);
GPTR_DECL(MethodTable,      g_pOverlappedDataClass);

GPTR_DECL(MethodDesc,       g_pPrepareConstrainedRegionsMethod);
GPTR_DECL(MethodDesc,       g_pPrepareConstrainedRegionsNoOpMethod);
GPTR_DECL(MethodDesc,       g_pExecuteBackoutCodeHelperMethod);

GPTR_DECL(MethodDesc,       g_pObjectCtorMD);
GPTR_DECL(MethodDesc,       g_pObjectFinalizerMD);
GPTR_DECL(GCHeap,           g_pGCHeap);

MethodTable *        TheSByteClass();
MethodTable *        TheInt16Class();
MethodTable *        TheInt32Class();

MethodTable *        TheByteClass();
MethodTable *        TheByteArrayClass();
MethodTable *        TheUInt16Class();
MethodTable *        TheUInt32Class();

MethodTable *        TheBooleanClass();
MethodTable *        TheSingleClass();
MethodTable *        TheDoubleClass();

MethodTable *        TheIntPtrClass();
MethodTable *        TheUIntPtrClass();

inline
MethodTable *        TheFnPtrClass()
{
    WRAPPER_CONTRACT;
    return TheUIntPtrClass();
}

EXTERN bool                 g_fVerifierOff;

// Global System Information
extern SYSTEM_INFO g_SystemInfo;

EXTERN OBJECTHANDLE         g_pPreallocatedOutOfMemoryException;
EXTERN OBJECTHANDLE         g_pPreallocatedStackOverflowException;
EXTERN OBJECTHANDLE         g_pPreallocatedExecutionEngineException;
EXTERN OBJECTHANDLE         g_pPreallocatedRudeThreadAbortException;

// We may not be able to create a normal thread abort exception if OOM or StackOverFlow.
// When this happens, we will use our pre-allocated thread abort exception.
EXTERN OBJECTHANDLE         g_pPreallocatedThreadAbortException;

// we use this as a dummy object to indicate free space in the handle tables -- this object is never visible to the world
EXTERN OBJECTHANDLE         g_pPreallocatedSentinelObject;

// Global SyncBlock cache
typedef DPTR(SyncTableEntry) PTR_SyncTableEntry;
GPTR_DECL(SyncTableEntry, g_pSyncTable);


EXTERN CertificateCache *g_pCertificateCache;

// support for IPCManager
typedef DPTR(IPCWriterInterface) PTR_IPCWriterInterface;
GPTR_DECL(IPCWriterInterface,  g_pIPCManagerInterface);

// support for Event Tracing for Windows (ETW)
EXTERN CEtwTracer* g_pEtwTracer;

#ifdef STRESS_LOG
class StressLog;
typedef DPTR(StressLog) PTR_StressLog;
GPTR_DECL(StressLog, g_pStressLog);
#endif

#ifdef DEBUGGING_SUPPORTED
//
// Support for the COM+ Debugger.
//
GPTR_DECL(DebugInterface,     g_pDebugInterface);
GPTR_DECL(EEDbgInterfaceImpl, g_pEEDbgInterfaceImpl);
GVAL_DECL(DWORD,              g_CORDebuggerControlFlags);

GPTR_DECL(DebugInfoManager,   g_pDebugInfoStore);


#endif // DEBUGGING_SUPPORTED

#ifdef PROFILING_SUPPORTED
EXTERN HINSTANCE            g_pDebuggerDll;
#endif

// Global default for Concurrent GC. The default is on (value 1)
EXTERN int g_IGCconcurrent;
extern int g_IGCHoardVM;



//
// Can we run managed code?
//
BOOL CanRunManagedCode(BOOL fCannotRunIsUserError = TRUE, HINSTANCE hInst = 0);
BOOL CanRunManagedCode(HINSTANCE hInst);

//
// Global state variable indicating if the EE is in its init phase.
//
EXTERN bool g_fEEInit;

//
// Global state variable indicating if the EE has been started up.
//
EXTERN BOOL g_fEEStarted;


//
// Global state variables indicating which stage of shutdown we are in
//
EXTERN DWORD g_fEEShutDown;
EXTERN DWORD g_fFastExitProcess;
#ifndef DACCESS_COMPILE
EXTERN BOOL g_fSuspendOnShutdown;
#endif // DACCESS_COMPILE
EXTERN BOOL g_fForbidEnterEE;
EXTERN bool g_fFinalizerRunOnShutDown;
GVAL_DECL(bool, g_fProcessDetach);
EXTERN bool g_fManagedAttach;
EXTERN bool g_fNoExceptions;

enum FWStatus
{
    FWS_WaitInterrupt = 0x00000001,
};

EXTERN DWORD g_FinalizerWaiterStatus;
extern ULONGLONG g_ObjFinalizeStartTime;
extern volatile BOOL g_FinalizerIsRunning;

extern LONG GetProcessedExitProcessEventCount();

//
// Default install library
//
#ifndef DACCESS_COMPILE
EXTERN const WCHAR g_pwBaseLibrary[];
EXTERN const WCHAR g_pwBaseLibraryName[];
EXTERN const char g_psBaseLibrary[];
EXTERN const char g_psBaseLibraryName[];
EXTERN const char g_psBaseLibrarySatelliteAssemblyName[];

#endif // DACCESS_COMPILE

EXTERN const WCHAR g_pwzClickOnceEnv_FullName[];
EXTERN const WCHAR g_pwzClickOnceEnv_Manifest[];
EXTERN const WCHAR g_pwzClickOnceEnv_Parameter[];

EXTERN DWORD g_dwGlobalSharePolicy;

//
// Do we own the lifetime of the process, ie. is it an EXE?
//
EXTERN bool g_fWeControlLifetime;

#ifdef _DEBUG
// The following should only be used for assertions.  (Famous last words).
EXTERN bool dbg_fDrasticShutdown;
#endif
EXTERN bool g_fInControlC;

// There is a global table of prime numbers that's available for e.g. hashing
extern const DWORD g_rgPrimes[71];

//
// Cached command line file provided by the host.
//
extern LPWSTR g_pCachedCommandLine;
extern LPWSTR g_pCachedModuleFileName;

//
// Host configuration file. One per process.
//
extern LPCWSTR g_pszHostConfigFile;
extern SIZE_T  g_dwHostConfigFile;

// AppDomainManager type
extern LPWSTR g_wszAppDomainManagerAsm;
extern LPWSTR g_wszAppDomainManagerType;
extern bool g_fDomainManagerInitialized;

#ifdef DEBUGGING_SUPPORTED

//
// Macros to check debugger and profiler settings.
//
inline bool CORDebuggerAttached()
{
    LEAF_CONTRACT;
    // If we're in rude shutdown, then pretend the debugger is detached.
    // We want shutdown to be as simple as possible, so this avoids
    // us trying to do elaborate operations while exiting.
    return (g_CORDebuggerControlFlags & DBCF_ATTACHED) && !g_fProcessDetach;
}

// We always track jit info, regardless of what the module bits say.
#define CORDebuggerTrackJITInfo(dwDebuggerBits) (true)

#define CORDebuggerAllowJITOpts(dwDebuggerBits)           \
    (((dwDebuggerBits) & DACF_ALLOW_JIT_OPTS)             \
     ||                                                   \
     ((g_CORDebuggerControlFlags & DBCF_ALLOW_JIT_OPT) && \
      !((dwDebuggerBits) & DACF_USER_OVERRIDE)))

#define CORDebuggerEnCMode(dwDebuggerBits)                         \
    ((dwDebuggerBits) & DACF_ENC_ENABLED)

#define CORDebuggerTraceCall() \
    (CORDebuggerAttached() && GetThread()->IsTraceCall())

#define CORLaunchedByDebugger() \
    (g_CORDebuggerControlFlags & DBCF_GENERATE_DEBUG_CODE)

#else

#define CORDebuggerEnCMode(dwDebuggerBits) (0)

#endif // DEBUGGING_SUPPORTED



//
// Define stuff for precedence between profiling and debugging
// flags that can both be set.
//

#ifdef PROFILING_SUPPORTED

#ifdef DEBUGGING_SUPPORTED

#define CORDisableJITOptimizations(dwDebuggerBits)        \
         (CORProfilerDisableOptimizations() ||            \
          !CORDebuggerAllowJITOpts(dwDebuggerBits))

#else // !DEBUGGING_SUPPORTED

#define CORDisableJITOptimizations(dwDebuggerBits)        \
         CORProfilerDisableOptimizations()

#endif// DEBUGGING_SUPPORTED

#else // !PROFILING_SUPPORTED

#ifdef DEBUGGING_SUPPORTED

#define CORDisableJITOptimizations(dwDebuggerBits)        \
          !CORDebuggerAllowJITOpts(dwDebuggerBits)

#endif// DEBUGGING_SUPPORTED

#endif// PROFILING_SUPPORTED






#define DEFINE_METASIG(body)            extern const body
#define DEFINE_METASIG_T(body)          extern body
#define METASIG_BODY(varname, types)    HardCodedMetaSig gsig_ ## varname;
#include "metasig.h"


//                     Undefine this once our native calling convention follows VC style
// of returned values and passed arguments containing garbage in unused
// high bits.
#define WRONGCALLINGCONVENTIONHACK

// This is an enum to make it very clear who should be using certain
// entrypoints. Currently, this is used in DispatchImplementationTable.
enum tag_RestrictedUse {
    e_TransparentProxyUseOnly,
    e_DynamicMethodTableUseOnly,
};

//
// IJW needs the shim HINSTANCE
//
EXTERN HINSTANCE g_hInstShim;



#define MAXULONG    0xffffffff  // winnt
#define MAXULONGLONG                     UI64(0xffffffffffffffff)


// ADID is an ID for an appdomain that is sparse and remains unique within the process for the lifetime of the process.
// ADIndex is an ID for an appdomain that's dense and may be reused once the appdomain is unloaded.
// Remoting and (I believe) the thread pool use the former as a way of referring to appdomains outside of their normal lifetime safely.
// Interop also uses ADID to handle issues involving unloaded domains.

struct ADIndex
{
    DWORD m_dwIndex;
    ADIndex ()
    : m_dwIndex(0)
    {}
    explicit ADIndex (DWORD id)
    : m_dwIndex(id)
    {}
    BOOL operator==(const ADIndex& ad) const
    {
        return m_dwIndex == ad.m_dwIndex;
    }
    BOOL operator!=(const ADIndex& ad) const
    {
        return m_dwIndex != ad.m_dwIndex;
    }
};

struct ADID
{
    DWORD m_dwId;
    ADID ()
    : m_dwId(0)
    {LEAF_CONTRACT;}
    explicit ADID (DWORD id)
    : m_dwId(id)
    {LEAF_CONTRACT;}
    BOOL operator==(const ADID& ad) const
    {
        LEAF_CONTRACT;
        return m_dwId == ad.m_dwId;
    }
    BOOL operator!=(const ADID& ad) const
    {
        LEAF_CONTRACT;
        return m_dwId != ad.m_dwId;
    }
};

//-----------------------------------------------------------------------------
// GSCookies (guard-stack cookies) for detecting buffer overruns
//-----------------------------------------------------------------------------

typedef DPTR(GSCookie) PTR_GSCookie;

// const is so that it gets placed in the .text section (which is read-only)
// volatile is so that accesses to it do not get optimized away because of the const
extern "C" volatile const GSCookie s_gsCookie;

inline
GSCookie * GetProcessGSCookiePtr() { return  const_cast<GSCookie *>(&s_gsCookie); }

inline
GSCookie GetProcessGSCookie() { return *(volatile GSCookie *)(&s_gsCookie); }


#endif /* _VARS_HPP */
