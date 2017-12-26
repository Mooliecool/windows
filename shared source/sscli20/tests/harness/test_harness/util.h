/*============================================================
**
** Source: util.h
**
** Purpose: Header for miscellaneous helper functions.
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

#ifndef _UTIL_H
#define _UTIL_H

TEST_TYPE StringToType(char *TypeString);
char *TypeToString(TEST_TYPE Type);

TEST_LANG StringToLang(char *LangString);
char *LangToString(TEST_LANG Lang);

int AppendStringsVA(char *Buffer, int MaxLength, ...);

int AppendDelimitedStringsVA(char *Buffer, int MaxLength, char DelimChar,
                             char EscapeChar, ...);

int AppendDelimitedStringsSum(char *Buffer, int MaxLength, char DelimChar,
                             char EscapeChar, ...);

int ReadLine(FILE *InputFile, char *ReadBuf, char *Line);
int ReadLineEx(FILE *pInputFile, char *pLine);

void TrimWhiteSpace(char *szBuf);

#endif
