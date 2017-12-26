//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Provide logging support for IDE compiler.
//
//-------------------------------------------------------------------------------------------------

#pragma once

#define VisualBasicIDECompilerLoggingGuid (0E229222,C408,42f4,8950,DAAF2D56C8EC)

// End-to-end scenario definition for logging purposes

#if !defined( VBDBGEE ) 
    #define VB_DECOMPILATION_LOG 1
    #define VB_THREAD_INTERACTION_LOG 2
    #define VB_COMPILATION_LOG 3
    #define VB_NAMESPACE_HASH_LOG 4
#endif // !VBDBGEE

// Include definition for logging levels and the logging macro
#include "..\Include\Logging.h"
#ifdef ENABLED_WMI_LOGGING
// Define the control guids and flags
    #define WPP_CONTROL_GUIDS \
        WPP_DEFINE_CONTROL_GUID(VisualBasicIDECompiler, VisualBasicIDECompilerLoggingGuid, \
            WPP_DEFINE_BIT(VB_DECOMPILATION)           \
            WPP_DEFINE_BIT(VB_THREAD_INTERACTION)       \
            WPP_DEFINE_BIT(VB_COMPILATION)       \
            WPP_DEFINE_BIT(VB_NAMESPACE_HASH)       \
        )
    #define WPP_DLL // Uncomment for a DLL component (
#endif ENABLED_WMI_LOGGING
