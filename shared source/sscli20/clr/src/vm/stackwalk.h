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
/* This is a poor man's implementation of virtual methods. */
/* The purpose of pCrawlFrame is to abstract (at least for the most common cases
   from the fact that not all methods are "framed" (basically all methods in
   "native" code are "unframed"). That way the job for the enumerator callbacks
   becomes much simpler (i.e. more transparent and hopefully less error prone).
   Two call-backs still need to distinguish between the two types: GC and exception.
   Both of these call-backs need to do really different things; for frameless methods
   they need to go through the codemanager and use the resp. apis.

   The reason for not implementing virtual methods on crawlFrame is solely because of
   the way exception handling is implemented (it does a "long jump" and bypasses
   the enumerator (stackWalker) when it finds a matching frame. By doing so couldn't
   properly destruct the dynamically created instance of CrawlFrame.
*/

#ifndef __stackwalk_h__
#define __stackwalk_h__

#include "eetwain.h"
#include "stackwalktypes.h"

class Frame;
class CrawlFrame;
class ICodeManager;
class IJitManager;
struct EE_ILEXCEPTION;
class AppDomain;

// This define controls handling of faults in managed code.  If it is defined,
//  the exception is handled (retried, actually), with a FaultingExceptionFrame
//  on the stack.  The FEF is used for unwinding.  If not defined, the unwinding
//  uses the exception context.
#define USE_FEF // to mark where code needs to be changed to eliminate the FEF
 #if defined(ELIMINATE_FEF)
  #undef ELIMINATE_FEF
 #endif 

//************************************************************************
// Enumerate all functions.
//************************************************************************

/* This enumerator is meant to be used for the most common cases, i.e. to
   enumerate just all the functions of the requested thread. It is just a
   cover for the "real" enumerator.
 */

StackWalkAction StackWalkFunctions(Thread * thread, PSTACKWALKFRAMESCALLBACK pCallback, VOID * pData);

/*                                                
#define StackWalkFunctions(thread, callBack, userdata) thread->StackWalkFrames(METHODSONLY, (callBack),(userData))
*/


class CrawlFrame
{
public:

    //************************************************************************
    // Functions available for the callbacks (using the current pCrawlFrame)
    //************************************************************************

    /* Widely used/benign functions */

    /* Is this a function? */
    /* Returns either a MethodDesc* or NULL for "non-function" frames */

    inline MethodDesc *GetFunction()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return pFunc;
    }

    Assembly *GetAssembly();

    MetaSig::RETURNTYPE ReturnsObject();

    /* Returns either a Frame * (for "framed items) or
       Returns NULL for frameless functions
     */
    inline Frame* GetFrame()       // will return NULL for "frameless methods"
    {
        LEAF_CONTRACT;

        if (isFrameless)
            return NULL;
        else
            return pFrame;
    }


    /* Returns address of the securityobject stored in the current function (method?)
       Returns NULL if
            - not a function OR
            - function (method?) hasn't reserved any room for it
              (which is an error)
     */
    OBJECTREF * GetAddrOfSecurityObject();

    // Fetch the extra type argument passed in some cases
    void *GetParamTypeArg();

    /* Returns 'this' for current method
       Returns NULL if
            - not a non-static method
            - 'this' not available (usually codegen problem)
     */
    OBJECTREF GetObject();

#ifdef _X86_
    /*
        Returns ambient Stack pointer for this crawlframe. 
        Must be a frameless method.
        Returns NULL if not available (includes prolog + epilog).
        This is safe to call on all methods, but it may return 
        garbage if the method does not have an ambient SP (eg, ebp-based methods). 
        x86 is the only platform using ambient SP.        
    */
    TADDR GetAmbientSPFromCrawlFrame();
