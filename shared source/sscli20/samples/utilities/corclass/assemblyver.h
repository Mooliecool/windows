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

/**
 * Version strings for corclass.
 * 
 */

//
// Insert just the #defines in winver.h, so that the
// C# compiler can include this file after macro preprocessing.
//

#ifdef __cplusplus
#pragma once
#else
#define RC_INVOKED 1
#endif

#include <winver.h>

//
// Include the definitions for rmj, rmm, rup, rpt
//

#include <product_version.h>

/*
 * Product version and name.
 */

#define VER_PRODUCTNAME_STR      "Microsoft (R) .NET corclass sample"


/*
 * File version, names, description.
 */

// COMPONENT_VER_INTERNALNAME_STR is passed in by the build environment.
#ifndef COMPONENT_VER_INTERNALNAME_STR
#define COMPONENT_VER_INTERNALNAME_STR     UNKNOWN_FILE
#endif

#define VER_INTERNALNAME_STR        QUOTE_MACRO(COMPONENT_VER_INTERNALNAME_STR)
#define VER_ORIGINALFILENAME_STR    QUOTE_MACRO(COMPONENT_VER_INTERNALNAME_STR)

#define VER_FILEDESCRIPTION_STR     "Microsoft (R) .NET corclass sample"

//URTVFT passed in by the build environment.
#ifndef URTVFT
#define URTVFT VFT_UNKNOWN
#endif

#define VER_FILETYPE                URTVFT
#define VER_FILESUBTYPE             VFT2_UNKNOWN

/* default is nodebug */
#if DBG
#define VER_DEBUG                   VS_FF_DEBUG
#else
#define VER_DEBUG                   0
#endif


#define VER_PRERELEASE              0

#define VER_PRIVATE                 VS_FF_PRIVATEBUILD


#define VER_FILEFLAGSMASK           VS_FFI_FILEFLAGSMASK
#define VER_FILEFLAGS               (VER_PRERELEASE|VER_DEBUG|VER_PRIVATE)
#define VER_FILEOS                  VOS__WINDOWS32

#define VER_COMPANYNAME_STR         "Microsoft Corporation"

#ifndef VER_LEGALCOPYRIGHT_YEARS
#define VER_LEGALCOPYRIGHT_YEARS    "1998-2002"
#endif

#ifndef VER_LEGALCOPYRIGHT_STR
#if CSC_INVOKED
#define VER_LEGALCOPYRIGHT_STR      "Copyright (C) Microsoft Corporation " + VER_LEGALCOPYRIGHT_YEARS + ". All rights reserved."
#else
#define VER_LEGALCOPYRIGHT_STR      "Copyright (C) Microsoft Corporation " VER_LEGALCOPYRIGHT_YEARS ". All rights reserved."
#endif
#endif

#ifndef VER_LEGALTRADEMARKS_STR
#define VER_LEGALTRADEMARKS_STR     "Microsoft and Windows are either registered trademarks or trademarks of Microsoft Corporation in the U.S. and/or other countries."
#endif


#define EXPORT_TAG


#ifdef VER_LANGNEUTRAL
#define VER_VERSION_UNICODE_LANG  "000004B0" /* LANG_NEUTRAL/SUBLANG_NEUTRAL, Unicode CP */
#define VER_VERSION_ANSI_LANG     "000004E4" /* LANG_NEUTRAL/SUBLANG_NEUTRAL, Ansi CP */
#define VER_VERSION_TRANSLATION   0x0000, 0x04B0
#else
#define VER_VERSION_UNICODE_LANG  "040904B0" /* LANG_ENGLISH/SUBLANG_ENGLISH_US, Unicode CP */
#define VER_VERSION_ANSI_LANG     "040904E4" /* LANG_ENGLISH/SUBLANG_ENGLISH_US, Ansi CP */
#define VER_VERSION_TRANSLATION   0x0409, 0x04B0
#endif

#if CSC_INVOKED
#define VER_COMMENTS_STR        "Build environement is " + QUOTE_MACRO(URTBLDENV_FRIENDLY) 
#else
#define VER_COMMENTS_STR        "Build environement is " QUOTE_MACRO(URTBLDENV_FRIENDLY) VALUE)
#endif

