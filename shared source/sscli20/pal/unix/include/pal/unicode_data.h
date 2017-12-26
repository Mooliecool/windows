/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/unicode_data.h

Abstract:

    Data, data retrieval function declarations.

--*/

#ifndef _UNICODE_DATA_H_
#define _UNICODE_DATA_H_

#if !HAVE_CFSTRING

#include "pal/palinternal.h"

typedef struct
{
  WCHAR nUnicodeValue;
  WORD  C1_TYPE_FLAGS;
  WCHAR nOpposingCase;             /* 0 if no opposing case. */
  WORD  rangeValue;
} UnicodeDataRec;

/* Global variables. */
extern CONST UnicodeDataRec UnicodeData[];
extern CONST UINT UNICODE_DATA_SIZE;
extern CONST UINT UNICODE_DATA_DIRECT_ACCESS;

/*++
Function:
  GetUnicodeData
  This function is used to get information about a Unicode character.

Parameters:
nUnicodeValue
  The numeric value of the Unicode character to get information about.
pDataRec
  The UnicodeDataRec to fill in with the data for the Unicode character.

Return value:
  TRUE if the Unicode character was found.

--*/
BOOL GetUnicodeData(INT nUnicodeValue, UnicodeDataRec *pDataRec);

#endif  /* !HAVE_CFSTRING */

#endif  /* _UNICODE_DATA_H_ */
