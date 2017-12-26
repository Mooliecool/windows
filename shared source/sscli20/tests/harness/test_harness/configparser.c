/*============================================================
**
** Source: configparser.c
**
** Purpose: Functions for parsing test configs.
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

#include "testharness.h"
#include "configparser.h"
#include "error.h"
#include "util.h"

/*
 * Initialize the members of a ConfigParser structure.
 */
int InitTestconfigParser(TestconfigParser *Parser, char *TestconfigName)
{
    Parser->ConfigFile = fopen(TestconfigName, "r");
    if (Parser->ConfigFile == NULL)
    {
        HarnessError("Unable to open configuration file: %s\n", 
                     TestconfigName);
    }

    Parser->ReadBuf = malloc(READ_BUF_SIZE);
    if (Parser->ReadBuf == 0)
    {
        return 1; /* Error */
    }

    Parser->LineBuf = malloc(LINE_BUF_SIZE);
    if (Parser->LineBuf == 0)
    {
        free(Parser->ReadBuf);
        return 1; /* Error */
    }

    Parser->ReadBuf[0] = 0;
    Parser->LineBuf[0] = 0;
    Parser->LineNum = 0;
    
    return 0; /* OK */
}

/*
 * Clean up a TestconfigParser. Free's the path string.
 */
void FreeTestconfigParser(TestconfigParser *Parser)
{
    if (Parser == 0)
    {
        return;
    }

    if (Parser->ConfigFile != 0)
    {
        fclose(Parser->ConfigFile);
        Parser->ConfigFile = 0;
    }

    free(Parser->ReadBuf);
    free(Parser->LineBuf);
}

/*
 * Parses a line of test config data, outputting to Path and Enabled. The 
 * input Data is assumed to be in the following format:
 * <test case dir>,<enabled>
 * Where enabled is 1 to indicate this test is in fact enabled, or 0.
 *
 * Returns 0 on Success,
 *         1 on Failure.
 *         2 on blank data or comment.
 */
static int ParseTestCaseData(char *Data, char *Path, int *pEnabled)
{
    char *pSep;
    int iLen;
  
    Path[0] = 0;
    *pEnabled = 0;

    /* Fail if NULL/empty string (special case) */
    if( (Data == NULL) ||
        (strlen(Data) <= 0) )
    {
        return 2;
    }

    /* Remove whitespaces */
    TrimWhiteSpace(Data);

    /* Check for a commented line */
    if( Data[0] == '#' )
    {
        return 2;
    }

    pSep = strchr(Data, ',');
  
    /* Fail if we find no comma */
    if (pSep == 0) 
    {
        return 1;
    }

    iLen = pSep - Data;
  
    strncpy(Path, Data, iLen);
    Path[iLen] = 0;

    /* Make sure there was no dir seperator at the end */
    if (Path[iLen - 1] == '\\' || Path[iLen - 1] == '/')
    {
        Path[iLen - 1] = 0;
    }

    *pEnabled = (pSep[1] == '1');
  
    return 0;
}

/**
 * GetNextTestCase
 * 
 * Gets the next valid test case line from Parser->ConfigFile
 *
 * Returns: 0 on success
 *          1 no more test case available
 *          2 invalid testinfo
 */
int GetNextTestCase(TestconfigParser *Parser, char *Path, int *pEnabled,
		    const char *SubTest)
{
    int iRet;

    Path[0] = 0;
    *pEnabled = 0;

    while (1)
    {
        /* Read one line from the file */
        if (ReadLineEx(Parser->ConfigFile, Parser->LineBuf) != 0)
        {
            return 1;
        }

        /* Get test case information */
        iRet = ParseTestCaseData(Parser->LineBuf, Path, pEnabled);
        if (iRet == 0)
        {
            break;
        }
        else if (iRet == 1)
        {
            return 2; /* Test Case Data was bad */
        }
    }

    if(SubTest != NULL)
    { 
        if(strcmp(Path, SubTest)!=0)
	{
	    if((strstr(Path, SubTest)!=Path) ||  
	       (Path[strlen(SubTest)]!='\\' && Path[strlen(SubTest)]!='/'))
	    {
	        *pEnabled = 0;
	    }
	}
    }
    return 0;
}







