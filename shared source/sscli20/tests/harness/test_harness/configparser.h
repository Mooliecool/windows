/*============================================================
**
** Header: configparser.h
**
** Purpose: Header file for test config parsing.
**
** 
**  
**   Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
**  
**   The use and distribution terms for this software are contained in the file
**   named license.txt, which can be found in the root of this distribution.
**   By using this software in any fashion, you are agreeing to be bound by the
**   terms of this license.
**  
**   You must not remove this notice, or any other, from this software.
**  
** 
**
**=========================================================*/

#ifndef _CONFIGPARSER_H
#define _CONFIGPARSER_H


typedef struct
{
    FILE *ConfigFile;

    char *ReadBuf;
    char *LineBuf;
    int LineNum;
} TestconfigParser;


int InitTestconfigParser(TestconfigParser *Parser, char *ConfigFileName);
void FreeTestconfigParser(TestconfigParser *Parser);

int GetNextTestCase(TestconfigParser *Parser, char *Path, int *pEnabled,
		    const char *SubTest);

#endif /* _CONFIGPARSER_H */
