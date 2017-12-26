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
// File: frameinfo.h
//
// Debugger stack walker
//
//*****************************************************************************

#ifndef FRAMEINFO_H_
#define FRAMEINFO_H_

/* ========================================================================= */

/* ------------------------------------------------------------------------- *
 * Classes
 * ------------------------------------------------------------------------- */

enum
{
    // Add an extra interception reason
    INTERCEPTION_THREAD_START = Frame::INTERCEPTION_COUNT
};


class DebuggerJitInfo;

// struct FrameInfo:  Contains the information that will be handed to
// DebuggerStackCallback functions (along with their own, individual
// pData pointers).
//
// Frame *frame:  The current frame.  NULL implies that
//      the frame is frameless, meaning either unmanaged or managed.  This
//      is set to be FRAME_TOP (0xFFffFFff) if the frame is the topmost, EE
//      placed frame.
//
// MethodDesc *md:  MetdhodDesc for the method that's
//      executing in this frame.  Will be NULL if there is no MethodDesc
//      If we're in generic code this may be a representative (i.e. canonical)
//      MD, and extra information is available in the exactGenericArgsToken.
//      For internal frames, this may point to the method the frame refers to
//      (i.e. the method being jitted, or the interface method being called through
//      COM interop), however it must always point to a method within the same
//      domain of the frame.  Therefore, it is not used to point to the target of
//      FuncEval frames since the target may be in a different domain.
// 
// void *fp:  frame pointer.  Actually filled in from
//      caller (parent) frame, so the DebuggerStackWalkProc must delay
//      the user callback for one frame.
struct FrameInfo
{
public:
    Frame               *frame; 
    MethodDesc          *md; 
    REGDISPLAY           registers;
    FramePointer         fp; 
    bool                 quickUnwind;
    bool                 internal;
    bool                 managed; 
    Context             *context;

    ULONG                relOffset; 

    // The ambient stackpointer. This can be use to compute esp-relative local variables,
    // which can be common in frameless methods.
    TADDR                ambientSP;
    
    IJitManager         *pIJM;
    METHODTOKEN          MethodToken;

    // This represents the current domain of the frame itself, and which
    // the method specified by 'md' is executing in.
    AppDomain           *currentAppDomain;
    
    void                *exactGenericArgsToken;


    // In addition to a Method, a FrameInfo may also represent either a Chain or a Stub (but not both)
    CorDebugChainReason  chainReason; 
    CorDebugInternalFrameType eStubFrameType;    

    // Helpers.
    void InitForM2UInternalFrame(CrawlFrame * pCF);
    void InitForU2MInternalFrame(CrawlFrame * pCF);
    bool InitForPossibleInternalCall(CrawlFrame * pCF);
    void InitForADTransition(CrawlFrame * pCF);
    void InitForDynamicMethod(CrawlFrame * pCF);
    void InitForFuncEval(CrawlFrame * pCF);
    void InitForThreadStart(bool fManaged, REGDISPLAY * pRDSrc);
    void InitForUMChain(FramePointer fpRoot, REGDISPLAY * pRDSrc);
    void InitForEnterManagedChain(FramePointer fpRoot);

    // Does this FrameInfo represent a method frame? (aka a frameless frame)
    // This may be combined w/ both StubFrames and ChainMarkers.
    bool HasMethodFrame() { return md != NULL && !internal; }

    // Is this frame for a stub?
    // This is mutually exclusive w/ Chain Markers.
    // StubFrames may also have a method frame as a "hint". Ex, a stub frame for a 
    // M2U transition may have the Method for the Managed Wrapper for the unmanaged call.
    bool HasStubFrame() { return eStubFrameType != STUBFRAME_NONE; }

    // Does this FrameInfo mark the start of a new chain? (A Frame info may both
    // start a chain and represent a method)
    bool HasChainMarker() { return chainReason != CHAIN_NONE; }

    DebuggerJitInfo * GetJitInfoFromFrame();
    DebuggerMethodInfo * GetMethodInfoFromFrameOrThrow();

    // Debug helper which nops in retail; and asserts invariants in debug.
#ifdef _DEBUG
    void AssertValid();

    // Debug helpers to get name of frame. Useful in asserts + log statements.
    LPCUTF8 DbgGetClassName();
    LPCUTF8 DbgGetMethodName();
    
#endif    

    // Internal helpers
protected:
    void InitForScratchFrameInfo();
    void InitFromStubHelper(CrawlFrame * pCF, MethodDesc * pMDHint, CorDebugInternalFrameType type);

};

//StackWalkAction (*DebuggerStackCallback):  This callback will
// be invoked by DebuggerWalkStackProc at each frame, passing the FrameInfo
// and callback-defined pData to the method.  The callback then returns a
// SWA - if SWA_ABORT is returned then the walk stops immediately.  If
// SWA_CONTINUE is called, then the frame is walked & the next higher frame
// will be used.  If the current frame is at the top of the stack, then
// in the next iteration, DSC will be invoked with frame->frame == FRAME_TOP
typedef StackWalkAction (*DebuggerStackCallback)(FrameInfo *frame, void *pData);

//StackWalkAction DebuggerWalkStack():  Sets up everything for a
// stack walk for the debugger, starts the stack walk (via
// g_pEEInterface->StackWalkFramesEx), then massages the output.  Note that it
// takes a DebuggerStackCallback as an argument, but at each frame
// DebuggerWalkStackProc gets called, which in turn calls the
// DebuggerStackCallback.
// Thread * thread:  Thread
// void *targetFP:  If you're looking for a specific frame, then
//  this should be set to the fp for that frame, and the callback won't
//  be called until that frame is reached.  Otherwise, set it to NULL &
//  the callback will be called on every frame.
// CONTEXT *context:  Never NULL, b/c the callbacks require the
//  CONTEXT as a place to store some information.  Either it points to an
//  uninitialized CONTEXT (contextValid should be false), or
//  a pointer to a valid CONTEXT for the thread.  If it's NULL, InitRegDisplay
//  will fill it in for us, so we shouldn't go out of our way to set this up.
// bool contextValid:  TRUE if context points to a valid CONTEXT, FALSE
//  otherwise.
// DebuggerStackCallback pCallback:  User supplied callback to
//  be invoked at every frame that's at targetFP or higher.
// void *pData:   User supplied data that we shuffle around,
//  and then hand to pCallback.

StackWalkAction DebuggerWalkStack(Thread *thread, 
                                  FramePointer targetFP,
                                  CONTEXT *pContext, 
                                  BOOL contextValid,
                                  DebuggerStackCallback pCallback,
                                  void *pData, 
                                  BOOL fIgnoreNonmethodFrames,
                                  IpcTarget iWhich = IPC_TARGET_OUTOFPROC);

#endif // FRAMEINFO_H_
