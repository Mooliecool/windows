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
//*****************************************************************************
// rspriv.
//
// Common include file for right-side of debugger.
//*****************************************************************************

#ifndef RSPRIV_H
#define RSPRIV_H

#include <winwrap.h>
#include <windows.h>

#include <utilcode.h>

#ifdef _DEBUG
#define LOGGING
#endif

#include <log.h>
#include <corerror.h>

#include "cor.h"

#include "cordebug.h"
#include "cordbpriv.h"
#include "dacprivate.h"
#include "mscoree.h"

#include <cordbpriv.h>
#include <dbgipcevents.h>

#include "ipcmanagerinterface.h"

#include "common.h"
#include "primitives.h"

#undef ASSERT
#define CRASH(x)  _ASSERTE(!x)
#define ASSERT(x) _ASSERTE(x)

// We want to keep the 'worst' HRESULT - if one has failed (..._E_...) & the
// other hasn't, take the failing one.  If they've both/neither failed, then
// it doesn't matter which we take.
// Note that this macro favors retaining the first argument
#define WORST_HR(hr1,hr2) (FAILED(hr1)?hr1:hr2)


/* ------------------------------------------------------------------------- *
 * Forward class declarations
 * ------------------------------------------------------------------------- */

class CordbBase;
class CordbValue;
class CordbValueHome;
class CordbModule;
class CordbClass;
class CordbFunction;
class CordbCode;
class CordbFrame;
class CordbJITILFrame;
class CordbInternalFrame;
class CordbChain;
class CordbContext;
class CordbThread;
class CordbUnmanagedThread;
struct CordbUnmanagedEvent;
class CordbProcess;
class CordbAppDomain;
class CordbAssembly;
class CordbBreakpoint;
class CordbStepper;
class Cordb;
class CordbEnCSnapshot;
class CordbWin32EventThread;
class CordbRCEventThread;
class CordbRegisterSet;
class CordbNativeFrame;
class CordbObjectValue;
class CordbEnCErrorInfo;
class CordbEnCErrorInfoEnum;
class Instantiation;
class CordbType;
class CordbJITInfo;

class CordbMDA;

class CorpubPublish;
class CorpubProcess;
class CorpubAppDomain;
class CorpubProcessEnum;
class CorpubAppDomainEnum;


class RSLock;
class NeuterList;
class NeuterTicket;


extern HINSTANCE GetModuleInst();

//-----------------------------------------------------------------------------
// Smartpointer for internal Addref/Release
// Using Wrapper / Holder infrastructure from src\inc\Holder.h
//-----------------------------------------------------------------------------
template <typename TYPE>
inline void HolderRSRelease(TYPE *value)
{
    _ASSERTE(value != NULL);
    value->InternalRelease();
}

template <typename TYPE>
inline void HolderRSAddRef(TYPE *value)
{
    _ASSERTE(value != NULL);
    value->InternalAddRef();
}

// Smart ptrs for external refs. External refs are important
// b/c they may keep an object alive.
template <typename TYPE>
inline void HolderRSReleaseExternal(TYPE *value)
{
    _ASSERTE(value != NULL);
    value->Release();
}

template <typename TYPE>
inline void HolderRSAddRefExternal(TYPE *value)
{
    _ASSERTE(value != NULL);
    value->AddRef();
}

// The CordbBase::m_pProcess backpointer needs to adjust the external reference count, but manipulate it from
// within the RS. This means we need to skip debugging checks that ensure
// that the external count is only manipulated from outside the RS. Since we're
// skipping these checks, we call this an "Unsafe" pointer.
template <typename TYPE>
inline void HolderRSUnsafeExtRelease(TYPE *value)
{
    _ASSERTE(value != NULL);
    value->BaseRelease();
}
template <typename TYPE>
inline void HolderRSUnsafeExtAddRef(TYPE *value)
{
    _ASSERTE(value != NULL);
    value->BaseAddRef();
}

//-----------------------------------------------------------------------------
// Base Smart pointer implementation.
// This abstracts out the AddRef + Release methods.
//-----------------------------------------------------------------------------
template <typename TYPE, void (*ACQUIREF)(TYPE*), void (*RELEASEF)(TYPE*)>
class BaseSmartPtr
{
public:
    BaseSmartPtr () {
        // Ensure that these smart-ptrs are really ptr-sized.
        C_ASSERT(sizeof(*this) == sizeof(void*));
        m_ptr = NULL;
    }
    explicit BaseSmartPtr (TYPE * ptr) : m_ptr(NULL) {
        if (ptr != NULL)
        {
            RawAcquire(ptr);
        }
    }

    ~BaseSmartPtr() {
        Clear();
    }

    FORCEINLINE void Assign(TYPE * ptr)
    {
        // Do the AddRef before the release to avoid the release pinging 0 if we assign to ourself.
        if (ptr != NULL)
        {
            ACQUIREF(ptr);
        }
        if (m_ptr != NULL)
        {
            RELEASEF(m_ptr);
        }
        m_ptr = ptr;
    };

    FORCEINLINE void Clear()
    {
        if (m_ptr != NULL)
        {
            RawRelease();
        }
    }

    FORCEINLINE operator TYPE*() const
    {
        return m_ptr;
    }

    FORCEINLINE TYPE* GetValue() const
    {
        return m_ptr;
    }

    FORCEINLINE TYPE** operator & ()
    {
        // We allow getting the address so we can pass it in as an outparam.
        // But if we have a non-null m_Ptr, then it may get silently overwritten,
        // and thus we'll lose the chance to call release on it.
        // So we'll just avoid that pattern and assert to enforce it.
        _ASSERTE(m_ptr == NULL);
        return &m_ptr;
    }

    // For legacy purposes, some pre smart-pointer code needs to be able to get the
    // address of the pointer. This is needed for RSPtrArray::GetAddrOfIndex.
    FORCEINLINE TYPE** UnsafeGetAddr()
    {
        return &m_ptr;
    }    

    FORCEINLINE TYPE* operator->()
    {
        return m_ptr;
    }

    FORCEINLINE int operator==(TYPE* p)
    {
        return (m_ptr == p);
    }

    FORCEINLINE int operator!= (TYPE* p)
    {
        return (m_ptr != p);
    }

private:
    TYPE * m_ptr;

    // Don't allow copy ctor. Explicitly don't define body to force linker errors if they're called.
    BaseSmartPtr(BaseSmartPtr<TYPE,ACQUIREF,RELEASEF> & other);
    void operator=(BaseSmartPtr<TYPE,ACQUIREF,RELEASEF> & other);

    void RawAcquire(TYPE * p)
    {
        _ASSERTE(m_ptr == NULL);
        m_ptr= p;
        ACQUIREF(m_ptr);
    }
    void RawRelease()
    {
        _ASSERTE(m_ptr != NULL);
        RELEASEF(m_ptr);
        m_ptr = NULL;
    }

};

//-----------------------------------------------------------------------------
// Helper to make it easy to declare new SmartPtrs
//-----------------------------------------------------------------------------
#define DECLARE_MY_NEW_HOLDER(NAME, ADDREF, RELEASE) \
template<typename TYPE> \
class NAME : public BaseSmartPtr<TYPE, ADDREF, RELEASE> { \
public: \
    NAME() { }; \
    explicit NAME(TYPE * p) : BaseSmartPtr<TYPE, ADDREF, RELEASE>(p) { }; \
    FORCEINLINE NAME * GetAddr() { return this; } \
private:\
    NAME(NAME & other); \
    void operator=(NAME & other);   \
}; \

//-----------------------------------------------------------------------------
// Declare the various smart ptrs.
//-----------------------------------------------------------------------------
DECLARE_MY_NEW_HOLDER(RSSmartPtr, HolderRSAddRef, HolderRSRelease);
DECLARE_MY_NEW_HOLDER(RSExtSmartPtr, HolderRSAddRefExternal, HolderRSReleaseExternal); 

// The CordbBase::m_pProcess backpointer needs to adjust the external reference count, but manipulate it from
// within the RS. This means we need to skip debugging checks that ensure
// that the external count is only manipulated from outside the RS. Since we're
// skipping these checks, we call this an "Unsafe" pointer.
// This is purely used by CordbBase::m_pProcess. 
DECLARE_MY_NEW_HOLDER(RSUnsafeExternalSmartPtr, HolderRSUnsafeExtAddRef, HolderRSUnsafeExtRelease); 

//-----------------------------------------------------------------------------
// Simple array holder of RSSmartPtrs.
// Holds a reference to each element.
//-----------------------------------------------------------------------------
template<typename T>
class RSPtrArray
{
public:
    RSPtrArray()
    {
        m_pArray = NULL;
        m_cElements = 0;
    }

    ~RSPtrArray()
    {
        // Caller should have already Neutered
        _ASSERTE(IsEmpty());
    }

    // Is the array emtpy? 
    bool IsEmpty() const
    {
        return (m_pArray == NULL);
    }

    // Allocate an array of ptrs.
    // Returns false if not enough memory; else true.
    bool Alloc(unsigned int cElements)
    {
        // Caller should have already Neutered
        _ASSERTE(IsEmpty());

        // It's legal to allocate 0 items. We'll succeed the allocation, but still claim that IsEmpty() == true.
        if (cElements == 0)
        {
            return true;
        }

        // RSSmartPtr ctor will ensure all elements are null initialized.
        m_pArray = new (nothrow) RSSmartPtr<T> [cElements];
        if (m_pArray == NULL)
        {
            return false;
        }

        m_cElements = cElements;
        return true;
    }

   
    // Neuter all elements in the array.
    void NeuterAndClear(NeuterTicket ticket)
    {
        for(unsigned int i = 0; i < m_cElements; i++)
        {
            if (m_pArray[i] != NULL)
            {
                m_pArray[i]->Neuter(ticket);
            }
        }

        // Invoke dtors on each element which will release.
        delete [] m_pArray;
        m_pArray = NULL;
        m_cElements = 0;
    }

    // Array lookup. Caller gaurantees this is in range.
    // Used for reading
    T* operator [] (unsigned int index) const
    {
        _ASSERTE(m_pArray != NULL);
        CONSISTENCY_CHECK_MSGF((index <= m_cElements), ("Index out of range. Index=%u, Max=%u\n", index, m_cElements));
        
        return m_pArray[index];
    }

    // Assign a given index to the given value. The array holder will increment the internal reference on the value.
    void Assign(unsigned int index, T* pValue)
    {
        _ASSERTE(m_pArray != NULL);
        CONSISTENCY_CHECK_MSGF((index <= m_cElements), ("Index out of range. Index=%u, Max=%u\n", index, m_cElements));
        
        m_pArray[index].Assign(pValue);
    }

    // Get lenght of array in elements.
    unsigned int Length() const
    {
        return m_cElements;
    }

    // Some things need to get the address of an element in the table.
    // For example, CordbThreads have an array of CordbFrame objects, and then CordbChains describe a range
    // or frames via pointers into the CordbThread's array.
    // This is a dangerous operation because it lets us side-step reference counting and protection.
    T ** UnsafeGetAddrOfIndex(unsigned int index)
    {
        return m_pArray[index].UnsafeGetAddr();
    }

private:
    // Raw array of values. 
    RSSmartPtr<T> * m_pArray;

    // Number of elements in m_pArray. Note the following is always true: (m_cElements == 0) == (m_pArray == NULL);
    unsigned int m_cElements; 
};



//-----------------------------------------------------------------------------
// Table for RSptrs
// This lets us map cookies <--> RSPTR_*,
// Then we just put the cookie in the IPC block instead of the raw RSPTR.
// This will also adjust the internal-reference count on the T* object.
// This isolates the RS from bugs in the LS.
// We templatize by type for type safety.
// Caller must syncrhonize all access (preferably w/ the stop-go lock).
//-----------------------------------------------------------------------------
template <class T>
class RsPtrTable
{
public:
    RsPtrTable()
    {
        m_pTable = NULL;
        m_cEntries = 0;
    }
    ~RsPtrTable()
    {        
        Clear();
    }
    void Clear()
    {
        for(UINT i = 0; i < m_cEntries; i++)
        {
            if (m_pTable[i])
            {
                m_pTable[i]->InternalRelease();
            }
        }
        delete [] m_pTable;
        m_pTable = NULL;
        m_cEntries = 0;
    }

    // Add a value into table.  Value can't be NULL.
    // Returns 0 on failure (such as oom),
    // Returns a non-zero cookie on success.
    UINT Add(T* pValue)
    {
        _ASSERTE(pValue != NULL);
        // skip 0 because it's an invalid handle.
        for(UINT i = 1; ; i++)
        {
            // If we've run out of space, allocate new space
            if( i >= m_cEntries ) 
            {
                if( !Grow() )
                {
                    return 0;   // failed to grow
                }
                _ASSERTE( i < m_cEntries );
                _ASSERTE( m_pTable[i] == NULL );
                // Since we grew, the next slot should now be open.
            }
            
            if (m_pTable[i] == NULL)
            {
                m_pTable[i] = pValue;
                pValue->InternalAddRef();
                return i;
            }
        }
        UNREACHABLE();      
    }

    // Lookup the value based off the cookie, which was obtained via "Add".
    // return NULL on error. 
    T* Lookup(UINT cookie)
    {
        _ASSERTE(cookie != 0);
        if (cookie >= m_cEntries) 
        {
            CONSISTENCY_CHECK_MSGF(false, ("Cookie out of range.Cookie=0x%x. Size=0x%x.\n", cookie, m_cEntries));
            return NULL;
        }
        T*  p = m_pTable[cookie];
        if (p == NULL) 
        {   
            CONSISTENCY_CHECK_MSGF(false, ("Cookie is for empty slot.Cookie=0x%x.\n", cookie));
            return NULL; // empty!
        }
        return p;
    }

    T* LookupAndRemove(UINT cookie)
    {
        _ASSERTE(cookie != 0);
        T* p  = Lookup(cookie);
        if (p != NULL)
        {
            m_pTable[cookie] = NULL;
            p->InternalRelease();
        }
        return p;
    }

protected:
    // Resize the m_pTable array.
    bool Grow()
    {
        if (m_pTable == NULL)
        {
            _ASSERTE(m_cEntries == 0);
            size_t cSize = 10;
            m_pTable = new (nothrow) T*[cSize];
            if (m_pTable == NULL)
            {
                return false;
            }
            m_cEntries = cSize;
            ZeroMemory(m_pTable, sizeof(T*) * m_cEntries);
            return true;
        }
        size_t cNewSize = (m_cEntries * 3 / 2) + 1;
        _ASSERTE(cNewSize > m_cEntries);
        T** p = new (nothrow) T*[cNewSize];
        if (p == NULL) 
        {
            return false;
        }
        ZeroMemory(p, sizeof(T*) * cNewSize);        


        // Copy over old stuff
        memcpy(p, m_pTable, sizeof(T*) * m_cEntries);
        delete [] m_pTable;        

        m_pTable = p;
        m_cEntries = cNewSize;
        return true;        
    }
    
    T** m_pTable;
    size_t m_cEntries;    
};


//-----------------------------------------------------------------------------
// Simple holder to keep a copy of a string.
//-----------------------------------------------------------------------------
class StringCopyHolder
{
public:
    StringCopyHolder();
    ~StringCopyHolder();

    bool AssignCopy(const WCHAR * pCopy);
    operator const WCHAR* () const
    {
        return m_szData;
    }

private:
    // Disallow copying (to prevent double-free) - no implementation
    StringCopyHolder( const StringCopyHolder& rhs );
    StringCopyHolder& operator=( const StringCopyHolder& rhs );
    
    WCHAR * m_szData;

};



//-----------------------------------------------------------------------------
// Have the extra level of indirection is useful for catching Cordbg errors.
//-----------------------------------------------------------------------------
#ifdef _DEBUG
    // On debug, we have an opportunity to catch failing hresults during reproes.
    #define ErrWrapper(hr) ErrWrapperHelper(hr, __FILE__, __LINE__)

    inline HRESULT ErrWrapperHelper(HRESULT hr, const char * szFile, int line)
    {
        if (FAILED(hr))
        {
            DWORD dwErr = REGUTIL::GetConfigDWORD(L"DbgBreakOnErr", 0);
            if (dwErr)
            {
                CONSISTENCY_CHECK_MSGF(false, ("Dbg Error break, hr=0x%08x, '%s':%d", hr, szFile, line));
            }
        }
        return hr;
    }
#else
    // On release, it's just an identity function
    #define ErrWrapper(hr) (hr)
#endif

//-----------------------------------------------------------------------------
// Quick helpers for threading semantics
//-----------------------------------------------------------------------------

bool IsWin32EventThread(CordbProcess* p);
bool IsRCEventThread(Cordb* p);

/* ------------------------------------------------------------------------- *
 * Typedefs
 * ------------------------------------------------------------------------- */

typedef void* REMOTE_PTR;


//-----------------------------------------------------------------------------
// Wrapper class for locks. This is like Crst on the LS
//-----------------------------------------------------------------------------

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(RSLock *);
template BOOL CompareDefault(RSLock*,RSLock*);
typedef Holder<RSLock*, DoNothing, DoNothing> HACKRSLockHolder;
#endif


class RSLock
{
public:
    // Attrs, can be bitwise-or together.
    enum ELockAttr
    {
        cLockUninit     = 0x00000000,
        cLockReentrant  = 0x00000001,
        cLockFlat       = 0x00000002,
    };

    // To prevent deadlocks, we order all locks.
    // A thread must acquire higher-numbered locks before lower numbered locks.
    // These are used as indices into an array, so number them accordingly!
    enum ERSLockLevel
    {
        // Size of the array..
        LL_MAX = 4,

        // The Stop-Go lock is used to make Stop + Continue be atomic operations.
        // These methods will toggle the Process-lock b/c they go between multiple threads.
        // This lock can never be taken on the Win32 ET.
        LL_STOP_GO_LOCK = 3,

        // The win32-event-thread behaves as if it held a lock at this level.
        LL_WIN32_EVENT_THREAD = 2,

        // The process lock is the primary lock for a CordbProcess object. It synchronizes
        // between RCET, W32ET, and user threads.
        LL_PROCESS_LOCK = 1,

        // These are all leaf locks (they don't take any other lock once they're held).
        LL_PROCESS_LIST_LOCK = 0,

        // Win32 send lock is shared by all processes accessing a single w32et.
        LL_WIN32_SEND_LOCK = 0,

        // Small lock around sending IPC events to support hacks in func-eval abort.
        // See CordbEval::Abort for details.
        LL_FUNC_EVAL_ABORT_HACK_LOCK = 0
    };

    // Initialize a lock w/ debugging info. szTag must be a string literal.
    void Init(const char * szTag, ELockAttr eAttr, ERSLockLevel level);
    void Destroy();

    void Lock();
    void Unlock();

protected:
    // Accessors for holders.
    static void HolderEnter(RSLock * pLock)
    {
        pLock->Lock();
    }
    static void HolderLeave(RSLock * pLock)
    {
        pLock->Unlock();
    }


    CRITICAL_SECTION m_lock;

#ifdef _DEBUG
public:
    RSLock();
    ~RSLock();

    const char * Name() { return m_szTag; }

    // Returns true if this thread has the lock.
    bool HasLock();

    // Returns true if this is safe to take on this thread (ie, this thread
    // doesn't already hold bigger locks).
    // bool IsSafeToTake();

    ERSLockLevel GetLevel() { return m_level; }

    // If we're inited, we must have either  cLockReentrant or cLockFlat specified.
    bool IsInit() { return m_eAttr != 0; }
    bool IsReentrant() { return (m_eAttr & cLockReentrant) == cLockReentrant; }

protected:
    ERSLockLevel m_level;
    ELockAttr m_eAttr;
    int m_count;
    DWORD m_tidOwner;
    const char * m_szTag;

#endif // #if debug

public:
    typedef Holder<RSLock *, RSLock::HolderEnter, RSLock::HolderLeave> RSLockHolder;

};

typedef RSLock::RSLockHolder RSLockHolder;

// In the RS, we should be using RSLocks instead of raw critical sections.
#define CRITICAL_SECTION USE_RSLOCK_INSTEAD_OF_CRITICAL_SECTION


/* ------------------------------------------------------------------------- *
 * Helper macros. Use the ATT_* macros below instead of these.
 * ------------------------------------------------------------------------- */

#define CORDBSetUnrecoverableError(__p, __hr, __code) \
    ((__p)->UnrecoverableError((__hr), (__code), __FILE__, __LINE__))

#define _CORDBCheckProcessStateOK(__p) \
    (!((__p)->m_unrecoverableError) && !((__p)->m_terminated) && !((__p)->m_detached))

#define _CORDBCheckProcessStateOKAndSync(__p, __c) \
    (!((__p)->m_unrecoverableError) && !((__p)->m_terminated) && !((__p)->m_detached) && \
    (__p)->GetSynchronized())

// Worker to get failure HR from given state. If not in a failure state, it yields __defaultHR.
// If a caller knows that we're in a failure state, it can pass in a failure value for __defaultHR.
#define CORDBHRFromProcessStateWorker(__p, __c, __defaultHR) \
        ((__p)->m_unrecoverableError ? CORDBG_E_UNRECOVERABLE_ERROR : \
         ((__p)->m_detached ? CORDBG_E_PROCESS_DETACHED : \
         ((__p)->m_terminated ? CORDBG_E_PROCESS_TERMINATED : \
         (!(__p)->GetSynchronized() ? CORDBG_E_PROCESS_NOT_SYNCHRONIZED \
         : (__defaultHR)))))

#define CORDBHRFromProcessState(__p, __c) \
    CORDBHRFromProcessStateWorker(__p, __c, S_OK) \



#define CORDBFailIfOnWin32EventThread(__p) \
    { \
        if (__p->IsWin32EventThread()) \
        { \
            return ErrWrapper(CORDBG_E_CANT_CALL_ON_THIS_THREAD); \
        } \
    }

#define CORDBRequireProcessStateOK(__p) { \
    if (!_CORDBCheckProcessStateOK(__p)) \
        return ErrWrapper(CORDBHRFromProcessState(__p, NULL)); }

// If we need to be synced, then we shouldn't be on the win32 Event-Thread.
#define CORDBRequireProcessStateOKAndSync(__p,__c) { \
    CORDBFailIfOnWin32EventThread(__p); \
    if (!_CORDBCheckProcessStateOKAndSync(__p, __c)) \
        return ErrWrapper(CORDBHRFromProcessState(__p, __c)); }

#define CORDBRequireProcessSynchronized(__p, __c) { \
    CORDBFailIfOnWin32EventThread(__p); \
    if (!(__p)->GetSynchronized()) return ErrWrapper(CORDBG_E_PROCESS_NOT_SYNCHRONIZED);}


//-----------------------------------------------------------------------------
// All public APIS fall into 2 categories regarding their API Threading Type (ATT)
// We use a standard set of macros to define & enforce each type.
//
// (1) ATT_REQUIRE_STOPPED
// We must be stopped (either synced or at a win32 event) to call this API.
// - We'll fail if we're not stopped.
// - If we're stopped, we'll sync. Thus after this API, we're always synced,
//   and Cordbg must call Continue to resume the process.
// - We'll take the Stop-Go-lock. This prevents another thread from continuing underneath us.
// - We may send IPC events.
// Common for APIs like Stacktracing
//
// (2) ATT_ALLOW_LIVE
// We do not have to be stopped to call this API.
// - We can be live, thus we can not take the stop-go lock (unless it's from a SC-holder).
// - If we're going to send IPC events, we must use a Stop-Continue holder.
// - Our stop-status is the same after this API as it was before.
// Common usage: read-only APIs.
//
// (2b) ATT_ALLOW_LIVE_DO_STOPGO.
// - shortcut macro to do #2, but throw in a stop-continue holder. These really
// should be in camp #1, but that would require an interface change.
//-----------------------------------------------------------------------------

// Helper macros for the ATT stuff
// Do checks that need to be done before we take the SG lock. These include checks
// where if we fail them, taking the SG lock could deadlock (such as being on win32 thread).
#define DO_PRE_STOP_GO_CHECKS \
    CORDBFailIfOnWin32EventThread(__proc_for_ATT) \
    if ((__proc_for_ATT)->m_unrecoverableError) { return CORDBG_E_UNRECOVERABLE_ERROR; } \

// Do checks after we take the SG lock. These include checks that rely on state protected
// by the SG lock.
#define DO_POST_STOP_GO_CHECKS \
    _ASSERTE((this->GetProcess() == __proc_for_ATT) || this->IsNeutered()); \
    if (this->IsNeutered()) { return CORDBG_E_OBJECT_NEUTERED; } \



// #1
// The exact details here are rocket-science.
// We cache the __proc value to a local variable (__proc_for_ATT) so that we don't re-evaluate __proc. (It also forces type-safety).
// This is essential in case __proc is something like "this->GetProcess()" and which can start returning NULL if 'this' 
// gets neutered underneath us. Caching guarantees that we'll be able to make it to the StopGo-lock. 
// 
// We explicitily check some things before taking the Stop-Go lock:
// - CORDBG_E_UNRECOVERABLE_ERROR before the lock because if that's set,
// we may have leaked locks to the outside world, so taking the StopGo lock later could fail.
// - Are we on the W32et - can't take sg lock if on W32et
// Then we immediately take the stop-go lock to prevent another thread from continuing underneath us.
// Then, if we're stopped, we ensure that we're also synced.
// Stopped includes:
// - Win32-stopped
// - fake win32-stopped. Eg, between SuspendUnmanagedThreads & ResumeUnmanagedThreads
//   (one way to get here is getting debug events during the special-deferment region)
// - synchronized
// If we're not stopped, then we fail.  This macro must never return S_OK.
#define ATT_REQUIRE_STOPPED_MAY_FAIL(__proc) \
    CordbProcess * __proc_for_ATT = (__proc); \
    DO_PRE_STOP_GO_CHECKS; \
    RSLockHolder __ch(__proc_for_ATT->GetStopGoLock()); \
    DO_POST_STOP_GO_CHECKS; \
    if (!__proc_for_ATT->m_initialized) { return CORDBG_E_NOTREADY; } \
    if ((__proc_for_ATT)->IsStopped()) { \
        HRESULT _hr2 = (__proc_for_ATT)->StartSyncFromWin32Stop(NULL); \
        if (FAILED(_hr2)) return _hr2; \
    } \
    if (!_CORDBCheckProcessStateOKAndSync(__proc_for_ATT, NULL)) \
        return CORDBHRFromProcessStateWorker(__proc_for_ATT, NULL, E_FAIL); \


// #1b - allows it to be non-inited. This should look just like ATT_REQUIRE_STOPPED_MAY_FAIL
// except it doesn't do SSFW32Stop and doesn't have the m_initialized check.
#define ATT_REQUIRE_SYNCED_OR_NONINIT_MAY_FAIL(__proc) \
    CordbProcess * __proc_for_ATT = (__proc); \
    DO_PRE_STOP_GO_CHECKS; \
    RSLockHolder __ch(__proc_for_ATT->GetStopGoLock()); \
    DO_POST_STOP_GO_CHECKS; \
    if (!_CORDBCheckProcessStateOKAndSync(__proc_for_ATT, NULL)) \
        return CORDBHRFromProcessStateWorker(__proc_for_ATT, NULL, E_FAIL); \


// Gross variant on #1.
// This is a very dangerous ATT contract; but we need to support it for backwards compat.
// Some APIs, like ICDProcess:EnumerateThreads can be used before the process is actually
// initialized (kind of for interop-debugging).
// These can't check the m_initialized flag b/c that may not be set yet.
// They also can't sync the runtime.
// This should only be used for non-blocking leaf activity.
#define ATT_EVERETT_HACK_REQUIRE_STOPPED_ALLOW_NONINIT(__proc) \
    CordbProcess * __proc_for_ATT = (__proc); \
    DO_PRE_STOP_GO_CHECKS; \
    RSLockHolder __ch(__proc_for_ATT->GetStopGoLock()); \
    DO_POST_STOP_GO_CHECKS; \
    if (!(__proc_for_ATT)->IsStopped()) { return CORDBG_E_PROCESS_NOT_SYNCHRONIZED; } \


// #2 - caller may think debuggee is live, but throw in a Stop-Continue holder.
#define ATT_ALLOW_LIVE_DO_STOPGO(__proc) \
    CordbProcess * __proc_for_ATT = (__proc); \
    DO_PRE_STOP_GO_CHECKS; \
    CORDBRequireProcessStateOK(__proc_for_ATT); \
    RSLockHolder __ch(__proc_for_ATT->GetStopGoLock()); \
    DO_POST_STOP_GO_CHECKS; \
    StopContinueHolder __hStopGo; \
    { \
        HRESULT _hr2 = __hStopGo.Init(__proc_for_ATT); \
        if (FAILED(_hr2)) return _hr2; \
    } \
    _ASSERTE((__proc_for_ATT)->GetSynchronized());



//-----------------------------------------------------------------------------
// StopContinueHolder. Ensure that we're synced during a certain region.
// (Particularly when sending an IPCEvent)
// Calls ICorDebugProcess::Stop & IMDArocess::Continue.
// Example usage:
//
// {
//   StopContinueHolder h;
//   IfFailRet(h.Init(process))
//   SendIPCEvent
// } // continue automatically called.
//-----------------------------------------------------------------------------

class CordbProcess;
class StopContinueHolder
{
public:
    StopContinueHolder() : m_p(NULL) { };

    HRESULT Init(CordbProcess * p);
    ~StopContinueHolder();

protected:
    CordbProcess * m_p;
};


/* ------------------------------------------------------------------------- *
 * Base class
 * ------------------------------------------------------------------------- */

#define COM_METHOD  HRESULT STDMETHODCALLTYPE

typedef enum {
    enumCordbUnknown,       //  0
    enumCordb,              //  1   1  [1]x1
    enumCordbProcess,       //  2   1  [1]x1
    enumCordbAppDomain,     //  3   1  [1]x1
    enumCordbAssembly,      //  4
    enumCordbModule,        //  5   15 [27-38,55-57]x1
    enumCordbClass,         //  6
    enumCordbFunction,      //  7
    enumCordbThread,        //  8   2  [4,7]x1
    enumCordbCode,          //  9
    enumCordbChain,         //  0
    enumCordbChainEnum,     //  11
    enumCordbContext,       //  12
    enumCordbFrame,         //  13
    enumCordbFrameEnum,     //  14
    enumCordbValueEnum,     //  15
    enumCordbRegisterSet,   //  16
    enumCordbJITILFrame,    //  17
    enumCordbBreakpoint,    //  18
    enumCordbStepper,       //  19
    enumCordbValue,         //  20
    enumCordbEnCSnapshot,   //  21
    enumCordbEval,          //  22
    enumCordbUnmanagedThread,// 23
    enumCorpubPublish,      //  24
    enumCorpubProcess,      //  25
    enumCorpubAppDomain,    //  26
    enumCorpubProcessEnum,  //  27
    enumCorpubAppDomainEnum,//  28
    enumCordbEnumFilter,    //  29
    enumCordbEnCErrorInfo,  //  30
    enumCordbEnCErrorInfoEnum,//31
    enumCordbUnmanagedEvent,//  32
    enumCordbWin32EventThread,//33
    enumCordbRCEventThread, //  34
    enumCordbNativeFrame,   //  35
    enumCordbObjectValue,   //  36
    enumCordbType,          //  37
    enumCordbJITInfo,       //  38
    enumCordbEval2,         //  39
    enumCordbMDA,           //  40
    enumCordbHashTableEnum, //  41
    enumCordbCodeEnum,      //  42
    enumMaxDerived,         //  
    enumMaxThis = 1024
} enumCordbDerived;



//-----------------------------------------------------------------------------
// Support for Native Breakpoints
//-----------------------------------------------------------------------------
struct NativePatch
{
    void * pAddress; // pointer into the LS address space.
    PRD_TYPE opcode; // opcode to restore with.

    inline bool operator==(NativePatch p2)
    {
        return memcmp(this, &p2, sizeof(p2)) == 0;
    }
};

//-----------------------------------------------------------------------------
// Cross-platform patch operations
//-----------------------------------------------------------------------------

// Remove the int3 from the remote address
HRESULT RemoveRemotePatch(HANDLE hPid, const void * pRemoteAddress, PRD_TYPE opcode);

