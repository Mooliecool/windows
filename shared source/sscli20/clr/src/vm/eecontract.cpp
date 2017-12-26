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

// ---------------------------------------------------------------------------
// EEContract.cpp
// ---------------------------------------------------------------------------

#include "common.h"
#include "dbginterface.h"




BYTE* __stdcall GetAddrOfContractShutoffFlag()
{

    // Exposed entrypoint where we cannot probe or do anything TLS
    // related
    static BYTE gContractShutoffFlag = 0;

    return &gContractShutoffFlag;
}

