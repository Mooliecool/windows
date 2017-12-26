/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    init/pal.c

Abstract:

    Implementation of PAL exported functions not part of the Win32 API.

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/shmemory.h"
#include "pal/handle.h"
#include "pal/critsect.h"
#include "pal/thread.h"
#include "pal/module.h"
#include "pal/seh.h"
#include "pal/virtual.h"
#include "pal/cruntime.h"
#include "pal/map.h"
#include "pal/socket2.h"
#include "pal/mutex.h"
#include "pal/misc.h"
#include "pal/utils.h"
#include "pal/file.h"
#include "pal/debug.h"
#include "pal/locale.h"
#include "pal/init.h"
#include "pal/identity.h"

#if HAVE_MACH_EXCEPTIONS
#include "../exception/machexception.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/stat.h>

#if defined(__ppc__)
#include <sys/sysctl.h>
int CacheLineSize;
#endif //__ppc__


SET_DEFAULT_DEBUG_CHANNEL(PAL);

volatile INT init_count = 0;
volatile BOOL shutdown_intent = 0;

/* critical section to protect access to init_count. This is allocated on the
   very first PAL_Initialize call, and is freed afterward. */
static PCRITICAL_SECTION init_critsec = NULL;

static char PALConfigDirName[MAX_PATH];
static int PALUserDirNameLength; // length of the path, incl. the '\0'
static WCHAR PALUserDirName[MAX_PATH];
static WCHAR NameManglerW[MAX_PATH];  // Unicode hashed path to the PAL
static int NameManglerLength;
static char PALLibRotorPalName[MAX_PATH];

static BOOL INIT_IncreaseDescriptorLimit(void);
static LPWSTR INIT_FormatCommandLine (int argc,char **argv);
static LPWSTR INIT_FindEXEPath(LPCSTR exe_name);

#ifdef _DEBUG
extern void PROCDumpThreadList(void);
#endif

char g_ExePath[MAX_PATH];