// This flavor is assuming our caller already knows the opcode.
HRESULT ApplyRemotePatch(HANDLE hPid, const void * pRemoteAddress);

// Apply the patch and get the opcode that we're replacing.
HRESULT ApplyRemotePatch(HANDLE hPid, const void * pRemoteAddress, PRD_TYPE * pOpcode);


class CordbHashTable;

// Common base for both CorPublish + CorDebug objects.
class CordbCommonBase : public IUnknown
{
public:
#ifdef _DEBUG
    static LONG m_saDwInstance[enumMaxDerived]; // instance x this
    static LONG m_saDwAlive[enumMaxDerived];
    static PVOID m_sdThis[enumMaxDerived][enumMaxThis];
    DWORD m_dwInstance;
    enumCordbDerived m_type;
#endif

// GENERIC: made this private as I'm changing the use of m_id for CordbClass, and
// I want to make sure I catch all the places where m_id is used directly and cast
// to/from tokens and/or (void*).
    UINT_PTR    m_id;

private:
    // Sticky bit set when we neuter an object. All methods (besides AddRef,Release,QI)
    // should check this bit and fail via the FAIL_IF_NEUTERED macro.
    bool        m_fIsNeutered;

    // Mark that this object can be "neutered at will". NeuterList::SweepAllNeuterAtWillObjects
    // looks at this bit.
    // For some objects, we don't explicitly mark when the lifetime is up. The only way
    // we know is when external count goes to 0. This avoids forcing us to do cleanup
    // in the dtor (which may come at a bad time). Sticky bit set in BaseRelease().
    bool        m_fNeuterAtWill;
public:

    static LONG s_CordbObjectUID;    // Unique ID for each object.
    static LONG s_TotalObjectCount; // total number of outstanding objects.

    CordbCommonBase(UINT_PTR id, enumCordbDerived type)
    {
        init(id, type);
    }

    CordbCommonBase(UINT_PTR id)
    {
        init(id, enumCordbUnknown);
    }

    void init(UINT_PTR id, enumCordbDerived type)
    {
        // To help us track object leaks, we want to log when we create & destory CordbBase objects.
#ifdef _DEBUG
        InterlockedIncrement(&s_TotalObjectCount);
        InterlockedIncrement(&s_CordbObjectUID);

        LOG((LF_CORDB, LL_EVERYTHING, "Memory: CordbBase object allocated: this=%p, count=%d, id=%p, Type=%d\n", this, s_CordbObjectUID, id, type));
#endif

        m_fNeuterAtWill = false;
        m_fIsNeutered = false;

        m_id = id;
        m_RefCount = 0;

#ifdef _DEBUG
        m_type = type;
        //m_dwInstance = CordbBase::m_saDwInstance[m_type];
        //InterlockedIncrement(&CordbBase::m_saDwInstance[m_type]);
        //InterlockedIncrement(&CordbBase::m_saDwAlive[m_type]);
        //if (m_dwInstance < enumMaxThis)
        //{
        //    m_sdThis[m_type][m_dwInstance] = this;
        //}
#endif
    }

    virtual ~CordbCommonBase()
    {
        // If we're deleting, we really should have released any outstanding reference.
        // If we call Release() on a deleted object, we'll av (especially b/c Release
        // may call delete again).
        CONSISTENCY_CHECK_MSGF(m_RefCount == 0, ("Deleting w/ non-zero ref count. 0x%08x", m_RefCount));

#ifdef _DEBUG
        //InterlockedDecrement(&CordbBase::m_saDwAlive[m_type]);
        //if (m_dwInstance < enumMaxThis)
        //{
        //    m_sdThis[m_type][m_dwInstance] = NULL;
        //}
#endif
        // To help us track object leaks, we want to log when we create & destory CordbBase objects.
        LOG((LF_CORDB, LL_EVERYTHING, "Memory: CordbBase object deleted: this=%p, id=%p, Refcount=0x%x\n", this, m_id, m_RefCount));

#ifdef _DEBUG
        InterlockedDecrement(&s_TotalObjectCount);
        _ASSERTE(s_TotalObjectCount >= 0);
#endif

        // Don't shutdown logic until everybody is done with it.
        // If we leak objects, this may mean that we never shutdown logging at all!
#if defined(_DEBUG) && defined(LOGGING)
        if (s_TotalObjectCount == 0)
        {
            ShutdownLogging();
        }
#endif
    }

    /*
        Documented:                                May 2, 2001

        Member function behavior of a neutered COM object:

             1. AddRef(), Release(), QueryInterface() work as normal.
                 a. This gives folks who are responsable for pairing a Release() with
                    an AddRef() a chance to dereferance thier pointer and call Release()
                    when they are informed, explicitly or implicitly, that the object is neutered.

             2. Any other member function will return an error code unless documented.
                 a. If a member fuction returns information when the COM object is
                    neutered then the semantics of that function need to be documented.
                    (ie. If an AppDomain is unloaded and you have a referance to the COM
                    object representing the AppDomain, how _should_ it behave? That behavior
                    should be documented)


        Postcondions of Neuter():

             1. All circular referances (aka back-pointers) are "broken". They are broken
                by calling Release() on all "Weak Referances" to the object. If you're a purist,
                these pointers should also be NULLed out.
                 a. Weak Referances/Strong Referances:
                     i. If any objects are not "reachable" from the root (ie. stack or from global pointers)
                        they should be reclaimed. If they are not, they are leaked and there is a bug.
                    ii. There must be a partial order on the objects such that if A < B then:
                         1. A has a referance to B. This referance is a "strong referance"
                         2. A, and thus B, is reachable from the root
                   iii. If a referance belongs in the partial order then it is a "strong referance" else
                        it is a weak referance.
         *** 2. Sufficient conditions to ensure no COM objects are leaked: ***
                 a. When Neuter() is invoked:
                     i. Calles Release on all its weak referances.
                    ii. Then, for each strong referance:
                         1. invoke Neuter()
                         2. invoke Release()
                   iii. If it's derived from a CordbXXX class, call Neuter() on the base class.
                         1. Sense Neuter() is virtual, use the scope specifier Cordb[BaseClass]::Neuter().
             3. All members return error codes, except:
                 a. Members of IUknown, AddRef(), Release(), QueryInterfac()
                 b. Those documented to have functionality when the object is neutered.
                     i. Neuter() still works w/o error. If it is invoke a second time it will have already
                        released all its strong and weak referances so it could just return.


        Alternate design ideas:

            DESIGN: Note that it's possible for object B to have two parents in the partial order
                    and it must be documented which one is responsible for calling Neuter() on B.
                     1. For example, CordbCode could reasonably be a sibling of CordbFunction and CordbNativeFrame.
                        Which one should call Release()? For now we have CordbFunction call Release() on CordbCode.

            DESIGN: It is not a necessary condition in that Neuter() invoke Release() on all
                    it's strong referances. Instead, it would be sufficent to ensure all object are released, that
                    each object call Release() on all its strong pointers in its destructor.
                     1. This might be done if its necessary for some member to return "tombstone"
                        information after the object has been netuered() which involves the siblings (wrt poset)
                        of the object. However, no sibling could access a parent (wrt poset) because
                        Neuter called Release() on all its weak pointers.

            DESIGN: Rename Neuter() to some name that more accurately reflect the semantics.
                     1. The three operations are:
                         a. ReleaseWeakPointers()
                         b. NeuterStrongPointers()
                         c. ReleaseStrongPointers()
                             1. Assert that it's done after NeuterStrongPointers()
                     2. That would introduce a bunch of functions... but it would be clear.

            DESIGN: CordbBase could provide a function to register strong and weak referances. That way CordbBase
                    could implement a general version of ReleaseWeak/ReleaseStrong/NeuterStrongPointers(). This
                    would provide a very error resistant framework for extending the object model plus it would
                    be very explicit about what is going on.
                        One thing that might trip this is idea up is that if an object has two parents,
                        like the CordbCode might, then either both objects call Neuter or one is referance
                        is made weak.


         Our implementation:

            The graph fromed by the strong referances must remain acyclic.
            It's up to the developer (YOU!) to ensure that each Neuter
            function maintains that invariant.

            Here is the current Partial Order on CordbXXX objects. (All these classes
            eventually chain to CordbBase.Neuter() for completeness.)

             Cordb
                CordbProcess
                    CordbAppDomain
                        CordbBreakPoints
                        CordbAssembly
                        CordbModule
                            CordbClass
                            CordbFunction
                                CordbCode (Can we assert a thread will not referance
                                            the same CordbCode as a CordbFunction?)
                    CordbThread
                        CordbChains
                        CordbNativeFrame -> CordbFrame (Chain to baseClass)
                            CordbJITILFrame



     */

    virtual void Neuter(NeuterTicket ticket);

    // Unsafe neuter for an object that's already dead.
    void UnsafeNeuterDeadObject();


#ifdef _DEBUG
    // For debugging (asserts, logging, etc) provide a pretty name (this is 1:1 w/ the VTable)
    // We provide a default impl in the base object in case this gets called from a dtor (virtuals
    // called from dtors use the base version, not the derived). A pure call would AV in that case.
    virtual const char * DbgGetName() { return "CordbBase"; };
#endif

    bool IsNeutered() const {LEAF_CONTRACT;  return m_fIsNeutered; }
    bool IsNeuterAtWill() const { LEAF_CONTRACT; return m_fNeuterAtWill; }
    void MarkNeuterAtWill() { LEAF_CONTRACT; m_fNeuterAtWill = true; }

    //-----------------------------------------------------------
    // IUnknown support
    //----------------------------------------------------------

private:
    // We maintain both an internal + external refcount. This allows us to catch
    // if an external caller has too many releases.
    // low  bits are internal count, high  bits are external count
    // so Total count = (m_RefCount & CordbBase_InternalRefCountMask) + (m_RefCount >> CordbBase_ExternalRefCountShift);
    typedef LONGLONG       MixedRefCountSigned;
    typedef ULONGLONG      MixedRefCountUnsigned;
    typedef LONG           ExternalRefCount;
    MixedRefCountUnsigned  m_RefCount;
public:

    // Adjust the internal ref count.
    // These aren't available to the external world, so only internal code can manipulate the internal count.
    void InternalAddRef();
    void InternalRelease();

    // Derived versions of AddRef / Release will call these.
    // External AddRef & Release
    // These do not have any additional Asserts to enforce that we're not manipulating the external count
    // from internal.
    ULONG STDMETHODCALLTYPE BaseAddRef();
    ULONG STDMETHODCALLTYPE BaseRelease();

    ULONG STDMETHODCALLTYPE BaseAddRefEnforceExternal();
    ULONG STDMETHODCALLTYPE BaseReleaseEnforceExternal();

    // Do an AddRef against the External count. This is a semantics issue.
    // We use this when an internal component Addrefs out-parameters (which Cordbg will call Release on).
    // This just does a regular external AddRef().
    void ExternalAddRef();

protected:
    void NeuterAndClearHashtable(CordbHashTable * pCordbHashtable, NeuterTicket ticket);

    static void InitializeCommon();

private:
    static void AddDebugPrivilege();
};

#define CordbBase_ExternalRefCountShift 32
#define CordbBase_InternalRefCountMask 0xFFFFFFFF
#define CordbBase_InternalRefCountMax  0x7FFFFFFF

#ifdef _DEBUG
// Does the given Cordb object type have affinity to a CordbProcess object?
// This is only used for certain asserts.
inline bool DoesCordbObjectTypeHaveProcessPtr(enumCordbDerived type)
{
    return 
        (type != enumCordbCodeEnum) &&
        (type != enumCordb) &&
        (type != enumCordbHashTableEnum);
}
#endif

// Base class specifically for CorDebug objects
class CordbBase : public CordbCommonBase
{
public:
    CordbBase(CordbProcess * pProcess, UINT_PTR id, enumCordbDerived type) : CordbCommonBase(id, type)
    {
        // CordbProcess can't pass 'this' to base class, per error C4355. So we pass null and set later.
        _ASSERTE((pProcess != NULL) || 
            ((type) == enumCordbProcess) ||
            !DoesCordbObjectTypeHaveProcessPtr(type));

        m_pProcess.Assign(pProcess);
    }

    CordbBase(CordbProcess * pProcess, UINT_PTR id) : CordbCommonBase(id)
    {
        _ASSERTE(pProcess != NULL);
        m_pProcess.Assign(pProcess);
    }

    virtual ~CordbBase()
    {
        // Derived classes should not have cleared out our pointer.
        // CordbProcess's Neuter explicitly nulls out its pointer to avoid circular reference.
        _ASSERTE(m_pProcess!= NULL || 
            (CordbCommonBase::m_type == enumCordbProcess) ||
            !DoesCordbObjectTypeHaveProcessPtr(CordbCommonBase::m_type));
        
        _ASSERTE(IsNeutered() ||
            (m_type == enumCordbBreakpoint) ||
            (m_type == enumCordbStepper));
    }

    // Get the CordbProcess object that this CordbBase object is associated with (or NULL if there's none).
    CordbProcess * GetProcess()
    {
        return m_pProcess;
    }
protected:
    // All objects need a strong pointer back to the process so that they can get access to key locks
    // held by the process (StopGo lock) so that they can synchronize their operations against neutering.
    // This pointer is cleared in our dtor, and not when we're neutered. Since we can't control when the
    // dtor is called (it's controlled by external references), we classify this as an external reference too.
    //
    // This is the only "strong" reference backpointer that objects need have. All other backpointers can be weak references
    // because when a parent object is neutered, it will null out all weak reference pointers in all of its children.
    // That will also break any potential cycles.     
    RSUnsafeExternalSmartPtr<CordbProcess> m_pProcess;
};





//-----------------------------------------------------------------------------
// Macro to check if a CordbXXX object is neutered, and return a standard
// error code if it is.
// We pass the 'this' pointer of the object in because it gives us some extra
// flexibility and lets us log debug info.
// It is an API breach to access a neutered object.
//-----------------------------------------------------------------------------
#define FAIL_IF_NEUTERED(pThis) \
int _____Neuter_Status_Already_Marked; \
_____Neuter_Status_Already_Marked = 0; \
{\
    if (pThis->IsNeutered()) { \
            LOG((LF_CORDB, LL_ALWAYS, "Accessing a neutered object at %p\n", pThis)); \
            return ErrWrapper(CORDBG_E_OBJECT_NEUTERED); \
    } \
}

// We have an OK_IF_NEUTERED macro to say that this method can be safely
// called if we're neutered. Mostly for semantic benefits.
// Also, if a method is marked OK, then somebody won't go and add a 'fail'
// This is an extremely dangerous quality because:
// 1) it means that we have no synchronization (can't take the Stop-Go lock)
// 2) none of our backpointers are usable (they may be nulled out at anytime by another thread).
//    - this also means we absolutely can't send IPC events (since that requires a CordbProcess)
// 3) The only safe data are blittalbe embedded fields (eg, a pid or stack range) 
//
// Any usage of this macro should clearly specificy why this is safe.
#define OK_IF_NEUTERED(pThis) \
int _____Neuter_Status_Already_Marked; \
_____Neuter_Status_Already_Marked = 0;


// ----------------------------------------------------------------------------
// Hash table for CordbBase objects.
// - Uses Internal AddRef/Release (not external)
// - Templatize for type-safety w/ Cordb objects
// - Many hashtables are implicitly protected by a lock. For debug-only, we
//   explicitly associate w/ an optional RSLock and assert that lock is held on access.
// ----------------------------------------------------------------------------

struct CordbHashEntry
{
    FREEHASHENTRY entry;
    CordbBase *pBase;
};

class CordbHashTable : private CHashTableAndData<CNewDataNoThrow>
{
private:
    bool    m_initialized;
    SIZE_T  m_count;

    BOOL Cmp(const BYTE *pc1, const HASHENTRY *pc2)
    {
        LEAF_CONTRACT;

        return ((ULONG_PTR)pc1) != ((CordbHashEntry*)pc2)->pBase->m_id;
    }

    ULONG HASH(ULONG_PTR id)
    {
        return (ULONG)(id);
    }

    BYTE *KEY(BYTE *id)
    {
        return (BYTE *)id;
    }

public:
    bool IsInitialized();

#ifndef DACCESS_COMPILE
    CordbHashTable(ULONG size)
    : CHashTableAndData<CNewDataNoThrow>(size), m_initialized(false), m_count(0)
    {
#ifdef _DEBUG
    m_pDbgLock = NULL;
    m_dbgChangeCount = 0;
#endif
    }
    virtual ~CordbHashTable();

#ifdef _DEBUG
    // CordbHashTables may be protected by a lock. For debug-builds, we can associate
    // the hash w/ that lock and then assert if it's not held.
    void DebugSetRSLock(RSLock * pLock)
    {
        m_pDbgLock = pLock;
    }
    int GetChangeCount() { return m_dbgChangeCount; }
private:
    void AssertIsProtected();

    // Increment the Change count. This can be used to check if the hashtable changes while being enumerated.
    void DbgIncChangeCount() { m_dbgChangeCount++; }

    int m_dbgChangeCount;
    RSLock * m_pDbgLock;
#else
    // RSLock association is a no-op on free builds.
    void AssertIsProtected() { };
    void DbgIncChangeCount() { };
#endif // _DEBUG

public:


#endif

    ULONG32 GetCount()
    {
        return ((ULONG32)m_count);
    }

    // These operators are unsafe b/c they have no typesafety.
    // Use a derived CordbSafeHashTable<T> instead.
    HRESULT UnsafeAddBase(CordbBase *pBase);
    HRESULT UnsafeSwapBase(CordbBase* pBaseOld, CordbBase* pBaseNew);
    CordbBase *UnsafeGetBase(ULONG_PTR id, BOOL fFab = TRUE);
    CordbBase *UnsafeRemoveBase(ULONG_PTR id);

    CordbBase *UnsafeFindFirst(HASHFIND *find);
    CordbBase *UnsafeFindNext(HASHFIND *find);

    // Unlocked versions don't assert that the lock us held.
    CordbBase *UnsafeUnlockedFindFirst(HASHFIND *find);
    CordbBase *UnsafeUnlockedFindNext(HASHFIND *find);
};


// Typesafe wrapper around a normal hash table
// T is expected to be a derived clas of CordbBase
template <class T>
class CordbSafeHashTable : public CordbHashTable
{
public:
#ifndef DACCESS_COMPILE
    CordbSafeHashTable<T>(ULONG size) : CordbHashTable(size)
    {
    }
#endif
    // Typesafe wrappers
    HRESULT AddBase(T * pBase) { return UnsafeAddBase(pBase); }
    HRESULT SwapBase(T* pBaseOld, T* pBaseNew) { return UnsafeSwapBase(pBaseOld, pBaseNew); }
    // Move the function definition of GetBase to rspriv.inl to work around gcc 2.9.5 warnings
    T* GetBase(ULONG_PTR id, BOOL fFab = TRUE);
    T* RemoveBase(ULONG_PTR id) { return static_cast<T*>(UnsafeRemoveBase(id)); }

    T* FindFirst(HASHFIND *find) { return static_cast<T*>(UnsafeFindFirst(find)); }
    T* FindNext(HASHFIND *find)  { return static_cast<T*>(UnsafeFindNext(find)); }
};

class CordbHashTableEnum : public CordbBase,
public ICorDebugProcessEnum,
public ICorDebugBreakpointEnum,
public ICorDebugStepperEnum,
public ICorDebugThreadEnum,
public ICorDebugModuleEnum,
public ICorDebugAppDomainEnum,
public ICorDebugAssemblyEnum
{
public:
    CordbHashTableEnum(CordbBase * pOwnerObj, NeuterList * pOwnerList, CordbHashTable *table,
                       const _GUID &id);
    CordbHashTableEnum(CordbHashTableEnum *cloneSrc);

    ~CordbHashTableEnum();
    virtual void Neuter(NeuterTicket ticket);


#ifdef _DEBUG
    // For debugging (asserts, logging, etc) provide a pretty name (this is 1:1 w/ the VTable)
    virtual const char * DbgGetName() { return "CordbHashTableEnum"; };
#endif


    HRESULT Next(ULONG celt, CordbBase *bases[], ULONG *pceltFetched);

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugEnum
    //-----------------------------------------------------------

    COM_METHOD Skip(ULONG celt);
    COM_METHOD Reset();
    COM_METHOD Clone(ICorDebugEnum **ppEnum);
    COM_METHOD GetCount(ULONG *pcelt);

    //-----------------------------------------------------------
    // ICorDebugProcessEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugProcess *processes[],
                    ULONG *pceltFetched)
    {
        VALIDATE_POINTER_TO_OBJECT_ARRAY(processes, ICorDebugProcess *,
            celt, true, true);
        VALIDATE_POINTER_TO_OBJECT(pceltFetched, ULONG *);

        return (Next(celt, (CordbBase **)processes, pceltFetched));
    }

    //-----------------------------------------------------------
    // ICorDebugBreakpointEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugBreakpoint *breakpoints[],
                    ULONG *pceltFetched)
    {
        VALIDATE_POINTER_TO_OBJECT_ARRAY(breakpoints, ICorDebugBreakpoint *,
            celt, true, true);
        VALIDATE_POINTER_TO_OBJECT(pceltFetched, ULONG *);

        return (Next(celt, (CordbBase **)breakpoints, pceltFetched));
    }

    //-----------------------------------------------------------
    // ICorDebugStepperEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugStepper *steppers[],
                    ULONG *pceltFetched)
    {
        VALIDATE_POINTER_TO_OBJECT_ARRAY(steppers, ICorDebugStepper *,
            celt, true, true);
        VALIDATE_POINTER_TO_OBJECT(pceltFetched, ULONG *);

        return (Next(celt, (CordbBase **)steppers, pceltFetched));
    }

    //-----------------------------------------------------------
    // ICorDebugThreadEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugThread *threads[],
                    ULONG *pceltFetched)
    {
        VALIDATE_POINTER_TO_OBJECT_ARRAY(threads, ICorDebugThread *,
            celt, true, true);
        VALIDATE_POINTER_TO_OBJECT(pceltFetched, ULONG *);

        return (Next(celt, (CordbBase **)threads, pceltFetched));
    }

    //-----------------------------------------------------------
    // ICorDebugModuleEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugModule *modules[],
                    ULONG *pceltFetched)
    {
        VALIDATE_POINTER_TO_OBJECT_ARRAY(modules, ICorDebugModule *,
            celt, true, true);
        VALIDATE_POINTER_TO_OBJECT(pceltFetched, ULONG *);

        return (Next(celt, (CordbBase **)modules, pceltFetched));
    }

    //-----------------------------------------------------------
    // ICorDebugAppDomainEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugAppDomain *appdomains[],
                    ULONG *pceltFetched)
    {
        VALIDATE_POINTER_TO_OBJECT_ARRAY(appdomains, ICorDebugAppDomain *,
            celt, true, true);
        VALIDATE_POINTER_TO_OBJECT(pceltFetched, ULONG *);

        return (Next(celt, (CordbBase **)appdomains, pceltFetched));
    }
    //-----------------------------------------------------------
    // ICorDebugAssemblyEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugAssembly *assemblies[],
                    ULONG *pceltFetched)
    {
        VALIDATE_POINTER_TO_OBJECT_ARRAY(assemblies, ICorDebugAssembly *,
            celt, true, true);
        VALIDATE_POINTER_TO_OBJECT(pceltFetched, ULONG *);

        return (Next(celt, (CordbBase **)assemblies, pceltFetched));
    }
private:
    // Owning object is our link to the CordbProcess* tree. Never null until we're neutered.
    // NeuterList is related to the owning object. Need to cache it so that we can pass it on
    // to our clones.
    CordbBase *     m_pOwnerObj; // provides us w/ a CordbProcess*
    NeuterList *    m_pOwnerNeuterList;


    CordbHashTable *m_table;
    bool            m_started;
    bool            m_done;
    HASHFIND        m_hashfind;
    REFIID          m_guid;
    ULONG           m_iCurElt;
    ULONG           m_count;
    BOOL            m_fCountInit;

#ifdef _DEBUG
    // timestampt of hashtable when we start enumerating it. Useful for detecting if the table
    // changes underneath us.
    int             m_DbgChangeCount;
    void AssertValid();
#else
    void AssertValid() { }
#endif

private:
    //These factor code between Next & Skip
    HRESULT PrepForEnum(CordbBase **pBase);

    // Note that the set of types advanced by Pre & by Post are disjoint, and
    // that the union of these two sets are all possible types enuerated by
    // the CordbHashTableEnum.
    HRESULT AdvancePreAssign(CordbBase **pBase);
    HRESULT AdvancePostAssign(CordbBase **pBase,
                              CordbBase     **b,
                              CordbBase   **bEnd);

    // This factors some code that initializes the module enumerator.
    HRESULT SetupModuleEnum(void);

};


//-----------------------------------------------------------------------------
// Must obtain a 'neuterticket' before neutering.
// The act of obtaining a ticket forces a caller to go through a validation
// layer before entering the neuter hierachy. Once in the hierarchy, the ticket
// can be passed from parent to child.
//-----------------------------------------------------------------------------
class NeuterTicket
{
public:
    NeuterTicket(CordbProcess *pProc);
    NeuterTicket(Cordb * pCordb) { }
};

//-----------------------------------------------------------------------------
// Neuter List
// Dtors can be called at any time (whenever Cordbg calls Release, which is outside
// of our control), so we never want to do significant work in a dtor
// (this includes sending IPC events + neutering).
// So objects can queue themselves up to be neutered at a safe time.
//-----------------------------------------------------------------------------
class NeuterList
{
public:
    NeuterList();
    ~NeuterList();

    // Add an object to be neutered.
    // Anybody calls this to add themselves to the list.
    // This will add it to the list and maintain an internal reference to it.
    void Add(CordbBase * pObject);

    // Add w/o checking for safety. Should only be used by Process-list enum.
    void UnsafeAdd(CordbBase * pObject);

    // Neuter everything on the list.
    // This should only be called by the "owner", but we can't really enforce that.
    // This will release all internal references and empty the list.
    void NeuterAndClear(NeuterTicket ticket);

    // Sweep for all objects that are marked as 'm_fNeuterAtWill'.
    // Neuter and remove these.
    void SweepAllNeuterAtWillObjects(NeuterTicket ticket);

protected:
    struct Node
    {
        RSSmartPtr<CordbBase> m_pObject;
        Node * m_pNext;
    };

    Node * m_pHead;

#ifdef _DEBUG
    int m_DbgCount;
#endif
};


/* ------------------------------------------------------------------------- *
 * Cordb class
 * ------------------------------------------------------------------------- */

class Cordb : public CordbBase, public ICorDebug
{
public:
    Cordb(CorDebugInterfaceVersion iDebuggerVersion);
    virtual ~Cordb();
    virtual void Neuter(NeuterTicket ticket);



#ifdef _DEBUG_IMPL
    virtual const char * DbgGetName() { return "Cordb"; }

    static LONG s_DbgMemTotalOutstandingCordb;
    static LONG s_DbgMemTotalOutstandingInternalRefs;
#endif

    //
    // Turn this on to enable an array which will contain all objects that have
    // not been completely released.
    //
    // #define TRACK_OUTSTANDING_OBJECTS 1

#ifdef TRACK_OUTSTANDING_OBJECTS

#define MAX_TRACKED_OUTSTANDING_OBJECTS 256
    static void *Cordb::s_DbgMemOutstandingObjects[MAX_TRACKED_OUTSTANDING_OBJECTS];
    static LONG Cordb::s_DbgMemOutstandingObjectMax;
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebug
    //-----------------------------------------------------------

    COM_METHOD Initialize();
    COM_METHOD Terminate();
    COM_METHOD SetManagedHandler(ICorDebugManagedCallback *pCallback);
    COM_METHOD SetUnmanagedHandler(ICorDebugUnmanagedCallback *pCallback);
    COM_METHOD CreateProcess(LPCWSTR lpApplicationName,
                             __in_z LPWSTR lpCommandLine,
                             LPSECURITY_ATTRIBUTES lpProcessAttributes,
                             LPSECURITY_ATTRIBUTES lpThreadAttributes,
                             BOOL bInheritHandles,
                             DWORD dwCreationFlags,
                             PVOID lpEnvironment,
                             LPCWSTR lpCurrentDirectory,
                             LPSTARTUPINFOW lpStartupInfo,
                             LPPROCESS_INFORMATION lpProcessInformation,
                             CorDebugCreateProcessFlags debuggingFlags,
                             ICorDebugProcess **ppProcess);
    COM_METHOD DebugActiveProcess(DWORD id, BOOL win32Attach, ICorDebugProcess **ppProcess);
    COM_METHOD EnumerateProcesses(ICorDebugProcessEnum **ppProcess);
    COM_METHOD GetProcess(DWORD dwProcessId, ICorDebugProcess **ppProcess);
    COM_METHOD CanLaunchOrAttach(DWORD dwProcessId, BOOL win32DebuggingEnabled);

    //-----------------------------------------------------------
    // CorDebug
    //-----------------------------------------------------------

    static COM_METHOD CreateObjectV1(REFIID id, void **object);
    static COM_METHOD CreateObject(CorDebugInterfaceVersion iDebuggerVersion, REFIID id, void **object);

    //-----------------------------------------------------------
    // Methods not exposed via a COM interface.
    //-----------------------------------------------------------

    bool AllowAnotherProcess();
    HRESULT AddProcess(CordbProcess* process);
    void RemoveProcess(CordbProcess* process);
    CordbSafeHashTable<CordbProcess> *GetProcessList();

    void LockProcessList(void);
    void UnlockProcessList(void);

    #ifdef _DEBUG
    bool ThreadHasProcessListLock();
    #endif


    HRESULT SendIPCEvent(CordbProcess* process,
                         DebuggerIPCEvent* event,
                         SIZE_T eventSize);
    void ProcessStateChanged(void);

    HRESULT WaitForIPCEventFromProcess(CordbProcess* process,
                                       CordbAppDomain *appDomain,
                                       DebuggerIPCEvent* event);

    // Gets the first event, used for in-proc stuff
    HRESULT GetFirstContinuationEvent(CordbProcess *process,
                                      DebuggerIPCEvent *event);

    HRESULT GetNextContinuationEvent(CordbProcess *process,
                                     DebuggerIPCEvent *event);


    HRESULT GetCorRuntimeHost(ICorRuntimeHost **ppHost);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    RSExtSmartPtr<ICorDebugManagedCallback>    m_managedCallback;
    RSExtSmartPtr<ICorDebugManagedCallback2>   m_managedCallback2;
    RSExtSmartPtr<ICorDebugUnmanagedCallback>  m_unmanagedCallback;

    RSExtSmartPtr<IMetaDataDispenser>          m_pMetaDispenser;

    CordbRCEventThread*         m_rcEventThread;

    ICorRuntimeHost            *m_pCorHost;

    CorDebugInterfaceVersion    GetDebuggerVersion() const;

private:
    bool IsInteropDebuggingSupported();

    CordbSafeHashTable<CordbProcess> m_processes;

    // List to track outstanding CordbProcessEnum objects.
    NeuterList                  m_pProcessEnumList;

    RSLock                      m_processListMutex;
    BOOL                        m_initialized;

    // This is the version of the ICorDebug APIs that the debugger believes it's consuming.
    CorDebugInterfaceVersion    m_debuggerSpecifiedVersion;
};




/* ------------------------------------------------------------------------- *
 * AppDomain class
 * ------------------------------------------------------------------------- */

class CordbAppDomain : public CordbBase, public ICorDebugAppDomain, public ICorDebugAppDomain2
{
public:
    CordbAppDomain(CordbProcess* pProcess,
                    LSPTR_APPDOMAIN pAppDomainToken,
                    ULONG id
                    );

    virtual ~CordbAppDomain();
    virtual void Neuter(NeuterTicket ticket);

    using CordbBase::GetProcess;

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbAppDomain"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugController
    //-----------------------------------------------------------

    COM_METHOD Stop(DWORD dwTimeout);
    COM_METHOD Deprecated_Continue(void);
    COM_METHOD Continue(BOOL fIsOutOfBand);
    COM_METHOD IsRunning(BOOL *pbRunning);
    COM_METHOD HasQueuedCallbacks(ICorDebugThread *pThread, BOOL *pbQueued);
    COM_METHOD EnumerateThreads(ICorDebugThreadEnum **ppThreads);
    COM_METHOD SetAllThreadsDebugState(CorDebugThreadState state,
                                       ICorDebugThread *pExceptThisThread);
    COM_METHOD Detach();
    COM_METHOD Terminate(unsigned int exitCode);

