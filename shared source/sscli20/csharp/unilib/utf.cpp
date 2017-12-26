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

// utf.cpp

#include "pch.h"
#include "utf.h"

int WINAPI UTF8ToUTF16 (PCSTR pUTF8, int cbUTF, PWSTR pUTF16, int cchUTF16)
{
    return MultiByteToWideChar(CP_UTF8, 0, pUTF8, cbUTF, pUTF16, cchUTF16);
}

int WINAPI UTF16LengthOfUTF8 (PCSTR pUTF8, int cbUTF8)
{
    return MultiByteToWideChar(CP_UTF8, 0, pUTF8, cbUTF8, NULL, 0);
}

int WINAPI UTF16ToUTF8 (PCWSTR pUTF16, int * pcchUTF16, PSTR pUTF8, int cbUTF8)
{
    int cchSrc = *pcchUTF16;
    if (cchSrc == -1)
    {
        *pcchUTF16 = (int)wcslen(pUTF16);
    }
    
    return WideCharToMultiByte(CP_UTF8, 0, pUTF16, cchSrc, pUTF8, cbUTF8, NULL, false);
}

int WINAPI UTF8LengthOfUTF16 (PCWSTR pUTF16, int cchUTF16)
{
    return WideCharToMultiByte(CP_UTF8, 0, pUTF16, cchUTF16, NULL, 0, 0, 0);
}