/*++
Function:
  PAL_Initialize

Abstract:
  This function is the first function of the PAL to be called.
  Internal structure initialization is done here. It could be called
  several time by the same process, a reference count is kept.

Return:
  0 if successful
  -1 if it failed

--*/
int
PALAPI
PAL_Initialize(
            int argc,
            char *argv[])
{
    LPWSTR command_line = NULL;
    LPWSTR exe_path = NULL;
    int retval = -1;

    /* the first ENTRY within the first call to PAL_Initialize is a special
       case, since debug channels are not initialized yet. So in that case the
       ENTRY will be called after the DBG channels initialization */
    ENTRY("PAL_Initialize(argc = %d argv = %p)\n", argc, argv);
    /*Firstly initiate a temporary lastError storage */
    StartupLastError = 0;

    if(NULL == init_critsec)
    {
        PCRITICAL_SECTION temp_critsec;
        temp_critsec = malloc(sizeof(CRITICAL_SECTION));
        if(NULL == temp_critsec)
        {
            /* note that these macros probably won't output anything, since the
               debug channels haven't been initialized yet */
            ERROR("couldn't allocate critical section!\n");
            LOGEXIT("PAL_Initialize returns %d.\n",-1);
            return -1;
        }

        if (0 != SYNCInitializeCriticalSection(temp_critsec))
        {
            ERROR("couldn't initialize critical section!\n");
            free(temp_critsec);
            LOGEXIT("PAL_Initialize returns %d.\n",-1);
            return -1;
        }

        if(NULL != InterlockedCompareExchangePointer((LPVOID *)&init_critsec,
                                                      temp_critsec, NULL))
        {
            /* another thread got in before us! shouldn't happen, if the PAL
               isn't initialized there shouldn't be any other threads */
            WARN("Another thread initialized the critical section\n");
            DeleteCriticalSection(temp_critsec);
            free(temp_critsec);
        }
    }

    SYNCEnterCriticalSection(init_critsec, TRUE);
    if(init_count==0)
    {
        // Set our pid.
        gPID = getpid();

#if defined(__ppc__)
        {
            int mib[2];
            size_t len;

            /* Determine the processor's cache line size, for
               FlushInstructionCache */
            mib[0] = CTL_HW;
            mib[1] = HW_CACHELINE;
            len = sizeof(CacheLineSize);
            if (sysctl(mib, 2, &CacheLineSize, &len, NULL, 0) == -1) {
                goto done;
            }
        }
#endif //__ppc__

        // Initialize the TLS lookaside cache
        if (FALSE == TLSInitialize())
        {
            goto done;
        }
    
        // Initialize the environment.
        if (FALSE == MiscInitialize())
        {
            goto done;
        }

        // Initialize debug channel settings before anything else.
        // This depends on the environment, so it must come after
        // MiscInitialize.
        if (FALSE == DBG_init_channels())
        {
            goto done;
        }

#if _DEBUG
        // Verify that our page size is what we think it is. If it's
        // different, we can't run.
        if (VIRTUAL_PAGE_SIZE != getpagesize())
        {
            ASSERT("VIRTUAL_PAGE_SIZE is incorrect for this system!\n"
                   "Change include/pal/virtual.h and clr/src/inc/stdmacros.h "
                   "to reflect the correct page size of %d.\n", getpagesize());
        }
#endif  // _DEBUG
    
        /* Output the ENTRY here, since it doesn't work before initializing
           debug channels */
        ENTRY("PAL_Initialize(argc = %d argv = %p)\n", argc, argv);

        if(argc<1 || argv==NULL)
        {
            ERROR("First-time initialization attempted with bad parameters!\n");
            goto done;
        }
        
        if (!INIT_IncreaseDescriptorLimit())
        {
            ERROR("Unable to increase the file descriptor limit!\n");
            // We can continue if this fails; we'll just have problems if
            // we use large numbers of threads or have many open files.
        }

        if( !CODEPAGEInit() )
        {
            ERROR( "Unable to initialize the locks or the codepage.\n" );
            goto done;
        }

        /* Create user's configuration directory if*/
        if(FALSE == INIT_InitPalConfigDir())
        {
            ERROR("Unable to setup PAL configuration directory!\n");
            goto CLEANUP0;
        }

        /* initialize the shared memory infrastructure */
        if(!SHMInitialize())
        {
            ERROR("Shared memory initialization failed!\n");
            INIT_RemovePalConfigDir();
            goto CLEANUP0;
        }

        /* initialize handle manager */
        if(!HMGRInitHandleManager())
        {
            ERROR("Handle manager initialization failed!\n");
            goto CLEANUP1;
        }

        /* build the command line */
        command_line=INIT_FormatCommandLine(argc,argv);

        /* find out the application's full path */
        exe_path=INIT_FindEXEPath(argv[0]);
        if (!WideCharToMultiByte (CP_ACP, 0, exe_path, -1, g_ExePath,
                                 sizeof (g_ExePath), NULL, NULL)) {
            ERROR("Failed to store process executable path\n");
            goto CLEANUP2;
        }
            

        if(NULL == command_line || NULL == exe_path)
        {
            ERROR("Failed to process command-line parameters!\n");
            goto CLEANUP2;
        }


        /* Initialize the mutex critical sections. */
        if (FALSE == MutexInitialize())
        {
            ERROR("Failed to initialize mutex support!\n");
            goto CLEANUP2;
        }

        /* initialize structure for the inital process */
        if (FALSE == PROCCreateInitialProcess(command_line, exe_path) )
        {
            ERROR("Unable to Create the initial process\n");
            goto CLEANUP4;
        }

        /* initialize structured exception handling stuff (signals, etc) */
        if (FALSE == SEHInitialize())
        {
            ERROR("Unable to initialize SEH support\n");
            goto CLEANUP5;
        }

        /* Initialize the File mapping critical section. */
        if (FALSE == MAPInitialize())
        {
            ERROR("Unable to initialize file mapping support\n");
            goto CLEANUP6;
        }

        /* initialize module manager */
        if(!LOADInitializeModules(exe_path))
        {
            ERROR("Unable to initialize module manager\n");
            goto CLEANUP8;
        }
    
        /* Initialize the Virtual* functions. */
        if (FALSE == VIRTUALInitialize())
        {
            ERROR("Unable to initialize virtual memory support\n");
            goto CLEANUP10;
        }


        /* initialize the socket worker thread */
        if(!SOCKInitWinSock())
        {
            ERROR("Unable to initialize socket worker thread\n");
            goto CLEANUP12;
        }

        /* create file objects for standard handles */
        if(!FILEInitStdHandles())
        {
            ERROR("Unable to initialize standard file handles\n");
            goto CLEANUP13;
        }

        /* Initialize the  MsgBox functions. */
        if (FALSE == MsgBoxInitialize())
        {
            ERROR("Unable to initialize MsgBox management\n");
            goto CLEANUP14;
        }

        if ( !CRTInitStdStreams() )
        {
            ERROR("Unable to initialize CRT standard streams\n");
            goto CLEANUP15;
        }

        /* Initialize the WaitForMultipleObjectsEx critical section */
        if (FALSE == WFMOInitialize())
        {
            ERROR("Unable to initialize object waiting support\n");
            goto CLEANUP16;
        }

        // Initialize the identity subsystem.
        // This depends on debug support, critical sections and MultiByteToWideChar.
        if (FALSE == IdentityInitialize())
        {
            ERROR( "Unable to initialize the identity subsystem!\n");
            goto done;
        }

        TRACE("First-time PAL initialization complete.\n");
        init_count++;

        /* Set LastError to a non-good value - functions within the
           PAL startup may set lasterror to a nonzero value. */
        SetLastError(NO_ERROR);

        retval = 0;
    }
    else
    {
        init_count++;

        TRACE("Initialization count increases to %d\n", init_count);
        retval = 0;
    }
    goto done;

CLEANUP16:
    /* No cleanup required for CRTInitStdStreams */ 
CLEANUP15:
    MsgBoxCleanup();
CLEANUP14:
    FILECleanupStdHandles();
CLEANUP13:
    SOCKTerminateWinSock();
CLEANUP12:
    VIRTUALCleanup();
CLEANUP10:
    LOADFreeModules(TRUE);
CLEANUP8:
    MAPCleanup();
CLEANUP6:
    SEHCleanup();
CLEANUP5:
    PROCCleanupInitialProcess();
CLEANUP4:
    FMTMSG_FormatMessageCleanUp();
    MutexCleanup();
CLEANUP2:
    free(command_line);
    free(exe_path);
    HMGRStopHandleManager();
CLEANUP1:
    SHMCleanup();
    INIT_RemovePalConfigDir();
CLEANUP0:
    CODEPAGECleanup();

    ERROR("PAL_Initialize failed\n");
done:

    SYNCLeaveCriticalSection(init_critsec, TRUE);
    LOGEXIT("PAL_Initialize returns int %d\n", retval);
    return retval;
}

