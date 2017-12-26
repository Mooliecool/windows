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
// FCALL.CPP -
//
//

#include "common.h"
#include "vars.hpp"
#include "fcall.h"
#include "excep.h"
#include "frames.h"
#include "gms.h"
#include "ecall.h"
#include "eeconfig.h"


LPVOID __stdcall __FCThrow(LPVOID __me, RuntimeExceptionKind reKind, UINT resID, LPCWSTR arg1, LPCWSTR arg2, LPCWSTR arg3)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_SO_TOLERANT;    // function probes before it does any work


    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_CAPTURE_DEPTH_2);

    _ASSERTE((reKind != kExecutionEngineException) ||
             !"Don't throw kExecutionEngineException from here. Go to EEPolicy directly, or throw something better.");
    
    if (resID == 0)
    {
        // If we have an string to add use NonLocalized otherwise just throw the exception.
        if (arg1)
            COMPlusThrowNonLocalized(reKind, arg1); //COMPlusThrow(reKind,arg1);
        else
            COMPlusThrow(reKind);
    }
    else 
        COMPlusThrow(reKind, resID, arg1, arg2, arg3);

    HELPER_METHOD_FRAME_END();
    _ASSERTE(!"Throw returned");
    return NULL;
}

LPVOID __stdcall __FCThrowArgument(LPVOID __me, RuntimeExceptionKind reKind, LPCWSTR argName, LPCWSTR resourceName)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_SO_TOLERANT;    // function probes before it does any work

    ENDFORBIDGC();
/*
    No contract here: causes crash with AMD64 build
    (repro: jit\Regression\CLR-x86-JIT\V1-M09\b14455\b14455.exe)

    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;
*/

    INCONTRACT(FCallCheck __fCallCheck(__FILE__, __LINE__));

    FC_GC_POLL_NOT_NEEDED();    // throws always open up for GC
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_CAPTURE_DEPTH_2);

    
    switch (reKind) {
    case kArgumentNullException:
        if (resourceName) {
            COMPlusThrowArgumentNull(argName, resourceName);
        } else {
            COMPlusThrowArgumentNull(argName);
        }
        break;

    case kArgumentOutOfRangeException:
        COMPlusThrowArgumentOutOfRange(argName, resourceName);
        break;

    case kArgumentException:
        COMPlusThrowArgumentException(argName, resourceName);
        break;

    default:
        // If you see this assert, add a case for your exception kind above.
        _ASSERTE(argName == NULL);
        COMPlusThrow(reKind, resourceName);
    }        
        
    HELPER_METHOD_FRAME_END();
    _ASSERTE(!"Throw returned");
    return NULL;
}

/**************************************************************************************/
/* erect a frame in the FCALL and then poll the GC, objToProtect will be protected
   during the poll and the updated object returned.  */

Object* FC_GCPoll(void* __me, Object* objToProtect) {

    ENDFORBIDGC();

    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;    // function probes before it does any work
    } CONTRACTL_END;

    INCONTRACT(FCallCheck __fCallCheck(__FILE__, __LINE__));

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_CAPTURE_DEPTH_2, objToProtect);

#ifdef _DEBUG
    BOOL GCOnTransition = FALSE;
    if (g_pConfig->FastGCStressLevel()) {
        GCOnTransition = GC_ON_TRANSITIONS (FALSE);
    }
#endif
    CommonTripThread();					
#ifdef _DEBUG
    if (g_pConfig->FastGCStressLevel()) {
        GC_ON_TRANSITIONS (GCOnTransition);
    }
#endif

    HELPER_METHOD_FRAME_END();
    BEGINFORBIDGC();
    return objToProtect;
}

#ifdef _DEBUG

unsigned FcallTimeHist[11];

#endif


