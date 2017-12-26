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
//*****************************************************************************

#ifndef _EEDBGINTERFACEIMPL_INL_
#define _EEDBGINTERFACEIMPL_INL_

#include "common.h"


// This class only serves as a wrapper for the debugger callbacks.
// Using this class eliminates the need to check "#ifdef DEBUGGING_SUPPORTED"
// and "CORDebuggerAttached()".
class EEToDebuggerExceptionInterfaceWrapper
{
  public:

#if defined(DEBUGGING_SUPPORTED) && !defined(DACCESS_COMPILE)
    static inline bool FirstChanceManagedException(Thread* pThread, SIZE_T currentIP, SIZE_T currentSP
                                                   IA64_ARG(SIZE_T currentBSP))
    {
        WRAPPER_CONTRACT;

        ThreadExceptionState* pExState = pThread->GetExceptionState();
        pExState->GetDebuggerState()->SetDebuggerIndicatedFramePointer((LPVOID)currentSP  IA64_ARG((LPVOID)currentBSP));

        if (CORDebuggerAttached())
        {
            // Notfiy the debugger that we are on the first pass for a managed exception.
            // Note that this callback is made for every managed frame.
            return g_pDebugInterface->FirstChanceManagedException(pThread, currentIP, currentSP
                                                                  IA64_ARG(currentBSP));
        }
        else
        {
            return false;
        }
    }

    static inline void FirstChanceManagedExceptionCatcherFound(Thread* pThread, MethodDesc* pMD, TADDR pMethodAddr, SIZE_T currentSP
                                                               IA64_ARG(SIZE_T currentBSP), EE_ILEXCEPTION_CLAUSE* pEHClause)
    {
        WRAPPER_CONTRACT;

        ThreadExceptionState* pExState = pThread->GetExceptionState();
        pExState->GetDebuggerState()->SetDebuggerIndicatedFramePointer((LPVOID)currentSP  IA64_ARG((LPVOID)currentBSP));

        if (CORDebuggerAttached())
        {
            g_pDebugInterface->FirstChanceManagedExceptionCatcherFound(pThread, pMD, pMethodAddr, (PBYTE)currentSP
                                                                       IA64_ARG((PBYTE)currentBSP), pEHClause);
        }
    }

    static inline void ManagedExceptionUnwindBegin(Thread* pThread)
    {
        WRAPPER_CONTRACT;

        if (CORDebuggerAttached())
        {
            g_pDebugInterface->ManagedExceptionUnwindBegin(pThread);
        }
    }

    static inline void ExceptionFilter(MethodDesc* pMD, TADDR pMethodAddr, SIZE_T offset, BYTE* pStack
                                       IA64_ARG(BYTE* pBStore))
    {
        WRAPPER_CONTRACT;

        if (CORDebuggerAttached())
        {
            g_pDebugInterface->ExceptionFilter(pMD, pMethodAddr, offset, pStack
                                               IA64_ARG(pBStore));
        }
    }

    static inline void ExceptionHandle(MethodDesc* pMD, TADDR pMethodAddr, SIZE_T offset, BYTE* pStack
                                       IA64_ARG(BYTE* pBStore))
    {
        WRAPPER_CONTRACT;

        if (CORDebuggerAttached())
        {
            g_pDebugInterface->ExceptionHandle(pMD, pMethodAddr, offset, pStack
                                               IA64_ARG(pBStore));
        }
    }

#else  // !defined(DEBUGGING_SUPPORTED) || defined(DACCESS_COMPILE)
    static inline bool FirstChanceManagedException(Thread* pThread, SIZE_T currentIP, SIZE_T currentSP) {LEAF_CONTRACT;}
    static inline void FirstChanceManagedExceptionCatcherFound(Thread* pThread, MethodDesc* pMD, TADDR pMethodAddr, BYTE* currentSP,
                                                               EE_ILEXCEPTION_CLAUSE* pEHClause) {LEAF_CONTRACT;}
    static inline void ManagedExceptionUnwindBegin(Thread* pThread) {LEAF_CONTRACT;}
    static inline void ExceptionFilter(MethodDesc* pMD, TADDR pMethodAddr, SIZE_T offset, BYTE* pStack
                                       IA64_ARG(BYTE* pBStore)) {LEAF_CONTRACT;}
    static inline void ExceptionHandle(MethodDesc* pMD, TADDR pMethodAddr, SIZE_T offset, BYTE* pStack
                                       IA64_ARG(BYTE* pBStore)) {LEAF_CONTRACT;}
#endif // !defined(DEBUGGING_SUPPORTED) || defined(DACCESS_COMPILE)
};


#endif // _EEDBGINTERFACEIMPL_INL_
