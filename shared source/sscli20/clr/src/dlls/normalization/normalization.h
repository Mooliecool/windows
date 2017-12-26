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

// Normalization.h
//
// WARNING: This .DLL is supposed to be the same in managed and native code,
// starting with the Longhorn & Whidbey releases.  Please make sure they stay in sync.

#define NORMALIZATION_API __declspec(dllimport)

#include <specstrings.h>

#ifdef __cplusplus
extern "C" {
#endif

enum NORM_FORM
{
    NormalizationOther  = 0,
    NormalizationC      = 0x1,
    NormalizationD      = 0x2,
    NormalizationKC     = 0x5,
    NormalizationKD     = 0x6
};

// Windows API Normalization Functions
int WINAPI NormalizeString(
    NORM_FORM                            NormForm,
    __in LPCWSTR                         lpSrcString,
    int                                  cwSrcLength,
    __out_ecount_opt(cwDstLength) LPWSTR lpDstString,
    int                                  cwDstLength );

bool WINAPI IsNormalizedString(
    NORM_FORM    NormForm,
    __in LPCWSTR lpString,
    int          cwLength );

// Not implimented at this time
/*
int WINAPI ConvertToNormalizedString(
    UINT        CodePage,
    NORM_FORM   NormForm,
    LPCWSTR     lpSrcString,
    int         cwSrcLength,
    LPWSTR      lpDstString,
    int         cwDstLength );
*/

#ifdef __cplusplus
}
#endif
