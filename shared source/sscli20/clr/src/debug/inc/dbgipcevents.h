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
/* ------------------------------------------------------------------------- *
 * DbgIPCEvents.h -- header file for private Debugger data shared by various
 *                   debugger components.
 * ------------------------------------------------------------------------- */

#ifndef _DbgIPCEvents_h_
#define _DbgIPCEvents_h_

#include <new.hpp>
#include <cor.h>
#include <cordebug.h>
#include <corjit.h> // for ICorJitInfo::VarLocType & VarLoc
#include <specstrings.h>


#define     MAX_LOG_SWITCH_NAME_LEN     256

//-----------------------------------------------------------------------------
// Versioning note:
// This file describes the IPC communication protocol between the LS (mscorwks)
// and the RS (mscordbi). It is private and can change on a daily basis.
// The version of the LS will always match the version of the RS. They are
// like a single conceptual DLL split across 2 processes.
// The only restriction is that it should be flavor agnostic - so don't change
// layout based off '#ifdef DEBUG'. This lets us drop a Debug flavor RS onto
// a retail installation w/o any further installation woes. That's very very useful
// for debugging.
//-----------------------------------------------------------------------------

// We want this available for DbgInterface.h - put it here.
typedef enum
{
    IPC_TARGET_OUTOFPROC,
    IPC_TARGET_COUNT,
} IpcTarget;

// Get version numbers for IPCHeader stamp
#include "ndpversion.h"

// The RemoteHANDLE encapsulates the PAL specific handling of handles to avoid FEATURE_PAL ifdefs
// everywhere else in the code.
// There are two common initialization patterns:
//
// 1. Publishing of local handle for other processes, the value of the wrapper is a local handle
//    in *this* process at the end:
//    - In this process, call SetLocal(hHandle) to initialize the handle.
//    - In the other processes, call DuplicateToLocalProcess to get a local copy of the handle.
//
// 2. Injecting of local handle into other process, the value of the wrapper is a local handle
//    in the *other* process at the end:
//    - In this process, call DuplicateToRemoteProcess(hProcess, hHandle) to initialize the handle.
//    - In the other process, call ImportToOtherProcess() to finish the initialization of the wrapper
//      with a local copy of the handle.
//
// Once initialized, the wrapper can be used the same way as a regular HANDLE in the process
// it was initialized for. There is casting operator HANDLE to achieve that.

struct RemoteHANDLE {
    HANDLE              m_hLocal;
    RHANDLE             m_hRemote;

    operator HANDLE& ()
    {
        _ASSERTE(m_hLocal != NULL);
        return m_hLocal;
    }

    void Close()
    {
        HANDLE hHandle = m_hLocal;
        if (hHandle != NULL) {
            m_hLocal = NULL;
            CloseHandle(hHandle);
        }
    }

    // Sets the local value of the handle. DuplicateToLocalProcess can be used later
    // by the remote process to acquire the remote handle.
    BOOL SetLocal(HANDLE hHandle)
    {
        // The PAL on Unix needs help in supporting cross-process DuplicateHandle.
        // The local handles can not be passed cross-process in Unix PAL.
        // RHANDLE  ("global cookie") needs to be obtained through PAL_LocalToRemove firsts.
        // The other process can then call PAL_RemoteToLocal on RHANDLE to get its local handle
        // of the object.
        m_hLocal = hHandle;
        if (!(m_hRemote = PAL_LocalHandleToRemote(hHandle)))
        {
            return FALSE;
        }
        return TRUE;
    }

    // Duplicates the current handle value to remote process. ImportToLocalProcess
    // should be called in the remote process before the handle is used in the remote process.
    BOOL DuplicateToRemoteProcess(HANDLE hProcess, HANDLE hHandle)
    {
        HANDLE hLocal;
        if (!DuplicateHandle(GetCurrentProcess(), hHandle, hProcess, &hLocal,
                NULL, FALSE, DUPLICATE_SAME_ACCESS))
        {
            return FALSE;
        }

        m_hLocal = NULL;
        if (!(m_hRemote = PAL_LocalHandleToRemote(hLocal)))
        {
            return FALSE;
        }

        return TRUE;
    }

    // Duplicates the current handle value to local process. To be used in combination with SetLocal.
    BOOL DuplicateToLocalProcess(HANDLE hProcess, HANDLE* pHandle)
    {
        HANDLE hTemp = PAL_RemoteHandleToLocal(m_hRemote);
        if (hTemp == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }
        return DuplicateHandle(hProcess, hTemp, GetCurrentProcess(), pHandle,
                        NULL, FALSE, DUPLICATE_SAME_ACCESS);
    }

    void CloseInRemoteProcess(HANDLE hProcess)
    {
        m_hLocal = NULL;
        m_hRemote = NULL;
        // ROTORTODO: need to come up with a way of doing cross-process HANDLEs
        // for the cordbg/ee interface
    }

    // Imports the handle to local process. To be used in combination with DuplicateToRemoteProcess.
    HANDLE ImportToLocalProcess()
    {
        if (m_hLocal == NULL && m_hRemote != NULL)
        {
            HANDLE hTemp = PAL_RemoteHandleToLocal(m_hRemote);
            if (hTemp == INVALID_HANDLE_VALUE)
            {
                return NULL;
            }
            m_hLocal = hTemp;
        }
        return m_hLocal;
    }
};

#include "dbgappdomain.h"

//
// Names of the setup sync event and shared memory used for IPC between the Left Side and the Right Side. NOTE: these
// names must include a %d for the process id. The process id used is the process id of the debuggee.
//
// For Rotor, always use the "Global\\Rotor"
#define CorDBEventNamePrefix L"Global\\"

#define CorDBIPCSetupSyncEventName CorDBEventNamePrefix L"CorDBIPCSetupSyncEvent_%d"
#define CorDBIPCLSEventAvailName   CorDBEventNamePrefix L"CorDBIPCLSEventAvailName_%d"
#define CorDBIPCLSEventReadName    CorDBEventNamePrefix L"CorDBIPCLSEventReadName_%d"

#define CorDBDebuggerAttachedEvent CorDBEventNamePrefix L"CorDBDebuggerAttachedEvent_%d"

//
// This define controls whether we always pass first chance exceptions to the in-process first chance hijack filter
// during interop debugging or if we try to short-circuit and make the decision out-of-process as much as possible.
//
#define CorDB_Short_Circuit_First_Chance_Ownership 1

//
// Defines for current version numbers for the left and right sides
//
#define CorDB_LeftSideProtocolCurrent           2
#define CorDB_LeftSideProtocolMinSupported      2
#define CorDB_RightSideProtocolCurrent          2
#define CorDB_RightSideProtocolMinSupported     2

//
// DebuggerIPCRuntimeOffsets contains addresses and offsets of important global variables, functions, and fields in
// Runtime objects. This is populated during Left Side initialization and is read by the Right Side. This struct is
// mostly to facilitate unmanaged debugging support, but it may have some small uses for managed debugging.
//
struct DebuggerIPCRuntimeOffsets
{
    SIZE_T  m_TLSIndex;                                 // The TLS index the CLR is using to hold Thread objects
    SIZE_T  m_TLSIsSpecialIndex;                        // The index into the Predef block of the the "IsSpecial" status for a thread.
    SIZE_T  m_TLSCantStopIndex;                         // The index into the Predef block of the the Can't-Stop count.
    SIZE_T  m_TLSIndexOfPredefs;                        // The TLS index of the Predef block.
    SIZE_T  m_EEThreadStateOffset;                      // Offset of m_state in a Thread
    SIZE_T  m_EEThreadStateNCOffset;                    // Offset of m_stateNC in a Thread
    SIZE_T  m_EEThreadPGCDisabledOffset;                // Offset of the bit for whether PGC is disabled or not in a Thread
    DWORD   m_EEThreadPGCDisabledValue;                 // Value at m_EEThreadPGCDisabledOffset that equals "PGC disabled".
    SIZE_T  m_EEThreadDebuggerWordOffset;               // Offset of debugger word in a Thread
    SIZE_T  m_EEThreadFrameOffset;                      // Offset of the Frame ptr in a Thread
    SIZE_T  m_EEThreadMaxNeededSize;                    // Max memory to read to get what we need out of a Thread object
    DWORD   m_EEThreadSteppingStateMask;                // Mask for Thread::TSNC_DebuggerIsStepping
    DWORD   m_EEMaxFrameValue;                          // The max Frame value
    SIZE_T  m_EEThreadDebuggerFilterContextOffset;      // Offset of debugger's filter context within a Thread Object.
    SIZE_T  m_EEThreadCantStopOffset;                   // Offset of the can't stop count in a Thread
    SIZE_T  m_EEFrameNextOffset;                        // Offset of the next ptr in a Frame
    DWORD   m_EEIsManagedExceptionStateMask;            // Mask for Thread::TSNC_DebuggerIsManagedException
    void   *m_pPatches;                                 // Addr of patch table
    BOOL   *m_pPatchTableValid;                         // Addr of g_patchTableValid
    SIZE_T  m_offRgData;                                // Offset of m_pcEntries
    SIZE_T  m_offCData;                                 // Offset of count of m_pcEntries
    SIZE_T  m_cbPatch;                                  // Size per patch entry
    SIZE_T  m_offAddr;                                  // Offset within patch of target addr
    SIZE_T  m_offOpcode;                                // Offset within patch of target opcode
    SIZE_T  m_cbOpcode;                                 // Max size of opcode
    SIZE_T  m_offTraceType;                             // Offset of the trace.type within a patch
    DWORD   m_traceTypeUnmanaged;                       // TRACE_UNMANAGED
};

