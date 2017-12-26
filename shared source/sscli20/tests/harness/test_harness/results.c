/*============================================================
**
** Source: results.c
**
** Purpose: Contains the functions to read in information from files and
** to write log results back out.
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
#include "results.h"
#include "error.h"
#include "util.h"



/*
 * Modifies the given string, replacing every carriage return or line feed 
 * with a space.  Returns a pointer to the given string.
 */
char *RemoveLinefeeds(char *Data)
{
    if (Data == NULL)
    {
        return NULL;
    }

    while (*Data != '\0')
    {
        if (*Data == '\n' || *Data == '\r')
        {
            *Data = ' ';
        }

        Data++;
    }
  
    return Data;
}

void ConvertBackSlashesToWindowsStyle(char *Data)
{
    char *tempData = Data;
	
	while (*tempData != '\0')
	{
		if (*tempData == '/') 
		{
			*tempData = '\\';
		}
		tempData++;
	}
    
}

void removeSpecialCharacters(const char *Name,char *processedName)
{
	char *tempName = processedName;
	while (*Name != '\0')
	{	if ((*Name != ' ') && (*Name != '#') && (*Name != ','))
		{ *tempName = *Name;
	       tempName++;
		}
		Name++;
	}

	*tempName = 0;

}			


void WriteSumFile(FILE *SumFile,const char* Name,const char* Section, const char* SubSection,
				  const char *Result,double timeTaken)

{
  int MaxSize;
  size_t nChars;
  char logBuffer[LINE_BUF_SIZE*2];
  char output[256];
  char *processedName;
  
  time_t ltime;
  struct tm *stime;
  char formattedDate[20];

 
  MaxSize = sizeof(logBuffer);
  sprintf(output,"%d",(int)ceil(timeTaken));

  time(&ltime);
  stime = localtime(&ltime);
  strftime(formattedDate,sizeof(formattedDate),"%Y-%m-%d",stime);

  ConvertBackSlashesToWindowsStyle((char *)SubSection);
   
  processedName = (char *)malloc(strlen(Name)+1);
  removeSpecialCharacters(Name,processedName);

  AppendDelimitedStringsSum(logBuffer, MaxSize, C_COMMA, C_ESCAPE,
        processedName,Section,SubSection,PLATFORM,BUILDTYPE,BUILDNUMBER,formattedDate,Result,output,NULL);
  
  
  free(processedName);
  // to remove the trailing commas at the end of every line in the summary file
  // logBuffer[MaxSize - 1] = 0;
  logBuffer[strlen(logBuffer)-1] = 0;
  RemoveLinefeeds(logBuffer);
  
  nChars = fprintf(SumFile, "%s%s", logBuffer,EOLN);
  
  if (nChars < strlen(logBuffer)+1)
    {
        HarnessError("ERROR: Failed to write to the file to log the "
                "results.  This is a fatal error.  errno is %d.\n",errno);
    }
  
}

/* 
 * Logs the result of a test case to the file in the following form:
 * <TestDir>,<Section>,<SubSection>,<Name>,<type>,<result>,<Output>
 */
void LogResult(FILE *ResultsFile, const char *Section, const char *SubSection,
               const char *Name, TEST_TYPE Type, const char *TestDir,
               const char *Phase, const char *Result, const char *Output)
{
    int MaxSize;
    size_t nChars;
    char logBuffer[LINE_BUF_SIZE * 2];
    char *testType;
  

    testType = TypeToString(Type);

    MaxSize = sizeof(logBuffer);

    if (Output == NULL)
    {
        Output = "";
    }

    /* 
     * Silently fail if we run out of buffer space, but throw a null in to be 
     * safe. 
     */
    AppendDelimitedStringsVA(logBuffer, MaxSize, C_COMMA, C_ESCAPE,
        TestDir, Section, SubSection, Name, testType, Phase, 
        Result, Output, NULL);

    logBuffer[MaxSize - 1] = 0;


    RemoveLinefeeds(logBuffer);

    nChars = fprintf(ResultsFile, "%s\n", logBuffer);
    if (nChars < strlen(logBuffer)+1)
    {
        HarnessError("ERROR: Failed to write to the file to log the "
                "results.  This is a fatal error.  errno is %d.\n",errno);
    }
}

/*
 * Logs a result the testharness was unable to change to the test case dir.
 */
void LogDirectoryError(FILE* ResultsFile, char* Path)
{
    LogResult(ResultsFile, SZ_BLANKDATA, SZ_BLANKDATA,
              SZ_BLANKDATA, UNKNOWN_TYPE, Path,
              TSTPHASE_BUILD, TSTRESULT_DIRERR, 
              "HARNESS: Could not change to test case directory.");
    
    LogResult(ResultsFile, SZ_BLANKDATA, SZ_BLANKDATA,
              SZ_BLANKDATA, UNKNOWN_TYPE, Path,
              TSTPHASE_EXEC, TSTRESULT_DIRERR, 
              "HARNESS: Could not change to test case directory.");
}

/*
 * Logs that the testharness was unable to read or parse the testinfo.dat for 
 * a test case.
 */
void LogTestDatError(FILE* ResultsFile, char* Path)
{
    LogResult(ResultsFile, SZ_BLANKDATA, SZ_BLANKDATA, 
              SZ_BLANKDATA, UNKNOWN_TYPE, Path,
              TSTPHASE_BUILD, TSTRESULT_NOINFO, 0);
    
    LogResult(ResultsFile, SZ_BLANKDATA, SZ_BLANKDATA, 
              SZ_BLANKDATA, UNKNOWN_TYPE, Path,
              TSTPHASE_EXEC, TSTRESULT_NOINFO, 0);   
}

/*
 * Logs that the testharness was unable to read or parse the testconfig.dat
 */
void LogConfigFileError(FILE* ResultsFile, char* path)
{
    LogResult(ResultsFile, SZ_BLANKDATA, SZ_BLANKDATA, 
              SZ_BLANKDATA, UNKNOWN_TYPE, path,
              "CONFIG", "HARNESS: Error in the testconfig.dat file.", 0);   
}


/*
 * Log that the testharness was unable to build the command to execute the 
 * test.
 */
void LogCommandLineError(FILE* ResultsFile, char* Path)
{    
    LogResult(ResultsFile, SZ_BLANKDATA, SZ_BLANKDATA, 
              SZ_BLANKDATA, UNKNOWN_TYPE, Path,
              TSTPHASE_EXEC, "HARNESS: Unable to build command line", 0);
}
