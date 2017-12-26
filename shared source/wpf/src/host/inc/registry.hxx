//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Shared definitions needed by unmanaged hosting code
//
//  History:
//     2002/06/19-murrayw
//          Created
//     2005/02/22-[....]
//          Moved, shared, simplified.
//     2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once 

//******************************************************************************
//
// Defines
//
//******************************************************************************

#define CONTENT_TYPE_XPSDOCUMENT            L"application/vnd.ms-xpsdocument"
#define CONTENT_TYPE_APPLICATION            L"application/x-ms-xbap"
#define CONTENT_TYPE_MARKUP                 L"application/xaml+xml"
#define CONTENT_TYPE_ZIP                    L"application/x-zip-compressed"

#define FILE_EXT_XPSDOCUMENT                L".xps"
#define FILE_EXT_APPLICATION                L".xbap"
#define FILE_EXT_MARKUP                     L".xaml"

//******************************************************************************
//
// Registry Key definitions
//
// Keep in [....] with:
//      setup\MSI\installerFiles\installDatabase\Registry.txt
//      Shared\MS\Internal\Registry.cs
//******************************************************************************

#define RegKey_DotNetFramework            L"SOFTWARE\\Microsoft\\.NETFramework"
    #define RegKey_WPF                        RegKey_DotNetFramework L"\\Windows Presentation Foundation"
    // Before v4, this key was .NETFramework\v3.0\Windows Presentation Foundation\Namespaces.
    #define RegKey_WPF_Namespaces         RegKey_WPF L"\\Namespaces"


#define RegKey_WPF_Hosting                RegKey_WPF L"\\Hosting"
    #define RegValue_RunUnrestricted                    L"RunUnrestricted"
    #define RegValue_NoHostTimeoutSeconds               L"NoHostTimeoutSeconds"
    #define RegValue_DisableSingleVersionOptimization   L"DisableSingleVersionOptimization"
//  #define RegValue_DisableXbapErrorPage -- defined and used in managed code
    #define RegValue_AutoShowXbapErrorDetails           L"AutoShowXbapErrorDetails"

#define RegKey_WPF_HostVersions           RegKey_WPF_Hosting L"\\Hosts"

#define RegKey_WPF_Features               RegKey_WPF L"\\Features"
    #define RegValue_XBAPDisallow                       L"XBAPDisallow"
    #define RegValue_XPSDocumentsDisallow               L"XPSDocumentsDisallow"
    #define RegValue_LooseXamlDisallow                  L"LooseXamlDisallow"
    // In Registry.cs: MediaImageDisallow, MediaVideoDisallow, MediaAudioDisallow, WebBrowserDisallow, ScriptInteropDisallow