#ifdef _X86_
#define CorDBIPC_BUFFER_SIZE (1500) // hand tuned to ensure that ipc block in IPCHeader.h fits in 1 page.
#else
// This is the size of a DebuggerIPCEvent.  You will hit an assert in Cordb::Initialize() (DI\process.cpp)
// if this is not defined correctly.
#define CorDBIPC_BUFFER_SIZE 4016 // (4016 + 6) * 2 + 148 = 8192 (two (DebuggerIPCEvent + alignment padding) +
                                  //                              other fields = page size)
#endif //_X86_
//
// DebuggerIPCControlBlock describes the layout of the shared memory shared between the Left Side and the Right
// Side. This includes error information, handles for the IPC channel, and space for the send/receive buffers.
//
struct DebuggerIPCControlBlock
{
    // Version data should be first in the control block to ensure that we can read it even if the control block
    // changes.
    SIZE_T                     m_DCBSize;
    ULONG                      m_verMajor;          // CLR build number for the Left Side.
    ULONG                      m_verMinor;          // CLR build number for the Left Side.

    // This next stuff fits in a  DWORD.
    bool                       m_checkedBuild;      // CLR build type for the Left Side.
    // using the first padding byte to indicate if hosted in fiber mode.
    // We actually just need one bit. So if needed, can turn this to a bit.
    // BYTE padding1;
    bool                       m_bHostingInFiber;
    BYTE padding2;
    BYTE padding3;

    ULONG                      m_leftSideProtocolCurrent;       // Current protocol version for the Left Side.
    ULONG                      m_leftSideProtocolMinSupported;  // Minimum protocol the Left Side can support.

    ULONG                      m_rightSideProtocolCurrent;      // Current protocol version for the Right Side.
    ULONG                      m_rightSideProtocolMinSupported; // Minimum protocol the Right Side requires.

    HRESULT                    m_errorHR;
    unsigned int               m_errorCode;

#ifndef _X86_
    // 64-bit needs this padding to make the handles after this aligned.
    // But x86 can't have this padding b/c it breaks binary compatability between v1.1 and v2.0.
    ULONG padding4;
#endif

    RemoteHANDLE               m_rightSideEventAvailable;
    RemoteHANDLE               m_rightSideEventRead;
    RemoteHANDLE               m_leftSideEventAvailable;
    RemoteHANDLE               m_leftSideEventRead;
    RemoteHANDLE               m_rightSideProcessHandle;

    //.............................................................................
    // Everything above this point must have the exact same binary layout as v1.1.
    // See protocol details below.
    //.............................................................................

    RemoteHANDLE               m_leftSideUnmanagedWaitEvent;
    RemoteHANDLE               m_syncThreadIsLockFree;


    // This is set immediately when the helper thread is created.
    // This will be set even if there's a temporary helper thread or if the real helper
    // thread is not yet pumping (eg, blocked on a loader lock).
    DWORD                      m_realHelperThreadId;

    // This is only published once the helper thread starts running in its main loop.
    // Thus we can use this field to see if the real helper thread is actually pumping.
    DWORD                      m_helperThreadId;

    // This is non-zero if the LS has a temporary helper thread.
    DWORD                      m_temporaryHelperThreadId;

    // ID of the Helper's canary thread.
    DWORD                      m_CanaryThreadId;

    DebuggerIPCRuntimeOffsets *m_runtimeOffsets;
    void                      *m_helperThreadStartAddr;
    void                      *m_helperRemoteStartAddr;
    DWORD                     *m_specialThreadList;
    BYTE                       m_receiveBuffer[CorDBIPC_BUFFER_SIZE];
    BYTE                       m_sendBuffer[CorDBIPC_BUFFER_SIZE];

    DWORD                      m_specialThreadListLength;
    bool                       m_shutdownBegun;
    bool                       m_rightSideIsWin32Debugger;
    bool                       m_specialThreadListDirty;

    bool                       m_rightSideShouldCreateHelperThread;

    // Version of the debugger consuming the RS. This is written by the RS into the DCB to notify the LS b/c
    // Some backwards behavior is much easier to implement on the LS.
    CorDebugInterfaceVersion   m_DebuggerVersion;

    // NOTE The Init method works since there are no virtual functions - don't add any virtual functions without
    // changing this!
    // Only initialized by the LS, opened by the RS.
    HRESULT Init(HANDLE rsea, HANDLE rser, HANDLE lsea, HANDLE lser,
                 HANDLE lsuwe, HANDLE stilf);

};


#define INITIAL_APP_DOMAIN_INFO_LIST_SIZE   16

//-----------------------------------------------------------------------------
// Provide some Type-safety in the IPC block when we pass remote pointers around.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// This is the same in both the LS & RS.
// Definitions on the LS & RS should be binary compatible. So all storage is
// declared in GeneralLsPointer, and then the Ls & RS each have their own
// derived accessors.
//-----------------------------------------------------------------------------
class GeneralLsPointer
{
protected:
    friend ULONG_PTR LsPtrToCookie(GeneralLsPointer p);
    void * m_ptr;

public:
    bool IsNull() { return m_ptr == NULL; }
};

class GeneralRsPointer
{
protected:
    UINT m_data;

public:
    bool IsNull() { return m_data == 0; }
};

// In some cases, we need to get a uuid from a pointer (ie, in a hash)
inline ULONG_PTR LsPtrToCookie(GeneralLsPointer p) {
    return (ULONG_PTR) p.m_ptr;
}


#ifdef RIGHT_SIDE_COMPILE
//-----------------------------------------------------------------------------
// Infrasturcture for RS Definitions
//-----------------------------------------------------------------------------

// On the RS, we don't have the LS classes defined, so we can't templatize that
// in terms of <class T>, but we still want things to be unique.
// So we create an empty enum for each LS type and then templatize it in terms
// of the enum.
template <typename n>
class LsPointer : public GeneralLsPointer
{
public:
    void Set(void * p)
    {
        m_ptr = p;
    }
    void * UnsafeGet()
    {
        return m_ptr;
    }

    static LsPointer<n> NullPtr()
    {
        LsPointer<n> t;
        t.Set(NULL);
        return t;
    }

    bool operator!= (void * p) { return m_ptr != p; }
    bool operator== (void * p) { return m_ptr == p; }
    bool operator==(LsPointer<n> p) { return p.m_ptr == this->m_ptr; }

    // We should never UnWrap() them in the RS, so we don't define that here.
};

class CordbProcess;
template <class T> UINT AllocCookie(CordbProcess *pProc, T* p);
template <class T> T * UnwrapCookie(CordbProcess *pProc, UINT cookie);

UINT AllocCookieCordbEval(CordbProcess *pProc, class CordbEval* p);
class CordbEval * UnwrapCookieCordbEval(CordbProcess *pProc, UINT cookie);

template <class CordbEval> UINT AllocCookie(CordbProcess *pProc, CordbEval* p)
{
    return AllocCookieCordbEval(pProc, p);
}
template <class CordbEval> CordbEval * UnwrapCookie(CordbProcess *pProc, UINT cookie)
{
    return UnwrapCookieCordbEval(pProc, cookie);
}



// This is how the RS sees the pointers in the IPC block.
template<class T>
class RsPointer : public GeneralRsPointer
{
public:
    // Since we're being used inside a union, we can't have a ctor.

    static RsPointer<T> NullPtr()
    {
        RsPointer<T> t;
        t.m_data = 0;
        return t;        
    }

    bool AllocHandle(CordbProcess *pProc, T* p)
    {    
        // This will force validation.
        m_data = AllocCookie<T>(pProc, p);
        return (m_data != 0);       
    }

    bool operator==(RsPointer<T> p) { return p.m_data == this->m_data; }

    T* UnWrapAndRemove(CordbProcess *pProc)
    {
        return UnwrapCookie<T>(pProc, m_data);
    }
    
protected:
};

// Create an enum to use as tag so that each type of LS pointer can have
// its own type.
#define DEFINE_LSPTR_TYPE(ls_type, ptr_name) \
    ls_type##_FakeTypeName { };  \
    typedef LsPointer<ls_type##_FakeTypeName> ptr_name;

// enum __LS__##ls_type { };  \


#define DEFINE_RSPTR_TYPE(rs_type, ptr_name) \
    class rs_type; \
    typedef RsPointer<rs_type> ptr_name;

#else // !RIGHT_SIDE_COMPILE
//-----------------------------------------------------------------------------
// Infrastructure for LS Definitions
//-----------------------------------------------------------------------------

// This is how the LS sees the pointers in the IPC block.
template<typename T>
class LsPointer : public GeneralLsPointer
{
public:
    // Since we're being used inside a union, we can't have a ctor.
    //LsPointer() { }

    static LsPointer<T> NullPtr()
    {
        return MakePtr(NULL);
    }

    static LsPointer<T> MakePtr(T* p)
    {
        LsPointer<T> t;
        t.Set(p);
        return t;
    }

    bool operator!= (void * p) { return m_ptr != p; }
    bool operator== (void * p) { return m_ptr == p; }
    bool operator==(LsPointer<T> p) { return p.m_ptr == this->m_ptr; }

