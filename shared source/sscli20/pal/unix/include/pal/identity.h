/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/identity.h

Abstract:

    Header file for identity functions.

--*/

#ifndef _PAL_IDENTITY_H_
#define _PAL_IDENTITY_H_

#include "config.h"
#include "pal/palinternal.h"

/*++

Function:
  IdentityInitialize

--*/
BOOL IdentityInitialize();

/*++
Function:
  IdentityCleanup

--*/
VOID IdentityCleanup();

#endif /* _PAL_IDENTITY_H_ */
