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
// ---------------------------------------------------------------------------
// CLREx.h
// ---------------------------------------------------------------------------

#ifndef _CLREX_H_
#define _CLREX_H_

#include <ex.h>

#include "objecthandle.h"
#include "runtimeexceptionkind.h"

class AssemblySpec;

struct StackTraceElement 
{
    UINT_PTR        ip;
    UINT_PTR        sp;
    MethodDesc*     pFunc;
    void*           pExactGenericArgsToken; // Required for exact instantiation info

    bool operator==(StackTraceElement const & rhs) const
    {
        return ip == rhs.ip
            && sp == rhs.sp
            && pFunc == rhs.pFunc
            && pExactGenericArgsToken == rhs.pExactGenericArgsToken;
    }

    bool operator!=(StackTraceElement const & rhs) const
    {
        return !(*this == rhs);
    }

    bool PartiallyEqual(StackTraceElement const & rhs) const
    {
        return pFunc == rhs.pFunc && pExactGenericArgsToken == rhs.pExactGenericArgsToken;
    }

    void PartialAtomicUpdate(StackTraceElement const & rhs)
    {
        ip = rhs.ip;
    }
};

class StackTraceInfo
{
private:    
    // for building stack trace info
    StackTraceElement*  m_pStackTrace;      // pointer to stack trace storage
    unsigned            m_cStackTrace;      // size of stack trace storage
    unsigned            m_dFrameCount;      // current frame in stack trace

public:
    void Init();
    BOOL IsEmpty();
    void AllocateStackTrace();
    void ClearStackTrace();
    void FreeStackTrace();
    void SaveStackTrace(BOOL bAllowAllocMem, OBJECTHANDLE hThrowable, BOOL bReplaceStack, BOOL bSkipLastElement);
    BOOL AppendElement(BOOL bAllowAllocMem, UINT_PTR currentIP, UINT_PTR currentSP, MethodDesc* pFunc, CrawlFrame* pCf);

    void GetLeafFrameInfo(StackTraceElement* pStackTraceElement);
};


// ---------------------------------------------------------------------------
// CLRException represents an exception which has a managed representation.
// It adds the generic method GetThrowable().
// ---------------------------------------------------------------------------
class CLRException : public Exception
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);
    friend class CLRLastThrownObjectException;
 private:
    static const int c_type = 0x434c5220;   // 'CLR '

 protected:
    OBJECTHANDLE            m_throwableHandle;

    void SetThrowableHandle(OBJECTHANDLE throwable);
    OBJECTHANDLE GetThrowableHandle() { return m_throwableHandle; }

 public:

    CLRException();
    ~CLRException();

    OBJECTREF GetThrowable();

    // Dynamic type query for catchers
    static int GetType() {LEAF_CONTRACT;  return c_type; }
    virtual int GetInstanceType() { LEAF_CONTRACT; return c_type; }
    BOOL IsType(int type) { WRAPPER_CONTRACT; return type == c_type || Exception::IsType(type);  }

    // Overrides
    virtual BOOL IsDomainBound() 
    {
        return TRUE;
    };    
    HRESULT GetHR();
    IErrorInfo *GetIErrorInfo();
    
    void GetMessage(SString &result);

 protected:

    virtual OBJECTREF CreateThrowable() { LEAF_CONTRACT; return NULL; }

 public: // These are really private, but are used by the exception macros


    // Accessors for all the preallocated exception objects.
    static OBJECTREF GetPreallocatedOutOfMemoryException();
    static OBJECTREF GetPreallocatedRudeThreadAbortException();
    static OBJECTREF GetPreallocatedThreadAbortException();
    static OBJECTREF GetPreallocatedStackOverflowException();
    static OBJECTREF GetPreallocatedExecutionEngineException();

    // Accessors for all the preallocated exception handles.
    static OBJECTHANDLE GetPreallocatedOutOfMemoryExceptionHandle();
    static OBJECTHANDLE GetPreallocatedRudeThreadAbortExceptionHandle();
    static OBJECTHANDLE GetPreallocatedThreadAbortExceptionHandle();
    static OBJECTHANDLE GetPreallocatedStackOverflowExceptionHandle();
    static OBJECTHANDLE GetPreallocatedExecutionEngineExceptionHandle();
    static OBJECTHANDLE GetPreallocatedHandleForObject(OBJECTREF o);

    // Use these to determine if a handle or object ref is one of the preallocated handles or object refs.
    static BOOL IsPreallocatedExceptionObject(OBJECTREF o);
    static BOOL IsPreallocatedExceptionHandle(OBJECTHANDLE h);
    
    // Prefer a new OOM exception if we can make one.  If we cannot, then give back the pre-allocated
    // one.
    static OBJECTREF GetBestOutOfMemoryException();

    static OBJECTREF GetThrowableFromException(Exception *pException);
    static OBJECTREF GetThrowableFromExceptionRecord(EXCEPTION_RECORD *pExceptionRecord);

    class HandlerState : public Exception::HandlerState
    {
    public:
        Thread* m_pThread;
        Frame*  m_pFrame;
        BOOL    m_fPreemptiveGCDisabled;
        BOOL    m_fUnwindExInfo;

#ifdef _X86_
#if defined(FEATURE_PAL) && defined(__GNUC__)
        // Use CQuickBytes to allocate instead of alloca
        // Use of alloca is discouraged in functions that use exception handling on gcc
        // Change 645922 is a similar issue
        // See exceptmacros.h for the use of this field
        CQuickBytes m_RecordBytes;
#endif // FEATURE_PAL & __GNUC__
        PEXCEPTION_REGISTRATION_RECORD  m_pDownlevelVectoredHandlerRecord;
#endif // _X86_

        HandlerState();

        void CleanupTry();
        void SetupCatchCommon();
        void SetupCatch(bool fCaughtInternalCxxException);
        void SucceedCatch(bool fCaughtInternalCxxException);
        void SetupFinally();
    };
};