    void Set(T* p)
    {
        m_ptr = p;
    }

    T* UnWrap()
    {
        return (T*) m_ptr;
    }
};

template <class n>
class RsPointer : public GeneralRsPointer
{
public:
    static RsPointer<n> NullPtr()
    {
        RsPointer<n> t;
        t.m_data = 0;
        return t;
    }

    bool operator==(RsPointer<n> p) { return p.m_data == this->m_data; }

    // We should never UnWrap() them in the LS, so we don't define that here.
};

#define DEFINE_LSPTR_TYPE(ls_type, ptr_name) \
    ls_type; \
    typedef LsPointer<ls_type> ptr_name;

#define DEFINE_RSPTR_TYPE(rs_type, ptr_name) \
    enum __RS__##rs_type { };  \
    typedef RsPointer<__RS__##rs_type> ptr_name;

#endif // !RIGHT_SIDE_COMPILE

// We must be binary compatible w/ a pointer.
C_ASSERT(sizeof(LsPointer<void>) == sizeof(GeneralLsPointer));

C_ASSERT(sizeof(void*) == sizeof(GeneralLsPointer));



//-----------------------------------------------------------------------------
// Definitions for Left-Side ptrs.
//-----------------------------------------------------------------------------
DEFINE_LSPTR_TYPE(class DebuggerModule, LSPTR_DMODULE);
DEFINE_LSPTR_TYPE(class Thread,    LSPTR_THREAD);
DEFINE_LSPTR_TYPE(class AppDomain, LSPTR_APPDOMAIN);
DEFINE_LSPTR_TYPE(class Assembly,  LSPTR_ASSEMBLY);
DEFINE_LSPTR_TYPE(class FieldDesc, LSPTR_FIELDDESC);
DEFINE_LSPTR_TYPE(class DebuggerJitInfo, LSPTR_DJI);
DEFINE_LSPTR_TYPE(class DebuggerMethodInfo, LSPTR_DMI);
DEFINE_LSPTR_TYPE(class MethodDesc,         LSPTR_METHODDESC);
DEFINE_LSPTR_TYPE(class DebuggerBreakpoint, LSPTR_BREAKPOINT);
DEFINE_LSPTR_TYPE(class DebuggerEval,       LSPTR_DEBUGGEREVAL);
DEFINE_LSPTR_TYPE(class DebuggerStepper,    LSPTR_STEPPER);

DEFINE_LSPTR_TYPE(struct _CONTEXT, LSPTR_CONTEXT);
DEFINE_LSPTR_TYPE(struct OBJECTHANDLE__,    LSPTR_OBJECTHANDLE);
DEFINE_LSPTR_TYPE(class TypeHandleDummyPtr, LSPTR_TYPEHANDLE); // TypeHandle in the LS is not a direct pointer.

//-----------------------------------------------------------------------------
// Definitions for Right-Side ptrs.
//-----------------------------------------------------------------------------
DEFINE_RSPTR_TYPE(CordbEval,               RSPTR_CORDBEVAL);


//-----------------------------------------------------------------------------
// We pass some fixed size strings in the IPC block.
// Helper class to wrap the buffer and protect against buffer overflows.
// This should be binary compatible w/ a wchar[] array.
//-----------------------------------------------------------------------------

template <int nMaxLengthIncludingNull>
class EmbeddedIPCString
{
public:
    // Set, caller responsiblity that wcslen(pData) < nMaxLengthIncludingNull
    void SetString(const WCHAR * pData)
    {
        // If the string doesn't fit into the buffer, that's a bug (and so this is a real
        // assert, not just a simplifying assumption). To fix it, either:
        // - make the buffer larger
        // - don't pass the string as an embedded string in the IPC block.
        // This will truncate (rather than AV on the RS).
        int ret;
        ret = SafeCopy(pData);

        // See comment above - caller should guarantee that buffer is large enough.
        _ASSERTE(ret != STRUNCATE);
    }

    // Set a string from a substring. This will truncate if necessary.
    void SetStringTruncate(const WCHAR * pData)
    {
        // ignore return value because truncation is ok.
        SafeCopy(pData);
    }

    const WCHAR * GetString()
    {
        // For a null-termination just in case a bug in the debuggee process
        // yields a malformed string.
        m_data[nMaxLengthIncludingNull - 1] = L'\0';
        return &m_data[0];
    }
    int GetMaxSize() const { return nMaxLengthIncludingNull; }

private:
    int SafeCopy(const WCHAR *pData)
    {
        return wcsncpy_s(
            m_data, nMaxLengthIncludingNull,
            pData, _TRUNCATE);
    }
    WCHAR m_data[nMaxLengthIncludingNull];
};

//
// Types of events that can be sent between the Runtime Controller and
// the Debugger Interface. Some of these events are one way only, while
// others go both ways. The grouping of the event numbers is an attempt
// to show this distinction and perhaps even allow generic operations
// based on the type of the event.
//
enum DebuggerIPCEventType
{
#define IPC_EVENT_TYPE0(type, val)  type = val,
#define IPC_EVENT_TYPE1(type, val)  type = val,
#define IPC_EVENT_TYPE2(type, val)  type = val,
#include "dbgipceventtypes.h"
#undef IPC_EVENT_TYPE2
#undef IPC_EVENT_TYPE1
#undef IPC_EVENT_TYPE0
};

#ifdef _DEBUG

// This is a static debugging structure to help breaking at the right place.
// Debug only. This is to track the number of events that have been happened so far.
// User can choose to set break point base on the number of events.
// Variables are named as the event name with prefix m_iDebugCount. For example
// m_iDebugCount_DB_IPCE_BREAKPOINT if for event DB_IPCE_BREAKPOINT.
struct DebugEventCounter
{
// we don't need the event type 0
#define IPC_EVENT_TYPE0(type, val)
#define IPC_EVENT_TYPE1(type, val)  int m_iDebugCount_##type;
#define IPC_EVENT_TYPE2(type, val)  int m_iDebugCount_##type;
#include "dbgipceventtypes.h"
#undef IPC_EVENT_TYPE2
#undef IPC_EVENT_TYPE1
#undef IPC_EVENT_TYPE0
};
#endif // _DEBUG


struct IPCEventTypeNameMapping
    {
            DebuggerIPCEventType    eventType;
            const char *            eventName;
};
static const IPCEventTypeNameMapping DbgIPCEventTypeNames[] =
        {
            #define IPC_EVENT_TYPE0(type, val)  { type, #type },
            #define IPC_EVENT_TYPE1(type, val)  { type, #type },
            #define IPC_EVENT_TYPE2(type, val)  { type, #type },
            #include "dbgipceventtypes.h"
            #undef IPC_EVENT_TYPE2
            #undef IPC_EVENT_TYPE1
            #undef IPC_EVENT_TYPE0
            { DB_IPCE_INVALID_EVENT, "DB_IPCE_Error" }
        };

        const size_t nameCount = sizeof(DbgIPCEventTypeNames) / sizeof(DbgIPCEventTypeNames[0]);



struct IPCENames // We use a class/struct so that the function can remain in a shared header file
{
    static const DebuggerIPCEventType GetEventType(__in_z char * strEventType)
    {
        // pass in the string of event name and find the matching enum value
        // This is a linear search which is pretty slow. However, this is only used
        // at startup time when debug assert is turn on and with registry key set. So it is not that bad.
        //
        for (size_t i = 0; i < nameCount; i++)
        {
            if (_stricmp(DbgIPCEventTypeNames[i].eventName, strEventType) == 0)
                return DbgIPCEventTypeNames[i].eventType;
        }
        return DB_IPCE_INVALID_EVENT;
    }
    static const char * GetName(DebuggerIPCEventType eventType)
    {

        enum DbgIPCEventTypeNum
        {
        #define IPC_EVENT_TYPE0(type, val)  type##_Num,
        #define IPC_EVENT_TYPE1(type, val)  type##_Num,
        #define IPC_EVENT_TYPE2(type, val)  type##_Num,
        #include "dbgipceventtypes.h"
        #undef IPC_EVENT_TYPE2
        #undef IPC_EVENT_TYPE1
        #undef IPC_EVENT_TYPE0
        };

        unsigned int i, lim;

        if (eventType < DB_IPCE_DEBUGGER_FIRST)
        {
            i = DB_IPCE_RUNTIME_FIRST_Num + 1;
            lim = DB_IPCE_DEBUGGER_FIRST_Num;
        }
        else
        {
            i = DB_IPCE_DEBUGGER_FIRST_Num + 1;
            lim = nameCount;
        }

        for (/**/; i < lim; i++)
        {
            if (DbgIPCEventTypeNames[i].eventType == eventType)
                return DbgIPCEventTypeNames[i].eventName;
        }

        return DbgIPCEventTypeNames[nameCount - 1].eventName;
    }
};


//
// NOTE:  CPU-specific values below!
//
// DebuggerREGDISPLAY is very similar to the EE REGDISPLAY structure. It holds
// register values that can be saved over calls for each frame in a stack
// trace.
//
// DebuggerIPCE_FloatCount is the number of doubles in the processor's
// floating point stack.
//
//
//

struct DebuggerREGDISPLAY
{
#ifdef _X86_
    #define DebuggerIPCE_FloatCount 8

