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
#ifndef __ExStateCommon_h__
#define __ExStateCommon_h__

class ExceptionFlags;

#ifdef DEBUGGING_SUPPORTED
class DebuggerExState
{
public:    

    DebuggerExState()
    {
        Init();
    }

    void Init()
    {
        m_pDebuggerInterceptFrame = EXCEPTION_CHAIN_END;
        m_dDebuggerInterceptHandlerDepth  = 0;
        m_pDebuggerInterceptStackPointer  = NULL;
        IA64_ONLY(m_pDebuggerInterceptBStorePointer = NULL);
        m_pDebuggerContext = NULL;
    }
    
    void* GetDebuggerInterceptContext() 
    { 
        LEAF_CONTRACT; 
        return m_pDebuggerContext; 
    }
    void SetDebuggerInterceptContext(void* pContext)
    {
        LEAF_CONTRACT;
        m_pDebuggerContext = pContext;
    }

    void SetDebuggerIndicatedFramePointer(void* stackPointer  IA64_ARG(void* bStorePointer))
    {
        LEAF_CONTRACT;
        m_pDebuggerIndicatedStackPointer  = stackPointer;
        IA64_ONLY(m_pDebuggerIndicatedBStorePointer = bStorePointer);
    }

    BOOL SetDebuggerInterceptInfo(IJitManager *pJitManager,
                                  Thread *pThread,
                                  METHODTOKEN methodToken,
                                  MethodDesc *pMethDesc,
                                  ULONG_PTR natOffset,
                                  void *stackPointer
                                  IA64_ARG(void *backingStorePointer),
                                  ExceptionFlags* pFlags);

    void GetDebuggerInterceptInfo(PEXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
                                  MethodDesc **ppFunc,
                                  int *pdHandler,
                                  BYTE **ppStack
                                  IA64_ARG(BYTE **ppBStore),
                                  ULONG_PTR *pNativeOffset,
                                  Frame **ppFrame)
    {
        LEAF_CONTRACT;
        if (pEstablisherFrame != NULL)
        {
            *pEstablisherFrame = m_pDebuggerInterceptFrame;
        }

        if (ppFunc != NULL)
        {
            *ppFunc = m_pDebuggerInterceptFunc;
        }

        if (pdHandler != NULL)
        {
            *pdHandler = m_dDebuggerInterceptHandlerDepth;
        }

        if (ppStack != NULL)
        {
            *ppStack = (BYTE *)m_pDebuggerInterceptStackPointer;
        }


        if (pNativeOffset != NULL)
        {
            *pNativeOffset = m_pDebuggerInterceptNativeOffset;
        }

        if (ppFrame != NULL)
        {
            *ppFrame = NULL;
        }
    }
    
private:     
    void*           m_pDebuggerIndicatedStackPointer;
    IA64_ONLY(void* m_pDebuggerIndicatedBStorePointer);
    PEXCEPTION_REGISTRATION_RECORD m_pDebuggerInterceptFrame;
    MethodDesc*     m_pDebuggerInterceptFunc;
    void*           m_pDebuggerInterceptStackPointer;
    IA64_ONLY(void* m_pDebuggerInterceptBStorePointer);
    void*           m_pDebuggerContext;
    int             m_dDebuggerInterceptHandlerDepth;
    ULONG_PTR       m_pDebuggerInterceptNativeOffset;
};
#endif // DEBUGGING_SUPPORTED

class EHClauseInfo
{
public:
    EHClauseInfo() 
    { 
        LEAF_CONTRACT; 

        // For the profiler, other clause fields are not valid if m_ClauseType is COR_PRF_CLAUSE_NONE.
        m_ClauseType          = COR_PRF_CLAUSE_NONE;
        m_IPForEHClause       = NULL;
        m_sfForEHClause.Clear();
        m_fManagedCodeEntered = FALSE;
    }

    void SetEHClauseType(COR_PRF_CLAUSE_TYPE EHClauseType)
    {
        LEAF_CONTRACT;
        m_ClauseType = EHClauseType;
    }

    void SetInfo(COR_PRF_CLAUSE_TYPE EHClauseType, 
                 UINT_PTR            uIPForEHClause, 
                 StackFrame          sfForEHClause)
    {
        LEAF_CONTRACT;

        m_ClauseType    = EHClauseType;
        m_IPForEHClause = uIPForEHClause;
        m_sfForEHClause = sfForEHClause;
    }

    void ResetInfo()
    {
        LEAF_CONTRACT;

        // For the profiler, other clause fields are not valid if m_ClauseType is COR_PRF_CLAUSE_NONE.
        m_ClauseType    = COR_PRF_CLAUSE_NONE;
        m_IPForEHClause = 0;
        m_sfForEHClause.Clear();
    }

    void SetManagedCodeEntered(BOOL fEntered)
    {
        LEAF_CONTRACT;
        m_fManagedCodeEntered = fEntered;
    }

    COR_PRF_CLAUSE_TYPE GetClauseType()     { LEAF_CONTRACT; return m_ClauseType;           }

