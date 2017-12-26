/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    debug.c

Abstract:

    Implementation of Win32 debugging API functions.

Revision History:

--*/

extern "C" {

#undef _LARGEFILE64_SOURCE
#undef _FILE_OFFSET_BITS

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "../thread/process.h"
#include "pal/context.h"
#include "pal/debug.h"
#include "pal/misc.h"

#include <signal.h>
#include <unistd.h>
#if HAVE_PROCFS_CTL
#include <fcntl.h>
#include <unistd.h>
#elif HAVE_TTRACE // HAVE_PROCFS_CTL
#include <sys/ttrace.h>
#else // HAVE_TTRACE
#include <sys/ptrace.h>
#endif  // HAVE_PROCFS_CTL
#if HAVE_VM_READ
#include <mach/mach.h>
#endif  // HAVE_VM_READ
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#if HAVE_PROCFS_H
#include <procfs.h>
#endif // HAVE_PROCFS_H

}

SET_DEFAULT_DEBUG_CHANNEL(DEBUG);

#if HAVE_PROCFS_CTL
#define CTL_ATTACH      "attach"
#define CTL_DETACH      "detach"
#define CTL_WAIT        "wait"
#endif   // HAVE_PROCFS_CTL

/* ------------------- Constant definitions ----------------------------------*/

const BOOL DBG_ATTACH       = TRUE;
const BOOL DBG_DETACH       = FALSE;
static const char PAL_OUTPUTDEBUGSTRING[]    = "PAL_OUTPUTDEBUGSTRING";
static const char PAL_RUN_ON_DEBUG_BREAK[]   = "PAL_RUN_ON_DEBUG_BREAK";

/* ------------------- Static function prototypes ----------------------------*/

#if !HAVE_VM_READ && !HAVE_PROCFS_CTL && !HAVE_TTRACE
static int
DBGWriteProcMem_Int(DWORD processId, int *addr, int data);
static int
DBGWriteProcMem_IntWithMask(DWORD processId, int *addr, int data,
                            unsigned int mask);
#endif  // !HAVE_VM_READ && !HAVE_PROCFS_CTL && !HAVE_TTRACE
static int
DBGSetProcessAttached(DWORD attachedProcId, DWORD attacherProcId, BOOL bAttach);

/*++
Function:
  FlushInstructionCache

The FlushInstructionCache function flushes the instruction cache for
the specified process.

Remarks

This is a no-op for x86 architectures where the instruction and data
caches are coherent in hardware. For non-X86 architectures, this call
usually maps to a kernel API to flush the D-caches on all processors.

--*/
BOOL
PALAPI
FlushInstructionCache(
        IN HANDLE hProcess,
        IN LPCVOID lpBaseAddress,
        IN SIZE_T dwSize)
{
    BOOL Ret;

    PERF_ENTRY(FlushInstructionCache);
    ENTRY("FlushInstructionCache (hProcess=%p, lpBaseAddress=%p dwSize=%d)\
          \n", hProcess, lpBaseAddress, dwSize);

    Ret = DBG_FlushInstructionCache(lpBaseAddress, dwSize);

    LOGEXIT("FlushInstructionCache returns BOOL %d\n", Ret);
    PERF_EXIT(FlushInstructionCache);
    return Ret;
}


/*++
Function:
  OutputDebugStringA

See MSDN doc.
--*/
VOID
PALAPI
OutputDebugStringA(
        IN LPCSTR lpOutputString)
{
    char *env_string;

    PERF_ENTRY(OutputDebugStringA);
    ENTRY("OutputDebugStringA (lpOutputString=%p (%s))\n",
          lpOutputString?lpOutputString:"NULL",
          lpOutputString?lpOutputString:"NULL");
    
    /* as we don't support debug events, we are going to output the debug string
      to stderr instead of generating OUT_DEBUG_STRING_EVENT */
    if ( (lpOutputString != NULL) && 
         (NULL != (env_string = MiscGetenv(PAL_OUTPUTDEBUGSTRING))))
    {
        fprintf(stderr, "%s", lpOutputString);
    }
        
    LOGEXIT("OutputDebugStringA returns\n");
    PERF_EXIT(OutputDebugStringA);
}