    COM_METHOD CanCommitChanges(
        ULONG cSnapshots,
        ICorDebugEditAndContinueSnapshot *pSnapshots[],
        ICorDebugErrorInfoEnum **pError);

    COM_METHOD CommitChanges(
        ULONG cSnapshots,
        ICorDebugEditAndContinueSnapshot *pSnapshots[],
        ICorDebugErrorInfoEnum **pError);

    //-----------------------------------------------------------
    // ICorDebugAppDomain
    //-----------------------------------------------------------
    /*
     * GetProcess returns the process containing the app domain
     */

    COM_METHOD GetProcess(ICorDebugProcess **ppProcess);

    /*
     * EnumerateAssemblies enumerates all assemblies in the app domain
     */

    COM_METHOD EnumerateAssemblies(ICorDebugAssemblyEnum **ppAssemblies);

    COM_METHOD GetModuleFromMetaDataInterface(IUnknown *pIMetaData,
                                              ICorDebugModule **ppModule);
    /*
     * EnumerateBreakpoints returns an enum of all active breakpoints
     * in the app domain.  This includes all types of breakpoints :
     * function breakpoints, data breakpoints, etc.
     */

    COM_METHOD EnumerateBreakpoints(ICorDebugBreakpointEnum **ppBreakpoints);

    /*
     * EnumerateSteppers returns an enum of all active steppers in the app domain.
     */

    COM_METHOD EnumerateSteppers(ICorDebugStepperEnum **ppSteppers);
    /*
     * IsAttached returns whether or not the debugger is attached to the
     * app domain.  The controller methods on the app domain cannot be used
     * until the debugger attaches to the app domain.
     */

    COM_METHOD IsAttached(BOOL *pbAttached);

    /*
     * GetName returns the name of the app domain.
     * Note:   This method is not yet implemented.
     */

    COM_METHOD GetName(ULONG32 cchName,
                      ULONG32 *pcchName,
                      WCHAR szName[]);

    /*
     * GetObject returns the runtime app domain object.
     * Note:   This method is not yet implemented.
     */

    COM_METHOD GetObject(ICorDebugValue **ppObject);
    COM_METHOD Attach (void);
    COM_METHOD GetID (ULONG32 *pId);

    //-----------------------------------------------------------
    // ICorDebugAppDomain2 APIs
    //-----------------------------------------------------------
    COM_METHOD GetArrayOrPointerType(CorElementType elementType,
                                     ULONG32 nRank,
                                     ICorDebugType *pTypeArg,
                                     ICorDebugType **ppType);

    COM_METHOD GetFunctionPointerType(ULONG32 nTypeArgs,
                                      ICorDebugType *ppTypeArgs[],
                                      ICorDebugType **ppType);

    LSPTR_APPDOMAIN GetADToken() { return m_pLSAppDomain; };

    CordbModule *GetAnyModule(void);
    CordbModule *LookupModule(LSPTR_DMODULE debuggerModuleToken);

    void InvalidateName() { m_nameIsValid = false; }
    const WCHAR * GetFriendlyName();

    HRESULT DetachWorker(void);

public:

    BOOL                m_fAttached;
    BOOL                m_fHasAtLeastOneThreadInsideIt; // So if we detach, we'll know
                                    // if we should eliminate the CordbAppDomain upon
                                    // thread_detach, or appdomain_exit.

public:
    ULONG               m_AppDomainId;

    CordbSafeHashTable<CordbAssembly>    m_assemblies;
    CordbSafeHashTable<CordbModule>      m_modules;
    CordbSafeHashTable<CordbBreakpoint>  m_breakpoints;

    CordbSafeHashTable<CordbType>        m_sharedtypes;   // Unique objects that represent the use of some
                                         // basic ELEMENT_TYPE's as type parameters.  These
                                         // are shared acrosss the entire process.  We could
                                         // go and try to find the classes corresponding to these
                                         // element types but it seems simpler just to keep
                                         // them as special cases.

private:
    // Use DAC to refresh our name
    void RefreshName();

    WCHAR               *m_szAppDomainName;
    bool                m_nameIsValid;
    static const WCHAR  *s_szBadAppDomainName;

    bool                m_synchronizedAD; // to be used later

    LSPTR_APPDOMAIN     m_pLSAppDomain;

    NeuterList          m_TypeNeuterList;  // List of types owned by this AppDomain.
    NeuterList          m_SweepableNeuterList;  // List of Sweepable objects owned by this AppDomain.

public:
    // The "Long" exit list is for items that don't get neuter until the appdomain exits.
    // The "Sweepable" exit list is for items that may be neuterable sooner than AD exit.
    // By splitting out the list, we can just try to sweep the "Sweepable" list and we
    // don't waste any time sweeping things on the "Long" list that aren't neuterable anyways.
    NeuterList * GetLongExitNeuterList() { return &m_TypeNeuterList; }
    NeuterList * GetSweepableExitNeuterList() { return &m_SweepableNeuterList; }

    void AddToTypeList(CordbBase *pObject);

};


/* ------------------------------------------------------------------------- *
 * Assembly class
 * ------------------------------------------------------------------------- */

class CordbAssembly : public CordbBase, public ICorDebugAssembly, ICorDebugAssembly2
{
public:
    CordbAssembly(CordbAppDomain* pAppDomain,
                    LSPTR_ASSEMBLY debuggerAssemblyToken,
                    const WCHAR *szName,
                    BOOL fIsSystemAssembly);
    virtual ~CordbAssembly();
    virtual void Neuter(NeuterTicket ticket);

    using CordbBase::GetProcess;

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbAssembly"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugAssembly
    //-----------------------------------------------------------

    /*
     * GetProcess returns the process containing the assembly
     */
    COM_METHOD GetProcess(ICorDebugProcess **ppProcess);

    /*
     * GetAppDomain returns the app domain containing the assembly.
     * Returns null if this is the system assembly
     */
    COM_METHOD GetAppDomain(ICorDebugAppDomain **ppAppDomain);

    /*
     * EnumerateModules enumerates all modules in the assembly
     */
    COM_METHOD EnumerateModules(ICorDebugModuleEnum **ppModules);

    /*
     * GetCodeBase returns the code base used to load the assembly
     */
    COM_METHOD GetCodeBase(ULONG32 cchName,
                        ULONG32 *pcchName,
                        WCHAR szName[]);

    /*
     * GetName returns the name of the assembly
     */
    COM_METHOD GetName(ULONG32 cchName,
                      ULONG32 *pcchName,
                      WCHAR szName[]);


    //-----------------------------------------------------------
    // ICorDebugAssembly2
    //-----------------------------------------------------------

    /*
     * IsFullyTrusted returns a flag indicating whether the security system 
     * has granted the assembly full trust.
     */
    COM_METHOD IsFullyTrusted( BOOL *pbFullyTrusted );

    //-----------------------------------------------------------
    // internal accessors
    //-----------------------------------------------------------

    CordbAppDomain *GetAppDomain()     { return m_pAppDomain; }
    const WCHAR* GetName()             { return m_szAssemblyName; }
    BOOL IsSystemAssembly()            { return m_fIsSystemAssembly;}

private:
    LSPTR_ASSEMBLY      m_debuggerAssemblyToken;
    CordbAppDomain      *m_pAppDomain;

    StringCopyHolder    m_szAssemblyName;
    BOOL                m_fIsSystemAssembly;
    BOOL                m_fIsTrustLevelKnown;
    BOOL                m_fIsFullTrust;             // valid only if above is true

};


//-----------------------------------------------------------------------------
// Describe what to do w/ a win32 debug event
//-----------------------------------------------------------------------------
class Reaction
{
public:
    enum Type
    {
        // Inband events: Dispatch to Cordbg
        // safe for stopping the shell and communicating with the runtime
        cInband,

        cInband_NotNewEvent,

        // Oob events: Dispatch to Cordbg
        // Not safe stopping events. They must be continued immediately.
        cOOB,

        // CLR internal exception, Continue(not_handled), don't dispatch
        // The CLR expects this exception and will deal with it properly.
        cCLR,

        // Don't dispatch. Continue(DBG_CONTINUE).
        // Common for flare.
        cIgnore,
    };

    Type GetType() const { return m_type; };

#ifdef _DEBUG
    const char * GetReactionName()
    {
        switch(m_type)
        {
            case cInband: return "cInband";
            case cInband_NotNewEvent: return "cInband_NotNewEvent";
            case cOOB: return "cOOB";
            case cCLR: return "cCLR";
            case cIgnore: return "cIgnore";
            default: return "<unknown>";
        }
    }
    int GetLine()
    {
        return m_line;
    }
#endif

    Reaction(Type t, int line) : m_type(t) {
#ifdef _DEBUG
        m_line = line;

        LOG((LF_CORDB, LL_EVERYTHING, "Reaction:%s (determined on line: %d)\n", GetReactionName(), line));
#endif
    };

protected:
    const Type m_type;

#ifdef _DEBUG
    // Under a debug build, track the line # for where this came from.
    int m_line;
#endif
};

// Macro for creating a Reaction.
#define REACTION(type) Reaction(Reaction::type, __LINE__)

// Different forms of Unmanaged Continue
enum EUMContinueType
{
    cOobUMContinue,
    cInternalUMContinue,
    cRealUMContinue
};

/* ------------------------------------------------------------------------- *
 * Process class
 * ------------------------------------------------------------------------- */
#ifdef _DEBUG
// On debug, we can afford a larger native event queue..
const int DEBUG_EVENTQUEUE_SIZE = 30;
#else
const int DEBUG_EVENTQUEUE_SIZE = 10;
#endif

void DeleteIPCEventHelper(DebuggerIPCEvent *pDel);

class CordbProcess : public CordbBase, public ICorDebugProcess, public ICorDebugProcess2
{
public:
    CordbProcess(Cordb* cordb, CordbWin32EventThread * pW32, DWORD processID, HANDLE handle);
    virtual ~CordbProcess();
    virtual void Neuter(NeuterTicket ticket);

    // Neuter all of all children, but not the actual process object.
    void NeuterChildren(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbProcess"; }
#endif

    //-----------------------------------------------------------
    // Creation
    //-----------------------------------------------------------

    static HRESULT CreateProcess(
          Cordb * pCordb,
          LPCWSTR programName,
          __in_z LPWSTR  programArgs,
          LPSECURITY_ATTRIBUTES lpProcessAttributes,
          LPSECURITY_ATTRIBUTES lpThreadAttributes,
          BOOL bInheritHandles,
          DWORD dwCreationFlags,
          PVOID lpEnvironment,
          LPCWSTR lpCurrentDirectory,
          LPSTARTUPINFOW lpStartupInfo,
          LPPROCESS_INFORMATION lpProcessInformation,
          CorDebugCreateProcessFlags corDebugFlags
    );

    static HRESULT DebugActiveProcess(
        Cordb * pCordb,
        DWORD pid,
        BOOL win32Attach

    );


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return BaseAddRefEnforceExternal();
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return BaseReleaseEnforceExternal();
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugController
    //-----------------------------------------------------------

    COM_METHOD Stop(DWORD dwTimeout);
    COM_METHOD Deprecated_Continue(void);
    COM_METHOD IsRunning(BOOL *pbRunning);
    COM_METHOD HasQueuedCallbacks(ICorDebugThread *pThread, BOOL *pbQueued);
    COM_METHOD EnumerateThreads(ICorDebugThreadEnum **ppThreads);
    COM_METHOD SetAllThreadsDebugState(CorDebugThreadState state,
                                       ICorDebugThread *pExceptThisThread);
    COM_METHOD Detach();
    COM_METHOD Terminate(unsigned int exitCode);

    COM_METHOD CanCommitChanges(
        ULONG cSnapshots,
        ICorDebugEditAndContinueSnapshot *pSnapshots[],
        ICorDebugErrorInfoEnum **pError);

    COM_METHOD CommitChanges(
        ULONG cSnapshots,
        ICorDebugEditAndContinueSnapshot *pSnapshots[],
        ICorDebugErrorInfoEnum **pError);

    COM_METHOD Continue(BOOL fIsOutOfBand);
    COM_METHOD ThreadForFiberCookie(DWORD fiberCookie,
                                    ICorDebugThread **ppThread);
    COM_METHOD GetHelperThreadID(DWORD *pThreadID);

    //-----------------------------------------------------------
    // ICorDebugProcess
    //-----------------------------------------------------------

    COM_METHOD GetID(DWORD *pdwProcessId);
    COM_METHOD GetHandle(HANDLE *phProcessHandle);
    COM_METHOD EnableSynchronization(BOOL bEnableSynchronization);
    COM_METHOD GetThread(DWORD dwThreadId, ICorDebugThread **ppThread);
    COM_METHOD EnumerateBreakpoints(ICorDebugBreakpointEnum **ppBreakpoints);
    COM_METHOD EnumerateSteppers(ICorDebugStepperEnum **ppSteppers);
    COM_METHOD EnumerateObjects(ICorDebugObjectEnum **ppObjects);
    COM_METHOD IsTransitionStub(CORDB_ADDRESS address, BOOL *pbTransitionStub);
    COM_METHOD EnumerateModules(ICorDebugModuleEnum **ppModules);
    COM_METHOD GetModuleFromMetaDataInterface(IUnknown *pIMetaData,
                                              ICorDebugModule **ppModule);
    COM_METHOD SetStopState(DWORD threadID, CorDebugThreadState state);
    COM_METHOD IsOSSuspended(DWORD threadID, BOOL *pbSuspended);
    COM_METHOD GetThreadContext(DWORD threadID, ULONG32 contextSize,
                                BYTE context[]);
    COM_METHOD SetThreadContext(DWORD threadID, ULONG32 contextSize,
                                BYTE context[]);
    COM_METHOD ReadMemory(CORDB_ADDRESS address, DWORD size, BYTE buffer[],
                          SIZE_T *read);
    COM_METHOD WriteMemory(CORDB_ADDRESS address, DWORD size, BYTE buffer[],
                           SIZE_T *written);

    COM_METHOD ClearCurrentException(DWORD threadID);

    /*
     * EnableLogMessages enables/disables sending of log messages to the
     * debugger for logging.
     */
    COM_METHOD EnableLogMessages(BOOL fOnOff);

    /*
     * ModifyLogSwitch modifies the specified switch's severity level.
     */
    COM_METHOD ModifyLogSwitch(__in_z WCHAR *pLogSwitchName, LONG lLevel);

    COM_METHOD EnumerateAppDomains(ICorDebugAppDomainEnum **ppAppDomains);
    COM_METHOD GetObject(ICorDebugValue **ppObject);

    //-----------------------------------------------------------
    // ICorDebugProcess2
    //-----------------------------------------------------------

    COM_METHOD GetThreadForTaskID(TASKID taskid, ICorDebugThread2 **ppThread);
    COM_METHOD GetVersion(COR_VERSION* pInfo);

    COM_METHOD SetUnmanagedBreakpoint(CORDB_ADDRESS address, ULONG32 bufsize, BYTE buffer[], ULONG32 * bufLen);
    COM_METHOD ClearUnmanagedBreakpoint(CORDB_ADDRESS address);
    COM_METHOD GetCodeAtAddress(CORDB_ADDRESS address, ICorDebugCode ** pCode, ULONG32 * offset);

    COM_METHOD SetDesiredNGENCompilerFlags(DWORD pdwFlags);
    COM_METHOD GetDesiredNGENCompilerFlags(DWORD *pdwFlags );

    COM_METHOD GetReferenceValueFromGCHandle(UINT_PTR handle, ICorDebugReferenceValue **pOutValue);


    //-----------------------------------------------------------
    // Methods not exposed via a COM interface.
    //-----------------------------------------------------------

    HRESULT ContinueInternal(BOOL fIsOutOfBand);
    HRESULT StopInternal(DWORD dwTimeout, LSPTR_APPDOMAIN pAppDomainToken);

    /*
     * This will request a buffer of size cbBuffer to be allocated
     * on the left side.
     *
     * If successful, returns S_OK.  If unsuccessful, returns E_OUTOFMEMORY.
     */
    HRESULT GetRemoteBuffer(CordbAppDomain *pDomain, ULONG cbBuffer, void **ppBuffer);

    // Same as above except also copy-in the contents of a RS buffer using WriteProcessMemory
    HRESULT GetAndWriteRemoteBuffer(CordbAppDomain *pDomain, unsigned int bufferSize, void *bufferFrom, void **ppBuffer);

    /*
     * This will release a previously allocated left side buffer.
     * Often they are deallocated by the LS itself.
     */
    HRESULT ReleaseRemoteBuffer(void **ppBuffer);

    HRESULT Init(bool win32Attached);
    void DeleteQueuedEvents();
    void CleanupHalfBakedLeftSide(void);
    void Terminating(BOOL fDetach);
    void HandleManagedCreateThread(DWORD dwThreadId, HANDLE hThread);

    HRESULT QueueManagedAttach();

    // Helper to get PID internally.
    DWORD GetPid();

    HRESULT GetRuntimeOffsets(void);
    bool IsSpecialStackOverflowCase(CordbUnmanagedThread *pUThread, DEBUG_EVENT *pEvent);

    // Are we blocked waiting fo ran OOB event to be continue?
    bool IsWaitingForOOBEvent()
    {
        // If no interop, then we're never waiting for an OOB event.
        return false;
    }


    void DispatchRCEvent(void);
    void MarkAllThreadsDirty(void);

    bool CheckIfLSExited();

    void Lock(void)
    {
        // Lock Hierarchy - shouldn't have List lock when taking/release the process lock.

        m_processMutex.Lock();
        LOG((LF_CORDB, LL_EVERYTHING, "P::Lock enter, this=0x%p\n", this));
    }

    void Unlock(void)
    {
        // Lock Hierarchy - shouldn't have List lock when taking/releasing the process lock.

        LOG((LF_CORDB, LL_EVERYTHING, "P::Lock leave, this=0x%p\n", this));
        m_processMutex.Unlock();
    }

#ifdef _DEBUG
    bool ThreadHoldsProcessLock(void)
    {
        return m_processMutex.HasLock();
    }
#endif

    // Expose the stop-go lock b/c varios Cordb objects in our process tree may need to take it.
    RSLock * GetStopGoLock()
    {
        return &m_StopGoLock;
    }


    void UnrecoverableError(HRESULT errorHR,
                            unsigned int errorCode,
                            const char *errorFile,
                            unsigned int errorLine);
    HRESULT CheckForUnrecoverableError(void);
    HRESULT VerifyControlBlock(void);

    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------

    // Is it safe to send events to the LS?
    bool IsSafeToSendEvents() { return !m_unrecoverableError && !m_terminated && !m_detached; }

    bool IsWin32EventThread();


    // Send a truly asynchronous IPC event.
    void SendAsyncIPCEvent(DebuggerIPCEventType t);

    HRESULT SendIPCEvent(DebuggerIPCEvent *event, SIZE_T eventSize)
    {
        _ASSERTE(m_cordb != NULL);
        return (m_cordb->SendIPCEvent(this, event, eventSize));
    }

    void InitAsyncIPCEvent(DebuggerIPCEvent *ipce,
                      DebuggerIPCEventType type,
                      LSPTR_APPDOMAIN appDomainToken)
    {
        // Async events only allowed for the following:
        _ASSERTE(type == DB_IPCE_ATTACHING);

        InitIPCEvent(ipce, type, false, appDomainToken);
        ipce->asyncSend = true;
    }

    void InitIPCEvent(DebuggerIPCEvent *ipce,
                      DebuggerIPCEventType type,
                      bool twoWay,
                      LSPTR_APPDOMAIN appDomainToken
                      )
    {
        // zero out the event in case we try and use any uninitialized fields
        memset( ipce, 0, sizeof(DebuggerIPCEvent) );
        
        _ASSERTE((appDomainToken != NULL) ||
                 type == DB_IPCE_GET_GCHANDLE_INFO ||
                 type == DB_IPCE_ENABLE_LOG_MESSAGES ||
                 type == DB_IPCE_MODIFY_LOGSWITCH ||
                 type == DB_IPCE_ASYNC_BREAK ||
                 type == DB_IPCE_CONTINUE ||
                 type == DB_IPCE_GET_BUFFER ||
                 type == DB_IPCE_RELEASE_BUFFER ||
                 type == DB_IPCE_ATTACH_TO_APP_DOMAIN ||
                 type == DB_IPCE_IS_TRANSITION_STUB ||
                 type == DB_IPCE_ATTACHING ||
                 type == DB_IPCE_APPLY_CHANGES ||
                 type == DB_IPCE_CONTROL_C_EVENT_RESULT ||
                 type == DB_IPCE_SET_REFERENCE ||
                 type == DB_IPCE_SET_DEBUG_STATE ||
                 type == DB_IPCE_SET_ALL_DEBUG_STATE ||
                 type == DB_IPCE_GET_THREAD_FOR_TASKID ||
                 type == DB_IPCE_GET_OSTHREADID ||
                 type == DB_IPCE_GET_TASKID ||
                 type == DB_IPCE_GET_CONNECTIONID ||
                 type == DB_IPCE_DETACH_FROM_PROCESS ||
                 type == DB_IPCE_INTERCEPT_EXCEPTION ||
                 type == DB_IPCE_GET_THREADHANDLE ||
                 type == DB_IPCE_GET_NGEN_COMPILER_FLAGS ||
                 type == DB_IPCE_SET_NGEN_COMPILER_FLAGS ||
                 type == DB_IPCE_GET_CURRENT_APPDOMAIN ||
                 type == DB_IPCE_GET_THREAD_OBJECT);

        ipce->type = type;
        ipce->hr = S_OK;
        ipce->processId = 0;
        ipce->appDomainToken = appDomainToken;
        ipce->threadId = 0;
        ipce->replyRequired = twoWay;
        ipce->asyncSend = false;
        ipce->next = NULL;
    }

    void ClearContinuationEvents(void)
    {
        DebuggerIPCEvent *event = (DebuggerIPCEvent *)m_DCB->m_sendBuffer;

        while (event->next != NULL)
        {
            LOG((LF_CORDB,LL_INFO1000, "About to CCE 0x%x\n",event));

            DebuggerIPCEvent *pDel = event->next;
            event->next = pDel->next;
            delete pDel;
        }
    }

    CordbModule *LookupModule( LSPTR_DMODULE debuggerModuleToken );
        
    CordbUnmanagedThread *GetUnmanagedThread(DWORD dwThreadId)
    {
        return m_unmanagedThreads.GetBase(dwThreadId);
    }

    /*
     * This will cleanup the patch table, releasing memory,etc.
     */
    void ClearPatchTable(void);

    /*
     * This will grab the patch table from the left side & go through
     * it to gather info needed for faster access.  If address,size,buffer
     * are passed in, while going through the table we'll undo patches
     * in buffer at the same time
     */
    HRESULT RefreshPatchTable(CORDB_ADDRESS address = NULL, SIZE_T size = NULL, BYTE buffer[] = NULL);

    // Find if a patch exists at a given address.
    HRESULT FindPatchByAddress(CORDB_ADDRESS address, bool *patchFound, bool *patchIsUnmanaged);

    enum AB_MODE
    {
        AB_READ,
        AB_WRITE
    };

    /*
     * Once we've called RefreshPatchTable to get the patch table,
     * this routine will iterate through the patches & either apply
     * or unapply the patches to buffer. AB_READ => Replaces patches
     * in buffer with the original opcode, AB_WRTE => replace opcode
     * with breakpoint instruction, caller is responsible for
     * updating the patchtable back to the left side.
     *
     *                                                     
     */
    HRESULT AdjustBuffer(CORDB_ADDRESS address,
                         SIZE_T size,
                         BYTE buffer[],
                         BYTE **bufferCopy,
                         AB_MODE mode,
                         BOOL *pbUpdatePatchTable = NULL);

    /*
     * AdjustBuffer, above, doesn't actually update the local patch table
     * if asked to do a write.  It stores the changes alongside the table,
     * and this will cause the changes to be written to the table (for
     * a range of left-side addresses
     */
    void CommitBufferAdjustments(CORDB_ADDRESS start,
                                 CORDB_ADDRESS end);

    /*
     * Clear the stored changes, or they'll sit there until we
     * accidentally commit them
     */
    void ClearBufferAdjustments(void);
    HRESULT Attach (ULONG AppDomainId);

    //-----------------------------------------------------------
    // Accessors for key synchronization fields.
    //-----------------------------------------------------------

    // If CAD is NULL, returns true if all appdomains (ie, the entire process)
    // is synchronized.  Otherwise, returns true if the specified appdomain is
    // synch'd.
    bool GetSynchronized(void);
    void SetSynchronized(bool fSynch);

    void IncStopCount();
    void DecStopCount();

    // Gets the exact stop count. You need the Proecss lock for this.
    int GetStopCount();

    // Just gets whether we're stopped or not (m_stopped > 0).
    // You only need the StopGo lock for this.
    // This is biases towards returning false.
    bool IsStopped();

    bool GetSyncCompleteRecv();
    void SetSyncCompleteRecv(bool fSyncRecv);


    // Cordbg may not always continue during a callback; but we really shouldn't do meaningful
    // work after a callback has returned yet before they've called continue. Thus we may need
    // to remember some state at the time of dispatch so that we do stuff at continue.
    // Only example here is neutering... we'd like to Neuter an object X after the ExitX callback,
    // but we can't neuter it until Continue. So remember X when we dispatch, and neuter this at continue.
    // Use a smart ptr to keep it alive until we neuter it.

    // Add objects to various neuter lists.
    // NeuterOnContinue is for all objects that can be neutered once we continue.
    // NeuterOnExit is for all objects that can survive continues (but are neutered on process shutdown).
    // If an object's external ref count goes to 0, it gets promoted to the NeuterOnContinue list.
    void AddToNeuterOnExitList(CordbBase *pObject);
    void AddToNeuterOnContinueList(CordbBase *pObject);

    NeuterList * GetContinueNeuterList() { return &m_ContinueNeuterList; }
    NeuterList * GetExitNeuterList() { return &m_ExitNeuterList; }

    // Routines to read and write thread context records between the processes safely.
    HRESULT SafeReadThreadContext(LSPTR_CONTEXT pRemoteContext, CONTEXT *pCtx);
    HRESULT SafeWriteThreadContext(LSPTR_CONTEXT pRemoteContext, CONTEXT *pCtx);


    // Record a win32 event for debugging purposes.
    void DebugRecordWin32Event(const DEBUG_EVENT * pEvent, CordbUnmanagedThread * pUThread);

    //-----------------------------------------------------------
    // Interop Helpers
    //-----------------------------------------------------------

    // Get the DAC interface.
    IXCLRDataProcess * GetDACAndFlush();
    void ForceDacFlush();

    void ResumeHijackedThreads();

    void HandleWin32DebugEvent(DEBUG_EVENT & event);

    CordbUnmanagedThread *HandleUnmanagedCreateThread(DWORD dwThreadId, HANDLE hThread, void *lpThreadLocalBase);
    HRESULT SuspendUnmanagedThreads();
    HRESULT ResumeUnmanagedThreads(bool unmarkHijacks);
    HRESULT StartSyncFromWin32Stop(BOOL *asyncBreakSent);

    // For interop attach, we first do native, and then once Cordbg continues from
    // the loader-bp, we kick off the managed attach. This field remembers that
    // whether we need the managed attach.
    bool m_fDoDelayedManagedAttached;


    // Keep a count of hijacked threads that haven't yet signaled a flare.
    // This is used for delaying a sync complete.
    DWORD m_cHijackedThreads;

    // Table of CordbEval objects that we've sent over to the LS.
    // This is synced via the StopGoLock.
    RsPtrTable<CordbEval> m_EvalTable;
private:

    
    void ProcessFirstLogMessage (DebuggerIPCEvent *event);
    void ProcessContinuedLogMessage (DebuggerIPCEvent *event);

    void CloseIPCHandles(void);
    void UpdateThreadsForAdUnload( CordbAppDomain* pAppDomain );

    // Each win32 debug event needs to be triaged to get a Reaction.
    Reaction TriageBreakpoint(CordbUnmanagedThread * ut, DEBUG_EVENT & event);
    Reaction TriageSyncComplete(CordbUnmanagedThread * ut, DEBUG_EVENT & event);
    Reaction Triage1stChanceHijackFlare(CordbUnmanagedThread * ut, DEBUG_EVENT & event);
    Reaction Triage1stChanceNonSpecial(CordbUnmanagedThread * ut, DEBUG_EVENT & event);
    Reaction TriageExcep1stChanceAndInit(CordbUnmanagedThread * pUT, DEBUG_EVENT & event);
    Reaction TriageExcep2ndChangeAndInit(CordbUnmanagedThread * pUT, DEBUG_EVENT & event);
    Reaction TriageWin32DebugEvent(CordbUnmanagedThread * pUT, DEBUG_EVENT & event);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    RSSmartPtr<Cordb>     m_cordb;
    HANDLE                m_handle;

    bool                  m_attached;
    bool                  m_detached;
    bool                  m_uninitializedStop;
    bool                  m_creating;

    // m_exiting is true if we know the LS is starting to exit (if the
    // RS is telling the LS to exit) or if we know the LS has already exited.
    bool                  m_exiting;

    bool                  m_firstExceptionHandled;

    // m_terminated can only be set to true if we know 100% the LS has exited (ie, somebody
    // waited on the LS process handle).
    bool                  m_terminated;

    bool                  m_unrecoverableError;

    bool                  m_sendAttachIPCEvent;


    bool                  m_firstManagedEvent;
    bool                  m_specialDeferment;
    bool                  m_helperThreadDead; // flag used for interop
    bool                  m_loaderBPReceived;

private:
    //
    // Count of the number of outstanding CordbEvals in the process.
    //
    LONG                  m_cOutstandingEvals;

public:
    LONG OutstandingEvalCount(void)
    {
        return m_cOutstandingEvals;
    }

    void IncrementOutstandingEvalCount(void)
    {
        InterlockedIncrement(&m_cOutstandingEvals);
    }

    void DecrementOutstandingEvalCount(void)
    {
        InterlockedDecrement(&m_cOutstandingEvals);
    }

    //
    // Is it OK to detach at this time
    //
    HRESULT IsReadyForDetach(void);

private:
    // List of things that get neutered on process exit and Continue respectively.
    NeuterList            m_ExitNeuterList;
    NeuterList            m_ContinueNeuterList;

    // m_stopCount, m_synchronized, & m_syncCompleteReceived are key fields describing
    // the processes' sync status.
    DWORD                 m_stopCount;

    // m_synchronized is the Debugger's view of SyncStatus. It will go high & low for each
    // callback. Continue() will set this to false.
    // This flag is true roughly from the time that we've dispatched a managed callback
    // until the time that it's continued.
    bool                  m_synchronized;

    // m_syncCompleteReceived tells us if the runtime is _actually_ sychronized. It goes
    // high once we get a SyncComplete, and it goes low once we actually send the continue.
    // This is always set by the thread that receives the sync-complete. In interop, that's the w32et.
    // Thus this is the most accurate indication of wether the Debuggee is _actually_ synchronized or not.
    bool                  m_syncCompleteReceived;




    // Each CordbProcess has its own win32 event thread.
    CordbWin32EventThread*      m_win32EventThread;

    static HRESULT CreateAndStartWin32ET(Cordb * pCordb, CordbWin32EventThread ** ppw32);

public:
    bool                  m_oddSync;

    CordbSafeHashTable<CordbThread>           m_userThreads;
    CordbSafeHashTable<CordbUnmanagedThread>  m_unmanagedThreads;
    CordbSafeHashTable<CordbAppDomain>        m_appDomains;

    // Since a stepper can begin in one appdomain, and complete in another,
    // we put the hashtable here, rather than on specific appdomains.
    CordbSafeHashTable<CordbStepper>          m_steppers;

    //  Used to figure out if we have to refresh any reference objects
    //  on the left side.  Gets incremented each time a continue is called, or
    //  global debugee state is modified in some other way.
    UINT                  m_continueCounter;

    DebuggerIPCControlBlock *m_DCB;
    DebuggerIPCRuntimeOffsets m_runtimeOffsets;
    HANDLE                m_leftSideEventAvailable;
    HANDLE                m_leftSideEventRead;
    HANDLE                m_rightSideEventAvailable;
    HANDLE                m_rightSideEventRead;
    HANDLE                m_leftSideUnmanagedWaitEvent;
    HANDLE                m_syncThreadIsLockFree;
    HANDLE                m_SetupSyncEvent;

    IPCReaderInterface    m_IPCReader;

    // This becomes true when the RS receives its first managed event.
    bool                  m_initialized;

    DebuggerIPCEvent*     m_queuedEventList;
    DebuggerIPCEvent*     m_lastQueuedEvent;

#ifdef _DEBUG
    DebuggerIPCEventType  m_pDBGLastIPCEventType;
#endif

    bool                  m_stopRequested;
    HANDLE                m_stopWaitEvent;
    RSLock                m_processMutex;

    CordbUnmanagedEvent  *m_lastIBStoppingEvent;
    bool                  m_dispatchingUnmanagedEvent;
    bool                  m_dispatchingOOBEvent;
    bool                  m_doRealContinueAfterOOBBlock;
    bool                  m_deferContinueDueToOwnershipWait;
    DWORD                 m_helperThreadId; // helper thread ID calculated from sniffing from UM thread-create events.

    // Is the given thread id a helper thread (real or worker?)
    bool IsHelperThreadWorked(DWORD tid);

    enum
    {
        PS_WIN32_STOPPED           = 0x0001,
        PS_HIJACKS_IN_PLACE        = 0x0002,
        PS_SOME_THREADS_SUSPENDED  = 0x0004,
        PS_WIN32_ATTACHED          = 0x0008,
        PS_SYNC_RECEIVED           = 0x0010,
        PS_WIN32_OUTOFBAND_STOPPED = 0x0020,
    };

    unsigned int          m_state;

    //
    // We cache the LS patch table on the RS.
    //

    // The array of entries. (The patchtable is a hash implemented as a single-array)
    // This array includes empty entries.
    // There is an auxillary bucket structure used to map hash codes to array indices.
    // We traverse the array, and we recognize an empty slot
    // if DebuggerControllerPatch::opcode == 0.
    // If we haven't gotten the table, then m_pPatchTable is NULL
    BYTE*                 m_pPatchTable;

    // The number of entries (both used & unused) in m_pPatchTable.
    UINT                  m_cPatch;

    // so we know where to write the changes patchtable back to
    // This has m_cPatch elements.
    BYTE                 *m_rgData;

    // Cached value of iNext entries such that:
    //      m_rgNextPatch[i] = ((DebuggerControllerPatch*)m_pPatchTable)[i]->iNext;
    //      where 0 <= i < m_cPatch
    // This provides a linked list (via indices) to traverse the used entries of m_pPatchTable.
    // This has m_cPatch elements.
    ULONG               *m_rgNextPatch;

    // This has m_cPatch elements.
    PRD_TYPE             *m_rgUncommitedOpcode;

    // CORDB_ADDRESS's are UINT_PTR's (64 bit under _WIN64, 32 bit otherwise)
#define MAX_ADDRESS     (ULONG_MAX)
#define MIN_ADDRESS     (0x0)
    CORDB_ADDRESS       m_minPatchAddr; //smallest patch in table
    CORDB_ADDRESS       m_maxPatchAddr;

#define DPT_TERMINATING_INDEX (UINT32_MAX)
    // Index into m_pPatchTable of the first patch (first used entry).
    ULONG                  m_iFirstPatch;



    bool SupportsVersion(CorDebugInterfaceVersion featureVersion);

    void StartEventDispatch(DebuggerIPCEventType event);
    void FinishEventDispatch();
    bool AreDispatchingEvent();
    DebuggerIPCEventType GetDispatchedEvent();

    HANDLE GetHelperThreadHandle() { return m_hHelperThread; }

    CordbAppDomain* GetDefaultAppDomain() { return m_pDefaultAppDomain; }

    // Lookup if there's a native BP at the given address. Return NULL not found.
    NativePatch * GetNativePatch(const void * pAddress);
    bool  IsBreakOpcodeAtAddress(const void * address);

private:
    // handle to helper thread. Used for managed debugging.
    // Initialized only after we get the tid from the DCB.
    HANDLE m_hHelperThread;

    DebuggerIPCEventType  m_dispatchedEvent;   // what event are we currently dispatching?

    RSLock            m_StopGoLock;

    // Each process has exactly one Default AppDomain
    CordbAppDomain*     m_pDefaultAppDomain;    // owned by m_appDomains

    // Helpers
    CordbUnmanagedThread * GetUnmanagedThreadFromEvent(DEBUG_EVENT & event);


    // // The full-blown debug event is too large, so we just remember the important stuff.
    struct MiniDebugEvent
    {
        BYTE code; // event code from the debug event
        CordbUnmanagedThread * pUThread;
        union
        {
            struct {
                void * pAddress; // address of an exception
                DWORD dwCode;
            } ExceptionData;
            struct {
                void * pBaseAddress; // for module load & unload
            } ModuleData;
        } u;
    };

    // Group fields that are just used for debug support here.
    // Some are included even in retail builds to help debug retail failures.
    struct DebugSupport
    {
        // For debugging, we keep a rolling queue of the last N Win32 debug events.
        MiniDebugEvent        m_DebugEventQueue[DEBUG_EVENTQUEUE_SIZE];
        int                   m_DebugEventQueueIdx;
        int                   m_TotalNativeEvents;

        // Breakdown of different types of native events
        int                   m_TotalIB;
        int                   m_TotalOOB;
        int                   m_TotalCLR;
    } m_DbgSupport;

    COR_VERSION                            m_leftSideVersion;

    CUnorderedArray<NativePatch, 10> m_NativePatchList;


    // DAC
    HRESULT InitializeDac();
    void FreeDac();

};

// Some IMDArocess APIs are supported as interop-only.
#define FAIL_IF_MANAGED_ONLY(pProcess) \
{ CordbProcess * __Proc = pProcess; if ((__Proc->m_state & PS_WIN32_ATTACHED) == 0) return CORDBG_E_MUST_BE_INTEROP_DEBUGGING; }


/* ------------------------------------------------------------------------- *
 * Module class
 * ------------------------------------------------------------------------- */

class CordbModule : public CordbBase, public ICorDebugModule, public ICorDebugModule2
{
public:
    CordbModule(CordbProcess *process, CordbAssembly *pAssembly,
                LSPTR_DMODULE debuggerModuleToken, REMOTE_PTR PEBaseAddress,
                ULONG nPESize, BOOL fDynamic, BOOL fInMemory,
                const WCHAR *szName,
                const WCHAR * szNgenName,
                CordbAppDomain *pAppDomain);

