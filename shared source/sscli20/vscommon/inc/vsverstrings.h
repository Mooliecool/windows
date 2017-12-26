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

#ifndef VER_PRODUCTNAME_STR
    #if defined(CSC_INVOKED)
        #define VER_PRODUCTNAME_STR      "Microsoft\u00AE Visual Studio\u00AE 2005"
    #else
        #define VER_PRODUCTNAME_STR      L"Microsoft\256 Visual Studio\256 2005"
    #endif
#endif

#ifndef VER_LEGALCOPYRIGHT_YEARS
#define VER_LEGALCOPYRIGHT_YEARS    "1998-2005"
#define VER_LEGALCOPYRIGHT_YEARS_L  L"1998-2005"
#endif

// The following copyright is intended for display in the Windows Explorer property box for a DLL or EXE
// See \\lca\pdm\TMGUIDE\Copyright\Crt_Tmk_Notices.xls for copyright guidelines
//
#ifndef VER_LEGALCOPYRIGHT_STR
    #if defined(CSC_INVOKED)
        #define VER_LEGALCOPYRIGHT_STR      "\u00A9 Microsoft Corporation. All rights reserved."
    #else
        #define VER_LEGALCOPYRIGHT_STR      "\251 Microsoft Corporation. All rights reserved."
    #endif
#endif

// VSWhidbey #495749
// Note: The following legal copyright is intended for situations where the copyright symbol doesn't display 
//       properly.  For example, the following copyright should be displayed as part of the logo for DOS command-line 
//       applications.  If you change the format or wording of the following copyright, you should apply the same
//       change to fxresstrings.txt (for managed applications).
#ifndef VER_LEGALCOPYRIGHT_LOGO_STR
    #define VER_LEGALCOPYRIGHT_LOGO_STR    "Copyright (c) Microsoft Corporation.  All rights reserved."
    #define VER_LEGALCOPYRIGHT_LOGO_STR_L L"Copyright (c) Microsoft Corporation.  All rights reserved."
#endif