    SIZE_T  Edi;
    void   *pEdi;
    SIZE_T  Esi;
    void   *pEsi;
    SIZE_T  Ebx;
    void   *pEbx;
    SIZE_T  Edx;
    void   *pEdx;
    SIZE_T  Ecx;
    void   *pEcx;
    SIZE_T  Eax;
    void   *pEax;
    SIZE_T  FP;
    void   *pFP;
    SIZE_T  SP;
    SIZE_T  PC;

#else
    #define DebuggerIPCE_FloatCount 1

    SIZE_T PC;
    SIZE_T FP;
    SIZE_T SP;
    void   *pFP;
#endif // !_X86 && !_IA64_ && !_AMD64_
};

inline LPVOID GetSPAddress(DebuggerREGDISPLAY *display)
{
    return (LPVOID)&display->SP;
}

inline LPVOID GetFPAddress(DebuggerREGDISPLAY *display)
{
    return (LPVOID)&display->FP;
}

// struct DebuggerIPCE_FuncData:   DebuggerIPCE_FunctionData holds data
// to describe a given function, its
// class, and a little bit about the code for the function. This is used
// in the stack trace result data to pass function information back that
// may be needed. Its also used when getting data about a specific function.
//
// void* nativeStartAddressPtr: Ptr to CORDB_ADDRESS, which is
//          the address of the real start address of the native code.
//          This field will be NULL only if the method hasn't been JITted
//          yet (and thus no code is available).  Otherwise, it will be
//          the adress of a CORDB_ADDRESS in the remote memory.  This
//          CORDB_ADDRESS may be NULL, in which case the code is unavailable
//          has been pitched (return CORDBG_E_CODE_NOT_AVAILABLE)
//
// SIZE_T nVersion: The version of the code that this instance of the
//          function is using.
struct DebuggerIPCE_FuncData
{
    mdMethodDef funcMetadataToken;
    SIZE_T      funcRVA;
    LSPTR_DMODULE funcDebuggerModuleToken;
    LSPTR_ASSEMBLY funcDebuggerAssemblyToken;

    mdTypeDef   classMetadataToken;

    void*       ilStartAddress;
    SIZE_T      ilSize;

    SIZE_T      currentEnCVersion;

    mdSignature  localVarSigToken;


};

// struct DebuggerIPCE_JITFuncData:   DebuggerIPCE_JITFuncData holds
// a little bit about the JITted code for the function.
//
// void* nativeStartAddressPtr: Ptr to CORDB_ADDRESS, which is
//          the address of the real start address of the native code.
//          This field will be NULL only if the method hasn't been JITted
//          yet (and thus no code is available).  Otherwise, it will be
//          the adress of a CORDB_ADDRESS in the remote memory.  This
//          CORDB_ADDRESS may be NULL, in which case the code is unavailable
//          or has been pitched (return CORDBG_E_CODE_NOT_AVAILABLE)
//
// SIZE_T nativeSize: Size of the native code.
//
// SIZE_T nativeOffset: Offset from the beginning of the function,
//          in bytes.  This may be non-zero even when nativeStartAddressPtr
//          is NULL
// void * nativeCodeJITInfoToken: An opaque value to hand back to the left
//          side when fetching the JITInfo for the native code, i.e. the
//          IL->native maps for the variables.  This may be NULL if no JITInfo is available.
// void * nativeCodeMethodDescToken: An opaque value to hand back to the left
//          side when fetching the code.  In addition this token can act as the
//          unique identity for the native code in the case where there are
//          multiple blobs of native code per IL method (i.e. if the method is
//          generic code of some kind)
// BOOL isInstantiatedGeneric: Indicates if the method is
//          generic code of some kind.
// void *ilToNativeMapAddr etc.: If nativeCodeJITInfoToken is not NULL then these
//          specify the table giving the mapping of IPs.
struct DebuggerIPCE_JITFuncData
{
    void*       nativeStartAddressPtr;
    SIZE_T      nativeSize;

    // If we have a cold region, need its size & the pointer to where starts.
    void*       nativeStartAddressColdPtr;
    SIZE_T      nativeColdSize;


    SIZE_T      nativeOffset;
    LSPTR_DJI   nativeCodeJITInfoToken;
    LSPTR_METHODDESC nativeCodeMethodDescToken;


    void*       ilToNativeMapAddr;
    SIZE_T      ilToNativeMapSize;

    // indicates if the MethodDesc is a generic function or a method inside a generic class (or
    // both!).
    BOOL         isInstantiatedGeneric;

    // this is the version of the jitted code
    SIZE_T       enCVersion;
};


class FramePointer
{
friend bool IsCloserToLeaf(FramePointer fp1, FramePointer fp2);
friend bool IsCloserToRoot(FramePointer fp1, FramePointer fp2);
friend bool IsEqualOrCloserToLeaf(FramePointer fp1, FramePointer fp2);
friend bool IsEqualOrCloserToRoot(FramePointer fp1, FramePointer fp2);

  public:
      static FramePointer MakeFramePointer(LPVOID sp)
      {
          FramePointer fp;
          fp.m_sp  = sp;
          return fp;
      }

      inline bool operator==(FramePointer fp)
      {
          return (m_sp == fp.m_sp
                 );
      }

      inline bool operator!=(FramePointer fp)
      {
          return !(*this == fp);
      }

      // This is needed because on the RS, the m_id values of CordbFrame and
      // CordbChain are really FramePointers.
      LPVOID GetSPValue() const
      {
          return m_sp;
      }



  private:
      // Declare some private constructors which signatures matching common usage of FramePointer
      // to prevent people from accidentally assigning a pointer to a FramePointer().
      FramePointer &operator=(LPVOID sp);
      FramePointer &operator=(BYTE* sp);
      FramePointer &operator=(const BYTE* sp);

      LPVOID m_sp;
};

// For non-IA64 platforms, we use stack pointers as frame pointers.
// (Stack grows towards smaller address.)
// On IA64, we use the backing store pointers.
// (Backing store grows towards larger address.)
    #define LEAF_MOST_FRAME FramePointer::MakeFramePointer(NULL)
    #define ROOT_MOST_FRAME FramePointer::MakeFramePointer((LPVOID)-1)

    C_ASSERT(sizeof(FramePointer) == sizeof(void*));


inline bool IsCloserToLeaf(FramePointer fp1, FramePointer fp2)
{
    return (fp1.m_sp < fp2.m_sp
           );
}

inline bool IsCloserToRoot(FramePointer fp1, FramePointer fp2)
{
    return (fp1.m_sp > fp2.m_sp
           );
}

inline bool IsEqualOrCloserToLeaf(FramePointer fp1, FramePointer fp2)
{
    return !IsCloserToRoot(fp1, fp2);
}

inline bool IsEqualOrCloserToRoot(FramePointer fp1, FramePointer fp2)
{
    return !IsCloserToLeaf(fp1, fp2);
}


//
// DebuggerIPCE_STRData holds data for each stack frame or chain. This data is passed
// from the RC to the DI during a stack walk.
//
struct DebuggerIPCE_STRData
{
    FramePointer            fp;
    DebuggerREGDISPLAY      rd;
    bool                    quicklyUnwound;

    LSPTR_APPDOMAIN         currentAppDomainToken;


    enum EType
    {
        cMethodFrame = 0,
        cChain,
        cStubFrame
    } eType;

    union
    {
        // Data for a chain
        struct
        {
            CorDebugChainReason chainReason;
            bool                managed;
            void               *context;
        } u;

        // Data for a Method
        struct
        {
            struct DebuggerIPCE_FuncData funcData;
            struct DebuggerIPCE_JITFuncData jitFuncData;
            void                        *ILIP;
            CorDebugMappingResult        mapping;

            struct
            {
                bool        fVarArgs;
                void       *rpSig;
                SIZE_T      cbSig;
                void       *rpFirstArg;
            } varargs;

            void       *ambientESP;
            void       *exactGenericArgsToken;
        } v;

        // Data for an Stub Frame.
        struct
        {
            mdMethodDef funcMetadataToken;
            LSPTR_DMODULE funcDebuggerModuleToken;
            CorDebugInternalFrameType frameType;
        } stubFrame;

    };
};

//
// DebuggerIPCE_FieldData holds data for each field within a class or type. This data
// is passed from the RC to the DI in response to a request for class info.
// This struct is also used by CordbClass and CordbType to hold the list of fields for the
// class.
//
struct DebuggerIPCE_FieldData
{
    mdFieldDef      fldMetadataToken;
    // fldStorageAvailable is true whenever the storage for this field is available.
    // If this is a field that is newly added with EnC and hasn't had any storage
    // allocated yet, then fldEnCAvailable will be false.
    BOOL            fldStorageAvailable;

    LSPTR_FIELDDESC fldDebuggerToken;

    // Bits that specificy what type of field this is
    bool            fldIsStatic;        // true if static field, false if instance field
    bool            fldIsRVA;           // true if static relative to module address
    bool            fldIsTLS;           // true if thread-specific static
    bool            fldIsContextStatic; // true if context-specific static
    bool            fldIsPrimitive;     // Only true if this is a value type masquerading as a primitive.

#ifdef RIGHT_SIDE_COMPILE
    HRESULT GetFieldSignature(class CordbModule *pModule, /*OUT*/ SigParser *pSigParser);
#else
    void Initialize()
    {
        fldSignatureCache = NULL;
        fldSignatureCacheSize = 0;
        fldInstanceOffset = 0;
        fldStaticAddress = NULL;
    }
#endif