/*++
Function:
  PAL_EntryPoint

Abstract:
  This function should be used to wrap code that uses PAL library on thread that was not created by PAL.
--*/
PALIMPORT
DWORD_PTR
PALAPI
PAL_EntryPoint(
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter)
{
    THREAD *lpThread;
    DWORD_PTR retval = -1;

    ENTRY("PAL_EntryPoint(lpStartAddress=%p, lpParameter=%p)\n", lpStartAddress, lpParameter);

    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        /* This function works only for thread that called PAL_Initialize for now. */
        ERROR( "Unable to get the thread object.\n" );
        goto done;
    }

    /* flush the TLS cache. */
    pthread_cleanup_push((void (__cdecl *)(void *))TLSCacheFlush, lpThread);

    retval = (*lpStartAddress)(lpParameter);

    pthread_cleanup_pop(1);

done:
    LOGEXIT("PAL_EntryPoint returns int %d\n", retval);
    return retval;
}

/*++
Function:
  PALCommonCleanup

Utility function to free any resource used by the PAL. 

Parameters :
    step: selects the desired cleanup step
    full_cleanup:  FALSE: cleanup only what's needed and leave the rest 
                          to the OS process cleanup
                   TRUE:  full cleanup 
--*/
void 
PALCommonCleanup(PALCLEANUP_STEP step, BOOL full_cleanup)
{
    static int step_done[PALCLEANUP_STEP_INVALID] = { 0 };

    switch (step)
    {
    case PALCLEANUP_ALL_STEPS:
    case PALCLEANUP_STEP_ONE:
        /* Note: in order to work correctly, this step should be ececuted with 
           init_count > 0
         */
        if (!step_done[PALCLEANUP_STEP_ONE])
        {
            step_done[PALCLEANUP_STEP_ONE] = 1;

            PALSetShutdownIntent();

            /* Stop the WFMO worker thread. Note that when PAL_Terminate is called, 
               there shouldn't be any other application thread running, so we can 
               stop the worker thread without fear that someone else is using it */
            WFMOCleanup();

#ifdef _DEBUG
            PROCDumpThreadList();
#endif

            TRACE("About to suspend every other thread\n");

            /* prevent other threads from acquiring signaled objects */
            PROCCondemnOtherThreads();
            /* prevent other threads from using services we're shutting down */
            PROCSuspendOtherThreads();

            TRACE("Every other thread suspended until exit\n");
        }

        /* Fall down for PALCLEANUP_ALL_STEPS */
        if (PALCLEANUP_ALL_STEPS != step)
            break;

    case PALCLEANUP_STEP_TWO:
        if (!step_done[PALCLEANUP_STEP_TWO])
        {
            step_done[PALCLEANUP_STEP_TWO] = 1;

            WFMOPostCleanup();

            /* LOADFreeeModules needs to be called before unitializing the rest
               of the PAL since it could result in calling DllMain for loaded
               libraries. For the user DllMain, all PAL APIs should still be
               functional. */
            LOADFreeModules(FALSE);

            /* release all mutexes held by this process */
            MutexAbandonAllMutexes();


            if (full_cleanup)
            {
                /* close primary handles of standard file objects */
                FILECleanupStdHandles();
                /* This unloads the palrt so, during its unloading, they
                   can call any number of APIs, so we have to be active for it to work. */
                FMTMSG_FormatMessageCleanUp();
                SOCKTerminateWinSock();
                VIRTUALCleanup();
                /* SEH requires information from the process structure to work;
                   LOADFreeModules requires SEH to be functional when calling DllMain.
                   Therefore SEHCleanup must go between LOADFreeModules and
                   PROCCleanupInitialProcess */
                SEHCleanup();
                PROCCleanupInitialProcess();
            }

            HMGRStopHandleManager();

            if (full_cleanup)
            {
                /* It needs to be done after stopping the handle manager, because
                   the cleanup will delete the critical section which is used
                   when closing the handle of a file mapping */
                MAPCleanup();
                MutexCleanup();
            }

            SHMCleanup();

            if (full_cleanup)
            {
                MsgBoxCleanup();
                IdentityCleanup();
                MiscCleanup();
                TIMECleanUpTransitionDates();
                CODEPAGECleanup();
                TLSCleanup();
            }
            TRACE("PAL Terminated.\n");
        }
        break;

    default:
        ASSERT("Unknown final cleanup step %d", step);
        break;
    }
}