// prototype for helper function to get exception object from thread's LastThrownObject.
void GetLastThrownObjectExceptionFromThread_Internal(Exception **ppException);


// ---------------------------------------------------------------------------
// EEException is a CLR exception subclass which has purely unmanaged representation.
// The standard methods will not do any GC dangerous operations.  Thus you
// can throw and catch such an exception without regard to GC mode.
// ---------------------------------------------------------------------------

class EEException : public CLRException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);

 private:
    static const int c_type = 0x45452020;   // 'EE '

 public:
    const RuntimeExceptionKind    m_kind;

    EEException(RuntimeExceptionKind kind);
    EEException(HRESULT hr);

    // Dynamic type query for catchers
    static int GetType() {LEAF_CONTRACT;  return c_type; }
    virtual int GetInstanceType() { LEAF_CONTRACT; return c_type; }
    BOOL IsType(int type) { WRAPPER_CONTRACT; return type == c_type || CLRException::IsType(type); }

    // Virtual overrides
    HRESULT GetHR();
    IErrorInfo *GetErrorInfo();
    void GetMessage(SString &result);
    OBJECTREF CreateThrowable();

    // GetThrowableMessage returns a message to be stored in the throwable.
    // Returns FALSE if there is no useful value.
    virtual BOOL GetThrowableMessage(SString &result);

    static BOOL GetResourceMessage(UINT iResourceID, SString &result,
                                   const SString &arg1 = SString::Empty(), const SString &arg2 = SString::Empty(),
                                   const SString &arg3 = SString::Empty(), const SString &arg4 = SString::Empty(),
                                   const SString &arg5 = SString::Empty(), const SString &arg6 = SString::Empty(),
                                   const SString &arg7 = SString::Empty(), const SString &arg8 = SString::Empty(),
                                   const SString &arg9 = SString::Empty(), const SString &arg10 = SString::Empty());

    // Note: reKind-->hr is a one-to-many relationship.
    //
    //   each reKind is associated with one or more hresults.
    //   every hresult is associated with exactly one reKind (with kCOMException being the catch-all.)
    static RuntimeExceptionKind GetKindFromHR(HRESULT hr);
  protected:
    static HRESULT GetHRFromKind(RuntimeExceptionKind reKind);

#ifdef _DEBUG    
    EEException() : m_kind(kException)
    {
        // Used only for DebugIsEECxxExceptionPointer to get the vtable pointer.
        // We need a variant which does not allocate memory.
    }
#endif // _DEBUG
    
    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new EEException(m_kind);
    }
        
};

// ---------------------------------------------------------------------------
// EEMessageException is an EE exception subclass composed of a type and
// an unmanaged message of some sort.
// ---------------------------------------------------------------------------

