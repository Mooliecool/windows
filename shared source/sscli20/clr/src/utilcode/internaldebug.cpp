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
// InternalDebug.cpp
//
// This is internal code for debug mode which will turn on memory dump checking
// and other settings.  Call the api's according to:
//      _DbgInit        On startup to init the system.
//      _DbgRecord      Call this when you are sure you want dump checking.
//      _DbgUninit      Call at process shutdown to force the dump.
//
// The reason not to enable dumping under all circumstance is one might want
// to pre-empt the dump when you hit Ctrl+C or otherwise terminate the process.
// This is actually pretty common while unit testing code, and seeing a dump
// when you obviously did not free resources is annoying.
//
//*****************************************************************************
#include "stdafx.h"                     // Standard header.
#include "utilcode.h"

#ifdef _DEBUG



//********** Globals. *********************************************************
int             g_bDumpMemoryLeaks = false; // Set to true to get a dump.


//********** Code. ************************************************************

void _DbgInit(HINSTANCE hInstance)
{
    LEAF_CONTRACT;
}


void _DbgRecord()
{
    LEAF_CONTRACT;

    g_bDumpMemoryLeaks = true;
}

// By default, don't report these leaks
BOOL    g_fReportLeaks=FALSE;

void _DbgUninit()
{
    LEAF_CONTRACT;
}

#endif // _DEBUG

