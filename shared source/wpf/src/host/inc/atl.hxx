//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      All #includes and #defines needed to use ATL in our projects.
//
//  History:
//     2007/06/14   [....]     Created (moved from host\proxy)
//     2007/09/20   [....]     Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once

#ifdef _DEBUG
#define _ATL_DEBUG_INTERFACES
#define _ATL_DEBUG_QI
#endif

#include <cguid.h>

#undef SubclassWindow
#define _ATL_APARTMENT_THREADED
#define ATLASSERT(x) ASSERT(x)
#pragma warning(push)
#pragma warning(disable: 4100 4995 4996)
#include <AtlBase.h>
extern CComModule _Module;
#include <AtlCom.h>
#include <AtlHost.h>
#pragma warning(pop)
#pragma warning(disable: 4505) // Warning-as-error C4505: 'ATL::CAxHostWindow::QueryInterface' : unreferenced local function has been removed