    // If this is an instance field, return its offset
    // Note that this offset is allways a real offset (possibly larger than 22 bits), which isn't
    // necessarily the same as the overloaded FieldDesc.dwOffset field which can have
    // some special FIELD_OFFSET tokens.
    SIZE_T  GetInstanceOffset()
    {
        _ASSERTE( !fldIsStatic );
        _ASSERTE( !fldIsRVA );
        _ASSERTE( !fldIsTLS );
        _ASSERTE( !fldIsContextStatic );
        _ASSERTE( fldStorageAvailable );
        _ASSERTE( fldStaticAddress == NULL );
        return fldInstanceOffset;
    }

    // If this is a "normal" static, get its absolute address
    // TLS and context-specific statics are "special" and must be queried
    // using the DB_IPCE_GET_SPECIAL_STATIC event
    void* GetStaticAddress()
    {
        _ASSERTE( fldIsStatic );
        _ASSERTE( !fldIsTLS );
        _ASSERTE( !fldIsContextStatic );
        _ASSERTE( fldStorageAvailable || (fldStaticAddress == NULL));
        _ASSERTE( fldInstanceOffset == 0 );
        return fldStaticAddress;
    }

#ifndef RIGHT_SIDE_COMPILE
    // If this is an instance field, store its offset
    void SetInstanceOffset( SIZE_T offset )
    {
        _ASSERTE( !fldIsStatic );
        _ASSERTE( !fldIsRVA );
        _ASSERTE( !fldIsTLS );
        _ASSERTE( !fldIsContextStatic );
        _ASSERTE( fldStorageAvailable );
        _ASSERTE( fldStaticAddress == NULL );
        fldInstanceOffset = offset;
    }

    // If this is a "normal" static, store its absolute address
    void SetStaticAddress( void* addr )
    {
        _ASSERTE( fldIsStatic );
        _ASSERTE( !fldIsTLS );
        _ASSERTE( !fldIsContextStatic );
        _ASSERTE( fldStorageAvailable );
        _ASSERTE( fldInstanceOffset == 0 );
        fldStaticAddress = addr;
    }
#endif

private:
    // The fldInstanceOffset and fldStaticAddress are mutually exclusive. Only one is ever set at a time.
    SIZE_T        fldInstanceOffset;   // The offset of a field within an object instance
    void*           fldStaticAddress;  // The absolute LS address of a static field

    PCCOR_SIGNATURE fldSignatureCache; // This is passed across as null. It is a RS-only cache, and SHOULD NEVER BE
                                       // ACCESSED DIRECTLY!
    ULONG fldSignatureCacheSize;       // This is passed across as 0. It is a RS-only cache, and SHOULD NEVER BE
                                       // ACCESSED DIRECTLY!
};

//
// DebuggerIPCE_BasicTypeData and DebuggerIPCE_ExpandedTypeData
// hold data for each type sent across the
// boundary, whether it be a constructed type List<String> or a non-constructed
// type such as String, Foo or Object.
//
// Logically speaking DebuggerIPCE_BasicTypeData might just be "typeHandle", as
// we could then send further events to ask what the elementtype, typeToken and moduleToken
// are for the type handle.  But as
// nearly all types are non-generic we send across even the basic type information in
// the slightly expanded form shown below, sending the element type and the
// tokens with the type handle itself. The fields debuggerModuleToken, metadataToken and typeHandle
// are only used as follows:
//                                   elementType    debuggerModuleToken metadataToken      typeHandle
//     E_T_INT8    :                  E_T_INT8        No                     No              No
//     Boxed E_T_INT8:                E_T_CLASS       No                     No              No
//     E_T_CLASS, non-generic class:  E_T_CLASS       Yes                    Yes             No
//     E_T_VALUETYPE, non-generic:    E_T_VALUETYPE   Yes                    Yes             No
//     E_T_CLASS,     generic class:  E_T_CLASS       Yes                    Yes             Yes
//     E_T_VALUETYPE, generic class:  E_T_VALUETYPE   Yes                    Yes             Yes
//     E_T_BYREF                   :  E_T_BYREF        No                   No              Yes
//     E_T_PTR                     :  E_T_PTR          No                   No              Yes
//     E_T_ARRAY etc.              :  E_T_ARRAY        No                   No              Yes
//     E_T_FNPTR etc.              :  E_T_FNPTR        No                     No              Yes
// This allows us to always set "typeHandle" to NULL except when dealing with highly nested
// types or function-pointer types (the latter are too complexe to transfer over in one hit).
//

struct DebuggerIPCE_BasicTypeData
{
    CorElementType  elementType;
    mdTypeDef       metadataToken;
    LSPTR_DMODULE   debuggerModuleToken;
    LSPTR_TYPEHANDLE typeHandle;
};

// DebuggerIPCE_ExpandedTypeData contains more information showing further
// details for array types, byref types etc.
// Whenever you fetch type information from the left-side
// you get back one of these.  These in turn contain further
// DebuggerIPCE_BasicTypeData's and typeHandles which you can
// then query to get further information about the type parameters.
// This copes with the nested cases, e.g. jagged arrays,
// String ****, &(String*), Pair<String,Pair<String>>
// and so on.
//
// So this type information is not "fully expanded", it's just a little
// more detail then DebuggerIPCE_BasicTypeData.  For type
// instantiatons (e.g. List<int>) and
// function pointer types you will need to make further requests for
// information about the type parameters.
// For array types there is always only one type parameter so
// we include that as part of the expanded data.
//
//
struct DebuggerIPCE_ExpandedTypeData
{
    CorElementType  elementType; // Note this is _never_ E_T_VAR, E_T_WITH or E_T_MVAR
    union
    {
        // used for E_T_CLASS and E_T_VALUECLASS, E_T_PTR, E_T_BYREF etc.
        // For non-constructed E_T_CLASS or E_T_VALUECLASS the tokens will be set and the typeHandle will be NULL
        // For constructed E_T_CLASS or E_T_VALUECLASS the tokens will be set and the typeHandle will be non-NULL
        // For E_T_PTR etc. the tokens will be NULL and the typeHandle will be non-NULL.
        struct
         {
            mdTypeDef       metadataToken;
            LSPTR_DMODULE   debuggerModuleToken;
            LSPTR_TYPEHANDLE typeHandle; // if non-null then further fetches will be needed to get type arguments
        } ClassTypeData;

        // used for E_T_PTR, E_T_BYREF etc.
        struct
         {
            DebuggerIPCE_BasicTypeData unaryTypeArg;  // used only when sending back to debugger
        } UnaryTypeData;


        // used for E_T_ARRAY etc.
        struct
        {
          DebuggerIPCE_BasicTypeData arrayTypeArg; // used only when sending back to debugger
            DWORD           arrayRank;
        } ArrayTypeData;

        // used for E_T_FNPTR
        struct
         {
            LSPTR_TYPEHANDLE typeHandle; // if non-null then further fetches needed to get type arguments
        } NaryTypeData;

    };
};

// DebuggerIPCE_TypeArgData is used when sending type arguments
// across to a funceval.  It contains the DebuggerIPCE_ExpandedTypeData describing the
// essence of the type, but the typeHandle and other
// BasicTypeData fields should be zero and will be ignored.
// The DebuggerIPCE_ExpandedTypeData is then followed
// by the required number of type arguments, each of which
// will be a further DebuggerIPCE_TypeArgData record in the stream of
// flattened type argument data.
struct DebuggerIPCE_TypeArgData
{
    DebuggerIPCE_ExpandedTypeData  data;
    SIZE_T                         numTypeArgs; // number of immediate children on the type tree
};


//
// DebuggerIPCE_ObjectData holds the results of a
// GetAndSendObjectInfo, i.e., all the info about an object that the
// Right Side would need to access it. (This include array, string,
// and nstruct info.)
//
struct DebuggerIPCE_ObjectData
{
    void           *objRef;
    bool            objRefBad;
    SIZE_T          objSize;
    SIZE_T          objOffsetToVars;

    // The type of the object....
    struct DebuggerIPCE_ExpandedTypeData objTypeData;

    union
    {
        struct
        {
            SIZE_T          length;
            SIZE_T          offsetToStringBase;
        } stringInfo;

        struct
        {
            SIZE_T          size;
            void           *ptr;
        } nstructInfo;

        struct
        {
            SIZE_T          rank;
            SIZE_T          offsetToArrayBase;
            SIZE_T          offsetToLowerBounds; // 0 if not present
            SIZE_T          offsetToUpperBounds; // 0 if not present
            DWORD           componentCount;
            SIZE_T          elementSize;
        } arrayInfo;

        struct
        {
            struct DebuggerIPCE_BasicTypeData typedByrefType; // the type of the thing contained in a typedByref...
        } typedByrefInfo;
    };
};

//
// Remote enregistered info used by CordbValues and for passing
// variable homes between the left and right sides during a func eval.
//

enum RemoteAddressKind
{
    RAK_NONE = 0,
    RAK_REG,
    RAK_REGREG,
    RAK_REGMEM,
    RAK_MEMREG,
    RAK_FLOAT
};

struct RemoteAddress
{
    RemoteAddressKind    kind;
    void                *frame;

