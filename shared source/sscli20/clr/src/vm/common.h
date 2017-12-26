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
// common.h - precompiled headers include for the COM+ Execution Engine
//

#ifndef _common_h_ 
#define _common_h_


#define STUBS_AS_IL_ONLY(x)
#define NOT_STUBS_AS_IL(x)      x

#ifdef FJITONLY
#define FJIT
#endif

#define USE_COM_CONTEXT_DEF

#ifdef _DEBUG 
#define DEBUG_REGDISPLAY
#endif

#ifdef _MSC_VER 

    // These don't seem useful, so turning them off is no big deal
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4510)   // can't generate default constructor
//#pragma warning(disable:4511)   // can't generate copy constructor
#pragma warning(disable:4512)   // can't generate assignment constructor
#pragma warning(disable:4610)   // user defined constructor required
#pragma warning(disable:4211)   // nonstandard extention used (char name[0] in structs)
#pragma warning(disable:4268)   // 'const' static/global data initialized with compiler generated default constructor fills the object with zeros
#pragma warning(disable:4238)   // nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable:4291)   // no matching operator delete found
#pragma warning(disable:4345)   // behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized

    // Depending on the code base, you may want to not disable these
#pragma warning(disable:4245)   // assigning signed / unsigned
//#pragma warning(disable:4146)   // unary minus applied to unsigned
#pragma warning(disable:4244)   // loss of data int -> char ..
#pragma warning(disable:4127)   // conditional expression is constant
#pragma warning(disable:4100)   // unreferenced formal parameter

#ifndef DEBUG 
#pragma warning(disable:4189)   // local variable initialized but not used
#pragma warning(disable:4505)   // unreferenced local function has been removed
//#pragma warning(disable:4702)   // unreachable code
#pragma warning(disable:4313)   // 'format specifier' in format string conflicts with argument %d of type 'type'
#endif

#pragma warning(disable:4063)   // bad switch value for enum (only in Disasm.cpp)
#pragma warning(disable:4710)   // function not inlined
#pragma warning(disable:4527)   // user-defined destructor required
#pragma warning(disable:4513)   // destructor could not be generated

//#pragma warning(disable:4701)   // local variable may be used without being initialized
#endif // _MSC_VER

#define _CRT_DEPENDENCY_   //this code depends on the crt file functions


#include <winwrap.h>

#include <windows.h>
#include <stdlib.h>
#include <wchar.h>
#include <objbase.h>
#include <stddef.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <limits.h>


#ifdef _MSC_VER 
//non inline intrinsics are faster
#pragma function(memcpy,memcmp,memset,strcmp,strcpy,strlen,strcat)
#endif // _MSC_VER

// make all the unsafe redefinitions available
#include "unsafe.h"


//-----------------------------------------------------------------------------------------------------------


// Leave these defines here to cover any usage of UnsafeEnterCriticalSection in
// header files
#define UnsafeEnterCriticalSection UnsafeEEEnterCriticalSection
#define UnsafeLeaveCriticalSection UnsafeEELeaveCriticalSection

#include "compatibilityflags.h"
extern BOOL GetCompatibilityFlag(CompatibilityFlag flag);
extern DWORD* GetGlobalCompatibilityFlags();

#include "strongname.h"
#include "stdmacros.h"

#define POISONC ((sizeof(int *) == 4)?0xCCCCCCCCL:I64(0xCCCCCCCCCCCCCCCC))

#include "ndpversion.h"
#include "switches.h"
#include "holder.h"
#include "classnames.h"
#include "dbgalloc.h"
#include "util.hpp"
#include "corpriv.h"
//#include "WarningControl.h"

#include <daccess.h>

