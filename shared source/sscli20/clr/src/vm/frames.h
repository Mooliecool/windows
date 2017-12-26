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
// FRAMES.H -
//
// These C++ classes expose activation frames to the rest of the EE.
// Activation frames are actually created by JIT-generated or stub-generated
// code on the machine stack. Thus, the layout of the Frame classes and
// the JIT/Stub code generators are tightly interwined.
//
// IMPORTANT: Since frames are not actually constructed by C++,
// don't try to define constructor/destructor functions. They won't get
// called.
//
// IMPORTANT: Not all methods have full-fledged activation frames (in
// particular, the JIT may create frameless methods.) This is one reason
// why Frame doesn't expose a public "Next()" method: such a method would
// skip frameless method calls. You must instead use one of the
// StackWalk methods.
//
//
// The following is the hierarchy of frames:
//
//    Frame                     - the root class. There are no actual instances
//    |                           of Frames.
//    |
//    |
//    +-GCFrame                 - this frame doesn't represent a method call.
//    |                           it's sole purpose is to let the EE gc-protect
//    |                           object references that it is manipulating.
//    |
//    +-CustomGCFrame           - it lets you specify your own GC scanning function.
//    |
//    |
//    +-HelperMethodFrame       - frame used allow stack crawling inside jit helpers and fcalls
//    | |
//    + +-HelperMethodFrame_1OBJ- reports additional object references
//    | |
//    + +-HelperMethodFrame_2OBJ- reports additional object references
//    | |
//    + +-HelperMethodFrame_PROTECTOBJ - reports additional object references
//    |
//    +-TransitionFrame         - this abstract frame represents a transition from
//    | |                         one or more nested frameless method calls
//    | |                         to either a EE runtime helper function or
//    | |                         a framed method.
//    | |
//    | +- FaultingExceptionFrame - this frame was placed on a method which faulted
//    | |                           to save additional state information
//    | |
//    | +-FuncEvalFrame         - frame for debugger function evaluation
//    | |
//    | +-FramedMethodFrame     - this abstract frame represents a call to a method
//    |   |                       that generates a full-fledged frame.
//    |   |
//    |   |
//    |   +-DelegateTransitionFrame - represents a DllImport call through a delegate.
//    |   |
//    |   +-NDirectMethodFrame      - abstract class to represent an N/Direct call.
//    |   | |
//    |   | +-NDirectMethodFrameEx  - abstract class to represent an N/Direct call w/ cleanup
//    |   | | |
//    |   | | +-NDirectMethodFrameSlim
//    |   | | |
//    |   | | +-NDirectMethodFrameStandaloneCleanup
//    |   | |
//    |   | +-NDirectMethodFrameStandalone
//    |   |
//    |   |
//    |   |
//    |   +-PrestubMethodFrame  - represents a call to a prestub
//    |   |
//    |   |
//    |
//    +-UnmanagedToManagedFrame - this frame represents a transition from
//    | |                         unmanaged code back to managed code. It's
//    | |                         main functions are to stop COM+ exception
//    | |                         propagation and to expose unmanaged parameters.
//    | |
//    | +-UnmanagedToManagedCallFrame - this frame is used when the target
//    |   |                             is a COM+ function or method call. it
//    |   |                             adds the capability to gc-promote callee
//    |   |                             arguments during marshaling.
//    |   +-UMThkCallFrame      - this frame represents an unmanaged->managed
//    |                           transition through N/Direct
//    |
//    +-ContextTransitionFrame  - this frame is used to mark an appdomain transition
//    |
//    |
#ifdef _X86_
//    |
//    +-TailCallFrame- padding for virtual stub dispatch tailcalls
#endif
//    |
//    +-ProtectByRefsFrame
//    |
//    +-ProtectValueClassFrame
//    |
//    +-DebuggerClassInitMarkFrame - marker frame to indicate that "class init" code is running
//    |
//    +-DebuggerSecurityCodeMarkFrame - marker frame to indicate that security code is running
//    |
//    +-DebuggerExitFrame - marker frame to indicate that a "break" IL instruction is being executed
//    |
//    +-ReverseEnterRuntimeFrame
//    |
//    +-LeaveRuntimeFrame
//    |
//    +-ExceptionFilterFrame - this frame wraps call to exception filter
//    |
//    +-SecurityContextFrame - place the security context of an assembly on the stack to ensure it will be included in security demands
//
//------------------------------------------------------------------------
//------------------------------------------------------------------------

#ifndef FRAME_ABSTRACT_TYPE_NAME
#define FRAME_ABSTRACT_TYPE_NAME(frameType)
#endif
#ifndef FRAME_TYPE_NAME
#define FRAME_TYPE_NAME(frameType)
#endif

FRAME_ABSTRACT_TYPE_NAME(FrameBase)
FRAME_ABSTRACT_TYPE_NAME(Frame)
FRAME_ABSTRACT_TYPE_NAME(TransitionFrame)
FRAME_TYPE_NAME(FaultingExceptionFrame)
FRAME_TYPE_NAME(FuncEvalFrame)
FRAME_TYPE_NAME(HelperMethodFrame)
FRAME_TYPE_NAME(HelperMethodFrame_1OBJ)
FRAME_TYPE_NAME(HelperMethodFrame_2OBJ)
FRAME_TYPE_NAME(HelperMethodFrame_PROTECTOBJ)
FRAME_ABSTRACT_TYPE_NAME(FramedMethodFrame)
FRAME_TYPE_NAME(TPMethodFrame)
FRAME_TYPE_NAME(DelegateTransitionFrame)
FRAME_ABSTRACT_TYPE_NAME(NDirectMethodFrame)
FRAME_ABSTRACT_TYPE_NAME(NDirectMethodFrameEx)
FRAME_TYPE_NAME(NDirectMethodFrameGeneric)
#if defined(_X86_)
FRAME_TYPE_NAME(NDirectMethodFrameSlim)
#endif // _X86_
FRAME_TYPE_NAME(NDirectMethodFrameStandalone)
FRAME_TYPE_NAME(NDirectMethodFrameStandaloneCleanup)
FRAME_TYPE_NAME(SecureDelegateFrame)
FRAME_TYPE_NAME(MulticastFrame)
FRAME_ABSTRACT_TYPE_NAME(UnmanagedToManagedFrame)
FRAME_ABSTRACT_TYPE_NAME(UnmanagedToManagedCallFrame)
FRAME_TYPE_NAME(SecurityFrame)
FRAME_TYPE_NAME(PrestubMethodFrame)
FRAME_TYPE_NAME(InterceptorFrame)
FRAME_TYPE_NAME(GCFrame)
FRAME_TYPE_NAME(CustomGCFrame)
FRAME_TYPE_NAME(ProtectByRefsFrame)
FRAME_TYPE_NAME(ProtectValueClassFrame)
FRAME_TYPE_NAME(DebuggerClassInitMarkFrame)
FRAME_TYPE_NAME(DebuggerSecurityCodeMarkFrame)
FRAME_TYPE_NAME(DebuggerExitFrame)
FRAME_TYPE_NAME(UMThkCallFrame)
FRAME_TYPE_NAME(ReverseEnterRuntimeFrame)
FRAME_TYPE_NAME(LeaveRuntimeFrame)
FRAME_TYPE_NAME(InlinedCallFrame)
FRAME_TYPE_NAME(ContextTransitionFrame)
#ifdef _X86_
FRAME_TYPE_NAME(TailCallFrame)
#endif // _X86_
FRAME_TYPE_NAME(ExceptionFilterFrame)
#if defined(_DEBUG)
FRAME_TYPE_NAME(AssumeByrefFromJITStack)
#endif // _DEBUG
FRAME_TYPE_NAME(SecurityContextFrame)

#undef FRAME_ABSTRACT_TYPE_NAME
#undef FRAME_TYPE_NAME

//------------------------------------------------------------------------

#ifndef __frames_h__
#define __frames_h__

#include "util.hpp"
#include "vars.hpp"
#include "object.h"
#include "objecthandle.h"
#include "regdisp.h"
#include <stddef.h>
#include "siginfo.hpp"
// context headers
#include "context.h"
#include "method.hpp"
#include "stackwalk.h"
#include "stubmgr.h"
#include "gms.h"
#include "threads.h"
// remoting headers
//#include "remoting.h"

// Forward references
class Frame;
class FieldMarshaler;
class FramedMethodFrame;
struct HijackArgs;
class UMEntryThunk;
class UMThunkMarshInfo;
class Marshaler;
class SecurityDescriptor;
struct ResolveCacheElem;


//
//
//
//
//

class CleanupWorkList
{
    public:
        //-------------------------------------------------------------------
        // Constructor.
        //-------------------------------------------------------------------
        CleanupWorkList()
        {
            // NOTE: IF YOU CHANGE THIS, YOU WILL ALSO HAVE TO CHANGE SOME
            // STUBS.
            LEAF_CONTRACT;

            m_pNodes = NULL;
        }

        //-------------------------------------------------------------------
        // Destructor (calls Cleanup(FALSE))
        //-------------------------------------------------------------------
        ~CleanupWorkList();


        //-------------------------------------------------------------------
        // Executes each stored cleanup task and resets the worklist back
        // to empty. Some task types are conditional based on the
        // "fBecauseOfException" flag. This flag distinguishes between
        // cleanups due to normal method termination and cleanups due to
        // an exception.
        //-------------------------------------------------------------------
        VOID __stdcall Cleanup(BOOL fBecauseOfException);


        //-------------------------------------------------------------------
        // CoTaskFree memory unconditionally
        //-------------------------------------------------------------------
        VOID ScheduleCoTaskFree(LPVOID pv);
        HRESULT ScheduleCoTaskFreeNonThrow(LPVOID pv);

        //-------------------------------------------------------------------
        // StackingAllocator.Collapse during exceptions
        //-------------------------------------------------------------------
        VOID ScheduleFastFree(LPVOID checkpoint);



        //-------------------------------------------------------------------
        // Schedules an unconditional free of the native version
        // of an NStruct reference field. Note that pNativeData points into
        // the middle of the external part of the NStruct, so someone
        // has to hold a gc reference to the wrapping NStruct until
        // the destroy is done.
        //-------------------------------------------------------------------
        VOID ScheduleUnconditionalNStructDestroy(const FieldMarshaler *pFieldMarshaler, LPVOID pNativeData);


        //-------------------------------------------------------------------
        // CleanupWorkList::ScheduleUnconditionalCultureRestore
        // schedule restoring thread's current culture to the specified
        // culture.
        // Throws a COM+ exception if failed.
        //-------------------------------------------------------------------
        VOID ScheduleUnconditionalCultureRestore(OBJECTREF *pCultureObj);

        //-------------------------------------------------------------------
        // CleanupWorkList::ScheduleLayoutDestroy
        // schedule cleanup of marshaled struct fields and of the struct itself.
        // Throws a COM+ exception if failed.
        //-------------------------------------------------------------------
        LPVOID NewScheduleLayoutDestroyNative(MethodTable *pMT);

        //-------------------------------------------------------------------
        // CleanupWorkList::ScheduleSafeHandleRelease
        // schedule releasing the SafeHandle which decreses it's ref count
        // by 1.
        // Throws a COM+ exception if failed.
        //-------------------------------------------------------------------
        VOID ScheduleSafeHandleRelease(SAFEHANDLE *pSafeHandleObj);

        //-------------------------------------------------------------------
        // CleanupWorkList::ScheduleRefSafeHandleBackPropagate
        // schedule back propagating the handle to the safe handle.
        // Back propagation only happens if the new handle is different from the
        // original value (this matters since the propagation is to a different
        // safe handle instance).
        // Throws a COM+ exception if failed.
        //-------------------------------------------------------------------
        VOID ScheduleRefSafeHandleBackPropagate(SAFEHANDLE *pSafeHandleObj, LPVOID *pHandle, LPVOID hOldHandle);

        //-------------------------------------------------------------------
        // CleanupWorkList::ScheduleRefCriticalHandleBackPropagate
        // schedule back propagating the handle to the critical handle.
        // Throws a COM+ exception if failed.
        //-------------------------------------------------------------------
        VOID ScheduleRefCriticalHandleBackPropagate(CRITICALHANDLE *pCriticalHandleObj, LPVOID *pHandle, LPVOID hOldHandle);


        //-------------------------------------------------------------------
        // CleanupWorkList::NewProtectedObjRef()
        // holds a protected objref (used for creating the buffer for
        // an unmanaged->managed byref object marshal. We can't use an
        // objecthandle because modifying those without using the handle
        // api opens up writebarrier violations.
        //
        // Must have called IsVisibleToGc() first.
        //-------------------------------------------------------------------
        OBJECTREF* NewProtectedObjectRef(OBJECTREF oref);

        //-------------------------------------------------------------------
        // CleanupWorkList::NewProtectedObjRef()
        // holds a Marshaler. The cleanupworklist will own the task
        // of calling the marshaler's GcScanRoots fcn.
        //
        // It makes little architectural sense for the CleanupWorkList to
        // own this item. But it's late in the project to be adding
        // fields to frames, and it so happens everyplace we need this thing,
        // there's alreay a cleanuplist. So it's elected.
        //
        // Must have called IsVisibleToGc() first.
        //-------------------------------------------------------------------
        VOID NewProtectedMarshaler(Marshaler *pMarshaler);


        //-------------------------------------------------------------------
        // CleanupWorkList::ScheduleMarshalerCleanupOnException()
        // holds a Marshaler. The cleanupworklist will own the task
        // of calling the marshaler's DoExceptionCleanup() if an exception
        // occurs.
        //
        // The return value is a cookie thru which the marshaler can
        // cancel this item. It must do this once to avoid double
        // destruction if the marshaler cleanups normally.
        //-------------------------------------------------------------------
        class MarshalerCleanupNode;
        MarshalerCleanupNode *ScheduleMarshalerCleanupOnException(Marshaler *pMarshaler);


        //-------------------------------------------------------------------
        // CleanupWorkList::IsVisibleToGc()
        //-------------------------------------------------------------------
        VOID IsVisibleToGc()
        {
#ifdef _DEBUG
            WRAPPER_CONTRACT;
            if (!Schedule(CL_ISVISIBLETOGC, NULL))
            {
                INSTALL_UNWIND_AND_CONTINUE_HANDLER;
                ThrowOutOfMemory();
                UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
            }
#else
            LEAF_CONTRACT;
#endif
        }



        //-------------------------------------------------------------------
        // If you've called IsVisibleToGc(), must call this.
        //-------------------------------------------------------------------
        void GcScanRoots(promote_func *fn, ScanContext* sc);




    private:
        //-------------------------------------------------------------------
        // Cleanup task types.
        //-------------------------------------------------------------------
        enum CleanupType {
            CL_COTASKFREE,      // unconditional cotaskfree
            CL_FASTFREE,       // unconditionally StackingAllocator.Collapse
            CL_NSTRUCTDESTROY, // unconditionally do a DestroyNative on an NStruct ref field
            CL_RESTORECULTURE, // unconditionally restore the culture
            CL_NEWLAYOUTDESTROYNATIVE,

            CL_PROTECTEDOBJREF, // holds a GC protected OBJECTREF - similar to CL_GCHANDLE
                             // but can be safely written into without updating
                             // write barrier.
                             //
                             // Must call IsVisibleToGc() before using this nodetype.
                             //
            CL_PROTECTEDMARSHALER, // holds a GC protected marshaler
                             // Must call IsVisibleToGc() before using this nodetype.


            CL_ISVISIBLETOGC,// a special do-nothing nodetype that simply
                             // records that "IsVisibleToGc()" was called on this.


            CL_MARSHALER_EXCEP, // holds a marshaler for cleanup on exception
            CL_MARSHALERREINIT_EXCEP, // holds a marshaler for reiniting on exception
            CL_RELEASESAFEHANDLE,     // releases the ref count on a safe handle
            CL_BACKPROPAGATESAFEHANDLE, // back propagate a safe handle.
            CL_BACKPROPAGATECRITICALHANDLE, // back propagate a critical handle.
        };

        //-------------------------------------------------------------------
        // These are linked into a list.
        //-------------------------------------------------------------------
        struct CleanupNode {
            CleanupType     m_type;       // See CleanupType enumeration
            DPTR(CleanupNode) m_next;     // pointer to next task
#ifdef _DEBUG
            ADID m_dwDomainId;           // domain Id of list.
#endif
            union {
                Object*     m_oref;       // CL_PROTECTEDOBJREF
                IUnknown   *m_ip;         // if CL_RELEASE
                LPVOID      m_pv;         // CleanupType-dependent contents.
                SAFEARRAY  *m_safearray;
                Thread     *m_pThread;
                Marshaler  *m_pMarshaler;


                struct {                  // if CL_NSTRUCTDESTROY
                    const FieldMarshaler *m_pFieldMarshaler;
                    LPVOID                m_pNativeData;
                } nd;

                struct {                  // if CL_BACKPROPAGATESAFEHANDLE / CL_BACKPROPAGATECRITICALHANDLE
                    Object* m_oref;
                    LPVOID *m_pHandle;
                    LPVOID  m_hOldHandle;
                } bpsh;

                struct {
                    LPVOID  m_pnative;
                    MethodTable *m_pMT;
                } nlayout;

                struct {
                    class LoaderHeap *m_pHeap;
                    void             *m_pMem;
                    size_t            m_dwRequestedSize;
#ifdef _DEBUG
                    char             *m_szFile;
                    int               m_line;
#endif
                } backout;
            };

        };