/*++
Function:
  PAL_Terminate

Abstract:
  This function is the called when a thread has finished using the PAL
  library.
--*/
void
PALAPI
PAL_Terminate(
          void)
{
    ENTRY("PAL_Terminate()\n");

    if(NULL == init_critsec)
    {
        /* note that these macros probably won't output anything, since the
           debug channels haven't been initialized yet */
        ASSERT("PAL_Initialize has never been called!\n");
        LOGEXIT("PAL_Terminate returns.\n");
        return;
    }
    SYNCEnterCriticalSection(init_critsec, TRUE);

    init_count--;
    TRACE("Initialization count drops to %d\n", init_count);

    if(init_count<0)
    {
        /* this is actually legal : on ExitProcess, PALShutdown takes init_count down to 
           zero, then, during the native exit, PAL_Terminate is called as atexit handler
           and it brings the count to -1. 
           ROTORTODO: is this acceptable? 
        */
        /* WARN("PAL initialization count dropped below zero (this is not illegal)\n"); 
        */
    }
        
    if(init_count <=0)
    {
        /* Do the actual cleanup */
        PALCommonCleanup(PALCLEANUP_ALL_STEPS, FALSE);
    }

    SYNCLeaveCriticalSection(init_critsec, TRUE);
    LOGEXIT("PAL_Terminate returns\n");

    /* Do this after the ENTRY message so it will make it out */
    if(init_count==0)
        DBG_close_channels();
}

/*++
Function:
  PAL_InitializeDebug

Abstract:
  This function is the called when cordbg attaches to the process.
--*/
void
PALAPI
PAL_InitializeDebug(
          void)
{
    PERF_ENTRY(PAL_InitializeDebug);
    ENTRY("PAL_InitializeDebug()\n");
#if HAVE_MACH_EXCEPTIONS
    MachExceptionInitializeDebug();
#endif
    LOGEXIT("PAL_InitializeDebug returns\n");
    PERF_EXIT(PAL_InitializeDebug);
}

/*++
Function:
  PALShutdown

  sets the PAL's initialization count to zero, so that PALIsInitialized will 
  return FALSE. called by PROCCleanupProcess to tell some functions that the
  PAL isn't fully funcitonal, and that they should use an alternate code path
  (example : SYNCEnterCriticalSection)
  
(no parameters, no retun vale)
--*/
void
PALShutdown(
          void)
{
    init_count = 0;
}

BOOL
PALIsShuttingDown()
{
    /* ROTORTODO: This function may be used to provide a reader/writer-like
       mechanism (or a ref counting one) to prevent PAL APIs that need to access 
       PAL runtime data, from working when PAL is shutting down. Each of those API 
       should acquire a read access while executing. The shutting down code would
       acquire a write lock, i.e. suspending any new incoming reader, and waiting 
       for the current readers to be done. That would allow us to get rid of the
       dangerous suspend-all-other-threads at shutdown time */
    return shutdown_intent;
}

void
PALSetShutdownIntent()
{
    /* ROTORTODO: See comment in PALIsShuttingDown */
    shutdown_intent = TRUE;
}


/*++
Function:
  PAL_GetUserConfigurationDirectoryW

Abstract:
  Returns the user configuration directory, which is a ".rotor"
  subdirectory of the user's home directory. 

  ~/.rotor is created if it doesn't exist. 
        -- We don't do this, instead we create it in INIT_InitPalConfigDir()
--*/
BOOL
PALAPI
PAL_GetUserConfigurationDirectoryW(
                   OUT LPWSTR lpDirectoryName,
                   IN UINT cbDirectoryName)
{
    BOOL retval;

    PERF_ENTRY(PAL_GetUserConfigurationDirectoryW);
    ENTRY("PAL_GetUserConfigurationDirectoryW (lpDirectoryName=%p "
          "cbDirectoryName=%d)\n", lpDirectoryName, cbDirectoryName);

    if (PALUserDirNameLength >= cbDirectoryName)
    {
        retval = FALSE;
    }
    else
    {
        PAL_wcscpy(lpDirectoryName, PALUserDirName);
        retval = TRUE;
    }
    
    LOGEXIT("PAL_GetUserConfigurationDirectoryW returns BOOL %d\n", retval);
    PERF_EXIT(PAL_GetUserConfigurationDirectoryW);
    return(retval);
}

/*++
Function:
  PALGetPalConfigDir

Retrieves the path of directory in which per-user PAL data is placed
(file mapping page files, etc)

Parameters :
    LPSTR dest : buffer in which path is stored (must be at least MAX_PATH)

Returns TRUE on success, FALSE otherwise.

Note :
    This function must be functional even if the PAL isn't initialized.
--*/
BOOL PALGetPalConfigDir( LPSTR dest, UINT nBufferLength )
{
    if (strlen(PALConfigDirName) >= nBufferLength) 
    {
        ASSERT( "The buffer was invalid, or too small!\n" );
        return FALSE;
    }
    strcpy(dest, PALConfigDirName);
    return TRUE;
}

/*++
Function:
  PALGetLibRotorPalName

    Retrieves the full name of the librotorpal.so

Parameters :
    LPSTR dest : buffer in which path is stored (must be at least MAX_PATH)

Returns TRUE on success, FALSE otherwise.
--*/
BOOL PALGetLibRotorPalName( LPSTR dest, UINT nBufferLength )
{
    if (strlen(PALLibRotorPalName) >= nBufferLength) 
    {
        ASSERT( "The buffer was invalid, or too small!\n" );
        return FALSE;
    }
    strcpy(dest, PALLibRotorPalName);
    return TRUE;
}


