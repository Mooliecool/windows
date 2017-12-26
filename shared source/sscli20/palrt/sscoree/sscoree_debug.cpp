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

#include "rotor_pal.h"
#include "rotor_palrt.h"
#include "syms.h"
#include "libs.h"
#include "sscoree_int.h"

#include <stdio.h>

#ifdef _DEBUG
//#define TRACE_VERIFY 1
#endif

/***************************************************************************/
/* Functions exported in the debugging/testing API                         */
/***************************************************************************/

#ifdef _DEBUG

BOOLEAN
SscoreeVerifyLibrary (ShimmedLib LibIndex)
{
    int i;
    BOOLEAN success = TRUE;
    _ASSERTE (g_Libs[LibIndex].Handle);
    _ASSERTE (LIB_INDEX_VALID (LibIndex));

    for (i = 0; i < SHIMSYM_MAX_SYMBOL; i++) {
        ShimmedLib syms_lib = FindSymbolsLib ((ShimmedSym)i);
        // syms_lib == LIB_MAX_LIB if i == a library's symbol
        if (syms_lib == LibIndex) {
#ifdef TRACE_VERIFY
            fprintf (stderr, "SscoreeVerifyLibrary: Testing %S:%s\n",
                     g_Libs[LibIndex].Name,
                     g_Syms[i].Name);
#endif
            FARPROC proc_addr = GetProcAddress (g_Libs[LibIndex].Handle,
                                              g_Syms[i].Name);
            if (!proc_addr) {
                fprintf (stderr, "Failed to load sym %s\n", g_Syms[i].Name);
                success = FALSE;
            }
            _ASSERTE (proc_addr);
        }
    }
    return success;
}

VOID
SscoreeDumpTables ()
{
    int i;
    int j;
    printf ("There are %d known libs:\n", LIB_MAX_LIB);
    for (i = 0; i < LIB_MAX_LIB; i++) {
        printf ("  Lib[%d] {name=\"%S\", handle=0x%p}\n",
                i,
                g_Libs[i].Name,
                g_Libs[i].Handle);
    }
    printf ("There are %d known functions:\n", SHIMSYM_MAX_SYMBOL);
    for (j = 0; j < SHIMSYM_MAX_SYMBOL; j++) {
        printf ("  Sym[%d] {name=\"%s\", handle=0x%p} is in lib %d\n",
                j,
                g_Syms[j].Name,
                g_Syms[j].Proc,
                FindSymbolsLib ((ShimmedSym)j));
    }
}

#endif // _DEBUG