typedef VPTR(class AppDomain)           PTR_AppDomain;
typedef VPTR(class AppDomainBaseObject) PTR_AppDomainBaseObject;
typedef DPTR(class ArrayBase)           PTR_ArrayBase;
typedef DPTR(class ArrayTypeDesc)       PTR_ArrayTypeDesc;
typedef DPTR(class Assembly)            PTR_Assembly;
typedef DPTR(class AssemblyBaseObject)  PTR_AssemblyBaseObject;
typedef DPTR(class AssemblyNameBaseObject) PTR_AssemblyNameBaseObject;
typedef VPTR(class BaseDomain)          PTR_BaseDomain;
typedef DPTR(class Binder)              PTR_Binder;
typedef DPTR(class ClassLoader)         PTR_ClassLoader;
typedef DPTR(class ComCallMethodDesc)   PTR_ComCallMethodDesc;
typedef VPTR(class CompilationDomain)   PTR_CompilationDomain;
typedef DPTR(class ComPlusCallMethodDesc) PTR_ComPlusCallMethodDesc;
typedef VPTR(class DebugInterface)      PTR_DebugInterface;
typedef DPTR(class Dictionary)          PTR_Dictionary;
typedef VPTR(class DomainAssembly)      PTR_DomainAssembly;
typedef VPTR(class DomainFile)          PTR_DomainFile;
typedef VPTR(class DomainModule)        PTR_DomainModule;
typedef DPTR(struct FailedAssembly)     PTR_FailedAssembly;
typedef VPTR(class EditAndContinueModule) PTR_EditAndContinueModule;
typedef DPTR(class EEClass)             PTR_EEClass;
typedef DPTR(class DelegateEEClass)     PTR_DelegateEEClass;
typedef DPTR(struct DomainLocalModule)  PTR_DomainLocalModule;
typedef VPTR(class EECodeManager)       PTR_EECodeManager;
typedef DPTR(class EEConfig)            PTR_EEConfig;
typedef VPTR(class EEDbgInterfaceImpl)  PTR_EEDbgInterfaceImpl;
typedef VPTR(class DebugInfoManager)    PTR_DebugInfoManager;
typedef DPTR(class FieldDesc)           PTR_FieldDesc;
typedef VPTR(class Frame)               PTR_Frame;
typedef VPTR(class ICodeManager)        PTR_ICodeManager;
typedef VPTR(class IJitManager)         PTR_IJitManager;
typedef DPTR(class InstMethodHashTable) PTR_InstMethodHashTable;
typedef DPTR(class MetaSig)             PTR_MetaSig;
typedef DPTR(class MethodDesc)          PTR_MethodDesc;
typedef DPTR(class MethodDescChunk)     PTR_MethodDescChunk;
typedef DPTR(class MethodImpl)          PTR_MethodImpl;
typedef DPTR(class MethodEntryChunk)    PTR_MethodEntryChunk;
typedef DPTR(class MethodTable)         PTR_MethodTable;
typedef VPTR(class Module)              PTR_Module;
typedef DPTR(class NDirectMethodDesc)   PTR_NDirectMethodDesc;
typedef VPTR(class Thread)              PTR_Thread;
typedef DPTR(class Object)              PTR_Object;
typedef DPTR(class ObjHeader)           PTR_ObjHeader;
typedef DPTR(class Precode)             PTR_Precode;
typedef VPTR(class ReflectionModule)    PTR_ReflectionModule;
typedef DPTR(class ReflectClassBaseObject) PTR_ReflectClassBaseObject;
typedef DPTR(class ReflectModuleBaseObject) PTR_ReflectModuleBaseObject;
typedef DPTR(class StringObject)        PTR_StringObject;
typedef DPTR(class StringBufferObject)  PTR_StringBufferObject;
typedef DPTR(class TypeHandle)          PTR_TypeHandle;
typedef VPTR(class VirtualCallStubManager) PTR_VirtualCallStubManager;
typedef VPTR(class VirtualCallStubManagerManager) PTR_VirtualCallStubManagerManager;
typedef VPTR(class GCHeap)              PTR_GCHeap;

inline void RetailBreak()  
{
    DebugBreak();
}

#define IA64WriteMemoryBarrier()
extern BOOL isMemoryReadable(const TADDR start, unsigned len);


#ifndef memcpyUnsafe_f 
#define memcpyUnsafe_f

// use this when you want to memcpy something that contains GC refs
inline void* memcpyUnsafe(void *dest, const void *src, size_t len)
{
    WRAPPER_CONTRACT;
    return memcpy(dest, src, len);
}

#endif // !memcpyUnsafe_f

