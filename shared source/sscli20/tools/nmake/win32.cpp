// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
#include "precomp.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif


#ifdef _M_IX86

UCHAR
FIsChicago(void)
{
    return 0;  // The PAL is Windows NT only
}

#endif


char *_pgmptr; // initialized in main() to be argv[0]

//
// The following functions are implementations of MS VC runtime
// APIs, built on top of the PAL
//

time_t ConvertFileTime(FILETIME *ft)
{
    // this routine was cloned from pal/unix/file/filetime.c
    const __int64 SECS_BETWEEN_EPOCHS = I64(11644473600);
    const __int64 SECS_TO_100NS = I64(10000000); /* 10^7 */

    __int64 UnixTime;

    /* get the full win32 value, in 100ns */
    UnixTime = ((__int64)ft->dwHighDateTime << 32) + 
        ft->dwLowDateTime;

    /* convert to the Unix epoch */
    UnixTime -= (SECS_BETWEEN_EPOCHS * SECS_TO_100NS);

    UnixTime /= SECS_TO_100NS; /* now convert to seconds */

    return (time_t)UnixTime;
}

void ConvertFindData(struct _finddata_t *f, LPWIN32_FIND_DATAA wf)
{
    f->attrib = wf->dwFileAttributes;
    f->time_create = ConvertFileTime(&wf->ftCreationTime);
    f->time_access = ConvertFileTime(&wf->ftLastAccessTime);
    f->time_write = ConvertFileTime(&wf->ftLastWriteTime);
    f->size = wf->nFileSizeLow;
    strcpy(f->name, wf->cFileName);
}

intptr_t __cdecl _findfirst(const char *name, struct _finddata_t *f)
{
    WIN32_FIND_DATAA wf;
    HANDLE wh;

    wh = FindFirstFileA(name, &wf);
    if (wh == INVALID_HANDLE_VALUE) {
        return -1;
    }
    ConvertFindData(f, &wf);
    return (intptr_t)wh;
}

int __cdecl _findnext(intptr_t h, struct _finddata_t *f)
{
    WIN32_FIND_DATAA wf;
    BOOL b;

    b = FindNextFileA((HANDLE)h, &wf);
    if (b == FALSE) {
        return -1;
    }
    ConvertFindData(f, &wf);
    return 0;
}

int __cdecl _findclose(intptr_t h)
{
    BOOL b;

    b = FindClose((HANDLE)h);
    if (b) {
        return 0;
    } else {
        return -1;
    }
}

int __cdecl _access(const char *path, int mode)
{
    FILE *fp;

    fp = fopen(path, "r");
    if (fp == NULL) {
        return -1;
    }
    fclose(fp);
    return 0;
}

char *  __cdecl _strupr(char *s)
{
    char *c = s;

    while (*c) {
        *c = toupper(*c);
        c++;
    }
    return s;
}

// 6 for "errno=", 11 for errno (length of max int and '-' sign), plus '\n''\0'
static char StrErrorBuffer[6+11+2];

char * __cdecl _strerror(const char *strErrMsg)
{
    _snprintf(StrErrorBuffer, sizeof(StrErrorBuffer), "errno=%d\n", errno);
    return StrErrorBuffer;
}

int __cdecl _spawnvp(int mode, const char *cmdname, const char * const * argv)
{
    char *command;
    int commandLength;
    int i;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD ExitCode;

    // Find out how long command should be.
    commandLength = strlen(cmdname) + 1;
    for(i = 0; argv[i] != NULL; i++) {
#if PLATFORM_UNIX
      // On Unix, we're going to escapte the double quotes
      // Unless it's the last argument (closing quote for bin/sh)
      // and once we're past the opending quote
      if (i > 2 && argv[i][0] == '\"' && argv[i+1] != NULL )
      {
          commandLength += 2;
      }
#endif
        commandLength++;    // For the space before the command
        commandLength += strlen(argv[i]);
    }

    // Allocate command, then copy into it.
    command = (char *) malloc(commandLength);
    if (command == NULL) {
        errno = ENOMEM;
        return -1;
    }

    strcpy(command, cmdname);
    for(i = 0; argv[i] != NULL; i++) {
        strcat(command, " ");
#if PLATFORM_UNIX
        if (i > 2 && argv[i][0] == '\"' && argv[i+1] != NULL )
        {
            strcat(command, "\\");
        }
#endif

        strcat(command, argv[i]);
#if PLATFORM_UNIX
        if (i > 2 && argv[i][0] == '\"' && argv[i+1] != NULL )
        {
            size_t length = strlen(command);
            command[length-1] = '\\';
            command[length] = '\"';
            command[length+1] = '\0';
        }
#endif
    }

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    if (CreateProcess(NULL,
                      command,
                      NULL, // lpProcessAttributes,
                      NULL, // lpThreadAttributes,
                      TRUE, // bInheritHandles
                      0,    // dwCreationFlags
                      NULL, // lpEnvironment
                      NULL, // lpCurrentDirectory
                      &si,
                      &pi) == FALSE) {
        free(command);
        return -1;
    }
    CloseHandle(pi.hThread);
    if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0) {
        ExitCode = (DWORD)-1;
    } else if (!GetExitCodeProcess(pi.hProcess, &ExitCode)) {
        ExitCode = (DWORD)-1;
    }
    CloseHandle(pi.hProcess);
    free(command);
    return (int)ExitCode;
}

int __cdecl putc(int c, FILE *s)
{
    char ch;

    ch = (char)c;
    return fwrite(&ch, 1, 1, s);
}