    virtual ~CordbModule();
    virtual void Neuter(NeuterTicket ticket);

    using CordbBase::GetProcess;

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbModule"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugModule
    //-----------------------------------------------------------

    COM_METHOD GetProcess(ICorDebugProcess **ppProcess);
    COM_METHOD GetBaseAddress(CORDB_ADDRESS *pAddress);
    COM_METHOD GetAssembly(ICorDebugAssembly **ppAssembly);
    COM_METHOD GetName(ULONG32 cchName, ULONG32 *pcchName, WCHAR szName[]);
    COM_METHOD EnableJITDebugging(BOOL bTrackJITInfo, BOOL bAllowJitOpts);
    COM_METHOD EnableClassLoadCallbacks(BOOL bClassLoadCallbacks);
    COM_METHOD GetFunctionFromToken(mdMethodDef methodDef,
                                    ICorDebugFunction **ppFunction);
    COM_METHOD GetFunctionFromRVA(CORDB_ADDRESS rva, ICorDebugFunction **ppFunction);
    COM_METHOD GetClassFromToken(mdTypeDef typeDef,
                                 ICorDebugClass **ppClass);
    COM_METHOD CreateBreakpoint(ICorDebugModuleBreakpoint **ppBreakpoint);
    /*
     * Edit & Continue support.  GetEditAndContinueSnapshot produces a
     * snapshot of the running process.  This snapshot can then be fed
     * into the compiler to guarantee the same token values are
     * returned by the meta data during compile, to find the address
     * where new static data should go, etc.  These changes are
     * comitted using ICorDebugProcess.
     */

    COM_METHOD GetEditAndContinueSnapshot(
        ICorDebugEditAndContinueSnapshot **ppEditAndContinueSnapshot);

    COM_METHOD GetMetaDataInterface(REFIID riid, IUnknown **ppObj);
    COM_METHOD GetToken(mdModule *pToken);
    COM_METHOD IsDynamic(BOOL *pDynamic);
    COM_METHOD GetGlobalVariableValue(mdFieldDef fieldDef,
                                   ICorDebugValue **ppValue);
    COM_METHOD GetSize(ULONG32 *pcBytes);
    COM_METHOD IsInMemory(BOOL *pInMemory);

    //-----------------------------------------------------------
    // ICorDebugModule2
    //-----------------------------------------------------------
    COM_METHOD SetJMCStatus(
        BOOL fIsUserCode,
        ULONG32 cOthers,
        mdToken others[]);

    COM_METHOD ApplyChanges(
        ULONG cbMetadata,
        BYTE pbMetadata[],
        ULONG cbIL,
        BYTE pbIL[]);

    // Resolve an assembly given an AssemblyRef token. Note that
    // this will not trigger the loading of assembly. If assembly is not yet loaded,
    // this will return an CORDBG_E_CANNOT_RESOLVE_ASSEMBLY error
    //
    COM_METHOD ResolveAssembly(mdToken tkAssemblyRef,
                                   ICorDebugAssembly **ppAssembly);

    COM_METHOD SetJITCompilerFlags(
       DWORD dwFlags );

    COM_METHOD GetJITCompilerFlags(
       DWORD *pdwFlags );

    //-----------------------------------------------------------
    // Internal members
    //-----------------------------------------------------------

    HRESULT Init( REMOTE_PTR pMetadataStart, ULONG nMetadataSize );
    HRESULT UpdateMetadataFromRemote( REMOTE_PTR pRemoteMetadataPtr, DWORD dwMetadataSize );

    BOOL IsDynamic();
    CordbFunction* LookupFunction(mdMethodDef methodToken);
    HRESULT LookupOrCreateFunction(mdMethodDef token, SIZE_T funcRVA, SIZE_T enCVersion, CordbFunction **ppRes);

    HRESULT CreateFunction(mdMethodDef token,
                           SIZE_T functionRVA,
                           SIZE_T enCVersion,
                           CordbFunction** ppFunction);
    HRESULT UpdateFunction(mdMethodDef token,
                           SIZE_T newEnCVersion,
                           CordbFunction** ppFunction);
    CordbClass* LookupClass(mdTypeDef classToken);
    HRESULT LookupOrCreateClass(mdTypeDef classToken, CordbClass** ppClass);
    HRESULT CreateClass(mdTypeDef classToken, CordbClass** ppClass);
    HRESULT LookupClassByToken(mdTypeDef token, CordbClass **ppClass);
    HRESULT ResolveTypeRef(mdTypeRef token, CordbClass **ppClass);
    HRESULT ResolveTypeRefOrDef(mdToken token, CordbClass **ppClass);

    HRESULT ApplyChangesInternal(
        ULONG cbMetaData,
        BYTE pbMetaData[],
        ULONG cbIL,
        BYTE pbIL[]);

private:
    HRESULT CopyRemoteMetadata( REMOTE_PTR pRemoteMetadataPtr, DWORD dwMetadataSize, PVOID* ppLocalMetadataPtr);
    HRESULT ResolveAssemblyInternal(mdToken tkAssemblyRef, CordbAssembly **ppAssembly);

    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------

public:
    CordbAppDomain *GetAppDomain()
    {
        return m_pAppDomain;
    }

    CordbAssembly *GetCordbAssembly (void);

    const WCHAR *GetModuleName(void)
    {
        return m_szModuleName;
    }

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    CordbAssembly*   m_pAssembly;
    CordbAppDomain*  m_pAppDomain;
    CordbSafeHashTable<CordbClass>    m_classes;
    CordbSafeHashTable<CordbFunction> m_functions;
    LSPTR_DMODULE    m_debuggerModuleToken;

    RSExtSmartPtr<IMetaDataImport> m_pIMImport;

    DWORD            m_EnCCount;

private:
    DWORD            m_dwProcessId;

    // Base Address and size of this module in debuggee's process. Maybe null if unknown.
    REMOTE_PTR       m_PEBaseAddress;
    ULONG            m_nPESize;

    BOOL             m_fDynamic; // Dynamic modules can grow (like Reflection Emit)
    BOOL             m_fInMemory; // In memory modules don't have file-backing.

    // Module's fullname.  (We own the memory). NULL if no name (in-memory module?)
    StringCopyHolder m_szModuleName;

    // Name of the ngen file. NULL if not ngenned.
    StringCopyHolder m_szNgenName;

    // "Global" class for this module. Global functions + vars exist in this class.
    RSSmartPtr<CordbClass> m_pClass;
};


//-----------------------------------------------------------------------------
// Cordb MDA notification
//-----------------------------------------------------------------------------
class CordbMDA : public CordbBase, public ICorDebugMDA
{
public:
    CordbMDA(CordbProcess * pProc, DebuggerMDANotification * pData);
    ~CordbMDA();

    virtual void Neuter(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbMDA"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRefEnforceExternal());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseReleaseEnforceExternal());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugMDA
    //-----------------------------------------------------------

    // Get the string for the type of the MDA. Never empty.
    // This is a convenient performant alternative to getting the XML stream and extracting
    // the type from that based off the schema.
    COM_METHOD GetName(ULONG32 cchName, ULONG32 * pcchName, WCHAR szName[]);

    // Get a string description of the MDA. This may be empty (0-length).
    COM_METHOD GetDescription(ULONG32 cchName, ULONG32 * pcchName, WCHAR szName[]);

    // Get the full associated XML for the MDA. This may be empty.
    // This could be a potentially expensive operation if the xml stream is large.
    // See the MDA documentation for the schema for this XML stream.
    COM_METHOD GetXML(ULONG32 cchName, ULONG32 * pcchName, WCHAR szName[]);

    COM_METHOD GetFlags(CorDebugMDAFlags * pFlags);

    // Thread that the MDA is fired on. We use the os tid instead of an ICDThread in case an MDA is fired on a
    // native thread (or a managed thread that hasn't yet entered managed code and so we don't have a ICDThread
    // object for it yet)
    COM_METHOD GetOSThreadId(DWORD * pOsTid);

private:
    NewArrayHolder<WCHAR> m_szName;
    NewArrayHolder<WCHAR> m_szDescription;
    NewArrayHolder<WCHAR> m_szXml;

    DWORD m_dwOSTID;
    CorDebugMDAFlags m_flags;
};



struct CordbSyncBlockField
{
    FREEHASHENTRY   entry;
    DebuggerIPCE_FieldData data;
};

// DebuggerIPCE_FieldData.fldMetadataToken is the key
class CordbSyncBlockFieldTable : public CHashTableAndData<CNewDataNoThrow>
{
  private:

    BOOL Cmp(const BYTE *pc1, const HASHENTRY *pc2)
    {LEAF_CONTRACT;  return (ULONG)(UINT_PTR)(pc1) !=
    ((CordbSyncBlockField*)pc2)->data.fldMetadataToken; }

    ULONG HASH(mdFieldDef fldToken)
    {LEAF_CONTRACT;  return fldToken; }

    BYTE *KEY(mdFieldDef fldToken)
    { return (BYTE *)(ULONG_PTR)fldToken; }

  public:

#ifndef DACCESS_COMPILE

    CordbSyncBlockFieldTable() : CHashTableAndData<CNewDataNoThrow>(11)
    {
        NewInit(11, sizeof(CordbSyncBlockField), 11);
    }

    void AddFieldInfo(DebuggerIPCE_FieldData *pInfo)
    {
        _ASSERTE(pInfo != NULL);

        CordbSyncBlockField *pEntry = (CordbSyncBlockField *)Add(HASH(pInfo->fldMetadataToken));
        pEntry->data = *pInfo; // copy everything over
    }

    void RemoveFieldInfo(mdFieldDef fldToken)
    {
        CordbSyncBlockField *entry = (CordbSyncBlockField*)Find(HASH(fldToken), KEY(fldToken));
        _ASSERTE(entry != NULL);
        Delete(HASH(fldToken), (HASHENTRY*)entry);
   }

#endif // #ifndef DACCESS_COMPILE

    DebuggerIPCE_FieldData *GetFieldInfo(mdFieldDef fldToken)
    {
        CordbSyncBlockField *entry = (CordbSyncBlockField*)Find(HASH(fldToken), KEY(fldToken));
        return (entry!=NULL?&(entry->data):NULL);
    }
};


/* ------------------------------------------------------------------------- *
 * Instantiation.
 *
 * This struct stores a set of type parameters.  It is used in
 * the heap-allocated data structures CordbType and  CordbJITInfo.
 *
 *   CordbType::m_inst.    Stores the class type parameters if any,
 *                         or the solitary array type parameter, or the solitary parameter
 *                         to a byref type.
 *
 *   CordbJITILFrame::m_genericArgs.  Stores exact generic parameters for the generic method frame if available
 *                                 Need not be identicial if code is shared between generic instantiations.
 *                                 May be inexact if real instantiation has been optimized away off
 *                                 the frame (nb this gets reported by the left side)
 * ------------------------------------------------------------------------- */

class Instantiation
{
public:
    Instantiation()
        : m_cInst(0), m_ppInst(NULL), m_cClassTyPars (0)
    { }

    Instantiation(unsigned int _cClassInst, CordbType **_ppClassInst)
        : m_cInst(_cClassInst), m_ppInst(_ppClassInst), m_cClassTyPars(_cClassInst)
    {LEAF_CONTRACT;  }

    Instantiation(unsigned int _cInst, CordbType **_ppInst, unsigned int numClassTyPars)
        : m_cInst(_cInst), m_ppInst(_ppInst),
        m_cClassTyPars (numClassTyPars)
    { }

    Instantiation(const Instantiation &inst)
        : m_cInst(inst.m_cInst), m_ppInst(inst.m_ppInst), m_cClassTyPars (inst.m_cClassTyPars)
    { }

    unsigned int m_cInst;
    CordbType **m_ppInst;
    unsigned int m_cClassTyPars;
};

//------------------------------------------------------------------------
// CordbType: replaces the use of signatures.
//
// Invariants on CordbType
// ---------------------------
//
//   The m_elementType is NEVER ELEMENT_TYPE_VAR or ELEMENT_TYPE_MVAR or ELEMENT_TYPE_GENERICINST
//   CordbTypes are always _ground_ types, and if they represent an instantiated type
//   like List<int> then m_inst will be non-empty.
//
//   !!!! The m_elementType is NEVER ELEMENT_TYPE_VALUETYPE !!!!
//   !!!! To find out if it is a value type call CordbType::IsValueType() !!!!
//
// Where CordbTypes are stored
// ---------------------------
//
// We use an tree-like scheme to hash-cons types.
//   - CordbTypes are created for "partially instantiated" types,
//     e.g. CordbTypes exist for "Dict" and "Dict<int>" even if the real
//     type being manipulated by the user is "Dict<int,string>"
//   - Subordinate types (E.g. Dict<int,sting> is subordinate to Dict<int>,
//     which is itself subordinate to the type for Dict) get stored
//     in the m_spinetypes hash table of the parent type.
//   - In m_spinetypes the pointers of the CordbType's themselves
//     are used for the unique ids for entries in the table.
//
// Thus the representation for  "Dict<class String,class Foo, class Foo* >" goes as follows:
//    1. Assume the type Foo is represented by CordbClass *5678x
//    1b. Assume the hashtable m_sharedtypes in the AppDomain maps E_T_STRING to the CordbType *0ABCx
//       Assume m_type in class Foo (i.e. CordbClass *5678x) is the CordbType *0DEFx
//       Assume m_type in class Foo maps E_T_PTR to the CordbType *0647x
//    2. The hash table m_spinetypes in "Dict" maps "0ABCx" to a new CordbType
//       representing Dict<String> (a single type application)
//    3. The hash table m_spinetypes in this new CordbType maps "0DEFx" to a
//        new CordbType representing Dict<class String,class Foo>
//    3. The hash table m_spinetypes in this new CordbType maps "0647" to a
//        new CordbType representing Dict<class String,class Foo, class Foo*>
//
// This is lets us reuse the existing hash table scheme to build
// up instantiated types of arbitrary size.
//
// Array types are similar, excpet that they start with a head type
// for the "type constructor", e.g. "_ []" is a type constructor with rank 1
// and m_elementType = ELEMENT_TYPE_SZARRAY.  These head constructors are
// stored in the m_sharedtypes table in the appdomain.  The actual instantiations
// of the array types are then subordinate types to the array constructor type.
//
// Other types are simpler, and have unique objects stored in the m_sharedtypes
// table in the appdomain.  This table is indexed by CORDBTYPE_ID in RsType.cpp
//
//
// Memory Management of CordbTypes
// ---------------------------
// All CordbTypes are ultimately stored off the CordbAppDomain object.
// The most common place is in the AppDomain's neuter-list.
//

class CordbType : public CordbBase, public ICorDebugType
{
public:
    CordbType(CordbAppDomain *appdomain, CorElementType ty, unsigned int rank);
    CordbType(CordbAppDomain *appdomain, CorElementType ty, CordbClass *c);
    CordbType(CordbType *tycon, CordbType *tyarg);
    virtual ~CordbType();
    virtual void Neuter(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbType"; }
#endif

    // If you want to force the init to happen even if we think the class
    // is up to date, set fForceInit to TRUE
    HRESULT Init(BOOL fForceInit);

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugType
    //-----------------------------------------------------------

    COM_METHOD GetType(CorElementType *ty);
    COM_METHOD GetClass(ICorDebugClass **ppClass);
    COM_METHOD EnumerateTypeParameters(ICorDebugTypeEnum **ppTyParEnum);
    COM_METHOD GetFirstTypeParameter(ICorDebugType **ppType);
    COM_METHOD GetBase(ICorDebugType **ppType);
    COM_METHOD GetStaticFieldValue(mdFieldDef fieldDef,
                                   ICorDebugFrame *pFrame,
                                   ICorDebugValue **ppValue);
    COM_METHOD GetRank(ULONG32 *pnRank);

    //-----------------------------------------------------------
    // Non-COM members
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // Basic constructor operations for the algebra of types.
    // These all create unique objects within an AppDomain.
    //-----------------------------------------------------------

    // This one is used to create simple types, e.g. int32, int64, typedbyref etc.
    static HRESULT MkType(CordbAppDomain *appdomain, CorElementType et, CordbType **ppType);

    // This one is used to create array, pointer and byref types
    static HRESULT MkType(CordbAppDomain *appdomain, CorElementType et, ULONG rank, CordbType *arg, CordbType **ppType);

    // This one is used to create function pointer types.  et must be ELEMENT_TYPE_FNPTR
    static HRESULT MkType(CordbAppDomain *appdomain, CorElementType et, const Instantiation &inst, CordbType **ppType);

    // This one is used to class and value class types, e.g. "class MyClass" or "class ArrayList<int>"
    static HRESULT MkType(CordbAppDomain *appdomain, CorElementType et, CordbClass *cl, const Instantiation &inst, CordbType **ppType);

    // Some derived constructors...  Use this one if the type is definitely not
    // a paramterized type, e.g. to implement functions on the API where types cannot
    // be parameterized.
    static HRESULT MkUnparameterizedType(CordbAppDomain *appdomain, CorElementType et, CordbClass *cl, CordbType **ppType);

    //-----------------------------------------------------------
    // Basic destructor operations over the algebra
    //-----------------------------------------------------------
    CorElementType GetElementType() { return m_elementType; }
    void DestUnaryType(CordbType **pRes) ;
    void DestConstructedType(CordbClass **pClass, Instantiation *pInst);
    void DestNaryType(Instantiation *pInst);

    // If this is a ptr type, get the CordbType that it points to.
    // Eg, for CordbType("Int*"), returns CordbType("Int").
    // If not a ptr type, returns null.
    // Since it's all internal, no reference counting.
    // This is effectively a specialized version of DestUnaryType.
    CordbType * GetPointerElementType();


    // Create a type from metadata
    static HRESULT SigToType(CordbModule *module, SigParser *pSigParser, const Instantiation &inst, CordbType **pRes);

    // Create a type from from the data received from the left-side
    static HRESULT TypeDataToType(CordbAppDomain *appdomain, DebuggerIPCE_ExpandedTypeData *data, CordbType **pRes);
    static HRESULT TypeDataToType(CordbAppDomain *appdomain, DebuggerIPCE_BasicTypeData *data, CordbType **pRes);
    static HRESULT InstantiateFromTypeHandle(CordbAppDomain *appdomain, LSPTR_TYPEHANDLE typeHandle, CorElementType et, CordbClass *tycon, CordbType **pRes);

    // Prepare data to send back to left-side during Init() and FuncEval.  Fail if the the exact
    // type data is requested but was not fetched correctly during Init()
    HRESULT TypeToBasicTypeData(DebuggerIPCE_BasicTypeData *data);
    void TypeToExpandedTypeData(DebuggerIPCE_ExpandedTypeData *data);
    void TypeToTypeArgData(DebuggerIPCE_TypeArgData *data);
    static void CountTypeDataNodes(CordbType *type, unsigned int *count);
    static void CountTypeDataNodesForInstantiation(unsigned int genericArgsCount, ICorDebugType *genericArgs[], unsigned int *count);
    static void GatherTypeData(CordbType *type, DebuggerIPCE_TypeArgData **curr_tyargData);
    static void GatherTypeDataForInstantiation(unsigned int genericArgsCount, ICorDebugType *genericArgs[], DebuggerIPCE_TypeArgData **curr_tyargData);

    HRESULT GetParentType(CordbClass *baseClass, CordbType **ppRes);

    // These are available after Init() has been called....
    HRESULT GetUnboxedObjectSize(ULONG32 *res);
    HRESULT GetFieldInfo(mdFieldDef fldToken, DebuggerIPCE_FieldData **ppFieldData);

    CordbAppDomain *GetAppDomain() { return m_appdomain; }

    HRESULT IsValueType(bool *isValueClass);

    // Is this type a GC-root.
    bool IsGCRoot();

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    CorElementType m_elementType;
    CordbAppDomain *m_appdomain; // Valid for all E_T
    CordbClass *m_class; // Initially only set for E_T_CLASS, computed for E_T_STRING and E_T_OBJECT if needed
    ULONG m_rank; // Only set for E_T_ARRAY etc.

    Instantiation           m_inst;
    CordbSafeHashTable<CordbType>         m_spinetypes; // A unique mapping from CordbType objects that are
                                          // type parameters to CordbType objects.  Each mapping
                                          // represents the use of the containing type as
                                          // type constructor.  e.g. If the containing type
                                          // is CordbType(CordbClass "List") then the table here
                                          // will map parameters such as (CordbType(CordbClass "String")) to
                                          // the constructed type CordbType(CordbClass "List", <CordbType(CordbClass "String")>)

    // Valid after Init(), only for E_T_ARRAY etc.and E_T_CLASS when m_class->m_typarCount > 0.
    // m_typeHandleExact is the precise type handle for this type.
    LSPTR_TYPEHANDLE m_typeHandleExact;

    // Valid after Init(), only for E_T_CLASS, and when m_class->m_typarCount > 0.  May not be set correctly if m_fieldInfoNeedsInit.
    SIZE_T                  m_objectSize;

    // DON'T KEEP POINTERS TO ELEMENTS OF m_fields AROUND!!
    // This may be deleted if the class gets EnC'd.
    //
    // Valid after Init(), only for E_T_CLASS, and when m_class->m_typarCount > 0
    // All fields will be valid if we have m_typeHandleExact.
    // Only some fields will be valid if we have called Init() but still have m_fieldInfoNeedsInit.
    DebuggerIPCE_FieldData *m_fields;

private:
    static HRESULT MkTyAppType(CordbAppDomain *appdomain, CordbType *tycon, const Instantiation &inst, CordbType **pRes);
    BOOL                    m_fieldInfoNeedsInit;

private:
    HRESULT InitInstantiationTypeHandle(BOOL fForceInit);
    HRESULT InitInstantiationFieldInfo(BOOL fForceInit);
    HRESULT InitStringOrObjectClass(BOOL fForceInit);
};

/* ------------------------------------------------------------------------- *
 * Class class
 * ------------------------------------------------------------------------- */

class CordbClass : public CordbBase, public ICorDebugClass, public ICorDebugClass2
{
public:
    CordbClass(CordbModule* m, mdTypeDef token);
    virtual ~CordbClass();
    virtual void Neuter(NeuterTicket ticket);

    using CordbBase::GetProcess;

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbClass"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugClass
    //-----------------------------------------------------------

    COM_METHOD GetStaticFieldValue(mdFieldDef fieldDef,
                                   ICorDebugFrame *pFrame,
                                   ICorDebugValue **ppValue);
    COM_METHOD GetModule(ICorDebugModule **pModule);
    COM_METHOD GetToken(mdTypeDef *pTypeDef);
    COM_METHOD GetParameterizedType(CorElementType et,ULONG32 cTypeArgs, ICorDebugType *ppTypeArgs[], ICorDebugType **ppType);

    //-----------------------------------------------------------
    // ICorDebugClass2
    //-----------------------------------------------------------
    COM_METHOD SetJMCStatus(BOOL fIsUserCode);

    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------

    CordbModule *GetModule()
    {
        return m_module;
    }

    CordbAppDomain *GetAppDomain()
    {
        return m_module->GetAppDomain();
    }

    //-----------------------------------------------------------
    // Functionality shared for CordbType and CordbClass
    //-----------------------------------------------------------

    static HRESULT SearchFieldInfo(CordbModule *module,
                                        unsigned int cData,
                                        DebuggerIPCE_FieldData *data,
                                        mdTypeDef classToken,
                                        mdFieldDef fldToken,
                                        DebuggerIPCE_FieldData **ppFieldData);

    static HRESULT GetStaticFieldValue2(CordbModule *pModule,
                                        DebuggerIPCE_FieldData *pFieldData,
                                        BOOL fSyncBlockField,
                                        const Instantiation &inst,
                                        ICorDebugFrame *pFrame,
                                        ICorDebugValue **ppValue);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    HRESULT GetSyncBlockField(mdFieldDef fldToken,
                                     DebuggerIPCE_FieldData **ppFieldData,
                                     CordbObjectValue *object);

    HRESULT GetFieldInfo(mdFieldDef fldToken, DebuggerIPCE_FieldData **ppFieldData);

    // If you want to force the init to happen even if we think the class
    // is up to date, set fForceInit to TRUE
    HRESULT Init(BOOL fForceInit);
    HRESULT IsValueClass(bool *pIsValueClass);
    HRESULT GetThisType(const Instantiation &inst, CordbType **res);
    static HRESULT PostProcessUnavailableHRESULT(HRESULT hr,
                               IMetaDataImport *pImport,
                               mdFieldDef fieldDef);
    mdTypeDef GetTypeDef() { return (mdTypeDef)m_id; }


    // when we get an added field or method, mark the class as old to force re-init when we access it
    BOOL NeedsInit()
    {
        return m_needsInit;
    }

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

private:
    BOOL                    m_needsInit;

public:
    BOOL                    m_loadEventSent;
    bool                    m_hasBeenUnloaded;

    // [m_type] is the type object for when this class is used
    // as a type.  If the class is a value class then it can represent
    // either the boxed or unboxed type - it depends on the context where the
    // type is used.  For example on a CordbBoxValue it represents the type of the
    // boxed VC, on a CordbVCObjectValue it represents the type of the unboxed VC.
    //
    // The type field starts of NULL as there
    // is no need to create the type object until it is needed.
    RSSmartPtr<CordbType>   m_type;
    CordbModule*            m_module;
    mdTypeDef               m_token;  // the token for the type constructor - m_id cannot be used for constructed types
    unsigned int            m_typarCount; // 0 if the type is non-generic

