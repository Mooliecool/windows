/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    modulename.cpp

Abstract:

    Implementation of internal functions to get module names

--*/

extern "C" {

#include "config.h"
#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/modulename.h"

#if HAVE_DYLIBS
#include "dlcompat.h"
#else   // HAVE_DYLIBS
#include <dlfcn.h>
#endif  // HAVE_DYLIBS

}

SET_DEFAULT_DEBUG_CHANNEL(LOADER);


/*++
    PAL_dladdr

    Internal wrapper for dladder used only to get module name

Parameters:
    None

Return value:
    Pointer to string with the fullpath to the librotor_pal.so being
    used.

    NULL if error occured.

Notes: 
    The string returned by this function is owned by the OS.
    If you need to keep it, strdup() it, because it is unknown how long
    this ptr will point at the string you want (over the lifetime of
    the system running)  It is only safe to use it immediately after calling
    this function.
--*/
const char *PAL_dladdr(LPVOID ProcAddress)
{
    Dl_info dl_info;
    if (!dladdr(ProcAddress, &dl_info))
    {
        WARN("dladdr() call failed! dlerror says '%s'\n", dlerror());
        /* If we get an error, return NULL */
        return (NULL);
    }
    else 
    {
        /* Return the module name */ 
        return dl_info.dli_fname;
    }
}

