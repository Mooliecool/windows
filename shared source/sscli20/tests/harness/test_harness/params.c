/*============================================================
**
** Source: params.c
**
** Purpose: Contains the functions to deal with the parameters for the
**          execution of the testharness.
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
**=========================================================*/

#include "testharness.h"
#include "error.h"
#include "params.h"

/*
 * Initialize the members of a THParams structure.
 */
void InitTestParams(THParams *HarnessParams)
{
    HarnessParams->TestCaseDir = NULL;
    HarnessParams->XRunFile = NULL;
    HarnessParams->TestConfigName = NULL;
    HarnessParams->TestResultsName = NULL;
	HarnessParams->sumResultsName = NULL;
    HarnessParams->SubTest = NULL;

    HarnessParams->TestResultsFile = NULL;
	HarnessParams->sumResultsFile = NULL;

}

/*
 * Clean up after THParams structure.  Closes the config and results files.
 */
void FreeTestParams(THParams *HarnessParams)
{
    if (HarnessParams == NULL)
    {
        return;
    }


    free(HarnessParams->TestCaseDir);
    HarnessParams->TestCaseDir = NULL;

    free(HarnessParams->XRunFile);
    HarnessParams->XRunFile = NULL;

    free(HarnessParams->TestConfigName);
    HarnessParams->TestConfigName = NULL;

    free(HarnessParams->TestResultsName);
    HarnessParams->TestResultsName = NULL;

    free(HarnessParams->SubTest);
    HarnessParams->SubTest = NULL;

    fclose(HarnessParams->TestResultsFile);
    HarnessParams->TestResultsFile = NULL;

	fclose(HarnessParams->sumResultsFile);
	HarnessParams->sumResultsFile = NULL;

}


/*
 * Read the parameters for the testharness from the environment.
 */
void GetEnvironmentParams(THParams *HarnessParams)
{
    char *envString;

    envString = getenv(ENV_DIR);
    if (envString != NULL)
    {
        HarnessParams->TestCaseDir = strdup(envString);
        if (HarnessParams->TestCaseDir == NULL)
        {
            HarnessError("ERROR: Unable to allocate memory!\n");
        }
    }
    else
    {
        envString = getenv(ENV_DIR_ALT);
        if (envString != NULL)
        {
            HarnessParams->TestCaseDir = strdup(envString);
            if (HarnessParams->TestCaseDir == NULL)
            {
                HarnessError("ERROR: Unable to allocate memory!\n");
            }
        }
    }

    envString = getenv(ENV_CONFIG);
    if (envString != NULL)
    {
        HarnessParams->TestConfigName = strdup(envString);
        if (HarnessParams->TestConfigName == NULL)
        {
            HarnessError("ERROR: Unable to allocate memory!\n");
        }
    }

    envString = getenv(ENV_RESULTS);
    if (envString != NULL)
    {
        HarnessParams->TestResultsName = strdup(envString);
        if (HarnessParams->TestResultsName == NULL)
        {
            HarnessError("ERROR: Unable to allocate memory!\n");
        }
    }

    envString = getenv(ENV_SUMRES);
    if (envString != NULL)
	{
		HarnessParams->sumResultsName = strdup(envString);
		if (HarnessParams->sumResultsName == NULL)
		{
			HarnessError("ERROR: Unable to allocate memory!\n");
		}
	}

	envString = getenv(ENV_XRUN);
    if (envString != NULL)
    {
        HarnessParams->XRunFile = strdup(envString);
        if (HarnessParams->XRunFile == NULL)
        {
            HarnessError("ERROR: Unable to allocate memory!\n");
        }
    }

    envString = getenv(ENV_SUBTEST);
    if (envString != NULL)
    {
        int subLen=0;
        HarnessParams->SubTest = strdup(envString); 

        if (HarnessParams->SubTest == NULL)
        {
            HarnessError("ERROR: Unable to allocate memory!\n");
        }

	subLen = strlen(envString);

        if(HarnessParams->SubTest[subLen-1]=='\\' ||
           HarnessParams->SubTest[subLen-1]=='/')

	{
	    HarnessParams->SubTest[subLen-1]='\0';
	}

    }
    else
    {
        HarnessParams->SubTest=NULL;
    }

}

void ReadParams(THParams *HarnessParams)
{

    GetEnvironmentParams(HarnessParams);

    if (HarnessParams->TestCaseDir == NULL)
    {
        HarnessError("ERROR: No test suite directory specified.  Set the"
            " %s (or %s) environment variable to the appropriate directory.\n", 
            ENV_DIR, ENV_DIR_ALT);
    }
    if (HarnessParams->TestConfigName == NULL)
    {
        HarnessError("ERROR: No test configuration file specified.  Set the"
            " %s environment variable to the appropriate directory.\n",
            ENV_CONFIG);
    }
    if (HarnessParams->TestResultsName == NULL)
    {
        HarnessError("ERROR: No test results file specified.  Set the"
            " %s environment variable to the appropriate directory.\n",
            ENV_RESULTS);
    }
    if (HarnessParams->XRunFile == NULL)
    {
        HarnessError("ERROR: The XRun application was not specified.  Set the"
            " %s environment variable to the appropriate directory.\n",
            ENV_XRUN);
    }
  
    /* Go to the test suite directory */
    if (chdir(HarnessParams->TestCaseDir) != 0)
    {
        HarnessError("ERROR: Failed to change into the directory "
                "specified by TH_TC_DIR.  Ensure that TH_TC_DIR is correctly "
                "set to the root of your test suite.\n");
    }


    /* Open the results file */    
    HarnessParams->TestResultsFile = fopen(HarnessParams->TestResultsName,"w");
    if (HarnessParams->TestResultsFile == 0)
    {
        HarnessError("ERROR: Failed to open test results file '%s'.\n",
            HarnessParams->TestResultsName);
    }

	HarnessParams->sumResultsFile = fopen(HarnessParams->sumResultsName,"w");
	if (HarnessParams->sumResultsFile == 0)
	{
		HarnessError("ERROR: Failed to open summary results file '%s'.\n",
			HarnessParams->sumResultsName);
	}

}