    // Whether the class is a VC or not is discovered either by
    // seeing the class used in a signature after ELEMENT_TYPE_VALUETYPE
    // or ELEMENT_TYPE_CLASS or by going and asking the EE.
    bool                    m_isValueClass;
    bool                    m_isValueClassKnown;

    UINT                    m_continueCounterLastSync;
    unsigned int            m_varCount; // number of elements in m_fields array


    // DON'T KEEP POINTERS TO ELEMENTS OF m_fields AROUND!!
    // This may be deleted if the class gets EnC'd.
    DebuggerIPCE_FieldData *m_fields;

    // if we do an EnC after this class is loaded (in the debuggee), then the new fields will be hung off the sync
    // block, thus available on a per-instance basis.
    CordbSyncBlockFieldTable m_syncBlockFieldsStatic;

    SIZE_T                  m_objectSize;  // Note: this is NOT valid for constructed value types,
                                           // e.g. value type Pair<DateTime,int>.  Use CordbType::m_objectSize instead.
};


/* ------------------------------------------------------------------------- *
 * TypeParameter enumerator class
 * ------------------------------------------------------------------------- */

class CordbTypeEnum : public CordbBase, public ICorDebugTypeEnum
{
public:
    // Factory method: Create a new instance of this class.  Returns NULL on out-of-memory.
    // On success, returns a new initialized instance of CordbTypeEnum with ref-count 0 (just like a ctor).
    static CordbTypeEnum* Build(CordbAppDomain * pAppDomain, unsigned int cTypars, CordbType **ppTypars);
    static CordbTypeEnum* Build(CordbAppDomain * pAppDomain, unsigned int cTypars, RSSmartPtr<CordbType>*ppTypars);

    virtual ~CordbTypeEnum() ;

    virtual void Neuter(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbTypeEnum"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugEnum
    //-----------------------------------------------------------

    COM_METHOD Skip(ULONG celt);
    COM_METHOD Reset(void);
    COM_METHOD Clone(ICorDebugEnum **ppEnum);
    COM_METHOD GetCount(ULONG *pcelt);

    //-----------------------------------------------------------
    // ICorDebugTypeEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugType *Types[], ULONG *pceltFetched);

private:
    // Private constructor, only partially initializes the object.
    // Clients should use the 'Build' factory method to create an instance of this class.
    CordbTypeEnum(CordbAppDomain * pAppDomain );
    template<class T> static CordbTypeEnum* BuildImpl(CordbAppDomain * pAppDomain, unsigned int cTypars, T* ppTypars );

    // Owning object.
    CordbAppDomain * m_pAppDomain;

    RSSmartPtr<CordbType> * m_ppTypars;
    UINT   m_iCurrent;
    UINT   m_iMax;
};

/* ------------------------------------------------------------------------- *
 * Code enumerator class
 * ------------------------------------------------------------------------- */

class CordbCodeEnum : public CordbBase, public ICorDebugCodeEnum
{
public:
    CordbCodeEnum(unsigned int cCode, RSSmartPtr<CordbCode> * ppCode);
    virtual ~CordbCodeEnum() ;


#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbCodeEnum"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugEnum
    //-----------------------------------------------------------

    COM_METHOD Skip(ULONG celt);
    COM_METHOD Reset(void);
    COM_METHOD Clone(ICorDebugEnum **ppEnum);
    COM_METHOD GetCount(ULONG *pcelt);

    //-----------------------------------------------------------
    // ICorDebugCodeEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugCode *Codes[], ULONG *pceltFetched);

private:
    // Ptr to an array of CordbCode*
    // We own the array.
    RSSmartPtr<CordbCode> * m_ppCodes;
    UINT   m_iCurrent;
    UINT   m_iMax;
};





typedef CUnorderedArray<CordbCode*,11> UnorderedCodeArray;
const int DMI_VERSION_INVALID = 0;
const int DMI_VERSION_MOST_RECENTLY_JITTED = 1;
const int DMI_VERSION_MOST_RECENTLY_EnCED = 2;


/* ------------------------------------------------------------------------- *
 * Function class
 *
 * @review dsyme.  The CordbFunction class now keeps a multiple MethodDescInfo
 * structures in a hash table indexed by tokens provided by the left-side.
 * In 99.9% of cases this hash table will only contain one entry - we only
 * use a hashtable to cover the case where we have multiple JITtings of
 * a single version of a function, in particular multiple JITtings of generic
 * code under different instantiations. This will increase space usage.
 * The way around it is to store one CordbJITInfo in-line in the CordbFunction
 * class, or at least store one such pointer so no hash table will normally
 * be needed.  This is similar to other cases, e.g. the hash table in
 * CordbClass used to indicate different CordbTypes made from that class -
 * again in the normal case these tables will only contain one element.
 *
 * However, for the moment I've focused on correctness and we can minimize
 * this space usage in due course.
 * ------------------------------------------------------------------------- */

const BOOL bNativeCode = FALSE;
const BOOL bILCode = TRUE;

//
// Each E&C version gets its own function object. So the IL that a function
// is associated w/ does not change.
// B/C of generics, a single IL function may get jitted multiple times and
// be associated w/ multiple native code blobs (CordbJitInfos).
//
class CordbFunction : public CordbBase, public ICorDebugFunction, public ICorDebugFunction2
{
public:
    //-----------------------------------------------------------
    // Create from scope and member objects.
    //-----------------------------------------------------------
    CordbFunction(CordbModule *m,
                  mdMethodDef token,
                  SIZE_T functionRVA,
                  SIZE_T enCVersion);
    virtual ~CordbFunction();
    virtual void Neuter(NeuterTicket ticket);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbFunction"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugFunction
    //-----------------------------------------------------------
    COM_METHOD GetModule(ICorDebugModule **pModule);
    COM_METHOD GetClass(ICorDebugClass **ppClass);
    COM_METHOD GetToken(mdMethodDef *pMemberDef);
    COM_METHOD GetILCode(ICorDebugCode **ppCode);
    COM_METHOD GetNativeCode(ICorDebugCode **ppCode);
    COM_METHOD CreateBreakpoint(ICorDebugFunctionBreakpoint **ppBreakpoint);
    COM_METHOD GetLocalVarSigToken(mdSignature *pmdSig);
    COM_METHOD GetCurrentVersionNumber(ULONG32 *pnCurrentVersion);

    //-----------------------------------------------------------
    // ICorDebugFunction2
    //-----------------------------------------------------------
    COM_METHOD SetJMCStatus(BOOL fIsUserCode);
    COM_METHOD GetJMCStatus(BOOL * pfIsUserCode);
    COM_METHOD EnumerateNativeCode(ICorDebugCodeEnum **ppCodeEnum) { return E_NOTIMPL; }
    COM_METHOD GetVersionNumber(ULONG32 *pnCurrentVersion);

    //-----------------------------------------------------------
    // Internal members
    //-----------------------------------------------------------
    static HRESULT LookupOrCreateFromFuncData(CordbProcess *pProcess, CordbAppDomain *pAppDomain,
                                              DebuggerIPCE_FuncData *data, SIZE_T enCVersion, CordbFunction **ppRes);

protected:
    HRESULT Populate();

public:

    void SetLocalVarToken(mdSignature  localVarSigToken);


    HRESULT GetLocalVarSig(SigParser *pLocalsSigParser,
                           ULONG *pLocalVarCount);
    HRESULT GetSig(SigParser *pMethodSigParser,
                   ULONG *pFunctionArgCount,
                   BOOL *pFunctionIsStatic);

    HRESULT GetArgumentType(DWORD dwIndex, const Instantiation &inst, CordbType **pType);
    HRESULT GetLocalVariableType(DWORD dwIndex, const Instantiation &inst, CordbType **type);


    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------

    CordbAppDomain *GetAppDomain()
    {
        return (m_module->GetAppDomain());
    }

    CordbModule *GetModule()
    {
        return m_module;
    }

    //-----------------------------------------------------------
    // Internal routines
    //-----------------------------------------------------------

    // Get the existing IL code object
    HRESULT GetCode(CordbCode **ppCode);

    // If the IL code object exists, then return it,
    // else create it w/ the info from the params.
    HRESULT GetOrCreateILCode(CordbCode **ppCode,
                               REMOTE_PTR startAddress, SIZE_T size,
                               SIZE_T nVersion);


    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:

    CordbModule             *m_module;
    CordbClass              *m_class;

    // We only have 1 IL blob associated with a given Function object.
    RSSmartPtr<CordbCode>    m_ILCode;


    // Generics allow a single IL method to be instantiated to multiple native
    // code blobs. So CordbFunction : CordbJitInfo is 1:n.
    CordbSafeHashTable<CordbJITInfo> m_jitinfos;

    mdMethodDef              m_token;
    SIZE_T                   m_functionRVA;

    SIZE_T                   m_dwEnCVersionNumber; // EnC version number of this instance
    RSSmartPtr<CordbFunction> m_pPrevVersion; // link to previous version of this function

    BOOL                     m_isNativeImpl; // Is the function implemented natively in the runtime??

private:
    BOOL                     m_fCachedMethodValuesValid;
    SigParser                m_methodSigParserCached;
    ULONG                    m_argCountCached;
    BOOL                     m_isStaticCached;

    BOOL                     m_fCachedLocalValuesValid;
    mdSignature              m_localVarSigToken;
    SigParser                m_localSigParserCached;
    ULONG                    m_localVarCachedCount;
};

/* ------------------------------------------------------------------------- *
 * CordbJITInfo class. These correspond to MethodDesc's on the left-side.
 * There may or may not be a DebuggerJitInfo associated with the MethodDesc.
 * At most one CordbJITInfo is created for each native code compilation of each method
 * that is seen by the right-side.  Note that if each method were JITted only once
 * then this information could go in CordbFunction, however generics allow
 * methods to be compiled more than once.
 *
 * The purpose of this class is to combine native code objects (CordbCode) with
 * an optional set of mappings from IL to offsets in that Native Code.
 * ------------------------------------------------------------------------- */

class CordbJITInfo : public CordbBase
{
public:
    //-----------------------------------------------------------
    // Create from scope and member objects.
    //-----------------------------------------------------------

    CordbJITInfo(CordbFunction *f, CordbCode *nativecode, BOOL isInstantiatedGeneric);
    virtual ~CordbJITInfo();
    virtual void Neuter(NeuterTicket ticket);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbJITInfo"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // Internal members
    //-----------------------------------------------------------

    HRESULT ILVariableToNative(DWORD dwIndex,
                               SIZE_T ip,
                               ICorJitInfo::NativeVarInfo **ppNativeInfo);
    HRESULT LoadNativeInfo(void);

    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------

    CordbAppDomain *GetAppDomain()
    {
        return (m_function->GetAppDomain());
    }

    CordbModule *GetModule()
    {
        return m_function->GetModule();
    }

    CordbFunction *GetFunction()
    {
        return m_function;
    }

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:

    static HRESULT LookupOrCreateFromJITData(CordbFunction *pFunction, DebuggerIPCE_FuncData *currentFuncData, DebuggerIPCE_JITFuncData* currentJITFuncData, CordbJITInfo **ppRes);

    CordbFunction           *m_function;
    RSSmartPtr<CordbCode>    m_nativecode;

    BOOL                     m_isInstantiatedGeneric;

    unsigned int             m_nativeInfoCount;
    ICorJitInfo::NativeVarInfo *m_nativeInfo;
    BOOL                     m_nativeInfoValid;
    unsigned int             m_argumentCount;

};

/* ------------------------------------------------------------------------- *
 * Code class
 * ------------------------------------------------------------------------- */

class CordbCode : public CordbBase, public ICorDebugCode, public ICorDebugCode2
{
private:
    // Create an IL code
    CordbCode(CordbFunction *m, REMOTE_PTR startAddress,
        SIZE_T size, SIZE_T nVersion);

    // Create an Native code
    CordbCode(CordbFunction *m, DebuggerIPCE_JITFuncData * pJitData);
public:
    //-----------------------------------------------------------
    // Have static Builders (rather than ctors) to make it obvious
    // if the caller is making IL or Native code.
    //-----------------------------------------------------------
    static CordbCode * BuildILCode(CordbFunction *m, REMOTE_PTR startAddress,
        SIZE_T size, SIZE_T nVersion);

    static CordbCode * BuildNativeCode(CordbFunction *m, DebuggerIPCE_JITFuncData * pJitData);

    virtual ~CordbCode();
    virtual void Neuter(NeuterTicket ticket);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbCode"; }
#endif



    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugCode
    //-----------------------------------------------------------

    COM_METHOD IsIL(BOOL *pbIL);
    COM_METHOD GetFunction(ICorDebugFunction **ppFunction);
    COM_METHOD GetAddress(CORDB_ADDRESS *pStart);
    COM_METHOD GetSize(ULONG32 *pcBytes);
    COM_METHOD CreateBreakpoint(ULONG32 offset,
                                ICorDebugFunctionBreakpoint **ppBreakpoint);
    COM_METHOD GetCode(ULONG32 startOffset, ULONG32 endOffset,
                       ULONG32 cBufferAlloc,
                       BYTE buffer[],
                       ULONG32 *pcBufferSize);
    COM_METHOD GetVersionNumber( ULONG32 *nVersion);
    COM_METHOD GetILToNativeMapping(ULONG32 cMap,
                                    ULONG32 *pcMap,
                                    COR_DEBUG_IL_TO_NATIVE_MAP map[]);
    COM_METHOD GetEnCRemapSequencePoints(ULONG32 cMap,
                                         ULONG32 *pcMap,
                                         ULONG32 offsets[]);

    //-----------------------------------------------------------
    // ICorDebugCode2
    //-----------------------------------------------------------
    COM_METHOD GetCodeChunks(ULONG32 cbufSize, ULONG32 * pcnumChunks, CodeChunkInfo chunks[]);

    COM_METHOD GetCompilerFlags(DWORD *pdwFlags);

    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------

    CordbAppDomain *GetAppDomain()
    {
        return (m_function->GetAppDomain());
    }

    REMOTE_PTR GetAddress() { return m_address; };

    //-----------------------------------------------------------
    // Internal methods
    //-----------------------------------------------------------
    bool HasColdRegion() { return m_addressCold != NULL; }
    BOOL IsIL() { return m_isIL; }
    HRESULT GetCompilerFlagsInternal(DWORD *pdwFlags);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

private:
    // struct
    // {
        UINT m_isIL : 1;
    // };


public:
    CordbFunction         *m_function;
    SIZE_T                 m_nVersion;
    LSPTR_DJI              m_nativeCodeJITInfoToken;
    LSPTR_METHODDESC       m_nativeCodeMethodDescToken;

protected:
    // Start address & size of the method. (If we're split, this is the
    // address of the hot region and size of both hot + cold regions).
    REMOTE_PTR             m_address;
    SIZE_T                 m_size;

    // Code may be split into Hot & Cold regions, so we need an extra address & size.
    // The jitter doesn't do this optimization w/ debuggable code, so we'll
    // rarely see these as non-null values.
    REMOTE_PTR             m_addressCold;
    SIZE_T                 m_sizeCold;

    // Our local copy of the code. m_size bytes long.
    BYTE                  *m_rgbCode; //will be NULL if we can't fit it into memory
    UINT                   m_continueCounterLastSync;

    REMOTE_PTR             m_ilToNativeMapAddr;
    SIZE_T                 m_ilToNativeMapSize;
};


/* ------------------------------------------------------------------------- *
 * Thread classes
 * ------------------------------------------------------------------------- */

class CordbThread : public CordbBase, public ICorDebugThread, public ICorDebugThread2
{
public:
    CordbThread(CordbProcess *process, DWORD id, HANDLE handle);
    virtual ~CordbThread();
    virtual void Neuter(NeuterTicket ticket);

    using CordbBase::GetProcess;
    
#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbThread"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        // there's an external add ref from within RS in CordbEnumFilter
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugThread
    //-----------------------------------------------------------

    COM_METHOD GetProcess(ICorDebugProcess **ppProcess);
    COM_METHOD GetID(DWORD *pdwThreadId);
    COM_METHOD GetHandle(void** phThreadHandle);
    COM_METHOD GetAppDomain(ICorDebugAppDomain **ppAppDomain);
    COM_METHOD SetDebugState(CorDebugThreadState state);
    COM_METHOD GetDebugState(CorDebugThreadState *pState);
    COM_METHOD GetUserState(CorDebugUserState *pState);
    COM_METHOD GetCurrentException(ICorDebugValue **ppExceptionObject);
    COM_METHOD ClearCurrentException();
    COM_METHOD CreateStepper(ICorDebugStepper **ppStepper);
    COM_METHOD EnumerateChains(ICorDebugChainEnum **ppChains);
    COM_METHOD GetActiveChain(ICorDebugChain **ppChain);
    COM_METHOD GetActiveFrame(ICorDebugFrame **ppFrame);
    COM_METHOD GetRegisterSet(ICorDebugRegisterSet **ppRegisters);
    COM_METHOD CreateEval(ICorDebugEval **ppEval);
    COM_METHOD GetObject(ICorDebugValue **ppObject);

    // ICorDebugThread2
    COM_METHOD GetConnectionID(DWORD *pdwConnectionId);
    COM_METHOD GetTaskID(TASKID *pTaskId);
    COM_METHOD GetVolatileOSThreadID(DWORD *pdwTid);
    COM_METHOD GetActiveFunctions(ULONG32 cStatements, ULONG32 *pcStatements, COR_ACTIVE_FUNCTION pFunctions[]);
    COM_METHOD InterceptCurrentException(ICorDebugFrame *pFrame);


    HRESULT FindFrame(CordbFrame** ppFrame, FramePointer fp);
    CordbFrame * GetActiveFrame();


    //-----------------------------------------------------------
    // Internal members
    //-----------------------------------------------------------
    HRESULT RefreshStack(void);
    void CleanupStack(void);
    void MarkStackFramesDirty(void);


    HRESULT SetIP(  bool fCanSetIPOnly,
                    LSPTR_DMODULE debuggerModule,
                    mdMethodDef mdMethodDef,
                    LSPTR_DJI versionToken,
                    SIZE_T offset,
                    bool fIsIL );

    HRESULT SetRemapIP(SIZE_T offset);

    // Ask the left-side for the current (up-to-date) AppDomain of this thread's IP.
    // This should be preferred over using the cached value from GetAppDomain.
    HRESULT GetCurrentAppDomain(CordbAppDomain **ppAppDomain);


    CordbAppDomain *GetAppDomain()
    {
        return (m_pAppDomain);
    }

    //////////////////////////////////////////////////////////////////////////
    //
    // Get Context
    //
    //
    // Returns: NULL if the thread's CONTEXT structure couldn't be obtained
    //   A pointer to the CONTEXT otherwise.
    //
    //
    //////////////////////////////////////////////////////////////////////////
    HRESULT GetManagedContext( CONTEXT **ppContext );
    HRESULT SetManagedContext( CONTEXT *pContext );

    // API to retrieve the thread handle from the LS.
    HRESULT  InternalGetHandle(HANDLE *phandle);
    HRESULT RefreshHandle(HANDLE *phandle);
    bool CLRTaskHosted();

    // NeuterList that's executed when this Thread's stack is refreshed.
    // Chain + Frame + some Value enums can be held on this.
    NeuterList * GetRefreshStackNeuterList()
    {
        return &m_RefreshStackNeuterList;
    }

    bool OwnsFrame(CordbFrame *pFrame);

    // Specify that there's an outstanding exception on this thread.
    void SetExInfo(LSPTR_OBJECTHANDLE ohThrown);

    LSPTR_OBJECTHANDLE GetThreadExceptionRawObjectHandle() { return m_thrown; }
    bool HasException() { return m_exception; }


    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    // RS Cache for LS context.
    // NULL if we haven't allocated memory for a Right side context
    CONTEXT              *m_pContext;

    // Set to the CONTEXT pointer in the LS if this LS thread is
    // stopped in managed code. This may be either stopped for execution control
    // (breakpoint / single-step exception) or hijacked w/ a redirected frame because
    // another thread synced the LS.
    // This context is used by the RS to set enregistered vars.
    LSPTR_CONTEXT         m_pvLeftSideContext;

    bool                  m_contextFresh;
    CordbAppDomain       *m_pAppDomain;     // last domain we've seen this thread
    LSPTR_THREAD          m_debuggerThreadToken;

    CorDebugThreadState   m_debugState; // Note that this is for resume
                                        // purposes, NOT the current state of
                                        // the thread.

    CorDebugUserState     m_userState;  // This is the current state of the
                                        // thread, at the time that the
                                        // left side synchronized

    // The frames are all protected under the Stop-Go lock.
    bool                  m_framesFresh;

    RSPtrArray<CordbFrame> m_stackFrames;
    RSPtrArray<CordbChain> m_stackChains; 


private:
    // True for the window after an Exception callback, but before it's been continued.
    bool                  m_exception;

    // Object handle for Exception object in debuggee.
    LSPTR_OBJECTHANDLE    m_thrown;

public:
    // These are used for LogMessages
    void                 *m_firstExceptionHandler; //left-side pointer - fs:[0] on x86

    bool                  m_detached;

    // On a RemapBreakpoint, the debugger will eventually call RemapFunction and
    // we need to communicate the IP back to LS. So we stash the address of where
    // to store the IP here and stuff it in on RemapFunction.
    REMOTE_PTR            m_EnCRemapFunctionIP;

    DWORD                 m_Tid; // OS Thread id

private:
    // NeuterList that's executed when this Thread's stack is refreshed
    NeuterList            m_RefreshStackNeuterList;

    HANDLE                m_handle;

    // The following two data members are only used when CLRTaskHosted.
    HANDLE                m_cachedHandle;
    HANDLE                m_cachedOutOfProcHandle;
};

/* ------------------------------------------------------------------------- *
 * Chain class
 * ------------------------------------------------------------------------- */

class CordbChain : public CordbBase, public ICorDebugChain
{
public:
    CordbChain(CordbThread* thread,
               bool managed, CordbFrame **start, CordbFrame **end, UINT iChainInThread);

    virtual ~CordbChain();
    virtual void Neuter(NeuterTicket ticket);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbChain"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugChain
    //-----------------------------------------------------------

    COM_METHOD GetThread(ICorDebugThread **ppThread);
    COM_METHOD GetReason(CorDebugChainReason *pReason);
    COM_METHOD GetStackRange(CORDB_ADDRESS *pStart, CORDB_ADDRESS *pEnd);
    COM_METHOD GetContext(ICorDebugContext **ppContext);
    COM_METHOD GetCaller(ICorDebugChain **ppChain);
    COM_METHOD GetCallee(ICorDebugChain **ppChain);
    COM_METHOD GetPrevious(ICorDebugChain **ppChain);
    COM_METHOD GetNext(ICorDebugChain **ppChain);
    COM_METHOD IsManaged(BOOL *pManaged);
    COM_METHOD EnumerateFrames(ICorDebugFrameEnum **ppFrames);
    COM_METHOD GetActiveFrame(ICorDebugFrame **ppFrame);
    COM_METHOD GetRegisterSet(ICorDebugRegisterSet **ppRegisters);

    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------

    FramePointer GetFramePointer()
    {
        return m_fp;
    }

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    CordbThread             *m_thread;
    UINT                     m_iThisChain;//in m_thread->m_stackChains
    CordbChain              *m_caller, *m_callee;
    bool                     m_managed;
    CordbFrame             **m_start, **m_end;
    CorDebugChainReason      m_reason;
    CORDB_ADDRESS            m_context;
    DebuggerREGDISPLAY       m_rd;
    bool                     m_quicklyUnwound;
    bool                     m_active;
    FramePointer             m_fp;

};

/* ------------------------------------------------------------------------- *
 * Chain enumerator class
 * ------------------------------------------------------------------------- */

class CordbChainEnum : public CordbBase, public ICorDebugChainEnum
{
public:
    CordbChainEnum(CordbThread *thread);
    ~CordbChainEnum();
    virtual void Neuter(NeuterTicket ticket);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbChainEnum"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugEnum
    //-----------------------------------------------------------

    COM_METHOD Skip(ULONG celt);
    COM_METHOD Reset(void);
    COM_METHOD Clone(ICorDebugEnum **ppEnum);
    COM_METHOD GetCount(ULONG *pcelt);

    //-----------------------------------------------------------
    // ICorDebugChainEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugChain *chains[], ULONG *pceltFetched);

private:
    // This will get null-out once we're neutered.
    CordbThread*    m_thread;
    unsigned long   m_currentChain;
};

class CordbContext : public CordbBase, public ICorDebugContext
{
public:

    CordbContext() : CordbBase(NULL, 0, enumCordbContext) {}



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbContext"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugContext
    //-----------------------------------------------------------
private:

} ;


/* ------------------------------------------------------------------------- *
 * Frame class
 * ------------------------------------------------------------------------- */

class CordbFrame : public CordbBase, public ICorDebugFrame
{
public:
    CordbFrame(CordbChain *chain, FramePointer fp,
               SIZE_T ip, UINT iFrameInChain, CordbAppDomain *currentAppDomain);

    virtual ~CordbFrame();
    virtual void Neuter(NeuterTicket ticket);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbFrame"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugFrame
    //-----------------------------------------------------------

    COM_METHOD GetChain(ICorDebugChain **ppChain);

    // Derived versions of Frame will implement GetCode.
    COM_METHOD GetCode(ICorDebugCode **ppCode) = 0;

    COM_METHOD GetFunction(ICorDebugFunction **ppFunction);
    COM_METHOD GetFunctionToken(mdMethodDef *pToken);

    COM_METHOD GetStackRange(CORDB_ADDRESS *pStart, CORDB_ADDRESS *pEnd);
    COM_METHOD GetCaller(ICorDebugFrame **ppFrame);
    COM_METHOD GetCallee(ICorDebugFrame **ppFrame);
    COM_METHOD CreateStepper(ICorDebugStepper **ppStepper);

    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------

    CordbAppDomain *GetCurrentAppDomain()
    {
        return m_currentAppDomain;
    }

    // Internal helper to get a CordbFunction for this frame.
    virtual CordbFunction *GetFunction() = 0;

    FramePointer GetFramePointer()
    {
        return m_fp;
    }

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

    // Accessors to return NULL or typesafe cast to derived frame
    virtual CordbInternalFrame * GetAsInternalFrame()   { return NULL; }
    virtual CordbNativeFrame * GetAsNativeFrame()       { return NULL; }

    bool IsLeafFrame();
    bool IsContainedInFrame(FramePointer fp);

    static CordbFrame* GetCordbFrameFromInterface(ICorDebugFrame *pFrame);

public:
    SIZE_T                  m_ip;
    CordbThread            *m_thread;
    CordbChain             *m_chain;
    bool                    m_active;
    UINT                    m_iThisFrame; // Index into the frame chain.
    CordbAppDomain         *m_currentAppDomain;
    FramePointer            m_fp;

private:
#ifdef _DEBUG
    // For tracking down neutering bugs;
    UINT                   m_DbgContinueCounter;
#endif
};

class CordbInternalFrame : public CordbFrame, public ICorDebugInternalFrame
{
public:
    CordbInternalFrame(
        CordbChain *chain,
        FramePointer fp,
        DebuggerREGDISPLAY *rd,
        UINT iFrameInChain,
        CordbAppDomain *currentAppDomain,
        DebuggerIPCE_STRData * pData
    );

    virtual void Neuter(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbInternalFrame"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugFrame
    //-----------------------------------------------------------

    COM_METHOD GetChain(ICorDebugChain **ppChain)
    {
        return (CordbFrame::GetChain(ppChain));
    }

    // We don't expose a code-object for stubs.
    COM_METHOD GetCode(ICorDebugCode **ppCode)
    {
        return CORDBG_E_CODE_NOT_AVAILABLE;
    }

    COM_METHOD GetFunction(ICorDebugFunction **ppFunction)
    {
        return (CordbFrame::GetFunction(ppFunction));
    }
    COM_METHOD GetFunctionToken(mdMethodDef *pToken)
    {
        return (CordbFrame::GetFunctionToken(pToken));
    }

    COM_METHOD GetCaller(ICorDebugFrame **ppFrame)
    {
        return (CordbFrame::GetCaller(ppFrame));
    }
    COM_METHOD GetCallee(ICorDebugFrame **ppFrame)
    {
        return (CordbFrame::GetCallee(ppFrame));
    }
    COM_METHOD CreateStepper(ICorDebugStepper **ppStepper)
    {
        return E_NOTIMPL;
    }

    COM_METHOD GetStackRange(CORDB_ADDRESS *pStart, CORDB_ADDRESS *pEnd);

    //-----------------------------------------------------------
    // ICorDebugInternalFrame
    //-----------------------------------------------------------

    // Get the type of internal frame. This will never be STUBFRAME_NONE.
    COM_METHOD GetFrameType(CorDebugInternalFrameType * pType)
    {
        VALIDATE_POINTER_TO_OBJECT(pType, CorDebugInternalFrameType)
        *pType = m_eFrameType;
        return S_OK;
    }

    //-----------------------------------------------------------
    // Non COM methods
    //-----------------------------------------------------------

    virtual CordbFunction *GetFunction();


    // Accessors to return NULL or typesafe cast to derived frame
    virtual CordbInternalFrame * GetAsInternalFrame()   { return this; }

protected:
    DebuggerREGDISPLAY        m_rd;
    CorDebugInternalFrameType m_eFrameType;
    mdMethodDef m_funcMetadataToken;
    RSSmartPtr<CordbFunction> m_function;
};


/* ------------------------------------------------------------------------- *
 * Frame enumerator class
 * ------------------------------------------------------------------------- */

class CordbFrameEnum : public CordbBase, public ICorDebugFrameEnum
{
public:
    CordbFrameEnum(CordbChain *chain);

    virtual ~CordbFrameEnum();
    virtual void Neuter(NeuterTicket ticket);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbFrameEnum"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugEnum
    //-----------------------------------------------------------

    COM_METHOD Skip(ULONG celt);
    COM_METHOD Reset(void);
    COM_METHOD Clone(ICorDebugEnum **ppEnum);
    COM_METHOD GetCount(ULONG *pcelt);

    //-----------------------------------------------------------
    // ICorDebugFrameEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugFrame *frames[], ULONG *pceltFetched);

private:
    // Parent object is really m_chain->m_thread. This will get nulled out once we're neutered.
    CordbChain*     m_chain;

    CordbFrame**    m_currentFrame;
};


class CordbValueEnum : public CordbBase, public ICorDebugValueEnum
{
public:
    enum ValueEnumMode {
        LOCAL_VARS,
        ARGS,
    } ;