        //-------------------------------------------------------------------
        // Inserts a new task of the given type and datum.
        // Returns non NULL on success.
        //-------------------------------------------------------------------
        CleanupNode* Schedule(CleanupType ct, LPVOID pv);

    public:
        class MarshalerCleanupNode : private CleanupNode
        {
            // DO NOT ADD ANY FIELDS!
            public:
                void CancelCleanup()
                {
                    LEAF_CONTRACT;
                    m_type = CL_MARSHALERREINIT_EXCEP;
                }

        };

    private:
        // NOTE: If you change the layout of this structure, you will
        // have to change some stubs which build and manipulate
        // CleanupWorkLists.
        DPTR(CleanupNode) m_pNodes;   //Pointer to first task.

    public:
        bool IsTrivial()
        {
            // zero or one entries
            return m_pNodes == NULL;
        }
};

typedef DPTR(CleanupWorkList) PTR_CleanupWorkList;

class NDirectMethodFrameEx;

// A helper used from our CLRToCOM compiled stub to add a Release to the cleanuplist.
VOID __stdcall ScheduleUnconditionalReleaseStatic(NDirectMethodFrameEx *pFrame, IUnknown *ip);

// A helper used from our CLRToCOM compiled stub to add a RCWStack pop to the cleanuplist.
VOID __stdcall ScheduleRCWStackPopStatic(NDirectMethodFrameEx* pFrame, Thread* pThread);

// Note: the value (-1) is used to generate the largest
// possible pointer value: this keeps frame addresses
// increasing upward.
#define FRAME_TOP ((Frame*)(-1))

#ifndef DACCESS_COMPILE

#define RETURNFRAMEVPTR(classname) \
    classname boilerplate;      \
    return *((TADDR*)&boilerplate)

#define DEFINE_VTABLE_GETTER(klass)             \
    public:                                     \
        static TADDR GetFrameVtable() {         \
            LEAF_CONTRACT;                      \
            klass boilerplate(false);           \
            return *((TADDR*)&boilerplate);     \
        }                                       \
        klass(bool dummy) { LEAF_CONTRACT; }

#define DEFINE_VTABLE_GETTER_AND_CTOR(klass)    \
        DEFINE_VTABLE_GETTER(klass)             \
    protected:                                  \
        klass() { LEAF_CONTRACT; }

#else

#define RETURNFRAMEVPTR(classname) \
    return classname::VPtrTargetVTable()
#define DEFINE_VTABLE_GETTER(klass)
#define DEFINE_VTABLE_GETTER_AND_CTOR(klass)

#endif // #ifndef DACCESS_COMPILE

#if _DEBUG
extern "C" void __stdcall CheckExitFrameDebuggerCalls();
#endif


//-----------------------------------------------------------------------------
// For reporting on types of frames at runtime.
class FrameTypeName 
{ 
public:
    TADDR vtbl; 
    PTR_CSTR name; 
};
typedef DPTR(FrameTypeName) PTR_FrameTypeName;

//-----------------------------------------------------------------------------
// For declaring structs with a preceeding GSCookie

template <typename T>
struct GSCookieFor
{
    GSCookie    m_gsCookie;
    T           m_val;
};

//-----------------------------------------------------------------------------
// Frame depends on the location of its vtable within the object. This
// superclass ensures that the vtable for Frame objects is in the same
// location under both MSVC and GCC.
//-----------------------------------------------------------------------------

class FrameBase
{
    VPTR_BASE_VTABLE_CLASS(FrameBase)

public:
    FrameBase() {LEAF_CONTRACT; }

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc) {
        LEAF_CONTRACT;
        // Nothing to protect
    }
#else
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags) = 0;
#endif
};

//------------------------------------------------------------------------
// Frame defines methods common to all frame types. There are no actual
// instances of root frames.
//------------------------------------------------------------------------

class Frame : public FrameBase
{
    friend class Binder;
    friend class CheckAsmOffsets;
#ifdef DACCESS_COMPILE
    friend void Thread::EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    VPTR_ABSTRACT_VTABLE_CLASS(Frame, FrameBase)

public:

    //------------------------------------------------------------------------
    // Special characteristics of a frame
    //------------------------------------------------------------------------
    enum FrameAttribs {
        FRAME_ATTR_NONE = 0,
        FRAME_ATTR_EXCEPTION = 1,           // This frame caused an exception
        FRAME_ATTR_OUT_OF_LINE = 2,         // The exception out of line (IP of the frame is not correct)
        FRAME_ATTR_FAULTED = 4,             // Exception caused by Win32 fault
        FRAME_ATTR_RESUMABLE = 8,           // We may resume from this frame
        FRAME_ATTR_RETURNOBJ = 0x10,        // Frame returns an object (helperFrame only)
        FRAME_ATTR_RETURN_INTERIOR = 0x20,  // Frame returns an interior GC poitner (helperFrame only)
        FRAME_ATTR_CAPTURE_DEPTH_2 = 0x40,  // This is a helperMethodFrame and the capture occured at depth 2
        FRAME_ATTR_EXACT_DEPTH = 0x80,      // This is a helperMethodFrame and a jit helper, but only crawl to the given depth
        FRAME_ATTR_NO_MD = 0x100,           // for helperMethodFrame that is doing a lazy init of a stub dispatch entrypoint
        FRAME_ATTR_POSSIBLE_MD = 0x200,     // for SetObjAppDomain called from stub which may be called from managed or unmanaged
    };
    virtual unsigned GetFrameAttribs()
    {
        LEAF_CONTRACT;
        return FRAME_ATTR_NONE;
    }

    //------------------------------------------------------------------------
    // Performs cleanup on an exception unwind
    //------------------------------------------------------------------------
#ifndef DACCESS_COMPILE
    virtual void ExceptionUnwind()
    {
        // Nothing to do here.
        LEAF_CONTRACT;
    }
#endif


    //------------------------------------------------------------------------
    // Is this a frame used on transition to native code from jitted code?
    //------------------------------------------------------------------------
    virtual BOOL IsTransitionToNativeFrame()
    {
        LEAF_CONTRACT;
        return FALSE;
    }

    virtual MethodDesc *GetFunction()
    {
        LEAF_CONTRACT;
        return NULL;
    }

    virtual Assembly *GetAssembly()
    {
        WRAPPER_CONTRACT;
        MethodDesc *pMethod = GetFunction();
        if (pMethod != NULL)
            return pMethod->GetModule()->GetAssembly();
        else
            return NULL;
    }

    virtual MetaSig::RETURNTYPE ReturnsObject()
    {
        WRAPPER_CONTRACT;
        MethodDesc* pMD = GetFunction();
        if (pMD == 0)
            return(MetaSig::RETNONOBJ);
        return(pMD->ReturnsObject());
    }

    // indicate the current X86 IP address within the current method
    // return 0 if the information is not available
    virtual const BYTE* GetIP()
    {
        LEAF_CONTRACT;
        return NULL;
    }

    // DACCESS: GetReturnAddressPtr should return the
    // target address of the return address in the frame.
    virtual TADDR GetReturnAddressPtr()
    {
        LEAF_CONTRACT;
        return NULL;
    }

    virtual TADDR GetReturnAddress()
    {
        WRAPPER_CONTRACT;
        TADDR ptr = GetReturnAddressPtr();
        return (ptr != NULL) ? *PTR_TADDR(ptr) : NULL;
    }

    virtual PTR_Context* GetReturnContextAddr()
    {
        LEAF_CONTRACT;
        return NULL;
    }

    Context *GetReturnContext()
    {
        WRAPPER_CONTRACT;
        PTR_Context* ppReturnContext = GetReturnContextAddr();
        if (! ppReturnContext)
            return NULL;
        return *ppReturnContext;
    }

    AppDomain *GetReturnDomain()
    {
        WRAPPER_CONTRACT;
        if (! GetReturnContext())
            return NULL;
        return GetReturnContext()->GetDomain();
    }

#ifndef DACCESS_COMPILE
    virtual Object **GetReturnExecutionContextAddr()
    {
        LEAF_CONTRACT;
        return NULL;
    }

    void SetReturnAddress(TADDR val)
    {
        WRAPPER_CONTRACT;
        TADDR ptr = GetReturnAddressPtr();
        _ASSERTE(ptr != NULL);
        *(TADDR*)ptr = val;
    }

#ifndef DACCESS_COMPILE
    void SetReturnContext(Context *pReturnContext)
    {
        WRAPPER_CONTRACT;
        PTR_Context* ppReturnContext = GetReturnContextAddr();
        _ASSERTE(ppReturnContext);
        *ppReturnContext = pReturnContext;
    }
#endif

    void SetReturnExecutionContext(OBJECTREF ref)
    {
        WRAPPER_CONTRACT;
        Object **pRef = GetReturnExecutionContextAddr();
        if (pRef != NULL)
            *pRef = OBJECTREFToObject(ref);
    }

    OBJECTREF GetReturnExecutionContext()
    {
        WRAPPER_CONTRACT;
        Object **pRef = GetReturnExecutionContextAddr();
        if (pRef == NULL)
            return NULL;
        else
            return ObjectToOBJECTREF(*pRef);
    }
#endif // #ifndef DACCESS_COMPILE

    PTR_GSCookie GetGSCookiePtr()
    {
        WRAPPER_CONTRACT;
        return PTR_GSCookie(PTR_HOST_TO_TADDR(this) + GetOffsOfGSCookie());
    }

    virtual INT32 GetOffsOfGSCookie()
    {
        LEAF_CONTRACT;
        ptrdiff_t offs = (0 - sizeof(GSCookie));
        _ASSERTE(FitsInI4(offs));
        return (INT32)offs;
    }

    static PTR_GSCookie SafeGetGSCookiePtr(Frame * pFrame);
    static void Init();

    virtual void UpdateRegDisplay(const PREGDISPLAY)
    {
        LEAF_CONTRACT;
        return;
    }

    //------------------------------------------------------------------------
    // Debugger support
    //------------------------------------------------------------------------


    enum ETransitionType
    {
        TT_NONE,
        TT_M2U, // we can safely cast to a FramedMethodFrame
        TT_U2M, // we can safely cast to a UnmanagedToManagedCallFrame
        TT_AppDomain, // transitioniting between AppDomains.
        TT_InternalCall, // calling into the CLR (ecall/fcall).
    };

    // Get the type of transition.
    // M-->U, U-->M
    virtual ETransitionType GetTransitionType()
    {
        return TT_NONE;
    }

    enum
    {
        TYPE_INTERNAL,
        TYPE_ENTRY,
        TYPE_EXIT,
        TYPE_CONTEXT_CROSS,
        TYPE_INTERCEPTION,
        TYPE_SECURITY,
        TYPE_CALL,
        TYPE_FUNC_EVAL,
        TYPE_TP_METHOD_FRAME,
        TYPE_MULTICAST,

        TYPE_COUNT
    };

    virtual int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_INTERNAL;
    };

    // When stepping into a method, various other methods may be called.
    // These are refererred to as interceptors. They are all invoked
    // with frames of various types. GetInterception() indicates whether
    // the frame was set up for execution of such interceptors

    enum Interception
    {
        INTERCEPTION_NONE,
        INTERCEPTION_CLASS_INIT,
        INTERCEPTION_EXCEPTION,
        INTERCEPTION_CONTEXT,
        INTERCEPTION_SECURITY,
        INTERCEPTION_OTHER,

        INTERCEPTION_COUNT
    };

    virtual Interception GetInterception()
    {
        LEAF_CONTRACT;
        return INTERCEPTION_NONE;
    }

    // Return information about an unmanaged call the frame
    // will make.
    // ip - the unmanaged routine which will be called
    // returnIP - the address in the stub which the unmanaged routine
    //            will return to.
    // returnSP - the location returnIP is pushed onto the stack
    //            during the call.
    //
    virtual void GetUnmanagedCallSite(TADDR* ip,
                                      TADDR* returnIP,
                                      TADDR* returnSP)
    {
        LEAF_CONTRACT;
        if (ip)
            *ip = NULL;

        if (returnIP)
            *returnIP = NULL;

        if (returnSP)
            *returnSP = NULL;
    }

    // Return where the frame will execute next - the result is filled
    // into the given "trace" structure.  The frame is responsible for
    // detecting where it is in its execution lifetime.
    virtual BOOL TraceFrame(Thread *thread, BOOL fromPatch,
                            TraceDestination *trace, REGDISPLAY *regs)
    {
        LEAF_CONTRACT;
        LOG((LF_CORDB, LL_INFO10000,
             "Default TraceFrame always returns false.\n"));
        return FALSE;
    }

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
    {
        WRAPPER_CONTRACT;
        DAC_ENUM_VTHIS();

        // Many frames store a MethodDesc pointer in m_Datum
        // so pick that up automatically.
        MethodDesc* func = GetFunction();
        if (func)
        {
            func->EnumMemoryRegions(flags);
        }

        // Include the NegSpace
        GSCookie * pGSCookie = GetGSCookiePtr();
        ULONG32 negSpaceSize = PBYTE(pGSCookie) - PBYTE(this);
        DacEnumMemoryRegion(PTR_HOST_TO_TADDR(this) - negSpaceSize, negSpaceSize);
    }
#endif

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static BYTE GetOffsetOfNextLink()
    {
        WRAPPER_CONTRACT;
        size_t ofs = offsetof(class Frame, m_Next);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    // get your VTablePointer (can be used to check what type the frame is)
    TADDR GetVTablePtr()
    {
        LEAF_CONTRACT;
        return VPTR_HOST_VTABLE_TO_TADDR(*(LPVOID*)this);
    }

#ifdef _DEBUG
    virtual BOOL Protects(OBJECTREF *ppObjectRef)
    {
        LEAF_CONTRACT;
        return FALSE;
    }
#endif

#ifndef DACCESS_COMPILE
    // Link and Unlink this frame
    VOID Push();
    VOID Pop();
    VOID Push(Thread *pThread);
    VOID Pop(Thread *pThread);
#endif // DACCESS_COMPILE

#ifdef _DEBUG_IMPL
    virtual void Log();
    static BOOL ShouldLogTransitions() { WRAPPER_CONTRACT; return LoggingOn(LF_STUBS, LL_INFO1000000); }
    static void __stdcall LogTransition(Frame* frame);
    virtual void LogFrame(int LF, int LL);       // General purpose logging.
    virtual void LogFrameChain(int LF, int LL);  // Log the whole chain.
    virtual char* GetFrameTypeName() {return NULL;}
#endif
    static PTR_CSTR GetFrameTypeName(TADDR vtbl);
    SPTR_DECL(FrameTypeName, s_pFrameTypeNames);

    //------------------------------------------------------------------------
    // Returns the address of a security object or
    // null if there is no space for an object on this frame.
    //------------------------------------------------------------------------
    virtual OBJECTREF *GetAddrOfSecurityDesc()
    {
        LEAF_CONTRACT;
        return NULL;
    }

private:
    // Pointer to the next frame up the stack.

protected:
    PTR_Frame m_Next;        // offset +4

public:
    PTR_Frame PtrNextFrame() { return m_Next; }

private:
    // Because JIT-method activations cannot be expressed as Frames,
    // everyone must use the StackCrawler to walk the frame chain
    // reliably. We'll expose the Next method only to the StackCrawler
    // to prevent mistakes.
    /*                                                                   
    */
    //        friend Frame* Thread::StackWalkFrames(PSTACKWALKFRAMESCALLBACK pCallback, VOID *pData);
    friend class Thread;
    friend void CrawlFrame::GotoNextFrame();
    friend class StackFrameIterator;
    friend class TailCallFrame;
    friend class AppDomain;
    friend VOID RealCOMPlusThrow(OBJECTREF);
    friend FCDECL0(VOID, JIT_StressGC);

    Frame   *Next()
    {
        LEAF_CONTRACT;
        return m_Next;
    }

protected:
#ifndef DACCESS_COMPILE
    // Frame is considered an abstract class: this protected constructor
    // causes any attempt to instantiate one to fail at compile-time.
    Frame() { LEAF_CONTRACT; }
#endif
};


//-----------------------------------------------------------------------------
// This frame provides context for a frame that
// took an exception that is going to be resumed.
//
// It is necessary to create this frame if garbage
// collection may happen during handling of the
// exception.  The FRAME_ATTR_RESUMABLE flag tells
// the GC that the preceding frame needs to be treated
// like the top of stack (with the important implication that
// caller-save-regsiters will be potential roots).
//-----------------------------------------------------------------------------
//------------------------------------------------------------------------

inline BOOL ISREDIRECTEDTHREAD(Thread * thread) { LEAF_CONTRACT; return FALSE; }
inline CONTEXT * GETREDIRECTEDCONTEXT(Thread * thread) { LEAF_CONTRACT; return (CONTEXT*) NULL; }

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// This frame represents a transition from one or more nested frameless
// method calls to either a EE runtime helper function or a framed method.
// Because most stackwalks from the EE start with a full-fledged frame,
// anything but the most trivial call into the EE has to push this
// frame in order to prevent the frameless methods inbetween from
// getting lost.
//------------------------------------------------------------------------

class TransitionFrame : public Frame
{
    VPTR_ABSTRACT_VTABLE_CLASS(TransitionFrame, Frame)

protected:


    TADDR           m_Datum;    // contents depend on subclass type

#if defined(_PPC_)
    DWORD           m_pad;
    // linkage area
    INT32           m_SavedSP; // stack pointer
    INT32           m_SavedCR; // flags
    TADDR           m_ReturnAddress; // SavedLR - return address into JIT'ted code
    INT32           m_Reserved1;
    INT32           m_Reserved2;
    INT32           m_Reserved3;
#else
    TADDR           m_ReturnAddress;  // return address into JIT'ted code
#endif

public:
    // DACCESS: GetReturnAddressPtr should return the
    // target address of the return address in the frame.
    virtual TADDR GetReturnAddressPtr()
    {
        LEAF_CONTRACT;
        return PTR_HOST_MEMBER_TADDR(TransitionFrame, this, m_ReturnAddress);
    }

    static BYTE GetOffsetOfReturnAddress()
    {
        WRAPPER_CONTRACT;
        size_t ofs = offsetof(class TransitionFrame, m_ReturnAddress);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    static int GetOffsetOfDatum()
    {
        LEAF_CONTRACT;
        return offsetof(class TransitionFrame, m_Datum);
    }






    virtual void UpdateRegDisplay(const PREGDISPLAY) = 0;

    friend class CheckAsmOffsets;
};


//-----------------------------------------------------------------------
// TransitionFrames for exceptions
//-----------------------------------------------------------------------

// The define USE_FEF controls how this class is used.  Look for occurances
//  of USE_FEF.

class FaultingExceptionFrame : public TransitionFrame
{
    friend class CheckAsmOffsets;

#if defined(_X86_)
    DWORD                   m_Esp;
#endif

#if defined(_X86_) || defined(_PPC_)
    CalleeSavedRegisters    m_regs;
#endif


    VPTR_VTABLE_CLASS(FaultingExceptionFrame, TransitionFrame)

public:
#ifndef DACCESS_COMPILE
    FaultingExceptionFrame() {
        LEAF_CONTRACT;
        m_Next = NULL;
#ifdef _DEBUG
        // This is unused
        m_Datum = 0xBAADF00D;
#endif
    }
#endif

    void Init(CONTEXT *pContext);
    void InitAndLink(CONTEXT *pContext);

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(FaultingExceptionFrame);
    }

    Interception GetInterception()
    {
        LEAF_CONTRACT;
        return INTERCEPTION_EXCEPTION;
    }

    unsigned GetFrameAttribs()
    {
        LEAF_CONTRACT;
        return FRAME_ATTR_EXCEPTION | FRAME_ATTR_FAULTED;
    }

#if defined(_X86_) || defined(_PPC_)
    CalleeSavedRegisters *GetCalleeSavedRegisters()
    {
        LEAF_CONTRACT;
        return &m_regs;
    }
#endif

    virtual void UpdateRegDisplay(const PREGDISPLAY);


    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(FaultingExceptionFrame)
};


//-----------------------------------------------------------------------
// TransitionFrame for debugger function evaluation
//
// m_Datum holds a ptr to a DebuggerEval object which contains a copy
// of the thread's context at the time it was hijacked for the func
// eval.
//
// UpdateRegDisplay updates all registers inthe REGDISPLAY, not just
// the callee saved registers, because we can hijack for a func eval
// at any point in a thread's execution.
//
// No callee saved registers are held in the negative space for this
// type of frame.
//
//-----------------------------------------------------------------------

class DebuggerEval;

class FuncEvalFrame : public TransitionFrame
{
    VPTR_VTABLE_CLASS(FuncEvalFrame, TransitionFrame)


    BOOL            m_showFrame;

public:
#ifndef DACCESS_COMPILE
    FuncEvalFrame(void *pDebuggerEval, LPVOID returnAddress, BOOL showFrame)
    {
        LEAF_CONTRACT;
        m_Datum = (TADDR)pDebuggerEval;
        m_ReturnAddress = (TADDR)returnAddress;
        m_showFrame = showFrame;
    }
#endif

    virtual BOOL IsTransitionToNativeFrame()
    {
        LEAF_CONTRACT;
        return FALSE;
    }

    virtual int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_FUNC_EVAL;
    }

    virtual unsigned GetFrameAttribs() DAC_EMPTY_RET(0);

    virtual void UpdateRegDisplay(const PREGDISPLAY) DAC_EMPTY_ERR();

    virtual DebuggerEval *GetDebuggerEval()
    {
        LEAF_CONTRACT;
        return (DebuggerEval*) (void*)m_Datum;
    }

    virtual TADDR GetReturnAddressPtr() DAC_EMPTY_RET(0);

    /*
     * ShowFrame
     *
     * Returns if this frame should be returned as part of a stack trace to a debugger or not.
     *
     */
    BOOL ShowFrame()
    {
        LEAF_CONTRACT;

        return m_showFrame;
    }

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(FuncEvalFrame)
};


//-----------------------------------------------------------------------
// Provides access to the caller's arguments from a FramedMethodFrame.
// Does *not* include the "this" pointer.
//-----------------------------------------------------------------------
class ArgIterator
{
public:
    //------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------
    ArgIterator(FramedMethodFrame *pFrame, MetaSig* pSig);

    //------------------------------------------------------------
    // Another constructor when you dont have active frame FramedMethodFrame
    //------------------------------------------------------------
    ArgIterator(LPBYTE pFrameBase, MetaSig* pSig, BOOL fIsStatic);

    //------------------------------------------------------------
    // An even more primitive constructor when dont have have a
    // a FramedMethodFrame
    //------------------------------------------------------------
    ArgIterator(LPBYTE pFrameBase, MetaSig* pSig, int stackArgsOfs, int regArgsOfs);


    //------------------------------------------------------------
    // Each time this is called, this returns a pointer to the next
    // argument. This pointer points directly into the caller's stack.
    // Whether or not object arguments returned this way are gc-protected
    // depends on the exact type of frame.
    //
    // Returns NULL once you've hit the end of the list.
    //------------------------------------------------------------
    LPVOID GetNextArgAddr()
    {
        WRAPPER_CONTRACT;
        BYTE   typeDummy;
        UINT32 structSizeDummy;
        return GetNextArgAddr(&typeDummy, &structSizeDummy);
    }

    static int GetThisOffset(MetaSig* pSig);
    static int GetRetBuffArgOffset(MetaSig* pSig, UINT *pRegStructOfs = NULL);
    static int GetVASigCookieOffset(MetaSig* pSig);

    int GetThisOffset() {
        WRAPPER_CONTRACT;
        return GetThisOffset(m_pSig);
    }
    int GetRetBuffArgOffset(UINT *pRegStructOfs = NULL) {
        WRAPPER_CONTRACT;
        return GetRetBuffArgOffset(m_pSig, pRegStructOfs);
    }

    LPVOID* GetThisAddr()   {
        WRAPPER_CONTRACT;
        return((LPVOID*) (m_pFrameBase + GetThisOffset(m_pSig)));
    }
    LPVOID* GetRetBuffArgAddr() {
        WRAPPER_CONTRACT;
        return((LPVOID*) (m_pFrameBase + GetRetBuffArgOffset(m_pSig)));
    }

    //------------------------------------------------------------
    // Like GetNextArgAddr but returns information about the
    // param type (IMAGE_CEE_CS_*) and the structure size if apropos.
    //------------------------------------------------------------
    LPVOID GetNextArgAddr(BYTE *pType, UINT32 *pStructSize);

    //------------------------------------------------------------
    // Same as GetNextArgAddr() but returns a byte offset from
    // the Frame* pointer. This offset can be positive *or* negative.
    //
    // Returns 0 once you've hit the end of the list. Since the
    // the offset is relative to the Frame* pointer itself, 0 can
    // never point to a valid argument.
    //------------------------------------------------------------
    int    GetNextOffset()
    {
        WRAPPER_CONTRACT;
        BYTE   typeDummy;
        UINT32 structSizeDummy;
        return GetNextOffset(&typeDummy, &structSizeDummy);
    }

    //------------------------------------------------------------
    // Like GetNextArgOffset but returns information about the
    // param type (IMAGE_CEE_CS_*) and the structure size if apropos.
    // The optional pRegStructOfs param points to a buffer which receives
    // either the appropriate offset into the ArgumentRegisters struct or
    // -1 if the argument is on the stack.
    //------------------------------------------------------------
    int    GetNextOffset(BYTE *pType, UINT32 *pStructSize, UINT *pRegStructOfs = NULL);

    int    GetNextOffsetFaster(BYTE *pType, UINT32 *pStructSize, UINT *pRegStructOfs = NULL);

    //------------------------------------------------------------
    // Must be called after all the args.  returns the offset of the
    // argument passed to methods implementing parameterized types.
    // If it is in a register pRegStructOfs is set, otherwise it is -1
    // In either case it returns an offset in the frame of the arg (assuming
    // it is framed).
    //------------------------------------------------------------
    int     GetParamTypeArgOffset(INT *pRegStructOfs);

    TypeHandle GetArgType();

    //------------------------------------------------------------------
    // Returns true if m_pSig is currently in the varargs area
    //     amd false is we are in the fixed args area
    //------------------------------------------------------------------
    bool atSentinel();

    //
    // The following is used by the profiler to dig into the iterator for
    // discovering if the method has a This pointer or a return buffer.
    // Do not use this to re-initialize the signature, use the exposed Init()
    // method in this class.
    //
    MetaSig *GetSig(void)
    {
        return m_pSig;
    }

private:
    MetaSig*            m_pSig;
    int                 m_curOfs;
    LPBYTE              m_pFrameBase;
    int                 m_numRegistersUsed;
    int                 m_regArgsOfs;       // add this to pFrameBase to find the the pointer
                                            // to where the last register based argument has
                                            // been saved in the frame (again stack grows down
                                            // first arg pushed first).  0 is an illegal value
                                            // than means the register args are not saved on the
                                            // stack.
    int                 m_argNum;

    // shared init
    void Init(LPBYTE pFrameBase, MetaSig* pSig, BOOL fIsStatic, int stackArgsOfs, int regArgsOfs);
};

//------------------------------------------------------------------------
// A HelperMethodFrame is created by jit helper (Modified slightly it could be used
// for native routines).   This frame just does the callee saved register
// fixup, it does NOT protect arguments (you can use GCPROTECT or the HelperMethodFrame subclases)
// see JitInterface for sample use, YOU CAN'T RETURN STATEMENT WHILE IN THE PROTECTED STATE!
//------------------------------------------------------------------------

class HelperMethodFrame : public Frame
{
    VPTR_VTABLE_CLASS(HelperMethodFrame, Frame);

    union {
        TADDR m_Datum;
        WORD m_wRefData;
    };

public:
#ifndef DACCESS_COMPILE
    // Lazy initialization of HelperMethodFrame.  Need to
    // call InsureInit to complete initialization
    // If this is an FCall, the second param is the entry point for the FCALL.
    // The MethodDesc will be looked up form this (lazily), and this method
    // will be used in stack reporting, if this is not an FCall pass a 0
    HelperMethodFrame(void* fCallFtnEntry, struct LazyMachState* ms, unsigned attribs = 0)
    {
        WRAPPER_CONTRACT;
         INDEBUG(memset(&m_Attribs, 0xCC, sizeof(HelperMethodFrame) - offsetof(HelperMethodFrame, m_Attribs));)
         m_Attribs = attribs;
         LazyInit(fCallFtnEntry, ms);
    }

    // If you give the optional MethodDesc parameter, then the frame
    // will act like like the given method for stack tracking purposes.
    // If you also give regArgs != 0, then the helper frame will
    // will also promote the arguments for you (Pretty neat, huh?)
    HelperMethodFrame(struct MachState* ms, MethodDesc* pMD, ArgumentRegisters* regArgs);

    // This is for virtual call dispatch, in a case where we only know the size of the
    // stack arguments. This works because we are guaranteed that the only time we will
    // try to run managed code is if we're about to throw an exception, in which case
    // the enregistered and stack arguments are going to be lost as soon as the frame is
    // unwound.
    enum tagWithRefData { WithRefData };
    HelperMethodFrame(tagWithRefData dummytag, struct MachState* ms,
                      WORD wRefData, ArgumentRegisters* regArgs);
#endif // DACCESS_COMPILE

    // Returns true if this frame has a bitmap for GC refs
    BOOL IsRefDataFrame() { LEAF_CONTRACT; return (m_Attribs & FRAME_ATTR_NO_MD); }

    virtual TADDR GetReturnAddress()
    {
        LEAF_CONTRACT;

        if (!m_MachState->isValid())
        {
#if defined(DACCESS_COMPILE)
            MachState unwoundState;
            InsureInit(false, &unwoundState);
            return unwoundState.GetRetAddr();

#else  // !DACCESS_COMPILE
            _ASSERTE(!"HMF's should always be initialized in the non-DAC world.");
            return NULL;

#endif // !DACCESS_COMPILE
        }
        else
        {
            return *PTR_TADDR(PTR_TO_TADDR(m_MachState->_pRetAddr));
        }
    }

    // DACCESS: GetReturnAddressPtr should return the
    // target address of the return address in the frame.
    TADDR GetReturnAddressPtr()
    {
        LEAF_CONTRACT;
#if !defined(DACCESS_COMPILE)
        _ASSERTE(m_MachState->isValid());
        return PTR_TO_TADDR(m_MachState->_pRetAddr);

#else  // DACCESS_COMPILE
        IA64_ONLY(_ASSERTE(!"HMF::GetReturnAddressPtr() - cannot return the target address of the return address"));
        return InsureInit(false, NULL);

#endif // DACCESS_COMPILE
    }

    virtual MethodDesc* GetFunction();
    virtual MetaSig::RETURNTYPE ReturnsObject();


    virtual void UpdateRegDisplay(const PREGDISPLAY);

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc);
#endif
    virtual Interception GetInterception()
    {
        WRAPPER_CONTRACT;
        if (GetFrameAttribs() & FRAME_ATTR_EXCEPTION)
            return(INTERCEPTION_EXCEPTION);
        return(INTERCEPTION_NONE);
    }

    virtual ETransitionType GetTransitionType()
    {
        return TT_InternalCall;
    }

    virtual unsigned GetFrameAttribs()
    {
        LEAF_CONTRACT;
        return(m_Attribs);
    }
    void SetFrameAttribs(unsigned attribs)
    {
        LEAF_CONTRACT;
        m_Attribs = attribs;
    }
#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
    {
        WRAPPER_CONTRACT;
        Frame::EnumMemoryRegions(flags);
        m_MachState.EnumMem();
        m_RegArgs.EnumMem();
    }
#endif

#ifndef DACCESS_COMPILE
    void Pop()
    {
        WRAPPER_CONTRACT;
        Frame::Pop(m_pThread);
    }
    void Poll()
    {
        WRAPPER_CONTRACT;
        if (m_pThread->CatchAtSafePoint())
            CommonTripThread();
    }
#endif // DACCESS_COMPILE
    TADDR InsureInit(bool initialInit, struct MachState* unwindState);
    void Init(Thread *pThread, struct MachState* ms, MethodDesc* pMD, ArgumentRegisters * regArgs);
#ifndef DACCESS_COMPILE
    inline void Init(struct LazyMachState* ms)
    {
        WRAPPER_CONTRACT;
        LazyInit(0, ms);
    }
#endif // DACCESS_COMPILE
    MachState * MachineState() {
        LEAF_CONTRACT;
        return m_MachState;
    }

    INDEBUG(static TADDR GetMethodFrameVPtr() {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(HelperMethodFrame);
    })
protected:

#ifndef DACCESS_COMPILE
    HelperMethodFrame::HelperMethodFrame()
    {
        WRAPPER_CONTRACT;
        INDEBUG(memset(&m_Attribs, 0xCC, sizeof(HelperMethodFrame) - offsetof(HelperMethodFrame, m_Attribs));)
    }
    void LazyInit(void* FcallFtnEntry, struct LazyMachState* ms);
#endif // DACCESS_COMPILE

protected:
    unsigned m_Attribs;
    PTR_MachState m_MachState;       // pRetAddr points to the return address and the stack arguments
    PTR_ArgumentRegisters m_RegArgs; // if non-zero we also report these as the register arguments
    PTR_Thread m_pThread;
    TADDR m_FCallEntry;              // used to determine our identity for stack traces

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(HelperMethodFrame)
};

// Restores registers saved in m_MachState
EXTERN_C int __fastcall HelperMethodFrameRestoreState(
        INDEBUG_COMMA(HelperMethodFrame *pFrame)
        MachState *pState
    );


#if defined(_DEBUG) && !defined(DACCESS_COMPILE)
// Returns TRUE if the given address can validly call a method that pushes a
// HelperMethodFrame.  HelperMethodFrame::UpdateRegDisplay, LazyMachState, etc. do some
BOOL IsValidHelperMethodCaller (LPCVOID pvReturnAddress);
#endif


/* report all the args (but not THIS if present), to the GC. 'framePtr' points at the
   frame (promote doesn't assume anthing about its structure).  'msig' describes the
   arguments, and 'ctx' has the GC reporting info.  'stackArgsOffs' is the byte offset
   from 'framePtr' where the arguments start (args start at last and grow bacwards).
   Simmilarly, 'regArgsOffs' is the offset to find the register args to promote */
void promoteArgs(BYTE* framePtr, MetaSig* msig, GCCONTEXT* ctx,
                 int stackArgsOffs, int regArgsOffs, bool skipFixedArgs);

