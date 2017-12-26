/*============================================================
**
** Header: params.h
**
** Purpose: Header file for test parameter information.
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

#ifndef _PARAMS_H
#define _PARAMS_H


typedef struct
{
    char *TestCaseDir;
    char *XRunFile;
    char *TestConfigName;
    char *TestResultsName;
	char *sumResultsName;
	char *SubTest;

    FILE *TestResultsFile;
	FILE *sumResultsFile;
		
} THParams;

void InitTestParams(THParams *HarnessParams);
void FreeTestParams(THParams *HarnessParams);
void ReadParams(THParams *HarnessParams);


#endif