    CordbValueEnum(CordbNativeFrame *frame, ValueEnumMode mode);
    HRESULT Init();
    ~CordbValueEnum();
    virtual void Neuter(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbValueEnum"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugEnum
    //-----------------------------------------------------------

    COM_METHOD Skip(ULONG celt);
    COM_METHOD Reset(void);
    COM_METHOD Clone(ICorDebugEnum **ppEnum);
    COM_METHOD GetCount(ULONG *pcelt);

    //-----------------------------------------------------------
    // ICorDebugValueEnum
    //-----------------------------------------------------------

    COM_METHOD Next(ULONG celt, ICorDebugValue *values[], ULONG *pceltFetched);

private:
    CordbNativeFrame*     m_frame;
    ValueEnumMode   m_mode;
    UINT            m_iCurrent;
    UINT            m_iMax;
};


/* ------------------------------------------------------------------------- *
 * Misc Info for the Native Frame class
 * ------------------------------------------------------------------------- */

struct CordbMiscFrame
{
};


/* ------------------------------------------------------------------------- *
 * Native Frame class
 * ------------------------------------------------------------------------- */

class CordbNativeFrame : public CordbFrame, public ICorDebugNativeFrame
{
public:
    CordbNativeFrame(CordbChain *chain, FramePointer m_fp,
                     CordbJITInfo *function,
                     SIZE_T ip, DebuggerREGDISPLAY* rd,
                     void * ambientESP,
                     bool quicklyUnwound,
                     UINT iFrameInChain,
                     CordbAppDomain *currentAppDomain,
                     CordbMiscFrame *pMisc = NULL);
    virtual ~CordbNativeFrame();
    virtual void Neuter(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbNativeFrame"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugFrame
    //-----------------------------------------------------------

    COM_METHOD GetChain(ICorDebugChain **ppChain)
    {
        return (CordbFrame::GetChain(ppChain));
    }
    COM_METHOD GetCode(ICorDebugCode **ppCode);
    COM_METHOD GetFunction(ICorDebugFunction **ppFunction)
    {
        return (CordbFrame::GetFunction(ppFunction));
    }
    COM_METHOD GetFunctionToken(mdMethodDef *pToken)
    {
        return (CordbFrame::GetFunctionToken(pToken));
    }
    COM_METHOD GetCaller(ICorDebugFrame **ppFrame)
    {
        return (CordbFrame::GetCaller(ppFrame));
    }
    COM_METHOD GetCallee(ICorDebugFrame **ppFrame)
    {
        return (CordbFrame::GetCallee(ppFrame));
    }
    COM_METHOD CreateStepper(ICorDebugStepper **ppStepper)
    {
        return (CordbFrame::CreateStepper(ppStepper));
    }

    COM_METHOD GetStackRange(CORDB_ADDRESS *pStart, CORDB_ADDRESS *pEnd);

    //-----------------------------------------------------------
    // ICorDebugNativeFrame
    //-----------------------------------------------------------

    COM_METHOD GetIP(ULONG32* pnOffset);
    COM_METHOD SetIP(ULONG32 nOffset);
    COM_METHOD GetRegisterSet(ICorDebugRegisterSet **ppRegisters);
    COM_METHOD GetLocalRegisterValue(CorDebugRegister reg,
                                     ULONG cbSigBlob,
                                     PCCOR_SIGNATURE pvSigBlob,
                                     ICorDebugValue **ppValue);

    COM_METHOD GetLocalDoubleRegisterValue(CorDebugRegister highWordReg,
                                           CorDebugRegister lowWordReg,
                                           ULONG cbSigBlob,
                                           PCCOR_SIGNATURE pvSigBlob,
                                           ICorDebugValue **ppValue);

    COM_METHOD GetLocalMemoryValue(CORDB_ADDRESS address,
                                   ULONG cbSigBlob,
                                   PCCOR_SIGNATURE pvSigBlob,
                                   ICorDebugValue **ppValue);

    COM_METHOD GetLocalRegisterMemoryValue(CorDebugRegister highWordReg,
                                           CORDB_ADDRESS lowWordAddress,
                                           ULONG cbSigBlob,
                                           PCCOR_SIGNATURE pvSigBlob,
                                           ICorDebugValue **ppValue);

    COM_METHOD GetLocalMemoryRegisterValue(CORDB_ADDRESS highWordAddress,
                                           CorDebugRegister lowWordRegister,
                                           ULONG cbSigBlob,
                                           PCCOR_SIGNATURE pvSigBlob,
                                           ICorDebugValue **ppValue);

    COM_METHOD CanSetIP(ULONG32 nOffset);

    //-----------------------------------------------------------
    // Non-COM members
    //-----------------------------------------------------------

    // Accessors to return NULL or typesafe cast to derived frame
    virtual CordbNativeFrame * GetAsNativeFrame()       { return this; }

    CordbFunction * GetFunction();
    CordbCode * GetCode();

    HRESULT GetLocalRegisterValue(CorDebugRegister reg,
                                     CordbType *type,
                                     ICorDebugValue **ppValue);
    HRESULT GetLocalDoubleRegisterValue(CorDebugRegister highWordReg,
                                           CorDebugRegister lowWordReg,
                                           CordbType *type,
                                           ICorDebugValue **ppValue);
    HRESULT GetLocalMemoryValue(CORDB_ADDRESS address,
                                   CordbType *type,
                                   ICorDebugValue **ppValue);
    HRESULT GetLocalByRefMemoryValue(CORDB_ADDRESS address,
                                        CordbType *type,
                                        ICorDebugValue **ppValue);
    HRESULT GetLocalRegisterMemoryValue(CorDebugRegister highWordReg,
                                           CORDB_ADDRESS lowWordAddress,
                                           CordbType *type,
                                           ICorDebugValue **ppValue);
    HRESULT GetLocalMemoryRegisterValue(CORDB_ADDRESS highWordAddress,
                                           CorDebugRegister lowWordRegister,
                                           CordbType *type,
                                           ICorDebugValue **ppValue);
    UINT_PTR *GetAddressOfRegister(CorDebugRegister regNum);
    void  *GetLeftSideAddressOfRegister(CorDebugRegister regNum);


    CORDB_ADDRESS GetLSStackAddress(ICorJitInfo::RegNum regNum, signed offset);
    SIZE_T GetInspectionIP();



    void * GetAmbientESP() { return m_ambientESP; }
    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    DebuggerREGDISPLAY m_rd;
    bool               m_quicklyUnwound;
    RSSmartPtr<CordbJITILFrame> m_JITILFrame;
    RSSmartPtr<CordbJITInfo>    m_jitinfo;
    CordbMiscFrame     m_misc;

private:
    void *   m_ambientESP;
};


/* ------------------------------------------------------------------------- *
 * CordbRegisterSet class
 *
 * This can be obtained via GetRegisterSet from
 *      CordbChain
 *      CordbNativeFrame
 *      CordbThread
 *
 * ------------------------------------------------------------------------- */

#define SETBITULONG64( x ) ( (ULONG64)1 << (x) )
#define SET_BIT_MASK(_mask, _reg)      (_mask[(_reg) >> 3] |=  (1 << ((_reg) & 7)))
#define RESET_BIT_MASK(_mask, _reg)    (_mask[(_reg) >> 3] &= ~(1 << ((_reg) & 7)))
#define IS_SET_BIT_MASK(_mask, _reg)   (_mask[(_reg) >> 3] &   (1 << ((_reg) & 7)))


class CordbRegisterSet : public CordbBase, public ICorDebugRegisterSet, public ICorDebugRegisterSet2
{
public:
    CordbRegisterSet( DebuggerREGDISPLAY *rd, CordbThread *thread,
                      bool active, bool quickUnwind );


    ~CordbRegisterSet();



    virtual void Neuter(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbRegisterSet"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }

    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);



    //-----------------------------------------------------------
    // ICorDebugRegisterSet
    // More extensive explanation are in Src/inc/CorDebug.idl
    //-----------------------------------------------------------
    COM_METHOD GetRegistersAvailable(ULONG64 *pAvailable);

    COM_METHOD GetRegisters(ULONG64 mask,
                            ULONG32 regCount,
                            CORDB_REGISTER regBuffer[]);
    COM_METHOD SetRegisters( ULONG64 mask,
                             ULONG32 regCount,
                             CORDB_REGISTER regBuffer[])
    {
        LEAF_CONTRACT;

        VALIDATE_POINTER_TO_OBJECT_ARRAY(regBuffer, CORDB_REGISTER,
                                         regCount, true, true);

        return E_NOTIMPL;
    }

    COM_METHOD GetThreadContext(ULONG32 contextSize, BYTE context[]);

    //
    COM_METHOD SetThreadContext(ULONG32 contextSize, BYTE context[])
    {
        return E_NOTIMPL;
    }

    //-----------------------------------------------------------
    // ICorDebugRegisterSet2
    // More extensive explanation are in Src/inc/CorDebug.idl
    //-----------------------------------------------------------
    COM_METHOD GetRegistersAvailable(ULONG32 regCount,
                                     BYTE    pAvailable[]);

    COM_METHOD GetRegisters(ULONG32 maskCount,
                            BYTE    mask[],
                            ULONG32 regCount,
                            CORDB_REGISTER regBuffer[]);

    COM_METHOD SetRegisters(ULONG32 maskCount,
                            BYTE    mask[],
                            ULONG32 regCount,
                            CORDB_REGISTER regBuffer[])
    {
        LEAF_CONTRACT;

        VALIDATE_POINTER_TO_OBJECT_ARRAY(regBuffer, CORDB_REGISTER,
                                         regCount, true, true);

        return E_NOTIMPL;
    }

protected:
    // Platform specific helper for GetThreadContext.
    void InternalCopyRDToContext(CONTEXT *pContext);

    // Adapters to impl v2.0 interfaces on top of v1.0 interfaces.
    HRESULT GetRegistersAvailableAdapter(ULONG32 regCount, BYTE pAvailable[]);
    HRESULT GetRegistersAdapter(ULONG32 maskCount, BYTE mask[], ULONG32 regCount, CORDB_REGISTER regBuffer[]);


    DebuggerREGDISPLAY  *m_rd;
    CordbThread         *m_thread;
    bool                m_active; // true if we're the leafmost register set.
    bool                m_quickUnwind;
} ;




/* ------------------------------------------------------------------------- *
 * JIT-IL Frame class
 * ------------------------------------------------------------------------- */

class CordbJITILFrame : public CordbBase, public ICorDebugILFrame, public ICorDebugILFrame2
{
public:
    CordbJITILFrame(CordbNativeFrame *nativeFrame,
                    CordbCode* code,
                    UINT_PTR ip,
                    CorDebugMappingResult mapping,
                    void *exactGenericArgsToken,
                    bool fVarArgFnx,
                    void *rpSig,
                    ULONG cbSig,
                    void *rpFirstArg);
    HRESULT Init();
    virtual ~CordbJITILFrame();
    virtual void Neuter(NeuterTicket ticket);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbJITILFrame"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugFrame
    //-----------------------------------------------------------

    COM_METHOD GetChain(ICorDebugChain **ppChain);
    COM_METHOD GetCode(ICorDebugCode **ppCode);
    COM_METHOD GetFunction(ICorDebugFunction **ppFunction);
    COM_METHOD GetFunctionToken(mdMethodDef *pToken);
    COM_METHOD GetStackRange(CORDB_ADDRESS *pStart, CORDB_ADDRESS *pEnd);
    COM_METHOD CreateStepper(ICorDebugStepper **ppStepper);
    COM_METHOD GetCaller(ICorDebugFrame **ppFrame);
    COM_METHOD GetCallee(ICorDebugFrame **ppFrame);

    //-----------------------------------------------------------
    // ICorDebugILFrame
    //-----------------------------------------------------------

    COM_METHOD GetIP(ULONG32* pnOffset, CorDebugMappingResult *pMappingResult);
    COM_METHOD SetIP(ULONG32 nOffset);
    COM_METHOD EnumerateLocalVariables(ICorDebugValueEnum **ppValueEnum);
    COM_METHOD GetLocalVariable(DWORD dwIndex, ICorDebugValue **ppValue);
    COM_METHOD EnumerateArguments(ICorDebugValueEnum **ppValueEnum);
    COM_METHOD GetArgument(DWORD dwIndex, ICorDebugValue **ppValue);
    COM_METHOD GetStackDepth(ULONG32 *pDepth);
    COM_METHOD GetStackValue(DWORD dwIndex, ICorDebugValue **ppValue);
    COM_METHOD CanSetIP(ULONG32 nOffset);
    COM_METHOD EnumerateTypeParameters(ICorDebugTypeEnum **ppTyParEnum);

    //-----------------------------------------------------------
    // ICorDebugILFrame2
    //-----------------------------------------------------------

    COM_METHOD RemapFunction(ULONG32 nOffset);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    CordbModule *GetModule(void);

    HRESULT GetNativeVariable(CordbType *type,
                              ICorJitInfo::NativeVarInfo *pJITInfo,
                              ICorDebugValue **ppValue);

    CordbAppDomain *GetCurrentAppDomain(void);

    CordbFunction *GetFunction(void);

    // ILVariableToNative serves to let the frame intercept accesses
    // to var args variables.
    HRESULT ILVariableToNative(DWORD dwIndex,
                               ICorJitInfo::NativeVarInfo **ppNativeInfo);

    // Fills in our array of var args variables
    HRESULT FabricateNativeInfo(DWORD dwIndex,
                                ICorJitInfo::NativeVarInfo **ppNativeInfo);

    HRESULT GetArgumentType(DWORD dwIndex,
                            CordbType **pType);

    HRESULT LoadGenericArgs();

    HRESULT QueryInterfaceInternal(REFIID id, void** pInterface);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    CordbNativeFrame* m_nativeFrame;
    CordbCode*        m_ilCode;
    UINT_PTR          m_ip;
    CorDebugMappingResult m_mapping;

    // var args stuff - if m_fVarArgFnx == true, it's a var args
    // fnx.  if m_pSigParserCached != NULL, then we've got the data we need
    bool              m_fVarArgFnx;
    ULONG             m_argCount;
    BYTE             *m_sigBuf;
    ULONG             m_cbSigBuf;
    SigParser         m_sigParserCached;   // Do not mutate this, instead make copies of it and use the 
                                           // copies, that way we are guaranteed to start at the correct
                                           // position in the signature each time.
    void *            m_rpFirstArg;
    ICorJitInfo::NativeVarInfo * m_rgNVI;

    Instantiation     m_genericArgs;
    BOOL              m_genericArgsLoaded;

    // An extra token to help fetch information about any generic
    // parameters passed to the method, perhaps dynamically.
    void *            m_frameParamsToken;

};

/* ------------------------------------------------------------------------- *
 * Breakpoint class
 * ------------------------------------------------------------------------- */

enum CordbBreakpointType
{
    CBT_FUNCTION,
    CBT_MODULE,
    CBT_VALUE
};

class CordbBreakpoint : public CordbBase, public ICorDebugBreakpoint
{
public:
    CordbBreakpoint(CordbProcess * pProcess, CordbBreakpointType bpType);
    virtual void Neuter(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbBreakpoint"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugBreakpoint
    //-----------------------------------------------------------

    COM_METHOD BaseIsActive(BOOL *pbActive);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------
    CordbBreakpointType GetBPType(void)
    {
        return m_type;
    }

    virtual void Disconnect() {}

    CordbAppDomain *GetAppDomain()
    {
        return m_pAppDomain;
    }
    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    bool                m_active;
    CordbAppDomain *m_pAppDomain;
    CordbBreakpointType m_type;
};

/* ------------------------------------------------------------------------- *
 * Function Breakpoint class
 * ------------------------------------------------------------------------- */

class CordbFunctionBreakpoint : public CordbBreakpoint,
                                public ICorDebugFunctionBreakpoint
{
public:
    CordbFunctionBreakpoint(CordbCode *code, SIZE_T offset);
    ~CordbFunctionBreakpoint();

    virtual void Neuter(NeuterTicket ticket);
#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbFunctionBreakpoint"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugBreakpoint
    //-----------------------------------------------------------

    COM_METHOD GetFunction(ICorDebugFunction **ppFunction);
    COM_METHOD GetOffset(ULONG32 *pnOffset);
    COM_METHOD Activate(BOOL bActive);
    COM_METHOD IsActive(BOOL *pbActive)
    {
        VALIDATE_POINTER_TO_OBJECT(pbActive, BOOL *);

        return BaseIsActive(pbActive);
    }

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    void Disconnect();

    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------


    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

    // Get a point to the LS BP object.
    LSPTR_BREAKPOINT GetLsPtrBP();
public:

    RSExtSmartPtr<CordbCode> m_code;
    SIZE_T          m_offset;
};

/* ------------------------------------------------------------------------- *
 * Module Breakpoint class
 * ------------------------------------------------------------------------- */

class CordbModuleBreakpoint : public CordbBreakpoint,
                              public ICorDebugModuleBreakpoint
{
public:
    CordbModuleBreakpoint(CordbModule *pModule);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbModuleBreakpoint"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugModuleBreakpoint
    //-----------------------------------------------------------

    COM_METHOD GetModule(ICorDebugModule **ppModule);
    COM_METHOD Activate(BOOL bActive);
    COM_METHOD IsActive(BOOL *pbActive)
    {
        VALIDATE_POINTER_TO_OBJECT(pbActive, BOOL *);

        return BaseIsActive(pbActive);
    }

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    void Disconnect();

public:
    CordbModule       *m_module;
};


/* ------------------------------------------------------------------------- *
 * Stepper class
 * ------------------------------------------------------------------------- */

class CordbStepper : public CordbBase, public ICorDebugStepper, public ICorDebugStepper2
{
public:
    CordbStepper(CordbThread *thread, CordbFrame *frame = NULL);



#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbStepper"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugStepper
    //-----------------------------------------------------------

    COM_METHOD IsActive(BOOL *pbActive);
    COM_METHOD Deactivate();
    COM_METHOD SetInterceptMask(CorDebugIntercept mask);
    COM_METHOD SetUnmappedStopMask(CorDebugUnmappedStop mask);
    COM_METHOD Step(BOOL bStepIn);
    COM_METHOD StepRange(BOOL bStepIn,
                         COR_DEBUG_STEP_RANGE ranges[],
                         ULONG32 cRangeCount);
    COM_METHOD StepOut();
    COM_METHOD SetRangeIL(BOOL bIL);

    //-----------------------------------------------------------
    // ICorDebugStepper2
    //-----------------------------------------------------------
    COM_METHOD SetJMC(BOOL fIsJMCStepper);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    void Disconnect();

    //-----------------------------------------------------------
    // Convenience routines
    //-----------------------------------------------------------

    CordbAppDomain *GetAppDomain()
    {
        return (m_thread->GetAppDomain());
    }

    LSPTR_STEPPER GetLsPtrStepper();

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

    CordbThread     *m_thread;
    CordbFrame      *m_frame;
    REMOTE_PTR      m_stepperToken;
    bool            m_active;
    bool            m_rangeIL;
    bool            m_fIsJMCStepper;
    CorDebugUnmappedStop m_rgfMappingStop;
    CorDebugIntercept m_rgfInterceptStop;
};

class CordbValueHome
{
public:
    HRESULT Read(BYTE* buffer, SIZE_T size);
    HRESULT Write(BYTE* buffer, SIZE_T size);
    static HRESULT CreateValueHome(CordbValue* value,
                                   CordbValueHome** newValue);

protected:
    CordbValueHome(CordbValue* value);

private:
    CordbValue* m_value;
};

/* ------------------------------------------------------------------------- *
 * Value class
 * ------------------------------------------------------------------------- */

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void HolderRSAddRef(CordbType*);
template void HolderRSRelease(CordbType*);
typedef RSSmartPtr<CordbType*> HACKRCordbTypeRSSmartPtr;
template void DoNothing(CordbType *);
template BOOL CompareDefault(CordbType*,CordbType*);
typedef Holder<CordbType*, DoNothing, DoNothing> HACKRSCordbTypeHolder;
#endif

class CordbValue : public CordbBase
{
public:
    //-----------------------------------------------------------
    // Constructor/destructor
    //-----------------------------------------------------------
    CordbValue(CordbAppDomain *appdomain,
               CordbType *type,
               REMOTE_PTR remoteAddress,
               void *localAddress,
               RemoteAddress *remoteRegAddr,
               bool isLiteral,
               NeuterList * pList = NULL);

    virtual ~CordbValue();
    virtual void Neuter(NeuterTicket ticket);

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }

    //-----------------------------------------------------------
    // ICorDebugValue
    //-----------------------------------------------------------

    COM_METHOD GetType(CorElementType *pType)
    {
        LEAF_CONTRACT;

        FAIL_IF_NEUTERED(this);
        VALIDATE_POINTER_TO_OBJECT(pType, CorElementType *);

        *pType = m_type->m_elementType;
        return (S_OK);
    }

    COM_METHOD GetSize(ULONG32 *pSize)
    {
        LEAF_CONTRACT;

        FAIL_IF_NEUTERED(this);
        VALIDATE_POINTER_TO_OBJECT(pSize, SIZE_T *);

        *pSize = m_size;
        return (S_OK);
    }

    COM_METHOD GetAddress(CORDB_ADDRESS *pAddress)
    {
        LEAF_CONTRACT;

        FAIL_IF_NEUTERED(this);
        VALIDATE_POINTER_TO_OBJECT(pAddress, CORDB_ADDRESS *);

        *pAddress = (CORDB_ADDRESS)m_id;
        return (S_OK);
    }

    COM_METHOD CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint);

    //-----------------------------------------------------------
    // ICorDebugValue2
    //-----------------------------------------------------------

    COM_METHOD GetExactType(ICorDebugType **ppType);

    //-----------------------------------------------------------
    // Methods not exported through COM
    //-----------------------------------------------------------

    static HRESULT CreateValueByType(CordbAppDomain *appdomain,
                                     CordbType *type,
                                     bool boxed,
                                     REMOTE_PTR remoteAddress,
                                     void *localAddress,
                                     RemoteAddress *remoteRegAddr,
                                     IUnknown *pParent,
                                     ICorDebugValue** ppValue);

    HRESULT Init(void);

    HRESULT GetEnregisteredValue(BYTE *pTo, SIZE_T size);
    HRESULT SetEnregisteredValue(void *pFrom);
    HRESULT SetContextRegister(CONTEXT *c,
                               CorDebugRegister reg,
                               SIZE_T newVal,
                               CordbNativeFrame *frame);

    virtual void GetRegisterInfo(DebuggerIPCE_FuncEvalArgData *pFEAD);

    virtual CordbAppDomain *GetAppDomain(void)
    {
        return m_appdomain;
    }

    void SetParent(IUnknown *pParent)
    {
        if (pParent != NULL)
        {
            m_pParent = pParent;
            pParent->AddRef();
        }
    }

    HRESULT InternalCreateHandle(
        CorDebugHandleType handleType,
        ICorDebugHandleValue ** ppHandle);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

public:
    CordbAppDomain*          m_appdomain;
    RSSmartPtr<CordbType>    m_type;
    CordbValueHome*          m_valueHome;

    //bool             m_sigCopied;   // Since the signature shouldn't change,
    //                                //  we only want to copy it once.
    ULONG32          m_size;
    void            *m_localAddress;
    RemoteAddress    m_remoteRegAddr; // register info on the Left Side.
    bool             m_isLiteral;     // true if the value is a RS fabrication.
    IUnknown        *m_pParent;

};

/* ------------------------------------------------------------------------- *
 * Value Breakpoint class
 * ------------------------------------------------------------------------- */

class CordbValueBreakpoint : public CordbBreakpoint,
                             public ICorDebugValueBreakpoint
{
public:
    CordbValueBreakpoint(CordbValue *pValue);


#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbValueBreakpoint"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugModuleBreakpoint
    //-----------------------------------------------------------

    COM_METHOD GetValue(ICorDebugValue **ppValue);
    COM_METHOD Activate(BOOL bActive);
    COM_METHOD IsActive(BOOL *pbActive)
    {
        VALIDATE_POINTER_TO_OBJECT(pbActive, BOOL *);

        return BaseIsActive(pbActive);
    }

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    void Disconnect();

public:
    CordbValue       *m_value;
};

/* ------------------------------------------------------------------------- *
 * Generic Value class
 * ------------------------------------------------------------------------- */

class CordbGenericValue : public CordbValue, public ICorDebugGenericValue, public ICorDebugValue2
{
public:
    CordbGenericValue(CordbAppDomain *appdomain,
                      CordbType *type,
                      REMOTE_PTR remoteAddress,
                      void *localAddress,
                      RemoteAddress *remoteRegAddr);

    CordbGenericValue(CordbAppDomain *appdomain,
                      CordbType *type,
                      DWORD highWord,
                      DWORD lowWord,
                      RemoteAddress *remoteRegAddr);
    CordbGenericValue(CordbType *type);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbGenericValue"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugValue
    //-----------------------------------------------------------

    COM_METHOD GetType(CorElementType *pType)
    {
        return (CordbValue::GetType(pType));
    }
    COM_METHOD GetSize(ULONG32 *pSize)
    {
        return (CordbValue::GetSize(pSize));
    }
    COM_METHOD GetAddress(CORDB_ADDRESS *pAddress)
    {
        return (CordbValue::GetAddress(pAddress));
    }
    COM_METHOD CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint)
    {
        return (CordbValue::CreateBreakpoint(ppBreakpoint));
    }

    //-----------------------------------------------------------
    // ICorDebugValue2
    //-----------------------------------------------------------

    COM_METHOD GetExactType(ICorDebugType **ppType)
    {
        return (CordbValue::GetExactType(ppType));
    }


    //-----------------------------------------------------------
    // ICorDebugGenericValue
    //-----------------------------------------------------------

    COM_METHOD GetValue(void *pTo);
    COM_METHOD SetValue(void *pFrom);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    HRESULT Init(void);
    bool CopyLiteralData(BYTE *pBuffer);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

private:
    BYTE m_copyOfData[8]; // hold copies of up to 64-bit values.
};


/* ------------------------------------------------------------------------- *
 * Reference Value class
 * ------------------------------------------------------------------------- */

class CordbReferenceValue : public CordbValue, public ICorDebugReferenceValue, public ICorDebugValue2
{
public:
    CordbReferenceValue(CordbAppDomain *appdomain,
                        CordbType *type,
                        REMOTE_PTR remoteAddress,
                        void *localAddress,
                        LSPTR_OBJECTHANDLE objectHandle,
                        RemoteAddress *remoteRegAddr );
    CordbReferenceValue(CordbType *type);
    virtual ~CordbReferenceValue();
    virtual void Neuter(NeuterTicket ticket);


#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbReferenceValue"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugValue
    //-----------------------------------------------------------

    COM_METHOD GetType(CorElementType *pType);
    COM_METHOD GetSize(ULONG32 *pSize)
    {
        return (CordbValue::GetSize(pSize));
    }
    COM_METHOD GetAddress(CORDB_ADDRESS *pAddress);
    COM_METHOD CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint)
    {
        return (CordbValue::CreateBreakpoint(ppBreakpoint));
    }

    //-----------------------------------------------------------
    // ICorDebugValue2
    //-----------------------------------------------------------

    COM_METHOD GetExactType(ICorDebugType **ppType)
    {
        return (CordbValue::GetExactType(ppType));
    }

    //-----------------------------------------------------------
    // ICorDebugReferenceValue
    //-----------------------------------------------------------

    COM_METHOD IsNull(BOOL *pbNULL);
    COM_METHOD GetValue(CORDB_ADDRESS *pTo);
    COM_METHOD SetValue(CORDB_ADDRESS pFrom);
    COM_METHOD Dereference(ICorDebugValue **ppValue);
    COM_METHOD DereferenceStrong(ICorDebugValue **ppValue);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    HRESULT InitRef();
    bool CopyLiteralData(BYTE *pBuffer);

    static HRESULT Build(
        CordbAppDomain *appdomain,
        CordbType * type,
        REMOTE_PTR remoteAddress,
        void *localAddress,
        LSPTR_OBJECTHANDLE objectHandle,
        RemoteAddress *remoteRegAddr,
        IUnknown *pParent,
        CordbReferenceValue** ppValue);

    static HRESULT BuildFromGCHandle(CordbAppDomain *pAppDomain, LSPTR_OBJECTHANDLE gcHandle, ICorDebugReferenceValue ** pOutRef);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

    // Common dereference routine shared by both CordbReferenceValue + CordbHandleValue
    static HRESULT DereferenceCommon(
        CordbAppDomain * pAppDomain,
        CordbType * pType,
        CordbType * pRealTypeOfTypedByref,
        DebuggerIPCE_ObjectData * m_pInfo,
        RSSmartPtr<CordbObjectValue> * ppCachedObject,
        ICorDebugValue **ppValue);

public:
    // Used if the Reference is for the other end of an object handle.
    // Null if the reference is a regular reference (including if it's enregistered).
    LSPTR_OBJECTHANDLE       m_objectHandle;

    DebuggerIPCE_ObjectData  m_info;
    CordbType               *m_realTypeOfTypedByref; // weak ref

    // Cached value from dereferencing
    RSSmartPtr<CordbObjectValue> m_objectWeak;

    // Indicates when we last syncronized our stored data (m_info) from the left side
    UINT                    m_continueCounterLastSync;
};

/* ------------------------------------------------------------------------- *
 * Object Value class
 *
 * Because of the oddness of string objects in the Runtime we have one
 * object that implements both ObjectValue and StringValue. There is a
 * definite string type, but its really just an object of the string
 * class. Furthermore, you can have a variable whose type is listed as
 * "class", but its an instance of the string class and therefore needs
 * to be treated like a string.                                                 
 * ------------------------------------------------------------------------- */

class CordbObjectValue : public CordbValue, public ICorDebugObjectValue,
                         public ICorDebugObjectValue2,
                         public ICorDebugGenericValue,
                         public ICorDebugStringValue, public ICorDebugValue2, public ICorDebugHeapValue2
{
    friend HRESULT CordbClass::GetSyncBlockField(mdFieldDef fldToken,
                                      DebuggerIPCE_FieldData **ppFieldData,
                                      CordbObjectValue *object);

public:
    CordbObjectValue(CordbAppDomain *appdomain,
                     CordbType *type,
                     DebuggerIPCE_ObjectData *pObjectData );
    virtual ~CordbObjectValue();


    virtual void Neuter(NeuterTicket ticket);
#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbObjectValue"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugValue
    //-----------------------------------------------------------

    COM_METHOD GetType(CorElementType *pType);
    COM_METHOD GetSize(ULONG32 *pSize);
    COM_METHOD GetAddress(CORDB_ADDRESS *pAddress);
    COM_METHOD CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint);

    //-----------------------------------------------------------
    // ICorDebugValue2
    //-----------------------------------------------------------

    COM_METHOD GetExactType(ICorDebugType **ppType)
    {
        return (CordbValue::GetExactType(ppType));
    }


    //-----------------------------------------------------------
    // ICorDebugHeapValue
    //-----------------------------------------------------------

    COM_METHOD IsValid(BOOL *pbValid);
    COM_METHOD CreateRelocBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint);

    //-----------------------------------------------------------
    // ICorDebugHeapValue2
    //-----------------------------------------------------------
    COM_METHOD CreateHandle(CorDebugHandleType type, ICorDebugHandleValue ** ppHandle);

    //-----------------------------------------------------------
    // ICorDebugObjectValue
    //-----------------------------------------------------------

    COM_METHOD GetClass(ICorDebugClass **ppClass);
    COM_METHOD GetFieldValue(ICorDebugClass *pClass,
                             mdFieldDef fieldDef,
                             ICorDebugValue **ppValue);
    COM_METHOD GetVirtualMethod(mdMemberRef memberRef,
                                ICorDebugFunction **ppFunction);
    COM_METHOD GetContext(ICorDebugContext **ppContext);
    COM_METHOD IsValueClass(BOOL *pbIsValueClass);
    COM_METHOD GetManagedCopy(IUnknown **ppObject);
    COM_METHOD SetFromManagedCopy(IUnknown *pObject);

    COM_METHOD GetFieldValueForType(ICorDebugType *pType,
                             mdFieldDef fieldDef,
                             ICorDebugValue **ppValue);

    COM_METHOD GetVirtualMethodAndType(mdMemberRef memberRef,
                       ICorDebugFunction **ppFunction,
                       ICorDebugType **ppType);

    //-----------------------------------------------------------
    // ICorDebugGenericValue
    //-----------------------------------------------------------

    COM_METHOD GetValue(void *pTo);
    COM_METHOD SetValue(void *pFrom);

    //-----------------------------------------------------------
    // ICorDebugStringValue
    //-----------------------------------------------------------
    COM_METHOD GetLength(ULONG32 *pcchString);
    COM_METHOD GetString(ULONG32 cchString,
                         ULONG32 *ppcchStrin,
                         WCHAR szString[]);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    HRESULT Init(void);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

protected:
    DebuggerIPCE_ObjectData  m_info;
    BYTE                    *m_objectCopy;
    BYTE                    *m_objectLocalVars; // var base in _this_ process
                                                // points _into_ m_objectCopy
    BYTE                    *m_stringBuffer;    // points _into_ m_objectCopy

    CordbSyncBlockFieldTable m_syncBlockFieldsInstance;

};


/* ------------------------------------------------------------------------- *
 * Value Class Object Value class
 * ------------------------------------------------------------------------- */

class CordbVCObjectValue : public CordbValue,
                           public ICorDebugObjectValue, public ICorDebugObjectValue2,
                           public ICorDebugGenericValue, public ICorDebugValue2
{
public:
    CordbVCObjectValue(CordbAppDomain *appdomain,
                       CordbType *type,
                       REMOTE_PTR remoteAddress,
                       void *localAddress,
                       RemoteAddress *remoteRegAddr);
    virtual ~CordbVCObjectValue();

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbVCObjectValue"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugValue
    //-----------------------------------------------------------

    COM_METHOD GetType(CorElementType *pType);

    COM_METHOD GetSize(ULONG32 *pSize)
    {
        return (CordbValue::GetSize(pSize));
    }
    COM_METHOD GetAddress(CORDB_ADDRESS *pAddress)
    {
        return (CordbValue::GetAddress(pAddress));
    }
    COM_METHOD CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint)
    {
        return (CordbValue::CreateBreakpoint(ppBreakpoint));
    }

