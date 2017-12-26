// ==++==
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
/*============================================================
**
** Implementation: SynchronizationContextNative.cpp
**
** Purpose: Native methods on System.Threading.SynchronizationContext.
**
** Date:  September 19th, 2003
**
===========================================================*/

#include "common.h"
#include "synchronizationcontextnative.h"

FCIMPL3(DWORD, SynchronizationContextNative::WaitHelper, PTRArray *handleArrayUNSAFE, CLR_BOOL waitAll, DWORD millis)
{
    DWORD ret = 0;

    PTRARRAYREF handleArrayObj = (PTRARRAYREF) handleArrayUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(handleArrayObj);
    
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CQuickArray<HANDLE> qbHandles;
    int cHandles = handleArrayObj->GetNumComponents();

    // Since DoAppropriateWait could cause a GC, we need to copy the handles to an unmanaged block
    // of memory to ensure they aren't relocated during the call to DoAppropriateWait.
    qbHandles.AllocThrows(cHandles);
    memcpy(qbHandles.Ptr(), handleArrayObj->GetDataPtr(), cHandles * sizeof(HANDLE));

    Thread * pThread = GetThread();
    ret = pThread->DoAppropriateWait(cHandles, qbHandles.Ptr(), waitAll, millis, 
                                     (WaitMode)(WaitMode_Alertable | WaitMode_IgnoreSyncCtx));
    
    HELPER_METHOD_FRAME_END();
    return ret;
}
FCIMPLEND

    