class EEMessageException : public EEException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);
    
 private:
    HRESULT             m_hr;
    UINT                m_resID;
    InlineSString<32>   m_arg1;
    InlineSString<32>   m_arg2;
    SString             m_arg3;
    SString             m_arg4;
    SString             m_arg5;
    SString             m_arg6;
    SString             m_arg7;
    SString             m_arg8;
    SString             m_arg9;
    SString             m_arg10;

 public:
    EEMessageException(RuntimeExceptionKind kind, UINT resID = 0, LPCWSTR szArg1 = NULL, LPCWSTR szArg2 = NULL, 
                       LPCWSTR szArg3 = NULL, LPCWSTR szArg4 = NULL, LPCWSTR szArg5 = NULL, LPCWSTR szArg6 = NULL, 
                       LPCWSTR szArg7 = NULL, LPCWSTR szArg8 = NULL, LPCWSTR szArg9 = NULL, LPCWSTR szArg10 = NULL);

    EEMessageException(HRESULT hr);

    EEMessageException(HRESULT hr, UINT resID, LPCWSTR szArg1 = NULL, LPCWSTR szArg2 = NULL, LPCWSTR szArg3 = NULL, 
                       LPCWSTR szArg4 = NULL, LPCWSTR szArg5 = NULL, LPCWSTR szArg6 = NULL, LPCWSTR szArg7 = NULL, 
                       LPCWSTR szArg8 = NULL, LPCWSTR szArg9 = NULL, LPCWSTR szArg10 = NULL);

    // This function used to be private.  Invconvenient, because of no default args, used mostly internally,
    //  but useful when access to both kind and hr are needed (as in COMPLusThrowWin32).
    EEMessageException(RuntimeExceptionKind kind, HRESULT hr, UINT resID, LPCWSTR szArg1, LPCWSTR szArg2, 
                       LPCWSTR szArg3, LPCWSTR szArg4, LPCWSTR szArg5, LPCWSTR szArg6, 
                       LPCWSTR szArg7, LPCWSTR szArg8, LPCWSTR szArg9, LPCWSTR szArg10);
    

    // Virtual overrides
    HRESULT GetHR();

    BOOL GetThrowableMessage(SString &result);

    UINT GetResID(void) { LEAF_CONTRACT; return m_resID; }

    static BOOL IsEEMessageException(Exception *pException)
    {
        return (*(PVOID*)pException == GetEEMessageExceptionVPtr());
    }

 protected:

    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new EEMessageException(
                m_kind, m_hr, m_resID, m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8, m_arg9, m_arg10);
    }

    
 private:
    
    static PVOID GetEEMessageExceptionVPtr()
    {
        CONTRACT (PVOID)
        {
            WRAPPER(THROWS);
            WRAPPER(GC_TRIGGERS);
            MODE_ANY;
            POSTCONDITION(CheckPointer(RETVAL));
        }
        CONTRACT_END;
        
        EEMessageException boilerplate(E_FAIL);
        RETURN (PVOID&)boilerplate;
    }

    BOOL GetResourceMessage(UINT iResourceID, SString &result);

#ifdef _DEBUG    
    EEMessageException()
    {
        // Used only for DebugIsEECxxExceptionPointer to get the vtable pointer.
        // We need a variant which does not allocate memory.
    }
#endif // _DEBUG
};

// ---------------------------------------------------------------------------
// EEResourceException is an EE exception subclass composed of a type and
// an message using a managed exception resource.
// ---------------------------------------------------------------------------

class EEResourceException : public EEException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);
    
 private:
    InlineSString<32>        m_resourceName;

 public:
    EEResourceException(RuntimeExceptionKind kind, const SString &resourceName);

    // Unmanaged message text containing only the resource name (GC safe)
    void GetMessage(SString &result);

    // Throwable message containig the resource contents (not GC safe)
    BOOL GetThrowableMessage(SString &result);

 protected:
    
    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new EEResourceException(m_kind, m_resourceName);
    }

private:
#ifdef _DEBUG    
    EEResourceException()
    {
        // Used only for DebugIsEECxxExceptionPointer to get the vtable pointer.
        // We need a variant which does not allocate memory.
    }
#endif // _DEBUG
};