//
// By default logging, and debug GC are enabled under debug
//
// These can be enabled in non-debug by removing the #ifdef _DEBUG
// allowing one to log/check_gc a free build.
//
#if defined(_DEBUG) && !defined(DACCESS_COMPILE) 

        // You should be using CopyValueClass if you are doing an memcpy
        // in the CG heap.
    #if !defined(memcpy) 
    inline void* memcpyNoGCRefs(void * dest, const void * src, size_t len) {
            WRAPPER_CONTRACT;
            
            void* result = memcpy(dest, src, len);
            IA64WriteMemoryBarrier();
            return result;
        }
    extern "C" void *  __cdecl GCSafeMemCpy(void *, const void *, size_t);
    #define memcpy(dest, src, len) GCSafeMemCpy(dest, src, len)
    #endif // !defined(memcpy)

    #if !defined(CHECK_APP_DOMAIN_LEAKS) 
    #define CHECK_APP_DOMAIN_LEAKS 1
    #endif
#else // !(defined(_DEBUG) && !defined(DACCESS_COMPILE))
    inline void* memcpyNoGCRefs(void * dest, const void * src, size_t len) {
            WRAPPER_CONTRACT;
            
            void* result = memcpy(dest, src, len);
            IA64WriteMemoryBarrier();
            return result;
        }
#endif // !(defined(_DEBUG) && !defined(DACCESS_COMPILE))

namespace Loader
{
    typedef enum 
    {
        Load, //should load
        DontLoad, //should not load
        SafeLookup  //take no locks, no allocations
    } LoadFlag;
}


// src/inc
#include "utilcode.h"
#include "log.h"
#include "loaderheap.h"

// src/vm
#include "util.hpp"
#include "ibclogger.h"
#include "eepolicy.h"

#include "vars.hpp"
#include "crst.h"
#include "argslot.h"
#include "stublink.h"
#include "cgensys.h"
#include "ceemain.h"
#include "hash.h"
#include "eecontract.h"
#include "pefile.h"
#include "sstring.h"
#include "list.h"

#include "eeconfig.h"

#include "spinlock.h"
#include "objecthandle.h"
#include "cgensys.h"
#include "declsec.h"


#include "typehandle.h"
#include "perfcounters.h"
#include "methodtable.h"
#include "typectxt.h"

#include "eehash.h"
#include "stubmgr.h"

#include "handletable.h"
#include "vars.hpp"
#include "eventstore.hpp"

#include "synch.h"
#include "regdisp.h"
#include "stackframe.h"
#include "gms.h"
#include "stackprobe.h"
#include "fcall.h"
#include "jitinterface.h"  // <NICE> We should not really need to put this so early... </NICE>
#include "syncblk.h"
#include "gcdesc.h"
#include "specialstatics.h"
#include "object.h"  // <NICE> We should not really need to put this so early... </NICE>
#include "clrex.h"
#include "clsload.hpp"  // <NICE> We should not really need to put this so early... </NICE>
#include "binder.h"
#include "siginfo.hpp"
#include "ceeload.h"
#include "memberload.h"
#include "genericdict.h"
#include "class.h"
#include "codeman.h"
#include "threads.h"
#include "appdomain.hpp"
#include "assembly.hpp"
#include "pefile.inl"
#include "excep.h"
#include "method.hpp"
#include "frames.h"

#include "stackwalk.h"
#include "stackingallocator.h"
#include "interoputil.h"
#include "wrappers.h"
#include "dynamicmethod.h"


#ifndef DACCESS_COMPILE 
#undef UnsafeEnterCriticalSection
#undef UnsafeLeaveCriticalSection

inline VOID UnsafeEEEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    WRAPPER_CONTRACT;

    if (CLRTaskHosted()) {
        Thread::BeginThreadAffinity();
    }
    UnsafeEnterCriticalSection(lpCriticalSection);
    INCTHREADLOCKCOUNT();
    INCTHREADNONHOSTLOCKCOUNT(GetThread());
}
#define UnsafeEnterCriticalSection UnsafeEEEnterCriticalSection

