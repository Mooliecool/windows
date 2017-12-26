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
// File: cmdoptdef.h
//
// ===========================================================================

#ifndef CMDOPTDEF
#error you must define CMDOPTDEF macro before including CMDOPTDEF.H!
#endif

////////////////////////////////////////////////////////////////////////////////
// OPTIONS
//
//
//         DESC ID          SHORT               LONG                    FLAGS

CMDOPTDEF( IDS_OD_OUT,      NULL,               L"out",                 COF_GRP_OUTPUT | COF_ARG_FILE)
CMDOPTDEF( IDS_OD_TEXE,     L"t:exe",           L"target:exe",          COF_GRP_OUTPUT)
CMDOPTDEF( IDS_OD_TWIN,     L"t:winexe",        L"target:winexe",       COF_GRP_OUTPUT)
CMDOPTDEF( IDS_OD_TDLL,     L"t:library",       L"target:library",      COF_GRP_OUTPUT)
CMDOPTDEF( IDS_OD_TMOD,     L"t:module",        L"target:module",       COF_GRP_OUTPUT)
CMDOPTDEF( IDS_OD_BASE,     NULL,               L"baseaddress",         COF_GRP_ADVANCED | COF_ARG_ADDR)
CMDOPTDEF( IDS_OD_EMBED,    L"res",             L"resource",            COF_GRP_RES | COF_ARG_RESINFO)
CMDOPTDEF( IDS_OD_LINK,     L"linkres",         L"linkresource",        COF_GRP_RES | COF_ARG_RESINFO)
CMDOPTDEF( IDS_OD_RESPONSE, NULL,               L"@",                   COF_GRP_MISC | COF_ARG_FILE | COF_ARG_NOCOLON)
CMDOPTDEF( IDS_OD_RECURSE,  NULL,               L"recurse",             COF_GRP_INPUT | COF_ARG_WILDCARD)
CMDOPTDEF( IDS_OD_HELP,     L"?",               L"help",                COF_GRP_MISC)
CMDOPTDEF( IDS_OD_NOLOGO,   NULL,               L"nologo",              COF_GRP_MISC)
CMDOPTDEF( IDS_OD_CODEPAGE, NULL,               L"codepage",            COF_GRP_ADVANCED | COF_ARG_NUMBER)
CMDOPTDEF( IDS_OD_UTF8OUT,  NULL,               L"utf8output",          COF_GRP_ADVANCED)
CMDOPTDEF( IDS_OD_MAIN,     L"m",               L"main",                COF_GRP_ADVANCED | COF_ARG_TYPE)
CMDOPTDEF( IDS_OD_FULLPATH, NULL,               L"fullpaths",           COF_GRP_ADVANCED)
CMDOPTDEF( IDS_OD_NOCONFIG, NULL,               L"noconfig",            COF_GRP_MISC)
CMDOPTDEF( IDS_OD_ALIGN,    NULL,               L"filealign",           COF_GRP_ADVANCED | COF_ARG_NUMBER)
CMDOPTDEF( IDS_OD_ALIAS,    L"r",               L"reference",           COF_GRP_INPUT | COF_ARG_ALIAS)
CMDOPTDEF( IDS_OD_PDB,      NULL,               L"pdb",                 COF_GRP_ADVANCED | COF_ARG_FILE)

#undef CMDOPTDEF
