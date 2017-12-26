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
// ===========================================================================
// File: JITinterfacePPC.CPP
//
// ===========================================================================

// This contains JITinterface routines that are tailored for
// PPC platforms.

#include "common.h"
#include "jitinterface.h"
#include "eeconfig.h"
#include "excep.h"
#include "comstring.h"
#include "comdelegate.h"
#include "remoting.h" // create context bound and remote class instances
#include "field.h"
#include "ecall.h"
#include "asmconstants.h"

void InitJITHelpers1()
{
    LEAF_CONTRACT;
}

void StompWriteBarrierEphemeral()
{
    LEAF_CONTRACT;
}

void StompWriteBarrierResize(BOOL bReqUpperBoundsCheck)
{
    LEAF_CONTRACT;
}
