/*============================================================
**
** Header: testinfo.h
**
** Purpose: Header file for test case information.
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

#ifndef _TESTINFO_H
#define _TESTINFO_H


#ifndef TRUE
  #define TRUE  1
  #define FALSE 0
#endif


typedef struct
{
    char *Path;
    char *Section;
    char *SubSection;
    char *Name;
    char *Desc;
    char *Executable1;
    char *Executable2;
    char *Library1;
    char *Library2;
    TEST_LANG Language;
    TEST_TYPE Type;
} TestInfo;


void InitTestInfo(TestInfo *pTestInfo);
void FreeTestInfo(TestInfo *pTestInfo);
int ReadTestInfo(TestInfo *pInfo, char *RelativePath);

#endif /* _TESTINFO_H */