// ---------------------------------------------------------------------------
// EECOMException is an EE exception subclass composed of COM-generated data.
// Note that you must ensure that the COM data was not derived from a wrapper
// on a managed Exception object.  (If so, you must compose the exception from
// the managed object itself.)
// ---------------------------------------------------------------------------

struct ExceptionData
{
    HRESULT hr;
    BSTR    bstrDescription;
    BSTR    bstrSource;
    BSTR    bstrHelpFile;
    DWORD   dwHelpContext;
    GUID    guid;
};

class EECOMException : public EEException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);
    
 private:
    ExceptionData m_ED;

 public:

    EECOMException(EXCEPINFO *pExcepInfo);
    EECOMException(ExceptionData *pED);
    EECOMException(HRESULT hr, IErrorInfo *pErrInfo);
    ~EECOMException();

    // Virtual overrides
    HRESULT GetHR();

    BOOL GetThrowableMessage(SString &result);
    OBJECTREF CreateThrowable();

 protected:
    
    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new EECOMException(&m_ED);
    }

private:
#ifdef _DEBUG    
    EECOMException()
    {
        // Used only for DebugIsEECxxExceptionPointer to get the vtable pointer.
        // We need a variant which does not allocate memory.
        ZeroMemory(&m_ED, sizeof(m_ED));
    }
#endif // _DEBUG
};

// ---------------------------------------------------------------------------
// EEFieldException is an EE exception subclass composed of a field
// ---------------------------------------------------------------------------
class EEFieldException : public EEException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);
    
 private:
    FieldDesc *m_pFD;

 public:
    EEFieldException(FieldDesc *pField);

    BOOL GetThrowableMessage(SString &result);
    virtual BOOL IsDomainBound() {return TRUE;};
protected:
    
    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new EEFieldException(m_pFD);
    }

private:
#ifdef _DEBUG    
    EEFieldException()
    {
        // Used only for DebugIsEECxxExceptionPointer to get the vtable pointer.
        // We need a variant which does not allocate memory.
    }
#endif // _DEBUG
};

// ---------------------------------------------------------------------------
// EEMethodException is an EE exception subclass composed of a field
// ---------------------------------------------------------------------------

class EEMethodException : public EEException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);
    
 private:
    MethodDesc *m_pMD;

 public:
    EEMethodException(MethodDesc *pMethod);

    BOOL GetThrowableMessage(SString &result);
    virtual BOOL IsDomainBound() {return TRUE;};
 protected:
    
    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new EEMethodException(m_pMD);
    }

private:
#ifdef _DEBUG    
    EEMethodException()
    {
        // Used only for DebugIsEECxxExceptionPointer to get the vtable pointer.
        // We need a variant which does not allocate memory.
    }
#endif // _DEBUG
};

// ---------------------------------------------------------------------------
// EEArgumentException is an EE exception subclass representing a bad argument
// exception
// ---------------------------------------------------------------------------

class EEArgumentException : public EEException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);
    
 private:
    InlineSString<32>        m_argumentName;
    InlineSString<32>        m_resourceName;

 public:
    EEArgumentException(RuntimeExceptionKind reKind, LPCWSTR pArgName,
                        LPCWSTR wszResourceName);


    OBJECTREF CreateThrowable();

 protected:
    
    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new EEArgumentException(m_kind, m_argumentName, m_resourceName);
    }

private:
#ifdef _DEBUG    
    EEArgumentException()
    {
        // Used only for DebugIsEECxxExceptionPointer to get the vtable pointer.
        // We need a variant which does not allocate memory.
    }
#endif // _DEBUG
};

// ---------------------------------------------------------------------------
// EETypeLoadException is an EE exception subclass representing a type loading
// error
// ---------------------------------------------------------------------------

class EETypeLoadException : public EEException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);
    
  private:
    InlineSString<64>   m_fullName;
    SString             m_pAssemblyName;
    SString             m_pMessageArg;
    UINT                m_resIDWhy;

 public:
    EETypeLoadException(LPCUTF8 pszNameSpace, LPCUTF8 pTypeName,
                        LPCWSTR pAssemblyName, LPCUTF8 pMessageArg, UINT resIDWhy);

    EETypeLoadException(LPCWSTR pFullTypeName,
                        LPCWSTR pAssemblyName, LPCUTF8 pMessageArg, UINT resIDWhy);

    // virtual overrides
    void GetMessage(SString &result);
    OBJECTREF CreateThrowable();

 protected:
    
    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new EETypeLoadException(m_fullName, m_pAssemblyName, m_pMessageArg, m_resIDWhy);
        }

 private:
    EETypeLoadException(InlineSString<64> fullName, LPCWSTR pAssemblyName,
                        const SString &pMessageArg, UINT resIDWhy)
       : EEException(kTypeLoadException),
         m_fullName(fullName),
         m_pAssemblyName(pAssemblyName),
         m_pMessageArg(pMessageArg),
         m_resIDWhy(resIDWhy)
    {
        WRAPPER_CONTRACT;
    }

    
