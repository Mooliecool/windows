// W4Char.h - UCS-4 Character definitions and string routines
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1999 Microsoft Corporation.  All Rights Reserved.
//
// July 24, 1999 [paulde] Paul Chase Dempsey
//
//-----------------------------------------------------------------
#pragma once
#include "unichar.h"

typedef DWORD W4CHAR;
typedef       W4CHAR * PW4STR;
typedef const W4CHAR * PCW4STR;

//-----------------------------------------------------------------
// W4CharRange - Returns a range code for a W4CHAR
//
// Returns:
// -1  Out of UTF-16 (Unicode) range
//  0  Basic Unicode, i.e. BMP, plane 1
//  1  Surrogate
//
int     WINAPI W4CharRange (W4CHAR ch);

//-----------------------------------------------------------------
// W4CHAR length routines
//
int WINAPI WLengthOfW4Char   (W4CHAR ch);       // Count of WCHARs to represent the W4CHAR
int WINAPI WStrLengthOfW4Str (PCW4STR pw4Str);  // Count of WCHARs in a W4STR
int WINAPI W4LengthOfWStr    ( __in_z PCWSTR psz);      // Count of W4CHARs in a WSTR
int WINAPI W4StrLen          (PCW4STR pw4Str);  // Count of W4CHARS in a W4STR

#define StrLenW4 W4Strlen   // alias

//-----------------------------------------------------------------
// CopyW4CharToWChar - Copy one W4CHAR to PWSTR
//
PWSTR   WINAPI CopyW4CharToWChar ( __out_z PWSTR pDst, W4CHAR ch);

//-----------------------------------------------------------------
// CopyNW4StrToW - Copy n W4Chars to PWSTR
//
PWSTR   WINAPI CopyNW4StrToW     ( __out_ecount(cch4) PWSTR pDst, PCW4STR pw4Str, int cch4);  

//-----------------------------------------------------------------
// CopyW4StrToWN - Copy W4Chars to a PWSTR, up to n WCHARs
//
int     WINAPI CopyW4StrToWN    ( __out_ecount(cchDstMax) PWSTR      pDst, 
                                 PCW4STR    pw4Str, 
                                 int        cchDstMax, 
                                 PCW4STR *  ppw4 = NULL);

//-----------------------------------------------------------------
// W4CharFromWChar Get 1 W4CHAR from PCWSTR
//
// Returns 1 W4CHAR from pch, optionally getting a pointer to the
// next position in the source.
//
W4CHAR  WINAPI W4CharFromWChar  ( __in_z PCWSTR pch, __deref_out_z PCWSTR * ppchNext = NULL);

//-----------------------------------------------------------------
// CopyWStrToW4Str - Copy from PWSTR to PW4STR
//
PW4STR  WINAPI CopyWStrToW4Str  (PW4STR pDst, __in_z PCWSTR pSrc);

//-----------------------------------------------------------------
// MakeW4Char - Make a W4CHAR from a surrogate pair
//
// The result is garbage if the characters are not a 
// surrogate pair in the specified order.
//
W4CHAR  WINAPI MakeW4Char (WCHAR chHi, WCHAR chLo);

//-----------------------------------------------------------------
// MakeSurrogatePair - Make a surrogate pair from a W4CHAR
//
// The result is garbage if the W4CHAR is not in the UCS-4 range 
// that surrogates represent.
//
PWSTR   WINAPI MakeSurrogatePair ( __out_ecount(2) PWSTR pch, W4CHAR ch);

// inline implementations
#include "W4Char.inl"

