/*============================================================
**
** Source: testinfo.c
**
**
** Purpose: Various functions to deal with the testinfo.dat files with each
** test case.
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

#include <ctype.h>

#include "testharness.h"
#include "error.h"
#include "testinfo.h"
#include "util.h"

#define INFO_VERSION( major, minor) ((major << 16) | minor)

/*
 * Initialize the members of a TestInfo structure.
 */
void InitTestInfo(TestInfo *pTestInfo)
{
    pTestInfo->Path = NULL;
    pTestInfo->Section = NULL;
    pTestInfo->SubSection = NULL;
    pTestInfo->Name = NULL;
    pTestInfo->Desc = NULL;
    pTestInfo->Executable1 = NULL;
    pTestInfo->Executable2 = NULL;
    pTestInfo->Library1 = NULL;
    pTestInfo->Library2 = NULL;

    pTestInfo->Type = UNKNOWN_TYPE;
    pTestInfo->Language = TEST_LANG_UNKNOWN;
}

/*
 * Clean up a TestInfo structure. Free all the strings.
 */
void FreeTestInfo(TestInfo *pTestInfo)
{
    if (pTestInfo == 0)
    {
        return;
    }
  
    if (pTestInfo->Path != 0)
    {
        free(pTestInfo->Path);
        pTestInfo->Path = 0;
    }

    if (pTestInfo->Section != 0)
    {
        free(pTestInfo->Section);
        pTestInfo->Section = 0;
    }
  
    if (pTestInfo->SubSection != 0)
    {
        free(pTestInfo->SubSection);
        pTestInfo->SubSection = 0;
    }
  
    if (pTestInfo->Name != 0)
    {
        free(pTestInfo->Name);
        pTestInfo->Name = 0;
    }
  
    if (pTestInfo->Desc != 0)
    {
        free(pTestInfo->Desc);
        pTestInfo->Desc = 0;
    }
  
    if (pTestInfo->Executable1 != 0)
    {
        free(pTestInfo->Executable1);
        pTestInfo->Executable1 = 0;
    }
  
    if (pTestInfo->Executable2 != 0)
    {
        free(pTestInfo->Executable2);
        pTestInfo->Executable2 = 0;
    }

    if (pTestInfo->Library1 != 0)
    {
	free(pTestInfo->Library1);
	pTestInfo->Library1 = 0;
    }
}

/*
 * Helper function to convert a string to lower case. I can't recall if there 
 * is a cross-platform function that does this already.
 */
void Lower(char *str)
{

    if (str == NULL)
    {
        return;
    }

    while (*str != 0)
    {
        *str = tolower(*str);
        str++;
    }
}

/*
 * Gets the key part of the buffer. If no valid key was
 * found, no error is returned and *key is null
 *
 * Returns: 0 -> no error
 *          1 -> error
 */
int GetKey( char *buffer, char **key )
{
    char *tmpValue;
    char *keyStart;
    char *keyEnd;

    int len;

    /* Sanity checks*/
    if( (buffer == NULL) ||
        (strlen(buffer) <= 0) ||
        (key == NULL) )
    {
        return 1;
    }

    /* Check for an equal sign*/
    tmpValue = strchr( buffer, '=' );
    if( tmpValue == NULL )
    {
        /* No =, it must be a key alone field 
           so assume the whole buffer is the key */
        len = strlen( buffer);
    }
    else
    {
        len = tmpValue - buffer;  
    }

    keyStart = buffer;

    /* Parse and allocate the value, stripping off whitespace from the ends */
    keyEnd = keyStart + len - 1;

    while (isspace((unsigned char) *keyStart))
    {
        keyStart++;
    }

    while (isspace((unsigned char) *keyEnd))
    {
        keyEnd--;
    }
    
    len = keyEnd - keyStart + 1;

    if (len < 0)
    {
        *key = NULL;
        return 0;
    }

    *key = malloc( len + 1 );
    strncpy( *key, keyStart, len );
    (*key)[len] = 0;
 
    Lower( *key );

    return 0;
}
/*
 * Gets the value part of the buffer. If no valid value was
 * found, no error is returned and *value is null
 * 
 * Returns: 0 -> no error
 *          -1 -> error
 */
