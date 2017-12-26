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
#include "common.h"
#include "object.h"
#include "excep.h"
#include "frames.h"
#include "vars.hpp"
#include "comcurrency.h"
#include "comstring.h"


FCIMPL2(void, COMCurrency::DoToDecimal, DECIMAL * result, CY c)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;		
    } CONTRACTL_END;
    
    // GC could only happen when exception is thrown, no need to protect result
    HELPER_METHOD_FRAME_BEGIN_0();
    //-[autocvtpro]-------------------------------------------------------

    _ASSERTE(result);
    HRESULT hr = VarDecFromCy(c, result);
    if (FAILED(hr))
    {
        // Didn't expect to get here.  Update code for this HR.
        _ASSERTE(S_OK == hr);
        COMPlusThrowHR(hr);
    }

    if (FAILED(DecimalCanonicalize(result)))
        COMPlusThrow(kOverflowException, L"Overflow_Currency");
    
    result->wReserved = 0;

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND
