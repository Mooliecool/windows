/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    process.c

Abstract:

    Implementation of functions related to processes.

--*/

#include "pal/palinternal.h"

#include "pal/handle.h"
#include "pal/file.h"
#include "pal/init.h"
#include "pal/critsect.h"
#include "pal/thread.h"
#include "pal/dbgmsg.h"
#include "pal/virtual.h"
#include "pal/mutex.h"
#include "pal/utils.h"
#include "pal/misc.h"

#include "process.h"

#include <unistd.h>
#include <errno.h>
#if HAVE_POLL
#include <poll.h>
#else
#include "pal/fakepoll.h"
#endif  // HAVE_POLL

#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#if HAVE_STROPTS_H
#include <stropts.h>
#endif  // HAVE_STROPTS_H
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
/* Type definitions ***********************************************************/


enum FILETYPE
{
    FILE_ERROR,/*ERROR*/
    FILE_PE,   /*PE/COFF file*/
    FILE_UNIX, /*Unix Executable*/
    FILE_DIR   /*Directory*/
};


/* ------------------- Static function prototypes ------------------------------*/
static BOOL getFileName(LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
                        char *lpFileName);
static char ** buildArgv(LPCWSTR lpCommandLine, LPSTR lpAppPath,
                         UINT *pnArg, BOOL prependLoader);
static BOOL getPath(LPCSTR lpFileName, UINT iLen, LPSTR  lpPathFileName);
static int dupProcessHandle( HANDLE handle, HOBJSTRUCT *handle_data);
static int closeProcessHandle(HOBJSTRUCT *handle_data);
static int checkFileType(char *lpFileName);
static BOOL isValidProcessObject(PROCESS *lpProcess);
static BOOL PROCGetProcessStatus( HANDLE hProcess, PROCESS_STATE *state,
                                  DWORD *exit_code);
static BOOL PROCEndProcess(HANDLE hProcess, UINT uExitCode,
                           BOOL bTerminateUnconditionally);

/* ------------------- Static variables ------------------------------*/
PROCESS *pCurrentProcess = NULL;

/* Thread ID of thread that has started the ExitProcess process */
volatile DWORD terminator = 0;

// Process ID of this process.
DWORD gPID = -1;

static WCHAR W16_WHITESPACE[]= {0x0020, 0x0009, 0x000D, 0};
static WCHAR W16_WHITESPACE_DQUOTE[]= {0x0020, 0x0009, 0x000D, '"', 0};

static const DWORD IMAGE_DOS_SIGNATURE = 0x5A4D;
static const DWORD IMAGE_NT_SIGNATURE  = 0x00004550;
static const DWORD IMAGE_SIZEOF_NT_OPTIONAL32_HEADER     = 224;
static const DWORD IMAGE_NT_OPTIONAL_HDR32_MAGIC         = 0x10b;
static const DWORD IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR  = 14;

