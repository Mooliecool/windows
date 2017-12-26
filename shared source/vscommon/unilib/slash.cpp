// slash.cpp
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
#include "pch.h"

#define BS '\\'
#define FS '/'


PWSTR WINAPI NormalizeFileSlashes (_Inout_z_ PWSTR szFile)
{
    PWSTR pchCopy;
    PWSTR pch = szFile;

    if (FS == *pch) *pch = BS;
    pch++;
    pchCopy = pch;

    WCHAR ch;
    while (ch = *pch)
    {
        if ((BS == ch) || (FS == ch))
        {
            *pchCopy++ = BS;
            pch++;
            while ((BS == (ch = *pch)) || (FS == ch))
                pch++;
        }
        else
        {
            *pchCopy++ = ch;
            pch++;
        }
    }
    *pchCopy = 0;
    return pchCopy;
}


PSTR WINAPI NormalizeFileSlashesA(_Inout_z_ PSTR szFile)
{
    PSTR pchCopy;
    PSTR pch = szFile;

    // If first char is a slash, convert and advance
    // This allows double slash at the start

    if (FS == *pch) *pch = BS;
    pch++;
    pchCopy = pch;

    char ch;
    while (ch = *pch)
    {
        if (IsDBCSLeadByte(ch))
        {
            *pchCopy++ = ch;
            pch++;
            *pchCopy++ = *pch++;
        }
        else if ((BS == ch) || (FS == ch))
        {
            *pchCopy++ = BS; 
            pch++;
            while ((BS == (ch = *pch)) || (FS == ch)) // these are never lead bytes, so MBCS safe
                pch++;
        }
        else
        {
            *pchCopy++ = ch;
            pch++;
        }
    }
    *pchCopy = 0;
    return pchCopy;
}
