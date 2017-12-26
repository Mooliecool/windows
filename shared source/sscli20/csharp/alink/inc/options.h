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
// ===========================================================================
// File: options.h
//
// ===========================================================================

#ifndef OPTION
#error You must define OPTION before including this file!
#endif

// Flags:
// MetaData bit     = 0x01
// CustomAttribute  = 0x02
// AllowMulti       = 0x04
// Security         = 0x08
// Arg Is a filename= 0x10 <- this means it must be less tham MAX_PATH characters!
// Deprecated       = 0x20
// Removed          = 0x40 <- Don't use this!
// No Bits to CA    = 0x80

        // Enum option      VT type     Flags   Help_ID             Short name,         Long Name           Custom Attribute name (fully qualified)
OPTION(optAssemTitle,       VT_BSTR,    0x03,   IDS_N_TITLE,        NULL,               L"title",           L"System.Reflection.AssemblyTitleAttribute")
OPTION(optAssemDescription, VT_BSTR,    0x03,   IDS_N_DESCR,        L"descr",           L"description",     L"System.Reflection.AssemblyDescriptionAttribute")
OPTION(optAssemConfig,      VT_BSTR,    0x02,   IDS_N_CONFIG,       L"config",          L"configuration",   L"System.Reflection.AssemblyConfigurationAttribute")
OPTION(optAssemOS,          VT_BSTR,    0x45,   IDS_N_OS,           NULL,               L"os",              L"System.Reflection.AssemblyOperatingSystemAttribute")
OPTION(optAssemProcessor,   VT_UI4,     0x45,   IDS_N_PROC,         L"proc",            L"processor",       L"System.Reflection.AssemblyProcessorAttribute")
OPTION(optAssemLocale,      VT_BSTR,    0x01,   IDS_N_LOCALE,       L"c",               L"culture",         L"System.Reflection.AssemblyCultureAttribute")
OPTION(optAssemVersion,     VT_BSTR,    0x01,   IDS_N_VERSION,      L"v",               L"version",         L"System.Reflection.AssemblyVersionAttribute")
OPTION(optAssemCompany,     VT_BSTR,    0x03,   IDS_N_COMPANY,      L"comp",            L"company",         L"System.Reflection.AssemblyCompanyAttribute")
OPTION(optAssemProduct,     VT_BSTR,    0x03,   IDS_N_PRODUCT,      L"prod",            L"product",         L"System.Reflection.AssemblyProductAttribute")
OPTION(optAssemProductVersion,VT_BSTR,  0x03,   IDS_N_PRODVER,      L"productv",        L"productversion",  L"System.Reflection.AssemblyInformationalVersionAttribute")
OPTION(optAssemCopyright,   VT_BSTR,    0x03,   IDS_N_COPYRIGHT,    L"copy",            L"copyright",       L"System.Reflection.AssemblyCopyrightAttribute")
OPTION(optAssemTrademark,   VT_BSTR,    0x03,   IDS_N_TRADEMARK,    L"trade",           L"trademark",       L"System.Reflection.AssemblyTrademarkAttribute")
OPTION(optAssemKeyFile,     VT_BSTR,    0x93,   IDS_N_KEYFILE,      L"keyf",            L"keyfile",         L"System.Reflection.AssemblyKeyFileAttribute")
OPTION(optAssemKeyName,     VT_BSTR,    0x83,   IDS_N_KEYNAME,      L"keyn",            L"keyname",         L"System.Reflection.AssemblyKeyNameAttribute")
OPTION(optAssemAlgID,       VT_UI4,     0x01,   IDS_N_ALGID,        NULL,               L"algid",           L"System.Reflection.AssemblyAlgorithmIdAttribute")
OPTION(optAssemFlags,       VT_UI4,     0x01,   IDS_N_FLAGS,        NULL,               L"flags",           L"System.Reflection.AssemblyFlagsAttribute")
OPTION(optAssemHalfSign,    VT_BOOL,    0x83,   IDS_N_DELAYSIGN,    L"delay",           L"delaysign",       L"System.Reflection.AssemblyDelaySignAttribute")
OPTION(optAssemFileVersion, VT_BSTR,    0x03,   IDS_N_FILEVER,      NULL,               L"fileversion",     L"System.Reflection.AssemblyFileVersionAttribute")
OPTION(optAssemSatelliteVer,VT_BSTR,    0x03,   IDS_N_SATELLITEVER, NULL,               NULL,               L"System.Resources.SatelliteContractVersionAttribute")
#ifdef INCLUDE_PRIVATE
OPTION(optPriAssemFriendAssem,VT_BSTR,  0x07,   IDS_N_FRIENDASSEM,  NULL,               NULL,               L"System.Runtime.CompilerServices.InternalsVisibleToAttribute")
#undef INCLUDE_PRIVATE
#endif

#undef OPTION
