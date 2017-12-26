//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  This module provides wrappers that simulate unicode API's on Win95.
//
//-------------------------------------------------------------------------------------------------

#pragma once

/*****************************************************************************/
// @@ fullwidth/halfwidth conversion macros

// MAKEFULLWIDTH - Converts a half-width to full-width character

#define MAKEFULLWIDTH(c) (((c) < 0x0021 || (c) > 0x007E) ? (c) : ((c) + 0xFF00 - 0x0020))

// MAKEHALFWIDTH - Converts a full-width character to half-width
#define MAKEHALFWIDTH(c) (((c) < 0xFF01 || (c) > 0xFF5E) ? (c) : ((c) - 0xFF00 + 0x0020))

// ISFULLWIDTH - Returns if the character is full width
#define ISFULLWIDTH(c) ((c) > 0xFF00 && (c) < 0xFF5F)

#if ID_TEST

/*****************************************************************************/
// @@ Unicode/Ansi string conversion functions
// - WIDESTR(sz) - converts an ANSI string to unicode on the stack
// - ANSISTR(wsz) - converts a unicode string to ANSI on the stack
// - WszCpyToSz(sz, wsz) - copy a unicode string into an ansi buffer
// - SzCpyToWsz(wsz, sz) - copy an ANSI string to unicode buffer
// Be care of using the WIDESTR and ANSISTR macros inside of a loop, since
// each invocation causes stack allocation.  If you need to perform
// ANSI / Unicode version within a loop, use WszCpyToSz or SzCpyToWsz

char * WszCpyToSz(
    _Out_z_cap_x_(strlen(wszSrc)+ 1)char * szDest,
    _In_z_ const WCHAR * wszSrc);

WCHAR * SzCpyToWsz(
    _Out_z_cap_x_((strlen(szSrc)+ 1)* sizeof(WCHAR))WCHAR * wszDest,
    _In_z_ const char * szSrc);

#endif

/*****************************************************************************/
// @@ Unicode string compare functions.
// - WszEqNoCase      - case insensitive,  default lcid, compare entire string
// - WszEqLenNoCase   - case insensitive,  default lcid,

bool WszEqNoCase(
    _In_z_ const WCHAR * wsz1,
    _In_z_ const WCHAR * wsz2);

bool WszEqLenNoCase(
    _In_count_(cch)const WCHAR * wsz1,
    int cch,
    _In_z_ const WCHAR * wsz2);

/*****************************************************************************/
// @@ Unicode string conversion functions.
// - BinaryToUnicode          - converts binary stream into hex-encoded string

void BinaryToUnicode(
    BYTE * pbSrc,
    UINT cbSrc,
    _Out_z_cap_x_(" At least cbSrc ")LPWSTR pwszDst);