/*++
Function:
  PALInitLock

Take the initializaiton critical section (init_critsec). necessary to serialize 
TerminateProcess along with PAL_Terminate and PAL_Initialize

(no parameters)

Return value :
    TRUE if critical section existed (and was acquired)
    FALSE if critical section doens't exist yet
--*/
BOOL PALInitLock(void)
{
    if(!init_critsec)
    {
        return FALSE;
    }
    SYNCEnterCriticalSection(init_critsec, TRUE);
    return TRUE;
}

/*++
Function:
  PALInitUnlock

Release the initialization critical section (init_critsec). 

(no parameters, no return value)
--*/
void PALInitUnlock(void)
{
    if(!init_critsec)
    {
        return;
    }
    SYNCLeaveCriticalSection(init_critsec, TRUE);
}

/* Internal functions *********************************************************/

/*++
Function:
    INIT_IncreaseDescriptorLimit [internal]

Abstract:
    Calls setrlimit(2) to increase the maximum number of file descriptors
    this process can open.

Return value:
    TRUE if the call to setrlimit succeeded; FALSE otherwise.
--*/
static BOOL INIT_IncreaseDescriptorLimit(void)
{
    struct rlimit rlp;
    int result;
    
    result = getrlimit(RLIMIT_NOFILE, &rlp);
    if (result != 0)
    {
        return FALSE;
    }
    // Set our soft limit for file descriptors to be the same
    // as the max limit.
    rlp.rlim_cur = rlp.rlim_max;
    result = setrlimit(RLIMIT_NOFILE, &rlp);
    if (result != 0)
    {
        return FALSE;
    }

    return TRUE;
}

/*++
Function:
    INIT_FormatCommandLine [Internal]

Abstract:
    This function converts an array of arguments (argv) into a Unicode
    command-line for use by GetCommandLineW

Parameters :
    int argc : number of arguments in argv
    char **argv : argument list in an array of NULL-terminated strings

Return value :
    pointer to Unicode command line. This is a buffer allocated with malloc;
    caller is responsible for freeing it with free()

Note : not all peculiarities of Windows command-line processing are supported; 

-what is supported :
    -arguments with white-space must be double quoted (we'll just double-quote
     all arguments to simplify things)
    -some characters must be escaped with \ : particularly, the double-quote,
     to avoid confusion with the double-quotes at the start and end of
     arguments, and \ itself, to avoid confusion with escape sequences.
-what is not supported:    
    -under Windows, \\ is interpreted as an escaped \ ONLY if it's followed by
     an escaped double-quote \". \\\" is passed to argv as \", but \\a is
     passed to argv as \\a... there may be other similar cases
    -there may be other characters which must be escaped 
--*/
static LPWSTR INIT_FormatCommandLine (int argc,char **argv)
{
    LPWSTR retval;
    LPSTR command_line=NULL, command_ptr, arg_ptr;
    INT length, i,j;
    BOOL bQuoted = FALSE;

    /* list of characters that need no be escaped with \ when building the
       command line. currently " and \ */
    LPCSTR ESCAPE_CHARS="\"\\";

    /* allocate temporary memory for the string. Play it safe :
       double the length of each argument (in case they're composed
       exclusively of escaped characters), and add 3 (for the double-quotes
       and separating space). This is temporary anyway, we return a LPWSTR */
    length=0;
    for(i=0; i<argc; i++)
    {
        TRACE("argument %d is %s\n", i, argv[i]);
        length+=3;
        length+=strlen(argv[i])*2;
    }
    command_line=malloc(length);

    if(!command_line)
    {
        ERROR("couldn't allocate memory for command line!\n");
        return NULL;
    }

    command_ptr=command_line;
    for(i=0; i<argc; i++)
    {
        /* double-quote at beginning of argument containing at leat one space */
        for(j = 0; (argv[i][j] != 0) && (!isspace((unsigned char) argv[i][j])); j++);

        if (argv[i][j] != 0)
        {
            *command_ptr++='"';
            bQuoted = TRUE;
        }
        /* process the argument one character at a time */
        for(arg_ptr=argv[i]; *arg_ptr; arg_ptr++)
        {
            /* if character needs to be escaped, prepend a \ to it. */
            if( strchr(ESCAPE_CHARS,*arg_ptr))
            {
                *command_ptr++='\\';
            }

            /* now we can copy the actual character over. */
            *command_ptr++=*arg_ptr;
        }
        /* double-quote at end of argument; space to separate arguments */
        if (bQuoted == TRUE)
        {
            *command_ptr++='"';
            bQuoted = FALSE;
        }
        *command_ptr++=' ';
    }
    /* replace the last space with a NULL terminator */
    command_ptr--;
    *command_ptr='\0';

    /* convert to Unicode */
    i = MultiByteToWideChar(CP_ACP, 0,command_line, -1, NULL, 0);
    if (i == 0)
    {
        ASSERT("MultiByteToWideChar failure\n");
        free(command_line);
        return NULL;
    }

    retval=malloc(sizeof(WCHAR)*i);
    if(retval == NULL)
    {
        ERROR("can't allocate memory for Unicode command line!\n");
        free(command_line);
        return NULL;
    }

    if(!MultiByteToWideChar(CP_ACP, 0,command_line, i, retval, i))
    {
        ASSERT("MultiByteToWideChar failure\n");
        free(retval);
        retval = NULL;
    }
    else
        TRACE("Command line is %s\n", command_line);

    free(command_line);
    return retval;
}