typedef struct _IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;
    DWORD   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER {
    //
    // Standard fields.
    //

    WORD    Magic;
    BYTE    MajorLinkerVersion;
    BYTE    MinorLinkerVersion;
    DWORD   SizeOfCode;
    DWORD   SizeOfInitializedData;
    DWORD   SizeOfUninitializedData;
    DWORD   AddressOfEntryPoint;
    DWORD   BaseOfCode;
    DWORD   BaseOfData;

    //
    // NT additional fields.
    //

    DWORD   ImageBase;
    DWORD   SectionAlignment;
    DWORD   FileAlignment;
    WORD    MajorOperatingSystemVersion;
    WORD    MinorOperatingSystemVersion;
    WORD    MajorImageVersion;
    WORD    MinorImageVersion;
    WORD    MajorSubsystemVersion;
    WORD    MinorSubsystemVersion;
    DWORD   Win32VersionValue;
    DWORD   SizeOfImage;
    DWORD   SizeOfHeaders;
    DWORD   CheckSum;
    WORD    Subsystem;
    WORD    DllCharacteristics;
    DWORD   SizeOfStackReserve;
    DWORD   SizeOfStackCommit;
    DWORD   SizeOfHeapReserve;
    DWORD   SizeOfHeapCommit;
    DWORD   LoaderFlags;
    DWORD   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[16];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_FILE_HEADER {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_NT_HEADERS {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

typedef struct _IMAGE_DOS_HEADER {      /* DOS .EXE header*/
    WORD   e_magic;                     /* Magic number*/
    WORD   e_cblp;                      /* Bytes on last page of file*/
    WORD   e_cp;                        /* Pages in file*/
    WORD   e_crlc;                      /* Relocations*/
    WORD   e_cparhdr;                   /* Size of header in paragraphs*/
    WORD   e_minalloc;                  /* Minimum extra paragraphs needed*/
    WORD   e_maxalloc;                  /* Maximum extra paragraphs needed*/
    WORD   e_ss;                        /* Initial (relative) SS value*/
    WORD   e_sp;                        /* Initial SP value*/
    WORD   e_csum;                      /* Checksum*/
    WORD   e_ip;                        /* Initial IP value*/
    WORD   e_cs;                        /* Initial (relative) CS value*/
    WORD   e_lfarlc;                    /* File address of relocation table*/
    WORD   e_ovno;                      /* Overlay number*/
    WORD   e_res[4];                    /* Reserved words*/
    WORD   e_oemid;                     /* OEM identifier (for e_oeminfo)*/
    WORD   e_oeminfo;                   /* OEM information; e_oemid specific*/
    WORD   e_res2[10];                  /* Reserved words*/
    LONG   e_lfanew;                    /* File address of new exe header*/
  } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

/* ------------------- Definitions ------------------------------*/
SET_DEFAULT_DEBUG_CHANNEL(THREAD);
LPWSTR pAppDir;
THREAD *pGThreadList;

#define PROCESS_PELOADER_FILENAME  "clix"

pthread_key_t thObjKey;
/* ------------------- Functions implementation ------------------------------*/

/*++
Function:
  GetCurrentProcessId

See MSDN doc.
--*/
DWORD
PALAPI
GetCurrentProcessId(
            VOID)
{
    PERF_ENTRY(GetCurrentProcessId);
    ENTRY("GetCurrentProcessId()\n" );

    if (pCurrentProcess == NULL)
    {
        ASSERT("Current process is invalid\n");
        LOGEXIT("GetCurrentProcessId returns DWORD 0x0\n");
        PERF_EXIT(GetCurrentProcessId);
        return 0;
    }

    LOGEXIT("GetCurrentProcessId returns DWORD %#x\n", pCurrentProcess->processId);
    PERF_EXIT(GetCurrentProcessId);
    return pCurrentProcess->processId;
}


/*++
Function:
  GetCurrentProcess

See MSDN doc.
--*/
HANDLE
PALAPI
GetCurrentProcess(
          VOID)
{
    PERF_ENTRY(GetCurrentProcess);
    ENTRY("GetCurrentProcess()\n" );

    LOGEXIT("GetCurrentProcess returns HANDLE %p\n", hPseudoCurrentProcess);
    PERF_EXIT(GetCurrentProcess);

    /* return a pseudo handle */
    return hPseudoCurrentProcess;
}


/*++
Function:
  PROCGetRealCurrentProcess

This returns the real process handle, and not a pseudo handle like
GetCurrentProcess do.
--*/
HANDLE
PROCGetRealCurrentProcess(
    VOID)
{
    return pCurrentProcess->hProcess;
}

/*++
Function:
  CreateProcessA

Note:
  Only Standard handles need to be inherited.
  Security attributes parameters are not used.

See MSDN doc.
--*/
BOOL
PALAPI
CreateProcessA(
           IN LPCSTR lpApplicationName,
           IN LPSTR lpCommandLine,
           IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
           IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
           IN BOOL bInheritHandles,
           IN DWORD dwCreationFlags,
           IN LPVOID lpEnvironment,
           IN LPCSTR lpCurrentDirectory,
           IN LPSTARTUPINFOA lpStartupInfo,
           OUT LPPROCESS_INFORMATION lpProcessInformation)
{
    BOOL bRet = FALSE;
    STARTUPINFOW StartupInfoW;
    LPWSTR CommandLineW = NULL;
    LPWSTR ApplicationNameW = NULL;
    LPWSTR CurrentDirectoryW = NULL;

    int n;

    PERF_ENTRY(CreateProcessA);
    ENTRY("CreateProcessA(lpAppName=%p (%s), lpCmdLine=%p (%s), lpProcessAttr=%p, "
          "lpThreadAttr=%p, bInherit=%d, dwFlags=%#x, lpEnv=%p, "
          "lpCurrentDir=%p (%s), lpStartupInfo=%p, lpProcessInfo=%p)\n",
           lpApplicationName?lpApplicationName:"NULL",
           lpApplicationName?lpApplicationName:"NULL",
           lpCommandLine?lpCommandLine:"NULL",
           lpCommandLine?lpCommandLine:"NULL",
           lpProcessAttributes, lpThreadAttributes, bInheritHandles,
           dwCreationFlags, lpEnvironment,
           lpCurrentDirectory?lpCurrentDirectory:"NULL",
           lpCurrentDirectory?lpCurrentDirectory:"NULL",
           lpStartupInfo, lpProcessInformation);

    if(lpStartupInfo == NULL)
    {
        ASSERT("lpStartupInfo is NULL!\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    /* convert parameters to Unicode */

    if(lpApplicationName)
    {
        n = MultiByteToWideChar(CP_ACP, 0, lpApplicationName, -1, NULL, 0);
        if(0 == n)
        {
            ASSERT("MultiByteToWideChar failed!\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto done;
        }
        ApplicationNameW = (LPWSTR)malloc(sizeof(WCHAR)*n);
        if(!ApplicationNameW)
        {
            ERROR("malloc() failed!\n");
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto done;
        }
        MultiByteToWideChar(CP_ACP, 0, lpApplicationName, -1, ApplicationNameW,
                            n);
    }

    if(lpCommandLine)
    {
        n = MultiByteToWideChar(CP_ACP, 0, lpCommandLine, -1, NULL, 0);
        if(0 == n)
        {
            ASSERT("MultiByteToWideChar failed!\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto done;
        }
        CommandLineW = (LPWSTR)malloc(sizeof(WCHAR)*n);
        if(!CommandLineW)
        {
            ERROR("malloc() failed!\n");
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto done;
        }
        MultiByteToWideChar(CP_ACP, 0, lpCommandLine, -1, CommandLineW, n);
    }

    if(lpCurrentDirectory)
    {
        n = MultiByteToWideChar(CP_ACP, 0, lpCurrentDirectory, -1, NULL, 0);
        if(0 == n)
        {
            ASSERT("MultiByteToWideChar failed!\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto done;
        }
        CurrentDirectoryW = (LPWSTR)malloc(sizeof(WCHAR)*n);
        if(!CurrentDirectoryW)
        {
            ERROR("malloc() failed!\n");
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto done;
        }
        MultiByteToWideChar(CP_ACP, 0, lpCurrentDirectory, -1,
                            CurrentDirectoryW, n);
    }

    // lpEnvironment should remain ansi on the call to CreateProcessW

    StartupInfoW.cb = sizeof StartupInfoW;
    StartupInfoW.dwFlags = lpStartupInfo->dwFlags;
    StartupInfoW.hStdError = lpStartupInfo->hStdError;
    StartupInfoW.hStdInput = lpStartupInfo->hStdInput;
    StartupInfoW.hStdOutput = lpStartupInfo->hStdOutput;
    /* all other members are PAL_Undefined, we can ignore them */

    bRet = CreateProcessW(ApplicationNameW, CommandLineW,
                          lpProcessAttributes, lpThreadAttributes,
                          bInheritHandles, dwCreationFlags, lpEnvironment,
                          CurrentDirectoryW, &StartupInfoW,
                          lpProcessInformation);
done:
    free(ApplicationNameW);
    free(CommandLineW);
    free(CurrentDirectoryW);

    LOGEXIT("CreateProcessA returns BOOL %d\n", bRet);
    PERF_EXIT(CreateProcessA);
    return bRet;
}


/*++
Function:
  CreateProcessW

Note:
  Only Standard handles need to be inherited.
  Security attributes parameters are not used.

See MSDN doc.
--*/
BOOL
PALAPI
CreateProcessW(
           IN LPCWSTR lpApplicationName,
           IN LPWSTR lpCommandLine,
           IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
           IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
           IN BOOL bInheritHandles,
           IN DWORD dwCreationFlags,
           IN LPVOID lpEnvironment,
           IN LPCWSTR lpCurrentDirectory,
           IN LPSTARTUPINFOW lpStartupInfo,
           OUT LPPROCESS_INFORMATION lpProcessInformation)
{
    pid_t processId;
    char lpFileName[MAX_PATH] ;
    char **lppArgv = NULL;
    UINT nArg;
    PROCESS *lpProcess = NULL;
    HANDLE hThread = INVALID_HANDLE_VALUE;
    int  iRet;
    DWORD dwLastError = 0;
    BOOL bRet = FALSE;
    SHMPTR shmprocess = 0;
    SHMPROCESS *shmprocess_ptr;
    file *stdInFile = NULL;
    file *stdOutFile = NULL;
    file *stdErrFile = NULL;
    char **EnvironmentArray=NULL;
    int child_blocking_pipe = -1;
    int parent_blocking_pipe = -1;

    PERF_ENTRY(CreateProcessW);
    ENTRY("CreateProcessW(lpAppName=%p (%S), lpCmdLine=%p (%S), lpProcessAttr=%p,"
           "lpThreadAttr=%p, bInherit=%d, dwFlags=%#x, lpEnv=%p,"
           "lpCurrentDir=%p (%S), lpStartupInfo=%p, lpProcessInfo=%p)\n",
           lpApplicationName?lpApplicationName:W16_NULLSTRING,
           lpApplicationName?lpApplicationName:W16_NULLSTRING,
           lpCommandLine?lpCommandLine:W16_NULLSTRING,
           lpCommandLine?lpCommandLine:W16_NULLSTRING,lpProcessAttributes,
           lpThreadAttributes, bInheritHandles, dwCreationFlags,lpEnvironment,
           lpCurrentDirectory?lpCurrentDirectory:W16_NULLSTRING,
           lpCurrentDirectory?lpCurrentDirectory:W16_NULLSTRING,
           lpStartupInfo, lpProcessInformation);

    /* Validate parameters */

    /* note : specs indicate lpApplicationName should always
       be NULL; however support for it is already implemented. Leaving the code
       in, specs can change; but rejecting non-NULL for now to conform to the
       spec. */
    if( NULL != lpApplicationName )
    {
        ASSERT("lpApplicationName should be NULL, but is %S instead\n",
               lpApplicationName);
        dwLastError = ERROR_INVALID_PARAMETER;
        goto CreateProcessExit;
    }

    if (0 != (dwCreationFlags & ~(CREATE_SUSPENDED|CREATE_NEW_CONSOLE)))
    {
        ASSERT("Unexpected creation flags (%#x)\n", dwCreationFlags);
        dwLastError = ERROR_INVALID_PARAMETER;
        goto CreateProcessExit;
    }

    /* Security attributes parameters are ignored */
    if (lpProcessAttributes != NULL &&
        (lpProcessAttributes->lpSecurityDescriptor != NULL ||
         lpProcessAttributes->bInheritHandle != TRUE))
    {
        ASSERT("lpProcessAttributes is invalid, parameter ignored (%p)\n",
               lpProcessAttributes);
        dwLastError = ERROR_INVALID_PARAMETER;
        goto CreateProcessExit;
    }

    if (lpThreadAttributes != NULL)
    {
        ASSERT("lpThreadAttributes parameter must be NULL (%p)\n",
               lpThreadAttributes);
        dwLastError = ERROR_INVALID_PARAMETER;
        goto CreateProcessExit;
    }

    /* note : Win32 crashes in this case */
    if(NULL == lpStartupInfo)
    {
        ERROR("lpStartupInfo is NULL\n");
        dwLastError = ERROR_INVALID_PARAMETER;
        goto CreateProcessExit;
    }

    /* Validate lpStartupInfo.cb field */
    if (lpStartupInfo->cb < sizeof(STARTUPINFOW))
    {
        ASSERT("lpStartupInfo parameter structure size is invalid (%u)\n",
              lpStartupInfo->cb);
        dwLastError = ERROR_INVALID_PARAMETER;
        goto CreateProcessExit;
    }

    /* lpStartupInfo should be either zero or STARTF_USESTDHANDLES */
    if (lpStartupInfo->dwFlags & ~STARTF_USESTDHANDLES)
    {
        ASSERT("lpStartupInfo parameter invalid flags (%#x)\n",
              lpStartupInfo->dwFlags);
        dwLastError = ERROR_INVALID_PARAMETER;
        goto CreateProcessExit;
    }

    /* validate given standard handles if we have any */
    if (lpStartupInfo->dwFlags & STARTF_USESTDHANDLES)
    {
        BOOL got_bad_handle = FALSE;

        /* get the handle objects */
        stdInFile =  FILEAcquireFileStruct(lpStartupInfo->hStdInput);
        stdOutFile = FILEAcquireFileStruct(lpStartupInfo->hStdOutput);
        stdErrFile = FILEAcquireFileStruct(lpStartupInfo->hStdError);

        /* make sure we got them all */
        if (!stdInFile || !stdOutFile || !stdErrFile)
        {
            ERROR ("found at least one invalid handle in StartupInfo\n");
            got_bad_handle = TRUE;
        }
        /* make sure they're all inheritable */
        else if(!stdInFile->inheritable ||
                !stdOutFile->inheritable ||
                !stdErrFile->inheritable)
        {
            ERROR ("found at least one non-inheritable handle!\n");
            got_bad_handle = TRUE;
        }
        /* remove their close-on-exec property, so the child inherits them */
        else if(-1 == fcntl(stdInFile->unix_fd, F_SETFD, 0))
        {
            ASSERT("couldn't remove close-on-exec flag from stdin "
                  "descriptor! errno is %d (%s)\n",
                   errno, strerror(errno));
            got_bad_handle = TRUE;
        }
        else if(-1 == fcntl(stdOutFile->unix_fd, F_SETFD, 0))
        {
            ASSERT("couldn't remove close-on-exec flag from stdout "
                  "descriptor! errno is %d (%s)\n",
                   errno, strerror(errno));
            got_bad_handle = TRUE;
        }
        else if(-1 == fcntl(stdErrFile->unix_fd, F_SETFD, 0))
        {
            ASSERT("couldn't remove close-on-exec flag from stderr "
                  "descriptor! errno is %d (%s)\n",
                   errno, strerror(errno));
            got_bad_handle = TRUE;
        }
        /* abort if we got any error */
        if (got_bad_handle)
        {
            TRACE("there was something wrong with the standard handles; "
                  "CreateProcess fails.\n");
            dwLastError = ERROR_INVALID_HANDLE;
            goto CreateProcessExit;
        }
    }

    if (!getFileName(lpApplicationName, lpCommandLine, lpFileName))
    {
        ERROR("Can't find executable!\n");
        dwLastError = ERROR_FILE_NOT_FOUND;
        goto CreateProcessExit;
    }

    /* check type of file */
    iRet = checkFileType(lpFileName);

    switch (iRet)
    {
        case FILE_ERROR: /* file not found, or not an executable */
            WARN ("File is not valid (%s)", lpFileName);
            dwLastError = ERROR_FILE_NOT_FOUND;
            goto CreateProcessExit;

        case FILE_PE: /* PE/COFF file */
            /*Get the path name where the PAL DLL was loaded from
             * I am using MAX_PATH - (strlen(PROCESS_PELOADER_FILENAME)+1)
             * as the length as I have to append the file name at the end */
            if ( PAL_GetPALDirectoryA( lpFileName,
                                      (MAX_PATH - (strlen(PROCESS_PELOADER_FILENAME)+1))))
            {
                strcat(lpFileName,"/");
                strcat(lpFileName,PROCESS_PELOADER_FILENAME);
            }
            else
            {
                ASSERT("PAL_GetPALDirectoryA failed to return the"
                       "pal installation directory \n");
                dwLastError = ERROR_INTERNAL_ERROR;
                goto CreateProcessExit;
            }

            break;

        case FILE_UNIX: /* Unix binary file */
            break;  /* nothing to do */

        case FILE_DIR:/*Directory*/
            WARN ("File is a Directory (%s)", lpFileName);
            dwLastError = ERROR_ACCESS_DENIED;
            goto CreateProcessExit;
            break;

        default: /* not supposed to get here */
            ASSERT ("Invalid return type from checkFileType");
            dwLastError = ERROR_FILE_NOT_FOUND;
            goto CreateProcessExit;
    }

    /* build Argument list, lppArgv is allocated in buildArgv function and
       requires to be freed */
    lppArgv = buildArgv(lpCommandLine, lpFileName, &nArg, iRet==1);

    /* set the Environment variable */
    if (lpEnvironment != NULL)
    {
        unsigned i;
        // Since CREATE_UNICODE_ENVIRONMENT isn't supported we know the string is ansi
        unsigned EnvironmentEntries = 0;
        // Convert the environment block to array of strings
        // Count the number of entries
        // Is it a string that contains null terminated string, the end is delimited
        // by two null in a row.
        for (i = 0; ((char *)lpEnvironment)[i]!='\0'; i++)
        {
            EnvironmentEntries ++;
            for (;((char *)lpEnvironment)[i]!='\0'; i++)
            {
            }
        }
        EnvironmentEntries++;
        EnvironmentArray = (char **)malloc(EnvironmentEntries * sizeof(char *));

        EnvironmentEntries = 0;
        // Convert the environment block to array of strings
        // Count the number of entries
        // Is it a string that contains null terminated string, the end is delimited
        // by two null in a row.
        for (i = 0; ((char *)lpEnvironment)[i]!='\0'; i++)
        {
            EnvironmentArray[EnvironmentEntries] = &((char *)lpEnvironment)[i];
            EnvironmentEntries ++;
            for (;((char *)lpEnvironment)[i]!='\0'; i++)
            {
            }
        }
        EnvironmentArray[EnvironmentEntries] = NULL;
    }

    /* pre-allocate SHMPROCESS structure */
    shmprocess = SHMalloc(sizeof(SHMPROCESS));
    if(0 == shmprocess)
    {
        ERROR("Can't allocate SHMPROCESS structure\n");
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto CreateProcessExit;
    }

    /* Create the process object */
    lpProcess = (PROCESS *) malloc(sizeof(PROCESS));

    if (!lpProcess)
    {
        ERROR("malloc() failed to create PROCESS object; errno is %d (%s)\n",
              errno, strerror(errno));
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto CreateProcessExit;
    }

    /* fill the process structure */
    lpProcess->objHeader.type = HOBJ_PROCESS;
    lpProcess->objHeader.close_handle = closeProcessHandle;
    lpProcess->objHeader.dup_handle = dupProcessHandle;

    lpProcess->refCount = 1;
    lpProcess->lpInitProcInfo = NULL;
    lpProcess->exit_code = 0;
    lpProcess->state = PS_RUNNING;
    
    lpProcess->hProcess = HMGRGetHandle((HOBJSTRUCT *) lpProcess);

    if (lpProcess->hProcess == INVALID_HANDLE_VALUE)
    {
        ERROR("Unable to create a handle\n");
        free(lpProcess);
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto CreateProcessExit;
    }

    /* initialize the critical section */
    
    /* NOTA: we can initialize process' the critical section before forking
       since we're 'exec'ing anyway - exec function completely replace 
       caller process with the 'exec'ed program)
     */
    if (0 != SYNCInitializeCriticalSection(&(lpProcess->critSection)))
    {
        ERROR("Unable to initialize the critical section\n");
        HMGRFreeHandle(lpProcess->hProcess);
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        free(lpProcess);
        goto CreateProcessExit;
    }
    

    /* Create our dummy thread handle */
    hThread = THREADCreateDummyThreadHandle();
    if (hThread == INVALID_HANDLE_VALUE)
    {
        ERROR("Unable to create a handle\n");
        DeleteCriticalSection(&(lpProcess->critSection));
        HMGRFreeHandle(lpProcess->hProcess); 
        free(lpProcess);
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto CreateProcessExit;
    }
    if(dwCreationFlags & CREATE_SUSPENDED)
    {
        THREAD *lpDummyThread;
        int pipe_descs[2];

        if(-1 == pipe(pipe_descs))
        {
            ERROR("pipe() failed! error is %d (%s)\n", errno, strerror(errno));
            DeleteCriticalSection(&(lpProcess->critSection));
            HMGRFreeHandle(lpProcess->hProcess); 
            free(lpProcess);
            CloseHandle(hThread);
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            goto CreateProcessExit;
        }                        

        lpDummyThread = (THREAD *)HMGRLockHandle2(hThread, HOBJ_THREAD);
        if(NULL == lpDummyThread)
        {
            /* handle is still internal, this shouldn't have failed */
            ASSERT("couldn't lock dummy thread's handle!");
        }
        /* [0] is read end, [1] is write end */
        lpDummyThread->blockingPipe = pipe_descs[1];
        parent_blocking_pipe = pipe_descs[1];
        child_blocking_pipe = pipe_descs[0];

        HMGRUnlockHandle(hThread, &lpDummyThread->objHeader);
    }

    /* fork the new process */
    processId = fork();

    if (processId == -1)
    {
        ASSERT("unable to create a new process with fork()\n");
        DeleteCriticalSection(&(lpProcess->critSection));
        HMGRFreeHandle(lpProcess->hProcess);
        free(lpProcess);
        CloseHandle(hThread);
        if(-1 != child_blocking_pipe)
        {
            close(child_blocking_pipe);
        }
        dwLastError = ERROR_INTERNAL_ERROR;
        goto CreateProcessExit;
    }
    if (processId == 0)  /* child process */
    {
        if(dwCreationFlags & CREATE_SUSPENDED)
        {
            DWORD resume_code = 0;
            ssize_t read_ret;

            /* close the write end of the pipe, the child doesn't need it */
            close(parent_blocking_pipe);

            TRACE("got CREATE_SUSPENDED, now blocking\n");

            /* block until ResumeThread writes something to the pipe */
            read_ret = read(child_blocking_pipe, &resume_code, sizeof(DWORD));
            if(sizeof(DWORD) != read_ret)
            {
                /* note : read might return 0 (and return EAGAIN) if the other 
                   end of the pipe gets closed - for example because the parent
                   process dies (very) abruptly */
                ERROR("read() returned %d instead of %d; errno is %d (%s)\n", 
                      read_ret, (int)sizeof(DWORD),errno, strerror(errno));
                exit(EXIT_FAILURE);
            }
            if(WAKEUPCODE == resume_code)
            {
                TRACE("got wakeupcode, resuming execution\n");
            }
            else if(GIVEUPCODE == resume_code)
            {
                ERROR("got giveupcode, exiting\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                ASSERT("got unexpected code %#x, exiting\n");
                exit(EXIT_FAILURE);
            }
            close(child_blocking_pipe);
        }

        /* Set the current directory */
        if (lpCurrentDirectory)
        {
            SetCurrentDirectoryW(lpCurrentDirectory);
        }

        /* Set the standard handles to the incoming values */
        if (lpStartupInfo->dwFlags & STARTF_USESTDHANDLES)
        {
            /* For each handle, we need to duplicate the incoming unix
               fd to the corresponding standard one.  The API that I use,
               dup2, will copy the source to the destination, automatically
               closing the existing destination, in an atomic way */
            if (dup2(stdInFile->unix_fd, STDIN_FILENO) == -1)
            {
                ASSERT ("Could not duplicate standard in\n");
            }

            if (dup2(stdOutFile->unix_fd, STDOUT_FILENO) == -1)
            {
                ASSERT ("Could not duplicate standard out\n");
            }

            if (dup2(stdErrFile->unix_fd, STDERR_FILENO) == -1)
            {
                ASSERT ("Could not duplicate standard error\n");
            }
            
            /* now close the original FDs, we don't need them anymore */
            close(stdInFile->unix_fd);
            close(stdOutFile->unix_fd);
            close(stdErrFile->unix_fd);
        }

        /* execute the new process */

        TRACE("executing process: lpFileName=%s, argv[0]=%s argv[1]=%s\n",
              lpFileName, *lppArgv, *(lppArgv+1));

        if (EnvironmentArray)
        {
            TRACE("Calling execve with EnvironmentArray");
            execve(lpFileName, lppArgv, EnvironmentArray);
        }
        else
        {
            TRACE("Calling execve with environ");
            execve(lpFileName, lppArgv, palEnvironment);
        }

        /* if we get here, it means there's a fatal error, just exit */
        ASSERT("execve function call failed, exiting. (error=%d)\n", errno);
        exit(1);
    }

    /* parent process */

    /* close the read end of the pipe, the parent doesn't need it */
    close(child_blocking_pipe);


    /* release file handle structures, we don't need them anymore */
    if (lpStartupInfo->dwFlags & STARTF_USESTDHANDLES)
    {
        FILEReleaseFileStruct(lpStartupInfo->hStdInput,stdInFile);
        FILEReleaseFileStruct(lpStartupInfo->hStdOutput,stdOutFile);
        FILEReleaseFileStruct(lpStartupInfo->hStdError,stdErrFile);
        stdInFile = NULL;
        stdOutFile = NULL;
        stdErrFile = NULL;
    }

    /* Set the process ID */
    lpProcess->processId = processId;

    /* fill PROCESS_INFORMATION strucutre */
    lpProcessInformation->hProcess = lpProcess->hProcess;
    lpProcessInformation->hThread = hThread; /* It's just a dummy handle */
    lpProcessInformation->dwProcessId = processId;
    lpProcessInformation->dwThreadId_PAL_Undefined = 0;

    /* create SHMPROCESS structure or hook into existing one */
    SHMLock();
    shmprocess_ptr = PROCFindSHMProcess(processId);
    if(NULL == shmprocess_ptr)
    {
        TRACE("Creating SHMPROCESS structure for process 0x%08x\n", processId);
        shmprocess_ptr = SHMPTR_TO_PTR(shmprocess);
        shmprocess_ptr->attachCount = 0;
        shmprocess_ptr->attachedByProcId = 0;
        shmprocess_ptr->info = NULL; /* must be set by the process itself */
        shmprocess_ptr->self = shmprocess; /* reference to the shmprocess shmptr */
        shmprocess_ptr->next = SHMGetInfo(SIID_PROCESS_INFO);
        shmprocess_ptr->refcount = 0; /* incremented below */
        shmprocess_ptr->processId = processId;

        SHMSetInfo(SIID_PROCESS_INFO, shmprocess);
    }
    else
    {
        TRACE("SHMPROCESS structure for process 0x%08x already exists\n",
              processId);

        /* release pre-allocated structure */
        SHMfree(shmprocess);
    }
    shmprocess = 0;
    shmprocess_ptr->refcount++;
    lpProcess->shmprocess = shmprocess_ptr;
    SHMRelease();


    TRACE("New process created: id=%#x\n", processId);

    bRet = TRUE;

CreateProcessExit:

    if (EnvironmentArray)
        free(EnvironmentArray);

    /* if we still have the file structures at this point, it means we 
       encountered an error sometime between when we acquired them and when we 
       fork()ed. We not only have to release them, we have to give them back 
       their close-on-exec flag */
    if (stdInFile) 
    {
        if(-1 == fcntl(stdInFile->unix_fd, F_SETFD, 1))
        {
            WARN("couldn't restore close-on-exec flag to stdin descriptor! "
                 "errno is %d (%s)\n", errno, strerror(errno));
        }
        FILEReleaseFileStruct(lpStartupInfo->hStdInput,stdInFile);
    }
    if (stdOutFile)
    {
        if(-1 == fcntl(stdOutFile->unix_fd, F_SETFD, 1))
        {
            WARN("couldn't restore close-on-exec flag to stdout descriptor! "
                 "errno is %d (%s)\n", errno, strerror(errno));
        }
        FILEReleaseFileStruct(lpStartupInfo->hStdOutput,stdOutFile);
    }
    if (stdErrFile)
    {
        if(-1 == fcntl(stdErrFile->unix_fd, F_SETFD, 1))
        {
            WARN("couldn't restore close-on-exec flag to stderr descriptor! "
                 "errno is %d (%s)\n", errno, strerror(errno));
        }
        FILEReleaseFileStruct(lpStartupInfo->hStdError,stdErrFile);
    }

    /* free allocated memory */
    if (lppArgv)
    {
        free (*lppArgv);
        free (lppArgv);
    }
    if(shmprocess)
    {
        SHMfree(shmprocess);
    }

    if (dwLastError != 0)
    {
        SetLastError(dwLastError);
    }

    LOGEXIT("CreateProcessW returns BOOL %d\n", bRet);
    PERF_EXIT(CreateProcessW);
    return bRet;
}


/*++
Function:
  GetExitCodeProcess

See MSDN doc.
--*/
BOOL
PALAPI
GetExitCodeProcess(
           IN HANDLE hProcess,
           IN LPDWORD lpExitCode)
{
    DWORD dwLastError = 0;
    BOOL retval = FALSE;
    PROCESS *process = NULL;
    PROCESS_STATE state;
    DWORD exit_code;

    PERF_ENTRY(GetExitCodeProcess);
    ENTRY("GetExitCodeProcess(hProcess = %p, lpExitCode = %p)\n",
          hProcess, lpExitCode);

    if(NULL == lpExitCode)
    {
        WARN("Got NULL lpExitCode\n");
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    process = (PROCESS *)HMGRLockHandle2(hProcess, HOBJ_PROCESS);
    if(NULL == process)
    {
        WARN("%p is invalid handle\n", hProcess);
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }

    if(!PROCGetProcessStatus(hProcess, &state, &exit_code))
    {
        ASSERT("Couldn't get process status information!\n");
        dwLastError = ERROR_INTERNAL_ERROR;
        goto done;
    }
    retval = TRUE;
    if( PS_DONE == state )
    {
        *lpExitCode = exit_code;
    }
    else
    {
        *lpExitCode = STILL_ACTIVE;
    }

done:
    if(NULL!=process)
    {
        HMGRUnlockHandle(hProcess,&process->objHeader);
    }
    if(0 != dwLastError)
    {
        SetLastError(dwLastError);
    }
    LOGEXIT("GetExitCodeProcess returns BOOL %d\n", retval);
    PERF_EXIT(GetExitCodeProcess);
    return retval;
}

/*++
Function:
  apccallback_threaddetach
  
  callback function given to QueueUserAPC; used to trigger calls to 
  DllMain(DLL_TREAD_DETACH) in another thread (during ExitProcess)

(no parameters,no return value)
--*/
static void PALAPI apccallback_threaddetach(ULONG_PTR param)
{
    TRACE("Calling DllMain(DLL_THREAD_DETACH) during process termination...\n");
    LOADCallDllMain(DLL_THREAD_DETACH, NULL);
}

/*++
Function:
  PROCQueueDllMainCalls
  
  Use QueueUserAPC to give threads a chance to receive DLL_THREAD_DETACH when 
  ExitProcess is called

(no parameters,no return value)
--*/
void PROCQueueDllMainCalls(void)
{
    THREAD *thread;
    THREAD **thread_array;
    int num_threads = 0;
    int curr_thread;
    DWORD this_thread;

    this_thread = GetCurrentThreadId();

    SYNCEnterCriticalSection(&pCurrentProcess->critSection,TRUE);

    thread = pGThreadList;
    while(NULL != thread)
    {
        if(thread->dwThreadId != this_thread)
        {
            num_threads++;
        }
        thread = thread->next;
    }
    /* build an array oh thread objects, locking each handle as we go. this is 
       necessary because we have to leave teh critical section before calling 
       QueuUserAPC, to avoid potential deadlocks */
    thread_array = malloc(num_threads*sizeof(*thread_array));
    if( (NULL == thread_array) && (num_threads != 0) )
    {
        /* we don't absolutely need to call DllMain, so if malloc() fails, just 
           give up */
        ERROR("malloc failure; won't call DllMain(DLL_THREAD_DETACH)\n");
        SYNCLeaveCriticalSection(&pCurrentProcess->critSection,TRUE);
        return;
    }

    thread = pGThreadList;
    curr_thread = 0;
    while(NULL != thread)
    {
        if(thread->dwThreadId != this_thread)
        {
            if(curr_thread>=num_threads)
            {
                ASSERT("thread count has changed!\n");
                break;
            }
            thread_array[curr_thread] = (THREAD *)
                                 HMGRLockHandle2(thread->hThread, HOBJ_THREAD);
            curr_thread++;
        }

        thread = thread->next;
    }
    SYNCLeaveCriticalSection(&pCurrentProcess->critSection,TRUE);
    
    for(curr_thread = 0; curr_thread<num_threads; curr_thread++)
    {
        /* in case lock failed; possible if handle was marked for closure... 
           or is it? we were holding the critical section... */
        if(NULL != thread_array[curr_thread])
        {                
            QueueUserAPC(&apccallback_threaddetach, 
                         thread_array[curr_thread]->hThread,0);
            HMGRUnlockHandle(thread_array[curr_thread]->hThread, 
                             &thread_array[curr_thread]->objHeader);
        }
    }
    if (thread_array)
    {
        free(thread_array);
    }

    /* give other threads a chance to run */
    sched_yield();
}


/*++
Function:
  ExitProcess

See MSDN doc.
--*/
PAL_NORETURN
VOID
PALAPI
ExitProcess(
        IN UINT uExitCode)
{
    DWORD old_terminator;

    PERF_ENTRY_ONLY(ExitProcess);
    ENTRY("ExitProcess(uExitCode=0x%x)\n", uExitCode );

    old_terminator = InterlockedCompareExchange(&terminator,
                                                GetCurrentThreadId(),0);

    if(GetCurrentThreadId() == old_terminator)
    {
        // This thread has already initiated termination. This can happen
        // in two ways:
        // 1) DllMain(DLL_PROCESS_DETACH) triggers a call to ExitProcess.
        // 2) PAL_exit() is called after the last PALTerminate().
        // If the PAL is still initialized, we go straight through to
        // PROCEndProcess. If it isn't, we simply exit.
        if (!PALIsInitialized())
        {
            exit(uExitCode);
            ASSERT("exit has returned\n");
        }
        else
        {
            WARN("thread re-called ExitProcess\n");
            PROCEndProcess(GetCurrentProcess(), uExitCode, FALSE);
        }
    }
    else if(0 != old_terminator)
    {
        /* another thread has already initiated the termination process. we 
           could just block on the PALInitLock critical section, but then 
           PROCSuspendOtherThreads would hang... so sleep forever here, we're
           terminating anyway */
        WARN("termination already started from another thread; blocking.\n");
        poll(NULL,0,INFTIM);
    }

    /* ExitProcess may be called even if PAL is not initialized.
       Verify if process structure exist
    */
    if ( PALInitLock() && PALIsInitialized() )
    {
        PROCEndProcess(GetCurrentProcess(), uExitCode, FALSE);

        /* Should not get here, because we terminate the current process */
        ASSERT("PROCEndProcess has returned\n");
    }
    else
    {
        exit(uExitCode);
        
        /* Should not get here, because we terminate the current process */
        ASSERT("exit has returned\n");
    }

    /* this should never get executed */
    ASSERT("ExitProcess should not return!\n");
    for (;;);
}


/*++
Function:
  TerminateProcess

Note:
  hProcess is a handle on the current process.

See MSDN doc.
--*/
BOOL
PALAPI
TerminateProcess(
         IN HANDLE hProcess,
         IN UINT uExitCode)
{
    BOOL ret;

    PERF_ENTRY(TerminateProcess);
    ENTRY("TerminateProcess(hProcess=%p, uExitCode=%u)\n",hProcess, uExitCode );
    ret = PROCEndProcess(hProcess, uExitCode, TRUE);
    
    LOGEXIT("TerminateProcess returns BOOL %d\n", ret);
    PERF_EXIT(TerminateProcess);
    return ret;
}

/*++
Function:
  PROCEndProcess
  
  Called from TerminateProcess and ExitProcess. This does the work of
  TerminateProcess, but also takes a flag that determines whether we
  shut down unconditionally. If the flag is set, the PAL will do very
  little extra work before exiting. Most importantly, it won't shut
  down any DLLs that are loaded.

--*/
static BOOL PROCEndProcess(HANDLE hProcess, UINT uExitCode,
                           BOOL bTerminateUnconditionally)
{
    DWORD dwProcessId;
    BOOL ret = FALSE;

    dwProcessId = PROCGetProcessIDFromHandle(hProcess);
    if (dwProcessId == 0)
    {
        SetLastError(ERROR_INVALID_HANDLE);
    }
    else if(dwProcessId != GetCurrentProcessId())
    {
        if (kill(dwProcessId, SIGKILL) == 0)
        {
            ret = TRUE;
        }
        else
        {
            switch (errno) {
            case ESRCH:
                SetLastError(ERROR_INVALID_HANDLE);
                break;
            case EPERM:
                SetLastError(ERROR_ACCESS_DENIED);
                break;
            default:
                // Unexpected failure.
                ASSERT(FALSE);
                SetLastError(ERROR_INTERNAL_ERROR);
                break;
            }
        }
    }
    else
    {
        TerminateCurrentProcessNoExit(bTerminateUnconditionally);
        LOGEXIT("PROCEndProcess will not return\n");
        exit(uExitCode);
        ASSERT(FALSE); // we shouldn't get here
    }

    return ret;
}

/*++
Function:
  PROCCleanupProcess
  
  Do all cleanup work for TerminateProcess, but don't terminate the process.
  If bTerminateUnconditionally is TRUE, we exit as quickly as possible.

(no return value)
--*/
void PROCCleanupProcess(BOOL bTerminateUnconditionally)
{
    /* Declare the beginning of shutdown */
    PALSetShutdownIntent();

    PALCommonCleanup(PALCLEANUP_STEP_ONE, FALSE);

    /* This must be called after PALCommonCleanup(PALCLEANUP_STEP_ONE, ...)
     */
    PALShutdown();
}

/*++
Function:
  GetProcessTimes

See MSDN doc.
--*/
BOOL
PALAPI
GetProcessTimes(
        IN HANDLE hProcess,
        OUT LPFILETIME lpCreationTime,
        OUT LPFILETIME lpExitTime,
        OUT LPFILETIME lpKernelTime,
        OUT LPFILETIME lpUserTime)
{
    BOOL retval = FALSE;
    struct rusage resUsage;
    __int64 calcTime;
    const __int64 SECS_TO_NS = 1000000000; /* 10^9 */
    const __int64 USECS_TO_NS = 1000;      /* 10^3 */


    PERF_ENTRY(GetProcessTimes);
    ENTRY("GetProcessTimes(hProcess=%p, lpExitTime=%p, lpKernelTime=%p,"
          "lpUserTime=%p)\n",
          hProcess, lpCreationTime, lpExitTime, lpKernelTime, lpUserTime );

    /* Make sure hProcess is the current process, this is the only supported
       case */
    if(PROCGetProcessIDFromHandle(hProcess)!=GetCurrentProcessId())
    {
        ASSERT("GetProcessTimes() does not work on a process other than the "
              "current process.\n");
        SetLastError(ERROR_INVALID_HANDLE);
        goto GetProcessTimesExit;
    }

    /* First, we need to actually retrieve the relevant statistics from the 
       OS */
    if (getrusage (RUSAGE_SELF, &resUsage) == -1)
    {
        ASSERT("Unable to get resource usage information for the current "
              "process\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        goto GetProcessTimesExit;
    }
    
    TRACE ("getrusage User: %ld sec,%ld microsec. Kernel: %ld sec,%ld"
           " microsec\n",
           resUsage.ru_utime.tv_sec, resUsage.ru_utime.tv_usec,
           resUsage.ru_stime.tv_sec, resUsage.ru_stime.tv_usec);

    if (lpUserTime)
    {
        /* Get the time of user mode execution, in 100s of nanoseconds */
        calcTime = (__int64)resUsage.ru_utime.tv_sec * SECS_TO_NS;
        calcTime += (__int64)resUsage.ru_utime.tv_usec * USECS_TO_NS;
        calcTime /= 100; /* Produce the time in 100s of ns */
        /* Assign the time into lpUserTime */
        lpUserTime->dwLowDateTime = (DWORD)calcTime;
        lpUserTime->dwHighDateTime = (DWORD)(calcTime >> 32);
    }

    if (lpKernelTime)
    {
        /* Get the time of kernel mode execution, in 100s of nanoseconds */
        calcTime = (__int64)resUsage.ru_stime.tv_sec * SECS_TO_NS;
        calcTime += (__int64)resUsage.ru_stime.tv_usec * USECS_TO_NS;
        calcTime /= 100; /* Produce the time in 100s of ns */
        /* Assign the time into lpUserTime */
        lpKernelTime->dwLowDateTime = (DWORD)calcTime;
        lpKernelTime->dwHighDateTime = (DWORD)(calcTime >> 32);
    }
    
    retval = TRUE;


GetProcessTimesExit:
    LOGEXIT("GetProcessTimes returns BOOL %d\n", retval);
    PERF_EXIT(GetProcessTimes);
    return (retval);
}


/*++
Function:
  GetCommandLineW

See MSDN doc.
--*/
LPWSTR
PALAPI
GetCommandLineW(
        VOID)
{
    PERF_ENTRY(GetCommandLineW);
    ENTRY("GetCommandLineW()\n");

    if (pCurrentProcess == NULL)
    {
        ASSERT("Current process handle is invalid\n");
        LOGEXIT("GetCommandLineW returns LPWSTR NULL\n");
        PERF_EXIT(GetCommandLineW);
        return NULL;
    }

    LOGEXIT("GetCommandLineW returns LPWSTR %p (%S)\n",
          pCurrentProcess->lpInitProcInfo->lpwstrCmdLine,
          pCurrentProcess->lpInitProcInfo->lpwstrCmdLine);
    PERF_EXIT(GetCommandLineW);
    return pCurrentProcess->lpInitProcInfo->lpwstrCmdLine;
}

/*++
Function:
  OpenProcess

See MSDN doc.

Notes :
dwDesiredAccess is ignored (all supported operations will be allowed)
bInheritHandle is ignored (no inheritance)
--*/
HANDLE
PALAPI
OpenProcess(
        DWORD dwDesiredAccess,
        BOOL bInheritHandle,
        DWORD dwProcessId)
{
    HANDLE hprocess = NULL;
    SHMPROCESS *shmprocess;
    PROCESS *process_info;

    PERF_ENTRY(OpenProcess);
    ENTRY("OpenProcess(dwDesiredAccess=0x%08x, bInheritHandle=%d, "
          "dwProcessId = 0x%08x)\n",
          dwDesiredAccess, bInheritHandle, dwProcessId );

    SHMLock();

    /* step 1 : find process ID in shared memory */
    shmprocess = PROCFindSHMProcess(dwProcessId);

    if(!shmprocess)
    {
        TRACE("Process 0x%08x was not found in shared memory; can't open.\n",
              dwProcessId);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    process_info = (PROCESS *)malloc(sizeof(PROCESS));
    if(!process_info)
    {
        ERROR("Failed to allocate memory for PROCESS structure!\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto done;
    }

    if (0 != SYNCInitializeCriticalSection(&process_info->critSection))
    {
        ERROR("Failed to initialize critsect of PROCESS structure!\n");
        free(process_info);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto done;
    }
        
    process_info->lpInitProcInfo = NULL;
    process_info->processId = dwProcessId;
    process_info->refCount = 1;

    process_info->objHeader.type = HOBJ_PROCESS;
    process_info->objHeader.close_handle = closeProcessHandle;
    process_info->objHeader.dup_handle = dupProcessHandle;

    process_info->hProcess = HMGRGetHandle(&process_info->objHeader);
    if(INVALID_HANDLE_VALUE == process_info->hProcess)
    {
        ERROR("Unable to allocate handle for new process object\n");
        DeleteCriticalSection(&process_info->critSection);
        free(process_info);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto done;
    }
    else
    {
        hprocess = process_info->hProcess;
        process_info->shmprocess = shmprocess;
        shmprocess->refcount++;
    }
 
done:
    SHMRelease();

    LOGEXIT("OpenProcess returns HANDLE %p\n", hprocess);
    PERF_EXIT(OpenProcess);
    return hprocess;
}


/*++
Function:
  PROCGetProcessIDFromHandle

Abstract
  Return the process ID from a process handle

Parameter
  hProcess:  process handle

Return
  Return the process ID, or 0 if it's not a valid handle
--*/
DWORD
PROCGetProcessIDFromHandle(
        HANDLE hProcess)
{
    PROCESS *lpProcess;
    DWORD retval;

    if (hProcess == hPseudoCurrentProcess)
    {
        hProcess = pCurrentProcess->hProcess;
    }

    lpProcess = (PROCESS *) HMGRLockHandle2(hProcess, HOBJ_PROCESS);

    if ( lpProcess == NULL)
    {
        ERROR("Process handle is invalid\n");
        retval = 0;
    }
    else
    {
        retval = lpProcess->processId;
    }
    if(NULL!=lpProcess)
    {
        HMGRUnlockHandle(hProcess,&lpProcess->objHeader);
    }

    return retval;
}


/*++
Function:
  PROCCreateInitialProcess

Abstract
  Initialize all the structures for the initial process.

Parameter
  lpwstrCmdLine:   Command line.
  lpwstrFullPath : Full path to executable

Return
  TRUE: if successful
  FALSE: otherwise

Notes :
    This function takes ownership of lpwstrCmdLine, but not of lpwstrFullPath
--*/
BOOL
PROCCreateInitialProcess(LPWSTR lpwstrCmdLine, LPWSTR lpwstrFullPath)
{
    INITIALPROCINFO *lpInitialProcInfo;
    char lpstrCurDir[MAX_PATH];
    INT n;
    LPWSTR lpwstr;
    LPWSTR initial_dir;
    THREAD *initial_thread;
    SHMPTR shmprocess;
    SHMPROCESS *shmprocess_ptr;
    BOOL bRet = FALSE;

    pGThreadList = NULL;
    pCurrentProcess = (PROCESS *) malloc(sizeof(PROCESS));

    if (!pCurrentProcess)
    {
        ERROR("Can't allocate memory for initial process structure\n");
        goto EXIT;
    }

    shmprocess = SHMalloc(sizeof(SHMPROCESS));
    if(!shmprocess)
    {
        ERROR("Can't allocate shared memory block for process information!\n");
        goto CLEANUP1;
    }

    pCurrentProcess->objHeader.type = HOBJ_PROCESS;
    pCurrentProcess->objHeader.close_handle = closeProcessHandle;
    pCurrentProcess->objHeader.dup_handle = dupProcessHandle;
    pCurrentProcess->refCount = 1;
    pCurrentProcess->processId = gPID;
    pCurrentProcess->state = PS_RUNNING;
    pCurrentProcess->exit_code = 0;
    pCurrentProcess->hProcess = HMGRGetHandle((HOBJSTRUCT *) pCurrentProcess);

    if (pCurrentProcess->hProcess == INVALID_HANDLE_VALUE)
    {
        goto CLEANUP2;
    }

    if (0 != SYNCInitializeCriticalSection(&(pCurrentProcess->critSection)))
    {
        HMGRFreeHandle(pCurrentProcess->hProcess);        
        goto CLEANUP2;
    }
    
    /* Save information relative to the initial process */
    lpInitialProcInfo = (INITIALPROCINFO *) malloc(sizeof(INITIALPROCINFO));

    if (!lpInitialProcInfo)
    {
        ERROR("malloc() failed!\n");
        goto CLEANUP3;
    }

    pCurrentProcess->lpInitProcInfo = lpInitialProcInfo;

    /* Save the command line */
    lpInitialProcInfo->lpwstrCmdLine = lpwstrCmdLine;

    /* Save the initial directory */
    lpwstr=PAL_wcsrchr(lpwstrFullPath,'/');
    lpwstr[0]='\0';
    n=lstrlenW(lpwstrFullPath)+1;

    initial_dir = malloc(n*sizeof(WCHAR));
    if( initial_dir == NULL )
    {
        ERROR("malloc() failed!\n");
        goto CLEANUP4;
    }

    lstrcpyW(initial_dir, lpwstrFullPath);
    lpwstr[0]='/';

    pAppDir = initial_dir;
    
    /* Save the current directory */
    getcwd(lpstrCurDir, MAX_PATH);
    /* convert it to unicode */
    MultiByteToWideChar(CP_ACP, 0, lpstrCurDir, -1,
                        lpInitialProcInfo->lpwstrCurDir, MAX_PATH);

    /* create the initial thread */
    initial_thread = THREADCreateInitialThread();
    if(!initial_thread)
    {
        ERROR("Couldn't create initial thread structure\n");
        goto CLEANUP5;
    }
    if (!PROCAddThread(initial_thread))
    {
        ERROR("Couldn't add initial thread to process structure\n");
        goto CLEANUP6;
    }

    /* Set the current thread object */
    if (pthread_setspecific(thObjKey, initial_thread))
    {
        ERROR("Couldn't set the thread object key's value\n");
        goto CLEANUP6;
    }

    /* Add process to process list in shared memory (if necessary) */

    SHMLock();
    shmprocess_ptr = PROCFindSHMProcess(pCurrentProcess->processId);
    if(!shmprocess_ptr)
    {
        TRACE("Creating SHMPROCESS structure for process 0x%08x\n",
              pCurrentProcess->processId);
        shmprocess_ptr = (SHMPROCESS *)SHMPTR_TO_PTR(shmprocess);
        shmprocess_ptr->processId = pCurrentProcess->processId;
        shmprocess_ptr->attachCount = 0;
        shmprocess_ptr->attachedByProcId = 0;
        shmprocess_ptr->refcount = 0;
        shmprocess_ptr->info = pCurrentProcess;
        shmprocess_ptr->next = SHMGetInfo(SIID_PROCESS_INFO);
        shmprocess_ptr->self = shmprocess;

        SHMSetInfo(SIID_PROCESS_INFO, shmprocess);
    }
    else
    {
        TRACE("SHMPROCESS structure for process 0x%08x already exists\n",
              pCurrentProcess->processId);

        /* release pre-allocated structure */
        SHMfree(shmprocess);
    }

    pCurrentProcess->shmprocess = shmprocess_ptr;
    shmprocess_ptr->info = pCurrentProcess;
    shmprocess_ptr->refcount++;
    SHMRelease();
    bRet = TRUE;
    goto EXIT;

CLEANUP6:
        HMGRFreeHandle(initial_thread->hThread);
        free(initial_thread);
CLEANUP5:
        free(initial_dir);
        pAppDir =  NULL;
CLEANUP4:
        free (lpInitialProcInfo);
CLEANUP3:
        HMGRFreeHandle(pCurrentProcess->hProcess);        
        DeleteCriticalSection(&(pCurrentProcess->critSection));
CLEANUP2:        
        SHMfree(shmprocess);
CLEANUP1:        
        free (pCurrentProcess);
EXIT:
    return bRet;
}


/*++
Function:
  PROCCleanupInitialProcess

Abstract
  Cleanup all the structures for the initial process.

Parameter
  VOID

Return
  VOID

--*/
VOID
PROCCleanupInitialProcess(VOID)
{
    THREAD *pThread;
    LPWSTR lpwstr;

    if (!pCurrentProcess)
    {
        ASSERT("Unable to obtain the process structure\n");
        return;
    }

    SYNCEnterCriticalSection(&(pCurrentProcess->critSection), TRUE);

    /* Close the current thread handle . This will release the structure
       associated with it without terminating the process */

    pThread = pGThreadList;
    if (pThread != NULL)
    {
        HANDLE hThread = pThread->hThread;
        PROCRemoveThread(pThread);
        CloseHandle(hThread);
    }
    else
    {
        ASSERT("The thread list is empty\n");
    }

    /* Free the application directory */
    lpwstr=pAppDir;
    free (lpwstr);
    
    /* Free the initial process info structure */
    free (pCurrentProcess->lpInitProcInfo->lpwstrCmdLine);
    free (pCurrentProcess->lpInitProcInfo);

    SYNCLeaveCriticalSection(&(pCurrentProcess->critSection), TRUE);

    /* Let the Handle manager close the handle of the initial process */
}

/*++
Function:
  PROCAddThread

Abstract
  Add a thread to the thread list of the current process

Parameter
  lpThread:   Thread object

Return
  TRUE: if successful
  FALSE: otherwise
--*/
BOOL
PROCAddThread(
    THREAD *lpThread)
{
    BOOL bRet = FALSE;

    if (!pCurrentProcess)
    {
        ASSERT("Unable to get the current process object\n");
        return FALSE;
    }

    /* protect the access of the thread list with critical section for 
       mutithreading access */
    SYNCEnterCriticalSection(&(pCurrentProcess->critSection), TRUE);

    lpThread->next = pGThreadList;
    pGThreadList = lpThread;

    bRet = TRUE;

    TRACE("Thread (%#x) added to the process thread list\n",
          lpThread->dwThreadId);

    SYNCLeaveCriticalSection(&(pCurrentProcess->critSection), TRUE);
    return bRet;
}


/*++
Function:
  PROCRemoveThread

Abstract
  Remove a thread form the thread list of the current process

Parameter
  THREAD *lpThread : thread object to remove

(no return value)
--*/
void
PROCRemoveThread(
    THREAD *lpThread)
{
    THREAD *curThread, *prevThread;

    if (!pCurrentProcess)
    {
        ERROR("Unable to get the current process object\n");
        return;
    }

    /* protect the access of the thread list with critical section for 
       mutithreading access */
    SYNCEnterCriticalSection(&(pCurrentProcess->critSection), TRUE);

    curThread = pGThreadList;

    /* if thread list is empty */
    if (curThread == NULL)
    {
        ASSERT("Thread list is empty.\n");
        goto EXIT;
    }

    /* do we remove the first thread? */
    if (curThread == lpThread)
    {
        pGThreadList =  curThread->next;
        TRACE("Thread %p removed from the process thread list\n", lpThread);
        goto EXIT;
    }

    prevThread = curThread;
    curThread = curThread->next;
    /* find the thread to remove */
    while (curThread != NULL)
    {
        if (curThread == lpThread)
        {
            /* found, fix the chain list */
            prevThread->next = curThread->next;
            TRACE("Thread %p removed from the process thread list\n", lpThread);
            goto EXIT;
        }

        prevThread = curThread;
        curThread = curThread->next;
    }

    WARN("Thread %p not removed (it wasn't found in the list)\n", lpThread);

EXIT:
    SYNCLeaveCriticalSection(&(pCurrentProcess->critSection), TRUE);
}


/*++
Function:
  PROCGetNumberOfThreads

Abstract
  Return the number of threads in the thread list.

Parameter
  void

Return
  the number of threads.
--*/
INT
PROCGetNumberOfThreads(
    void)
{
    THREAD *pThread;
    INT count = 0;

    if (!pCurrentProcess)
    {
        return count;
    }

    /* protect the access of the thread list with critical section */
    SYNCEnterCriticalSection(&(pCurrentProcess->critSection), TRUE);

    pThread = pGThreadList;

    while (pThread != NULL)
    {
        count++;
        pThread = pThread->next;
    }

    SYNCLeaveCriticalSection(&(pCurrentProcess->critSection), TRUE);

    return count;
}


/*++
Function:
  PROCProcessLock

Abstract
  Enter the critical section associated to the current process

Parameter
  void

Return
  void
--*/
VOID
PROCProcessLock(
    VOID)
{
    if (!pCurrentProcess)
    {
        ASSERT("can't get data for current process!\n");
        return;
    }

    SYNCEnterCriticalSection(&pCurrentProcess->critSection, TRUE);
}


/*++
Function:
  PROCProcessUnlock

Abstract
  Leave the critical section associated to the current process

Parameter
  void

Return
  void
--*/
VOID
PROCProcessUnlock(
    VOID)
{
    if (!pCurrentProcess)
    {
        ASSERT("can't get data for current process!\n");
        return;
    }

    SYNCLeaveCriticalSection(&pCurrentProcess->critSection, TRUE);
}

/*++
PROCFindSHMProcess

Find a SHMPROCESS structure in shared memory

Parameters :
    DWORD dwProcessId : ID of process to look for

Return Value :
    Pointer to SHMPROCESS structure of process in shared memory
    (NULL if not found)
--*/
SHMPROCESS *PROCFindSHMProcess(DWORD dwProcessId)
{
    SHMPTR shmptr;
    SHMPROCESS *shmprocess = NULL;

    SHMLock();

    shmptr = SHMGetInfo(SIID_PROCESS_INFO);
    while(0 != shmptr)
    {
        shmprocess = SHMPTR_TO_PTR(shmptr);
        if(NULL == shmprocess)
        {
            ASSERT("found invalid non-NULL SHMPTR in process list!\n");
            shmptr = 0;
            break;
        }
        if(shmprocess->processId == dwProcessId)
        {
            break;
        }
        shmptr = shmprocess->next;
        shmprocess = NULL;
    }
    if(!shmprocess)
    {
        TRACE("Process ID 0x%08x was not found in shared memory\n",
              dwProcessId);
    }
    else
    {
        TRACE("Process ID 0x%08x was found in shared memory, SHMPTR is %#x\n",
              dwProcessId, shmptr);

    }
    SHMRelease();

    return shmprocess;
}


/* Internal function definitions **********************************************/

/*++
Function:
  getFileName

Abstract:
    Helper function for CreateProcessW, it retrieves the executable filename
    from the application name, and the command line.

Parameters:
    IN  lpApplicationName:  first parameter from CreateProcessW (an unicode string)
    IN  lpCommandLine: second parameter from CreateProcessW (an unicode string)
    OUT lpFileName: file to be executed (the new process)

Return:
    TRUE: if the file name is retrieved
    FALSE: otherwise

--*/
static
BOOL
getFileName(
       LPCWSTR lpApplicationName,
       LPWSTR lpCommandLine,
       char *lpPathFileName)
{
    LPWSTR lpEnd;
    WCHAR wcEnd;
    char lpFileName[MAX_PATH];
    char *lpTemp;

    if (lpApplicationName)
    {
        int path_size = MAX_PATH;
        lpTemp = lpPathFileName;
        /* if only a file name is specified, prefix it with "./" */
        if ((*lpApplicationName != '.') && (*lpApplicationName != '/') &&
            (*lpApplicationName != '\\'))
        {
            strcpy(lpPathFileName, "./");
            lpTemp+=2;
            path_size -= 2;
       }

        /* Convert to ASCII */
        if (!WideCharToMultiByte(CP_ACP, 0, lpApplicationName, -1,
                                 lpTemp, path_size, NULL, NULL))
        {
            ASSERT("WideCharToMultiByte failure\n");
            return FALSE;
        }

        /* Replace '\' by '/' */
        FILEDosToUnixPathA(lpPathFileName);

        return TRUE;
    }
    else
    {
        /* use the Command line */

        /* filename should be the first token of the command line */

        /* first skip all leading whitespace */
        lpCommandLine = UTIL_inverse_wcspbrk(lpCommandLine,W16_WHITESPACE);
        if(NULL == lpCommandLine)
        {
            ERROR("CommandLine contains only whitespace!\n");
            return FALSE;
        }

        /* check if it is starting with a quote (") character */
        if (*lpCommandLine == 0x0022)
        {
            lpCommandLine++; /* skip the quote */

            /* file name ends with another quote */
            lpEnd = PAL_wcschr(lpCommandLine+1, 0x0022);

            /* if no quotes found, set lpEnd to the end of the Command line */
            if (lpEnd == NULL)
                lpEnd = lpCommandLine + PAL_wcslen(lpCommandLine);
        }
        else
        {
            /* filename is end out by a whitespace */
            lpEnd = PAL_wcspbrk(lpCommandLine, W16_WHITESPACE);

            /* if no whitespace found, set lpEnd to end of the Command line */
            if (lpEnd == NULL)
            {
                lpEnd = lpCommandLine + PAL_wcslen(lpCommandLine);
            }
        }

        if (lpEnd == lpCommandLine)
        {
            ERROR("application name and command line are both empty!\n");
            return FALSE;
        }

        /* replace the last character by a null */
        wcEnd = *lpEnd;
        *lpEnd = 0x0000;

        /* Convert to ASCII */
        if (!WideCharToMultiByte(CP_ACP, 0, lpCommandLine, -1,
                                 lpFileName, MAX_PATH, NULL, NULL))
        {
            ASSERT("WideCharToMultiByte failure\n");
            return FALSE;
        }

        /* restore last character */
        *lpEnd = wcEnd;

        /* Replace '\' by '/' */
        FILEDosToUnixPathA(lpFileName);

        if (!getPath(lpFileName, MAX_PATH, lpPathFileName))
        {
            /* file is not in the path */
            return FALSE;
        }
    }
    return TRUE;
}

/*++
Functions: VAL16 & VAL32
   Byte swapping functions for reading in little endian format files
--*/
#ifdef BIGENDIAN

static inline USHORT    VAL16(USHORT x)
{
    return ( ((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8) );
}
static inline ULONG   VAL32(DWORD x)
{
    return( ((x & 0xFF000000L) >> 24) |
            ((x & 0x00FF0000L) >>  8) |
            ((x & 0x0000FF00L) <<  8) |
            ((x & 0x000000FFL) << 24) );
}
#else   // BIGENDIAN
// For little-endian machines, do nothing
static __inline USHORT  VAL16(unsigned short x) { return x; }
static __inline DWORD   VAL32(DWORD x){ return x; }
#endif  // BIGENDIAN

/*++
Function:
  isManagedExecutable

Determines if the passed in file is a managed executable

--*/
static
int
isManagedExecutable(LPSTR lpFileName)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD cbRead;
    IMAGE_DOS_HEADER        dosheader;
    IMAGE_NT_HEADERS32      NtHeaders; 
    BOOL ret = 0;

    /* then check if it is a PE/COFF file */ 
    if((hFile = CreateFileA(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                            NULL)) == INVALID_HANDLE_VALUE)
    {        
          goto isManagedExecutableExit;
    }
      
    /* Open the file and read the IMAGE_DOS_HEADER structure */ 
    if(!ReadFile(hFile, &dosheader, sizeof(IMAGE_DOS_HEADER), &cbRead, NULL) || cbRead != sizeof(IMAGE_DOS_HEADER) )
      goto isManagedExecutableExit;
       
    /* check the DOS headers */
    if ( (dosheader.e_magic != VAL16(IMAGE_DOS_SIGNATURE)) || (VAL32(dosheader.e_lfanew) <= 0) ) 
      goto isManagedExecutableExit;         
 
    /* Advance the file pointer to File address of new exe header */
    if( SetFilePointer(hFile, VAL32(dosheader.e_lfanew), NULL, FILE_BEGIN) == 0xffffffff)
      goto isManagedExecutableExit;
            
    if( !ReadFile(hFile, &NtHeaders , sizeof(IMAGE_NT_HEADERS32), &cbRead, NULL) || cbRead != sizeof(IMAGE_NT_HEADERS32) )
      goto isManagedExecutableExit;
   
    /* check the NT headers */   
    if ((NtHeaders.Signature != VAL32(IMAGE_NT_SIGNATURE)) ||
        (NtHeaders.FileHeader.SizeOfOptionalHeader != VAL16(IMAGE_SIZEOF_NT_OPTIONAL32_HEADER)) ||
        (NtHeaders.OptionalHeader.Magic != VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC)))
        goto isManagedExecutableExit;
     
    /* Check that the virtual address of IMAGE_DIRECTORY_ENTRY_COMHEADER is non-null */
    if ( NtHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress == 0 )
        goto isManagedExecutableExit;
  
    /* The file is a managed executable */
    ret =  1;
     
 isManagedExecutableExit:
    /* Close the file handle if we opened it */
    if ( hFile != INVALID_HANDLE_VALUE )
      CloseHandle(hFile);

    return ret;
}

/*++
Function:
    checkFileType

Abstract:
    Return the type of the file.

Parameters:
    IN  lpFileName:  file name

Return:
    FILE_DIR: Directory
    FILE_UNIX: Unix executable file
    FILE_PE: managed PE/COFF file
    FILE_ERROR: Error
--*/
static
int
checkFileType( char *lpFileName)
{ 
    struct stat stat_data;

    /* check if the file exist */
    if ( access(lpFileName, F_OK) != 0 )
    {
        return FILE_ERROR;
    }
    
    if( isManagedExecutable(lpFileName) )
    {
        return FILE_PE;
    }

    /* if it's not a PE/COFF file, check if it is executable */
    if ( -1 != stat( lpFileName, &stat_data ) )
    {

        if((stat_data.st_mode & S_IFMT) == S_IFDIR )
        {
            /*The given file is a directory*/
            return FILE_DIR;
        }
        if ( UTIL_IsExecuteBitsSet( &stat_data ) )
        {
            return FILE_UNIX;
        }
        else
        {
            return FILE_ERROR;
        }
    }
    return FILE_ERROR;

}


/*++
Function:
  buildArgv

Abstract:
    Helper function for CreateProcessW, it builds the array of argument in
    a format than can be passed to execve function.lppArgv is allocated
    in this function and must be freed by the caller.

Parameters:
    IN  lpCommandLine: second parameter from CreateProcessW (an unicode string)
    IN  lpAppPath: cannonical name of the application to launched
    OUT lppArgv: array of arguments to be passed to the new process
    IN  prependLoader:  If True first argument should be the PE loader

Return:
    the number of arguments

note: this doesn't yet match precisely the behavior of Windows, but should be 
sufficient.
what's here:
1) stripping nonquoted whitespace
2) handling of quoted parameters and quoted "parts" of parameters, removal of 
   doublequotes (<aaaa"b bbb b"ccc> becomes <aaaab bbb bccc>)
3) \" as an escaped doublequote, both within doublequoted sequences and out
what's known missing :
1) \\ as an escaped backslash, but only if the string of '\' 
   is followed by a " (escaped or not)                                       
2) "alternate" escape sequence : double-doublequote within a double-quoted 
    argument (<"aaa a""aa aaa">) expands to a single-doublequote(<aaa a"aa aaa>)
note that there may be other special cases
--*/
static
char **
buildArgv(
      LPCWSTR lpCommandLine,
      LPSTR lpAppPath,
      UINT *pnArg,
      BOOL prependLoader)
{
    UINT iWlen;
    char *lpAsciiCmdLine;
    char *pChar;
    char **lppArgv;
    char **lppTemp;
    UINT i,j;

    *pnArg = 0;

    iWlen = WideCharToMultiByte(CP_ACP,0,lpCommandLine,-1,NULL,0,NULL,NULL);

    if(0 == iWlen)
    {
        ASSERT("Can't determine length of command line\n");
        return NULL;
    }

    /* make sure to allocate enough space, up for the worst case scenario */
    lpAsciiCmdLine = (char *) malloc(iWlen
                                     + strlen(PROCESS_PELOADER_FILENAME)
                                     + strlen(lpAppPath) + 2);

    if (lpAsciiCmdLine == NULL)
    {
        ERROR("Unable to allocate memory\n");
        return NULL;
    }

    pChar = lpAsciiCmdLine;

    /* Prepend the PE loader, if it's required */
    if (prependLoader)
    {
        strcpy(lpAsciiCmdLine, PROCESS_PELOADER_FILENAME);
        strcat(lpAsciiCmdLine, " ");
        pChar = lpAsciiCmdLine + strlen (lpAsciiCmdLine);

    }
    else
    {
        /* put the cannonical name of the application as the first parameter */
        strcpy(lpAsciiCmdLine, "\"");
        strcat(lpAsciiCmdLine, lpAppPath);
        strcat(lpAsciiCmdLine, "\"");
        strcat(lpAsciiCmdLine, " ");
        pChar = lpAsciiCmdLine + strlen (lpAsciiCmdLine);

        /* let's skip the first argument in the command line */

        /* strip leading whitespace; function returns NULL if there's only 
           whitespace, so the if statement below will work correctly */
        lpCommandLine = UTIL_inverse_wcspbrk((LPWSTR)lpCommandLine, W16_WHITESPACE);

        if (lpCommandLine)
        {
            LPCWSTR stringstart = lpCommandLine;

            do
            {
                /* find first whitespace or dquote character */
                lpCommandLine = PAL_wcspbrk(lpCommandLine,W16_WHITESPACE_DQUOTE);
                if(NULL == lpCommandLine)
                {
                    /* no whitespace or dquote found : first arg is only arg */
                    break;
                }
                else if('"' == *lpCommandLine)
                {
                    /* got a dquote; skip over it if it's escaped; make sure we 
                       don't try to look before the first character in the 
                       string */
                    if(lpCommandLine > stringstart && '\\' == lpCommandLine[-1])
                    {
                        lpCommandLine++;
                        continue;
                    } 

                    /* found beginning of dquoted sequence, run to the end */
                    /* don't stop if we hit an escaped dquote */
                    lpCommandLine++;
                    while( *lpCommandLine )
                    {
                        lpCommandLine = PAL_wcschr(lpCommandLine, '"');
                        if(NULL == lpCommandLine)
                        {
                            /* no ending dquote, arg runs to end of string */
                            break;
                        }
                        if('\\' != lpCommandLine[-1])
                        {
                            /* dquote is not escaped, dquoted sequence is over*/
                            break;
                        } 
                        lpCommandLine++;
                    }   
                    if(NULL == lpCommandLine || '\0' == *lpCommandLine)
                    {
                        /* no terminating dquote */
                        break;
                    }

                    /* step over dquote, keep looking for end of arg */
                    lpCommandLine++;
                }
                else
                {
                    /* found whitespace : end of arg. */
                    lpCommandLine++;
                    break;
                }
            }while(lpCommandLine);
        }
    }

    /* Convert to ASCII */
    if (lpCommandLine)
    {
        if (!WideCharToMultiByte(CP_ACP, 0, lpCommandLine, -1,
                                 pChar, iWlen+1, NULL, NULL))
        {
            ASSERT("Unable to convert to a multibyte string\n");
            free (lpAsciiCmdLine);
            return NULL;
        }
    }

    pChar = lpAsciiCmdLine;

    /* loops through all the arguments, to find out how many arguments there 
       are; while looping replace whitespace by \0 */

    /* skip leading whitespace (and replace by '\0') */
    /* note : there shouldn't be any, command starts either with PE loader name 
       or computed application path, but this won't hurt */
    while (*pChar)
    {
        if (!isspace((unsigned char) *pChar))
        {
           break;
        }
        WARN("unexpected whitespace in command line!\n");
        *pChar++ = '\0';
    }

    while (*pChar)
    {
        (*pnArg)++;

        /* find end of current arg */
        while(*pChar && !isspace((unsigned char) *pChar))
        {
            if('"' == *pChar)
            {
                /* skip over dquote if it's escaped; make sure we don't try to 
                   look before the start of the string for the \ */
                if(pChar > lpAsciiCmdLine && '\\' == pChar[-1])
                {
                    pChar++;
                    continue;
                }
                
                /* found leading dquote : look for ending dquote */
                pChar++;
                while (*pChar)
                {
                    pChar = strchr(pChar,'"');
                    if(NULL == pChar)
                    {
                        /* no ending dquote found : argument extends to the end 
                           of the string*/
                        break;
                    }
                    if('\\' != pChar[-1])
                    {
                        /* found a dquote, and it's not escaped : quoted 
                           sequence is over*/
                        break;
                    }      
                    /* found a dquote, but it was escaped : skip over it, keep 
                       looking */
                    pChar++;
                }
                if(NULL == pChar || '\0' == *pChar)
                {
                    /* reached the end of the string : we're done */
                    break;
                }
            }
            pChar++;
        }
        if(NULL == pChar)
        {
            /* reached the end of the string : we're done */
            break;
        }
        /* reached end of arg; replace trailing whitespace by '\0', to split 
           arguments into separate strings */
        while (isspace((unsigned char) *pChar))
        {
            *pChar++ = '\0';
        }
    }

    /* allocate lppargv according to the number of arguments
       in the command line */
    lppArgv = (char **) malloc (((*pnArg)+1) * sizeof(char *));

    if (lppArgv == NULL)
    {
        free (lpAsciiCmdLine);
        return NULL;
    }

    lppTemp = lppArgv;

    /* at this point all parameters are separated by NULL
       we need to fill the array of arguments; we must also remove all dquotes 
       from arguments (new process shouldn't see them) */
    for (i = *pnArg, pChar = lpAsciiCmdLine; i; i--)
    {
        /* skip NULLs */
        while (!*pChar)
        {
            pChar++;
        }

        *lppTemp = pChar;

        /* go to the next parameter, removing dquotes as we go along */
        j = 0;
        while (*pChar)
        {
            /* copy character if it's not a dquote */
            if('"' != *pChar)
            {
                /* if it's the \ of an escaped dquote, skip over it, we'll 
                   copy the " instead */
                if( '\\' == pChar[0] && '"' == pChar[1] )
                {
                    pChar++;
                }
                (*lppTemp)[j++] = *pChar;
            }
            pChar++;
        }
        /* re-NULL terminate the argument */
        (*lppTemp)[j] = '\0';

        lppTemp++;
    }

    *lppTemp = NULL;

    return lppArgv;
}


/*++
Function:
  getPath

Abstract:
    Helper function for CreateProcessW, it looks in the path environment
    variable to find where the process to executed is.

Parameters:
    IN  lpFileName: file name to search in the path
    IN  iLen: length of lpPathFileName buffer
    OUT lpPathFileName: returned string containing the path and the filename

Return:
    TRUE if found
    FALSE otherwise
--*/
static
BOOL
getPath(
      LPCSTR lpFileName,
      UINT iLen,
      LPSTR  lpPathFileName)
{
    LPSTR lpPath;
    LPSTR lpNext;
    LPSTR lpCurrent;
    LPWSTR lpwstr;
    INT n;
    INT nextLen;
    INT slashLen;

    /* if a path is specified, only look there */
    if(strchr(lpFileName, '/'))
    {
        if (access (lpFileName, F_OK) == 0)
        {
            strcpy(lpPathFileName, lpFileName);
            TRACE("file %s exists\n", lpFileName);
            return TRUE;
        } 
        else
        {
            TRACE("file %s doesn't exist.\n", lpFileName);
            return FALSE;
        }
    }

    /* first look in directory from which the application loaded */
    lpwstr=pAppDir;

    /* convert path to multibyte, check buffer size */
    n = WideCharToMultiByte(CP_ACP, 0, lpwstr, -1, lpPathFileName, iLen,
                            NULL, NULL);
    if(n == 0)
    {
        ASSERT("WideCharToMultiByte failure!\n");
        return FALSE;
    }

    n += strlen(lpFileName) + 2;
    if( n > iLen )
    {
        ERROR("Buffer too small for full path!\n");
        return FALSE;
    }
    strcat(lpPathFileName, "/");
    strcat(lpPathFileName, lpFileName);
    if(access(lpPathFileName, F_OK) == 0)
    {
        TRACE("found %s in application directory (%s)\n", lpFileName, lpPathFileName);
        return TRUE;
    }

    /* then try the current directory */
    strcpy(lpPathFileName, "./");
    strcat(lpPathFileName, lpFileName);

    if (access (lpPathFileName, R_OK) == 0)
    {
        TRACE("found %s in current directory.\n", lpFileName);
        return TRUE;
    }

    /* Then try to look in the path */
    lpPath = (LPSTR) malloc(strlen(MiscGetenv("PATH"))+1);

    if (!lpPath)
    {
        ERROR("couldn't allocate memory for $PATH\n");
        return FALSE;
    }

    strcpy(lpPath, MiscGetenv("PATH"));

    lpNext = lpPath;

    /* search in every path directory */
    TRACE("looking for file %s in $PATH (%s)\n", lpFileName, lpPath);
    while (lpNext)
    {
        /* skip all leading ':' */
        while(*lpNext==':')
        {
            lpNext++;
        }
        
        /* search for ':' */
        lpCurrent = strchr(lpNext, ':');
        if (lpCurrent)
        {
            *lpCurrent++ = '\0';
        }
        
        nextLen = strlen(lpNext);
        slashLen = (lpNext[nextLen-1] == '/') ? 0:1;

        /* verify if the path fit in the OUT parameter */
        if (slashLen + nextLen + strlen (lpFileName) >= iLen)
        {
            free (lpPath);
            ERROR("buffer too small for full path\n");
            return FALSE;
        }

        strcpy (lpPathFileName, lpNext);

        /* append a '/' if there's no '/' at the end of the path */
        if ( slashLen == 1 )
        {
            strcat (lpPathFileName, "/");
        }
        
        strcat (lpPathFileName, lpFileName);

        if (access (lpPathFileName, F_OK) == 0)
        {
            TRACE("Found %s in $PATH element %s\n", lpFileName, lpNext);
            free (lpPath);
            return TRUE;
        }

        lpNext = lpCurrent;  /* search in the next directory */
    }

    free (lpPath);
    TRACE("File %s not found in $PATH\n", lpFileName);
    return FALSE;
}



/*++
Function:
  dupProcessHandle

Abstract:
    Duplicate a process handle

Parameters:
    IN  lpFileName: file name to search in the path
    IN  iLen: length of lpPathFileName buffer
    OUT lpPathFileName: returned string containing the path and the filename

Return:
    0: success
    -1: error
--*/
static
int
dupProcessHandle(
      HANDLE handle,
      HOBJSTRUCT *handle_data)
{
    PROCESS *pProcess;

    pProcess = (PROCESS *)handle_data;
                         
    if (!isValidProcessObject(pProcess))
    {
        ASSERT("Invalid process object\n");
        return -1;
    }

    SYNCEnterCriticalSection(&pProcess->critSection, TRUE);

    
    /* we need two refCount, because other processes could refere to
       SHMPROCESS struct in shared memory */
    pProcess->refCount++;
    SHMLock();
    pProcess->shmprocess->refcount++;
    SHMRelease();

    SYNCLeaveCriticalSection(&pProcess->critSection, TRUE);

    return 0;
}


/*++
Function:
  closeProcessHandle

Abstract:
    Close a process handle. Only process handles that have been duplicated
    could be close, other process handles could not be closed.

Parameters:
    IN  handle: process handle to close
    IN  handle_data: structure associated with this handle

Return:
    0: success
    -1: error
--*/
static
int
closeProcessHandle(
      HOBJSTRUCT *handle_data)
{
    PROCESS *pProcess;

    pProcess = (PROCESS *)handle_data;
                         
    if (!isValidProcessObject(pProcess))
    {
        ASSERT("Invalid process object\n");
        return -1;
    }

    SYNCEnterCriticalSection(&pProcess->critSection, TRUE);
    
    pProcess->refCount--;
    
    /* Release SHMPROCESS reference */
    SHMLock();
    pProcess->shmprocess->refcount--;

    if (pProcess->refCount == 0)
    {
        TRACE("Releasing process object (process=%p)\n", pProcess->hProcess);

        SYNCLeaveCriticalSection(&pProcess->critSection, TRUE);
        DeleteCriticalSection(&pProcess->critSection);


        /* If this is the last reference, we can remove the SHMPROCESS from the
           linked list */
        if(0 == pProcess->shmprocess->refcount)
        {
            SHMPROCESS *shmprocess_ptr;
            SHMPTR shmprocess;

            TRACE("Releasing last reference to SHMPROCESS for process %#x; "
                  "removing from shared memory\n", pProcess->processId);

            shmprocess = SHMGetInfo(SIID_PROCESS_INFO);
            shmprocess_ptr = SHMPTR_TO_PTR(shmprocess);

            /* Is it the first in the list? */
            if(shmprocess_ptr == pProcess->shmprocess)
            {
                /* remove it from the list by setting the next link as the new
                   list head */
                SHMSetInfo(SIID_PROCESS_INFO, shmprocess_ptr->next);
            }
            else
            {
                SHMPROCESS *shmprocess_temp;

                /* process isn't first in the list, walk the list to find it.*/
                while(0 != shmprocess_ptr->next)
                {
                    /* shmprocess_temp will point to the item right before the
                       one we are now examining */
                    shmprocess_temp = shmprocess_ptr;

                    /* shmprocess_ptr will point to the item we are examining */
                    shmprocess = shmprocess_ptr->next;
                    shmprocess_ptr = SHMPTR_TO_PTR(shmprocess);

                    /* Found it? */
                    if(shmprocess_ptr == pProcess->shmprocess)
                    {
                        /* remove it from the list by linking the one before it
                           to the one after it */
                        shmprocess_temp->next = shmprocess_ptr->next;
                        break;
                    }
                }
            }
            SHMfree(shmprocess);
        }
        free (pProcess);

        if (pProcess == pCurrentProcess)
        {
            /* at this point the current process shouldn't be used */
            pCurrentProcess = NULL;
        }
    }
    else
    {
        SYNCLeaveCriticalSection(&pProcess->critSection, TRUE);
    }

    SHMRelease();
    return 0;
}

/*++
Function :
    ProcessLocalToRemote

    Makes a Process handle accessible to another process using a Remote Handle

Parameters, 
    IN handle_data         : structure associated with this handle
    IN remote_handle_data  : structure associated with the remote handle

Return value : TRUE if functions succeeded, FALSE otherwise.

--*/
BOOL ProcessLocalToRemote(HOBJSTRUCT *handle_data, 
                          HREMOTEOBJSTRUCT *remote_handle_data)
{
    PROCESS* pProcess = (PROCESS *)handle_data;

    if(!isValidProcessObject(pProcess))
    {
        ASSERT("Handle %p is not a valid process object!\n", pProcess);
        return FALSE;
    }

    remote_handle_data->type = handle_data->type;
    remote_handle_data->ShmKernelObject = pProcess->shmprocess->self;
    
    /* Increment this Process shared memory refcount */
    SHMLock(); 
    ((PROCESS *)handle_data)->shmprocess->refcount++;
    SHMRelease();
    
    return TRUE;
}

/*++
Function :
    ProcessRemoteToLocal

    Creates a Process handle using a Remote Handle from another process

Parameters, 
    IN remote_handle_data  : structure associated with the remote handle
    
Returns an handle to the instantiated event object

--*/
HANDLE ProcessRemoteToLocal(HREMOTEOBJSTRUCT *remote_handle_data)
{    
    HANDLE hProcess = INVALID_HANDLE_VALUE;
    SHMPROCESS* pProcessInfo = NULL;
    SHMPTR shmpProcessInfo = remote_handle_data->ShmKernelObject;
    PROCESS* lpProcess = NULL;

    pProcessInfo = (SHMPROCESS *) SHMPTR_TO_PTR( shmpProcessInfo );
    if (pProcessInfo == NULL)
    {
        ASSERT("Invalid shared memory pointer\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        goto Exit;
    }

    /* Create the process object */
    lpProcess = (PROCESS *) malloc(sizeof(PROCESS));

    if (!lpProcess)
    {
        ERROR("malloc() failed to create PROCESS object; errno is %d (%s)\n",
              errno, strerror(errno));
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Exit;
    }
    
    if (0 != SYNCInitializeCriticalSection(&(lpProcess->critSection)))
    {
        ERROR("Failed to initialize critsect of PROCESS structure!\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        free(lpProcess);
        goto Exit;
    }

    /* fill the process structure */
    lpProcess->objHeader.type = HOBJ_PROCESS;
    lpProcess->objHeader.close_handle = closeProcessHandle;
    lpProcess->objHeader.dup_handle = dupProcessHandle;

    lpProcess->refCount = 1;
    lpProcess->lpInitProcInfo = NULL;
    lpProcess->exit_code = 0;
    lpProcess->state = PS_RUNNING;

    lpProcess->processId = pProcessInfo->processId;
    lpProcess->shmprocess = pProcessInfo;

    lpProcess->hProcess = HMGRGetHandle((HOBJSTRUCT *) lpProcess);
    if (lpProcess->hProcess == INVALID_HANDLE_VALUE)
    {
        ERROR("Unable to create a handle\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        DeleteCriticalSection(&(lpProcess->critSection));
        free(lpProcess);
        goto Exit;
    }

    hProcess = lpProcess->hProcess;

Exit:
    return hProcess;

}

/*++
Function:
    isValidProcessObject

Abstract:
    Verify if the object is a process, by checking the magic field

Parameters:
    IN  lpProcess: PROCESS object

Return:
    TRUE: if valid
    FALSE: otherwise
--*/
static
BOOL
isValidProcessObject(
      PROCESS *lpProcess)
{
    return (lpProcess->objHeader.type = HOBJ_PROCESS);
}

/*++
Function:
    PROCGetProcessStatus

Abstract:
    Retrieve process state information (state & exit code).

Parameters:
    DWORD process_id : PID of process to retrieve state for
    PROCESS_STATE *state : state of process (starting, running, done)
    DWORD *exit_code : exit code of process (from ExitProcess, etc.)

Return value :
    TRUE on success
--*/
static BOOL PROCGetProcessStatus( HANDLE hProcess, PROCESS_STATE *state,
                                  DWORD *exit_code)
{
    pid_t wait_retval;
    int status;
    PROCESS *process;

    // First, check if we already know the status of this process. This will be 
    // the case if this function has already been called for the same process.
    process = (PROCESS *)HMGRLockHandle2(hProcess, HOBJ_PROCESS);
    if(NULL == process)
    {
        ERROR("Can't get data for handle %p\n", hProcess);
        return FALSE;
    }
    
    SYNCEnterCriticalSection(&process->critSection, TRUE);
    
    if(PS_DONE == process->state)
    {
        TRACE("We already called waitpid() on process ID %#x; process has "
              "terminated, exit code is %d\n", 
              process->processId, process->exit_code);
        *state = process->state;
        *exit_code = process->exit_code;
        SYNCLeaveCriticalSection(&process->critSection, TRUE);
        HMGRUnlockHandle(hProcess,&process->objHeader);
        return TRUE;
    }

    /* By using waitpid(), we can even retrieve the exit code of a non-PAL 
       process. However, note that waitpid() can only provide the low 8 bits 
       of the exit code. This is all that is required for the PAL spec. */
    TRACE("Looking for status of process; trying wait()");

    while(1)
    {
        /* try to get state of process, using non-blocking call */
        wait_retval = waitpid(process->processId, &status, WNOHANG);
        
        if ( wait_retval == process->processId )
        {
            /* success; get the exit code */
            if ( WIFEXITED( status ) )
            {
                *exit_code = WEXITSTATUS(status);
                TRACE("Exit code was %d\n", *exit_code);
            }
            else
            {
                WARN("process terminated without exiting; can't get exit "
                     "code. faking it.\n");
                *exit_code = EXIT_FAILURE;
            }
            *state = PS_DONE;
        }
        else if (0 == wait_retval)
        {
            // The process is still running.
            TRACE("Process %#x is still active.\n", process->processId);
            *state = PS_RUNNING;
            *exit_code = 0;
        }
        else if (-1 == wait_retval)
        {
            // This might happen if waitpid() had already been called, but 
            // this shouldn't happen - we call waitpid once, store the 
            // result, and use that afterwards.
            // One legitimate cause of failure is EINTR; if this happens we 
            // have to try again. A second legitimate cause is ECHILD, which
            // happens if we're trying to retrieve the status of a currently-
            // running process that isn't a child of this process.
            if(EINTR == errno)
            {
                TRACE("waitpid() failed with EINTR; re-waiting");
                continue;
            }
            else if (ECHILD == errno)
            {
                TRACE("waitpid() failed with ECHILD; calling kill instead");
                if (kill(process->processId, 0) != 0)
                {
                    WARN("kill(pid, 0) failed; errno is %d (%s)\n",
                         errno, strerror(errno));
                    *exit_code = EXIT_FAILURE;
                    *state = PS_DONE;
                }
                else
                {
                    *state = PS_RUNNING;
                    *exit_code = 0;
                }
            }
            else
            {
                ASSERT("waitpid() failed (returned -1); errno is %d (%s)\n",
                       errno, strerror(errno));
                *exit_code = EXIT_FAILURE;
                *state = PS_DONE;
            }
        }
        else
        {
            ASSERT("waitpid returned unexpected value %d\n",wait_retval);
            *exit_code = EXIT_FAILURE;
            *state = PS_DONE;
        }
        // Break out of the loop in all cases except EINTR.
        break;
    }

    // Save the exit code for future reference (waitpid will only work once).
    if(PS_DONE == *state)
    {
        process->state = PS_DONE;
        process->exit_code = *exit_code;
    }
    TRACE( "State of process 0x%08x : %d (exit code %d)\n", 
           process->processId, *state, *exit_code );
    
    SYNCLeaveCriticalSection(&process->critSection, TRUE);
    HMGRUnlockHandle(hProcess,&process->objHeader);
    
    return TRUE;
}

/*++
Function:
    PROCSetExitCode

Abstract:
    Save the exit code of a process in its object's structure

Parameters:
    HANDE hProcess : handle of process to modify
    DWORD exit_code : exit code of process (waitpid)
    
Return value :
    TRUE on success
    
Notes :
    -The reason for this is that waitpid() only succeeds on a particular PID 
     the first time it is called, so we must save the exit code it returns for 
     future reference. (specifically, it is common to call WaitForSingleObject()
     on a process, followed by GetExitCodeProcess(); both would normally use 
     waitpid())
--*/
BOOL PROCSetProcessExitCode( HANDLE hProcess, DWORD exit_code)
{
    BOOL bRet;
    PROCESS *process;

    process = (PROCESS *)HMGRLockHandle2(hProcess, HOBJ_PROCESS);
    if(NULL == process)
    {
        ERROR("Unable to obtain process structure for handle %p!\n", hProcess);
        return FALSE;
    }

    SYNCEnterCriticalSection(&process->critSection, TRUE);
    if(PS_DONE == process->state)
    {
        ASSERT("Exit code has already been set for process %#x!\n", 
              process->processId);
        bRet = FALSE;
    }
    else
    {
        process->state = PS_DONE;
        process->exit_code = exit_code;
        bRet = TRUE;
    }
    SYNCLeaveCriticalSection(&process->critSection, TRUE);
    HMGRUnlockHandle(hProcess,&process->objHeader);
    return bRet;
}

#ifdef _DEBUG
void PROCDumpThreadList()
{
    THREAD *pThread;

    PROCProcessLock();

    TRACE ("Threads:{\n");

    pThread = pGThreadList;
    while (NULL != pThread)
    {
        TRACE ("    {pThr=%p tid=%d lwpid=%d hThread=%p state=%d finsusp=%d}\n",
               pThread, (int)pThread->dwThreadId, (int)pThread->dwLwpId, pThread->hThread, 
               (int)pThread->thread_state, (int)pThread->final_suspension);

        pThread = pThread->next;
    }
    TRACE ("Threads:}\n");
    
    PROCProcessUnlock();
}
#endif

/*++
Function:
  PROCSuspendOtherThreads

  Calls SuspendThread on all threads in the process, except the current 
  thread. Used by PAL_Terminate.

(no parameters, no return value)
--*/
void PROCSuspendOtherThreads(void)
{
    THREAD *pThread;
    HANDLE hThread = NULL;
    DWORD  dwThreadId = 0;
    DWORD  dwLwpId = 0;

    TRACE("Terminating all threads except this one...\n");

    while(1)
    {
        PROCProcessLock();

        pThread = pGThreadList;
        while (NULL != pThread)
        {
            /* skip the current thread */
            if (pThread->dwThreadId != GetCurrentThreadId())
            {
                /* skip already-suspended threads */
                if(!pThread->final_suspension)
                {
                    pThread->final_suspension = TRUE;                    
                    /* lock the object so it remains valid after we unlock the 
                       process */
                    pThread = (THREAD *)HMGRLockHandle2(
                                                pThread->hThread, HOBJ_THREAD);
                    if(NULL == pThread)
                    {
                        /* shouldn't happen; by locking the process list, we should have 
                        guaranteed that the handle was still valid */
                        ASSERT("couldn't lock a handle to a thread in the process list!\n");
                        /* we already flagged the object, so we won't hit it again if we 
                        keep going */
                        continue;
                    }
                    break;
                }
            }
            pThread = pThread->next;
        }
        /* unlock the process, we must not hold any critical sections when we 
           suspend the thread */

        if (pThread)
        {
            hThread = pThread->hThread; 
            dwThreadId = pThread->dwThreadId;
            dwLwpId = pThread->dwLwpId;
        }

        PROCProcessUnlock();

        if(NULL == pThread)
        {
            /* reached end of the list : all other threads have been suspended*/
            break;
        }

        TRACE("Suspending thread {tid=%u lwpid=%u hThread=%p pThread=%p} ...\n", 
              (unsigned int)dwThreadId, (unsigned int)dwLwpId, hThread, pThread);
        if (-1 == SuspendThread(hThread))
        {
            ERROR("Failed uspending thread {tid=%u lwpid=%u hThread=%p pThread=%p} with error 0x%x\n", 
                  (unsigned int)dwThreadId, (unsigned int)dwLwpId, hThread, pThread, GetLastError());
        }

        /* If we reached here, pThread is good and still locked */
        HMGRUnlockHandle(hThread,&pThread->objHeader);
    }
    TRACE("All threads except this one are now suspended\n");
}

/*++
Function:
  PROCCondemnOtherThreads

  Set the waiting state of other threads to TWS_EARLYDEATH; this will prevent 
  signaled objects from trying to wake up other threads during process termination

(no parameters, no return value)
--*/
void PROCCondemnOtherThreads(void)
{
    THREAD *pThread;
    DWORD *pWaitState;

    TRACE("Marking all other threads as suspended...\n");
    PROCProcessLock();

    pThread = pGThreadList;
    while (NULL != pThread)
    {
        /* skip the current thread */
        if (pThread->dwThreadId != GetCurrentThreadId())
        {
            pWaitState = SHMPTR_TO_PTR(pThread->waitAwakened);
            *pWaitState = TWS_EARLYDEATH;
        }
        pThread = pThread->next;
    }
    PROCProcessUnlock();
    TRACE("All threads except this one are now condemned\n");
}


/*++
Function:
  TerminateCurrentProcessNoExit

Abstract:
    Terminate current Process, but leave the caller alive

Parameters:
    BOOL bTerminateUnconditionally - If this is set, the PAL will exit as
    quickly as possible. In particular, it will not unload DLLs.

Return value :
    No return

Note:
  This function is used in TerminateCurrentThread and TerminateProcess

--*/
void
TerminateCurrentProcessNoExit(BOOL bTerminateUnconditionally)
{
    BOOL locked;
    DWORD old_terminator;

    old_terminator = InterlockedCompareExchange(&terminator,
                                                    GetCurrentThreadId(),0);

    if(0 != old_terminator && GetCurrentThreadId() != old_terminator)
    {
        /* another thread has already initiated the termination process. we
        could just block on the PALInitLock critical section, but then
        PROCSuspendOtherThreads would hang... so sleep forever here, we're
        terminating anyway */
        /* note that if *this* thread has already started the termination
        process, we want to proceed. the only way this can happen is if a
        call to DllMain (from ExitProcess) brought us here (because DllMain
        called ExitProcess, or TerminateProcess, or ExitThread);
        TerminateProcess won't call DllMain, so there's no danger to get
        caught in an infinite loop */
         WARN("termination already started from another thread; blocking.\n");
         poll(NULL,0,INFTIM);
     }

     /* Try to lock the initialization count to prevent multiple threads from
     terminating/initializing the PAL simultaneously */
     /* note : it's also important to take this lock before the process lock,
     because Init/Shutdown take the init lock, and the functions they call
     may take the process lock. We must do it in the same order to avoid
     deadlocks */
     locked = PALInitLock();
     if(locked && PALIsInitialized())
     {
         PROCCleanupProcess(bTerminateUnconditionally);
     }
}