int GetValue( char *buffer, char **value )
{
    char *valueStart;
    char *valueEnd;
    int len;

    /* Sanity checks*/
    if( (buffer == NULL) ||
        (strlen(buffer) <= 0) ||
        (value == NULL) )
    {
        return -1;
    }

    /* Check for an equal sign*/
    valueStart = strchr( buffer, '=' );
    if( valueStart == NULL )
    {
        /* This buffer doesn't contain any value */
        *value = NULL;
        return 0;
    }

    /* Advance to first character of "value" */
    valueStart++;

    /* Parse and allocate the value, stripping off whitespace from the ends */
    valueEnd = valueStart + strlen(valueStart) - 1;

    while (isspace((unsigned char) *valueStart))
    {
        valueStart++;
    }

    while (isspace((unsigned char) *valueEnd))
    {
        valueEnd--;
    }
    
    len = valueEnd - valueStart + 1;

    if (len < 0)
    {
        *value = NULL;
        return 0;
    }
    *value = malloc( len + 1 );
    strncpy( *value, valueStart, len );
    (*value)[len] = 0;

    return 0;
}

/*
 * Gets the next valid key and/or value from testinfo.dat file.
 *
 * A valid key is when you have: key
 *                               key = value
 *
 * A valid value is when you have: key = value
 *                                 = value 
 *
 *  return: 0 -> success
 *               key or value could be NULL
 *          1 -> failure
 *               or NULL key and value
 */
int GetNextKeyAndValue( FILE *FInfo, 
                        char *lnBuffer, 
                        char **key, 
                        char **value )
{
    int retKey;
    int retValue;

    /* Skip all lines that start with a # char */
    do 
      {
      if( ReadLineEx(FInfo, lnBuffer) != 0 )
	{
	return 1;
	}
      } while (lnBuffer[0] == '#');

    /* Get the key and value from lnBuffer */
    retKey   = GetKey(lnBuffer, key);
    retValue = GetValue(lnBuffer, value);

    /* Validate the results */
    if( ((0 != retKey ) || (key == NULL)) &&
        ((0 != retValue ) || (value == NULL)) )
    {
        /* Try to get a valid key */     
        return GetNextKeyAndValue(FInfo, lnBuffer, key, value);
    }
    return 0;
}

/*
 * Assigns the value of the key in the TestInfo struct
 *
 * Note: key must be in lowercase
 *
 * returns: 0 -> no error
 *          1 -> error occured
 */
