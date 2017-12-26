/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    common.c

Abstract:

    Implementation of the common mapping functions.

--*/
#include "pal/palinternal.h"
#include "pal/dbgmsg.h"

#include "common.h"

#include <sys/mman.h>

SET_DEFAULT_DEBUG_CHANNEL(VIRTUAL);

/*****
 *
 * W32toUnixAccessControl( DWORD ) - Maps Win32 to Unix memory access controls .
 *
 */
INT W32toUnixAccessControl( IN DWORD flProtect )
{
    INT MemAccessControl = 0;

    switch ( flProtect & 0xff )
    {
    case PAGE_READONLY :
        MemAccessControl = PROT_READ;
        break;
    case PAGE_READWRITE :
        MemAccessControl = PROT_READ | PROT_WRITE;
        break;
    case PAGE_EXECUTE_READWRITE:
        MemAccessControl = PROT_EXEC | PROT_READ | PROT_WRITE;
        break;
    case PAGE_EXECUTE :
        MemAccessControl = PROT_EXEC;
        break;
    case PAGE_EXECUTE_READ :
        MemAccessControl = PROT_EXEC | PROT_READ;
        break;
    case PAGE_NOACCESS :
        MemAccessControl = PROT_NONE;
        break;
    
    default:
        MemAccessControl = 0;
        ERROR( "Incorrect or no protection flags specified.\n" );
        break;
    }
    return MemAccessControl;
}