// workhorse for our promotion efforts
inline void DoPromote(promote_func *fn, ScanContext* sc, OBJECTREF *address, BOOL interior)
{
    WRAPPER_CONTRACT;

    // We use OBJECTREF_TO_UNCHECKED_OBJECTREF since address may be an interior pointer
    LOG((LF_GC, INFO3,
         "    Promoting pointer argument at" FMT_ADDR "from" FMT_ADDR "to ",
         DBG_ADDR(address), DBG_ADDR(OBJECTREF_TO_UNCHECKED_OBJECTREF(*address)) ));

    if (interior)
        PromoteCarefully(fn, *((Object**)address), sc);
    else
        (*fn) (*((Object **)address), sc, 0);

    LOG((LF_GC, INFO3, "    " FMT_ADDR "\n", DBG_ADDR(OBJECTREF_TO_UNCHECKED_OBJECTREF(*address)) ));
}


//-----------------------------------------------------------------------------
// a HelplerMethodFrames that also report additional object references
//-----------------------------------------------------------------------------

class HelperMethodFrame_1OBJ : public HelperMethodFrame
{
    VPTR_VTABLE_CLASS(HelperMethodFrame_1OBJ, HelperMethodFrame)

public:
#ifndef DACCESS_COMPILE
    HelperMethodFrame_1OBJ(void* fCallFtnEntry, struct LazyMachState* ms, unsigned attribs, OBJECTREF* aGCPtr1)
        : HelperMethodFrame(fCallFtnEntry, ms, attribs)
        {
            LEAF_CONTRACT;            
            gcPtrs[0] = aGCPtr1;
            INDEBUG(Thread::ObjectRefProtected(aGCPtr1);)
            INDEBUG((*aGCPtr1)->Validate ();)
        }
#endif

    void SetProtectedObject(OBJECTREF* objPtr)
    {
        LEAF_CONTRACT;
        gcPtrs[0] = objPtr;
        INDEBUG(Thread::ObjectRefProtected(objPtr);)
        }

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        DoPromote(fn, sc, gcPtrs[0], FALSE);
        HelperMethodFrame::GcScanRoots(fn, sc);
    }
#endif

#ifdef _DEBUG
#ifndef DACCESS_COMPILE
    void Pop()
    {
        WRAPPER_CONTRACT;
        HelperMethodFrame::Pop();
        Thread::ObjectRefNew(gcPtrs[0]);
    }
#endif // DACCESS_COMPILE

    BOOL Protects(OBJECTREF *ppORef)
    {
        LEAF_CONTRACT;
        return (ppORef == gcPtrs[0]) ? TRUE : FALSE;
    }

#endif

private:
    OBJECTREF*  gcPtrs[1];

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(HelperMethodFrame_1OBJ)
};


//-----------------------------------------------------------------------------
// HelperMethodFrame_2OBJ
//-----------------------------------------------------------------------------

class HelperMethodFrame_2OBJ : public HelperMethodFrame
{
    VPTR_VTABLE_CLASS(HelperMethodFrame_2OBJ, HelperMethodFrame)

public:
#ifndef DACCESS_COMPILE
    HelperMethodFrame_2OBJ(void* fCallFtnEntry, struct LazyMachState* ms, unsigned attribs, OBJECTREF* aGCPtr1, OBJECTREF* aGCPtr2)
        : HelperMethodFrame(fCallFtnEntry, ms, attribs)
        {
            LEAF_CONTRACT;
        gcPtrs[0] = aGCPtr1;
        gcPtrs[1] = aGCPtr2;
        INDEBUG(Thread::ObjectRefProtected(aGCPtr1);)
        INDEBUG(Thread::ObjectRefProtected(aGCPtr2);)
        INDEBUG((*aGCPtr1)->Validate ();)
        INDEBUG((*aGCPtr2)->Validate ();)
        }
#endif

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        DoPromote(fn, sc, gcPtrs[0], FALSE);
        DoPromote(fn, sc, gcPtrs[1], FALSE);
        HelperMethodFrame::GcScanRoots(fn, sc);
    }
#endif

#ifdef _DEBUG
#ifndef DACCESS_COMPILE
    void Pop()
    {
        WRAPPER_CONTRACT;
        HelperMethodFrame::Pop();
        Thread::ObjectRefNew(gcPtrs[0]);
        Thread::ObjectRefNew(gcPtrs[1]);
    }
#endif // DACCESS_COMPILE

    BOOL Protects(OBJECTREF *ppORef)
    {
        LEAF_CONTRACT;
        return (ppORef == gcPtrs[0] || ppORef == gcPtrs[1]) ? TRUE : FALSE;
    }
#endif

private:
    OBJECTREF*  gcPtrs[2];

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(HelperMethodFrame_2OBJ)
};


//-----------------------------------------------------------------------------
// HelperMethodFrame_PROTECTOBJ
//-----------------------------------------------------------------------------

class HelperMethodFrame_PROTECTOBJ : public HelperMethodFrame
{
    VPTR_VTABLE_CLASS(HelperMethodFrame_PROTECTOBJ, HelperMethodFrame)

public:
#ifndef DACCESS_COMPILE
    HelperMethodFrame_PROTECTOBJ(void* fCallFtnEntry, struct LazyMachState* ms, unsigned attribs, OBJECTREF* pObjRefs, int numObjRefs)
        : HelperMethodFrame(fCallFtnEntry, ms, attribs)
    {
        LEAF_CONTRACT;
        m_pObjRefs = pObjRefs;
        m_numObjRefs = numObjRefs;
#ifdef _DEBUG
        for (UINT i = 0; i < m_numObjRefs; i++) {
            Thread::ObjectRefProtected(&m_pObjRefs[i]);
            m_pObjRefs[i]->Validate();
        }
#endif
    }
#endif

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        for (UINT i = 0; i < m_numObjRefs; i++) {
            DoPromote(fn, sc, &m_pObjRefs[i], FALSE);
        }
        HelperMethodFrame::GcScanRoots(fn, sc);
    }
#endif

#ifdef _DEBUG
#ifndef DACCESS_COMPILE
    void Pop()
    {
        WRAPPER_CONTRACT;
        HelperMethodFrame::Pop();
        for (UINT i = 0; i < m_numObjRefs; i++) {
            Thread::ObjectRefNew(&m_pObjRefs[i]);
        }
    }
#endif // DACCESS_COMPILE

    BOOL Protects(OBJECTREF *ppORef)
    {
        LEAF_CONTRACT;
        for (UINT i = 0; i < m_numObjRefs; i++) {
            if (ppORef == &m_pObjRefs[i])
                return TRUE;
        }
        return FALSE;
    }
#endif

private:
    OBJECTREF *m_pObjRefs;
    UINT       m_numObjRefs;

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(HelperMethodFrame_PROTECTOBJ)
};


//
//
//
//


#define DEFINE_NEG_SPACE_ACCESSORS                                          \
                                                                            \
    typedef DPTR(NegInfo) PTR_NegInfo;                                      \
                                                                            \
    /* The total size of the NegInfo, including any NegInfo declared        \
       by a parent Frame type, and also INCLIDING the GSCookie.             \
       Note that this never be used by a sub-type as the GSCookie           \
       is never "inherited" from the parent.                                \
     */                                                                     \
    static UINT32 GetNegSpaceSize()                                         \
    {                                                                       \
        LEAF_CONTRACT;                                                      \
        size_t sz = sizeof(GSCookieFor<NegInfo>);                           \
        _ASSERTE(FitsInU4(sz));                                             \
        return (UINT32)sz;                                                  \
    }                                                                       \
                                                                            \
    /* The size of NegInfo, including parent's raw NegInfo (if any),        \
       but EXCLUDING the GSCookie.                                          \
       This is the size that will be "inherited" by sub-types               \
     */                                                                     \
    static UINT32 GetRawNegSpaceSize()                                      \
    {                                                                       \
        LEAF_CONTRACT;                                                      \
        size_t sz = sizeof(NegInfo);                                        \
        _ASSERTE(FitsInU4(sz));                                             \
        return (UINT32)sz;                                                  \
    }                                                                       \
                                                                            \
    /* Simple accessor to do the pointer arithmetic to get to the NegInfo,  \
       so that the caller can access it in a type-safe way                  \
     */                                                                     \
    PTR_NegInfo GetNegInfo()                                                \
    {                                                                       \
        LEAF_CONTRACT;                                                      \
        return (PTR_NegInfo) (PTR_HOST_TO_TADDR(this) - sizeof(NegInfo));   \
    }                                                                       \
                                                                            \
    virtual INT32 GetOffsOfGSCookie()                                       \
    {                                                                       \
        WRAPPER_CONTRACT;                                                   \
        return GetOffsetOfGSCookie();                                       \
    }                                                                       \
                                                                            \
    static INT32 GetOffsetOfGSCookie()                                      \
    {                                                                       \
        LEAF_CONTRACT;                                                      \
        ptrdiff_t offs = (ptrdiff_t)(0 - sizeof(GSCookieFor<NegInfo>));     \
        _ASSERTE(FitsInI4(offs));                                           \
        return (INT32)offs;                                                 \
    }

// Accessors for data in the NegInfo

#define DEFINE_NEG_SPACE_ACCESSOR_FOR(fieldName, publicName)                \
    static INT32 GetOffsetOf##publicName()                                  \
    {                                                                       \
        LEAF_CONTRACT;                                                      \
        ptrdiff_t offs = (ptrdiff_t)(0 - sizeof(NegInfo)                    \
                                       + offsetof(NegInfo, fieldName));     \
        _ASSERTE(FitsInI4(offs));                                           \
        return (INT32)offs;                                                 \
    }

//------------------------------------------------------------------------
// FramedMethodFrame represents a method call. This is an abstract class
// and no actual instances of thisframe exist: there are subclasses for
// each method type.
//
// However, they all share a similar image ...
//
#if defined(_X86_)
//
// x86:
//              +...    stack-based arguments here
//              +12     return address
//              +8      datum (typically a MethodDesc*)
//              +4      m_Next
//              +0      the frame vptr
//              -...    preserved CalleeSavedRegisters
//              -...    VC5Frame
//              -...    ArgumentRegisters
//
#elif defined(_PPC_)
//
// PPC:
//              +...    stack-based arguments here
//              +...    this
//              +...    retbugarg
//      -- FramedMethodFrame end
//              +12     LinkageArea (contains return address at offset 8)
//              +8      datum (typically a MethodDesc*)
//              +4      m_Next
//              +0      the frame vptr
//      -- FramedMethodFrame start
//              -...    CalleeSavedRegisters
//              -...    ArgumentRegisters
//
#else
PORTABILITY_WARNING("FramedMethodFrame layout not documented")
#endif
//
//------------------------------------------------------------------------

class FramedMethodFrame : public TransitionFrame
{
    VPTR_VTABLE_CLASS(FramedMethodFrame, TransitionFrame)


public:

    struct NegInfo
    {
#if defined(_X86_) || defined(_PPC_)
        ArgumentRegisters       m_argumentRegisters;
        #ifdef VC5FRAME_SIZE
        VC5Frame                m_vc5Frame;
        #endif
        CalleeSavedRegisters    m_calleeSavedRegisters;

#endif
    };

    // FramedMethodFrame must store some fields at negative offset.  This
    // method exposes the size for places needing to allocate a
    // FramedMethodFrame.
    static UINT32 GetRawNegSpaceSize()
    {
        LEAF_CONTRACT;
    #if defined(_X86_) || defined(_PPC_) || defined(_IA64_)
        return sizeof(NegInfo);
    #else
        return 0;
    #endif
    }

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static int GetOffsetOfArgumentRegisters()
    {
        LEAF_CONTRACT;
        ptrdiff_t offs;
    #ifdef CALLDESCR_ARGREGS
        offs = (ptrdiff_t)(0 - int(sizeof(NegInfo) -
                                   offsetof(NegInfo, m_argumentRegisters)));
    #else
        offs = sizeof(FramedMethodFrame);
    #endif
        _ASSERTE(FitsInI4(offs));
        return (int)offs;
    }



    static BOOL IsArgumentRegisterOffset (int offset)
    {
        WRAPPER_CONTRACT;

        int ofsArgRegs = GetOffsetOfArgumentRegisters();

        return offset >= ofsArgRegs && offset < (int)(ofsArgRegs + NUM_ARGUMENT_REGISTERS * sizeof(void*));
    }

    static UINT GetArgumentIndexFromOffset (int offset)
    {
        WRAPPER_CONTRACT;
        _ASSERTE(IsArgumentRegisterOffset(offset));
        return (offset - GetOffsetOfArgumentRegisters()) / sizeof(void*);
    }

    static BOOL IsStackArgumentOffset (int offset)
    {
        WRAPPER_CONTRACT;

        int ofsArgRegs = GetOffsetOfArgumentRegisters();

        return offset >= (int)(ofsArgRegs + NUM_ARGUMENT_REGISTERS * sizeof(void*));
    }

    static int GetOffsetOfCalleeSavedRegisters()
    {
        LEAF_CONTRACT;
        return -int(sizeof(NegInfo) - offsetof(NegInfo, m_calleeSavedRegisters));
    }

    CalleeSavedRegisters *GetCalleeSavedRegisters()
    {
        WRAPPER_CONTRACT;
        return PTR_CalleeSavedRegisters( PTR_HOST_TO_TADDR(this) +
                                         GetOffsetOfCalleeSavedRegisters());
    }

    virtual MethodDesc *GetFunction()
    {
        WRAPPER_CONTRACT;
        return PTR_MethodDesc(m_Datum);
    }

    virtual void UpdateRegDisplay(const PREGDISPLAY);

    // Get return value address
    virtual INT64 *GetReturnValuePtr()
    {
        LEAF_CONTRACT;
        return NULL;
    }

    virtual ETransitionType GetTransitionType()
    {
        return TT_M2U; // we can safely cast to a FramedMethodFrame
    }

    IMDInternalImport *GetMDImport()
    {
        WRAPPER_CONTRACT;
        _ASSERTE(GetFunction());
        return GetFunction()->GetMDImport();
    }

    Module *GetModule()
    {
        WRAPPER_CONTRACT;
        _ASSERTE(GetFunction());
        return GetFunction()->GetModule();
    }

    //---------------------------------------------------------------
    // Get the "this" object.
    //---------------------------------------------------------------
    virtual OBJECTREF GetThis()
    {
        WRAPPER_CONTRACT;
        Object* obj = PTR_Object(*PTR_TADDR(GetAddrOfThis()));
        return ObjectToOBJECTREF(obj);
    }

    //---------------------------------------------------------------
    // Get the extra info for shared generic code.
    //---------------------------------------------------------------
    void *GetParamTypeArg();

protected:  // we don't want people using this directly
    //---------------------------------------------------------------
    // Get the address of the "this" object. WARNING!!! Whether or not "this"
    // is gc-protected is depends on the frame type!!!
    //---------------------------------------------------------------
    TADDR GetAddrOfThis();
public:
    
#ifdef _X86_
    //---------------------------------------------------------------
    // Get the offset of the stored "this" pointer relative to the frame.
    // This is a special _X86_ version that does not depends on the method signature
    //---------------------------------------------------------------
    static int GetOffsetOfThis() {
        WRAPPER_CONTRACT;
        return FramedMethodFrame::GetOffsetOfArgumentRegisters() + offsetof(ArgumentRegisters, THIS_REG);
    }
#endif // _X86_

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static BYTE GetOffsetOfMethod()
    {
        WRAPPER_CONTRACT;
        size_t ofs = offsetof(class FramedMethodFrame, m_Datum);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    static BYTE GetOffsetOfArgs()
    {
        LEAF_CONTRACT;
        size_t ofs = sizeof(FramedMethodFrame);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    //---------------------------------------------------------------
    // For vararg calls, return cookie.
    //---------------------------------------------------------------
    VASigCookie *GetVASigCookie()
    {
#if defined(_X86_)
        LEAF_CONTRACT;
        return PTR_VASigCookie(*PTR_TADDR(PTR_HOST_TO_TADDR(this) +
                                          sizeof(*this)));
#else
        WRAPPER_CONTRACT;
        MetaSig msig(GetFunction());
        return PTR_VASigCookie(*PTR_TADDR(PTR_HOST_TO_TADDR(this) +
                   ArgIterator::GetVASigCookieOffset(&msig)));
#endif
    }

    int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_CALL;
    }


#if defined (_DEBUG) && !defined (DACCESS_COMPILE)
    virtual BOOL Protects(OBJECTREF *ppORef);
#endif //defined (_DEBUG) && defined (DACCESS_COMPILE)

protected:

#ifndef DACCESS_COMPILE
    // For use by classes deriving from FramedMethodFrame.
    void PromoteCallerStack(promote_func* fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        PromoteCallerStackWorker(fn, sc, FALSE);
    }

    // For use by classes deriving from FramedMethodFrame.
    void PromoteCallerStackWithPinning(promote_func* fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        PromoteCallerStackWorker(fn, sc, TRUE);
    }
#endif

    // Helper for ComPlus and NDirect method calls that are implemented via
    // compiled stubs. This function retrieves the stub (after unwrapping
    // interceptors) and asks it for the stack count computed by the stublinker.
    void AskStubForUnmanagedCallSite(TADDR* ip,
                                     TADDR* returnIP,
                                     TADDR* returnSP);

#ifdef _PPC_
public:
    // deregister the arguments
    static void Deregister(BYTE* pFrameBase, MetaSig* pSig, BOOL fIsStatic, UINT nActualStackBytes) {
        WRAPPER_CONTRACT;
        RegisterWorker(pFrameBase, pSig, fIsStatic, nActualStackBytes, FALSE);
    }

    // enregister the argument
    static void Enregister(BYTE* pFrameBase, MetaSig* pSig, BOOL fIsStatic, UINT nActualStackBytes) {
        WRAPPER_CONTRACT;
        RegisterWorker(pFrameBase, pSig, fIsStatic, nActualStackBytes, TRUE);
    }

    // this is called from the generated assembly code
    static void RegisterHelper(FramedMethodFrame* pThis, BOOL fEnregister);

    static void RegisterWorker(BYTE* pFrameBase, MetaSig* pSig, BOOL fIsStatic,
        UINT nActualStackBytes, BOOL fEnregister);
#endif // _PPC_


private:

#ifndef DACCESS_COMPILE
    // For use by classes deriving from FramedMethodFrame.
    virtual void PromoteCallerStackWorker(promote_func* fn, ScanContext* sc, BOOL fPinArrays);

    virtual void PromoteCallerStackHelper(promote_func* fn, ScanContext* sc, BOOL fPinArrays,
        ArgIterator *pargit, MetaSig *pmsig);
#endif

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(FramedMethodFrame)
    friend struct StubStackFrame;
};


//----------------------------------------------------------------------

#if defined(_X86_) || defined(_PPC_) || defined(_IA64_)

#define DEFINE_FIELD_FramedMethodFrame_NegInfo FramedMethodFrame::NegInfo m_FramedMethodFrameNegInfo

#define DEFINE_IDENTICAL_FramedMethodFrame_NegInfo  \
    struct NegInfo {                                \
        DEFINE_FIELD_FramedMethodFrame_NegInfo;     \
    };                                              \
    DEFINE_NEG_SPACE_ACCESSORS;

#else

#define DEFINE_FIELD_FramedMethodFrame_NegInfo
#define DEFINE_IDENTICAL_FramedMethodFrame_NegInfo

#endif

//----------------------------------------------------------------------
// The layout of the stub stackframe
//----------------------------------------------------------------------

#if defined(_PPC_)

struct StubStackFrame {
    LinkageArea             link;
    INT32                   params[8]; // max 8 local variables or parameters passed down
    DEFINE_FIELD_FramedMethodFrame_NegInfo;
    FramedMethodFrame       methodframe; // LinkageArea is allocated by caller!
};

#else
// define one if it makes sense for the platform
#endif

//+----------------------------------------------------------------------------
//
//  Class:      TPMethodFrame            private
//
//  Synopsis:   This frame is pushed onto the stack for calls on transparent
//              proxy
//
//+----------------------------------------------------------------------------

class TPMethodFrame : public FramedMethodFrame
{
    VPTR_VTABLE_CLASS(TPMethodFrame, FramedMethodFrame)

