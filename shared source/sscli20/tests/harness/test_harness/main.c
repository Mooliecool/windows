/*============================================================
**
** Source: main.c
**
**
** Purpose: The heart of the test harness.  This file reads the environment
** and gets everything into motion.
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

#include "testharness.h"
#include "configparser.h"
#include "testinfo.h"
#include "error.h"
#include "exectest.h"
#include "util.h"
#include "params.h"
#include "results.h"


#define SEPARATOR_LINE  "#######################################################"
#define UNDER_LINE      "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

char szOutputBuf[MAX_OUTPUT];
char szCmdBuf[LINE_BUF_SIZE];


/*
 * Given the parameters for the harness and the test info, build the command 
 * line to execute the test in the CmdLine buffer.Returns 1 for failure, 0 
 * for success.
 */
int BuildCommandLine(char *CmdLine, int MaxSize, THParams *HarnessParams, 
                     TestInfo testInfo)
{
    char *tstType;
    
    if(testInfo.Type == DEFAULT_TYPE) 
    {
        /* For a default Test, build the normal Full Path */

        if (AppendStringsVA(CmdLine, MaxSize, HarnessParams->TestCaseDir, "/",
            testInfo.Path, "/", testInfo.Executable1, NULL))
        {
            HarnessMessage("ERROR: Attempted to construct the full" 
                    " path, but it was too large for the "
                    "buffer.\n");
            return 1;
        }
    }
    else 
    {
        /* 
         * For types that aren't default, use the Xrun utility, and build a 
         * special path for it.  xrun test1 test2 <1|2>
         */
      
        /* Check the type, get the appropriate number */
        if(testInfo.Type == SERVER_TYPE) 
        {
            tstType = "1";
        }
        else if(testInfo.Type == CLIENT_TYPE) 
        {
            tstType = "2";
        }
        else 
        {
            HarnessMessage("ERROR: Type of test isn't valid.");
            return 1;
        }

        if (AppendStringsVA(CmdLine, MaxSize, 
            HarnessParams->XRunFile, " ",
            testInfo.Executable1, " ",
            testInfo.Executable2, " ",
            tstType, NULL))
        {
            HarnessMessage("ERROR: Attempted to construct the full" 
                    " path, but it was too large for the "
                    "buffer.\n");

            return 1;
        }
    }

    return 0;
}

/*
 * Runs the test case, logging the result.
 *
 * Returns: 0 on Success
 *          1 on Failure
 */
int RunAndLogTest(THParams *HarnessParams, TestInfo testInfo) 
{
    int nRetCode = 0;
    int ret;
	clock_t start,end;
	double timeTaken;

    start = clock();
	/* Build the full path, and execute it. */  
    if (BuildCommandLine(szCmdBuf, MAX_OUTPUT, HarnessParams, testInfo))
    {
        HarnessMessage("ERROR: Unable to build the complete commandline. "
                "The harness will continue, this test will be logged as "
                "failed.");
        LogCommandLineError(HarnessParams->TestResultsFile, testInfo.Path);
		WriteSumFile(HarnessParams->sumResultsFile,testInfo.Name,TEST_AREA,testInfo.Path,
		         TSTRESULT_FAIL,-1.00);
        return 1; /* FAIL */
    }

    memset(szOutputBuf,0,MAX_OUTPUT);
    ret = ExecuteTest(szCmdBuf, &nRetCode, szOutputBuf, MAX_OUTPUT);
	
	end = clock();
    timeTaken = ((double)(end-start))/ ( CLOCKS_PER_SEC / (double) 1000.0) ;

    /* Dump test case output for debugging */
    HarnessMessage(szOutputBuf);

    if (ret != 0 || nRetCode != 0)
    {
        LogResult(HarnessParams->TestResultsFile, testInfo.Section, 
            testInfo.SubSection, testInfo.Name, testInfo.Type, testInfo.Path,
            TSTPHASE_EXEC, TSTRESULT_FAIL, szOutputBuf);
		WriteSumFile(HarnessParams->sumResultsFile,testInfo.Name,TEST_AREA,testInfo.Path,
		             TSTRESULT_FAIL,timeTaken);
        return 1; /* FAIL */
    }

    LogResult(HarnessParams->TestResultsFile, testInfo.Section, 
              testInfo.SubSection,testInfo.Name, testInfo.Type, 
              testInfo.Path,TSTPHASE_EXEC, TSTRESULT_PASS, szOutputBuf);
	WriteSumFile(HarnessParams->sumResultsFile,testInfo.Name,TEST_AREA,testInfo.Path,
		         TSTRESULT_PASS,timeTaken);

    return 0;
}