#endif

    void GetExactGenericInstantiations(TypeHandle **pClassInst, 
                                       TypeHandle **pMethodInst);

    /* Returns extra information required to reconstruct exact generic parameters,
       if any. 
       Returns NULL if
            - no extra information is required (i.e. the code is non-shared, which 
              you can tell from the MethodDesc)
            - the extra information is not available (i.e. optimized away or codegen problem)
       Returns a MethodTable if the pMD returned by GetFunction satisfies RequiresInstMethodTableArg,
       and returns a MethodDesc if the pMD returned by GetFunction satisfies RequiresInstMethodDescArg.
       These together carry the exact instantiation information.
     */
    void *GetExactGenericArgsToken();

    inline CodeManState * GetCodeManState() { return & codeManState; }
    /*
       IF YOU USE ANY OF THE SUBSEEQUENT FUNCTIONS, YOU NEED TO REALLY UNDERSTAND THE
       STACK-WALKER (INCLUDING UNWINDING OF METHODS IN MANAGED NATIVE CODE)!
       YOU ALSO NEED TO UNDERSTAND THE THESE FUNCTIONS MIGHT CHANGE ON A AS-NEED BASIS.
     */

    /* The rest are meant to be used only by the exception catcher and the GC call-back  */

    /* Is currently a frame available? */
    /* conceptually returns (GetFrame(pCrawlFrame) == NULL)
     */
    inline bool IsFrameless()
    {
        LEAF_CONTRACT;

        return isFrameless;
    }


    /* Is it the current active (top-most) frame 
     */
    inline bool IsActiveFrame()
    {
        LEAF_CONTRACT;

        return isFirst;
    }

    /* Is it the current active function (top-most frame)
       asserts for non-functions, should be used for managed native code only
     */
    inline bool IsActiveFunc()
    {
        LEAF_CONTRACT;

        return (pFunc && isFirst);
    }

    /* Is it the current active function (top-most frame)
       which faulted or threw an exception ?
       asserts for non-functions, should be used for managed native code only
     */
    bool IsInterrupted()
    {
        LEAF_CONTRACT;

        return (pFunc && isInterrupted /* && isFrameless?? */);
    }

    /* Is it the current active function (top-most frame) which faulted ?
       asserts for non-functions, should be used for managed native code only
     */
    bool HasFaulted()
    {
        LEAF_CONTRACT;

        return (pFunc && hasFaulted /* && isFrameless?? */);
    }

    /* Is this CrawlFrame just marking that we're in native code?
       Such frames are only provided when the stackwalk is inited w/ NOTIFY_ON_U2M_TRANSITIONS.
       The only use of these crawlframes is to get the Regdisplay.
     */
    bool IsNativeMarker()
    {
        LEAF_CONTRACT;
        return isNativeMarker;
    }

    /* Has the IP been adjusted to a point where it is safe to do GC ?
       (for OutOfLineThrownExceptionFrame)
       asserts for non-functions, should be used for managed native code only
     */
    bool IsIPadjusted()
    {
        LEAF_CONTRACT;

        return (pFunc && isIPadjusted /* && isFrameless?? */);
    }

    /* Gets the ICodeMangerFlags for the current frame */

    unsigned GetCodeManagerFlags()
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
        } CONTRACTL_END;

        unsigned flags = 0;

        if (IsActiveFunc())
            flags |= ActiveStackFrame;

        if (IsInterrupted())
        {
            flags |= ExecutionAborted;

            if (!HasFaulted() && !IsIPadjusted())
            {
                _ASSERTE(!(flags & ActiveStackFrame));
                flags |= AbortingCall;
            }
        }

        return flags;
    }

    AppDomain *GetAppDomain()
    {
        LEAF_CONTRACT;

        return pAppDomain;
    }

    /* Is this frame at a safe spot for GC?
     */
    bool IsGcSafe();


    PREGDISPLAY GetRegisterSet()
    {
        LEAF_CONTRACT;

        // We would like to make the following assertion, but it is legitimately
        // violated when we perform a crawl to find the return address for a hijack.
        // _ASSERTE(isFrameless);
        return pRD;
    }

    LPVOID GetInfoBlock();

    METHODTOKEN GetMethodToken()
    {
        LEAF_CONTRACT;
        _ASSERTE(isFrameless);
        return methodToken;
    }    

    unsigned GetRelOffset()
    {
        LEAF_CONTRACT;
        _ASSERTE(isFrameless);
        return relOffset;
    }

    IJitManager*  GetJitManager()
    {
        LEAF_CONTRACT;
        _ASSERTE(isFrameless);
        return JitManagerInstance;
    }

    /* not yet used, maybe in exception catcher call-back ? */

    unsigned GetOffsetInFunction();


    /* Returns codeManager that is responsible for crawlFrame's function in
       managed native code,
       Returns NULL in all other cases (asserts for "frames")
     */

    ICodeManager* CrawlFrame::GetCodeManager()
    {
        LEAF_CONTRACT;
        _ASSERTE(isFrameless);
        return codeMgrInstance;
    }

    inline StackwalkCacheEntry* GetStackwalkCacheEntry()
    {
        _ASSERTE (isCachedMethod != stackWalkCache.IsEmpty());
        if (isCachedMethod && stackWalkCache.m_CacheEntry.IsSafeToUseCache())
        {
            return &(stackWalkCache.m_CacheEntry);
        }
        else
        {
            return NULL;
        }
    }

    inline void ClearStackwalkCacheEntry()
    {
        _ASSERTE (isCachedMethod != stackWalkCache.IsEmpty());
        isCachedMethod = FALSE;
        stackWalkCache.ClearEntry();
    }

    void CheckGSCookies() DAC_EMPTY();


protected:
    // CrawlFrames are temporarily created by the enumerator.
    // Do not create one from C++. This protected constructor polices this rule.
    CrawlFrame();

    void SetCurGSCookie(GSCookie * pGSCookie) DAC_EMPTY();