    UINT_PTR GetIPForEHClause()             { LEAF_CONTRACT; return m_IPForEHClause;        }
    UINT_PTR GetFramePointerForEHClause()   { LEAF_CONTRACT; return m_sfForEHClause.SP;     }

    StackFrame GetStackFrameForEHClause()   { LEAF_CONTRACT; return m_sfForEHClause;        }

    BOOL     IsManagedCodeEntered()         { LEAF_CONTRACT; return m_fManagedCodeEntered;  }

private:
    UINT_PTR   m_IPForEHClause;         // the entry point of the current notified exception clause
    StackFrame m_sfForEHClause;         // the assocated frame pointer of the current notified exception clause

    COR_PRF_CLAUSE_TYPE m_ClauseType;   // this has a value from COR_PRF_CLAUSE_TYPE while an exception notification is pending
    BOOL m_fManagedCodeEntered;         // this flag indicates that we have called the managed code for the current EH clause   
};

class ExceptionFlags
{
public:
    ExceptionFlags()
    {
        Init();
    }


    void AssertIfReadOnly()
    {
    }

    void Init()
    {
        m_flags = 0;
    }
    
    BOOL IsRethrown()      { LEAF_CONTRACT; return m_flags & Ex_IsRethrown; }
    void SetIsRethrown()   { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_IsRethrown; }
    void ResetIsRethrown() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags &= ~Ex_IsRethrown; }

    BOOL UnwindHasStarted()      { LEAF_CONTRACT; return m_flags & Ex_UnwindHasStarted; }
    void SetUnwindHasStarted()   { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_UnwindHasStarted; }
    void ResetUnwindHasStarted() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags &= ~Ex_UnwindHasStarted; }

    BOOL UnwindingToFindResumeFrame()      { LEAF_CONTRACT; return m_flags & Ex_UnwindingToFindResumeFrame; }
    void SetUnwindingToFindResumeFrame()   { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_UnwindingToFindResumeFrame; }
    void ResetUnwindingToFindResumeFrame() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags &= ~Ex_UnwindingToFindResumeFrame; }

    BOOL UseExInfoForStackwalk()      { LEAF_CONTRACT; return m_flags & Ex_UseExInfoForStackwalk; }
    void SetUseExInfoForStackwalk()   { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_UseExInfoForStackwalk; }
    void ResetUseExInfoForStackwalk() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags &= ~Ex_UseExInfoForStackwalk; }

#ifdef DEBUGGING_SUPPORTED
    BOOL SentDebugUserFirstChance()    { LEAF_CONTRACT; return m_flags & Ex_SentDebugUserFirstChance; }
    void SetSentDebugUserFirstChance() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_SentDebugUserFirstChance; }

    BOOL SentDebugFirstChance()    { LEAF_CONTRACT; return m_flags & Ex_SentDebugFirstChance; }
    void SetSentDebugFirstChance() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_SentDebugFirstChance; }

    BOOL SentDebugUnwindBegin()    { LEAF_CONTRACT; return m_flags & Ex_SentDebugUnwindBegin; }
    void SetSentDebugUnwindBegin() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_SentDebugUnwindBegin; }

    BOOL DebuggerInterceptNotPossible()    { LEAF_CONTRACT; return m_flags & Ex_DebuggerInterceptNotPossible; }
    void SetDebuggerInterceptNotPossible() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_DebuggerInterceptNotPossible; }

    BOOL DebuggerInterceptInfo()    { LEAF_CONTRACT; return m_flags & Ex_DebuggerInterceptInfo; }
    void SetDebuggerInterceptInfo() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_DebuggerInterceptInfo; }
#endif

    BOOL ImpersonationTokenSet()      { LEAF_CONTRACT; return m_flags & Ex_ImpersonationTokenSet; }
    void SetImpersonationTokenSet()   { LEAF_CONTRACT; AssertIfReadOnly(); m_flags |= Ex_ImpersonationTokenSet; }
    void ResetImpersonationTokenSet() { LEAF_CONTRACT; AssertIfReadOnly(); m_flags &= ~Ex_ImpersonationTokenSet; }

private:
    enum 
    {
        Ex_IsRethrown                   = 0x00000001,
        Ex_UnwindingToFindResumeFrame   = 0x00000002,
        Ex_UnwindHasStarted             = 0x00000004,
        Ex_UseExInfoForStackwalk         = 0x00000008        // Use this ExInfo to unwind a fault (AV, zerodiv) back to managed code?

#ifdef DEBUGGING_SUPPORTED
    ,
        Ex_SentDebugUserFirstChance     = 0x00000010,
        Ex_SentDebugFirstChance         = 0x00000020,
        Ex_SentDebugUnwindBegin         = 0x00000040,
        Ex_DebuggerInterceptInfo        = 0x00000080,
        Ex_DebuggerInterceptNotPossible = 0x00000100
#endif
    ,
        Ex_ImpersonationTokenSet        = 0x00000200

    };

    UINT32 m_flags;
};


#endif // __ExStateCommon_h__