    struct NegInfo
    {
#ifdef ENREGISTERED_RETURNTYPE_MAXSIZE
        BYTE        m_returnValue[ENREGISTERED_RETURNTYPE_MAXSIZE];
#else
        BYTE        m_returnValue[sizeof(ARG_SLOT)];
#endif
        DEFINE_FIELD_FramedMethodFrame_NegInfo;
    };

public:

    DEFINE_NEG_SPACE_ACCESSORS;

    virtual int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_TP_METHOD_FRAME;
    }

    // GC protect arguments
#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc);
#endif

    // Return only a valid Method Descriptor
    virtual MethodDesc *GetFunction();

    // For proxy calls m_Datum contains the number of stack bytes containing arguments.
#ifndef DACCESS_COMPILE
    void SetFunction(void *pMD)
    {
        LEAF_CONTRACT;
        m_Datum = (TADDR)pMD;
    }
#endif

    // Return value is stored here
    Object *&GetReturnObject()
    {
        WRAPPER_CONTRACT;
        Object *&pReturn = *(Object **)GetReturnValuePtr();
        return(pReturn);
    }

    // Get return value address
    virtual INT64 *GetReturnValuePtr()
    {
        WRAPPER_CONTRACT;
        return (INT64*) &GetNegInfo()->m_returnValue;
    }


    // Get slot number on which we were called
    INT32 GetSlotNumber()
    {
        WRAPPER_CONTRACT;
        return GetSlotNumber(m_Datum);
    }

    static INT32 GetSlotNumber(TADDR MDorSlot)
    {
        LEAF_CONTRACT;
        if(( MDorSlot & ~0xFFFF) == 0)
        {
            // The slot number was pushed on the stack
            return (INT32)MDorSlot;
        }
        else
        {
            // The method descriptor was pushed on the stack
            return -1;
        }
    }

    // Get offset used during stub generation
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(TPMethodFrame);
    }

    // Our base class is a a M2U TransitionType; but we're not. So override and set us back to None.
    ETransitionType GetTransitionType()
    {
        return TT_NONE;
    }


    // Aid the debugger in finding the actual address of callee
    virtual BOOL TraceFrame(Thread *thread, BOOL fromPatch,
                            TraceDestination *trace, REGDISPLAY *regs);

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(TPMethodFrame)
};


//------------------------------------------------------------------------

enum LeaveRuntimeFlags
{
    FLAG_ENTER_RUNTIME_NOT_REQUIRED = 0x0,
    FLAG_ENTER_RUNTIME_REQUIRED     = 0x1,
#ifdef _X86_
    FLAG_SKIP_HANDLE_THREAD_ABORT   = 0x2,  // Hi-jacking a flag bit to indicate when x86 standlone stubs should not poll for thread abort
#endif
};

//------------------------------------------------------------------------
//  DelegateFrame layout
//              +0      FramedMethodFrame
//              -...    Host Notification DWORD (bit field)
//              -...    CleanupWorkList
//              -...    DelegateEEClass
//
//      m_Datum will store the unmanaged call site, and the DelegateEEClass will live on top of the
//       frame in the negative space.
//
//      Note that the MethodDesc must be munged in the UM fptr case to remove the 'this' parameter

//------------------------------------------------------------------------
// This represents a call through DllImport via a Delegate
//------------------------------------------------------------------------

class DelegateTransitionFrame : public FramedMethodFrame
{
    VPTR_VTABLE_CLASS(DelegateTransitionFrame, FramedMethodFrame)

public:
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(DelegateTransitionFrame);
    }

    // We're overriding this method because the MD signature doesn't match the way we call the function
    // Specifically, the MD has a this pointer, but we are calling the method as static.
    virtual MethodDesc* GetFunction()
    {
        LEAF_CONTRACT;
        return GetInvokeFunctionUnsafe();
    }

    // Get the unmanaged callsite for this transition
    TADDR GetFunctionPointer()
    {
        return m_Datum;
    }

    virtual void UpdateRegDisplay(const PREGDISPLAY pRD);


#ifndef DACCESS_COMPILE
        // For use by classes deriving from FramedMethodFrame.
        virtual void PromoteCallerStackWorker(promote_func* fn, ScanContext* sc, BOOL fPinArrays);

        virtual void PromoteCallerStackHelper(promote_func* fn, ScanContext* sc, BOOL fPinArrays,
            ArgIterator *pargit, MetaSig *pmsig);

    //------------------------------------------------------------------------
    // Performs cleanup on an exception unwind
    //------------------------------------------------------------------------
    virtual void ExceptionUnwind()
    {
        WRAPPER_CONTRACT;
        if (EnterRuntimeRequired())
        {
            Thread::EnterRuntime();
            UnMarkEnterRuntimeRequired();
        }

        // This should be removed after bug 27409 is fixed.
        CONTRACT_VIOLATION(GCViolation);
        GetCleanupWorkList()->Cleanup(TRUE);
    }

    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        // IL stubs don't need their caller's stack reported because the method is IL
        FramedMethodFrame::GcScanRoots(fn, sc);
        PromoteCallerStackWithPinning(fn, sc);
        if (GetCleanupWorkList())
        {
            GetCleanupWorkList()->GcScanRoots(fn, sc);
        }
    }

    VOID MarkEnterRuntimeRequired()
    {
        WRAPPER_CONTRACT;
        if (CLRTaskHosted())
            GetNegInfo()->m_flags = FLAG_ENTER_RUNTIME_REQUIRED;
    }

    VOID UnMarkEnterRuntimeRequired()
    {
        WRAPPER_CONTRACT;
        if (CLRTaskHosted())
            GetNegInfo()->m_flags = FLAG_ENTER_RUNTIME_NOT_REQUIRED;
    }

#endif

    struct NegInfo
    {
        PTR_DelegateEEClass     m_delegateEEClass;
        CleanupWorkList         m_cleanupWorkList;
        LeaveRuntimeFlags       m_flags;
        DEFINE_FIELD_FramedMethodFrame_NegInfo;
    };

    DEFINE_NEG_SPACE_ACCESSORS;
    DEFINE_NEG_SPACE_ACCESSOR_FOR(m_flags, RuntimeRequired);
    DEFINE_NEG_SPACE_ACCESSOR_FOR(m_cleanupWorkList, CleanupWorkList);
    DEFINE_NEG_SPACE_ACCESSOR_FOR(m_delegateEEClass, DelegateClass);

    virtual BOOL IsTransitionToNativeFrame()
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    DelegateEEClass* GetDelegateClass()
    {
        WRAPPER_CONTRACT;
        return GetNegInfo()->m_delegateEEClass;
    }

    // This will return the MD, and it is the caller's responsibility to
    // swizzle the signature to remove the 'this' if needed.
    MethodDesc* GetInvokeFunctionUnsafe()
    {
        LEAF_CONTRACT;

        return PTR_MethodDesc(GetDelegateClass()->m_pInvokeMethod);
    }

    virtual CleanupWorkList *GetCleanupWorkList()
    {
        LEAF_CONTRACT;
        return &GetNegInfo()->m_cleanupWorkList;
    }

#ifndef DACCESS_COMPILE
    BOOL EnterRuntimeRequired()
    {
        WRAPPER_CONTRACT;
        if (CLRTaskHosted())
            return GetNegInfo()->m_flags == FLAG_ENTER_RUNTIME_REQUIRED;
        else
            return FALSE;
    }
#endif // DACCESS_COMPILE

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------

    int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_EXIT;
    };

    void GetUnmanagedCallSite(TADDR* ip,
                              TADDR* returnIP,
                              TADDR* returnSP);

    BOOL TraceFrame(Thread *thread, BOOL fromPatch,
                    TraceDestination *trace, REGDISPLAY *regs);

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(DelegateTransitionFrame)
};


//------------------------------------------------------------------------
// This represents a call to a NDirect method.
//------------------------------------------------------------------------

class NDirectMethodFrame : public FramedMethodFrame
{
    VPTR_ABSTRACT_VTABLE_CLASS(NDirectMethodFrame, FramedMethodFrame)

public:
    //------------------------------------------------------------------------
    // Performs cleanup on an exception unwind
    //------------------------------------------------------------------------
#ifndef DACCESS_COMPILE
    virtual void ExceptionUnwind()
    {
        WRAPPER_CONTRACT;
        if (EnterRuntimeRequired()) {
            Thread::EnterRuntime();
            UnMarkEnterRuntimeRequired();
        }
    }

    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        // IL stubs don't need their caller's stack reported because the method is IL
        FramedMethodFrame::GcScanRoots(fn, sc);
        PromoteCallerStackWithPinning(fn, sc);
        if (GetCleanupWorkList())
        {
            GetCleanupWorkList()->GcScanRoots(fn, sc);
        }
    }
#endif // DACCESS_COMPILE

    virtual BOOL IsTransitionToNativeFrame()
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    struct NegInfo
    {
        LeaveRuntimeFlags   m_flags;      // bitmask of NDirectMethodFrameFlags
        DEFINE_FIELD_FramedMethodFrame_NegInfo;
    };

    DEFINE_NEG_SPACE_ACCESSORS;
    DEFINE_NEG_SPACE_ACCESSOR_FOR(m_flags, LeaveRuntimeFlags);



    virtual CleanupWorkList *GetCleanupWorkList()
    {
        LEAF_CONTRACT;
        return NULL;
    }

#ifndef DACCESS_COMPILE
    VOID MarkEnterRuntimeRequired()
    {
        WRAPPER_CONTRACT;

        if (CLRTaskHosted())
            GetNegInfo()->m_flags = FLAG_ENTER_RUNTIME_REQUIRED;
    }

    VOID UnMarkEnterRuntimeRequired()
    {
        WRAPPER_CONTRACT;

        if (CLRTaskHosted())
            GetNegInfo()->m_flags = FLAG_ENTER_RUNTIME_NOT_REQUIRED;
    }

    BOOL EnterRuntimeRequired()
    {
        WRAPPER_CONTRACT;
        if (CLRTaskHosted())
            return (GetNegInfo()->m_flags == FLAG_ENTER_RUNTIME_REQUIRED);
        else
            return FALSE;
    }
#endif // DACCESS_COMPILE


    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------

    int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_EXIT;
    };

    void GetUnmanagedCallSite(TADDR* ip,
                              TADDR* returnIP,
                              TADDR* returnSP) = 0;

    BOOL TraceFrame(Thread *thread, BOOL fromPatch,
                    TraceDestination *trace, REGDISPLAY *regs);

};


//------------------------------------------------------------------------
// This represents a call to a NDirect method with cleanup.
//------------------------------------------------------------------------
class NDirectMethodFrameEx : public NDirectMethodFrame
{
    VPTR_ABSTRACT_VTABLE_CLASS(NDirectMethodFrameEx, NDirectMethodFrame)

public:

    //------------------------------------------------------------------------
    // Performs cleanup on an exception unwind
    //------------------------------------------------------------------------
#ifndef DACCESS_COMPILE
    virtual void ExceptionUnwind()
    {
        WRAPPER_CONTRACT;
        NDirectMethodFrame::ExceptionUnwind();
        // IL stubs have try/finally clauses in IL to do this clean-up.
        GetCleanupWorkList()->Cleanup(TRUE);
    }
#endif

    struct NegInfo
    {
#ifdef _X86_
        // This currently doesn't provide any perf improvement on non-x86
        Checkpoint                     m_checkpoint;
#endif
        CleanupWorkList             m_cleanupWorkList;
        NDirectMethodFrame::NegInfo m_NDirectMethodFrameNegInfo;
    };

    DEFINE_NEG_SPACE_ACCESSORS;
    DEFINE_NEG_SPACE_ACCESSOR_FOR(m_cleanupWorkList, CleanupWorkList);

    //------------------------------------------------------------------------
    // Gets the cleanup worklist for this method call.
    //------------------------------------------------------------------------
    virtual CleanupWorkList *GetCleanupWorkList()
    {
        WRAPPER_CONTRACT;
        return PTR_CleanupWorkList( PTR_HOST_TO_TADDR(this) + GetOffsetOfCleanupWorkList() );
    }

    // This frame must store some fields at negative offset.
    // This method exposes the size for people needing to allocate
    // TransitionFrames.

    // DACCESS: GetReturnAddressPtr should return the
    // target address of the return address in the frame.
    virtual TADDR GetReturnAddressPtr()
    {
        LEAF_CONTRACT;

        // Although IsCallsiteStateValid() might be false at this point, a caller could
        // be using this to help do the frame initialization (e.g., write NULL where
        // GetReturnAddressPtr() is pointing).  So we don't need to check
        // for IsCallsiteStateValid() here.
        
        return PTR_HOST_MEMBER_TADDR(NDirectMethodFrameEx, this,
                                        m_ReturnAddress);
    }


    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------

    void GetUnmanagedCallSite(TADDR* ip,
                              TADDR* returnIP,
                              TADDR* returnSP) = 0;
};


//------------------------------------------------------------------------
// This represents a call to a NDirect method with the generic worker
// (the subclass is so the debugger can tell the difference)
//------------------------------------------------------------------------

class NDirectMethodFrameGeneric : public NDirectMethodFrameEx
{
    friend class CheckAsmOffsets;

    VPTR_VTABLE_CLASS(NDirectMethodFrameGeneric, NDirectMethodFrameEx)

public:
    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(NDirectMethodFrameGeneric);
    }

    void GetUnmanagedCallSite(TADDR* ip,
                              TADDR* returnIP,
                              TADDR* returnSP);

public:

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(NDirectMethodFrameGeneric)
};


#ifdef _X86_

//------------------------------------------------------------------------
// This represents a call to a NDirect method with the slimstub
// (the subclass is so the debugger can tell the difference)
//------------------------------------------------------------------------
class NDirectMethodFrameSlim : public NDirectMethodFrameEx
{
    VPTR_VTABLE_CLASS(NDirectMethodFrameSlim, NDirectMethodFrameEx)

public:
    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(NDirectMethodFrameSlim);
    }

    void GetUnmanagedCallSite(TADDR* ip,
                              TADDR* returnIP,
                              TADDR* returnSP);

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(NDirectMethodFrameSlim)
};

#endif // _X86_


//------------------------------------------------------------------------
// This represents a call to a NDirect method with the standalone stub (no cleanup)
// (the subclass is so the debugger can tell the difference)
//------------------------------------------------------------------------
class NDirectMethodFrameStandalone : public NDirectMethodFrame
{
    VPTR_VTABLE_CLASS(NDirectMethodFrameStandalone, NDirectMethodFrame)
    VPTR_UNIQUE(VPTR_UNIQUE_NDirectMethodFrameStandalone)

public:
    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(NDirectMethodFrameStandalone);
    }

    void GetUnmanagedCallSite(TADDR* ip,
                              TADDR* returnIP,
                              TADDR* returnSP)
    {
        WRAPPER_CONTRACT;
        
        
        AskStubForUnmanagedCallSite(ip, returnIP, returnSP);
    }


    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(NDirectMethodFrameStandalone)
};