const char *ExtensionsToClean[] = {
    ".exe", ".ilk", ".obj", ".pdb", ".dll", ".exp", ".lib", ".so", "", ".o", NULL
};

void CleanupExecutable(char *Executable)
{
    char FileName[MAX_PATH];
    char *pExtDestination;
    const char ** ppExt;

    if (NULL == Executable) {
	return;
    }

    strcpy(FileName, Executable);
    pExtDestination = FileName+strlen(FileName);
    for (ppExt = ExtensionsToClean; *ppExt; ++ppExt) {
	strcpy(pExtDestination, *ppExt);
	unlink(FileName);
    }
    unlink("vc70.pdb");
}


void CleanupTest(TestInfo *pTestInfo)
{
    memset(szOutputBuf,0,MAX_OUTPUT);
    CleanupExecutable(pTestInfo->Executable1);
    CleanupExecutable(pTestInfo->Executable2);
    CleanupExecutable(pTestInfo->Library1);
    CleanupExecutable(pTestInfo->Library2);
}

int BuildExecutable(const THParams *HarnessParams, char *Executable, TEST_LANG Language,
		    int fDll, int *pnRetCode)
{
    char CommandLine[4*MAX_PATH];

    if (NULL == Executable) {
	/* Nothing to build - return success */
	return 0;
    }

    /* build a command line of the form "cl /Fe foo.exe -I%TH_DIR%/common -I%ROTOR_DIR%/pal/inc -I.. foo.c"
       to compile foo.c into foo.exe */
    strcpy(CommandLine, (fDll) ? COMPILEANDLINKDLL : COMPILEANDLINKEXE);
    strcat(CommandLine, Executable);
    strcat(CommandLine, (fDll) ? DLLEXT : EXEEXT);
    strcat(CommandLine, " -I");
    strcat(CommandLine, getenv("TH_DIR"));
    strcat(CommandLine, "/common -I");
    strcat(CommandLine, getenv("ROTOR_DIR"));
    strcat(CommandLine, "/pal/inc -I.. ");

#ifdef WIN32
    if (Language == TEST_LANG_C) 
    {
        strcat(CommandLine, "/Tc ");
    }
    else if (Language == TEST_LANG_CPP)
    {
        strcat(CommandLine, "/EHsc /Tp ");
    }
#else
    if (Language == TEST_LANG_C) 
    {
        strcat(CommandLine, "-xc ");
    }
    else if (Language == TEST_LANG_CPP)
    {
        strcat(CommandLine, "-xc++ ");
    }
#endif

#ifndef WIN32
    strcat(CommandLine, getenv("LD_LIB_DIRS"));
    strcat(CommandLine, " ");

    strcat(CommandLine, COMPILEANDLINKLIBS);
    if (Language == TEST_LANG_CPP)
        strcat(CommandLine, " -lstdc++ ");
    else
        strcat(CommandLine, " ");

#endif
    strcat(CommandLine, Executable);
    strcat(CommandLine, ".c");

    /* Call the compiler to build the test case*/
    HarnessMessage("%s\n", CommandLine);
    fprintf(HarnessParams->TestResultsFile, "%s\n", CommandLine);
    return ExecuteTest(CommandLine, pnRetCode, szOutputBuf, MAX_OUTPUT);
}

int BuildTest(const THParams *HarnessParams, TestInfo *pTestInfo, int *pnRetCode)
{
    int ret;

    *pnRetCode = 0;
    memset(szOutputBuf,0,MAX_OUTPUT);
    ret = BuildExecutable(HarnessParams, pTestInfo->Executable1,
                          pTestInfo->Language, FALSE, pnRetCode);
    if (ret != 0 || *pnRetCode != 0) {
	/* Build failed */
	return ret;
    }
    ret = BuildExecutable(HarnessParams, pTestInfo->Executable2, 
                          pTestInfo->Language, FALSE, pnRetCode);
    if (ret != 0 || *pnRetCode != 0) {
	/* Build failed */
	return ret;
    }
    ret = BuildExecutable(HarnessParams, pTestInfo->Library1, 
                          pTestInfo->Language, TRUE, pnRetCode);
    if (ret != 0 || *pnRetCode != 0) {
	/* Build failed */
	return ret;
    }
    return BuildExecutable(HarnessParams, pTestInfo->Library2, 
			   pTestInfo->Language, TRUE, pnRetCode);
}

