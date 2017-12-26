//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Custom Watson reporting for problems encountered during DocObject activation
//
//      All problems detected (at least initially) stem from ACCESS DENIED errors when accessing 
//      certain registry keys or file locations. We get these while normal applications don't because
//      our process runs with a restricted token: in particular, the Local Administrators SID is 
//      removed. Normally, registry keys and file locations associated with a user's profile give 
//      that user's account explicit Full Access permission. The same permission is given to the 
//      Administrators group. On some Windows XP computers, however, the ACL for the user's account
//      is lost or it shows some unresolved SID--probably result of profile/account migration and/or
//      OS upgrade. When a normal process runs, the user gets access via the Administrators group
//      membership.
//
//      The purpose of the Watson reporting is to lead users of affected computers to a KB article 
//      and possibly a patch utility to resolve the problem. Otherwise they are left with just a 
//      blank browser window. Or, in some cases, infinite relaunching of PresHost and IE happens.
//
//  History:
//      2007/07/20   [....]     Created
//      2007/09/20   [....]     Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once

#include "..\shared\WatsonReportingShared.hxx"

// Our 'generic' event registered with Watson:
// http://watson/Madlib.aspx?MadlibId=GenericEventTypeParams&EventTypeName=WPFHostActivationProblem
#define ACTIVATION_PROBLEM_WATSON_EVENT L"WPFHostActivationProblem"

// Values for P4 in the manifest report
namespace ActivationProblemIds
{
    LPCWSTR const 
        HKCUClassesAccessDenied = L"HKCU Classes Access Denied",
        AppDataFolderAccessDenied = L"AppData Folder Access Denied",
        IPersistMonikerMarshalingFailed = L"IPersistMoniker Marshaling";
};

void TriggerWatson(LPCWSTR problemId);

HRESULT CheckRegistryAccess();
HRESULT CheckAppDataFolderAccess();

