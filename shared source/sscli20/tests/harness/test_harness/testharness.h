/*============================================================
**
** Header: testharness.h
**
** Purpose: Primary header file for test harness.
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
 
#ifndef _TESTHARNESS_H 
#define _TESTHARNESS_H 

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <time.h>
#include <math.h>

#define COMPILEANDLINKEXE "cl /nologo /DWIN32 /D_X86_=1 /Di386=1 /DPAL_PORTABLE_SEH=1 /Gz /Zi /Zl /Od /W3 rotor_pal.lib msvcrtd.lib /Fe"
#define COMPILEANDLINKDLL "cl /nologo /DWIN32 /D_X86_=1 /Di386=1 /DPAL_PORTABLE_SEH=1 /Gz /Zi /Zl /Od /W3 rotor_pal.lib msvcrtd.lib /LD /Fe"
#define EXEEXT ".exe"
#define DLLEXT ".dll"
#define EOLN "\n"


#else   // WIN32

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>


#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "harness_comp_commands.h"
#define EOLN "\015\012"

#define EXEEXT ""

#endif  // WIN32

#include <errno.h>
#define READ_BUF_SIZE       8192
#define LINE_BUF_SIZE       1024

#define MAX_OUTPUT          2048

#ifndef MAX_PATH
#define MAX_PATH            255
#endif

#define TSTTYPE_UNKNOWN     "unknown"
#define TSTTYPE_DEFAULT     "DEFAULT"
#define TSTTYPE_CLIENT      "CLIENT"
#define TSTTYPE_SERVER      "SERVER"
#define TSTTYPE_CLNTSRV     "CLNTSRV"

typedef enum 
{
    UNKNOWN_TYPE = 0,
    DEFAULT_TYPE,
    CLIENT_TYPE,
    SERVER_TYPE,
    CLNTSRV_TYPE,    
} TEST_TYPE;

#define TSTLANG_UNKNOWN    "unknown"
#define TSTLANG_C          "c"
#define TSTLANG_CPP        "cpp"

typedef enum
{
    TEST_LANG_UNKNOWN = 0,
    TEST_LANG_C,
    TEST_LANG_CPP
} TEST_LANG;

#define TEST_INFO_FILE      "testinfo.dat"

#define TEST_AREA "palsuite"

#define TSTPHASE_BUILD     "BUILD"
#define TSTPHASE_EXEC       "EXEC"

#define TSTRESULT_PASS      "PASS"
#define TSTRESULT_FAIL      "FAIL"
#define TSTRESULT_DIRERR    "DIR_ERROR"
#define TSTRESULT_NOINFO    "NO_INFO"
#define TSTRESULT_DISABLED  "DISABLED"
#define TSTRESULT_CFG_ERROR "CONFIG_ERROR"

#define SZ_BLANKDATA    "-"
#define C_ESCAPE        '\\'
#define C_COMMA         ','

#define ENV_DIR       "TH_DIR"
#define ENV_DIR_ALT   "TH_TC_DIR"
#define ENV_CONFIG    "TH_CONFIG"
#define ENV_XRUN      "TH_XRUN"
#define ENV_RESULTS   "TH_RESULTS"
#define ENV_SUBTEST   "TH_SUBTEST"
#define ENV_SUMRES    "TH_SUMRES"

#define PLATFORM  getenv("PAL_PLATFORM")
#define BUILDTYPE getenv("PAL_BUILDTYPE")
#define BUILDNUMBER getenv("PAL_BUILDNUMBER")
#endif /* _TESTHARNESS_H */