inline VOID UnsafeEELeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    WRAPPER_CONTRACT;

    UnsafeLeaveCriticalSection(lpCriticalSection);
    DECTHREADLOCKCOUNT();
    Thread *pThread = GetThread();
    DECTHREADNONHOSTLOCKCOUNT(pThread);
    if (CLRTaskHosted()) {
        Thread::EndThreadAffinity();
    }
}
#define UnsafeLeaveCriticalSection UnsafeEELeaveCriticalSection
#endif // !DACCESS_COMPILE

// This wrapper class ensures that the HENUMInternal is EnumTypeDefClose'd no matter how the scope is exited.
class HENUMTypeDefInternalHolder
{
    public:
        FORCEINLINE HENUMTypeDefInternalHolder(IMDInternalImport *pInternalImport)
        {
            WRAPPER_CONTRACT;

            m_pInternalImport = pInternalImport;
            m_fAcquired       = FALSE;
        }

        FORCEINLINE VOID EnumTypeDefInit()
        {
            CONTRACTL {
                THROWS;
                WRAPPER(GC_TRIGGERS);
            } CONTRACTL_END;

            _ASSERTE(!m_fAcquired);
            HRESULT hr = m_pInternalImport->EnumTypeDefInit(&m_hEnum);
            if (FAILED(hr))
            {
                COMPlusThrowHR(hr);
            }
            m_fAcquired = TRUE;

        }


        FORCEINLINE ~HENUMTypeDefInternalHolder()
        {
            WRAPPER_CONTRACT;

            if (m_fAcquired)
            {
                m_pInternalImport->EnumTypeDefClose(&m_hEnum);
            }
        }

        FORCEINLINE HENUMInternal* operator& ()
        {
            LEAF_CONTRACT;

            _ASSERTE(m_fAcquired);
            return &m_hEnum;
        }

    private:
        FORCEINLINE HENUMTypeDefInternalHolder(const HENUMTypeDefInternalHolder &)
        {
            LEAF_CONTRACT;

            _ASSERTE(!"Don't try to assign this class.");
        }

    private:
        IMDInternalImport *m_pInternalImport;
        HENUMInternal      m_hEnum;
        BOOL               m_fAcquired;
};




// This wrapper class ensures that the HENUMInternal is EnumClose'd no matter how the scope is exited.
class HENUMInternalHolder
{
    public:
        FORCEINLINE HENUMInternalHolder(IMDInternalImport *pInternalImport)
        {
            WRAPPER_CONTRACT;

            m_pInternalImport = pInternalImport;
            m_fAcquired       = FALSE;
        }

        FORCEINLINE BOOL EnumPermissionSetsInit(mdToken tkToken, CorDeclSecurity action)
        {
            CONTRACTL {
                THROWS;
                WRAPPER(GC_TRIGGERS);
            } CONTRACTL_END;

            _ASSERTE(!m_fAcquired);
            HRESULT hr = m_pInternalImport->EnumPermissionSetsInit(tkToken, action, &m_hEnum);
            m_fAcquired = TRUE;

            if (hr == CLDB_E_RECORD_NOTFOUND)
                return FALSE;

            if (FAILED(hr) )
            {
                COMPlusThrowHR(hr);
            }

            return TRUE;
        }

        FORCEINLINE VOID EnumGlobalFunctionsInit()
        {
            CONTRACTL {
                THROWS;
                WRAPPER(GC_TRIGGERS);
            } CONTRACTL_END;

            _ASSERTE(!m_fAcquired);
            HRESULT hr = m_pInternalImport->EnumGlobalFunctionsInit(&m_hEnum);
            if (FAILED(hr))
            {
                COMPlusThrowHR(hr);
            }
            m_fAcquired = TRUE;

        }


        FORCEINLINE VOID EnumGlobalFieldsInit()
        {
            CONTRACTL {
                THROWS;
                WRAPPER(GC_TRIGGERS);
            } CONTRACTL_END;

            _ASSERTE(!m_fAcquired);
            HRESULT hr = m_pInternalImport->EnumGlobalFieldsInit(&m_hEnum);
            if (FAILED(hr))
            {
                COMPlusThrowHR(hr);
            }
            m_fAcquired = TRUE;

        }

