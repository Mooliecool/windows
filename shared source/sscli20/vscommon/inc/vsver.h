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
 * Version strings for Frameworks.
 * 
 */

//
// Insert just the #defines in winver.h, so that the
// C# compiler can include this file after macro preprocessing.
//

/*
 * Product version and name.  This uses the original value of RC_INVOKED before it is #defined
 * below, so it must be above the following block of directives.
 */
#ifdef RC_INVOKED
#define VER_PRODUCTNAME_STR      L"Microsoft\256 Visual Studio\256 2005"
#else
#define VER_PRODUCTNAME_STR      "Microsoft (R) Visual Studio (R) 2005"
#endif

#ifdef __cplusplus
#ifndef VSVER_H_
#define VSVER_H_
#define INCLUDE_VSVER_H_
#endif
#else
#define RC_INVOKED 1
#define INCLUDE_VSVER_H_
#endif

#ifdef INCLUDE_VSVER_H_

#include <winver.h>

//
// Include the definitions for rmj, rmm, rup, rpt
//

#include <product_version.h>


#include "vsverstrings.h"

/*
 * File version, names, description.
 */

// FX_VER_INTERNALNAME_STR is passed in by the build environment.
#ifndef FX_VER_INTERNALNAME_STR
#define FX_VER_INTERNALNAME_STR     UNKNOWN_FILE
#endif

#define VER_INTERNALNAME_STR        QUOTE_MACRO(FX_VER_INTERNALNAME_STR)
#define VER_ORIGINALFILENAME_STR    QUOTE_MACRO(FX_VER_INTERNALNAME_STR)


//
#ifdef FX_VER_FILEDESCRIPTION_STR
#undef FX_VER_FILEDESCRIPTION_STR
#endif

#ifndef FX_VER_FILEDESCRIPTION_STR
#define FX_VER_FILEDESCRIPTION_STR  QUOTE_MACRO(FX_VER_INTERNALNAME_STR)
#endif

#ifndef VER_FILEDESCRIPTION_STR
#define VER_FILEDESCRIPTION_STR     FX_VER_FILEDESCRIPTION_STR
#endif

#ifndef FX_VER_FILEVERSION_STR
#define FX_VER_FILEVERSION_STR      VER_PRODUCTVERSION_STR
#endif

#define VER_FILEVERSION_STR         FX_VER_FILEVERSION_STR
#define VER_FILEVERSION_STR_L       VER_PRODUCTVERSION_STR_L

#ifndef FX_VER_FILEVERSION
#define FX_VER_FILEVERSION          VER_PRODUCTVERSION
#endif

#define VER_FILEVERSION             FX_VER_FILEVERSION

//URT_VFT passed in by the build environment.
#ifndef FX_VFT
#define FX_VFT VFT_UNKNOWN
#endif

#define VER_FILETYPE                FX_VFT
#define VER_FILESUBTYPE             VFT2_UNKNOWN

/* default is nodebug */
#if DBG
#define VER_DEBUG                   VS_FF_DEBUG
#else
#define VER_DEBUG                   0
#endif

// DEBUG flag is set for debug build, not set for retail build
// #if defined(DEBUG) || defined(_DEBUG)
// #define VER_DEBUG        VS_FF_DEBUG
// #else  // DEBUG
// #define VER_DEBUG        0
// #endif // DEBUG


/* default is prerelease */
#define VER_PRERELEASE              0

// PRERELEASE flag is always set unless building SHIP version
// #ifndef _SHIP
// #define VER_PRERELEASE   VS_FF_PRERELEASE
// #else
// #define VER_PRERELEASE   0
// #endif // _SHIP

#define VER_PRIVATE                 VS_FF_PRIVATEBUILD

// PRIVATEBUILD flag is set if not built by build lab
// #ifndef _VSBUILD
// #define VER_PRIVATEBUILD VS_FF_PRIVATEBUILD
// #else  // _VSBUILD
// #define VER_PRIVATEBUILD 0
// #endif // _VSBUILD