private:

    friend class Thread;
    friend class EECodeManager;
    friend class StackFrameIterator;

    CodeManState      codeManState;

    bool              isFrameless;
    bool              isFirst;
    bool              isInterrupted;
    bool              hasFaulted;
    bool              isIPadjusted;
    bool              isNativeMarker;
    bool              isProfilerDoStackSnapshot;
    Frame            *pFrame;
    MethodDesc       *pFunc;
    // the rest is only used for "frameless methods"
    ICodeManager     *codeMgrInstance;
    AppDomain        *pAppDomain;
    PREGDISPLAY       pRD; // "thread context"/"virtual register set"
    METHODTOKEN       methodToken;
    unsigned          relOffset;
    EE_ILEXCEPTION   *methodEHInfo;
    IJitManager      *JitManagerInstance;
    Thread*           pThread;

    // fields used for stackwalk cache
    OBJECTREF         *pSecurityObject;
    BOOL              isCachedMethod;
    StackwalkCache    stackWalkCache;

    GSCookie         *pCurGSCookie;
    GSCookie         *pFirstGSCookie;

    friend class Frame; // added to allow 'friend void CrawlFrame::GotoNextFrame();' declaration in class Frame, frames.h
    void GotoNextFrame();
};

void GcEnumObject(LPVOID pData, OBJECTREF *pObj);
StackWalkAction GcStackCrawlCallBack(CrawlFrame* pCF, VOID* pData);

#if defined(ELIMINATE_FEF)
//******************************************************************************
// This class is used to help use exception context records to resync a 
//  stackwalk, when managed code has generated an exception (eg, AV, zerodiv.,,)
// Such an exception causes a transition from the managed code into unmanaged
//  OS and runtime code, but without the benefit of any Frame.  This code helps
//  the stackwalker simulate the effect that such a frame would have.
// In particular, this class has methods to walk the chain of ExInfos, looking
//  for records with pContext pointers with certain characteristics.  The 
//  characteristics that are important are the location in the stack (ie, is a
//  given pContext relevant at a particular point in the stack walk), and 
//  whether the pContext was generated in managed code.
//******************************************************************************
class ExInfoWalker
{
public:
    ExInfoWalker() : m_pExInfo(0) {}
    void Init (ExInfo *pExInfo) { m_pExInfo = pExInfo; }
    // Skip one ExInfo.
    void WalkOne();
    // Attempt to find an ExInfo with a pContext that is higher (older) than
    //  a given minimum location.
    void WalkToPosition(void *pMinimum, BOOL bPopFrames);
    // Attempt to find an ExInfo with a pContext that has an IP in managed code.
    void WalkToManaged();
    // Return current ExInfo's m_pContext, or NULL if no m_pExInfo.
    PTR_CONTEXT GetContext() { return m_pExInfo ? m_pExInfo->m_pContext : NULL; }
    // Useful to see if there is more on the ExInfo chain.
    ExInfo* GetExInfo() { return m_pExInfo; }
    TADDR GetSPFromContext() { return TADDR((m_pExInfo && m_pExInfo->m_pContext) ? GetSP(m_pExInfo->m_pContext) : NULL); }

    DWORD GetFault() { return m_pExInfo ? m_pExInfo->m_pExceptionRecord->ExceptionCode : 0; }

private:
    ExInfo      *m_pExInfo;
};  // class ExInfoWalker
#endif // ELIMINATE_FEF


#if defined(DACCESS_COMPILE)
// The StackFrameIterator is NOT thread-safe on WIN64!  Currently it is only used by DAC, which should be fine
// because the debugger should have suspended the runtime already.
class StackFrameIterator
{
public:
    StackFrameIterator(void);
    ~StackFrameIterator(void);

    void Init(Thread* thread,
              Frame* frame,
              PREGDISPLAY regDisp,
              ULONG32 flags);

    BOOL IsValid(void);
    void SetInvalid(void);
    StackWalkAction Next(void);

    void ResetRegDisp(PREGDISPLAY regDisp);
    
    void SetIsFirstFrame(bool isFirst)
    {
        m_crawl.isFirst = isFirst;
    }
    
    enum FrameState
    {
        SFITER_UNINITIALIZED,
        SFITER_FRAMELESS_METHOD,
        SFITER_FRAME_FUNCTION,
        SFITER_SKIPPED_FRAME_FUNCTION,
    };
    
    // Iteration state.
    FrameState m_frameState;
    CrawlFrame m_crawl;

private:
    void UpdateRegDisp(void);
    void ProcessIp(SLOT Ip);
    StackWalkAction ProcessCurrentFrame(void);
    BOOL CheckForSkippedFrames(void);

    // Initial state.  Must be preserved for restarting.
    Thread* m_thread;                       // Thread on which to walk.
    Frame*  m_initFrame;                    // Frame* passed to Init
    Frame*  m_startFrame;                   // Frame* at which to start walk, maybe != initFrame, if it initFrame==NULL.
    ULONG32 m_flags;                        // StackWalkFrames flags.

    IJitManager::ScanFlag m_scanFlag;
#if defined(ELIMINATE_FEF)
    ExInfoWalker m_exInfoWalk;
#endif // ELIMINATE_FEF



#if defined(USE_DBGHELP_TO_WALK_STACK_IN_DAC)
    bool m_fDbgHelpInit;
#endif // USE_DBGHELP_TO_WALK_STACK_IN_DAC

    bool m_fHandlingSpecialFramelessStub;
};

#endif // DACCESS_COMPILE

#endif