/*++
Function:
  OutputDebugStringW

See MSDN doc.
--*/
VOID
PALAPI
OutputDebugStringW(
        IN LPCWSTR lpOutputString)
{
    CHAR *lpOutputStringA;
    int strLen;

    PERF_ENTRY(OutputDebugStringW);
    ENTRY("OutputDebugStringW (lpOutputString=%p (%S))\n",
          lpOutputString ? lpOutputString: W16_NULLSTRING,
          lpOutputString ? lpOutputString: W16_NULLSTRING);
    
    if (lpOutputString == NULL) 
    {
        OutputDebugStringA("");
        goto EXIT;
    }

    if ((strLen = WideCharToMultiByte(CP_ACP, 0, lpOutputString, -1, NULL, 0, 
                                      NULL, NULL)) 
        == 0)
    {
        ASSERT("failed to get wide chars length\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        goto EXIT;
    }
    
    /* strLen includes the null terminator */
    if ((lpOutputStringA = (LPSTR) malloc(strLen * sizeof(CHAR))) == NULL)
    {
        ERROR("Insufficient memory available !\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto EXIT;
    }
    
    if(! WideCharToMultiByte(CP_ACP, 0, lpOutputString, -1, 
                             lpOutputStringA, strLen, NULL, NULL)) 
    {
        ASSERT("failed to convert wide chars to multibytes\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        free(lpOutputStringA);
        goto EXIT;
    }
    
    OutputDebugStringA(lpOutputStringA);
    free(lpOutputStringA);

EXIT:    
    LOGEXIT("OutputDebugStringW returns\n");
    PERF_EXIT(OutputDebugStringW);
}

/*
   When DebugBreak() is called, if PAL_RUN_ON_DEBUG_BREAK is set,
   DebugBreak() will execute whatever command is in there.

   PAL_RUN_ON_DEBUG_BREAK must be no longer than 255 characters.

   This command string inherits the current process's environment,
   with two additions:
      PAL_EXE_PID  - the process ID of the current process
      PAL_EXE_NAME - the name of the executable of the current process

   When DebugBreak() runs this string, it periodically polls the child process
   and blocks until it finishes. If you use this mechanism to start a
   debugger, you can break this poll loop by setting the "spin" variable in
   run_debug_command()'s frame to 0, and then the parent process can
   continue.

   suggested values for PAL_RUN_ON_DEBUG_BREAK:
     to halt the process for later inspection:
       'echo stopping $PAL_EXE_PID; kill -STOP $PAL_EXE_PID; sleep 10'

     to print out the stack trace:
       'pstack $PAL_EXE_PID'

     to invoke the gdb debugger on the process:
       'set -x; gdb $PAL_EXE_NAME $PAL_EXE_PID'

     to invoke the ddd debugger on the process (requires X11):
       'set -x; ddd $PAL_EXE_NAME $PAL_EXE_PID'
*/

static
int
run_debug_command (const char *command)
{
    int pid;
    volatile int spin = 1;

    if (!command) {
        return 1;
    }

    printf ("Spawning command: %s\n", command);
    
    pid = fork();
    if (pid == -1) {
        return -1;
    }
    if (pid == 0) {
        const char *argv[4] = { "sh", "-c", command, 0 };
        execv("/bin/sh", (char **)argv);
        exit(127);
    }

    /* We continue either when the spawned process has stopped, or when
       an attached debugger sets spin to 0 */
    while (spin) {
        int status = 0;
        int ret = waitpid(pid, &status, WNOHANG);
        if (ret == 0) {
            int i;
            /* I tried to use sleep for this, and that works everywhere except
               FreeBSD. The problem on FreeBSD is that if the process gets a
               signal while blocked in sleep(), gdb is confused by the stack */
            for (i = 0; i < 1000000; i++)
                ;
        }
        else if (ret == -1) {
            if (errno != EINTR) {
                return -1;
            }
        }
        else if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        else {
            fprintf (stderr, "unexpected return from waitpid\n");
            return -1;
        }
    };
    return 0;
}

static
int
DebugBreakCommand()
{
    const char *command_string = getenv (PAL_RUN_ON_DEBUG_BREAK);
    if (command_string) {
        char pid_buf[sizeof ("PAL_EXE_PID=") + 32];
        char exe_buf[sizeof ("PAL_EXE_NAME=") + 256];
        extern char g_ExePath[MAX_PATH];
        if (snprintf (pid_buf, sizeof (pid_buf),
                      "PAL_EXE_PID=%d", getpid()) <= 0) {
            goto FAILED;
        }
        if (snprintf (exe_buf, sizeof (exe_buf),
                      "PAL_EXE_NAME=%s", g_ExePath) <= 0) {
            goto FAILED;
        }

        /* strictly speaking, we might want to only set these environment
           variables in the child process, but if we do that we can't check
           for errors. putenv/setenv can fail when out of memory */

        /* ROTORTODO:

           Not all platforms have setenv, and the PAL doens't provide
           MiscSetenv, only MiscPutenv. MiscPutenv has the problem of doing
           more mallocs than we need here, and I'd like to minimize mallocs
           during an assert. See bug 245190.

           Furthermore, MiscPutenv doesn't seem to work for us on FreeBSD.
           See bug 245187. So, calling setenv directly.

           When those bugs are fixed, use MiscSetenv.

           Also, when using MiscSetenv, we can get rid of the length limit on
           PAL_RUN_ON_DEBUG_BREAK
         */

           
        if (putenv (pid_buf) || putenv (exe_buf)) {
            goto FAILED;
        }
        if (run_debug_command (command_string)) {
            goto FAILED;
        }
        return 1;
    }
    return 0;
FAILED:
    fprintf (stderr, "Failed to execute command: '%s'\n", command_string);
    return -1;
}

/*++
Function:
  DebugBreak

See MSDN doc.
--*/
VOID
PALAPI
DebugBreak(
       VOID)
{
    PERF_ENTRY(DebugBreak);
    ENTRY("DebugBreak()\n");

    if (DebugBreakCommand() <= 0) {
        // either didn't do anything, or failed
        ERROR("Calling DBG_DebugBreak\n");
        DBG_DebugBreak();
    }
    
    LOGEXIT("DebugBreak returns\n");
    PERF_EXIT(DebugBreak);
}

/*++
Function:
  GetThreadContext

See MSDN doc.
--*/
BOOL
PALAPI
GetThreadContext(
           IN HANDLE hThread,
           IN OUT LPCONTEXT lpContext)
{    
    BOOL ret;
    PERF_ENTRY(GetThreadContext);
    ENTRY("GetThreadContext (hThread=%p, lpContext=%p)\n",hThread,lpContext);

    if(hThread == hPseudoCurrentThread)
    {
        hThread = PROCGetRealCurrentThread();
    } 
    
    ret = CONTEXT_GetThreadContext(hThread, lpContext);
    
    LOGEXIT("GetThreadContext returns ret:%d\n", ret);
    PERF_EXIT(GetThreadContext);
    return ret;
}

/*++
Function:
  SetThreadContext

See MSDN doc.
--*/
BOOL
PALAPI
SetThreadContext(
           IN HANDLE hThread,
           IN CONST CONTEXT *lpContext)
{
    BOOL ret;
    PERF_ENTRY(SetThreadContext);
    ENTRY("SetThreadContext (hThread=%p, lpContext=%p)\n",hThread,lpContext);

    if(hThread == hPseudoCurrentThread)
    {
        hThread = PROCGetRealCurrentThread();
    } 
    
    ret = CONTEXT_SetThreadContext(hThread, lpContext);
    
    LOGEXIT("SetThreadContext returns ret:%d\n", ret);
    PERF_EXIT(SetThreadContext);
    return ret;
}

/*++
Function:
  ReadProcessMemory

See MSDN doc.
--*/
BOOL
PALAPI
ReadProcessMemory(
           IN HANDLE hProcess,
           IN LPCVOID lpBaseAddress,
           IN LPVOID lpBuffer,
           IN SIZE_T nSize,
           OUT SIZE_T * lpNumberOfBytesRead
           )
{
    DWORD processId;
    volatile BOOL ret = FALSE;
    volatile SIZE_T numberOfBytesRead = 0;
#if HAVE_VM_READ
    kern_return_t result;
    vm_map_t task;
    LONG_PTR bytesToRead;
#elif HAVE_PROCFS_CTL
    int fd;
    char memPath[64];
#elif !HAVE_TTRACE
    SIZE_T nbInts;
    int* ptrInt;
    int* lpTmpBuffer;
#endif
#if !HAVE_PROCFS_CTL && !HAVE_TTRACE
    LPVOID lpBaseAddressAligned;
    SIZE_T offset;
#endif  // !HAVE_PROCFS_CTL && !HAVE_TTRACE

    PERF_ENTRY(ReadProcessMemory);
    ENTRY("ReadProcessMemory (hProcess=%p,lpBaseAddress=%p, lpBuffer=%p, "
          "nSize=%u, lpNumberOfBytesRead=%p)\n",hProcess,lpBaseAddress,
          lpBuffer, (unsigned int)nSize, lpNumberOfBytesRead);
    
    if (!(processId = PROCGetProcessIDFromHandle(hProcess)))
    {
        ERROR("Invalid process handler hProcess:%p.",hProcess);
        SetLastError(ERROR_INVALID_HANDLE);        
        goto EXIT;
    }
    
    // Check if the read request is for the current process. 
    // We don't need ptrace in that case.
    if (GetCurrentProcessId() == processId) 
    {
        TRACE("We are in the same process, so ptrace is not needed\n");
        
	PAL_TRY {
	    // Seg fault in memcpy can't be caught
	    // so we simulate the memcpy here

            SIZE_T i;

	    for (i = 0; i<nSize; i++)
		*((char*)(lpBuffer)+i) = *((char*)(lpBaseAddress)+i);

            numberOfBytesRead = nSize;
            ret = TRUE;
	} PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
            SetLastError(ERROR_ACCESS_DENIED);
	} PAL_ENDTRY    
        goto EXIT;        
    }

#if HAVE_VM_READ
    result = task_for_pid(mach_task_self(), processId, &task);
    if (result != KERN_SUCCESS)
    {
        ERROR("No Mach task for pid %d: %d\n", processId, ret);
        SetLastError(ERROR_INVALID_HANDLE);
        goto EXIT;
    }
    // vm_read_overwrite usually requires that the address be page-aligned
    // and the size be a multiple of the page size.  We can't differentiate
    // between the cases in which that's required and those in which it
    // isn't, so we do it all the time.
    lpBaseAddressAligned = (LPVOID) ((SIZE_T) lpBaseAddress & ~VIRTUAL_PAGE_MASK);
    offset = ((SIZE_T) lpBaseAddress & VIRTUAL_PAGE_MASK);
    while (nSize > 0)
    {
        vm_size_t bytesRead;
        char data[VIRTUAL_PAGE_SIZE];
        
        bytesToRead = VIRTUAL_PAGE_SIZE - offset;
        if (bytesToRead > (LONG_PTR)nSize)
        {
            bytesToRead = nSize;
        }
        bytesRead = VIRTUAL_PAGE_SIZE;
        result = vm_read_overwrite(task, (vm_address_t) lpBaseAddressAligned,
                                   VIRTUAL_PAGE_SIZE, (vm_address_t) data, &bytesRead);
        if (result != KERN_SUCCESS || bytesRead != VIRTUAL_PAGE_SIZE)
        {
            ERROR("vm_read_overwrite failed for %d bytes from %p in %d: %d\n",
                  VIRTUAL_PAGE_SIZE, (char *) lpBaseAddressAligned, task, result);
            if (result <= KERN_RETURN_MAX)
            {
                SetLastError(ERROR_INVALID_ACCESS);
            }
            else
            {
                SetLastError(ERROR_INTERNAL_ERROR);
            }
            goto EXIT;
        }
        memcpy((LPSTR)lpBuffer + numberOfBytesRead, data + offset, bytesToRead);
        numberOfBytesRead += bytesToRead;
        lpBaseAddressAligned = (char *) lpBaseAddressAligned + VIRTUAL_PAGE_SIZE;
        nSize -= bytesToRead;
        offset = 0;
    }
    ret = TRUE;
#else   // HAVE_VM_READ
#if HAVE_PROCFS_CTL
    snprintf(memPath, sizeof(memPath), "/proc/%u/%s", processId, PROCFS_MEM_NAME);
    fd = open(memPath, O_RDONLY);
    if (fd == -1)
    {
        ERROR("Failed to open %s\n", memPath);
        SetLastError(ERROR_INVALID_ACCESS);
        goto PROCFSCLEANUP;
    }

    if (lseek(fd, (off_t) lpBaseAddress, SEEK_SET) == -1)
    {
        ERROR("Failed to seek to base address\n");
        SetLastError(ERROR_INVALID_ACCESS);
        goto PROCFSCLEANUP;
    }
    
    numberOfBytesRead = read(fd, lpBuffer, nSize);
    ret = TRUE;

#else   // HAVE_PROCFS_CTL
    // Attach the process before calling ttrace/ptrace otherwise it fails.
    if (DBGAttachProcess(processId))
    {
#if HAVE_TTRACE
        if (ttrace(TT_PROC_RDDATA, processId, 0, (uint64_t)lpBaseAddress, (uint64_t)nSize, (uint64_t)lpBuffer) == -1)
        {
            if (errno == EFAULT) 
            {
                ERROR("ttrace(TT_PROC_RDDATA, pid:%d, 0, addr:%p, data:%d, addr2:%d) failed"
                      " errno=%d (%s)\n", processId, lpBaseAddress, (int)nSize, lpBuffer,
                      errno, strerror(errno));
                
                SetLastError(ERROR_ACCESS_DENIED);
            }
            else
            {
                ASSERT("ttrace(TT_PROC_RDDATA, pid:%d, 0, addr:%p, data:%d, addr2:%d) failed"
                      " errno=%d (%s)\n", processId, lpBaseAddress, (int)nSize, lpBuffer,
                      errno, strerror(errno));
                SetLastError(ERROR_INTERNAL_ERROR);
            }

            goto CLEANUP1;
        }

        numberOfBytesRead = nSize;
        ret = TRUE;
        
#else   // HAVE_TTRACE

        offset = (SIZE_T)lpBaseAddress % sizeof(int);
        lpBaseAddressAligned =  (LPVOID) ((char*)lpBaseAddress - offset);    
        nbInts = (nSize + offset)/sizeof(int) + 
                 ((nSize + offset)%sizeof(int) ? 1:0);
        
        /* before transferring any data to lpBuffer we should make sure that all 
           data is accessible for read. so we need to use a temp buffer for that.*/
        if (!(lpTmpBuffer = (int*)malloc(nbInts * sizeof(int))))
        {

#if MALLOC_ZERO_RETURNS_NULL
            /* Some implementations return NULL for malloc(0), check for that */
            if (0 == nbInts)
            {
                numberOfBytesRead = 0;
                ret = TRUE;
                goto CLEANUP1;
            }
#endif  // MALLOC_ZERO_RETURNS_NULL

            ERROR("Insufficient memory available !\n");
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto CLEANUP1;
        }
        
        for (ptrInt = lpTmpBuffer; nbInts; ptrInt++,
             ((int*)lpBaseAddressAligned)++, nbInts--)
        {
            errno = 0;
            *ptrInt =
                PAL_PTRACE(PAL_PT_READ_D, processId, lpBaseAddressAligned, 0);
            if (*ptrInt == -1 && errno) 
            {
                if (errno == EFAULT) 
                {
                    ERROR("ptrace(PT_READ_D, pid:%d, addr:%p, data:0) failed"
                          " errno=%d (%s)\n", processId, lpBaseAddressAligned,
                          errno, strerror(errno));
                    
                    SetLastError(ptrInt == lpTmpBuffer ? ERROR_ACCESS_DENIED : 
                                                         ERROR_PARTIAL_COPY);
                }
                else
                {
                    ASSERT("ptrace(PT_READ_D, pid:%d, addr:%p, data:0) failed"
                          " errno=%d (%s)\n", processId, lpBaseAddressAligned,
                          errno, strerror(errno));
                    SetLastError(ERROR_INTERNAL_ERROR);
                }
                
                goto CLEANUP2;
            }
        }
        
        /* transfer data from temp buffer to lpBuffer */
        memcpy( (char *)lpBuffer, ((char*)lpTmpBuffer) + offset, nSize);
        numberOfBytesRead = nSize;
        ret = TRUE;
#endif // HAVE_TTRACE        
    }
    else
    {
        /* Failed to attach processId */
        goto EXIT;    
    }
#endif  // HAVE_PROCFS_CTL

#if HAVE_PROCFS_CTL
PROCFSCLEANUP:
    if (fd != -1)
    {
        close(fd);
    }    
#elif !HAVE_TTRACE
CLEANUP2:
    if (lpTmpBuffer) 
    {
        free(lpTmpBuffer);
    }
#endif  // !HAVE_TTRACE

#if !HAVE_PROCFS_CTL
CLEANUP1:
    if (!DBGDetachProcess(processId))
    {
        /* Failed to detach processId */
        ret = FALSE;
    }
#endif  // HAVE_PROCFS_CTL
#endif  // HAVE_VM_READ

EXIT:
    if (lpNumberOfBytesRead)
    {
        *lpNumberOfBytesRead = numberOfBytesRead;
    }
    LOGEXIT("ReadProcessMemory returns BOOL %d\n", ret);
    PERF_EXIT(ReadProcessMemory);
    return ret;
}

/*++
Function:
  WriteProcessMemory

See MSDN doc.
--*/
BOOL
PALAPI
WriteProcessMemory(
           IN HANDLE hProcess,
           IN LPVOID lpBaseAddress,
           IN LPVOID lpBuffer,
           IN SIZE_T nSize,
           OUT SIZE_T * lpNumberOfBytesWritten
           )

{
    DWORD processId;
    volatile BOOL ret = FALSE;
    volatile SIZE_T numberOfBytesWritten = 0;
#if HAVE_VM_READ
    kern_return_t result;
    vm_map_t task;
#elif HAVE_PROCFS_CTL
    int fd;
    char memPath[64];
    LONG_PTR bytesWritten;
#elif !HAVE_TTRACE
    SIZE_T FirstIntOffset;
    SIZE_T LastIntOffset;
    unsigned int FirstIntMask;
    unsigned int LastIntMask;
    SIZE_T nbInts;
    int *lpTmpBuffer = 0, *lpInt;
    LPVOID lpBaseAddressAligned;
#endif

    PERF_ENTRY(WriteProcessMemory);
    ENTRY("WriteProcessMemory (hProcess=%p,lpBaseAddress=%p, lpBuffer=%p, "
           "nSize=%u, lpNumberOfBytesWritten=%p)\n",
           hProcess,lpBaseAddress, lpBuffer, (unsigned int)nSize, lpNumberOfBytesWritten);    
    
    if (!(nSize && (processId = PROCGetProcessIDFromHandle(hProcess))))
    {
        ERROR("Invalid nSize:%u number or invalid process handler "
              "hProcess:%p\n", (unsigned int)nSize, hProcess);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto EXIT;
    }
    
    // Check if the write request is for the current process.
    // In that case we don't need ptrace.
    if (GetCurrentProcessId() == processId) 
    {
        TRACE("We are in the same process so we don't need ptrace\n");
        
	PAL_TRY {
	    // Seg fault in memcpy can't be caught
	    // so we simulate the memcpy here

            SIZE_T i;

	    for (i = 0; i<nSize; i++)
            {
		*((char*)(lpBaseAddress)+i) = *((char*)(lpBuffer)+i);
            }

            numberOfBytesWritten = nSize;
            ret = TRUE;
	} PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
            SetLastError(ERROR_ACCESS_DENIED);
	} PAL_ENDTRY
        goto EXIT;        
    }

#if HAVE_VM_READ
    result = task_for_pid(mach_task_self(), processId, &task);
    if (result != KERN_SUCCESS)
    {
        ERROR("No Mach task for pid %d: %d\n", processId, ret);
        SetLastError(ERROR_INVALID_HANDLE);
        goto EXIT;
    }
    result = vm_write(task, (vm_address_t) lpBaseAddress, 
                      (vm_address_t) lpBuffer, nSize);
    if (result != KERN_SUCCESS)
    {
        ERROR("vm_write failed for %d bytes from %p in %d: %d\n",
              (int)nSize, lpBaseAddress, task, result);
        if (result <= KERN_RETURN_MAX)
        {
            SetLastError(ERROR_ACCESS_DENIED);
        }
        else
        {
            SetLastError(ERROR_INTERNAL_ERROR);
        }
        goto EXIT;
    }
    numberOfBytesWritten = nSize;
    ret = TRUE;
#else   // HAVE_VM_READ
#if HAVE_PROCFS_CTL
    snprintf(memPath, sizeof(memPath), "/proc/%u/%s", processId, PROCFS_MEM_NAME);
    fd = open(memPath, O_WRONLY);
    if (fd == -1)
    {
        ERROR("Failed to open %s\n", memPath);
        SetLastError(ERROR_INVALID_ACCESS);
        goto PROCFSCLEANUP;
    }

    if (lseek(fd, (off_t) lpBaseAddress, SEEK_SET) == -1)
    {
        ERROR("Failed to seek to base address\n");
        SetLastError(ERROR_INVALID_ACCESS);
        goto PROCFSCLEANUP;
    }
    
    bytesWritten = write(fd, lpBuffer, nSize);
    if (bytesWritten < 0)
    {
        ERROR("Failed to write to %s\n", memPath);
        SetLastError(ERROR_INVALID_ACCESS);
        goto PROCFSCLEANUP;
    }

    numberOfBytesWritten = bytesWritten;
    ret = TRUE;

#else   // HAVE_PROCFS_CTL
    /* Attach the process before calling ptrace otherwise it fails */
    if (DBGAttachProcess(processId))
    {
#if HAVE_TTRACE
        if (ttrace(TT_PROC_WRDATA, processId, 0, (uint64_t)lpBaseAddress, (uint64_t)nSize, (uint64_t)lpBuffer) == -1)
        {
            if (errno == EFAULT) 
            {
                ERROR("ttrace(TT_PROC_WRDATA, pid:%d, addr:%p, data:%d, addr2:%d) failed"
                      " errno=%d (%s)\n", processId, lpBaseAddress, nSize, lpBuffer,
                      errno, strerror(errno));
                
                SetLastError(ERROR_ACCESS_DENIED);
            }
            else
            {
                ASSERT("ttrace(TT_PROC_WRDATA, pid:%d, addr:%p, data:%d, addr2:%d) failed"
                      " errno=%d (%s)\n", processId, lpBaseAddress, nSize, lpBuffer,
                      errno, strerror(errno));
                SetLastError(ERROR_INTERNAL_ERROR);
            }

            goto CLEANUP1;
        }

        numberOfBytesWritten = nSize;
        ret = TRUE;
        
#else   // HAVE_TTRACE

        FirstIntOffset = (SIZE_T)lpBaseAddress % sizeof(int);    
        FirstIntMask = -1;
        FirstIntMask <<= (FirstIntOffset * 8);
        
        nbInts = (nSize + FirstIntOffset) / sizeof(int) + 
                 (((nSize + FirstIntOffset)%sizeof(int)) ? 1:0);
        lpBaseAddressAligned = (LPVOID)((char*)lpBaseAddress - FirstIntOffset);
        
        if ((lpTmpBuffer = (int*)malloc(nbInts * sizeof(int))) == NULL)
        {
            ERROR("Insufficient memory available !\n");
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto CLEANUP1;
        }
        
        memcpy( (char *)lpTmpBuffer + FirstIntOffset, (char *)lpBuffer, nSize);    
        lpInt = lpTmpBuffer;

        LastIntOffset = (nSize + FirstIntOffset) % sizeof(int);
        LastIntMask = -1;
        LastIntMask >>= ((sizeof(int) - LastIntOffset) * 8);
        
        if (nbInts == 1)
        {
            if (DBGWriteProcMem_IntWithMask(processId, (int*)lpBaseAddressAligned, 
                                            *lpInt,
                                            LastIntMask & FirstIntMask)
                  == 0)
            {
                goto CLEANUP2;
            }
            numberOfBytesWritten = nSize;
            ret = TRUE;
            goto CLEANUP2;
        }
        
        if (DBGWriteProcMem_IntWithMask(processId,
                                        ((int*)lpBaseAddressAligned)++,
                                        *lpInt++, FirstIntMask) 
            == 0)
        {
            goto CLEANUP2;
        }

        while (--nbInts > 1)
        {      
          if (DBGWriteProcMem_Int(processId, ((int*)lpBaseAddressAligned)++,
                                  *lpInt++) == 0)
          {
              goto CLEANUP2;
          }
        }
        
        if (DBGWriteProcMem_IntWithMask(processId, (int*)lpBaseAddressAligned,
                                        *lpInt, LastIntMask ) == 0)
        {
            goto CLEANUP2;
        }

        numberOfBytesWritten = nSize;
        ret = TRUE;
#endif  // HAVE_TTRACE         
    }     
    else
    {
        /* Failed to attach processId */
        goto EXIT;    
    }  
#endif  // HAVE_PROCFS_CTL

#if HAVE_PROCFS_CTL
PROCFSCLEANUP:
    if (fd != -1)
    {
        close(fd);
    }
#elif !HAVE_TTRACE
CLEANUP2:
    if (lpTmpBuffer) 
    {
        free(lpTmpBuffer);
    }
#endif  // !HAVE_TTRACE

#if !HAVE_PROCFS_CTL
CLEANUP1:
    if (!DBGDetachProcess(processId))
    {
        /* Failed to detach processId */
        ret = FALSE;
    }
#endif  // !HAVE_PROCFS_CTL
#endif  // HAVE_VM_READ

EXIT:
    if (lpNumberOfBytesWritten)
    {
        *lpNumberOfBytesWritten = numberOfBytesWritten;
    }

    LOGEXIT("WriteProcessMemory returns BOOL %d\n", ret);
    PERF_EXIT(WriteProcessMemory);
    return ret;
}

#if !HAVE_VM_READ && !HAVE_PROCFS_CTL && !HAVE_TTRACE
/*++
Function:
  DBGWriteProcMem_Int

Abstract
  write one int to a process memory address

Parameter
  processId : process handle
  addr : memory address where the int should be written
  data : int to be written in addr

Return
  Return 1 if it succeeds, or 0 if it's fails
--*/
static
int
DBGWriteProcMem_Int(IN DWORD processId, 
                    IN int *addr,
                    IN int data)
{
    if (PAL_PTRACE( PAL_PT_WRITE_D, processId, addr, data ) == -1)
    {
        if (errno == EFAULT) 
        {
            ERROR("ptrace(PT_WRITE_D, pid:%d caddr_t:%p data:%x) failed "
                  "errno:%d (%s)\n", processId, addr, data, errno, strerror(errno));
            SetLastError(ERROR_INVALID_ADDRESS);
        }
        else
        {
            ASSERT("ptrace(PT_WRITE_D, pid:%d caddr_t:%p data:%x) failed "
                  "errno:%d (%s)\n", processId, addr, data, errno, strerror(errno));
            SetLastError(ERROR_INTERNAL_ERROR);
        }
        return 0;
    }

    return 1;
}

/*++
Function:
  DBGWriteProcMem_IntWithMask

Abstract
  write one int to a process memory address space using mask

Parameter
  processId : process ID
  addr : memory address where the int should be written
  data : int to be written in addr
  mask : the mask used to write only a parts of data

Return
  Return 1 if it succeeds, or 0 if it's fails
--*/
static
int
DBGWriteProcMem_IntWithMask(IN DWORD processId,
                            IN int *addr,
                            IN int data,
                            IN unsigned int mask )
{
    int readInt;

    if (mask != ~0)
    {
        errno = 0;
        if (((readInt = PAL_PTRACE( PAL_PT_READ_D, processId, addr, 0 )) == -1)
             && errno)
        {
            if (errno == EFAULT) 
            {
                ERROR("ptrace(PT_READ_D, pid:%d, caddr_t:%p, 0) failed "
                      "errno:%d (%s)\n", processId, addr, errno, strerror(errno));
                SetLastError(ERROR_INVALID_ADDRESS);
            }
            else
            {
                ASSERT("ptrace(PT_READ_D, pid:%d, caddr_t:%p, 0) failed "
                      "errno:%d (%s)\n", processId, addr, errno, strerror(errno));
                SetLastError(ERROR_INTERNAL_ERROR);
            }

            return 0;
        }
        data = (data & mask) | (readInt & ~mask);
    }    
    return DBGWriteProcMem_Int(processId, addr, data);
}
#endif  // !HAVE_VM_READ && !HAVE_PROCFS_CTL && !HAVE_TTRACE

/*++
Function:
  DBGAttachProcess

Abstract  
  
  Attach the indicated process to the current process. 
  
  if the indicated process is already attached by the current process, then 
  increment the number of attachment pending. if ot, attach it to the current 
  process (with PT_ATTACH).

Parameter
  processId : process ID to attach
Return
  Return true if it succeeds, or false if it's fails
--*/
extern "C"
BOOL 
DBGAttachProcess(DWORD processId)
{
    int attchmentCount;
    int savedErrno;
#if HAVE_PROCFS_CTL
    int fd;
    char ctlPath[1024];
#endif  // HAVE_PROCFS_CTL

    attchmentCount = 
        DBGSetProcessAttached(processId, GetCurrentProcessId(), DBG_ATTACH);

    if (attchmentCount == -1)
    {
        /* Failed to set the process as attached */
        goto EXIT;
    }
    
    if (attchmentCount == 1)
    {
#if HAVE_PROCFS_CTL
        struct timespec waitTime;

        // FreeBSD has some trouble when a series of attach/detach sequences
        // occurs too close together.  When this happens, we'll be able to
        // attach to the process, but waiting for the process to stop
        // (either via writing "wait" to /proc/<pid>/ctl or via waitpid)
        // will hang.  If we pause for a very short amount of time before
        // trying to attach, we don't run into the bug.
        // PR 35175 in the FreeBSD bug database is likely related to this
        // problem, which occurs regardless of whether we use ptrace
        // or procfs to communicate with the other process.
        waitTime.tv_sec = 0;
        waitTime.tv_nsec = 50000000;
        nanosleep(&waitTime, NULL);
        
        sprintf(ctlPath, "/proc/%d/ctl", processId);
        fd = open(ctlPath, O_WRONLY);
        if (fd == -1)
        {
            ERROR("Failed to open %s: errno is %d (%s)\n", ctlPath,
                  errno, strerror(errno));
            goto DETACH1;
        }
        
        if (write(fd, CTL_ATTACH, sizeof(CTL_ATTACH)) < (int)sizeof(CTL_ATTACH))
        {
            ERROR("Failed to attach to %s: errno is %d (%s)\n", ctlPath,
                  errno, strerror(errno));
            close(fd);
            goto DETACH1;
        }
        
        if (write(fd, CTL_WAIT, sizeof(CTL_WAIT)) < (int)sizeof(CTL_WAIT))
        {
            ERROR("Failed to wait for %s: errno is %d (%s)\n", ctlPath,
                  errno, strerror(errno));
            goto DETACH2;
        }
        
        close(fd);
#elif HAVE_TTRACE
        if (ttrace(TT_PROC_ATTACH, processId, 0, TT_DETACH_ON_EXIT, TT_VERSION, 0) == -1)
        {
            if (errno != ESRCH)
            {                
                ASSERT("ttrace(TT_PROC_ATTACH, pid:%d) failed errno:%d (%s)\n",
                     processId, errno, strerror(errno));
            }
            goto DETACH1;
        }
#else   // HAVE_TTRACE
        if (PAL_PTRACE( PAL_PT_ATTACH, processId, 0, 0 ) == -1)
        {
            if (errno != ESRCH)
            {                
                ASSERT("ptrace(PT_ATTACH, pid:%d) failed errno:%d (%s)\n",
                     processId, errno, strerror(errno));
            }
            goto DETACH1;
        }
                    
        if (waitpid(processId, NULL, WUNTRACED) == -1)
        {
            if (errno != ESRCH)
            {
                ASSERT("waitpid(pid:%d, NULL, WUNTRACED) failed.errno:%d"
                       " (%s)\n", processId, errno, strerror(errno));
            }
            goto DETACH2;
        }
#endif  // HAVE_PROCFS_CTL
    }
    
    return TRUE;

#if HAVE_PROCFS_CTL
DETACH2:
    if (write(fd, CTL_DETACH, sizeof(CTL_DETACH)) < (int)sizeof(CTL_DETACH))
    {
        ASSERT("Failed to detach from %s: errno is %d (%s)\n", ctlPath,
               errno, strerror(errno));
    }
    close(fd);
#elif !HAVE_TTRACE
DETACH2:
    if (PAL_PTRACE(PAL_PT_DETACH, processId, 0, 0) == -1)
    {
        ASSERT("ptrace(PT_DETACH, pid:%d) failed. errno:%d (%s)\n", processId, 
              errno, strerror(errno));
    }
#endif  // HAVE_PROCFS_CTL

DETACH1:
    savedErrno = errno;
    DBGSetProcessAttached(processId, GetCurrentProcessId(), DBG_DETACH);
    errno = savedErrno;
EXIT:
    if (errno == ESRCH || errno == ENOENT || errno == EBADF)
    {
        ERROR("Invalid process ID:%d\n", processId);
        SetLastError(ERROR_INVALID_PARAMETER);
    }
    else
    {
        SetLastError(ERROR_INTERNAL_ERROR);
    }
    return FALSE;
}

/*++
Function:
  DBGDetachProcess

Abstract
  Detach the indicated process from the current process.
  
  if the indicated process is already attached by the current process, then 
  decrement the number of attachment pending and detach it from the current 
  process (with PT_DETACH) if there's no more attachment left. 
  
Parameter
  processId : process handle

Return
  Return true if it succeeds, or true if it's fails
--*/
extern "C"
BOOL
DBGDetachProcess(DWORD processId)
{     
    int nbAttachLeft;
#if HAVE_PROCFS_CTL
    int fd;
    char ctlPath[1024];
#endif  // HAVE_PROCFS_CTL

    nbAttachLeft = DBGSetProcessAttached(processId, GetCurrentProcessId(), 
                                         DBG_DETACH);    
    if (nbAttachLeft == -1)
    {
        /* Failed to set the process as detached */
        return FALSE;
    }
    
    /* check if there's no more attachment left on processId */
    if (nbAttachLeft == 0)
    {
#if HAVE_PROCFS_CTL
        sprintf(ctlPath, "/proc/%d/ctl", processId);
        fd = open(ctlPath, O_WRONLY);
        if (fd == -1)
        {
            if (errno == ENOENT)
            {
                ERROR("Invalid process ID: %d\n", processId);
                SetLastError(ERROR_INVALID_PARAMETER);
            }
            else
            {
                ERROR("Failed to open %s: errno is %d (%s)\n", ctlPath,
                      errno, strerror(errno));
                SetLastError(ERROR_INTERNAL_ERROR);
            }
            return FALSE;
        }
        
        if (write(fd, CTL_DETACH, sizeof(CTL_DETACH)) < (int)sizeof(CTL_DETACH))
        {
            ERROR("Failed to detach from %s: errno is %d (%s)\n", ctlPath,
                  errno, strerror(errno));
            close(fd);
            return FALSE;
        }
        close(fd);

#elif HAVE_TTRACE  
        if (ttrace(TT_PROC_DETACH, processId, 0, 0, 0, 0) == -1)
        {
            if (errno == ESRCH)
            {
                ERROR("Invalid process ID: %d\n", processId);
                SetLastError(ERROR_INVALID_PARAMETER);
            }
            else
            {
                ASSERT("ttrace(TT_PROC_DETACH, pid:%d) failed. errno:%d (%s)\n", 
                      processId, errno, strerror(errno));
                SetLastError(ERROR_INTERNAL_ERROR);
            }
            return FALSE;
        }
#else   // HAVE_TTRACE
        if (PAL_PTRACE(PAL_PT_DETACH, processId, 1, 0) == -1)
        {            
            if (errno == ESRCH)
            {
                ERROR("Invalid process ID: %d\n", processId);
                SetLastError(ERROR_INVALID_PARAMETER);
            }
            else
            {
                ASSERT("ptrace(PT_DETACH, pid:%d) failed. errno:%d (%s)\n", 
                      processId, errno, strerror(errno));
                SetLastError(ERROR_INTERNAL_ERROR);
            }
            return FALSE;
        }
#endif  // HAVE_PROCFS_CTL

#if !HAVE_TTRACE
        if (kill(processId, SIGCONT) == -1)
        {
            ERROR("Failed to continue the detached process:%d errno:%d (%s)\n",
                  processId, errno, strerror(errno));
            return FALSE;
        }
#endif  // !HAVE_TTRACE        
    }
    return TRUE;
}

/*++
Function:
  DBGSetProcessAttached

Abstract
  saves the current process Id in the attached process structure

Parameter
  attachedProcId : process ID of the attached process
  attacherProcId : process ID of the attacher process
  bAttach : true (false) to set the process as attached (as detached)
Return
 returns the number of attachment left on attachedProcId, or -1 if it fails
--*/
static int
DBGSetProcessAttached(DWORD attachedProcId,
                      DWORD attacherProcId,
                      BOOL  bAttach)
{
    SHMPROCESS *shmprocess;
    int ret = -1;
    
    SHMLock();
    
    if (attachedProcId == GetCurrentProcessId())
    {
        ERROR("The attached process couldn't be the current process\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto EXIT;        
    }
    
    if(!(shmprocess = PROCFindSHMProcess(attachedProcId)))
    {
        ASSERT("Process 0x%08x was not found in shared memory.\n", 
               attachedProcId);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto EXIT;
    }    
    
    if ((shmprocess->attachedByProcId != attacherProcId) && 
        (shmprocess->attachedByProcId !=0))
    {
        ERROR("process ID:%d already attached by an other process ID:%d\n",
              attachedProcId, shmprocess->attachedByProcId);
        SetLastError(ERROR_NOACCESS);
        goto EXIT;
    }
    
    if (bAttach)
    {
        shmprocess->attachedByProcId = attacherProcId;
        shmprocess->attachCount++;
    }
    else
    {
        if (shmprocess->attachCount-- <= 0)
        {
            ASSERT("attachCount <= 0 check for extra PROCSetProcessDetached calls\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto EXIT;
        }
        
        if (shmprocess->attachCount == 0)
        {
            shmprocess->attachedByProcId = 0;
        }
    }
    ret = shmprocess->attachCount;

EXIT:    
    SHMRelease();
    return ret;
}

/*++
Function:
  PAL_CreateExecWatchpoint

Abstract
  Creates an OS exec watchpoint for the specified instruction
  and thread. This function should only be called on architectures
  that do not support a hardware single-step mode (e.g., SPARC).

Parameter
  hThread : the thread for which the watchpoint is to apply
  pvInstruction : the instruction on which the watchpoint is to be set

Return
  A Win32 error code
--*/

DWORD
PAL_CreateExecWatchpoint(
    HANDLE hThread,
    PVOID pvInstruction
    )
{
    PERF_ENTRY(PAL_CreateExecWatchpoint);
    ENTRY("PAL_CreateExecWatchpoint (hThread=%p, pvInstruction=%p)\n", hThread, pvInstruction);

    DWORD dwError = ERROR_NOT_SUPPORTED;

#if HAVE_PRWATCH_T

    THREAD *lpThread = NULL;
    int fd = -1;
    char ctlPath[MAX_PATH];

    struct
    {
        long ctlCode;
        prwatch_t prwatch;
    } ctlStruct;

    //
    // We must never set a watchpoint on an instruction that enters a syscall;
    // if such a request comes in we succeed it w/o actually creating the
    // watchpoint. This mirrors the behavior of setting the single-step flag
    // in a thread context when the thread is w/in a system service -- the
    // flag is ignored and will not be present when the thread returns
    // to user mode.
    //

#error Need syscall instruction for this platform

    if (hPseudoCurrentThread == hThread)
    {
        hThread = PROCGetRealCurrentThread();
    }
    
    lpThread = (THREAD*) HMGRLockHandle2(hThread, HOBJ_THREAD);
    if (NULL == lpThread)
    {
        ERROR("Unable to access the thread data\n");
        dwError = ERROR_INVALID_HANDLE;
        goto PAL_CreateExecWatchpointExit;
    }

    snprintf(ctlPath, sizeof(ctlPath), "/proc/%u/lwp/%u/lwpctl", getpid(), lpThread->dwLwpId);
    fd = open(ctlPath, O_WRONLY);
    if (-1 == fd)
    {
        ERROR("Failed to open %s\n", ctlPath);
        dwError = ERROR_INVALID_ACCESS;
        goto PAL_CreateExecWatchpointExit;
    }

    ctlStruct.ctlCode = PCWATCH;
    ctlStruct.prwatch.pr_vaddr = (uintptr_t) pvInstruction;
    ctlStruct.prwatch.pr_size = sizeof(DWORD);
    ctlStruct.prwatch.pr_wflags = WA_EXEC | WA_TRAPAFTER;

    if (write(fd, (void*) &ctlStruct, sizeof(ctlStruct)) != sizeof(ctlStruct))
    {
        ERROR("Failure writing control structure (errno = %u)\n", errno);
        dwError = ERROR_INTERNAL_ERROR;
        goto PAL_CreateExecWatchpointExit;
    }

    dwError = ERROR_SUCCESS;
    
PAL_CreateExecWatchpointExit:

    if (NULL != lpThread)
    {
        HMGRUnlockHandle(hThread, &lpThread->objHeader);
    }

    if (-1 != fd)
    {
        close(fd);
    }

#endif // HAVE_PRWATCH_T     
    
    LOGEXIT("PAL_CreateExecWatchpoint returns ret:%d\n", dwError);
    PERF_EXIT(PAL_CreateExecWatchpoint);
    return dwError;
}

/*++
Function:
  PAL_DeleteExecWatchpoint

Abstract
  Deletes an OS exec watchpoint for the specified instruction
  and thread. This function should only be called on architectures
  that do not support a hardware single-step mode (e.g., SPARC).

Parameter
  hThread : the thread to remove the watchpoint from
  pvInstruction : the instruction for which the watchpoint is to be removed

Return
  A Win32 error code. Attempting to delete a watchpoint that does not exist
  may or may not result in an error, depending on the behavior of the
  underlying operating system.
--*/

DWORD
PAL_DeleteExecWatchpoint(
    HANDLE hThread,
    PVOID pvInstruction
    )
{
    PERF_ENTRY(PAL_DeleteExecWatchpoint);
    ENTRY("PAL_DeleteExecWatchpoint (hThread=%p, pvInstruction=%p)\n", hThread, pvInstruction);

    DWORD dwError = ERROR_NOT_SUPPORTED;

#if HAVE_PRWATCH_T

    THREAD *lpThread = NULL;
    int fd = -1;
    char ctlPath[MAX_PATH];

    struct
    {
        long ctlCode;
        prwatch_t prwatch;
    } ctlStruct;


    if (hPseudoCurrentThread == hThread)
    {
        hThread = PROCGetRealCurrentThread();
    }
    
    lpThread = (THREAD*) HMGRLockHandle2(hThread, HOBJ_THREAD);
    if (NULL == lpThread)
    {
        ERROR("Unable to access the thread data\n");
        dwError = ERROR_INVALID_HANDLE;
        goto PAL_DeleteExecWatchpoint;
    }

    snprintf(ctlPath, sizeof(ctlPath), "/proc/%u/lwp/%u/lwpctl", getpid(), lpThread->dwLwpId);
    fd = open(ctlPath, O_WRONLY);
    if (-1 == fd)
    {
        ERROR("Failed to open %s\n", ctlPath);
        dwError = ERROR_INVALID_ACCESS;
        goto PAL_DeleteExecWatchpoint;
    }

    ctlStruct.ctlCode = PCWATCH;
    ctlStruct.prwatch.pr_vaddr = (uintptr_t) pvInstruction;
    ctlStruct.prwatch.pr_size = sizeof(DWORD);
    ctlStruct.prwatch.pr_wflags = 0;

    if (write(fd, (void*) &ctlStruct, sizeof(ctlStruct)) != sizeof(ctlStruct))
    {
        ERROR("Failure writing control structure (errno = %u)\n", errno);
        dwError = ERROR_INTERNAL_ERROR;
        goto PAL_DeleteExecWatchpoint;
    }

    dwError = ERROR_SUCCESS;
    
PAL_DeleteExecWatchpoint:

    if (NULL != lpThread)
    {
        HMGRUnlockHandle(hThread, &lpThread->objHeader);
    }

    if (-1 != fd)
    {
        close(fd);
    }

#endif // HAVE_PRWATCH_T    
    
    LOGEXIT("PAL_DeleteExecWatchpoint returns ret:%d\n", dwError);
    PERF_EXIT(PAL_DeleteExecWatchpoint);
    return dwError;
}