/*
 * This function is responsible for actually running a test case, given the 
 * relative path to that test.  It goes to the directory, reads in the 
 * testinfo.dat, builds the test, and runs it once or twice (depending on the 
 * test type).  The results are logged, and it finishs by cleaning up the test.
 */
int RunSingleTest(THParams *HarnessParams, char *RelativePath, int enabled )
{    
    int nRetCode, ret;
    int nTestResult = 0;
    TestInfo testInfo;
    int nBuildOk = 0;
	char *tempName;

    InitTestInfo(&testInfo);
    
    /* Build the Test Directory */    
    if (AppendStringsVA(szCmdBuf, LINE_BUF_SIZE, HarnessParams->TestCaseDir,
        "/", RelativePath, NULL))
    {
        HarnessMessage("ERROR: Attempted to construct the test case path, " 
                    "but it was too large for the buffer.\n");

        LogDirectoryError(HarnessParams->TestResultsFile, RelativePath); 
        return 1;  /* FAIL */
    }

    /* Change into the test directory */
    if (chdir(szCmdBuf) != 0) 
    {
        HarnessMessage("ERROR: Could not change into the test case "
                "directory for this test.\n");
        
        LogDirectoryError(HarnessParams->TestResultsFile, RelativePath); 
        return 1;  /* FAIL */
    }    

    /* Read in the testinfo.dat file */
    if (ReadTestInfo(&testInfo, RelativePath))
    {
        LogTestDatError(HarnessParams->TestResultsFile, RelativePath);
        FreeTestInfo(&testInfo);
        return 1;  /* FAIL */
    }
        
    /* Enabled test */
    if (enabled)
    {
        /* Print a separator line to keep the output neat & orderly */
        HarnessMessage("\n\n\n%s\n\n", SEPARATOR_LINE);

        /* Output the Building message */
        HarnessMessage("\n\nBUILDING: %s in %s (%s)\n%s\n", 
                       testInfo.SubSection, testInfo.Section,
                       RelativePath, UNDER_LINE);

        /* Clean up any leftovers from a previous run */
	CleanupTest(&testInfo);

        /* Build the test case*/
	ret = BuildTest(HarnessParams, &testInfo, &nRetCode);

        /* Check if the build failed */
        if (ret != 0 || nRetCode != 0)
        {
            LogResult(HarnessParams->TestResultsFile,testInfo.Section, 
                    testInfo.SubSection,testInfo.Name, 
                    testInfo.Type, RelativePath,
                    TSTPHASE_BUILD, TSTRESULT_FAIL,0);
                              
            LogResult(HarnessParams->TestResultsFile, testInfo.Section, 
                    testInfo.SubSection,testInfo.Name, 
                    testInfo.Type, RelativePath,
                    TSTPHASE_EXEC, TSTRESULT_FAIL, 0);

            HarnessMessage(szOutputBuf);

			if(testInfo.Type != CLNTSRV_TYPE) 
			{
			  WriteSumFile(HarnessParams->sumResultsFile,testInfo.Name,TEST_AREA,testInfo.Path,
		                 TSTRESULT_FAIL,-1.00);
			} 
			else
			{
			  tempName = (char*)malloc(strlen(testInfo.Name)+ 8);
			  strcpy(tempName,testInfo.Name);
			  strcat(tempName,"_CLIENT");
			  WriteSumFile(HarnessParams->sumResultsFile,tempName,TEST_AREA,testInfo.Path,
		                 TSTRESULT_FAIL,-1.00);
			  strcpy(tempName,testInfo.Name);
			  strcat(tempName,"_SERVER");
              WriteSumFile(HarnessParams->sumResultsFile,tempName,TEST_AREA,testInfo.Path,
		                 TSTRESULT_FAIL,-1.00);
			  free(tempName);

			}

            nBuildOk = 1; /* Fail */
        }
        else
        {
            /* Make passes, log success on build */
            LogResult(HarnessParams->TestResultsFile,testInfo.Section, 
                    testInfo.SubSection,testInfo.Name, 
                    testInfo.Type, RelativePath,
                    TSTPHASE_BUILD, TSTRESULT_PASS, 0);

            /* Dump the build output */
            HarnessMessage(szOutputBuf);
            nBuildOk = 0; /* Build succeeded */
        }

        /* Only run the test if the Build succeeded */
        if (nBuildOk == 0) 
        {
            /* Output the Testing message */
            HarnessMessage("\n\nTESTING: %s in %s (%s)\n%s\n",
                        testInfo.SubSection, testInfo.Section,
                        RelativePath, UNDER_LINE);

            /* If it is CLNTSRV, run it once with CLIENT as
            the type, once with SERVER. Otherwise, run just
            once and use the value of Type 
            */
            if(testInfo.Type == CLNTSRV_TYPE) 
            {
                testInfo.Type = CLIENT_TYPE;
                tempName = testInfo.Name;
				testInfo.Name = (char*)malloc(strlen(tempName)+ strlen("_CLIENT") + 1);
				strcpy(testInfo.Name,tempName);
				strcat(testInfo.Name,"_CLIENT");
				
                if (RunAndLogTest(HarnessParams, testInfo) != 0)
                {
                    nTestResult = 1; /* FAIL */
                }
                                      
                testInfo.Type = SERVER_TYPE;
				strcpy(testInfo.Name,tempName);
				strcat(testInfo.Name,"_SERVER");
				if (RunAndLogTest(HarnessParams, testInfo) != 0)
                {
                    nTestResult = 1; /* FAIL */
                }
				free(tempName);
            }
            else 
            {
                nTestResult = RunAndLogTest(HarnessParams, testInfo); 
            }
        }

        /* Output the Cleaning message */
        HarnessMessage("\n\nCLEANING: %s in %s (%s)\n%s\n",
                       testInfo.SubSection, testInfo.Section,
                       RelativePath, UNDER_LINE);

        /* Clean up the test binaries */
    	CleanupTest(&testInfo);

        /* Dump the clean output */
        HarnessMessage(szOutputBuf);
    }
    /* Disabled test */
    else
    {
        LogResult(HarnessParams->TestResultsFile,testInfo.Section, 
                  testInfo.SubSection,testInfo.Name, 
                  testInfo.Type, RelativePath,
                  TSTRESULT_DISABLED, NULL, NULL);
		
    }

    FreeTestInfo(&testInfo);
    return (nBuildOk || nTestResult);
}