//------------------------------------------------------------------------
// This represents a call to a NDirect method with the standalone stub (with cleanup)
// (the subclass is so the debugger can tell the difference)
//------------------------------------------------------------------------
class NDirectMethodFrameStandaloneCleanup : public NDirectMethodFrameEx
{
    VPTR_VTABLE_CLASS(NDirectMethodFrameStandaloneCleanup,
                      NDirectMethodFrameEx)
    VPTR_UNIQUE(VPTR_UNIQUE_NDirectMethodFrameStandaloneCleanup)

public:
    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(NDirectMethodFrameStandaloneCleanup);
    }

    void GetUnmanagedCallSite(TADDR* ip,
                              TADDR* returnIP,
                              TADDR* returnSP)
    {
        WRAPPER_CONTRACT;


        AskStubForUnmanagedCallSite(ip, returnIP, returnSP);
    }


    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(NDirectMethodFrameStandaloneCleanup)
};


//------------------------------------------------------------------------
// This represents a call Delegate.Invoke for secure delegate
// It's only used to gc-protect the arguments during the call.
// Actually the only reason to have this frame is so a proper
// Assembly can be reported
//------------------------------------------------------------------------

class SecureDelegateFrame : public FramedMethodFrame
{
    VPTR_VTABLE_CLASS(SecureDelegateFrame, FramedMethodFrame)

public:

    DEFINE_IDENTICAL_FramedMethodFrame_NegInfo;

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        FramedMethodFrame::GcScanRoots(fn, sc);
        PromoteCallerStack(fn, sc);
    }
#endif

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(SecureDelegateFrame);
    }

    virtual Assembly *GetAssembly();

    int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_MULTICAST;
    }

    // For the debugger:
    // Our base class, FramedMethodFrame, is a M2U transition;
    // but Delegate.Invoke isn't. So override and fix it here.
    // If we didn't do this, we'd see a Managed/Unmanaged transition in debugger's stack trace.
    virtual ETransitionType GetTransitionType()
    {
        return TT_NONE;
    }

    virtual BOOL TraceFrame(Thread *thread, BOOL fromPatch,
                            TraceDestination *trace, REGDISPLAY *regs);

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(SecureDelegateFrame)
};


//------------------------------------------------------------------------
// This represents a call Multicast.Invoke. It's only used to gc-protect
// the arguments during the iteration.
//------------------------------------------------------------------------

class MulticastFrame : public SecureDelegateFrame
{
    VPTR_VTABLE_CLASS(MulticastFrame, SecureDelegateFrame)

    public:
    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(MulticastFrame);
    }

    virtual Assembly *GetAssembly()
    {
        WRAPPER_CONTRACT;
        return Frame::GetAssembly();
    }

    int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_MULTICAST;
    }

    virtual BOOL TraceFrame(Thread *thread, BOOL fromPatch,
                            TraceDestination *trace, REGDISPLAY *regs);

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(MulticastFrame)
};


//-----------------------------------------------------------------------
// Transition frame from unmanaged to managed
//-----------------------------------------------------------------------

class UnmanagedToManagedFrame : public Frame
{
    friend class CheckAsmOffsets;
    friend struct UMStubStackFrame;

    VPTR_ABSTRACT_VTABLE_CLASS(UnmanagedToManagedFrame, Frame)

public:

    // DACCESS: GetReturnAddressPtr should return the
    // target address of the return address in the frame.
    virtual TADDR GetReturnAddressPtr()
    {
        LEAF_CONTRACT;
        return PTR_HOST_MEMBER_TADDR(UnmanagedToManagedFrame, this,
                                     m_ReturnAddress);
    }

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static BYTE GetOffsetOfReturnAddress()
    {
        LEAF_CONTRACT;
        size_t ofs = offsetof(class UnmanagedToManagedFrame, m_ReturnAddress);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    // Retrives pointer to the lowest-addressed argument on
    // the stack. Depending on the calling convention, this
    // may or may not be the first argument.
    TADDR GetPointerToArguments()
    {
        LEAF_CONTRACT;
        return PTR_HOST_TO_TADDR(this) + sizeof(*this);
    }

    // Exposes an offset for stub generation.
    static BYTE GetOffsetOfArgs()
    {
        LEAF_CONTRACT;
        size_t ofs = sizeof(UnmanagedToManagedFrame);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    // depends on the sub frames to return approp. type here
    virtual TADDR GetDatum()
    {
        LEAF_CONTRACT;
        return m_pvDatum;
    }

    static UINT GetOffsetOfDatum()
    {
        LEAF_CONTRACT;
        return (UINT)offsetof(class UnmanagedToManagedFrame, m_pvDatum);
    }

    int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_ENTRY;
    };

#ifndef DACCESS_COMPILE
    virtual const BYTE* GetManagedTarget()
    {
        LEAF_CONTRACT;
        return NULL;
    }
#endif

    // Return the # of stack bytes pushed by the unmanaged caller.
    virtual UINT GetNumCallerStackBytes() = 0;

    virtual void UpdateRegDisplay(const PREGDISPLAY);

protected:
    TADDR     m_pvDatum;        // type depends on the sub class

#if defined(_PPC_)
    // linkage area
    INT32           m_SavedSP; // stack pointer
    INT32           m_SavedCR; // flags
    TADDR           m_ReturnAddress; // SavedLR - return address into JIT'ted code
    INT32           m_Reserved1;
    INT32           m_Reserved2;
    INT32           m_Reserved3;
#else
    TADDR           m_ReturnAddress;  // return address into unmanaged code
#endif

};

//-----------------------------------------------------------------------
// Structure that tracks the GC information for an unmanaged to managed
// transition frame.
//------------------------------------------------------------------------
struct UnmanagedToManagedCallGCInfo
{
    inline void EnableArgsGCProtection()
    {
        LEAF_CONTRACT;
        m_fArgsGCProtect |= 1;
    }

    inline void DisableArgsGCProtection()
    {
        LEAF_CONTRACT;
        m_fArgsGCProtect &= ~1;
    }

    inline ULONG IsArgsGCProtectionEnabled()
    {
        LEAF_CONTRACT;
        return m_fArgsGCProtect & 1;
    }

    inline void EnableGcScanOwnedByCtxTransFrame()
    {
        LEAF_CONTRACT;
        m_fArgsGCProtect |= 2;
    }

    inline ULONG IsGcScanOwnedByCtxTransFrame()
    {
        LEAF_CONTRACT;
        return m_fArgsGCProtect & 2;
    }

    ULONG       m_fArgsGCProtect;   // Bit 0 for args
                                    // Bit 1 to indicate that GC reporting is owned
                                    // by a ctx transition frame.
};


//-----------------------------------------------------------------------
// Transition frame from unmanaged to managed
//
// this frame contains some object reference at negative
// offset which need to be promoted, the references could be [in] args during
// in the middle of marshalling or [out], [in,out] args that need to be tracked
//------------------------------------------------------------------------

class UnmanagedToManagedCallFrame : public UnmanagedToManagedFrame
{
    friend class CheckAsmOffsets;

    VPTR_ABSTRACT_VTABLE_CLASS(UnmanagedToManagedCallFrame,
                               UnmanagedToManagedFrame)

public:

    struct NegInfo
    {
        CleanupWorkList         m_List;
        PTR_Context             m_pReturnContext;
        LPVOID                  m_pArgs;
        UnmanagedToManagedCallGCInfo m_GCInfo;
        Marshaler  *            m____NOLONGERUSED____; // marshaler structures that want to be notified of GC promotes
#ifdef RETBUF_ARG_SPECIAL_PARAM
        LPVOID                  m_pbRetBuffArg;
#endif // RETBUF_ARG_SPECIAL_PARAM

#ifdef _X86_
        VC5Frame                m_vc5Frame;
        CalleeSavedRegisters    m_calleeSavedRegisters;
#endif
    };

    DEFINE_NEG_SPACE_ACCESSORS;
#ifdef _X86_
    DEFINE_NEG_SPACE_ACCESSOR_FOR(m_vc5Frame, VC5Frame);
#endif
#ifdef RETBUF_ARG_SPECIAL_PARAM
    DEFINE_NEG_SPACE_ACCESSOR_FOR(m_pbRetBuffArg, RetBuffArg);
#endif // RETBUF_ARG_SPECIAL_PARAM

    // Return the # of stack bytes pushed by the unmanaged caller.
    virtual UINT GetNumCallerStackBytes()
    {
        LEAF_CONTRACT;
        return 0;
    }

    // Convert a thrown COM+ exception to an unmanaged result.
#ifndef DACCESS_COMPILE
    virtual UINT32 ConvertComPlusException(OBJECTREF pException)
    {
        LEAF_CONTRACT;
        return 0;
    }
#endif

    //------------------------------------------------------------------------
    // For the debugger.
    //------------------------------------------------------------------------
    virtual ETransitionType GetTransitionType()
    {
        return TT_U2M;
    }

    //------------------------------------------------------------------------
    // Performs cleanup on an exception unwind
    //------------------------------------------------------------------------
#ifndef DACCESS_COMPILE
    virtual void ExceptionUnwind();
#endif

    CleanupWorkList *GetCleanupWorkList()
    {
        WRAPPER_CONTRACT;
        return &GetNegInfo()->m_List;
    }

    UnmanagedToManagedCallGCInfo* GetGCInfoPtr()
    {
        WRAPPER_CONTRACT;
        return &GetNegInfo()->m_GCInfo;
    }

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        if (!GetGCInfoPtr()->IsGcScanOwnedByCtxTransFrame())
            GcScanRootsWorker(fn, sc);
    }

    void GcScanRootsFromCtxTransFrame(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        GcScanRootsWorker(fn, sc);
    }
#endif

    virtual PTR_Context* GetReturnContextAddr()
    {
        WRAPPER_CONTRACT;
        return &(GetNegInfo()->m_pReturnContext);
    }

    // ********************** END OF WARNING *************************


#ifdef RETBUF_ARG_SPECIAL_PARAM
    LPVOID GetRetBuffArg()
    {
        return GetNegInfo()->m_pbRetBuffArg;
    }
#endif // RETBUF_ARG_SPECIAL_PARAM

    virtual LPVOID GetPointerToDstArgs()
    {
        WRAPPER_CONTRACT;
        return GetNegInfo()->m_pArgs;
    }

    virtual void SetDstArgsPointer(LPVOID pv)
    {
        WRAPPER_CONTRACT;
        _ASSERTE(pv != NULL);
        GetNegInfo()->m_pArgs = pv;
    }

    // UnmanagedToManagedCallFrames must store some fields at negative offset.
    // This method exposes the size

protected:
#ifndef DACCESS_COMPILE
    virtual void GcScanRootsWorker(promote_func *fn, ScanContext* sc);
#endif
};

typedef DPTR(class UnmanagedToManagedCallFrame) PTR_UnmanagedToManagedCallFrame;


//------------------------------------------------------------------------
// This represents a call from ComPlus to unmanaged target
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// This represents a declarative secuirty check. This frame is inserted
// prior to calls on methods that have declarative security defined for
// the class or the specific method that is being called. This frame
// is only created when the prestubworker creates a real stub.
//------------------------------------------------------------------------
class SecurityFrame : public FramedMethodFrame
{
    VPTR_VTABLE_CLASS(SecurityFrame, FramedMethodFrame)

public:

    struct NegInfo
    {
        // Additional field for referencing per-frame security information.
        // This field is not necessary for most frames, so it is a costly
        // addition for all frames. Leave it here for M3 after which we can
        // be smarter about when to insert this extra field. This field should
        // not always be added to the negative offset
        OBJECTREF   m_securityData;

        DEFINE_FIELD_FramedMethodFrame_NegInfo;
    };

public:

    DEFINE_NEG_SPACE_ACCESSORS;

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(SecurityFrame);
    }

    //-----------------------------------------------------------
    // Returns the address of the frame security descriptor ref
    //-----------------------------------------------------------

    virtual OBJECTREF *GetAddrOfSecurityDesc()
    {
        WRAPPER_CONTRACT;
        return & GetNegInfo()->m_securityData;
    }

#ifndef DACCESS_COMPILE
    VOID GcScanRoots(promote_func *fn, ScanContext* sc);
#endif

    BOOL TraceFrame(Thread *thread, BOOL fromPatch,
                    TraceDestination *trace, REGDISPLAY *regs);

    int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_SECURITY;
    }


    virtual void UpdateRegDisplay(const PREGDISPLAY pRD);


private:

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(SecurityFrame)
};


//------------------------------------------------------------------------
// This represents a call to a method prestub. Because the prestub
// can do gc and throw exceptions while building the replacement
// stub, we need this frame to keep things straight.
//------------------------------------------------------------------------

class PrestubMethodFrame : public FramedMethodFrame
{
    VPTR_VTABLE_CLASS(PrestubMethodFrame, FramedMethodFrame)

public:

    DEFINE_IDENTICAL_FramedMethodFrame_NegInfo;

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        FramedMethodFrame::GcScanRoots(fn, sc);
        PromoteCallerStack(fn, sc);
    }
#endif


    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(PrestubMethodFrame);
    }

    BOOL TraceFrame(Thread *thread, BOOL fromPatch,
                    TraceDestination *trace, REGDISPLAY *regs);

    int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_INTERCEPTION;
    }

    // Our base class is a a M2U TransitionType; but we're not. So override and set us back to None.
    ETransitionType GetTransitionType()
    {
        return TT_NONE;
    }

    Interception GetInterception();

    // Link this frame, setting the vptr
    VOID Push();

private:
    friend const BYTE * __stdcall PreStubWorker(PrestubMethodFrame *pPFrame);

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(PrestubMethodFrame)
};



//------------------------------------------------------------------------
// this frame is used whenever we reset the stack pointer before
// calling a catch handler.  It's needed on win64 because we call
// our catches as functions instead of jumping to them as x86 does.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// This represents a call to a method prestub. Because the prestub
// can do gc and throw exceptions while building the replacement
// stub, we need this frame to keep things straight.
//------------------------------------------------------------------------

class InterceptorFrame : public SecurityFrame
{
    VPTR_VTABLE_CLASS(InterceptorFrame, SecurityFrame)

public:

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        SecurityFrame::GcScanRoots(fn, sc);
        PromoteCallerStack(fn, sc);
    }
#endif

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.

    //---------------------------------------------------------------

    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(InterceptorFrame);
    }

    Interception GetInterception();


    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(InterceptorFrame)
};



//------------------------------------------------------------------------
// This frame protects object references for the EE's convenience.
// This frame type actually is created from C++.
//------------------------------------------------------------------------
class GCFrame : public Frame
{
    VPTR_VTABLE_CLASS(GCFrame, Frame)

public:


    //--------------------------------------------------------------------
    // This constructor pushes a new GCFrame on the frame chain.
    //--------------------------------------------------------------------
#ifndef DACCESS_COMPILE
    GCFrame() {
        LEAF_CONTRACT;
    };

    GCFrame(OBJECTREF *pObjRefs, UINT numObjRefs, BOOL maybeInterior);
#endif
    void Init(Thread *pThread, OBJECTREF *pObjRefs, UINT numObjRefs, BOOL maybeInterior);


    //--------------------------------------------------------------------
    // Pops the GCFrame and cancels the GC protection. Also
    // trashes the contents of pObjRef's in _DEBUG.
    //--------------------------------------------------------------------
    VOID Pop();

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc);
#endif

#ifdef _DEBUG
    virtual BOOL Protects(OBJECTREF *ppORef)
    {
        LEAF_CONTRACT;
        for (UINT i = 0; i < m_numObjRefs; i++) {
            if (ppORef == m_pObjRefs + i) {
                return TRUE;
            }
        }
        return FALSE;
    }
#endif

#ifndef DACCESS_COMPILE
    void *operator new (size_t sz, void* p)
    {
        LEAF_CONTRACT;
        return p ;
    }
#endif

#if defined(_DEBUG_IMPL)
    char* GetFrameTypeName() { return "GCFrame"; }
#endif

private:
    OBJECTREF *m_pObjRefs;
    UINT       m_numObjRefs;
    Thread    *m_pCurThread;
    BOOL       m_MaybeInterior;

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(GCFrame)
};


// Like GCFrame this frame helps protect object references. The
// difference is, it lets you specify your own GC scanning function.
// Its suitable if the GC references arent in an array, like GCFrame expects
typedef void (*GCSCANFUNCTION) (promote_func *, ScanContext *, void *);

class CustomGCFrame : public Frame
{
    VPTR_VTABLE_CLASS(CustomGCFrame, Frame)
    GCSCANFUNCTION   m_pScanFunction;
    void           * m_pContext;

    public:
        //--------------------------------------------------------------------
        // This constructor pushes a new GCFrame on the frame chain.
        //--------------------------------------------------------------------
#ifndef DACCESS_COMPILE
        CustomGCFrame() { };
        CustomGCFrame(GCSCANFUNCTION scanFunc, void *context);
#endif
#ifndef DACCESS_COMPILE
        virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
        {
            _ASSERTE(m_pScanFunction);
            _ASSERTE(m_pContext);
            (*m_pScanFunction)(fn, sc, m_pContext);
        }
#endif

