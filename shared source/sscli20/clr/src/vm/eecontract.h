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
// EEContract.h
// ---------------------------------------------------------------------------

#ifndef EECONTRACT_H_
#define EECONTRACT_H_

#include "contract.h"
#include "stackprobe.h"

// --------------------------------------------------------------------------------
// EECONTRACT is an extension of the lower level CONTRACT macros to include some
// EE specific stuff like GC mode checking.  See check.h for more info on CONTRACT.
// --------------------------------------------------------------------------------

#undef GC_TRIGGERS
#undef GC_NOTRIGGER


#define MODE_COOPERATIVE
#define MODE_PREEMPTIVE
#define MODE_ANY
#define GC_TRIGGERS
#define GC_NOTRIGGER
#define HOST_NOCALLS
#define HOST_CALLS


// Replace the CONTRACT macro with the EE version
#undef CONTRACT
#define CONTRACT(_returntype)  CUSTOM_CONTRACT(EEContract, _returntype)

#undef CONTRACT_VOID
#define CONTRACT_VOID  CUSTOM_CONTRACT_VOID(EEContract)

#undef CONTRACTL
#define CONTRACTL  CUSTOM_CONTRACTL(EEContract)

#undef LEAF_CONTRACT
#define LEAF_CONTRACT CUSTOM_LEAF_CONTRACT(EEContract)

#undef WRAPPER_CONTRACT
#define WRAPPER_CONTRACT CUSTOM_WRAPPER_CONTRACT(EEContract)

// This contract should be used for leaf methods of GC objects (Object and it's subclasses).
#undef GCOBJECT_LEAF_CONTRACT
#define GCOBJECT_LEAF_CONTRACT  \
    CONTRACTL                   \
    {                           \
        NOTHROW;                \
        GC_NOTRIGGER;           \
        MODE_COOPERATIVE;       \
    }                           \
    CONTRACTL_END;              \

#endif  // EECONTRACT_H_