#ifdef _DEBUG    
    EETypeLoadException()
    {
        // Used only for DebugIsEECxxExceptionPointer to get the vtable pointer.
        // We need a variant which does not allocate memory.
    }
#endif // _DEBUG
};

// ---------------------------------------------------------------------------
// EEFileLoadException is an EE exception subclass representing a file loading
// error
// ---------------------------------------------------------------------------

class EEFileLoadException : public EEException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);
    
  private:
    SString m_name;
    IFusionBindLog *m_pFusionLog;
    HRESULT m_hr;       
                        

  public:

    EEFileLoadException(const SString &name, HRESULT hr, IFusionBindLog *pFusionLog = NULL, Exception *pInnerException = NULL);
    ~EEFileLoadException();

    // virtual overrides
    HRESULT GetHR()
    {
        LEAF_CONTRACT;
        return m_hr;
    }
    void GetMessage(SString &result);
    OBJECTREF CreateThrowable();

    static RuntimeExceptionKind GetFileLoadKind(HRESULT hr);

    static void DECLSPEC_NORETURN Throw(AssemblySpec *pSpec, IFusionBindLog *pFusionLog, HRESULT hr, Exception *pInnerException = NULL);
    static void DECLSPEC_NORETURN Throw(AssemblySpec *pSpec, HRESULT hr, Exception *pInnerException = NULL);
    static void DECLSPEC_NORETURN Throw(PEFile *pFile, HRESULT hr, Exception *pInnerException = NULL);
    static void DECLSPEC_NORETURN Throw(LPCWSTR path, HRESULT hr, Exception *pInnerException = NULL);
    static void DECLSPEC_NORETURN Throw(IAssembly *pIAssembly, IHostAssembly *pIHostAssembly, HRESULT hr, Exception *pInnerException = NULL);
    static void DECLSPEC_NORETURN Throw(PEAssembly *parent, const void *memory, COUNT_T size, HRESULT hr, Exception *pInnerException = NULL);
    static BOOL CheckType(Exception* ex);

 protected:
    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new EEFileLoadException(m_name, m_hr, m_pFusionLog);
    }

 private:
#ifdef _DEBUG    
    EEFileLoadException() : m_pFusionLog(NULL)
    {
        // Used only for DebugIsEECxxExceptionPointer to get the vtable pointer.
        // We need a variant which does not allocate memory.
    }
#endif // _DEBUG

    void SetFileName(const SString &fileName, BOOL removePath);
};

// ---------------------------------------------------------------------------
// Throw/catch macros.  These are derived from the generic EXCEPTION macros,
// but add extra functionality for cleaning up thread state on catches
//
// Usage:
// EX_TRY
// {
//      EX_THROW(EEMessageException, (kind, L"This sucks - I'm quitting"));
// }
// EX_CATCH
// {
//      EX_RETHROW()
// }
// EX_END_CATCH(RethrowTerminalExceptions or SwallowAllExceptions)
// ---------------------------------------------------------------------------

#undef EX_TRY
#define EX_TRY                                                              \
    EX_TRY_CUSTOM(CLRException::HandlerState, CLRLastThrownObjectException) \
    INSTALL_DOWNLEVEL_VECTORED_HANDLER(__state);

#undef  EX_TRY_FOR_FINALLY
#define EX_TRY_FOR_FINALLY  EX_TRY_FOR_FINALLY_CUSTOM(CLRException::HandlerState)

#if defined(_DEBUG)
  // Redefine GET_EXCEPTION to validate CLRLastThrownObjectException as much as possible.
  #undef GET_EXCEPTION
  #define GET_EXCEPTION() (__pException.IsNull() ? __defaultException.Validate() : __pException.GetValue())
