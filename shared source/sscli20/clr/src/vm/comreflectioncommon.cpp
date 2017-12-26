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
////////////////////////////////////////////////////////////////////////////////
// Date: April 15, 1998
////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "comreflectioncommon.h"

// GetClassStringVars
// This routine converts the contents of a STRINGREF into a LPUTF8.  It returns
//  the size of the string
//  stringRef -- The string reference
//  szStr -- The output string.  This is allocated by the caller
//  cStr -- the size of the allocated string
//  pCnt -- output string size
LPUTF8 GetClassStringVars(STRINGREF stringRef, CQuickBytes *pBytes,
                          DWORD* pCnt, bool bEatWhitespace)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    WCHAR* wzStr = stringRef->GetBuffer();
    int len = stringRef->GetStringLength();
    _ASSERTE(pBytes);

    // If we have to eat the whitespace, do it.
    if (bEatWhitespace) {
        while(1) {
            if(COMCharacter::nativeIsWhiteSpace(wzStr[0])) {
                wzStr++;
                len--;
            }
            else
                break;
        }

        while(1) {
            if(COMCharacter::nativeIsWhiteSpace(wzStr[len-1]))
                len--;
            else
                break;
        }
    }
    
    *pCnt = WszWideCharToMultiByte(CP_UTF8, 0, wzStr, len,
                                   0, 0, 
                                   NULL, NULL);
    if (*pCnt + 1 < *pCnt)
        COMPlusThrowArgumentOutOfRange(L"name", L"ArgumentOutOfRange_GetByteCountOverflow");
    
    pBytes->AllocThrows(*pCnt + 1);
    LPSTR pStr = (LPSTR)pBytes->Ptr();

    *pCnt = WszWideCharToMultiByte(CP_UTF8, 0, wzStr, len,
                                   pStr, *pCnt, 
                                   NULL, NULL);

    // Null terminate the string
    pStr[*pCnt] = '\0';
    return pStr;
}

