/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Tests iswprint with all wide characters, ensuring they are 
**          consistent with GetStringTypeExW.
**
** 
**  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
** 
**  The use and distribution terms for this software are contained in the file
**  named license.txt, which can be found in the root of this distribution.
**  By using this software in any fashion, you are agreeing to be bound by the
**  terms of this license.
** 
**  You must not remove this notice, or any other, from this software.
** 
**
**===================================================================*/

#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
    WORD Info;
    int ret;
    int i;
    WCHAR ch;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    for (i=0; i<=0xFFFF; i++)
    {
        ch = i;
        ret = GetStringTypeExW(LOCALE_USER_DEFAULT, CT_CTYPE1, &ch, 1, &Info);
        if (!ret)
        {
            Fail("GetStringTypeExW failed to get information for %#X!\n", ch);
        }

        ret = iswprint(ch);
        if (Info & (C1_BLANK|C1_PUNCT|C1_ALPHA|C1_DIGIT))
        {
            if (!ret)
            {
                Fail("iswprint returned incorrect results for %#X: "
                    "expected printable\n", ch);
            }
        }
        else
        {
            if (ret)
            {
                Fail("iswprint returned incorrect results for %#X: "
                    "expected non-printable\n", ch);
            }
        }
    }

    PAL_Terminate();
    return PASS;
}