#endif // _DEBUG

// When we throw an exception, we need stay in SO-intolerant state and
// probe for sufficient stack so that we don't SO during the processing.
#undef HANDLE_SO_TOLERANCE_FOR_THROW
#define HANDLE_SO_TOLERANCE_FOR_THROW STACK_PROBE_FOR_THROW(GetThread());

LONG CLRNoCatchHandler(EXCEPTION_POINTERS* pExceptionInfo, PVOID pv);


#undef EX_TRY_NOCATCH
#define EX_TRY_NOCATCH                  \
    PAL_TRY                             \
    {                                   \
        CLRException::HandlerState __state; \
        CAutoTryCleanup<CLRException::HandlerState> __autoCleanupTry(__state); \

#undef EX_END_NOCATCH
#define EX_END_NOCATCH                  \
    }                                   \
    PAL_EXCEPT_FILTER(CLRNoCatchHandler, NULL) \
    {                                   \
    }                                   \
    PAL_ENDTRY


#define GET_THROWABLE() CLRException::GetThrowableFromException(GET_EXCEPTION())

//==============================================================================
// High-level macros for common uses of EX_TRY. Try using these rather
// than the raw EX_TRY constructs.
//==============================================================================

//===================================================================================
// Macro for defining external entrypoints such as COM interop boundaries.
// The boundary will catch all exceptions (including terminals) and convert
// them into HR/IErrorInfo pairs as appropriate.
//
// Usage:
//
//   HRESULT hr;                     ;; BEGIN will initialize HR
//   BEGIN_EXTERNAL_ENTRYPOINT(&hr)
//   <do managed stuff>              ;; this part will execute in cooperative GC mode
//   END_EXTERNAL_ENTRYPOINT
//   return hr;
//
// Comments:
//   The BEGIN macro will setup a Thread if necessary. It should only be called
//   in preemptive mode.  If you are calling it from cooperative mode, this implies
//   we are executing "external" code in cooperative mode.  The Reentrancy MDA
//   complains about this.
//
//   Only use this macro for actual boundaries between CLR and
//   outside unmanaged code. If you want to connect internal pieces
//   of CLR code, use BEGIN_EXCEPTION_GLUE instead.
//===================================================================================

#define BEGIN_EXTERNAL_ENTRYPOINT(phresult)              \
    {                                                    \
        HRESULT *__phr = (phresult);                     \
        *__phr = S_OK;                                   \
        Thread *__pThread;                               \
        _ASSERTE(GetThread() == NULL ||                  \
                 !GetThread()->PreemptiveGCDisabled());  \
        if (HasIllegalReentrancy())                      \
        {                                                \
            *__phr = COR_E_ILLEGAL_REENTRANCY;           \
        }                                                \
        else                                             \
        if (!CanRunManagedCode())                        \
        {                                                \
            *__phr = E_PROCESS_SHUTDOWN_REENTRY;         \
        }                                                \
        else                                             \
        {                                                \
            __pThread=SetupThreadNoThrow(__phr);         \
            if (__pThread != NULL)                       \
            {                                            \
                GCX_COOP();                              \
                BEGIN_SO_INTOLERANT_CODE_NOTHROW(__pThread, *__phr = COR_E_STACKOVERFLOW); \
                EX_TRY                                   \
                {                                        \
            
                
#define END_EXTERNAL_ENTRYPOINT                          \
                }                                        \
                EX_CATCH_HRESULT(*__phr);                \
                END_SO_INTOLERANT_CODE;                  \
           }                                             \
        }                                                \
    }



//===================================================================================
// Macro for *throwaway code* to facilitate calling exception-based code for non-exception-based code.
// Usage:
//
//   HRESULT Foo(OBJECTREF *pThrowable)
//   {
//       HRESULT hr;
//       BEGIN_EXCEPTION_GLUE(&hr, pThrowable)
//       FooThrowing();
//       END_EXCEPTION_GLUE
//       return hr;
//   }
//
// Comments:
//   This macro is both for convenience and tracking (by using it,
//   you indicate you're writing "glue" code and that it's a goal
//   to get rid of the need for this once we're completely exception-based.)
//   Don't use this for nonthrowaway code (if you're implementing an external
//   entrypoint, use BEGIN_EXTERNAL_ENTRYPOINT instead, which does the
//   right thing for that purpose.)
//
//   The HR and pThrowable arguments can be NULL if you're not interested
//   in retrieving those values. If you call this macro on a thread that
//   hasn't had SetupThread() called on it, both arguments MUST be NULL.
//
//   The macro can be called in either preemptive or cooperative GC mode.
//===================================================================================

