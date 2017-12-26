/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/utf8.h

Abstract:
    Header file for UTF-8 conversion functions.

Revision History:

--*/

#ifndef _PAL_UTF8_H_
#define _PAL_UTF8_H_

#include <pal/palinternal.h> /* for WCHAR */

/*++
Function :
    UTF8ToUnicode

    Convert a string from UTF-8 to UTF-16 (UCS-2)
--*/
int UTF8ToUnicode(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, DWORD dwFlags);


/*++
Function :
    UnicodeToUTF8

    Convert a string from UTF-16 (UCS-2) to UTF-8
--*/
int UnicodeToUTF8(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);


#endif /* _PAL_UTF8_H_ */