#define VER_FILEFLAGSMASK           VS_FFI_FILEFLAGSMASK
#define VER_FILEFLAGS               (VER_PRERELEASE|VER_DEBUG|VER_PRIVATE)
#define VER_FILEOS                  VOS__WINDOWS32

#define VER_COMPANYNAME_STR         "Microsoft Corporation"

#ifndef VER_LEGALTRADEMARKS_STR
#ifndef DONT_DEFINE_LEGALTRADEMARKS_STR
#define VER_LEGALTRADEMARKS_STR     ""
#endif
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

#define VER_PRIVATEBUILD_STR    QUOTE_MACRO(FX_VER_PRIVATEBUILD_STR)

#if defined(__BUILDMACHINE__)
#if defined(__BUILDDATE__)
#define B2(x,y) " (" #x "." #y ")"
#define B1(x,y) B2(x, y)
#define BUILD_MACHINE_TAG B1(__BUILDMACHINE__, __BUILDDATE__)
#else
#define B2(x) " built by: " #x
#define B1(x) B2(x)
#define BUILD_MACHINE_TAG B1(__BUILDMACHINE__)
#endif

// BUILD_MACHINE_TAG_HELP_ABOUT is used to populate VS Helpabout.
#if defined(__BUILDDATE__)
#define BUILD_MACHINE_TAG_HELP_ABOUT BUILD_MACHINE_TAG
#else
#define B4(x) "" #x
#define B3(x) B4(x)
#define BUILD_MACHINE_TAG_HELP_ABOUT B3(__BUILDMACHINE__)
#endif

#if defined(__BUILDMACHINE_LEN__)
#if __BUILDMACHINE_LEN__ >= 25
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG
#elif __BUILDMACHINE_LEN__ == 24
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG " "
#elif __BUILDMACHINE_LEN__ == 23
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "  "
#elif __BUILDMACHINE_LEN__ == 22
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "   "
#elif __BUILDMACHINE_LEN__ == 21
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "    "
#elif __BUILDMACHINE_LEN__ == 20
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "     "
#elif __BUILDMACHINE_LEN__ == 19
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "      "
#elif __BUILDMACHINE_LEN__ == 18
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "       "
#elif __BUILDMACHINE_LEN__ == 17
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "        "
#elif __BUILDMACHINE_LEN__ == 16
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "         "
#elif __BUILDMACHINE_LEN__ == 15                       
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "          "
#elif __BUILDMACHINE_LEN__ == 14                               
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "           "
#elif __BUILDMACHINE_LEN__ == 13                                 
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "            "
#elif __BUILDMACHINE_LEN__ == 12                               
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "             "
#elif __BUILDMACHINE_LEN__ == 11                               
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "              "
#elif __BUILDMACHINE_LEN__ == 10                               
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "               "
#elif __BUILDMACHINE_LEN__ == 9                                
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "                "
#elif __BUILDMACHINE_LEN__ == 8                                
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "                 "
#elif __BUILDMACHINE_LEN__ == 7                                
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "                  "
#elif __BUILDMACHINE_LEN__ == 6                                
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "                   "
#elif __BUILDMACHINE_LEN__ == 5                                
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "                    "
#elif __BUILDMACHINE_LEN__ == 4                                
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "                     "
#elif __BUILDMACHINE_LEN__ == 3                                
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "                      "
#elif __BUILDMACHINE_LEN__ == 2                                
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "                       "
#elif __BUILDMACHINE_LEN__ == 1                                
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG "                        "
#else
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG
#endif
#else
#define BUILD_MACHINE_TAG_PADDED BUILD_MACHINE_TAG
#endif
#else
#define BUILD_MACHINE_TAG
#define BUILD_MACHINE_TAG_PADDED
#define BUILD_MACHINE_TAG_HELP_ABOUT
#endif

#endif // INCLUDE_VSVER_H_