        VOID Pop();
    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(CustomGCFrame)
};


//-----------------------------------------------------------------------------

struct ByRefInfo
{
    ByRefInfo *pNext;
    INT32      argIndex;
    CorElementType typ;
    TypeHandle typeHandle;
    char       data[1];
};

//-----------------------------------------------------------------------------
// ProtectByRefsFrame
//-----------------------------------------------------------------------------

class ProtectByRefsFrame : public Frame
{
    VPTR_VTABLE_CLASS(ProtectByRefsFrame, Frame)

public:
    ProtectByRefsFrame(Thread *pThread, ByRefInfo *brInfo);
    void Pop();

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext *sc);
#endif

private:
    ByRefInfo *m_brInfo;
    Thread    *m_pThread;

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(ProtectByRefsFrame)
};


//-----------------------------------------------------------------------------

struct ValueClassInfo
{
    ValueClassInfo  *pNext;
    INT32           argIndex;
    CorElementType  typ;
    TypeHandle      typeHandle;
    LPVOID          pData;
};

//-----------------------------------------------------------------------------
// ProtectValueClassFrame
//-----------------------------------------------------------------------------


class ProtectValueClassFrame : public Frame
{
    VPTR_VTABLE_CLASS(ProtectValueClassFrame, Frame)

public:
    ProtectValueClassFrame(Thread *pThread, ValueClassInfo *vcInfo);
    void Pop();

    static void PromoteValueClassEmbeddedObjects(promote_func *fn, ScanContext *sc,
                                          TypeHandle ty, PVOID pvObject);
#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext *sc);
#endif

private:

    ValueClassInfo *m_pVCInfo;
    Thread    *m_pThread;

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(ProtectValueClassFrame)
};


#ifdef _DEBUG
BOOL IsProtectedByGCFrame(OBJECTREF *ppObjectRef);
#endif

void DoPromote(promote_func *fn, ScanContext* sc, OBJECTREF *address, BOOL interior);


//------------------------------------------------------------------------
// DebuggerClassInitMarkFrame is a small frame whose only purpose in
// life is to mark for the debugger that "class initialization code" is
// being run. It does nothing useful except return good values from
// GetFrameType and GetInterception.
//------------------------------------------------------------------------

class DebuggerClassInitMarkFrame : public Frame
{
    VPTR_VTABLE_CLASS(DebuggerClassInitMarkFrame, Frame)

public:

#ifndef DACCESS_COMPILE
    DebuggerClassInitMarkFrame()
    {
        WRAPPER_CONTRACT;
        Push();
    };
#endif

    virtual int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_INTERCEPTION;
    }

    virtual Interception GetInterception()
    {
        LEAF_CONTRACT;
        return INTERCEPTION_CLASS_INIT;
    }

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(DebuggerClassInitMarkFrame)
};


//------------------------------------------------------------------------
// DebuggerSecurityCodeMarkFrame is a small frame whose only purpose in
// life is to mark for the debugger that "security code" is
// being run. It does nothing useful except return good values from
// GetFrameType and GetInterception.
//------------------------------------------------------------------------

class DebuggerSecurityCodeMarkFrame : public Frame
{
    VPTR_VTABLE_CLASS(DebuggerSecurityCodeMarkFrame, Frame)

public:
#ifndef DACCESS_COMPILE
    DebuggerSecurityCodeMarkFrame()
    {
        WRAPPER_CONTRACT;
        Push();
    }
#endif

    virtual int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_INTERCEPTION;
    }

    virtual Interception GetInterception()
    {
        LEAF_CONTRACT;
        return INTERCEPTION_SECURITY;
    }

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(DebuggerSecurityCodeMarkFrame)
};

//------------------------------------------------------------------------
// DebuggerExitFrame is a small frame whose only purpose in
// life is to mark for the debugger that there is an exit transiton on
// the stack.  This is special cased for the "break" IL instruction since
// it is an fcall using a helper frame which returns TYPE_CALL instead of
// an ecall (as in System.Diagnostics.Debugger.Break()) which returns
// TYPE_EXIT.  This just makes the two consistent for debugging services.
//------------------------------------------------------------------------

class DebuggerExitFrame : public Frame
{
    VPTR_VTABLE_CLASS(DebuggerExitFrame, Frame)

public:
#ifndef DACCESS_COMPILE
    DebuggerExitFrame()
    {
        WRAPPER_CONTRACT;
        Push();
    }
#endif

    virtual int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_EXIT;
    }

    // Return information about an unmanaged call the frame
    // will make.
    // ip - the unmanaged routine which will be called
    // returnIP - the address in the stub which the unmanaged routine
    //            will return to.
    // returnSP - the location returnIP is pushed onto the stack
    //            during the call.
    //
    virtual void GetUnmanagedCallSite(TADDR* ip,
                                      TADDR* returnIP,
                                      TADDR* returnSP)
    {
        LEAF_CONTRACT;
        if (ip)
            *ip = NULL;

        if (returnIP)
            *returnIP = NULL;

        if (returnSP)
            *returnSP = NULL;
    }

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(DebuggerExitFrame)
};

class UMThunkMarshInfo;
typedef DPTR(class UMThunkMarshInfo) PTR_UMThunkMarshInfo;

class UMEntryThunk;
typedef DPTR(class UMEntryThunk) PTR_UMEntryThunk;


//------------------------------------------------------------------------
// This frame guards an unmanaged->managed transition thru a UMThk
//------------------------------------------------------------------------

class UMThkCallFrame : public UnmanagedToManagedCallFrame
{
    friend class UMThunkStubCache;

    VPTR_VTABLE_CLASS(UMThkCallFrame, UnmanagedToManagedCallFrame)

public:

    // promote callee stack, if we are the topmost frame
    void PromoteCalleeStack(promote_func *fn, ScanContext* sc);

    // used by PromoteCalleeStack to get the destination function sig and module
    // NOTE: PromoteCalleeStack only promotes bona-fide arguments, and not
    // the "this" reference. The whole purpose of PromoteCalleeStack is
    // to protect the partially constructed argument array during
    // the actual process of argument marshaling.
#ifndef DACCESS_COMPILE
    virtual PCCOR_SIGNATURE GetTargetCallSig(DWORD *pcbSigSize);
    virtual Module *GetTargetModule();
#endif

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    // Return the # of stack bytes pushed by the unmanaged caller.
    UINT GetNumCallerStackBytes();


    UMEntryThunk *GetUMEntryThunk();

    static UINT GetOffsetOfUMEntryThunk()
    {
        WRAPPER_CONTRACT;
        return GetOffsetOfDatum();
    }

#ifndef DACCESS_COMPILE
    const BYTE* GetManagedTarget();
#endif

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetUMThkCallFrameVPtr();

protected:
#ifndef DACCESS_COMPILE
    virtual void GcScanRootsWorker(promote_func *fn, ScanContext* sc)
    {
        WRAPPER_CONTRACT;
        PromoteCalleeStack(fn, sc);
        UnmanagedToManagedCallFrame::GcScanRootsWorker(fn, sc);
    }
#endif

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(UMThkCallFrame)
};

//------------------------------------------------------------------------

#if defined(_X86_)
struct UMStubStackFrame {
    EXCEPTION_REGISTRATION_RECORD   exReg;

    // scratch area - corresponds to FrameHandlerExRecord::m_tct
    ThrowCallbackType               tct;

    GSCookieFor<UnmanagedToManagedCallFrame::NegInfo> info;
    UMThkCallFrame                  umframe; // LinkageArea is allocated by caller!
};

#elif defined(_PPC_)

struct UMStubStackFrame {
    LinkageArea                     link;
    INT32                           params[2]; // max 2 parameters passed down

    EXCEPTION_REGISTRATION_RECORD   exReg;

    // scratch area - corresponds to FrameHandlerExRecord::m_tct
    ThrowCallbackType               tct;

#ifdef _DEBUG
    BYTE                            padding[8];
#endif

    GSCookieFor<UnmanagedToManagedCallFrame::NegInfo> info;
    UMThkCallFrame                  umframe; // LinkageArea is allocated by caller!
};

#else
// define one if it makes sense for the platform
#endif


//-----------------------------------------------------------------------------
// ReverseEnterRuntimeFrame
//-----------------------------------------------------------------------------

class ReverseEnterRuntimeFrame : public Frame
{
    VPTR_VTABLE_CLASS(ReverseEnterRuntimeFrame, Frame)

public:
    //------------------------------------------------------------------------
    // Performs cleanup on an exception unwind
    //------------------------------------------------------------------------
#ifndef DACCESS_COMPILE
    virtual void ExceptionUnwind()
    {
        WRAPPER_CONTRACT;
         GetThread()->ReverseLeaveRuntime();
    }
#endif

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------

    static TADDR GetReverseEnterRuntimeFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(ReverseEnterRuntimeFrame);
    }

    static UINT32 GetNegSpaceSize()
    {
        LEAF_CONTRACT;
        return sizeof(GSCookie);
    }

    static INT32 GetOffsetOfGSCookie()
    {
        LEAF_CONTRACT;
        ptrdiff_t offs = (ptrdiff_t)(0 - sizeof(GSCookie));
        _ASSERTE(FitsInI4(offs));
        return (INT32)offs;
    }

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(ReverseEnterRuntimeFrame)
};


//-----------------------------------------------------------------------------
// LeaveRuntimeFrame
//-----------------------------------------------------------------------------

class LeaveRuntimeFrame : public Frame
{
    VPTR_VTABLE_CLASS(LeaveRuntimeFrame, Frame)

public:
    //------------------------------------------------------------------------
    // Performs cleanup on an exception unwind
    //------------------------------------------------------------------------
#ifndef DACCESS_COMPILE
    virtual void ExceptionUnwind()
    {
        WRAPPER_CONTRACT;
         Thread::EnterRuntime();
    }
#endif

    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetLeaveRuntimeFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(LeaveRuntimeFrame);
    }

    static INT32 GetOffsetOfGSCookie()
    {
        LEAF_CONTRACT;
        ptrdiff_t offs = (ptrdiff_t)(0 - sizeof(GSCookie));
        _ASSERTE(FitsInI4(offs));
        return (INT32)offs;
    }

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(LeaveRuntimeFrame)
};


//------------------------------------------------------------------------
// This frame is pushed by any JIT'ted method that contains one or more
// inlined N/Direct calls. Note that the JIT'ted method keeps it pushed
// the whole time to amortize the pushing cost across the entire method.
//------------------------------------------------------------------------

struct CORINFO_EE_INFO;

class InlinedCallFrame : public Frame
{
    VPTR_VTABLE_CLASS(InlinedCallFrame, Frame)

public:
    virtual MethodDesc *GetFunction()
    {
        WRAPPER_CONTRACT;
        if (FrameHasActiveCall(this) &&
            ((PTR_TO_TADDR(m_Datum)) & ~0xffff) != 0)
            return PTR_MethodDesc(m_Datum);
        else
            return NULL;
    }

    // Retrieves the return address into the code that called out
    // to managed code
    virtual TADDR GetReturnAddressPtr()
    {
        WRAPPER_CONTRACT;
        /* m_pCallSiteSP contains the ESP just before the call, i.e.*/
        /* the return address pushed by the call is just in front of it  */

        if (FrameHasActiveCall(this))
            return PTR_HOST_MEMBER_TADDR(InlinedCallFrame, this,
                                            m_pCallerReturnAddress);
        else
            return NULL;
    }


    virtual void UpdateRegDisplay(const PREGDISPLAY);

protected:

    // func desc of the NDirect function being called,
    // or the size of the stack arguments for an indirect call
    PTR_NDirectMethodDesc   m_Datum;

    // ESP after pushing the outgoing arguments, and just before calling out
    // to the ndirect method.
    // This is set to NULL in the method prolog. It gets set just before the
    // first call to an NDirect method. It does not get reset after that,
    // though we could do that if really required.
    // We rely on the stack walk completely skipping over any inactive
    // InlinedCallFrames.
    LPVOID               m_pCallSiteSP;

    // EIP where the unmanaged call will return to. This will be a pointer into
    // the code of the managed frame which has the InlinedCallFrame
    TADDR                m_pCallerReturnAddress;

    // This is used only for EBP. Hence, a stackwalk will miss the other
    // callee-saved registers for the method with the InlinedCallFrame.
    // To prevent GC-holes, we do not keep any GC references in callee-saved
    // registers across an NDirect call.
    CalleeSavedRegisters m_pCalleeSavedRegisters;

public:
    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static TADDR GetInlinedCallFrameFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(InlinedCallFrame);
    }

    static void GetEEInfo(CORINFO_EE_INFO * pEEInfo);

    // Is the specified frame an InlinedCallFrame which has an active call
    // inside it right now?
    static BOOL FrameHasActiveCall(Frame *pFrame)
    {
        WRAPPER_CONTRACT;
        return (pFrame &&
                (pFrame != FRAME_TOP) &&
                (GetInlinedCallFrameFrameVPtr() == pFrame->GetVTablePtr()) &&
                (((InlinedCallFrame *)pFrame)->m_pCallSiteSP != 0));
    }

    int GetFrameType()
    {
        LEAF_CONTRACT;
        return TYPE_INTERNAL; // will have to revisit this case later
    }

    virtual BOOL IsTransitionToNativeFrame()
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(InlinedCallFrame)
};

//------------------------------------------------------------------------
// This frame is used to mark a Context/AppDomain Transition
//------------------------------------------------------------------------

class ContextTransitionFrame : public Frame
{
private:

    // to preserve the state between CPFH_RealFirstPassHandler and COMPlusAfterUnwind
    ThrowCallbackType m_tct;

    PTR_Context m_pReturnContext;
    Object*     m_ReturnExecutionContext;
    Object*     m_LastThrownObjectInParentContext;                                        
    ULONG_PTR   m_LockCount;            // Number of locks the thread takes
                                        // before the transition.
    ULONG_PTR   m_CriticalRegionCount;
    PTR_UnmanagedToManagedCallFrame m_pOwnedUMCallFrame;

    VPTR_VTABLE_CLASS(ContextTransitionFrame, Frame)

public:

#ifndef DACCESS_COMPILE
    virtual void GcScanRoots(promote_func *fn, ScanContext* sc);
#endif

    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(ContextTransitionFrame);
    }

    virtual PTR_Context* GetReturnContextAddr()
    {
        LEAF_CONTRACT;
        return &m_pReturnContext;
    }

    virtual Object **GetReturnExecutionContextAddr()
    {
        LEAF_CONTRACT;
        return (Object **) &m_ReturnExecutionContext;
    }

    OBJECTREF GetLastThrownObjectInParentContext()
    {
        return ObjectToOBJECTREF(m_LastThrownObjectInParentContext);
    }

    void SetLastThrownObjectInParentContext(OBJECTREF lastThrownObject)
    {
        m_LastThrownObjectInParentContext = OBJECTREFToObject(lastThrownObject);
    }

    void SetLockCount(DWORD lockCount, DWORD criticalRegionCount)
    {
        LEAF_CONTRACT;
        m_LockCount = lockCount;
        m_CriticalRegionCount = criticalRegionCount;
    }
    void GetLockCount(DWORD* pLockCount, DWORD* pCriticalRegionCount)
    {
        LEAF_CONTRACT;
        *pLockCount = (DWORD) m_LockCount;
        *pCriticalRegionCount = (DWORD) m_CriticalRegionCount;
    }

    void SetOwnedUMCallFrame(PTR_UnmanagedToManagedCallFrame pFrame)
    {
        LEAF_CONTRACT;
        pFrame->GetGCInfoPtr()->EnableGcScanOwnedByCtxTransFrame();
        m_pOwnedUMCallFrame = pFrame;
    }

    // Let debugger know that we're transitioning between AppDomains.
    ETransitionType GetTransitionType()
    {
        return TT_AppDomain;
    }

#ifndef DACCESS_COMPILE
    ContextTransitionFrame()
    : m_pReturnContext(NULL)
    , m_ReturnExecutionContext(NULL)
    , m_LastThrownObjectInParentContext(NULL)
    , m_LockCount(0)
    , m_CriticalRegionCount(0)
    , m_pOwnedUMCallFrame(NULL)
    {
        LEAF_CONTRACT;
    }
#endif

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(ContextTransitionFrame)
};

bool isRetAddr(TADDR retAddr, TADDR* whereCalled);

#ifdef _X86_
//------------------------------------------------------------------------
// This frame is used as padding for virtual stub dispatch tailcalls.
//------------------------------------------------------------------------

class TailCallFrame : public Frame
{
    VPTR_VTABLE_CLASS(TailCallFrame, Frame)

    TADDR           m_CallerAddress;    // the address the tailcall was initiated from
    CalleeSavedRegisters    m_regs;     // callee saved registers - the stack walk assumes that all non-JIT frames have them
    TADDR           m_ReturnAddress;    // the return address of the tailcall

public:
    virtual TADDR GetReturnAddressPtr()
    {
        LEAF_CONTRACT;
        return PTR_HOST_MEMBER_TADDR(TailCallFrame, this,
                                        m_ReturnAddress);
    }