int AssignKeyAndValue(TestInfo *pInfo, char *key, char *value)
{
    static int bInDescription = FALSE;

    /* Sanity checks */
    if( pInfo == NULL )
    {
        return 1;
    }

    /* When the key is NULL, check if we are in Description
       mode, if yes, save the value */
    if( key == NULL )
    {
        if( bInDescription == TRUE )
        {
            if( (value != NULL) && 
                (strlen(value) > 0))
            {
                char *newDesc;
                newDesc = malloc(strlen(pInfo->Desc) + strlen(value) + 2);
                
                if (newDesc == NULL)
                {
                    HarnessError("ERROR: Unable to allocate memory!\n");
                    return 1;
                }
                strcpy(newDesc, pInfo->Desc);
                strcat(newDesc, "\n");
                strcat(newDesc, value);

                free(pInfo->Desc);
                free(value);

                pInfo->Desc = newDesc;
            } 
        }
        else
        {
            /* Invalid Key */
            return 1;
        }
    }
    /* Determine which field this is for */
    else if (strcmp(key, "description") == 0)
    {
        if( bInDescription == TRUE )
        {
            /* Already been here */
            HarnessMessage("\nDuplicated key! Key: DESCRIPTION\n");
            return 1;
        }

        pInfo->Desc = malloc(1);
        if (pInfo->Desc == NULL)
        {
            HarnessError("ERROR: Unable to allocate memory!\n");
            return 1;
        }
        /* Special case for description */
        /* Just set the field to 0 to indicate 
           that we got the description key */
        *pInfo->Desc = 0;

        bInDescription = TRUE;

    }
    else if (strcmp(key, "section") == 0)
    {
        /* Not the Description mode */
        bInDescription = FALSE;

        if(pInfo->Section != NULL)
        {
            HarnessMessage("\nDuplicated key! Key: SECTION\n");
            return 1;
        }
        pInfo->Section = value;
    }
    else if (strcmp(key, "function") == 0)
    {    
        /* Not the Description mode */
        bInDescription = FALSE;

        if(pInfo->SubSection != NULL)
        {
            HarnessMessage("\nDuplicated key! Key: FUNCTION\n");
            return 1;
        }
        pInfo->SubSection = value;
    }
    else if (strcmp(key, "name") == 0)
    {     
        /* Not the Description mode */
        bInDescription = FALSE;

        if(pInfo->Name != NULL)
        {
            HarnessMessage("\nDuplicated key! Key: NAME\n");
            return 1;
        }
        pInfo->Name = value;
    }
    else if (strcmp(key, "type") == 0)
    {     
        /* Not the Description mode */
        bInDescription = FALSE;
      
        if(pInfo->Type != UNKNOWN_TYPE)
        {
            HarnessMessage("\nDuplicated key! Key: TYPE\n");
            return 1;
        }
        pInfo->Type = StringToType(value);
        free(value);
    }
    else if (strcmp(key, "exe1") == 0)
    {     
        /* Not the Description mode */
        bInDescription = FALSE;

        if(pInfo->Executable1 != NULL)
        {
            HarnessMessage("\nDuplicated key! Key: EXE1\n");
            return 1;
        } 
        pInfo->Executable1 = value;
    }
    else if (strcmp(key, "exe2") == 0)
    {     
        /* Not the Description mode */
        bInDescription = FALSE;

        if(pInfo->Executable2 != NULL)
        {
            HarnessMessage("\nDuplicated key! Key: EXE2\n");
            return 1;
        } 
        pInfo->Executable2 = value;
    }
    else if (strcmp(key, "lang") == 0)
    {     
        /* Not the Description mode */
        bInDescription = FALSE;

        if(pInfo->Language != TEST_LANG_UNKNOWN)
        {
            HarnessMessage("\nDuplicated key! Key: LANG\n");
            return 1;
        } 
        pInfo->Language = StringToLang(value);
    }
    else if (strcmp(key, "lib1") == 0)
    {
        /* Not the Description mode */
        bInDescription = FALSE;

        if(pInfo->Library1 != NULL)
        {
            HarnessMessage("\nDuplicated key! Key: LIB1\n");
            return 1;
        } 
        pInfo->Library1 = value;
    }
    else if (strcmp(key, "lib2") == 0)
    {
        /* Not the Description mode */
        bInDescription = FALSE;

        if(pInfo->Library2 != NULL)
        {
            HarnessMessage("\nDuplicated key! Key: LIB1\n");
            return 1;
        } 
        pInfo->Library2 = value;
    }
    else
    {     
        /* Not the Description mode */
        bInDescription = FALSE;

        return 1;
    }

    return 0;
}


/*
 * Parses the test file with the key, value format.
 * Returns 1 on failure, 0 on success.
 */
int ParseTestInfo(FILE *FInfo, TestInfo *pInfo)
{
    char *key;
    char *value;
    char lnBuffer[LINE_BUF_SIZE];

    /* Make sure we are at the beginning of the file */
    if( 0 != fseek( FInfo, 0L, SEEK_SET ) )
    {
        return 1;
    }

    /* Get, analyse and save all valid keys and their associated value */
    while( GetNextKeyAndValue(FInfo, lnBuffer, &key, &value) != 1 )
    {
        /* We don't really care if AssignKeyAndValue fails or not */
        AssignKeyAndValue(pInfo, key, value);
    }

    return 0;
}


/*
 * Reads in a key value pair for the version.  Places the version in the int
 * pointed to by pVersion.  Returns 0 on success, 1 on failure.
 */