    //-----------------------------------------------------------
    // ICorDebugValue2
    //-----------------------------------------------------------

    COM_METHOD GetExactType(ICorDebugType **ppType)
    {
        return (CordbValue::GetExactType(ppType));
    }

    //-----------------------------------------------------------
    // ICorDebugObjectValue
    //-----------------------------------------------------------

    COM_METHOD GetClass(ICorDebugClass **ppClass);
    COM_METHOD GetFieldValue(ICorDebugClass *pClass,
                             mdFieldDef fieldDef,
                             ICorDebugValue **ppValue);
    COM_METHOD GetVirtualMethod(mdMemberRef memberRef,
                                ICorDebugFunction **ppFunction);
    COM_METHOD GetContext(ICorDebugContext **ppContext);
    COM_METHOD IsValueClass(BOOL *pbIsValueClass);
    COM_METHOD GetManagedCopy(IUnknown **ppObject);
    COM_METHOD SetFromManagedCopy(IUnknown *pObject);
    COM_METHOD GetFieldValueForType(ICorDebugType *pType,
                                    mdFieldDef fieldDef,
                                    ICorDebugValue **ppValue);
    COM_METHOD GetVirtualMethodAndType(mdMemberRef memberRef,
                                       ICorDebugFunction **ppFunction,
                                       ICorDebugType **ppType);

    //-----------------------------------------------------------
    // ICorDebugGenericValue
    //-----------------------------------------------------------

    COM_METHOD GetValue(void *pTo);
    COM_METHOD SetValue(void *pFrom);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    HRESULT Init(void);
    //HRESULT ResolveValueClass(void);
    CordbClass *GetClass();

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

private:
    BYTE       *m_objectCopy;
};


/* ------------------------------------------------------------------------- *
 * Box Value class
 * ------------------------------------------------------------------------- */

class CordbBoxValue : public CordbValue, public ICorDebugBoxValue,
                      public ICorDebugGenericValue, public ICorDebugValue2, public ICorDebugHeapValue2
{
public:
    CordbBoxValue(CordbAppDomain *appdomain,
                  CordbType *type,
                  REMOTE_PTR remoteAddress,
                  SIZE_T objectSize,
                  SIZE_T offsetToVars);
    virtual ~CordbBoxValue();

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbBoxValue"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugValue
    //-----------------------------------------------------------

    COM_METHOD GetType(CorElementType *pType);

    COM_METHOD GetSize(ULONG32 *pSize)
    {
        return (CordbValue::GetSize(pSize));
    }
    COM_METHOD GetAddress(CORDB_ADDRESS *pAddress)
    {
        return (CordbValue::GetAddress(pAddress));
    }
    COM_METHOD CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint)
    {
        return (CordbValue::CreateBreakpoint(ppBreakpoint));
    }

    //-----------------------------------------------------------
    // ICorDebugValue2
    //-----------------------------------------------------------

    COM_METHOD GetExactType(ICorDebugType **ppType)
    {
        return (CordbValue::GetExactType(ppType));
    }


    //-----------------------------------------------------------
    // ICorDebugHeapValue
    //-----------------------------------------------------------

    COM_METHOD IsValid(BOOL *pbValid);
    COM_METHOD CreateRelocBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint);

    //-----------------------------------------------------------
    // ICorDebugHeapValue2
    //-----------------------------------------------------------
    COM_METHOD CreateHandle(CorDebugHandleType type, ICorDebugHandleValue ** ppHandle);

    //-----------------------------------------------------------
    // ICorDebugGenericValue
    //-----------------------------------------------------------

    COM_METHOD GetValue(void *pTo);
    COM_METHOD SetValue(void *pFrom);

    //-----------------------------------------------------------
    // ICorDebugBoxValue
    //-----------------------------------------------------------
    COM_METHOD GetObject(ICorDebugObjectValue **ppObject);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    HRESULT Init(void);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

private:
    SIZE_T      m_offsetToVars;

};

/* ------------------------------------------------------------------------- *
 * Array Value class
 * ------------------------------------------------------------------------- */

class CordbArrayValue : public CordbValue, public ICorDebugArrayValue,
                        public ICorDebugGenericValue, public ICorDebugValue2, public ICorDebugHeapValue2
{
public:
    CordbArrayValue(CordbAppDomain *appdomain,
                    CordbType *type,
                    DebuggerIPCE_ObjectData *pObjectInfo);
    virtual ~CordbArrayValue();

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbArrayValue"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugValue
    //-----------------------------------------------------------

    COM_METHOD GetType(CorElementType *pType)
    {
        return (CordbValue::GetType(pType));
    }
    COM_METHOD GetSize(ULONG32 *pSize)
    {
        return (CordbValue::GetSize(pSize));
    }
    COM_METHOD GetAddress(CORDB_ADDRESS *pAddress)
    {
        return (CordbValue::GetAddress(pAddress));
    }
    COM_METHOD CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint)
    {
        return (CordbValue::CreateBreakpoint(ppBreakpoint));
    }

    //-----------------------------------------------------------
    // ICorDebugValue2
    //-----------------------------------------------------------

    COM_METHOD GetExactType(ICorDebugType **ppType)
    {
        return (CordbValue::GetExactType(ppType));
    }


    //-----------------------------------------------------------
    // ICorDebugHeapValue
    //-----------------------------------------------------------

    COM_METHOD IsValid(BOOL *pbValid);
    COM_METHOD CreateRelocBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint);

    //-----------------------------------------------------------
    // ICorDebugHeapValue2
    //-----------------------------------------------------------
    COM_METHOD CreateHandle(CorDebugHandleType type, ICorDebugHandleValue ** ppHandle);

    //-----------------------------------------------------------
    // ICorDebugArrayValue
    //-----------------------------------------------------------

    COM_METHOD GetElementType(CorElementType *pType);
    COM_METHOD GetRank(ULONG32 *pnRank);
    COM_METHOD GetCount(ULONG32 *pnCount);
    COM_METHOD GetDimensions(ULONG32 cdim, ULONG32 dims[]);
    COM_METHOD HasBaseIndicies(BOOL *pbHasBaseIndicies);
    COM_METHOD GetBaseIndicies(ULONG32 cdim, ULONG32 indicies[]);
    COM_METHOD GetElement(ULONG32 cdim, ULONG32 indicies[],
                          ICorDebugValue **ppValue);
    COM_METHOD GetElementAtPosition(ULONG32 nIndex,
                                    ICorDebugValue **ppValue);

    //-----------------------------------------------------------
    // ICorDebugGenericValue
    //-----------------------------------------------------------

    COM_METHOD GetValue(void *pTo);
    COM_METHOD SetValue(void *pFrom);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------

    HRESULT Init(void);
    HRESULT CreateElementValue(void *remoteElementPtr,
                               void *localElementPtr,
                               ICorDebugValue **ppValue);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

private:
    DebuggerIPCE_ObjectData  m_info;
    CordbType               *m_elemtype;
    BYTE                    *m_objectCopy;
    DWORD                   *m_arrayLowerBase; // points _into_ m_objectCopy
    DWORD                   *m_arrayUpperBase; // points _into_ m_objectCopy
    unsigned int             m_idxLower; // index of Lower bound of data
    unsigned int             m_idxUpper; // index of Upper bound of data

};

class CordbHandleValue : public CordbValue, public ICorDebugHandleValue, public ICorDebugValue2
{
public:
    CordbHandleValue(CordbAppDomain *appdomain,
                     CordbType *type,
                     CorDebugHandleType handleType);
    HRESULT Init(LSPTR_OBJECTHANDLE pHandle);

    virtual ~CordbHandleValue();

    virtual void Neuter(NeuterTicket ticket);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbHandleValue"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugHandleValue interface
    //-----------------------------------------------------------
    COM_METHOD GetHandleType(CorDebugHandleType *pType);


    /*
      * The final release of the interface will also dispose of the handle. This
      * API provides the ability for client to early dispose the handle.
      *
      */
    COM_METHOD Dispose();

    //-----------------------------------------------------------
    // ICorDebugValue interface
    //-----------------------------------------------------------
    COM_METHOD GetType(CorElementType *pType);
    COM_METHOD GetSize(ULONG32 *pSize);
    COM_METHOD GetAddress(CORDB_ADDRESS *pAddress);
    COM_METHOD CreateBreakpoint(ICorDebugValueBreakpoint **ppBreakpoint);

    //-----------------------------------------------------------
    // ICorDebugValue2
    //-----------------------------------------------------------

    COM_METHOD GetExactType(ICorDebugType **ppType)
    {
        // If AppDomain is already unloaded, return error
        if (m_appdomain->IsNeutered() == TRUE)
        {
            return COR_E_APPDOMAINUNLOADED;
        }
        if (m_pHandle == NULL)
        {
            return CORDBG_E_HANDLE_HAS_BEEN_DISPOSED;
        }

        return (CordbValue::GetExactType(ppType));
    }

    //-----------------------------------------------------------
    // ICorDebugReferenceValue interface
    //-----------------------------------------------------------

    COM_METHOD IsNull(BOOL *pbNull);
    COM_METHOD GetValue(CORDB_ADDRESS *pValue);
    COM_METHOD SetValue(CORDB_ADDRESS value);
    COM_METHOD Dereference(ICorDebugValue **ppValue);
    COM_METHOD DereferenceStrong(ICorDebugValue **ppValue);


private:
    //BOOL RefreshHandleValue(void **pObjectToken);
    HRESULT RefreshHandleValue();

    LSPTR_OBJECTHANDLE  m_pHandle;         // EE object handle pointer. Can be casted to OBJECTHANDLE when go to LS
    BOOL                m_fCanBeValid;      // true if object "can" be valid. False when object is no longer valid.
    CorDebugHandleType m_handleType;        // handle type can be strong or weak
    DebuggerIPCE_ObjectData  m_info;
; // ICORDebugClass of this object when we create the handle
};

/* ------------------------------------------------------------------------- *
 * Eval class
 * ------------------------------------------------------------------------- */

class CordbEval : public CordbBase, public ICorDebugEval, public ICorDebugEval2
{
public:
    CordbEval(CordbThread* pThread);
    virtual ~CordbEval();

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbEval"; }
#endif

    virtual void Neuter(NeuterTicket ticket);

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorDebugEval
    //-----------------------------------------------------------

    COM_METHOD CallFunction(ICorDebugFunction *pFunction,
                            ULONG32 nArgs,
                            ICorDebugValue *ppArgs[]);
    COM_METHOD NewObject(ICorDebugFunction *pConstructor,
                         ULONG32 nArgs,
                         ICorDebugValue *ppArgs[]);
    COM_METHOD NewObjectNoConstructor(ICorDebugClass *pClass);
    COM_METHOD NewString(LPCWSTR string);
    COM_METHOD NewArray(CorElementType elementType,
                        ICorDebugClass *pElementClass,
                        ULONG32 rank,
                        ULONG32 dims[],
                        ULONG32 lowBounds[]);
    COM_METHOD IsActive(BOOL *pbActive);
    COM_METHOD Abort(void);
    COM_METHOD GetResult(ICorDebugValue **ppResult);
    COM_METHOD GetThread(ICorDebugThread **ppThread);
    COM_METHOD CreateValue(CorElementType elementType,
                           ICorDebugClass *pElementClass,
                           ICorDebugValue **ppValue);
    COM_METHOD NewStringWithLength(LPCWSTR string, UINT uiLength);

    COM_METHOD CallParameterizedFunction(ICorDebugFunction *pFunction,
                                         ULONG32 nTypeArgs,
                                         ICorDebugType *ppTypeArgs[],
                                         ULONG32 nArgs,
                                         ICorDebugValue *ppArgs[]);

    COM_METHOD CreateValueForType(ICorDebugType *pType,
                                  ICorDebugValue **ppValue);

    COM_METHOD NewParameterizedObject(ICorDebugFunction *pConstructor,
                                      ULONG32 nTypeArgs,
                                      ICorDebugType *ppTypeArgs[],
                                      ULONG32 nArgs,
                                      ICorDebugValue *ppArgs[]);

    COM_METHOD NewParameterizedObjectNoConstructor(ICorDebugClass *pClass,
                                                   ULONG32 nTypeArgs,
                                                   ICorDebugType *ppTypeArgs[]);

    COM_METHOD NewParameterizedArray(ICorDebugType *pElementType,
                                     ULONG32 rank,
                                     ULONG32 dims[],
                                     ULONG32 lowBounds[]);

    //-----------------------------------------------------------
    // ICorDebugEval2
    //-----------------------------------------------------------

    COM_METHOD RudeAbort(void);

    //-----------------------------------------------------------
    // Non-COM methods
    //-----------------------------------------------------------
    HRESULT GatherArgInfo(ICorDebugValue *pValue,
                          DebuggerIPCE_FuncEvalArgData *argData);
    HRESULT SendCleanup(void);

    //-----------------------------------------------------------
    // Data members
    //-----------------------------------------------------------

    bool IsEvalDuringException() { return m_evalDuringException; }
private:
    // We must keep a strong reference to the thread so we can properly fail out of SendCleanup if someone releases an
    // ICorDebugEval after the process has completely gone away.                            
    RSSmartPtr<CordbThread>    m_thread;
    
    CordbFunction             *m_function;
    CordbClass                *m_class;
    DebuggerIPCE_FuncEvalType  m_evalType;

    HRESULT SendFuncEval(unsigned int genericArgsCount, ICorDebugType *genericArgs[], void *argData1, unsigned int argData1Size, void *argData2, unsigned int argData2Size, DebuggerIPCEvent * event);
    HRESULT FilterHR(HRESULT hr);
    BOOL DoAppDomainsMatch( CordbAppDomain* pAppDomain, ULONG32 nTypes, ICorDebugType *pTypes[], ULONG32 nValues, ICorDebugValue *pValues[] );

public:
    bool                       m_complete;
    bool                       m_successful;
    bool                       m_aborted;
    void                      *m_resultAddr;
    LSPTR_OBJECTHANDLE         m_objectHandle;          // This is an OBJECTHANDLE on the LS if func-eval creates a strong handle
    RSExtSmartPtr<CordbHandleValue> m_pHandleValue;          // This is the corresponding cached CordbHandleValue for GetResult
    DebuggerIPCE_ExpandedTypeData m_resultType;
    LSPTR_APPDOMAIN            m_resultAppDomainToken;
    LSPTR_DEBUGGEREVAL         m_debuggerEvalKey;


    // If we're evalling during a thread's exception, remember the info so that we can restore it when we're done.
    bool                       m_evalDuringException;     // flag whether we're during the thread's exception.
    LSPTR_OBJECTHANDLE         m_ThreadOldExceptionHandle; // object handle for thread's managed exception object.

#ifdef _DEBUG
    // Func-eval should perturb the the thread's current appdomain. So we remember it at start
    // and then ensure that the func-eval complete restores it.
    CordbAppDomain *           m_DbgAppDomainStarted;
#endif
};


/* ------------------------------------------------------------------------- *
 * Win32 Event Thread class
 * ------------------------------------------------------------------------- */
const unsigned int CW32ET_UNKNOWN_PROCESS_SLOT = 0xFFffFFff; // it's a managed process,
        //but we don't know which slot it's in - for Detach.

class CordbWin32EventThread
{
    friend class CordbProcess; //so that Detach can call ExitProcess
public:
    CordbWin32EventThread(Cordb* cordb);
    virtual ~CordbWin32EventThread();

    //
    // You create a new instance of this class, call Init() to set it up,
    // then call Start() start processing events. Stop() terminates the
    // thread and deleting the instance cleans all the handles and such
    // up.
    //
    HRESULT Init(void);
    HRESULT Start(void);
    HRESULT Stop(void);

    HRESULT SendCreateProcessEvent(LPCWSTR programName,
                                   __in_z LPWSTR  programArgs,
                                   LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                   LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                   BOOL bInheritHandles,
                                   DWORD dwCreationFlags,
                                   PVOID lpEnvironment,
                                   LPCWSTR lpCurrentDirectory,
                                   LPSTARTUPINFOW lpStartupInfo,
                                   LPPROCESS_INFORMATION lpProcessInformation,
                                   CorDebugCreateProcessFlags corDebugFlags);

    HRESULT SendDebugActiveProcessEvent(DWORD pid,
                                        bool fWin32Attach,
                                        CordbProcess *pProcess);
    HRESULT SendDetachProcessEvent(CordbProcess *pProcess);

    void LockSendToWin32EventThreadMutex(void)
    {
        LOG((LF_CORDB, LL_INFO10000, "W32ET::LockSendToWin32EventThreadMutex\n"));
        m_sendToWin32EventThreadMutex.Lock();
    }

    void UnlockSendToWin32EventThreadMutex(void)
    {
        m_sendToWin32EventThreadMutex.Unlock();
        LOG((LF_CORDB, LL_INFO10000, "W32ET::UnlockSendToWin32EventThreadMutex\n"));
    }

    bool IsWin32EventThread(void)
    {
        return (m_threadId == GetCurrentThreadId());
    }

    void Win32EventLoop(void);

private:
    void ThreadProc(void);
    static DWORD WINAPI ThreadProc(LPVOID parameter);

    void CreateProcess(void);

    //
    // EnsureCorDbgEnvVarSet makes sure that a user supplied
    // environment block contains the proper environment variable to
    // enable debugging on the Left Side.
    // On success: returns true, and *ppEnv points to a newly allocated buffer which caller must free.
    // On failure, returns false.
    //
    template<class T> bool EnsureCorDbgEnvVarSet(const T **ppEnv,
                                                 const T *varName,
                                                 bool isUnicode,
                                                 DWORD flag)
    {
        _ASSERTE(ppEnv != NULL);
        CONSISTENCY_CHECK_MSGF((flag == 1),
                               ("EnsureCorDbgEnvVarSet needs to be updated to set environment variables to values other than 1"));

        if (isUnicode)
        {
            _ASSERTE(sizeof(T) == sizeof(WCHAR));
        }
        else
        {
            _ASSERTE(sizeof(T) == sizeof(CHAR));
        }

        const T *pEnv = *ppEnv;

        // Nothing to do if there is no user supplied env block since
        // the initialization of Cordb set the env var in this
        // process's env block.
        if (pEnv == NULL)
        {
            return false;
        }

        // Find where the env var should be in the block
        const T *p = pEnv;
        const T *lowEnd = NULL;
        const T *ourVar = NULL;
        SIZE_T varNameLen;

        if (isUnicode)
        {
            varNameLen = wcslen((WCHAR*)varName);
        }
        else
        {
            varNameLen = strlen((CHAR*)varName);
        }

        while (*p)
        {
            int res;

            if (isUnicode)
            {
                res = _wcsnicmp((WCHAR*)p, (WCHAR*)varName, varNameLen);
            }
            else
            {
                res = _strnicmp((CHAR*)p, (CHAR*)varName, varNameLen);
            }

            // It seems the environment block is only sorted on NT, so
            // we only look for our var in sorted position on NT.

            if (res == 0)
            {
                // Found it. lowEnd should point to the end of the
                // last var already. Remember where the good var is
                // and skip over it to find the next highest one.
                ourVar = p;
                while (*p++)
                {
                    // do nothing
                }
                break;
            }
            else if ((res < 0) || !RunningOnWinNT())
            {
                // Skip over this var since its smaller than ours
                while (*p++)
                {
                    // do nothing
                }

                // Remember the first char past the end
                lowEnd = p;
            }
            else if (res > 0)
            {
                // This var is too big. lowEnd still points to the end
                // of the last smaller var.
                break;
            }
        }

        // Remember where the high part starts.
        const T *highStart = p;

        if (ourVar == NULL)
        {
            _ASSERTE(lowEnd == highStart);

            // At this point, we know that p is pointing to the first character before which
            // varname should be inserted.  If ourvar != NULL, then p points to the first
            // character of the next variable.
            //
            // In the case that we're at the end of the environment block, then p points to
            // the second NULL that terminates the block.
            //

            if (*p)
            {
                //
                // We are in a situation where p points to the start of the env var we
                // are looking for.  Here we advance the pointer to the end of the block
                // by looking for the double null termination.
                //
                while (*p || *(p+1))
                {
                    p++;
                }

                // Advance p to point to just after the last character of the block
                p += 2;
            }
            else
            {
                p++;
            }

            //
            // 'p' now points to the character just after the last in the block.  It is
            // unsafe to dereference it from this point forward.
            //

            // Since pEnv points to the first character of the environment block and
            // p points to the first non-block character, p-pEnv is the total size in
            // characters of the block.  Add the size of the variable plus 2 for the
            // value and null char
            SIZE_T totalLen = ((p - pEnv) + (varNameLen + 2)); // Add 2 here, one for the value and one for the NULL

            // Allocate a new buffer.
            T *newEnv = new (nothrow) T[totalLen];

            if (newEnv == NULL)
            {
                return false;
            }

            T *p2 = newEnv;

            // Copy the low part in
            if (lowEnd != NULL)
            {
                memcpy(p2, pEnv, (lowEnd - pEnv) * sizeof(T));
                p2 += (lowEnd - pEnv);
            }

            // Copy in our env var and a null terminator (wcs/strcopy also copies in the null)
            if (isUnicode)
            {
                wcscpy_s((WCHAR*)p2,
                         varNameLen + 1,  // add one for the NULL termination to be copied in.
                         (WCHAR*)varName);
            }
            else
            {
                strcpy_s((CHAR*)p2,
                         varNameLen + 1,  // add one for the NULL termination to be copied in.
                         (CHAR*)varName);
            }

            // Advance p2
            p2 += varNameLen; // Do not add 1 here because we want to over-write the NULL with the value.

            // Assign a default value
            if (isUnicode)
            {
                wcscpy_s((WCHAR*)p2, 2, L"1");
            }
            else
            {
                strcpy_s((CHAR*)p2, 2, "1");
            }

            // Advance past the single-character default value and terminating NULL
            p2 += 2;

            // Copy in the high part. Note: the high part has both the
            // null terminator for the last string and the null
            // termination for the entire block on it. Thus, the +3
            // instead of +2. Also, because of this, the high part is
            // never empty.
            memcpy(p2, highStart, (p - highStart) * sizeof(T));

            // Assert that we didn't go overboard here...
            _ASSERTE(((p2 + (p - highStart)) - newEnv) == (int) totalLen);

            *ppEnv = newEnv;

            return true;
        }
        else
        {
            // Make a copy of the block.
            
            const T * pEnd = pEnv;
            while(*pEnd != L'\0' || *(pEnd+1) != L'\0') 
            {
                pEnd++;
            }
            size_t len = (pEnd - pEnv + 2);
            T *newEnv = new (nothrow) T[len];
            if (newEnv == NULL)
            {
                return false;
            }
            memcpy(newEnv, pEnv, len * sizeof(T));
            
            *ppEnv = newEnv;
            
        
            // Found our var. So just make sure that the value
            // includes DBCF_GENERATE_DEBUG_CODE. Note: in order to
            // ensure that we'll never have to increase the size of
            // the environment block if our var is already in there,
            // we make sure that DBCF_GENERATE_DEBUG_CODE == 1 so that
            // we only have to toggle the low bit of the value.
            _ASSERTE(DBCF_GENERATE_DEBUG_CODE == 0x01);

            // Pointer to the last digit of the value
            T *pValue = (highStart - 2) - pEnv + newEnv;

            // Set the low bit of the last digit and replace it.
            if (IS_DIGIT(*pValue))
            {
                unsigned int v = DIGIT_TO_INT(*pValue);
                v |= flag;

                _ASSERTE(v <= 9);

                *pValue = (T)(INT_TO_DIGIT(v));
            }
            else
            {
                unsigned int v;

                _ASSERTE(IS_HEXDIGIT(*pValue));

                v = HEXDIGIT_TO_INT(*pValue);

                v |= flag;

                _ASSERTE(v <= 15);

                *pValue = (T)(INT_TO_HEXDIGIT(v));
            }

            return true;
        }
    }




    void AttachProcess(void);
    void ExitProcess(bool fDetach);

private:
    RSSmartPtr<Cordb>    m_cordb;

    HANDLE               m_thread;
    DWORD                m_threadId;
    HANDLE               m_threadControlEvent;
    HANDLE               m_actionTakenEvent;
    BOOL                 m_run;

    // The process that we're 1:1 with.
    // This is set when we get a Create / Attach event.
    // This is only used on the W32ET, which guarantees it will free of races.
    RSSmartPtr<CordbProcess> m_pProcess;

    RSLock               m_sendToWin32EventThreadMutex;

    unsigned int         m_action;
    HRESULT              m_actionResult;
    union
    {
        struct
        {
            LPCWSTR programName;
            LPWSTR  programArgs;
            LPSECURITY_ATTRIBUTES lpProcessAttributes;
            LPSECURITY_ATTRIBUTES lpThreadAttributes;
            BOOL bInheritHandles;
            DWORD dwCreationFlags;
            PVOID lpEnvironment;
            LPCWSTR lpCurrentDirectory;
            LPSTARTUPINFOW lpStartupInfo;
            LPPROCESS_INFORMATION lpProcessInformation;
            CorDebugCreateProcessFlags corDebugFlags;
        } createData;

        struct
        {
            DWORD           processId;
            CordbProcess    *pProcess;
        } attachData;

        struct
        {
            CordbProcess    *pProcess;
        } detachData;

        struct
        {
            CordbProcess *process;
            EUMContinueType eContType;
        } continueData;
    }                    m_actionData;
};


// Thread-safe stack which.
template <typename T>
class InterlockedStack
{
public:
    InterlockedStack();
    ~InterlockedStack();

    // Thread safe pushes + pops.
    // Many threads can push simultaneously.
    // Only 1 thread can pop.
    void Push(T * pItem);
    T * Pop();

protected:
    T * m_pHead;
};

//-----------------------------------------------------------------------------
// Workitem to be placed on RCET worker queue.
// There's 1 RCET for to be shared by all processes.
//-----------------------------------------------------------------------------
class RCETWorkItem
{
public:

    // Item is executed and then removed from the list and deleted.
    virtual void Do() = 0;

    CordbProcess * GetProcess() { return m_pProcess; }

protected:
    RCETWorkItem(CordbProcess * pProcess)
    {
        m_pProcess.Assign(pProcess);
        m_next = NULL;
    }

    RSSmartPtr<CordbProcess> m_pProcess;

    // This field is accessed by the InterlockedStack.
    friend class InterlockedStack<RCETWorkItem>;
    RCETWorkItem * m_next;
};


// Item to do Neutering work on ExitProcess.
class ExitProcessWorkItem : public RCETWorkItem
{
public:
    ExitProcessWorkItem(CordbProcess * pProc) : RCETWorkItem(pProc)
    {
    }

    virtual void Do();
};

// Item to do send Attach event.
class SendAttachProcessWorkItem : public RCETWorkItem
{
public:
    SendAttachProcessWorkItem(CordbProcess * pProc) : RCETWorkItem(pProc)
    {
    }

    virtual void Do();
};


/* ------------------------------------------------------------------------- *
 * Runtime Controller Event Thread class
 * ------------------------------------------------------------------------- */

class CordbRCEventThread
{
public:
    CordbRCEventThread(Cordb* cordb);
    virtual ~CordbRCEventThread();

    //
    // You create a new instance of this class, call Init() to set it up,
    // then call Start() start processing events. Stop() terminates the
    // thread and deleting the instance cleans all the handles and such
    // up.
    //
    HRESULT Init(void);
    HRESULT Start(void);
    HRESULT Stop(void);

    // RCET will take ownership of this item and delete it.
    void QueueAsyncWorkItem(RCETWorkItem * pItem);

    HRESULT SendIPCEvent(CordbProcess* process,
                         DebuggerIPCEvent* event,
                         SIZE_T eventSize);

    void ProcessStateChanged(void);
    void FlushQueuedEvents(CordbProcess* process);

    HRESULT WaitForIPCEventFromProcess(CordbProcess* process,
                                       CordbAppDomain *pAppDomain,
                                       DebuggerIPCEvent* event);

    HRESULT ReadRCEvent(CordbProcess* process,
                        DebuggerIPCEvent* event);
    void CopyRCEvent(BYTE *src, BYTE *dst);

    bool IsRCEventThread(void);

private:
    void DrainWorkerQueue();

    void ThreadProc(void);
    static DWORD WINAPI ThreadProc(LPVOID parameter);
    HRESULT HandleFirstRCEvent(CordbProcess* process);
    void HandleRCEvent(CordbProcess* process,
                       DebuggerIPCEvent* event);

private:
    InterlockedStack<class RCETWorkItem> m_WorkerStack;

    RSSmartPtr<Cordb>    m_cordb;
    HANDLE               m_thread;
    DWORD                m_threadId;
    BOOL                 m_run;
    HANDLE               m_threadControlEvent;
    BOOL                 m_processStateChanged;
};

/* ------------------------------------------------------------------------- *
 * Unmanaged Event struct
 * ------------------------------------------------------------------------- */

enum CordbUnmanagedEventState
{
    CUES_None                 = 0x00,
    CUES_ExceptionCleared     = 0x01,
    CUES_EventContinued       = 0x02,
    CUES_Dispatched           = 0x04,
    CUES_ExceptionUnclearable = 0x08
};

struct CordbUnmanagedEvent
{
public:
    BOOL IsExceptionCleared(void) { return m_state & CUES_ExceptionCleared; }
    BOOL IsEventContinued(void) { return m_state & CUES_EventContinued; }
    BOOL IsDispatched(void) { return m_state & CUES_Dispatched; }
    BOOL IsExceptionUnclearable(void) { return m_state & CUES_ExceptionUnclearable; }

    void SetState(CordbUnmanagedEventState state) { m_state = (CordbUnmanagedEventState)(m_state | state); }
    void ClearState(CordbUnmanagedEventState state) { m_state = (CordbUnmanagedEventState)(m_state & ~state); }

    CordbUnmanagedThread     *m_owner;
    CordbUnmanagedEventState  m_state;
    DEBUG_EVENT               m_currentDebugEvent;
    CordbUnmanagedEvent      *m_next;
};


/* ------------------------------------------------------------------------- *
 * Unmanaged Thread class
 * ------------------------------------------------------------------------- */

enum CordbUnmanagedThreadState
{
    CUTS_None                        = 0x0000,
    CUTS_Deleted                     = 0x0001,
    CUTS_FirstChanceHijacked         = 0x0002,
    CUTS_HideFirstChanceHijackState  = 0x0004,
    CUTS_GenericHijacked             = 0x0008,
    CUTS_SecondChanceHijacked        = 0x0010,
    CUTS_HijackedForSync             = 0x0020,
    CUTS_Suspended                   = 0x0040,
    CUTS_IsSpecialDebuggerThread     = 0x0080,
    CUTS_HasIBEvent                  = 0x0200,
    CUTS_HasOOBEvent                 = 0x0400,
    CUTS_HasSpecialStackOverflowCase = 0x0800,
#ifdef _DEBUG
    CUTS_DEBUG_SingleStep            = 0x1000,
#endif
    CUTS_SkippingNativePatch         = 0x2000,
    CUTS_HasContextSet               = 0x4000
};

class CordbUnmanagedThread : public CordbBase
{
public:
    CordbUnmanagedThread(CordbProcess *pProcess, DWORD dwThreadId, HANDLE hThread, void *lpThreadLocalBase);
    ~CordbUnmanagedThread();

    using CordbBase::GetProcess;

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbUnmanagedThread"; }
#endif


    // CordbUnmanagedThread is a purely internal object. It's not exposed via ICorDebug APIs and so
    // we should never use External AddRef.
    ULONG STDMETHODCALLTYPE AddRef() { _ASSERTE(!"Don't use external addref on a CordbUnmanagedThread"); return (BaseAddRef());}
    ULONG STDMETHODCALLTYPE Release() { _ASSERTE(!"Don't use external release on a CordbUnmanagedThread"); return (BaseRelease());}