/*
 * Does all the work. Initializes the harness, runs the tests, and cleans up.
 */
int main(int argc, char **argv) 
{    
    THParams CurrentParams;
    TestconfigParser Parser;
    char *szRelativePath;
    int Enabled;
    int iFailCnt = 0;
    int iRet;

    /* Read the required environment variables */
    ReadParams(&CurrentParams);  
    
    szRelativePath = (char *)malloc(LINE_BUF_SIZE);
    if (szRelativePath == 0)
    {
        HarnessError("Unable to allocate path storage.");
    }

    if (InitTestconfigParser(&Parser, CurrentParams.TestConfigName))
    {
        HarnessError("Unable to intialaize Parser.");
    }

    /*
     * Start the main loop, continue until the entire
     * Test Harness config file has been read & parsed
     */
    while (1)
    {
        iRet = GetNextTestCase(&Parser,szRelativePath, &Enabled,
			       CurrentParams.SubTest);

        if (iRet == 1)
        {
            /* No more test cases, break out of loop */
            break;
        }

        if (iRet == 2)
        {
            /* Bad data detected in test cases config file. */
            LogConfigFileError(CurrentParams.TestResultsFile,szRelativePath);
			iFailCnt++;
            iRet = 0;
            continue;
        }
	
        if (RunSingleTest(&CurrentParams, szRelativePath, Enabled))
        {
            iFailCnt++;
            HarnessMessage("Error returned\n");
        }
    }
    
    free(szRelativePath);
    FreeTestconfigParser(&Parser);
    FreeTestParams(&CurrentParams);
    
    return iFailCnt;
}



