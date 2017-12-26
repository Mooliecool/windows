/*============================================================
**
** Source: test.c
**
** Purpose: Test for wsprintfW() function
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
**=========================================================*/

#define UNICODE
#include <palsuite.h>


char * ErrorMessage = NULL; 
WCHAR * BadResult = NULL; 
WCHAR buf[256];

/* memcmp is used to verify the results, so this test is dependent on it. */
/* ditto with strlen */


BOOL test1()
{
   int pos = 42;

   wsprintf(buf, convert("foo %i"), pos);
   if (memcmp(buf, convert("foo 42"), wcslen(buf)*2 + 2) != 0)
   {
       ErrorMessage = "ERROR: (Test 1) Failed. The correct string is"
                      " 'foo 42' and the result returned was ";
       BadResult = buf;
       return FAIL;
   }

   wsprintf(buf, convert("foo %li"), 0xFFFF);
   if (memcmp(buf, convert("foo 65535"), wcslen(buf)*2 + 2) != 0)
   {
       ErrorMessage = "ERROR: (Test 2) Failed. The correct string is"
                      " 'foo 65535' and the result returned was ";
       BadResult = buf;
       return FAIL;
   }

   wsprintf(buf, convert("foo %3i"), pos);
   if (memcmp(buf, convert("foo  42"), wcslen(buf)*2 + 2) != 0)
   {
       ErrorMessage = "ERROR: (Test 3) Failed. The correct string is"
                      " 'foo  42' and the result returned was ";
       BadResult = buf;
       return FAIL;
   }

   wsprintf(buf, convert("foo %-3i"), pos);
   if (memcmp(buf, convert("foo 42 "), wcslen(buf)*2 + 2) != 0)
   {
       ErrorMessage = "ERROR: (Test 4) Failed. The correct string is"
                      " 'foo 42 ' and the result returned was ";
       BadResult = buf;
       return FAIL;
   }

   wsprintf(buf, convert("foo %.1i"), pos);
   if (memcmp(buf, convert("foo 42"), wcslen(buf)*2 + 2) != 0)
   {
       ErrorMessage = "ERROR: (Test 5) Failed. The correct string is"
                      " 'foo 42' and the result returned was ";
       BadResult = buf;
       return FAIL;
   }

   wsprintf(buf, convert("foo %.3i"), pos);
   if (memcmp(buf, convert("foo 042"), wcslen(buf)*2 + 2) != 0)
   {
       ErrorMessage = "ERROR: (Test 6) Failed. The correct string is"
                      " 'foo 042' and the result returned was ";
       BadResult = buf;
       return FAIL;
   }

   wsprintf(buf, convert("foo %03i"), pos);
   if (memcmp(buf, convert("foo 042"), wcslen(buf)*2 + 2) != 0)
   {
       ErrorMessage = "ERROR: (Test 7) Failed. The correct string is"
                      " 'foo 042' and the result returned was ";
       BadResult = buf;
       return FAIL;
   }

   wsprintf(buf, convert("foo %#i"), pos);
  if (memcmp(buf, convert("foo 42"), wcslen(buf)*2 + 2) != 0)
  {
       ErrorMessage = "ERROR: (Test 8) Failed. The correct string is"
                      " 'foo 42' and the result returned was ";
       BadResult = buf;
       return FAIL;
   }

   return PASS;
}

int __cdecl main(int argc, char *argv[])
{

   /*
    * Initialize the PAL and return FAILURE if this fails
    */

   if(0 != (PAL_Initialize(argc, argv)))
   {
       return FAIL;
   }

   if(test1())
   {
       Fail("%s '%s'\n",ErrorMessage,convertC(BadResult));

   }

   PAL_Terminate();
   return PASS;

}