    CorDebugRegister     reg1;
    void                *reg1Addr;

    union
    {
        struct
        {
            CorDebugRegister  reg2;
            void             *reg2Addr;
        } u;

        CORDB_ADDRESS    addr;
    };
};

//
// DebuggerIPCE_FuncEvalType specifies the type of a function
// evaluation that will occur.
//
enum DebuggerIPCE_FuncEvalType
{
    DB_IPCE_FET_NORMAL,
    DB_IPCE_FET_NEW_OBJECT,
    DB_IPCE_FET_NEW_OBJECT_NC,
    DB_IPCE_FET_NEW_STRING,
    DB_IPCE_FET_NEW_ARRAY,
    DB_IPCE_FET_RE_ABORT
};


enum NameChangeType
{
    APP_DOMAIN_NAME_CHANGE,
    THREAD_NAME_CHANGE
};

//
// DebuggerIPCE_FuncEvalArgData holds data for each argument to a
// function evaluation.
//
struct DebuggerIPCE_FuncEvalArgData
{
    RemoteAddress     argHome;  // enregistered variable home
    void             *argAddr;  // address if not enregistered
    CorElementType    argElementType;
    unsigned int      fullArgTypeNodeCount; // Pointer to LS (DebuggerIPCE_TypeArgData *) buffer holding full description of the argument type (if needed - only needed for struct types)
    void             *fullArgType; // Pointer to LS (DebuggerIPCE_TypeArgData *) buffer holding full description of the argument type (if needed - only needed for struct types)
    BYTE              argLiteralData[8]; // copy of generic value data
    bool              argIsLiteral; // true if value is in argLiteralData
    bool              argIsHandleValue; // true if argAddr is OBJECTHANDLE
};


//
// DebuggerIPCE_FuncEvalInfo holds info necessary to setup a func eval
// operation.
//
struct DebuggerIPCE_FuncEvalInfo
{
    LSPTR_THREAD               funcDebuggerThreadToken;
    DebuggerIPCE_FuncEvalType  funcEvalType;
    mdMethodDef                funcMetadataToken;
    mdTypeDef                  funcClassMetadataToken;
    LSPTR_DMODULE              funcDebuggerModuleToken;
    RSPTR_CORDBEVAL            funcEvalKey;
    bool                       evalDuringException;

    SIZE_T                     argCount;
    SIZE_T                     genericArgsCount;
    SIZE_T                     genericArgsNodeCount;

    SIZE_T                     stringSize;

    SIZE_T                     arrayRank;
};

//
// DebuggerIPCFirstChanceData holds info communicated from the LS to the RS when signaling that an exception does not
// belong to the runtime from a first chance hijack. This is used when Win32 debugging only.
//
struct DebuggerIPCFirstChanceData
{
    LSPTR_CONTEXT     pLeftSideContext;
    void             *pOriginalHandler;
};

//
// DebuggerIPCSecondChanceData holds info communicated from the RS
// to the LS when setting up a second chance exception hijack. This is
// used when Win32 debugging only.
//
struct DebuggerIPCSecondChanceData
{
    CONTEXT          threadContext;
};


//-----------------------------------------------------------------------------
// This struct holds pointer from the LS and needs to copy to
// the RS. We have to free the memory on the RS.
// The transfer function is called when the RS first reads the event. At this point, 
// the LS is stopped while sending the event. Thus the LS pointers only need to be
// valid while the LS is in SendIPCEvent.
//-----------------------------------------------------------------------------
struct Ls_Rs_BaseBuffer
{
#ifdef RIGHT_SIDE_COMPILE
protected:
    // copy data can happen on both LS and RS. In LS case,
    // ReadProcessMemory is really reading from its own process memory.
    //
    HRESULT CopyLSDataToRSWorker(HANDLE hProcess)
    {
        BOOL succ;

        const DWORD cbCacheSize = m_cbSize;
        
        // SHOULD not happen for more than once
        _ASSERTE(m_pbRS == NULL);
        m_pbRS = new (nothrow) BYTE[cbCacheSize];
        if (m_pbRS == NULL)
        {
            return E_OUTOFMEMORY;
        }

        succ = ReadProcessMemory(hProcess, m_pbLS, m_pbRS, cbCacheSize , NULL);
        _ASSERTE(succ);

        if (!succ)
        {
            delete [] m_pbRS;
            m_pbRS = NULL;
            return E_FAIL;
        }
        return S_OK;
    }
    
    // retrieve the RS data and own it
    BYTE *TransferRSDataWorker()
    {
        BYTE *pbRS = m_pbRS;
        m_pbRS = NULL;
        return pbRS;
    }
public:
    

    void CleanUp()
    {
        if (m_pbRS != NULL)
        {
            delete [] m_pbRS;
            m_pbRS = NULL;
        }
    }
#else
public:
    // Only LS can call this API
    void SetLsData(BYTE *pbLS, DWORD cbSize)
    {
        m_pbRS = NULL;
        m_pbLS = pbLS;
        m_cbSize = cbSize;
    }
#endif // RIGHT_SIDE_COMPILE

public:
    // Common APIs.
    DWORD  GetSize() { return m_cbSize; }



protected:
    DWORD  m_cbSize;
    BYTE  *m_pbLS;
    BYTE  *m_pbRS;
};

//-----------------------------------------------------------------------------
// Byte wrapper around the buffer.
//-----------------------------------------------------------------------------
struct Ls_Rs_ByteBuffer : public Ls_Rs_BaseBuffer
{
#ifdef RIGHT_SIDE_COMPILE
    BYTE *GetRSPointer() 
    { 
        return m_pbRS;
    }

    HRESULT CopyLSDataToRS(HANDLE hProcess)
    {
        return CopyLSDataToRSWorker(hProcess);
    }
    BYTE *TransferRSData()
    {
        return TransferRSDataWorker();
    }
#endif    
};

//-----------------------------------------------------------------------------
// Wrapper around a Ls_rS_Buffer to get it as a string.
// This can also do some sanity checking.
//-----------------------------------------------------------------------------
struct Ls_Rs_StringBuffer : public Ls_Rs_BaseBuffer
{
#ifdef RIGHT_SIDE_COMPILE
    const WCHAR * GetString()
    {
        return reinterpret_cast<const WCHAR*> (m_pbRS);
    }

    // Copy over the string.
    HRESULT CopyLSDataToRS(HANDLE hProcess)
    {        
        HRESULT hr = CopyLSDataToRSWorker(hProcess);
        if (FAILED(hr))
        {
            return hr;
        }
        // Ensure we're a valid, well-formed string.
        // - null terminated.
        // - no embedded nulls.
        const WCHAR * pString = GetString();
        SIZE_T dwExpectedLen = m_cbSize / sizeof(WCHAR);
        SIZE_T dwActualLen = wcslen(pString);
        if (dwActualLen != dwExpectedLen)
        {
            return E_INVALIDARG;
        }
        return S_OK;        
    }

    // Caller will pick up ownership.
    // Since caller will delete this data, we can't give back a constant pointer.
    WCHAR * TransferStringData()
    {
        return reinterpret_cast<WCHAR*> (TransferRSDataWorker());
    }
#endif  
};



// Data for an Managed Debug Assistant Probe (MDA).
struct DebuggerMDANotification
{
    Ls_Rs_StringBuffer szName;
    Ls_Rs_StringBuffer szDescription;
    Ls_Rs_StringBuffer szXml;
    DWORD        dwOSThreadId;
    CorDebugMDAFlags flags;
};

//
// Event structure that is passed between the Runtime Controller and the
// Debugger Interface. Some types of events are a fixed size and have
// entries in the main union, while others are variable length and have
// more specialized data structures that are attached to the end of this
// structure.
//
struct DebuggerIPCEvent
{
    DebuggerIPCEvent*       next;
    DebuggerIPCEventType    type;
    DWORD             processId;
    LSPTR_APPDOMAIN   appDomainToken;
    DWORD             threadId;
    HRESULT           hr;
    bool              replyRequired;
    bool              asyncSend;

    union
    {
        struct
        {
            BOOL fAttachToAllAppDomains;
            ULONG id;
        } DebuggerAttachData;

        struct
        {
            ULONG id;
            BOOL fIsDefaultDomain;
        } AppDomainData;

        struct
        {
            LSPTR_ASSEMBLY debuggerAssemblyToken;
            BOOL  fIsSystemAssembly;
            EmbeddedIPCString<MAX_PATH> rcName;
        } AssemblyData;

        struct
        {
            LSPTR_THREAD debuggerThreadToken;
            HANDLE  threadHandle;
            void*   firstExceptionHandler; //points to the beginning of hte SEH chain
        } ThreadAttachData;

        struct
        {
            LSPTR_THREAD debuggerThreadToken;
        } StackTraceData;

        struct
        {
            unsigned int          totalFrameCount;
            unsigned int          totalChainCount;
            unsigned int          traceCount;
            CorDebugUserState     threadUserState;

            // NULL if thread was stopped,
            // nonNULL if thread is in an exception
            // (in which case, it's the address
            // of the memory to get the
            // right CONTEXT from)
            LSPTR_CONTEXT         pContext;

            DebuggerIPCE_STRData  traceData;
        } StackTraceResultData;

