/*============================================================
**
** Header: results.h
**
** Purpose: Header file for results logging.
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

#ifndef _RESULTS_H
#define _RESULTS_H

void LogResult(FILE *ResultsFile, const char *Section, const char *SubSection,
               const char *Name, TEST_TYPE Type, const char *TestDir,
               const char *Phase, const char *Result, const char *Output);

void LogDisabledTestError(FILE* ResultsFile, char* Path);
void LogDirectoryError(FILE* ResultsFile, char* Path);
void LogTestDatError(FILE* ResultsFile, char* Path);
void LogConfigFileError(FILE* ResultsFile, char* path);
void LogCommandLineError(FILE* ResultsFile, char* Path);
void WriteSumFile(FILE *SumFile,const char* Name,const char* Section, const char* SubSection,
				  const char *Result,double timeTaken);


#endif
