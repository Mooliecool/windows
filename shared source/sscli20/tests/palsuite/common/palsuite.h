/*============================================================================
**
** Source:  palsuite.h
**
** Purpose: Define constants and implement functions that are useful to
**          multiple function categories. If common functions are useful
**          only amongst the test cases for a particular function, a separate
**          header file is placed in the root of those test cases.
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
**==========================================================================*/

#ifndef __PALSUITE_H__
#define __PALSUITE_H__

#include <rotor_pal.h>

/* Environment Variable Constants */
const char *szTestPortEnvVar = "ROTOR_TEST_PORT";
const int nDefaultRotorTestPort = 3333;

const char *szTestHostEnvVar = "ROTOR_TEST_HOST";
const char *szDefaultRotorTestHost = "gotdotnet.com";


enum
{
    PASS = 0,
    FAIL = 1
};


void Trace(char *format, ...)
{
    va_list arglist;
	
    va_start(arglist, format);

    vprintf(format, arglist);
}

void Fail(char *format, ...)
{
    va_list arglist;
	
    va_start(arglist, format);

    vprintf(format, arglist);    
    printf("\n");

    PAL_Terminate();

    exit(FAIL);
}


#ifdef BIGENDIAN
inline ULONG   VAL32(ULONG x)
{
    return( ((x & 0xFF000000L) >> 24) |               
            ((x & 0x00FF0000L) >>  8) |              
            ((x & 0x0000FF00L) <<  8) |              
            ((x & 0x000000FFL) << 24) );
}
#define th_htons(w)  (w)
#else   // BIGENDIAN
#define VAL32(x)    (x)
#define th_htons(w)  (((w) >> 8) | ((w) << 8))
#endif  // BIGENDIAN

WCHAR* convert(char * aString) 
{
    int size;
    WCHAR* wideBuffer;

    size = MultiByteToWideChar(CP_ACP,0,aString,-1,NULL,0);
    wideBuffer = (WCHAR*) malloc(size*sizeof(WCHAR));
    if (wideBuffer == NULL)
    {
        Fail("ERROR: Unable to allocate memory!\n");
    }
    MultiByteToWideChar(CP_ACP,0,aString,-1,wideBuffer,size);
    return wideBuffer;
}

char* convertC(WCHAR * wString) 
{
    int size;
    char * MultiBuffer = NULL;

    size = WideCharToMultiByte(CP_ACP,0,wString,-1,MultiBuffer,0,NULL,NULL);
    MultiBuffer = (char*) malloc(size);
    if (MultiBuffer == NULL)
    {
        Fail("ERROR: Unable to allocate memory!\n");
    }
    WideCharToMultiByte(CP_ACP,0,wString,-1,MultiBuffer,size,NULL,NULL);
    return MultiBuffer;
}

unsigned short getRotorTestPort()
{
    char *szPort;
    int iTemp;

    szPort = getenv(szTestPortEnvVar);
    if (szPort == NULL)
    {
        return th_htons((unsigned short)nDefaultRotorTestPort);
    }

    iTemp = atoi(szPort);
    return th_htons((unsigned short)iTemp);
}

const char *getRotorTestHost()
{
    static char szHost[256];
    char *szTemp;

    szTemp = getenv(szTestHostEnvVar);
    if (szTemp == NULL)
    {
        strcpy(szHost, szDefaultRotorTestHost);
    }
    else
    {
        strcpy(szHost, szTemp);
    }

    return szHost;
}

#endif