        struct
        {
            LSPTR_DMODULE debuggerModuleToken;
            LSPTR_ASSEMBLY debuggerAssemblyToken;
            void* pMetadataStart;
            ULONG nMetadataSize;
            void* pPEBaseAddress;
            ULONG nPESize;
            BOOL  fIsDynamic;
            BOOL  fInMemory;
            EmbeddedIPCString<MAX_PATH> rcName;
            EmbeddedIPCString<MAX_PATH> rcFullNgenName; // 0-length if not-ngenned.
        } LoadModuleData;

        struct
        {
            LSPTR_DMODULE debuggerModuleToken;
            LSPTR_ASSEMBLY debuggerAssemblyToken;
        } UnloadModuleData;

        struct
        {
            LSPTR_DMODULE debuggerModuleToken;
            LSPTR_APPDOMAIN debuggerAppDomainToken;
            Ls_Rs_ByteBuffer dataBuffer;            
        } UpdateModuleSymsData;

        struct
        {
            mdMethodDef funcMetadataToken;
            DWORD       funcRVA; // future...
            DWORD       implFlags; // future...
            LSPTR_DMODULE funcDebuggerModuleToken;
            SIZE_T      nVersion;
        } GetFunctionData;

        DebuggerMDANotification MDANotification;

        struct
        {
            DebuggerIPCE_FuncData basicData;
            // The field below gives details about one native-code version of the method
            // should we have one to hand.  This is NOT necessarily the unique JITting of this version of
            // the method, as generic methods can be JITted more than once.  However, for
            // non-generic methods it is the unique version.  This lets us implement certain V1
            // functionality in a non-breaking way (in particular anything to do with looking
            // at the native code for IL or setting breakpoints using native code offsets), i.e.
            // we want to be able to fetch a sample blob of native code if we can.
            DebuggerIPCE_JITFuncData possibleNativeData;
        } FunctionData;

        struct
        {
            LSPTR_METHODDESC nativeCodeMethodDescToken; // points to the MethodDesc
            LSPTR_DJI  nativeCodeJITInfoToken; // points to the DebuggerJitInfo structure
        } GetJITInfo;

        struct
        {
            unsigned int            argumentCount;
            unsigned int            totalNativeInfos;
            unsigned int            nativeInfoCount; // for this event only
            ICorJitInfo::NativeVarInfo nativeInfo;
        } GetJITInfoResult;

        struct
        {
            LSPTR_BREAKPOINT breakpointToken;
            mdMethodDef  funcMetadataToken;
            LSPTR_DMODULE funcDebuggerModuleToken;
            bool         isIL;
            SIZE_T       offset;
            SIZE_T       encVersion;
            LSPTR_METHODDESC  nativeCodeMethodDescToken; // points to the MethodDesc if !isIL
            LSPTR_DJI  nativeCodeJITInfoToken; // points to optional DebuggerJitInfo if !isIL
        } BreakpointData;

        struct
        {
            LSPTR_BREAKPOINT breakpointToken;
        } BreakpointSetErrorData;

        struct
        {
            LSPTR_STEPPER        stepperToken;
            LSPTR_THREAD         threadToken;
            FramePointer         frameToken;
            bool                 stepIn;
            bool                 rangeIL;
            bool                 IsJMCStop;
            unsigned int         totalRangeCount;
            CorDebugStepReason   reason;
            CorDebugUnmappedStop rgfMappingStop;
            CorDebugIntercept    rgfInterceptStop;
            unsigned int         rangeCount;
            COR_DEBUG_STEP_RANGE range; //note that this is an array
        } StepData;

        struct
        {
            // The Object reference can be stored in several ways:
            // - as an ObjectHandle. (objectHandle is set, RefAddress, RefIsValue are ignored)
            // - as a ptr into the RS process. (objHandle ignored, RefAddres=ptr, isValue=true)
            // - as a ptr into the LS process. (objHandle ignored, RefAddres=ptr, isValue=false)
            LSPTR_OBJECTHANDLE objectHandle;
            void           *objectRefAddress;
            bool            objectRefIsValue;

            CorElementType  objectType;
        } GetObjectInfo;

        struct
        {
            // An unvalidated GC-handle
            LSPTR_OBJECTHANDLE GCHandle;
        } GetGCHandleInfo;

        struct
        {
            // An unvalidated GC-handle for which we're returning the results
            LSPTR_OBJECTHANDLE GCHandle;

            // The following are initialized by the LS in response to our query:
            LSPTR_APPDOMAIN pLSAppDomain; // AD that handle is in (only applicable if fValid).
            bool            fValid; // Did the LS determine the GC handle to be valid?
        } GetGCHandleInfoResult;

        struct DebuggerIPCE_ObjectData GetObjectInfoResult;

        struct
        {
            LSPTR_TYPEHANDLE typeHandle;
        } GetTypeHandleParams;

        struct
        {
            mdTypeDef  classMetadataToken;
            //void      *classDebuggerModuleToken;
            unsigned int           totalGenericArgsCount; // total.  Entry valid in the first event only
            unsigned int           genericArgsCount; // for this event only - multiple events may get sent....
            DebuggerIPCE_ExpandedTypeData  genericArgs; // array of type parameters off the end....
        } GetTypeHandleParamsResult;

        struct
        {
            LSPTR_TYPEHANDLE typeHandle;
        } ExpandType;
        DebuggerIPCE_ExpandedTypeData        ExpandTypeResult;

        struct
        {
            LSPTR_METHODDESC       methodDesc;
            // exactGenericArgsToken is extra information extracted from the frame executing the methodDesc
            // when we are getting runtime type information for a frame.
            // It will be a handle to the class instantiation
            // or the exact method descriptor according to the nature of the method being
            // executed.  If NULL and the method requires runtime type information on the frame
            // then exact information is not available (the JIT has optimized it away)
            // and we return the approximate parameters instead.
            void                   *exactGenericArgsToken;
        } GetMethodDescParams;

        struct
        {
            unsigned int           totalGenericArgsCount; // total.  Entry valid in the first event only
            unsigned int           totalGenericClassArgsCount; // how many are class type parameters?
            unsigned int           genericArgsCount; // for this event only - multiple events may get sent....
            DebuggerIPCE_ExpandedTypeData  genericArgs; // array of type parameters off the end....
        } GetMethodDescParamsResult;

        struct
        {
            CorElementType  elementType;
            mdTypeDef       metadataToken;
            LSPTR_DMODULE   debuggerModuleToken;
            LSPTR_TYPEHANDLE typeHandleExact;
            LSPTR_TYPEHANDLE typeHandleApprox;
        } GetClassInfo;

        struct
        {
            bool                   isValueClass;
            unsigned int           genericArgsCount;
            SIZE_T                 objectSize;
            unsigned int           varCount;
            unsigned int           fieldCount; // for this event only
            DebuggerIPCE_FieldData fieldData;
        } GetClassInfoResult;

        CorElementType GetSimpleType;

        struct
        {
            mdTypeDef       metadataToken;
            LSPTR_DMODULE   debuggerModuleToken;
        } GetSimpleTypeResult;

        struct
        {
            DebuggerIPCE_ExpandedTypeData typeData;
            unsigned int           genericArgsCount;
            void                  *genericArgsBuffer; // array of type parameters stored in buffer on LS....
        } GetTypeHandle;

        struct
        {
            LSPTR_TYPEHANDLE typeHandleExact;
        } GetTypeHandleResult;

        struct
        {
            unsigned int           typeDataNodeCount;
            void                  *typeDataBuffer; // a specification of the type as data
        } GetApproxTypeHandle;

        struct
        {
            LSPTR_TYPEHANDLE typeHandleApprox;
        } GetApproxTypeHandleResult;

        struct
        {
            mdMethodDef funcMetadataToken;
            LSPTR_DMODULE funcDebuggerModuleToken;
            bool        il;
            SIZE_T      start, end;
            BYTE        code;
            LSPTR_METHODDESC nativeCodeMethodDescToken;  // if this is set then we're getting the native code data
            LSPTR_DJI   nativeCodeJITInfoToken;
        } GetCodeData;

        struct
        {
            ULONG      bufSize;
        } GetBuffer;

        struct
        {
            void        *pBuffer;
            HRESULT     hr;
        } GetBufferResult;

        struct
        {
            void        *pBuffer;
        } ReleaseBuffer;

        struct
        {
            HRESULT     hr;
        } ReleaseBufferResult;

        struct
        {
            LSPTR_DMODULE debuggerModuleToken;
            DWORD cbDeltaMetadata;
            BYTE *pDeltaMetadata;
            DWORD cbDeltaIL;
            BYTE *pDeltaIL;
        } ApplyChanges;

        struct
        {
            HRESULT hr;
        } ApplyChangesResult;

        struct
        {
            LSPTR_THREAD debuggerThreadToken;
        } GetFloatState;

        struct
        {
            bool         floatStateValid;
            unsigned int floatStackTop;
            double       floatValues[DebuggerIPCE_FloatCount];
        } GetFloatStateResult;

        struct
        {
            mdTypeDef   classMetadataToken;
            LSPTR_DMODULE classDebuggerModuleToken;
            LSPTR_ASSEMBLY classDebuggerAssemblyToken;
            BYTE       *pNewMetaData; // This is only valid if the class
                // is on a dynamic module, and therefore the metadata needs to
                // be refreshed.
            DWORD       cbNewMetaData;
        } LoadClass;

