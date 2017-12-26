/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/common.h

Abstract:
    Header file for common helper functions in the map module.

--*/

#ifndef __COMMON_H_
#define __COMMON_H_

/*****
 *
 * W32toUnixAccessControl( DWORD ) - Maps Win32 to Unix memory access controls .
 *
 */
INT W32toUnixAccessControl( IN DWORD flProtect );

#endif /* __COMMON_H_ */




