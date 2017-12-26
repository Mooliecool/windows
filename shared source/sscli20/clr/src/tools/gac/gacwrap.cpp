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
//*****************************************************************************
// GacWrap.cpp
//
// This file contains wrapper functions for Win32 API's that take strings.
//
//*****************************************************************************
#include "gacwrap.h"
#include <cor.h>


//********** Globals. *********************************************************

ULONG           DBCS_MAXWID = 0;
int             g_bOnUnicodeBox = -1;   // true if on UNICODE system.


BOOL OnUnicodeSystem()
{
    if (g_bOnUnicodeBox != -1) {
        return g_bOnUnicodeBox;
    }

    if (DBCS_MAXWID == 0) {
        CPINFO  cpInfo;

        if (GetCPInfo(CP_ACP, &cpInfo))
            DBCS_MAXWID = cpInfo.MaxCharSize;
        else
            DBCS_MAXWID = 2;
    }

    g_bOnUnicodeBox = TRUE;

    return TRUE;
}

