//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Defines the main entry point of PresentationHost used for hosting
//     Windows Client Applications in the browser
//
//  History:
//     2002/06/12-murrayw
//          Created
//     2003/06/03-kusumav
//          Ported to WCP
//     2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once 

/**************************************************************************
   global variables
**************************************************************************/

extern long       g_ServerRefCount;
extern int        g_exitCode;
extern DWORD      g_dwCommandLineFlags;

// This is the handle for the mechanism that will terminate the process if
// the timeout period for starting the activation has elapsed. This timer is
// started in Main, and cancelled in OleObject::DoVerb, after the mechanism
// for monitoring the host process is started.
extern HANDLE     g_hNoHostTimer_shim;
extern LPWSTR     g_pswzDebugSecurityZoneURL;