int GetFileVersion(FILE *FInfo, int *pVersion)
{
    char *key   = NULL;
    char *value = NULL;
    char lnBuffer[LINE_BUF_SIZE];
    int major;
    int minor;

    /* Make sure we are at the beginning of the file */
    if( 0 != fseek( FInfo, 0L, SEEK_SET ) )
    {
        return 1;
    }

    /* Go through all file to find the version key*/
    while( GetNextKeyAndValue(FInfo, lnBuffer, &key, &value) != 1 )
    {
        if( key != NULL )
        {
            /* Check for the version key*/
            if( strcmp(key, "version") == 0 )
            {
                if( value == NULL )
                {
                    free(key);
                    return 1;
                }

                if (sscanf(value, "%d.%d", &major, &minor) != 2)
                {
                    free(key);
                    free(value);
                    return 1;
                }

                *pVersion = INFO_VERSION(major, minor);

                free(value);
            }
            free(key);  
        }
    }
    return 0;
}

/*
 * Makes sure all the fields of the structure are present.
 * Returns 0 on success, 1 on fail
 */
int ValidateTestInfoFields(TestInfo *pInfo)
{
    if (pInfo->Section == 0)
    {
        HarnessMessage("Missing Section field in testinfo.dat\n");
        return 1;
    }
    else if (pInfo->SubSection == 0)
    {
        HarnessMessage("Missing Subsection field in testinfo.dat\n");
        return 1;
    }
    else if (pInfo->Name == 0)
    {
        HarnessMessage("Missing Name field in testinfo.dat\n");
        return 1;
    }
    else if (pInfo->Type == UNKNOWN_TYPE)
    {
        HarnessMessage("Missing Type field in testinfo.dat\n");
        return 1;
    }
    else if (pInfo->Executable1 == 0)
    {
        HarnessMessage("Missing Exe1 field in testinfo.dat\n");
        return 1;
    }
    else if (pInfo->Executable2 == 0 && pInfo->Type != DEFAULT_TYPE)
    {
        HarnessMessage("Missing Exe2 field in testinfo.dat\n");
        return 1;
    }
    else if (pInfo->Desc == 0)
    {
        HarnessMessage("Missing Description in testinfo.dat\n");
        return 1;
    }

    /* Set the default language if it is not set */
    if (pInfo->Language == TEST_LANG_UNKNOWN)
    {
        pInfo->Language = TEST_LANG_C;
    }

    return 0;
}

/*
 * Reads and parses a testinfo.dat file from the current working directory, 
 * placing the results TestInfo pointed to by pInfo.  RelativePath should be
 * the path from the suite directory to the test, without a terminating dir
 * seperator.  Returns 0 on success, 1 on failure. 
 *
 * Note: the function will still return success when the file is finished.
 */
int ReadTestInfo(TestInfo *pInfo, char *RelativePath)
{
    FILE *FInfo;
    int Version;

    FreeTestInfo(pInfo);

    pInfo->Path = strdup(RelativePath);
    if (pInfo->Path == NULL)
    {
        HarnessError("ERROR: Unable to allocate memory!\n");
    }

    /* Open the testinfo.dat file */
    FInfo = fopen(TEST_INFO_FILE, "r");
    if (FInfo == 0)
    {
        HarnessMessage("ERROR: Unable to open '%s' in '%s'!\n", 
            TEST_INFO_FILE, RelativePath);
        return 1;
    }

    /* Verify testinfo.dat version */
    if (GetFileVersion(FInfo, &Version))
    {
        HarnessMessage("Unable to determine verion of %s!\n", TEST_INFO_FILE);
        return 1;
    }
    else if (Version == INFO_VERSION(1, 0))
    {
        if (ParseTestInfo(FInfo, pInfo) != 0)
        {
            return 1;
        }
    }
    else
    {
        HarnessMessage("Unsupported version of %s: %d!\n", 
            TEST_INFO_FILE, Version);
        return 1;
    }

    /* Validate testinfo.dat fields */
    if (ValidateTestInfoFields(pInfo))
    {
        return 1;
    }
      

    /* Close the testinfo.dat file */
    fclose(FInfo);
    return 0;
}

