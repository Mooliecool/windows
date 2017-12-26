//------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Defines a factory for CVersion objects
//
// History:
//      2005/06/20 - [....]
//          Created
//      2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once

/*

This is where our Avalon policy is implemented. Given a requested version
of Avalon, we need to decide which version of Avalon we are actually going
to run.

The currently implemented policy is as follows:

For applications:

    if the requested version is available, use it
    else return the latest version

For containers

    always run the latest version

Note that this is just the Avalon version; the CLR version will be exactly
specified by PresentationHostDLL.dll

*/

#include "Precompiled.hxx"
#include "..\shared\StringMap.hxx"

class CVersionFactory
{
public:
    static int GetCount();
    static CVersion* GetVersion(__in_ecount(1) LPCWSTR pwzRequestedVersion, BOOL bDefaultToLatest = TRUE);
    static CVersion* GetLatestVersion();

private:
    static HRESULT EnsureVersionList();

private:
    static CStringMap<CVersion*>*   m_pVersions;
    static CVersion*                m_pLatestVersion;
};