    COM_METHOD QueryInterface(REFIID riid, void **ppInterface)
    {
        _ASSERTE(!"Don't use QI on a CordbUnmanagedThread");
        // Not really used since we never expose this class. If we ever do expose this class via the ICorDebug API then
        // we should, of course, implement this.
        return E_NOINTERFACE;
    }


    BOOL IsDeleted(void) {LEAF_CONTRACT;  return m_state & CUTS_Deleted; }
    BOOL IsFirstChanceHijacked(void) {LEAF_CONTRACT;  return m_state & CUTS_FirstChanceHijacked; }
    BOOL IsHideFirstChanceHijackState(void) { return m_state & CUTS_HideFirstChanceHijackState; }
    BOOL IsGenericHijacked(void) {LEAF_CONTRACT;  return m_state & CUTS_GenericHijacked; }
    BOOL IsSecondChanceHijacked(void) {LEAF_CONTRACT;  return m_state & CUTS_SecondChanceHijacked; }
    BOOL IsHijackedForSync(void) {LEAF_CONTRACT;  return m_state & CUTS_HijackedForSync; }
    BOOL IsSuspended(void) {LEAF_CONTRACT;  return m_state & CUTS_Suspended; }
    BOOL IsSpecialDebuggerThread(void) {LEAF_CONTRACT;  return m_state & CUTS_IsSpecialDebuggerThread; }
    BOOL HasIBEvent(void) {LEAF_CONTRACT;  return m_state & CUTS_HasIBEvent; }
    BOOL HasOOBEvent(void) { return m_state & CUTS_HasOOBEvent; }
    BOOL HasSpecialStackOverflowCase(void) {LEAF_CONTRACT;  return m_state & CUTS_HasSpecialStackOverflowCase; }
#ifdef _DEBUG
    BOOL IsDEBUGTrace() { return m_state & CUTS_DEBUG_SingleStep; }
#endif
    BOOL IsSkippingNativePatch() { LEAF_CONTRACT; return m_state & CUTS_SkippingNativePatch; }

    BOOL IsContextSet() { LEAF_CONTRACT; return m_state & CUTS_HasContextSet; }

    void SetState(CordbUnmanagedThreadState state) 
    {
        LEAF_CONTRACT;  
        m_state = (CordbUnmanagedThreadState)(m_state | state); 
        _ASSERTE(!IsSuspended() || !IsHijackedForSync());
        _ASSERTE(!IsSuspended() || !IsFirstChanceHijacked());
    }
    void ClearState(CordbUnmanagedThreadState state) {LEAF_CONTRACT;  m_state = (CordbUnmanagedThreadState)(m_state & ~state); }

    CordbUnmanagedEvent *IBEvent(void)  {LEAF_CONTRACT;  return &m_IBEvent; }
    CordbUnmanagedEvent *IBEvent2(void) {LEAF_CONTRACT;  return &m_IBEvent2; }
    CordbUnmanagedEvent *OOBEvent(void) { return &m_OOBEvent; }

    DWORD GetOSTid() { return (DWORD) this->m_id; }

public:
    HANDLE                     m_handle;
    void                      *m_threadLocalBase;
    void                      *m_pTLSArray;

    CordbUnmanagedThreadState  m_state;

    CordbUnmanagedEvent        m_IBEvent;
    CordbUnmanagedEvent        m_IBEvent2;
    CordbUnmanagedEvent        m_OOBEvent;

    LSPTR_CONTEXT              m_pLeftSideContext;
    void                      *m_originalHandler;

private:
    // Spare context used for various purposes. This context is used in several different way.
    CONTEXT                    m_context;

    HRESULT EnableSSAfterBP();
    bool GetEEThreadCantStopHelper();
    HRESULT SetupFirstChanceHijack(void* pHijackProc);
    DWORD_PTR GetTlsSlot(SIZE_T slot);
    DWORD_PTR GetPreDefTlsSlot(SIZE_T slot, bool * pRead);

    void * m_pPatchSkipAddress;



    /* 
     * This abstracts away an overload of the OS thread's TLS slot. In 
     * particular the runtime may or may not have created a thread object for
     * a particular OS thread at any point.
     *
     * If the runtime has created a thread object, then it stores a pointer to
     * that thread object in the thread's TLS slot.
     *
     * If not, then interop-debugging uses that TLS slot to store temporary 
     * information.
     *
     * To determine this, interop-debugging will set the low bit.  Thus when
     * we read the TLS slot, if it is non-NULL, anything w/o the low bit set
     * is an EE thread object ptr.  Anything with the low bit set is an 
     * interop-debugging value.  Any NULL is null, and an indicator that 
     * there does not exist a runtime thread object for this thread yet.
     *
     */
    REMOTE_PTR m_pEEThread;
    DWORD_PTR m_pdwTlsValue;
    BOOL m_fValidTlsData;

    UINT m_continueCountCached;

    void CacheEEDebuggerWord(void);
    HRESULT SetEETlsValue(REMOTE_PTR EETlsValue);

public:
    HRESULT GetEEDebuggerWord(DWORD_PTR *pValue);
    HRESULT SetEEDebuggerWord(DWORD_PTR value);
    HRESULT GetEEThreadPtr(REMOTE_PTR *ppEEThread);
    
    bool GetEEPGCDisabled(void);
    void GetEEState(bool *threadStepping, bool *specialManagedException);
    bool GetEEFrame(void);

};




//********************************************************************************
//**************** App Domain Publishing Service API *****************************
//********************************************************************************

class EnumElement
{
public:
    EnumElement()
    {
        m_pData = NULL;
        m_pNext = NULL;
    }

    void SetData (void *pData) { m_pData = pData;}
    void *GetData (void) { return m_pData;}
    void SetNext (EnumElement *pNext) { m_pNext = pNext;}
    EnumElement *GetNext (void) { return m_pNext;}

private:
    void        *m_pData;
    EnumElement *m_pNext;
};


// Prototype of psapi!GetModuleFileNameEx.
typedef DWORD FPGetModuleFileNameEx(HANDLE, HMODULE, LPTSTR, DWORD);


class CorpubPublish : public CordbCommonBase, public ICorPublish
{
public:
    CorpubPublish();
    virtual ~CorpubPublish();

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbPublish"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorPublish
    //-----------------------------------------------------------

    COM_METHOD EnumProcesses(
        COR_PUB_ENUMPROCESS Type,
        ICorPublishProcessEnum **ppIEnum);

    COM_METHOD GetProcess(
        unsigned pid,
        ICorPublishProcess **ppProcess);

    //-----------------------------------------------------------
    // CreateObject
    //-----------------------------------------------------------
    static COM_METHOD CreateObject(REFIID id, void **object)
    {
        *object = NULL;

        if (id != IID_IUnknown && id != IID_ICorPublish)
            return (E_NOINTERFACE);

        CorpubPublish *pCorPub = new (nothrow) CorpubPublish();

        if (pCorPub == NULL)
            return (E_OUTOFMEMORY);

        *object = (ICorPublish*)pCorPub;
        pCorPub->AddRef();

        return (S_OK);
    }

private:
    HRESULT GetProcessInternal( unsigned pid, CorpubProcess **ppProcess );

    // Cached information to get the process name. Not available on all platforms, so may be null.
    HModuleHolder m_hPSAPIdll;
    FPGetModuleFileNameEx * m_fpGetModuleFileNameEx;
};

class CorpubProcess : public CordbCommonBase, public ICorPublishProcess
{
public:
    CorpubProcess(DWORD dwProcessId, bool fManaged, HANDLE hProcess,
        HANDLE hMutex, 
        AppDomainEnumerationIPCBlock *pAD, 
        IPCReaderInterface *pIPCReader, 
        FPGetModuleFileNameEx * fpGetModuleFileNameEx);
    virtual ~CorpubProcess();

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CorpubProcess"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorPublishProcess
    //-----------------------------------------------------------
    COM_METHOD IsManaged(BOOL *pbManaged);

    /*
     * Enumerate the list of known application domains in the target process.
     */
    COM_METHOD EnumAppDomains(ICorPublishAppDomainEnum **ppEnum);

    /*
     * Returns the OS ID for the process in question.
     */
    COM_METHOD GetProcessID(unsigned *pid);

    /*
     * Get the display name for a process.
     */
    COM_METHOD GetDisplayName(ULONG32 cchName,
                                ULONG32 *pcchName,
                                WCHAR szName[]);

    CorpubProcess   *GetNextProcess (void) { return m_pNext;}
    void SetNext (CorpubProcess *pNext) { m_pNext = pNext;}

    // Helper to tell if this process has exited
    bool IsExited();

public:
    DWORD                           m_dwProcessId;

private:
    bool                            m_fIsManaged;
    HANDLE                          m_hProcess;
    HANDLE                          m_hMutex;
    AppDomainEnumerationIPCBlock    *m_AppDomainCB;
    IPCReaderInterface              *m_pIPCReader;  // controls the lifetime of the AppDomainEnumerationIPCBlock
    CorpubProcess                   *m_pNext;   // pointer to the next process in the process list
    WCHAR                           *m_szProcessName;

};

class CorpubAppDomain  : public CordbCommonBase, public ICorPublishAppDomain
{
public:
    CorpubAppDomain (WCHAR *szAppDomainName, ULONG Id);
    virtual ~CorpubAppDomain();

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CorpubAppDomain"; }
#endif

    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface (REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorPublishAppDomain
    //-----------------------------------------------------------

    /*
     * Get the name and ID for an application domain.
     */
    COM_METHOD GetID (ULONG32 *pId);

    /*
     * Get the name for an application domain.
     */
    COM_METHOD GetName (ULONG32 cchName,
                        ULONG32 *pcchName,
                        WCHAR szName[]);

    CorpubAppDomain *GetNextAppDomain (void) { return m_pNext;}
    void SetNext (CorpubAppDomain *pNext) { m_pNext = pNext;}

private:
    CorpubAppDomain *m_pNext;
    WCHAR           *m_szAppDomainName;
    ULONG           m_id;

};

class CorpubProcessEnum : public CordbCommonBase, public ICorPublishProcessEnum
{
public:
    CorpubProcessEnum(CorpubProcess *pFirst);
    virtual ~CorpubProcessEnum();

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CorpubProcessEnum"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorPublishProcessEnum
    //-----------------------------------------------------------

    COM_METHOD Skip(ULONG celt);
    COM_METHOD Reset();
    COM_METHOD Clone(ICorPublishEnum **ppEnum);
    COM_METHOD GetCount(ULONG *pcelt);
    COM_METHOD Next(ULONG celt,
                    ICorPublishProcess *objects[],
                    ULONG *pceltFetched);

private:
    CorpubProcess       *m_pFirst;
    CorpubProcess       *m_pCurrent;

};

class CorpubAppDomainEnum : public CordbCommonBase, public ICorPublishAppDomainEnum
{
public:
    CorpubAppDomainEnum(CorpubAppDomain *pFirst);
    virtual ~CorpubAppDomainEnum();


#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbAppDomainEnum"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // ICorPublishAppDomainEnum
    //-----------------------------------------------------------
    COM_METHOD Skip(ULONG celt);
    COM_METHOD Reset();
    COM_METHOD Clone(ICorPublishEnum **ppEnum);
    COM_METHOD GetCount(ULONG *pcelt);

    COM_METHOD Next(ULONG celt,
                    ICorPublishAppDomain *objects[],
                    ULONG *pceltFetched);

private:
    CorpubAppDomain     *m_pFirst;
    CorpubAppDomain     *m_pCurrent;

};

// Since the hash table of modules is per app domain (and
// threads is per prcoess) (for fast lookup from the appdomain/proces),
// we need this wrapper
// here which allows us to iterate through an assembly's
// modules.  Is basically filters out modules/threads that aren't
// in the assembly/appdomain. This slow & awkward for assemblies, but fast
// for the common case - appdomain lookup.
class CordbEnumFilter : public CordbBase,
                        public ICorDebugThreadEnum,
                        public ICorDebugModuleEnum
{
public:
    CordbEnumFilter(CordbBase * pOwnerObj, NeuterList * pOwnerList);
    CordbEnumFilter(CordbEnumFilter*src);
    virtual ~CordbEnumFilter();

    virtual void Neuter(NeuterTicket ticket);


#ifdef _DEBUG
    virtual const char * DbgGetName() { return "CordbEnumFilter"; }
#endif


    //-----------------------------------------------------------
    // IUnknown
    //-----------------------------------------------------------

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }
    COM_METHOD QueryInterface(REFIID riid, void **ppInterface);

    //-----------------------------------------------------------
    // Common methods
    //-----------------------------------------------------------
    COM_METHOD Skip(ULONG celt);
    COM_METHOD Reset();
    COM_METHOD Clone(ICorDebugEnum **ppEnum);
    COM_METHOD GetCount(ULONG *pcelt);
    //-----------------------------------------------------------
    // ICorDebugModuleEnum
    //-----------------------------------------------------------
    COM_METHOD Next(ULONG celt,
                    ICorDebugModule *objects[],
                    ULONG *pceltFetched);

    //-----------------------------------------------------------
    // ICorDebugThreadEnum
    //-----------------------------------------------------------
    COM_METHOD Next(ULONG celt,
                    ICorDebugThread *objects[],
                    ULONG *pceltFetched);

    HRESULT Init (ICorDebugModuleEnum *pModEnum, CordbAssembly *pAssembly);
    HRESULT Init (ICorDebugThreadEnum *pThreadEnum, CordbAppDomain *pAppDomain);

private:
    // Owning object is our link to the CordbProcess* tree. Never null until we're neutered.
    // NeuterList is related to the owning object. Need to cache it so that we can pass it on
    // to our clones.
    CordbBase *     m_pOwnerObj; // provides us w/ a CordbProcess*
    NeuterList *    m_pOwnerNeuterList;


    EnumElement *m_pFirst;
    EnumElement *m_pCurrent;
    int         m_iCount;
};

// Helpers to double-check the RS results against DAC.
#if defined(_DEBUG)
void CheckAgainstDAC(CordbFunction * pFunc, void * pIP, mdMethodDef mdExpected);
#endif

// Do we take advantage of the WinXP fix that allows us suspend threads
// that just sent a debug event.
bool ShouldDoRealHijackForSync();
HRESULT CopyOutString(const WCHAR * pInputString, ULONG32 cchName, ULONG32 * pcchName, WCHAR szName[]);



inline UINT AllocCookieCordbEval(CordbProcess *pProc, CordbEval* p)
{
    _ASSERTE(pProc->GetStopGoLock()->HasLock());
    return pProc->m_EvalTable.Add(p);
}
inline CordbEval * UnwrapCookieCordbEval(CordbProcess *pProc, UINT cookie)
{
    _ASSERTE(pProc->GetStopGoLock()->HasLock());
    return pProc->m_EvalTable.LookupAndRemove(cookie);
}


// We defined this at the top of the file - undef it now so that we don't pollute other files.
#undef CRITICAL_SECTION

//-----------------------------------------------------------------------------
// Contracts for RS threading.
// We only do this for debug builds and not for inproc
//-----------------------------------------------------------------------------
#if defined(_DEBUG) && defined (_X86_)
    #define RSCONTRACTS
#endif

#ifdef RSCONTRACTS

//-----------------------------------------------------------------------------
// For debug builds, we maintain some thread-state to track debug bits
// to help us do some more aggressive asserts.
//-----------------------------------------------------------------------------

class PublicAPIHolder;
class PublicReentrantAPIHolder;
class PublicCallbackHolder;
class PublicDebuggerErrorCallbackHolder;

class DbgRSThread
{
public:
    friend class PublicAPIHolder;
    friend class PublicReentrantAPIHolder;
    friend class PublicCallbackHolder;
    friend class PublicDebuggerErrorCallbackHolder;

    DbgRSThread();

    // The TLS slot that we'll put this thread object in.
    static DWORD s_TlsSlot;

    static LONG s_Total; // Total count of thread objects

    // Get a thread object for the current thread via a TLS lookup.
    static DbgRSThread * GetThread();

    // Call during DllMain to release this.
    static DbgRSThread * Create()
    {
        InterlockedIncrement(&s_Total);

        DbgRSThread * p = new (nothrow) DbgRSThread();
        BOOL f = TlsSetValue(s_TlsSlot, p);
        _ASSERT(f);
        return p;
    }

    void Destroy()
    {
        InterlockedDecrement(&s_Total);

        BOOL f = TlsSetValue(s_TlsSlot, NULL);
        _ASSERT(f);

        delete this;
    }

    // Return true if this thread is inside the RS.
    bool IsInRS() { return m_cInsideRS > 0; }

    // Locking API..
    // These will assert if the operation is unsafe.
    void NotifyTakeLock(RSLock * pLock);
    void NotifyReleaseLock(RSLock * pLock);

    // Used to map other resources (like thread access) into the lock hierachy.
    // Note this only effects lock leveling checks and doesn't effect HoldsAnyLock().
    void TakeVirtualLock(RSLock::ERSLockLevel level);
    void ReleaseVirtualLock(RSLock::ERSLockLevel level);

    // return true if this thread is holding any RS locks. Useful to check on Public API transition boundaries.
    bool HoldsAnyLocks() { return m_cTotalLocks > 0; }

    bool IsSafeToNeuter();

    enum EThreadType
    {
        cOther,
        cW32ET
    };
    void SetThreadType(EThreadType e) { m_eThreadType = e; }

    bool IsWin32EventThread() { return m_eThreadType == cW32ET; }

    void SetUnrecoverableCallback(bool fIsUnrecoverableErrorCallback)
    {
        // Not reentrant.
        _ASSERTE(m_fIsUnrecoverableErrorCallback != fIsUnrecoverableErrorCallback);

        m_fIsUnrecoverableErrorCallback = fIsUnrecoverableErrorCallback;
    }

    inline void AssertThreadIsLockFree()
    {
        // If we're in an unrecoverable callback, we may hold locks.
        _ASSERTE(m_fIsUnrecoverableErrorCallback
            || !HoldsAnyLocks() ||
            !"Thread should not have locks on public/internal transition");
    }

protected:
    EThreadType m_eThreadType;

    // More debugging tidbits - tid that we're on, and a sanity checking cookie.
    DWORD m_tid;
    DWORD m_Cookie;

    enum ECookie
    {
        COOKIE_VALUE = 0x12345678
    };


    // This tells us if the thread is currently in the scope of a PublicAPIHolder.
    int m_cInsideRS;

    // This tells us if a thread is currently being dispatched via a callback.
    bool m_fIsInCallback;

    // We explicitly track if this thread is in an unrecoverable error callback
    // b/c that will weaken some other asserts.
    // It would be nice to clean up the unrecoverable error callback and have it
    // behave like all the other callbacks. Then we can remove this.
    bool m_fIsUnrecoverableErrorCallback;

    // Locking context. Used to tell what levels of locks we hold so we can determine if a lock is safe to take.
    int m_cLocks[RSLock::LL_MAX];
    int m_cTotalLocks;
};

//-----------------------------------------------------------------------------
// Mark when we enter / exit public APIs
//-----------------------------------------------------------------------------

// Holder for Non-reentrant Public API (this is the vast majority)
class PublicAPIHolder
{
public:
    PublicAPIHolder()
    {
        // on entry
        DbgRSThread * pThread = DbgRSThread::GetThread();
        pThread->m_cInsideRS++;
        _ASSERTE(pThread->m_cInsideRS == 1 || !"Non-reentrant API being called re-entrantly");

        // Should never be in public w/ these locks
        pThread->AssertThreadIsLockFree();
    }
    ~PublicAPIHolder() {
        // On exit.
        DbgRSThread * pThread = DbgRSThread::GetThread();
        pThread->m_cInsideRS--;
        _ASSERTE(!pThread->IsInRS());

        // Should never be in public w/ these locks. If we assert here,
        // then we're leaking locks.
        pThread->AssertThreadIsLockFree();
    }
};

// Holder for reentrant public API
class PublicReentrantAPIHolder
{
public:
    PublicReentrantAPIHolder()
    {
        // on entry
        DbgRSThread * pThread = DbgRSThread::GetThread();
        pThread->m_cInsideRS++;

        // Cache count now so that we can calidate it in the dtor.
        m_oldCount = pThread->m_cInsideRS;
        // Since a we may have been called from within the RS, we may hold locks
    }
    ~PublicReentrantAPIHolder()
    {

        // On exit.
        DbgRSThread * pThread = DbgRSThread::GetThread();

        // Ensure that our children were balanced
        _ASSERTE(pThread->m_cInsideRS == m_oldCount);

        pThread->m_cInsideRS--;
        _ASSERTE(pThread->m_cInsideRS >= 0);

        // Since a we may have been called from within the RS, we may hold locks
    }
private:
    int  m_oldCount;
};

class PublicCallbackHolder
{
public:
    PublicCallbackHolder(DebuggerIPCEventType type)
    {
        m_type = type;
        // Exiting from RS; entering Cordbg via a callback
        DbgRSThread * pThread = DbgRSThread::GetThread();
        pThread->m_cInsideRS--;
        _ASSERTE(!pThread->IsInRS());

        // Should never be in public w/ these locks. (Even if we're re-entrant.)
        pThread->AssertThreadIsLockFree();
    }

    ~PublicCallbackHolder()
    {
        // Re-entering RS from after a callback.
        DbgRSThread * pThread = DbgRSThread::GetThread();
        _ASSERTE(!pThread->IsInRS());
        pThread->m_cInsideRS++;

        // Should never be in public w/ these locks. (Even if we're re-entrant.)
        pThread->AssertThreadIsLockFree();
    }
protected:
    DebuggerIPCEventType m_type;
};

// Special holder for DebuggerError callback. This adjusts InsideRS count w/o
// verifying locks. This is very dangerous. We allow this b/c the Debugger Error callback can come at any time.
class PublicDebuggerErrorCallbackHolder
{
public:
    PublicDebuggerErrorCallbackHolder()
    {
        // Exiting from RS; entering Cordbg via a callback
        DbgRSThread * pThread = DbgRSThread::GetThread();

        // This callback is called from within the RS
        _ASSERTE(pThread->IsInRS());

        // Debugger error callback may be called from deep within the RS (after many nestings).
        // So immediately jump to outside. We'll restore this in dtor.
        m_oldCount = pThread->m_cInsideRS;
        pThread->m_cInsideRS = 0;

        _ASSERTE(!pThread->IsInRS());

        // We may be leaking locks for the unrecoverable callback. We mark that so that
        // the asserts about locking can be relaxed.
        pThread->SetUnrecoverableCallback(true);
    }

    ~PublicDebuggerErrorCallbackHolder()
    {
        // Re-entering RS from after a callback.
        DbgRSThread * pThread = DbgRSThread::GetThread();

        pThread->SetUnrecoverableCallback(false);
        pThread->m_cInsideRS = m_oldCount;

        // Our status of being "Inside the RS" is now restored.
        _ASSERTE(pThread->IsInRS());
    }
private:
    int m_oldCount;
};

class InternalAPIHolder
{
public:
    InternalAPIHolder()
    {
        DbgRSThread * pThread = DbgRSThread::GetThread();

        // Internal APIs should already be inside the RS.
        _ASSERTE(pThread->IsInRS() ||
            !"Internal API being called directly from outside (there should be a public API on the stack)");
    }
    void dummy() {}
};

//-----------------------------------------------------------------------------
// Declare whether an API is public or internal
// Public APIs have the following:
// - We may be called concurrently from multiple threads (ie, not thread safe)
// - This thread does not hold any RS Locks while entering or leaving this function.
// - May or May-not be reentrant.
// Internal APIs:
// - let us specifically mark that we're not a public API, and
// - we're only being called through a public API.
//-----------------------------------------------------------------------------
#define PUBLIC_API_ENTRY(_pThis) \
    STRESS_LOG1(LF_CORDB, LL_INFO1000, "[Public API '" __FUNCTION__ "', this=0x%p]\n", _pThis); \
    PublicAPIHolder __pah;

// Mark public APIs that are re-entrant.
// Very few of our APIs should be re-entrant. Even for field access APIs (like GetXXX), the
// public version is heavier (eg, checking the HRESULT) so we benefit from having a fast
// internal version and calling that directly.
#define PUBLIC_REENTRANT_API_ENTRY(_pThis) \
    STRESS_LOG1(LF_CORDB, LL_INFO1000, "[Public API (re) '" __FUNCTION__ "', this=0x%p]\n", _pThis); \
    PublicReentrantAPIHolder __pah;



// Mark internal APIs.
// All internal APIs are reentrant (duh)
#define INTERNAL_API_ENTRY(_pThis) InternalAPIHolder __pah; __pah.dummy();

// Mark an internal API from ATT_REQUIRE_STOP / ATT_ALLOW_LIVE_DO_STOP_GO.
// This can assert that we're safe to send IPC events (that we're stopped and hold the SG lock)
#define INTERNAL_SYNC_API_ENTRY(pProc)  \
    CordbProcess * __pProc = (pProc); \
    _ASSERTE(__pProc->GetStopGoLock()->HasLock() || !"Must have stop go lock for internal-sync-api"); \
    _ASSERTE(__pProc->GetSynchronized() || !"Must by synced for internal-sync-api"); \
    InternalAPIHolder __pah; __pah.dummy();


// Mark that a thread is owned by us. Thus the thread's "Inside RS" count > 0.
#define INTERNAL_THREAD_ENTRY(_pThis) \
    STRESS_LOG1(LF_CORDB, LL_INFO1000, "[Internal thread started, this=0x%p]\n", _pThis); \
    PublicAPIHolder __pah;

// Mark that a thread is calling out via a callback. This will adjust the "Inside RS" counter.
#define PUBLIC_CALLBACK_IN_THIS_SCOPE(_pThis, event) \
    STRESS_LOG1(LF_CORDB, LL_EVERYTHING, "[Dispatching '%s']\n", IPCENames::GetName((event)->type)); \
    PublicCallbackHolder __pahCallback((event)->type);

#define PUBLIC_CALLBACK_IN_THIS_SCOPE1(_pThis, event, formatLiteralString, arg0) \
    STRESS_LOG2(LF_CORDB, LL_EVERYTHING, "[Dispatching '%s' " formatLiteralString "]\n", IPCENames::GetName((event)->type), arg0); \
    PublicCallbackHolder __pahCallback((event)->type);

#define PUBLIC_CALLBACK_IN_THIS_SCOPE2(_pThis, event, formatLiteralString, arg0, arg1) \
    STRESS_LOG3(LF_CORDB, LL_EVERYTHING, "[Dispatching '%s' " formatLiteralString "]\n", IPCENames::GetName((event)->type), arg0, arg1); \
    PublicCallbackHolder __pahCallback((event)->type);

#define PUBLIC_CALLBACK_IN_THIS_SCOPE3(_pThis, event, formatLiteralString, arg0, arg1, arg2) \
    STRESS_LOG4(LF_CORDB, LL_EVERYTHING, "[Dispatching '%s' " formatLiteralString "]\n", IPCENames::GetName((event)->type), arg0, arg1, arg2); \
    PublicCallbackHolder __pahCallback((event)->type);


#define PUBLIC_CALLBACK_IN_THIS_SCOPE0(_pThis) \
    PublicCallbackHolder __pahCallback(DB_IPCE_INVALID_EVENT);

#define PUBLIC_CALLBACK_IN_THIS_SCOPE_DEBUGGERERROR(_pThis) \
    PublicDebuggerErrorCallbackHolder __pahCallback;

// Helper to log debug events.
inline void StressLogNativeDebugEvent(DEBUG_EVENT * pDebugEvent, bool fOOB)
{
    {
        STRESS_LOG3(LF_CORDB, LL_EVERYTHING, "[Dispatching Win32 code=%d, tid=%x, oob=%d.]\n",
            pDebugEvent->dwDebugEventCode, pDebugEvent->dwThreadId, fOOB);
    }

}

#define PUBLIC_WIN32_CALLBACK_IN_THIS_SCOPE(_pThis, _pDebugEvent, _fOOB) \
    StressLogNativeDebugEvent(_pDebugEvent, _fOOB); \
    PublicCallbackHolder __pahCallback(DB_IPCE_INVALID_EVENT);

#define DTOR_ENTRY(_pThis) PUBLIC_REENTRANT_API_ENTRY(_pThis)


//-----------------------------------------------------------------------------
// Typesafe bool for thread safety. This typesafety forces us to use
// an specific reason for thread-safety, taken from a well-known list.
// This is mostly concerned w/ being serialized.
// Note that this assertion must be done on a per function basis and we
// can't have any sort of 'ThreadSafetyReason CallerIsSafe()' b/c we can't
// enforce that all of our callers are thread safe (only that our current caller is safe).
//-----------------------------------------------------------------------------
struct ThreadSafetyReason
{
public:
    ThreadSafetyReason(bool f) { fIsSafe = f; }

    bool fIsSafe;
};

// Different valid reasons that we may be threads safe.
inline ThreadSafetyReason HoldsLock(RSLock * pLock)
{
    _ASSERTE(pLock != NULL);
    return ThreadSafetyReason(pLock->HasLock());
}
inline ThreadSafetyReason OnW32ET(CordbProcess * pProc)
{
    return ThreadSafetyReason(IsWin32EventThread(pProc));
}

inline ThreadSafetyReason OnRCET(Cordb *pCordb)
{
    return ThreadSafetyReason (IsRCEventThread(pCordb));
}

// We use this when we assume that a function is thread-safe (b/c it's serialized).
// The reason also lets us assert that our assumption is true.
// By using a function, we enforce typesafety and thus require a valid reason
// (as opposed to an arbitrary bool)
inline void AssertThreadSafeHelper(ThreadSafetyReason r) {
    _ASSERTE(r.fIsSafe);
}

//-----------------------------------------------------------------------------
// Assert that the given scope is always called on a single thread b/c of
// xReason. Common reasons may be b/c we hold a lock or we're always
// called on a specific thread (Eg w32et).
// The only valid reasons are of type ThreadSafetyReason (thus forcing us to
// choose from a well-known list of valid reasons).
//-----------------------------------------------------------------------------
#define ASSERT_SINGLE_THREAD_ONLY(xReason) \
    AssertThreadSafeHelper(xReason);

#else

//-----------------------------------------------------------------------------
// Retail versions just nop. See the debug implementation for these
// for their semantics.
//-----------------------------------------------------------------------------
#define PUBLIC_API_ENTRY(_pThis)
#define PUBLIC_REENTRANT_API_ENTRY(_pThis)
#define INTERNAL_API_ENTRY(_pThis)
#define INTERNAL_SYNC_API_ENTRY(pProc)
#define INTERNAL_THREAD_ENTRY(_pThis)
#define PUBLIC_CALLBACK_IN_THIS_SCOPE(_pThis, event)
#define PUBLIC_CALLBACK_IN_THIS_SCOPE1(_pThis, event, formatLiteralString, arg0)
#define PUBLIC_CALLBACK_IN_THIS_SCOPE2(_pThis, event, formatLiteralString, arg0, arg1)
#define PUBLIC_CALLBACK_IN_THIS_SCOPE3(_pThis, event, formatLiteralString, arg0, arg1, arg2)
#define PUBLIC_CALLBACK_IN_THIS_SCOPE0(_pThis)
#define PUBLIC_CALLBACK_IN_THIS_SCOPE_DEBUGGERERROR(_pThis)
#define PUBLIC_WIN32_CALLBACK_IN_THIS_SCOPE(_pThis, _pDebugEvent, _fOOB)
#define DTOR_ENTRY(_pThis)


#define ASSERT_SINGLE_THREAD_ONLY(x)

#endif // #if RSCONTRACTS


//-----------------------------------------------------------------------------
// For debugging ease, cache some global values.
// Include these in retail & free because that's where we need them the most!!
// Optimized builds may not let us view locals & parameters. So Having these
// cached as global values should let us inspect almost all of
// the interesting parts of the RS even in a Retail build!
//-----------------------------------------------------------------------------
struct RSDebuggingInfo
{
    // There should only be 1 global Cordb object. Store it here.
    Cordb * m_Cordb;

    // We have lots of processes. Keep a pointer to the most recently touched
    // (subjective) process, as a hint about what our "current" process is.
    // If we're only debugging 1 process, this will be sufficient.
    CordbProcess * m_MRUprocess;

    CordbRCEventThread * m_RCET;
};

#include "rspriv.inl"

#endif // #if RSPRIV_H


