//------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All right reserved.
//
//------------------------------------------------------------------------------
#pragma once

#define DUMMYUNIONNAME
#define DUMMYUNIONNAME2
#define DUMMYUNIONNAME3
#define DUMMYUNIONNAME4
#define DUMMYSTRUCTNAME
#define DUMMYSTRUCTNAME2
#include "evntprov.h"
#pragma warning(push)
// Disable warning 4748 in the generated code for code coverage builds.
#pragma warning(disable:4748 4061)
#include "..\Tracing\native\wpf-etw.h"
#pragma warning(pop)
#include "..\Tracing\native\wpf-etw-valuemaps.h"

#define ETW_ENABLED_CHECK(level) (MICROSOFT_WINDOWS_WPF_PROVIDER_Context.IsEnabled && level <= MICROSOFT_WINDOWS_WPF_PROVIDER_Context.Level)