/*++
Function:
  INIT_FindEXEPath

Abstract:
    Determine the full, canonical path of the current executable by searching
    $PATH.

Parameters:
    LPCSTR exe_name : file to search for

Return:
    pointer to buffer containing the full path. This buffer must be released
    by the caller using free()

Notes :
    this function assumes that "exe_name" is in Unix style (no \)

Notes 2:
    This doesn't handle the case of directories with the desired name
    (and directories are usually executable...)
--*/
static LPWSTR INIT_FindEXEPath(LPCSTR exe_name)
{
    CHAR real_path[MAXPATHLEN];
    LPSTR env_path;
    LPSTR path_ptr;
    LPSTR cur_dir;
    INT exe_name_length;
    BOOL need_slash;
    LPWSTR return_value;
    INT return_size;
    struct stat theStats;

    /* if a path is specified, only search there */
    if(strchr(exe_name, '/'))
    {
        if ( -1 == stat( exe_name, &theStats ) )
        {
            ERROR( "The file does not exist\n" );
            return NULL;
        }

        if ( UTIL_IsExecuteBitsSet( &theStats ) )
        {
            if(!realpath(exe_name, real_path))
            {
                ERROR("realpath() failed!\n");
                return NULL;
            }

            return_size=MultiByteToWideChar(CP_ACP,0,real_path,-1,NULL,0);
            if ( 0 == return_size )
            {
                ASSERT("MultiByteToWideChar failure\n");
                return NULL;
            }

            return_value=malloc(return_size*sizeof(WCHAR));
            if ( NULL == return_value )
            {
                ERROR("Not enough memory to create full path\n");
                return NULL;
            }
            else
            {
                if(!MultiByteToWideChar(CP_ACP, 0, real_path, -1, 
                                        return_value, return_size))
                {
                    ASSERT("MultiByteToWideChar failure\n");
                    free(return_value);
                    return_value = NULL;
                }
                else
                {
                    TRACE("full path to executable is %s\n", real_path);
                }
            }
            return return_value;
        }
    }

    /* no path was specified : search $PATH */

    env_path=MiscGetenv("PATH");
    if(!env_path || *env_path=='\0')
    {
        WARN("$PATH isn't set.\n");
        goto last_resort;
    }

    /* get our own copy of env_path so we can modify it */
    env_path=strdup(env_path);
    if(!env_path)
    {
        ERROR("Not enough memory to copy $PATH!\n");
        return NULL;
    }

    exe_name_length=strlen(exe_name);

    cur_dir=env_path;

    while(cur_dir)
    {
        LPSTR full_path;
        struct stat theStats;

        /* skip all leading ':' */
        while(*cur_dir==':')
        {
            cur_dir++;
        }
        if(*cur_dir=='\0')
        {
            break;
        }

        /* cut string at next ':' */
        path_ptr=strchr(cur_dir, ':');
        if(path_ptr)
        {
            /* check if we need to add a '/' between the path and filename */
            need_slash=(*(path_ptr-1))!='/';

            /* NULL_terminate path element */
            *path_ptr++='\0';
        }
        else
        {
            /* check if we need to add a '/' between the path and filename */
            need_slash=(cur_dir[strlen(cur_dir)-1])!='/';
        }

        TRACE("looking for %s in %s\n", exe_name, cur_dir);

        /* build tentative full file name */
        full_path=malloc(strlen(cur_dir)+exe_name_length+2);
        if(!full_path)
        {
            ERROR("Not enough memory!\n");
            break;
        }
        strcpy(full_path,cur_dir);
        if(need_slash)
        {
            strcat(full_path, "/");
        }

        strcat(full_path, exe_name);

        /* see if file exists AND is executable */
        if ( -1 != stat( full_path, &theStats ) )
        {
            if( UTIL_IsExecuteBitsSet( &theStats ) )
            {
                /* generate canonical path */
                if(!realpath(full_path, real_path))
                {
                    ERROR("realpath() failed!\n");
                    free(full_path);
                    free(env_path);
                    return NULL;
                }
                free(full_path);
    
                return_size = MultiByteToWideChar(CP_ACP,0,real_path,-1,NULL,0);
                if ( 0 == return_size )
                {
                    ASSERT("MultiByteToWideChar failure\n");
                    free(env_path);
                    return NULL;
                }

                return_value = malloc(return_size*sizeof(WCHAR));
                if ( NULL == return_value )
                {
                    ERROR("Not enough memory to create full path\n");
                    free(env_path);
                    return NULL;
                }

                if(!MultiByteToWideChar(CP_ACP, 0, real_path, -1, return_value,
                                    return_size))
                {
                    ASSERT("MultiByteToWideChar failure\n");
                    free(return_value);
                    return_value = NULL;
                }
                else
                {
                    TRACE("found %s in %s; real path is %s\n", exe_name,
                          cur_dir,real_path);
                }
                free(env_path);
                return return_value;
            }
        }
        /* file doesn't exist : keep searching */
        free(full_path);

        /* path_ptr is NULL if there's no ':' after this directory */
        cur_dir=path_ptr;
    }
    free(env_path);
    TRACE("No %s found in $PATH (%s)\n", exe_name, MiscGetenv("PATH"));

last_resort:
    /* last resort : see if the executable is in the current directory. This is
       possible if it comes from a exec*() call. */
    if(0 == stat(exe_name,&theStats))
    {
        if ( UTIL_IsExecuteBitsSet( &theStats ) )
        {
            if(!realpath(exe_name, real_path))
            {
                ERROR("realpath() failed!\n");
                return NULL;
            }

            return_size = MultiByteToWideChar(CP_ACP,0,real_path,-1,NULL,0);
            if (0 == return_size)
            {
                ASSERT("MultiByteToWideChar failure\n");
                return NULL;
            }

            return_value = malloc(return_size*sizeof(WCHAR));
            if (NULL == return_value)
            {
                ERROR("Not enough memory to create full path\n");
                return NULL;
            }
            else
            {
                if(!MultiByteToWideChar(CP_ACP, 0, real_path, -1, 
                                        return_value, return_size))
                {
                    ASSERT("MultiByteToWideChar failure\n");
                    free(return_value);
                    return_value = NULL;
                }
                else
                {
                    TRACE("full path to executable is %s\n", real_path);
                }
            }
            return return_value;
        }
        else
        {
            ERROR("found %s in current directory, but it isn't executable!\n",
                  exe_name);
        }                                                                   
    }
    else
    {
        TRACE("last resort failed : executable %s is not in the current "
              "directory\n",exe_name);
    }
    ERROR("executable %s not found anywhere!\n", exe_name);
    return NULL;
}