        struct
        {
            mdTypeDef   classMetadataToken;
            LSPTR_DMODULE classDebuggerModuleToken;
            LSPTR_ASSEMBLY classDebuggerAssemblyToken;
        } UnloadClass;

        struct
        {
            LSPTR_DMODULE debuggerModuleToken;
            bool  flag;
        } SetClassLoad;

        struct
        {
            LSPTR_OBJECTHANDLE exceptionHandle;
            bool        firstChance;
            bool        continuable;
        } Exception;

        struct
        {
            LSPTR_THREAD   debuggerThreadToken;
        } ClearException;

        struct
        {
            void        *address;
        } IsTransitionStub;

        struct
        {
            bool        isStub;
        } IsTransitionStubResult;

        struct
        {
            bool        fCanSetIPOnly;
            LSPTR_THREAD debuggerThreadToken;
            LSPTR_DMODULE debuggerModule;
            mdMethodDef mdMethod;
            LSPTR_DJI   nativeCodeJITInfoToken;
            SIZE_T      offset;
            bool        fIsIL;
            void        *firstExceptionHandler;
        } SetIP; // this is also used for CanSetIP

        struct
        {
            int iLevel;

            EmbeddedIPCString<MAX_LOG_SWITCH_NAME_LEN + 1> szCategory;
            Ls_Rs_StringBuffer szContent;
        } FirstLogMessage;

        struct
        {
            int iLevel;
            int iReason;

            EmbeddedIPCString<MAX_LOG_SWITCH_NAME_LEN + 1> szSwitchName;
            EmbeddedIPCString<MAX_LOG_SWITCH_NAME_LEN + 1> szParentSwitchName;
        } LogSwitchSettingMessage;

        struct
        {
            LSPTR_THREAD debuggerThreadToken;
            CorDebugThreadState debugState;
        } SetDebugState;

        struct
        {
            LSPTR_THREAD debuggerExceptThreadToken;
            CorDebugThreadState debugState;
        } SetAllDebugState;

        DebuggerIPCE_FuncEvalInfo FuncEval;

        struct
        {
            BYTE           *argDataArea;
            LSPTR_DEBUGGEREVAL debuggerEvalKey;
        } FuncEvalSetupComplete;

        struct
        {
            RSPTR_CORDBEVAL funcEvalKey;
            bool            successful;
            bool            aborted;
            void           *resultAddr;
            LSPTR_APPDOMAIN resultAppDomainToken;
            LSPTR_OBJECTHANDLE objectHandle;
            DebuggerIPCE_ExpandedTypeData resultType;
        } FuncEvalComplete;

        struct
        {
            LSPTR_DEBUGGEREVAL debuggerEvalKey;
        } FuncEvalAbort;

        struct
        {
            LSPTR_DEBUGGEREVAL debuggerEvalKey;
        } FuncEvalRudeAbort;

        struct
        {
            LSPTR_DEBUGGEREVAL debuggerEvalKey;
        } FuncEvalCleanup;

        struct
        {
            void           *objectRefAddress;
            LSPTR_OBJECTHANDLE objectHandle;
            void           *newReference;
        } SetReference;

        struct
        {
            NameChangeType  eventType;
            LSPTR_APPDOMAIN debuggerAppDomainToken;
            DWORD           debuggerThreadIdToken;
        } NameChange;

        struct
        {
            LSPTR_THREAD     debuggerObjectToken;
            LSPTR_OBJECTHANDLE managedObject;
        } ObjectRef;

        struct
        {
            LSPTR_DMODULE    debuggerModuleToken;
            BOOL             fAllowJitOpts;
            BOOL             fEnableEnC;
        } JitDebugInfo;

        struct
        {
            LSPTR_FIELDDESC  fldDebuggerToken;
            LSPTR_THREAD     debuggerThreadToken;
        } GetSpecialStatic;

        struct
        {
            void            *fldAddress;
        } GetSpecialStaticResult;

        struct
        {
            LSPTR_DMODULE debuggerModuleToken; //LS pointer to DebuggerModule
            mdMethodDef funcMetadataToken ;
            SIZE_T          currentVersionNumber;
            SIZE_T          resumeVersionNumber;
            SIZE_T          currentILOffset;
            SIZE_T          *resumeILOffset;
        } EnCRemap;

        struct
        {
            LSPTR_DMODULE debuggerModuleToken; //LS pointer to DebuggerModule
            mdMethodDef funcMetadataToken ;
        } EnCRemapComplete;

        struct
        {
            LSPTR_DMODULE   debuggerModuleToken; //LS pointer to DebuggerModule
            mdToken                 memberMetadataToken ;
            mdTypeDef              classMetadataToken ;
            SIZE_T          newVersionNumber;
        } EnCUpdate;

        struct
        {
            DebuggerIPCE_BasicTypeData objectTypeData;
            void            *pObject;
            SIZE_T           offsetToVars;
            mdFieldDef       fldToken;
            void            *GCstaticVarBase;
            void            *NonGCstaticVarBase;
        } GetSyncBlockField;

        struct
        {
            // If it's static, then we don't have to refresh
            // it later since the object/int/etc will be right
            // there.
            BOOL                   fStatic;
            DebuggerIPCE_FieldData fieldData;
        } GetSyncBlockFieldResult;

        struct
        {
            void      *oldData;
            void      *newData;
            DebuggerIPCE_BasicTypeData type;
        } SetValueClass;


        struct
        {
            LSPTR_DMODULE   debuggerModuleToken;
            mdMethodDef     funcMetadataToken;
            DWORD           dwStatus;
        } SetJMCFunctionStatus;

        struct
        {
            TASKID      taskid;
        } GetThreadForTaskId;

        struct
        {
            LSPTR_THREAD debuggerThreadToken;
        } GetThreadForTaskIdResult;

        struct
        {
            LSPTR_THREAD debuggerThreadToken;
        } GetTaskId;

        struct
        {
            TASKID    taskId;
        } GetTaskIdResult;

        struct
        {
            LSPTR_THREAD debuggerThreadToken;
        } GetConnectionId;

        struct
        {
            CONNID    connectionId;
        } GetConnectionIdResult;

        struct
        {
            LSPTR_THREAD debuggerThreadToken;
        } GetOSThreadId;

        struct
        {
            DWORD      osThreadId;
        } GetOSThreadIdResult;

        struct
        {
            LSPTR_THREAD debuggerThreadToken;
        } GetThreadHandle;

        struct
        {
            HANDLE     handle;
        } GetThreadHandleResult;


        struct
        {
            CONNID     connectionId;
        } ConnectionChange;

        struct
        {
            CONNID     connectionId;
            EmbeddedIPCString<MAX_PATH> wzConnectionName;
        } CreateConnection;

        struct
        {
            void            *objectToken;
            BOOL          fStrong;
        } CreateHandle;

        struct
        {
            LSPTR_OBJECTHANDLE objectHandle;
        } CreateHandleResult;

        // used in DB_IPCE_DISPOSE_HANDLE event
        struct
        {
            LSPTR_OBJECTHANDLE objectHandle;
            BOOL            fStrong;
        } DisposeHandle;

        struct
        {
            FramePointer                  framePointer;
            SIZE_T                        nOffset;
            CorDebugExceptionCallbackType eventType;
            DWORD                         dwFlags;
            LSPTR_OBJECTHANDLE            exceptionHandle;
        } ExceptionCallback2;

        struct
        {
            CorDebugExceptionUnwindCallbackType eventType;
            DWORD                               dwFlags;
        } ExceptionUnwind;

        struct
        {
            LSPTR_THREAD threadToken;
            FramePointer frameToken;
        } InterceptException;

        struct
        {
            LSPTR_THREAD threadToken;
            DWORD exceptionNumber;
        } GetException;

        struct
        {
            LSPTR_OBJECTHANDLE exceptionHandle;
        } GetExceptionResult;

        struct
        {
            LSPTR_DMODULE refingModuleToken;
            mdToken       assemblyRefToken;
        } ResolveAssembly;

        struct
        {
            LSPTR_ASSEMBLY debuggerAssemblyToken;
        } ResolveAssemblyResult;

        struct
        {
            LSPTR_DMODULE refingModuleToken;
            mdToken       typeRefToken;
        } ResolveTypeRef;

        struct
        {
            LSPTR_DMODULE debuggerModuleToken;
            mdToken       typeDefToken;
        } ResolveTypeRefResult;

        struct
        {
            LSPTR_ASSEMBLY debuggerAssemblyToken;
            LSPTR_APPDOMAIN debuggerAppDomainToken;
        } GetAssemblyTrust;

        struct
        {
            BOOL fIsFullyTrusted;
        } GetAssemblyTrustResult;

        struct
        {
            LSPTR_THREAD threadToken;
        } GetCurrentAppDomain;
        
        struct
        {
            LSPTR_APPDOMAIN currentAppDomainToken;
        } GetCurrentAppDomainResult;
    };
};

// A DebuggerIPCEvent must fit in the send & receive buffers, which are CorDBIPC_BUFFER_SIZE bytes.
C_ASSERT(sizeof(DebuggerIPCEvent) < CorDBIPC_BUFFER_SIZE);


// 2*sizeof(WCHAR) for the two string terminating characters in the FirstLogMessage
#define LOG_MSG_PADDING         4

#endif /* _DbgIPCEvents_h_ */