    virtual void UpdateRegDisplay(const PREGDISPLAY pRD);

#ifndef DACCESS_COMPILE
    static TailCallFrame* FindTailCallFrame(Frame* pFrame)
    {
        LEAF_CONTRACT;
        // loop through the frame chain
        while (pFrame->GetVTablePtr() != TailCallFrame::GetFrameVtable())
            pFrame = pFrame->m_Next;
        return (TailCallFrame*)pFrame;
    }

    TADDR GetCallerAddress()
    {
        LEAF_CONTRACT;
        return m_CallerAddress;
    }
#endif // #ifndef DACCESS_COMPILE

private:
    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(TailCallFrame)
};
#endif

//------------------------------------------------------------------------
// ExceptionFilterFrame is a small frame whose only purpose in
// life is to set SHADOW_SP_FILTER_DONE during unwind from exception filter.
//------------------------------------------------------------------------

class ExceptionFilterFrame : public Frame
{
    VPTR_VTABLE_CLASS(ExceptionFilterFrame, Frame)
    size_t* m_pShadowSP;

public:
#ifndef DACCESS_COMPILE
    ExceptionFilterFrame(size_t* pShadowSP)
    {
        WRAPPER_CONTRACT;
        m_pShadowSP = pShadowSP;
        Push();
    }

    void Pop()
    {
        // Nothing to do here.
        WRAPPER_CONTRACT;
        SetFilterDone();
        Frame::Pop();
    }

    void SetFilterDone()
    {
        LEAF_CONTRACT;

        // Mark the filter as having completed
        if (m_pShadowSP)
        {
            // Make sure that CallJitEHFilterHelper marked us as being in the filter.
            _ASSERTE(*m_pShadowSP & ICodeManager::SHADOW_SP_IN_FILTER);
            *m_pShadowSP |= ICodeManager::SHADOW_SP_FILTER_DONE;
        }
    }
#endif

private:
    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(ExceptionFilterFrame)
};

//------------------------------------------------------------------------

#ifdef _DEBUG
// We use IsProtectedByGCFrame to check if some OBJECTREF pointers are protected 
// against GC. That function doesn't know if a byref is from managed stack thus
// protected by JIT. AssumeByrefFromJITStack is used to bypass that check if an 
// OBJECTRef pointer is passed from managed code to an FCall and it's in stack.
class AssumeByrefFromJITStack : public Frame
{
    VPTR_VTABLE_CLASS(AssumeByrefFromJITStack, Frame)
public:
#ifndef DACCESS_COMPILE
    AssumeByrefFromJITStack(OBJECTREF *pObjRef)
    {
        m_pObjRef      = pObjRef;
    }
#endif

    BOOL Protects(OBJECTREF *ppORef)
    {
        LEAF_CONTRACT;
        return ppORef == m_pObjRef;
    }

private:
    OBJECTREF *m_pObjRef;

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER(AssumeByrefFromJITStack)
}; //AssumeByrefFromJITStack

#endif //_DEBUG

//-----------------------------------------------------------------------------
// FrameWithCookie is used to declare a Frame in source code with a cookie
// immediately preceeding it.
// This is just a specialized version of GSCookieFor<T>
//
// For Frames that are set up by stubs, the stub is responsible for setting up
// the GSCookie.
//
// Note that we have to play all these games for the GSCookie as the GSCookie
// needs to precede the vtable pointer, as well as any other NegInfo, so that
// the GSCookie is guaranteed to catch any stack-buffer-overrun corruptions
// that overwrite the Frame data. Since different Frame types had NegInfo of
// different sizes, the GSCookie cannot be at a fixed offset, and hence cannot
// be declared as a simple field of FrameBase.
//
//-----------------------------------------------------------------------------


class DebuggerEval;

template <typename FrameType>
class FrameWithCookie
{
protected:

    GSCookie        m_gsCookie;
    FrameType       m_frame;

public:

    //
    // Overload all the required constructors
    //

    FrameWithCookie() :
        m_gsCookie(GetProcessGSCookie()), m_frame() { WRAPPER_CONTRACT; }

    FrameWithCookie(CONTEXT * pContext) :
        m_gsCookie(GetProcessGSCookie()), m_frame(pContext) { WRAPPER_CONTRACT; }

    // GCFrame
    FrameWithCookie(OBJECTREF *pObjRefs, UINT numObjRefs, BOOL maybeInterior) :
        m_gsCookie(GetProcessGSCookie()), m_frame(pObjRefs, numObjRefs, maybeInterior) { WRAPPER_CONTRACT; }

    // CustomGCFrame
    FrameWithCookie(GCSCANFUNCTION scanFunc, void *context) :
        m_gsCookie(GetProcessGSCookie()), m_frame(scanFunc, context) { WRAPPER_CONTRACT; }

    // HijackFrame
    FrameWithCookie(LPVOID returnAddress, Thread *thread, HijackArgs *args) :
        m_gsCookie(GetProcessGSCookie()), m_frame(returnAddress, thread, args) { WRAPPER_CONTRACT; }

    // FuncEvalFrame
    FrameWithCookie(DebuggerEval *pDebuggerEval, LPVOID returnAddress, BOOL showFrame) :
        m_gsCookie(GetProcessGSCookie()), m_frame(pDebuggerEval, returnAddress, showFrame) { WRAPPER_CONTRACT; }

#ifndef DACCESS_COMPILE
    // HelperMethodFrame
    FrameWithCookie(void* fCallFtnEntry, struct LazyMachState* ms, unsigned attribs = 0) :
        m_gsCookie(GetProcessGSCookie()), m_frame(fCallFtnEntry, ms, attribs) { WRAPPER_CONTRACT; }

    FrameWithCookie(struct MachState* ms, MethodDesc* pMD, ArgumentRegisters* regArgs) :
        m_gsCookie(GetProcessGSCookie()), m_frame(ms, pMD, regArgs) { WRAPPER_CONTRACT; }

    FrameWithCookie(HelperMethodFrame::tagWithRefData dummytag, MachState* ms, WORD wRefData, ArgumentRegisters* regArgs) :
        m_gsCookie(GetProcessGSCookie()), m_frame(dummytag, ms, wRefData, regArgs) { WRAPPER_CONTRACT; }

    // HelperMethodFrame_1OBJ
    FrameWithCookie(void* fCallFtnEntry, struct LazyMachState* ms, unsigned attribs, OBJECTREF * aGCPtr1) :
        m_gsCookie(GetProcessGSCookie()), m_frame(fCallFtnEntry, ms, attribs, aGCPtr1) { WRAPPER_CONTRACT; }

    // HelperMethodFrame_2OBJ
    FrameWithCookie(void* fCallFtnEntry, struct LazyMachState* ms, unsigned attribs, OBJECTREF * aGCPtr1, OBJECTREF * aGCPtr2) :
        m_gsCookie(GetProcessGSCookie()), m_frame(fCallFtnEntry, ms, attribs, aGCPtr1, aGCPtr2) { WRAPPER_CONTRACT; }

    // HelperMethodFrame_PROTECTOBJ
    FrameWithCookie(void* fCallFtnEntry, struct LazyMachState* ms, unsigned attribs, OBJECTREF* pObjRefs, int numObjRefs) :
        m_gsCookie(GetProcessGSCookie()), m_frame(fCallFtnEntry, ms, attribs, pObjRefs, numObjRefs) { WRAPPER_CONTRACT; }

#endif // DACCESS_COMPILE

    // ProtectByRefsFrame
    FrameWithCookie(Thread * pThread, ByRefInfo * pByRefs) :
        m_gsCookie(GetProcessGSCookie()), m_frame(pThread, pByRefs) { WRAPPER_CONTRACT; }

    // ProtectValueClassFrame
    FrameWithCookie(Thread * pThread, ValueClassInfo * pValueClasses) :
        m_gsCookie(GetProcessGSCookie()), m_frame(pThread, pValueClasses) { WRAPPER_CONTRACT; }

    // ExceptionFilterFrame
    FrameWithCookie(size_t* pShadowSP) :
        m_gsCookie(GetProcessGSCookie()), m_frame(pShadowSP) { WRAPPER_CONTRACT; }
        
#ifdef _DEBUG
    // AssumeByrefFromJITStack
    FrameWithCookie(OBJECTREF *pObjRef) :
        m_gsCookie(GetProcessGSCookie()), m_frame(pObjRef) { WRAPPER_CONTRACT; }
#endif //_DEBUG

    //
    // Overload some common Frame methods for easy redirection
    //

    void Push() { WRAPPER_CONTRACT; m_frame.Push(); }
    void Pop() { WRAPPER_CONTRACT; m_frame.Pop(); }
    void Push(Thread * pThread) { WRAPPER_CONTRACT; m_frame.Push(pThread); }
    void Pop(Thread * pThread) { WRAPPER_CONTRACT; m_frame.Pop(pThread); }
    TADDR GetReturnAddress() { WRAPPER_CONTRACT; return m_frame.GetReturnAddress(); }
    CONTEXT * GetContext() { WRAPPER_CONTRACT; return m_frame.GetContext(); }
    FrameType* operator&() { LEAF_CONTRACT; return &m_frame; }
    MachState * MachineState() { WRAPPER_CONTRACT; return m_frame.MachineState(); }
    TADDR InsureInit(bool initialInit, struct MachState* unwindState)
        { WRAPPER_CONTRACT; return m_frame.InsureInit(initialInit, unwindState); }
    void Poll() { WRAPPER_CONTRACT; m_frame.Poll(); }
    void SetStackPointerPtr(TADDR sp) { WRAPPER_CONTRACT; m_frame.SetStackPointerPtr(sp); }
    void InitAndLink(CONTEXT *pContext) { WRAPPER_CONTRACT; m_frame.InitAndLink(pContext); }
    void Init(Thread *pThread, OBJECTREF *pObjRefs, UINT numObjRefs, BOOL maybeInterior)
        { WRAPPER_CONTRACT; m_frame.Init(pThread, pObjRefs, numObjRefs, maybeInterior); }


    // Since the "&" operator is overloaded, use this function to get to the
    // address of FrameWithCookie, rather than that of FrameWithCookie::m_frame.
    GSCookie * GetGSCookiePtr() { LEAF_CONTRACT; return &m_gsCookie; }

#if defined(PAL_PORTABLE_SEH) && defined(PLATFORM_UNIX)

    //
    // Frames normally should never have destructors, since (for the most part)
    // they're not realy C++ objects. The unwinding of the frame chain when
    // an exception occurs is done by the COM+ frame handler during the EH second
    // pass.
    //
    // When using PAL_PORTABLE_SEH, though, the COM+ frame handler is never called
    // during the second pass (which really is just a C++ throw that unwinds the
    // stack). PAL_CPP_EHUNWIND_END attempts to handle these situations by either
    // manually setting the current frame pointer, or unwinding the frame chain
    // before performing a longjmp. This mechanism, though, leaves a hole when
    // a managed exception traverses unmanaged code frames that contain explicitly
    // constructed frames -- when the unwind handler is hit the stack frames containing
    // an explicitly constructed frame will already have been unwound, but the thread's
    // frame pointer will still point to them. If a GC occurs during this time a crash
    // will occur when the collector attempts to walk the stack for this thread.
    //
    // (N.B. PAL_PORTABLE_SEH on Windows doesn't have this problem as the second
    // pass is performed through RtlUnwind instead of a C++ throw, so the COM+
    // frame handler will get called.)
    //
    // Since explicitly constructed frames are embedded w/in the FrameWithCookie
    // class (as opposed to the normal stub-constructed frame) we can insert a
    // destructor here to ensure that the frame chain is properly updated during
    // the C++ unwind.
    //
    
    ~FrameWithCookie()
    {
        if (GetThread() && GetThread()->GetFrame() == &m_frame)
        {
            GCX_COOP();
            Pop();
        }
    }
#endif
};


// The frame doesn't represent a transition of any sort, it's simply placed on the stack to represent an assembly that will be found
// and checked by stackwalking security demands. This can be used in scenarios where an assembly is implicitly controlling a
// security sensitive operation without being explicitly represented on the stack. For example, an assembly decorating one of its
// classes or methods with a custom attribute can implicitly cause the ctor or property setters for that attribute to be executed by
// a third party if they happen to browse the attributes on the assembly.
// Note: This frame is pushed from managed code, so be sure to keep the layout synchronized with that in
// bcl\system\reflection\customattribute.cs.
class SecurityContextFrame : public Frame
{
    VPTR_VTABLE_CLASS(SecurityContextFrame, Frame)

    Assembly *m_pAssembly;

public:

    static TADDR GetMethodFrameVPtr()
    {
        LEAF_CONTRACT;
        RETURNFRAMEVPTR(SecurityContextFrame);
    }

    virtual Assembly *GetAssembly() { LEAF_CONTRACT; return m_pAssembly; }

    void SetAssembly(Assembly *pAssembly) { LEAF_CONTRACT; m_pAssembly = pAssembly; }

    // Keep as last entry in class
    DEFINE_VTABLE_GETTER_AND_CTOR(SecurityContextFrame)
};


//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

#ifndef DACCESS_COMPILE

#define GCPROTECT_BEGIN(ObjRefStruct)           do {                    \
                FrameWithCookie<GCFrame> __gcframe((OBJECTREF*)&(ObjRefStruct),  \
                        sizeof(ObjRefStruct)/sizeof(OBJECTREF),         \
                        FALSE);                                         \
                /* work around unreachable code warning */              \
                if (true) { DEBUG_ASSURE_NO_RETURN_BEGIN

#define GCPROTECT_ARRAY_BEGIN(ObjRefArray,cnt) do {                     \
                FrameWithCookie<GCFrame> __gcframe((OBJECTREF*)&(ObjRefArray),   \
                        cnt * sizeof(ObjRefArray) / sizeof(OBJECTREF),  \
                        FALSE);                                         \
                /* work around unreachable code warning */              \
                if (true) { DEBUG_ASSURE_NO_RETURN_BEGIN

#define GCPROTECT_BEGININTERIOR(ObjRefStruct)           do {            \
                FrameWithCookie<GCFrame> __gcframe((OBJECTREF*)&(ObjRefStruct),  \
                        sizeof(ObjRefStruct)/sizeof(OBJECTREF),         \
                        TRUE);                                          \
                /* work around unreachable code warning */              \
                if (true) { DEBUG_ASSURE_NO_RETURN_BEGIN

#define GCPROTECT_BEGININTERIOR_ARRAY(ObjRefArray,cnt) do {             \
                FrameWithCookie<GCFrame> __gcframe((OBJECTREF*)&(ObjRefArray),   \
                        cnt,                                            \
                        TRUE);                                          \
                /* work around unreachable code warning */              \
                if (true) { DEBUG_ASSURE_NO_RETURN_BEGIN


#define GCPROTECT_END()                                                 \
                DEBUG_ASSURE_NO_RETURN_END; }                           \
                __gcframe.Pop(); } while(0)


#else // #ifndef DACCESS_COMPILE

#define GCPROTECT_BEGIN(ObjRefStruct)
#define GCPROTECT_ARRAY_BEGIN(ObjRefArray,cnt)
#define GCPROTECT_BEGININTERIOR(ObjRefStruct)
#define GCPROTECT_END()

#endif // #ifndef DACCESS_COMPILE


#define ASSERT_ADDRESS_IN_STACK(address)

#if defined (_DEBUG) && !defined (DACCESS_COMPILE)
#define ASSUME_BYREF_FROM_JIT_STACK_BEGIN(__objRef)                                      \
                /* make sure we are only called inside an FCall */                       \
                if (__me == 0) {};                                                       \
                /* make sure the address is in stack. If the address is an interior */   \
                /*pointer points to GC heap, the FCall still needs to protect it explicitly */             \
                ASSERT_ADDRESS_IN_STACK (__objRef);                                      \
                do {                                                                     \
                FrameWithCookie<AssumeByrefFromJITStack> __dummyAssumeByrefFromJITStack ((__objRef));       \
                __dummyAssumeByrefFromJITStack.Push ();                       \
                /* work around unreachable code warning */                               \
                if (true) { DEBUG_ASSURE_NO_RETURN_BEGIN

#define ASSUME_BYREF_FROM_JIT_STACK_END()                                          \
                DEBUG_ASSURE_NO_RETURN_END; }                                            \
                __dummyAssumeByrefFromJITStack.Pop(); } while(0)
#else //defined (_DEBUG) && !defined (DACCESS_COMPILE)
#define ASSUME_BYREF_FROM_JIT_STACK_BEGIN(__objRef)
#define ASSUME_BYREF_FROM_JIT_STACK_END()    
#endif //defined (_DEBUG) && !defined (DACCESS_COMPILE)

#ifndef DACCESS_COMPILE
// this struct keeps track of a value type instance sitting on the stack
// because it is on the stack the instance does not have any type info so a TypeHandle is stored as well
// This two pieces of info are used to determine what references to gc objects are on the stack that need to be protected.
// Currently an array of StackStructData is allocated on the stack to keep track of every value type that is passed in the arg list
// to a MethodInfo.Invoke. However the struct is needed iff the value type argument is copied on the stack and passed ByRef 
struct StackStructData {
    StackStructData (void * location = NULL, MethodTable * mt = NULL)
        : pLocation (location), pValueType (mt)
    {}
    
    void *pLocation;
    MethodTable *pValueType;
};

// function that loops throguh the struct on the stack to determine live gc pointers
VOID ReportPointersFromStruct(promote_func *fn, ScanContext *sc, void *data);

#endif //DACCESS_COMPILE
//------------------------------------------------------------------------
#endif  //__frames_h__