#define ROTOR_NAME "rotor"
#define MAX_HASH_RESULT 40
#define ROTOR_USER_CONFIG_DIR "ROTOR_USER_CONFIG_DIR"

/*++
HashPath

Hash a pathname down into a 40-character or shorter string
suitable for use as part of a named-object name or as
part of a pathname.  Note that the destination buffer
must be at least MAX_HASH_RESULT+1 bytes long.

--*/
void HashPath(char *destination, const char *path)
{
    size_t len;

    len = strlen(path);
    if (len < MAX_HASH_RESULT) {
        /* The pathname is short - store it as-is, with
           path separators replaced by another character */
        char *p;
    
        strcpy(destination, path);
        p = strchr(destination, '/');
        while (p) {
            *p = '#';
            p = strchr(p+1, '/');
        }
    } else {
        /* Use a simple hash, and convert the result to Ascii */
        size_t i;
    
        memset(destination, 0, MAX_HASH_RESULT);
        for (i = 0; i<len; ++i) {
            destination[i % MAX_HASH_RESULT] = (destination[i % MAX_HASH_RESULT] + *path + i) % 23;
            path++;
        }
        for (i=0; i<MAX_HASH_RESULT; ++i) {
            destination[i] += 'a';
        }
        destination[i] = '\0';
    }
}