#define BEGIN_EXCEPTION_GLUE(pHResult, pThrowable)       \
    {                                                    \
        HRESULT *__phr = (pHResult);                     \
        if (__phr) *__phr = S_OK;                        \
        OBJECTREF *__pThrowable = (pThrowable);          \
                                                         \
        _ASSERTE( (__phr == NULL && __pThrowable == NULL) ||   /* You can use this macro on unmanaged threads, but if you, */ \
                  GetThread() != NULL );                       /* you can't get hresult or throwable back. */ \
                                                         \
        EX_TRY                                           \
        {                                                \



#define END_EXCEPTION_GLUE                               \
        }                                                \
        EX_CATCH                                         \
        {                                                \
            if (__phr)                                   \
            {                                            \
                *__phr = GET_EXCEPTION()->GetHR();       \
            }                                            \
            if (__pThrowable)                            \
            {                                            \
                GCX_COOP();                              \
                if ( (*__pThrowable) == NULL )           \
                {                                        \
                    *__pThrowable = GETTHROWABLE();      \
                }                                        \
            }                                            \
                                                         \
        }                                                \
        EX_END_CATCH(SwallowAllExceptions)                 /* Provides compatibility with V1, but is probably */ \
    }                                                      /* the wrong setting for Whidbey. Our excuse is that this */ \
                                                           /* macro is itself only supposed to be used for throwaway */
                                                           /* code to help the intransition to exception-based error */
                                                           /* handling. */

//==============================================================================

// ---------------------------------------------------------------------------
// Inline implementations. Pay no attention to that man behind the curtain.
// ---------------------------------------------------------------------------

inline CLRException::CLRException()
  : m_throwableHandle(NULL)
{
    LEAF_CONTRACT;
}

inline void CLRException::SetThrowableHandle(OBJECTHANDLE throwable)
{
    STRESS_LOG1(LF_EH, LL_INFO100, "in CLRException::SetThrowableHandle: obj = %x\n", throwable);
    m_throwableHandle = throwable;
}

inline EEException::EEException(RuntimeExceptionKind kind)
  : m_kind(kind)
{
    LEAF_CONTRACT;
}

inline EEException::EEException(HRESULT hr)
  : m_kind(GetKindFromHR(hr))
{
    LEAF_CONTRACT;
}

inline EEMessageException::EEMessageException(HRESULT hr)
  : EEException(GetKindFromHR(hr)),
    m_hr(hr),
    m_resID(0)
{
    WRAPPER_CONTRACT;

    m_arg1.Printf("%.8x", hr);
}

//-----------------------------------------------------------------------------
// Constructor with lots of defaults (to 0 / null)
//   kind       -- "clr kind" of the exception
//   resid      -- resource id for message
//   strings    -- substitution text for message
inline EEMessageException::EEMessageException(RuntimeExceptionKind kind, UINT resID, LPCWSTR szArg1, LPCWSTR szArg2, 
                                              LPCWSTR szArg3, LPCWSTR szArg4, LPCWSTR szArg5, LPCWSTR szArg6, 
                                              LPCWSTR szArg7, LPCWSTR szArg8, LPCWSTR szArg9, LPCWSTR szArg10)
  : EEException(kind),
    m_hr(EEException::GetHRFromKind(kind)),
    m_resID(resID),
    m_arg1(szArg1),
    m_arg2(szArg2),
    m_arg3(szArg3),
    m_arg4(szArg4),
    m_arg5(szArg5),
    m_arg6(szArg6),
    m_arg7(szArg7),
    m_arg8(szArg8),
    m_arg9(szArg9),
    m_arg10(szArg10)
{
    WRAPPER_CONTRACT;
}