        FORCEINLINE VOID EnumTypeDefInit()
        {
            CONTRACTL {
                THROWS;
                WRAPPER(GC_TRIGGERS);
            } CONTRACTL_END;

            _ASSERTE(!m_fAcquired);
            HRESULT hr = m_pInternalImport->EnumTypeDefInit(&m_hEnum);
            if (FAILED(hr))
            {
                COMPlusThrowHR(hr);
            }
            m_fAcquired = TRUE;
        }

        FORCEINLINE VOID EnumAssociateInit(mdToken     tkParent                // [IN] token to scope the search
                                 )
        {
            CONTRACTL {
                THROWS;
                WRAPPER(GC_TRIGGERS);
            } CONTRACTL_END;

            _ASSERTE(!m_fAcquired);
            m_pInternalImport->EnumAssociateInit(tkParent, &m_hEnum);
            m_fAcquired = TRUE;

        }

        FORCEINLINE VOID EnumInit(DWORD       tkKind,                 // [IN] which table to work on
                                  mdToken     tkParent                // [IN] token to scope the search
                                 )
        {
            CONTRACTL {
                THROWS;
                WRAPPER(GC_TRIGGERS);
            } CONTRACTL_END;

            _ASSERTE(!m_fAcquired);
            HRESULT hr = m_pInternalImport->EnumInit(tkKind, tkParent, &m_hEnum);
            if (FAILED(hr))
            {
                COMPlusThrowHR(hr);
            }
            m_fAcquired = TRUE;

        }


        FORCEINLINE VOID EnumAllInit(DWORD       tkKind                 // [IN] which table to work on
                                 )
        {
            CONTRACTL {
                THROWS;
                WRAPPER(GC_TRIGGERS);
            } CONTRACTL_END;

            _ASSERTE(!m_fAcquired);
            HRESULT hr = m_pInternalImport->EnumAllInit(tkKind, &m_hEnum);
            if (FAILED(hr))
            {
                COMPlusThrowHR(hr);
            }
            m_fAcquired = TRUE;

        }



        FORCEINLINE ~HENUMInternalHolder()
        {
            WRAPPER_CONTRACT;

            if (m_fAcquired)
            {
                m_pInternalImport->EnumClose(&m_hEnum);
            }
        }

        FORCEINLINE HENUMInternal* operator& ()
        {
            LEAF_CONTRACT;

            _ASSERTE(m_fAcquired);
            return &m_hEnum;
        }

    private:
        FORCEINLINE HENUMInternalHolder(const HENUMInternalHolder &)
        {
            WRAPPER_CONTRACT;

            _ASSERTE(!"Don't try to assign this class.");
        }


    private:
        IMDInternalImport *m_pInternalImport;
        HENUMInternal      m_hEnum;
        BOOL               m_fAcquired;
};

HINSTANCE GetModuleInst();

#ifndef memcpyGCRefs_f 
#define memcpyGCRefs_f

// use this when you want to memcpy something that contains GC refs
inline void *  memcpyGCRefs(void *dest, const void *src, size_t len)
{
    WRAPPER_CONTRACT;

    void *ret = memcpyUnsafe(dest, src, len);
    IA64WriteMemoryBarrier();
    GCHeap::GetGCHeap()->SetCardsAfterBulkCopy((Object**) dest, len);
    return ret;
}

#endif // !memcpyGCRefs_f



// For down level platform compiles.
#if !defined(_WIN64) && (_WIN32_WINNT < 0x0500) 
typedef VOID (__stdcall *WAITORTIMERCALLBACK)(PVOID, BOOL);
#endif



// All files get to see all of these .inl files to make sure all files
// get the benefit of inlining.
#include "ceeload.inl"
#include "typedesc.inl"
#include "class.inl"
#include "methodtable.inl"
#include "typehandle.inl"
#include "object.inl"
#include "ceeload.inl"
#include "clsload.inl"
#include "domainfile.inl"
#include "handletable.inl"
#include "clsload.inl"
#include "method.inl"
#include "stackprobe.inl"
#include "syncblk.inl"
#include "threads.inl"

#endif // !_common_h_