/*++
INIT_InitPalConfigDir

Create the PAL per-machine and per-configuration directories if necessary

returns TRUE on success, FALSE on failure; in case of failure, PAL_Initialize
must fail

--*/
BOOL INIT_InitPalConfigDir(void)
{
    uid_t uid;
    struct stat librotor_stat;
    int length;
    ino_t librotor_inode = -1;
    dev_t librotor_dev = -1;
    struct passwd *passwd;
    char hashpath[sizeof(ROTOR_NAME)+MAX_HASH_RESULT];
    char buffer[MAX_PATH];
    int ret = -1;
    char *rotorUserConfigDir = NULL;

    uid = getuid();

    /* Grab the full pathname to our librotor_pal.so for future storage.
           Then we will try to grab the inode # using stat.
           If there is any error, we will just use an inode of
           -1 and put a WARN on the screen */
    ret = LOADGetLibRotorPalSoFileName((LPSTR) &PALLibRotorPalName);
    if (ret < 0)
    {
        return FALSE;
    }
    if (lstat(PALLibRotorPalName, &librotor_stat) != -1)
    {
        librotor_inode = librotor_stat.st_ino;
        librotor_dev = librotor_stat.st_dev;
    }

    if ( (librotor_inode == -1) || (librotor_dev == -1) )
    {
        WARN("Unable to get librotor_pal.so inode, cannot guarantee unique config dir\n");
    }
    if (snprintf(PALConfigDirName, 
                 sizeof(PALConfigDirName),
                 "/tmp/.rotor-pal-%ld-%ld-%ld", 
                 (long) uid,
                 (long) librotor_dev,
                 (long) librotor_inode) >= sizeof(PALConfigDirName)-1) {
        /* The directory name is too long */
        return FALSE;
    }

    /* Try to create the directory */
    if(-1 == mkdir(PALConfigDirName, 0700) )
    {
        /* If creation failed, see if it's because the directory existed */
        if( EEXIST == errno)
        {
            TRACE("PAL configuration directory %s already exists.\n",
                  PALConfigDirName);

            /* If the directory existed, see if we have full access to it */
            if(-1 == access(PALConfigDirName, R_OK|W_OK|X_OK))
            {
                ASSERT("PAL configuration directory %s has wrong permissions\n",
                      PALConfigDirName);
                return FALSE;
            }
        }
        else
        {
            ASSERT("mkdir() failed! error is %d (%s)\n", errno, strerror(errno));
            return FALSE;
        }
    }
    else
    {
        TRACE("Created PAL configuration directory %s\n", PALConfigDirName);
    }

    passwd = getpwuid(uid);
    if(!passwd)
    {
        ASSERT("getpwuid(%d) returned NULL!\n", uid);
        return FALSE;
    }
    TRACE("Home directory of user %d (%s) is %s\n",
          uid, passwd->pw_name, passwd->pw_dir);

    if ( (rotorUserConfigDir = getenv(ROTOR_USER_CONFIG_DIR)) != NULL)
    {
        if (snprintf(hashpath, sizeof(hashpath), "%s", rotorUserConfigDir) >= sizeof(hashpath)-1)
        {
            return FALSE;
        }
    }
    else
    {
        /* Build the string used to mangle object names, which is unique
           based on the uid and the inode of the PAL, plus the name "rotor" */
        if (snprintf(hashpath, sizeof(hashpath), 
                     "%s-%ld-%ld-%ld", ROTOR_NAME, (long) uid, 
                     (long) librotor_dev,
                     (long) librotor_inode) >= sizeof(hashpath)-1) {
            /* The directory name is too long */
            return FALSE;
        }
    }

    NameManglerLength = MultiByteToWideChar(CP_ACP, 0, 
                                            hashpath, -1, 
                                            NameManglerW, sizeof(NameManglerW)/sizeof(WCHAR))-1;
    TRACE("PAL mangled name is %s, with length %d\n", hashpath, NameManglerLength);

    /* Build the per-user configuration directory name */
    length = snprintf(buffer, sizeof(buffer), "%s/.%s", passwd->pw_dir, hashpath);
    if (length >= sizeof(buffer)) {
        /* the user directory name is too long */
        return FALSE;
    }
    TRACE("User directory name is %s\n", buffer);

    length = MultiByteToWideChar(CP_ACP, 0, 
                                 buffer, -1,
                                 PALUserDirName, 
                                 sizeof(PALUserDirName)/sizeof(WCHAR));
    if (length == 0) {
        /* The user directory name is too long */
        return FALSE;
    }

    if (! CreateDirectoryW(PALUserDirName, NULL))
    {
        /* Ignore only the ERROR_ALREADY_EXISTS error */
        if (GetLastError() != ERROR_ALREADY_EXISTS)
        {
           TRACE("CreateDirectoryW(%s) failed with error code %ld\n", PALUserDirName, GetLastError());
           return FALSE;
        }
    }

    return TRUE;
}

/*++
INIT_RemovePalConfigDir

Delete the PAL configuration directory if it's empty

(no parameters, no return value)

--*/
void INIT_RemovePalConfigDir(void)
{
    CHAR config_dir[MAX_PATH];

    if ( !PALGetPalConfigDir( config_dir, MAX_PATH ) )
    {
        ASSERT( "Unable to determine the PAL config directory.\n" );
    }    
    if(-1 == rmdir(config_dir))
    {
        WARN("rmdir() failed, errno is %d (%s)\n", errno, strerror(errno));
    }
    else
    {
        TRACE("PAL temp directory removed.\n");
    }
}

/*++
MangleObjectNameW

Mangle an object name to be side-by-side with other Rotors

lplpObjectName - [in out] pointer to pointer to object name to mangle
lpMangledObjectName - buffer of MAX_PATH characters to store the mangled
                      object name in

Returns TRUE on success.  On failure, returns FALSE with LastError set.

--*/
BOOL MangleObjectNameW(LPCWSTR *lplpObjectName, LPWSTR lpMangledObjectName)
{
    size_t ObjectNameLength;
    LPCWSTR lpObjectName;
    WCHAR *pSlash;

    lpObjectName = *lplpObjectName;
    if (!lpObjectName) {
        // No object name, so no work to do.
        return TRUE;
    }

    ObjectNameLength = PAL_wcslen(lpObjectName);
    if (ObjectNameLength+NameManglerLength+1 >= MAX_PATH) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }

    *lplpObjectName = lpMangledObjectName;

    pSlash = PAL_wcschr(lpObjectName, L'\\');
    if (pSlash) {
        size_t PathPartLength;

        // The mangling needs to be done after the Global\\ or Local\\ portion
        PathPartLength = pSlash-lpObjectName+1;
        memcpy(lpMangledObjectName, lpObjectName, PathPartLength*sizeof(WCHAR));
        lpObjectName+=PathPartLength;
        lpMangledObjectName+=PathPartLength;
    }
    PAL_wcsncpy(lpMangledObjectName, NameManglerW, NameManglerLength);
    PAL_wcscpy(lpMangledObjectName+NameManglerLength, lpObjectName);

    return TRUE;
}