//-----------------------------------------------------------------------------
// Constructor with lots of defaults (to 0 / null)
//   hr         -- hresult that lead to this exception
//   resid      -- resource id for message
//   strings    -- substitution text for message
inline EEMessageException::EEMessageException(HRESULT hr, UINT resID, LPCWSTR szArg1, LPCWSTR szArg2, LPCWSTR szArg3, 
                                              LPCWSTR szArg4, LPCWSTR szArg5, LPCWSTR szArg6, LPCWSTR szArg7, 
                                              LPCWSTR szArg8, LPCWSTR szArg9, LPCWSTR szArg10)
  : EEException(GetKindFromHR(hr)),
    m_hr(hr),
    m_resID(resID),
    m_arg1(szArg1),
    m_arg2(szArg2),
    m_arg3(szArg3),
    m_arg4(szArg4),
    m_arg5(szArg5),
    m_arg6(szArg6),
    m_arg7(szArg7),
    m_arg8(szArg8),
    m_arg9(szArg9),
    m_arg10(szArg10)
{
}
    
//-----------------------------------------------------------------------------
// Constructor with no defaults
//   kind       -- "clr kind" of the exception
//   hr         -- hresult that lead to this exception
//   resid      -- resource id for message
//   strings    -- substitution text for message
inline EEMessageException::EEMessageException(RuntimeExceptionKind kind, HRESULT hr, UINT resID, LPCWSTR szArg1,
                                              LPCWSTR szArg2, LPCWSTR szArg3, LPCWSTR szArg4, LPCWSTR szArg5, 
                                              LPCWSTR szArg6, LPCWSTR szArg7, LPCWSTR szArg8, LPCWSTR szArg9, 
                                              LPCWSTR szArg10)
  : EEException(kind),
    m_hr(hr),
    m_resID(resID),
    m_arg1(szArg1),
    m_arg2(szArg2),
    m_arg3(szArg3),
    m_arg4(szArg4),
    m_arg5(szArg5),
    m_arg6(szArg6),
    m_arg7(szArg7),
    m_arg8(szArg8),
    m_arg9(szArg9),
    m_arg10(szArg10)
{
    WRAPPER_CONTRACT;
}


inline EEResourceException::EEResourceException(RuntimeExceptionKind kind, const SString &resourceName)
  : EEException(kind),
    m_resourceName(resourceName)
{
    WRAPPER_CONTRACT;
}


inline EEFieldException::EEFieldException(FieldDesc *pField)
  : EEException(kFieldAccessException),
    m_pFD(pField)
{
    WRAPPER_CONTRACT;
}

inline EEMethodException::EEMethodException(MethodDesc *pMethod)
  : EEException(kMethodAccessException),
    m_pMD(pMethod)
{
    WRAPPER_CONTRACT;
}

inline EEArgumentException::EEArgumentException(RuntimeExceptionKind reKind, LPCWSTR pArgName,
                    LPCWSTR wszResourceName)
  : EEException(reKind),
    m_argumentName(pArgName),
    m_resourceName(wszResourceName)
{
    WRAPPER_CONTRACT;
}


class ObjrefException : public CLRException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);

 public:

    ObjrefException();
    ObjrefException(OBJECTREF throwable);

 private:
    static const int c_type = 0x4F522020;   // 'OR '

 public:
    // Dynamic type query for catchers
    static int GetType() {LEAF_CONTRACT;  return c_type; }
    virtual int GetInstanceType() { LEAF_CONTRACT; return c_type; }
    BOOL IsType(int type) { WRAPPER_CONTRACT; return type == c_type || CLRException::IsType(type); }

protected:
    virtual Exception *CloneHelper()
    {
        WRAPPER_CONTRACT;
        return new ObjrefException();
    }
    
    virtual Exception *DomainBoundCloneHelper();
};


class CLRLastThrownObjectException : public CLRException
{
    friend bool DebugIsEECxxExceptionPointer(void* pv);

 public:
    CLRLastThrownObjectException();
    
 private:
    static const int c_type = 0x4C544F20;   // 'LTO '
 
 public:
    // Dynamic type query for catchers
    static int GetType() {LEAF_CONTRACT;  return c_type; }
    virtual int GetInstanceType() { LEAF_CONTRACT; return c_type; }
    BOOL IsType(int type) { WRAPPER_CONTRACT; return type == c_type || CLRException::IsType(type); }
    
    #if defined(_DEBUG)
      CLRLastThrownObjectException* Validate();
    #endif // _DEBUG

 protected:
    virtual Exception *CloneHelper();
   
    virtual Exception *DomainBoundCloneHelper();

    virtual OBJECTREF CreateThrowable();
};


#endif // _CLREX_H_

