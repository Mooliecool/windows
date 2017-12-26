/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    context.c

Abstract:

    Implementation of GetThreadContext/SetThreadContext/DebugBreak functions for
    the Intel x86 platform. These functions are processor dependent.

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/thread.h"
#include "pal/context.h"
#include "pal/debug.h"

#include <sys/ptrace.h> 
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#if HAVE_BSD_REGS_T
#include <machine/reg.h>
#include <machine/npx.h>
#endif  // HAVE_BSD_REGS_T
#if HAVE_PT_REGS
#include <asm/ptrace.h>
#endif  // HAVE_PT_REGS

SET_DEFAULT_DEBUG_CHANNEL(DEBUG);

#if HAVE_GREGSET_T

#define MCREG_Ebx(mc)       ((mc).gregs[REG_EBX])
#define MCREG_Ecx(mc)       ((mc).gregs[REG_ECX])
#define MCREG_Edx(mc)       ((mc).gregs[REG_EDX])
#define MCREG_Esi(mc)       ((mc).gregs[REG_ESI])
#define MCREG_Edi(mc)       ((mc).gregs[REG_EDI])
#define MCREG_Ebp(mc)       ((mc).gregs[REG_EBP])
#define MCREG_Eax(mc)       ((mc).gregs[REG_EAX])
#define MCREG_Eip(mc)       ((mc).gregs[REG_EIP])
#define MCREG_SegCs(mc)     ((mc).gregs[REG_CS])
#define MCREG_EFlags(mc)    ((mc).gregs[REG_EFL])
#define MCREG_Esp(mc)       ((mc).gregs[REG_ESP])
#define MCREG_SegSs(mc)     ((mc).gregs[REG_SS])

#else // HAVE_GREGSET_T

#define MCREG_Ebx(mc)       ((mc).mc_ebx)
#define MCREG_Ecx(mc)       ((mc).mc_ecx)
#define MCREG_Edx(mc)       ((mc).mc_edx)
#define MCREG_Esi(mc)       ((mc).mc_esi)
#define MCREG_Edi(mc)       ((mc).mc_edi)
#define MCREG_Ebp(mc)       ((mc).mc_ebp)
#define MCREG_Eax(mc)       ((mc).mc_eax)
#define MCREG_Eip(mc)       ((mc).mc_eip)
#define MCREG_SegCs(mc)     ((mc).mc_cs)
#define MCREG_EFlags(mc)    ((mc).mc_eflags)
#define MCREG_Esp(mc)       ((mc).mc_esp)
#define MCREG_SegSs(mc)     ((mc).mc_ss)

#endif // HAVE_GREGSET_T


#if HAVE_PT_REGS

#define PTREG_Ebx(ptreg)    ((ptreg).ebx)
#define PTREG_Ecx(ptreg)    ((ptreg).ecx)
#define PTREG_Edx(ptreg)    ((ptreg).edx)
#define PTREG_Esi(ptreg)    ((ptreg).esi)
#define PTREG_Edi(ptreg)    ((ptreg).edi)
#define PTREG_Ebp(ptreg)    ((ptreg).ebp)
#define PTREG_Eax(ptreg)    ((ptreg).eax)
#define PTREG_Eip(ptreg)    ((ptreg).eip)
#define PTREG_SegCs(ptreg)  ((ptreg).xcs)
#define PTREG_EFlags(ptreg) ((ptreg).eflags)
#define PTREG_Esp(ptreg)    ((ptreg).esp)
#define PTREG_SegSs(ptreg)  ((ptreg).xss)

#endif // HAVE_PT_REGS


#if HAVE_BSD_REGS_T

#define BSDREG_Ebx(reg)     ((reg).r_ebx)
#define BSDREG_Ecx(reg)     ((reg).r_ecx)
#define BSDREG_Edx(reg)     ((reg).r_edx)
#define BSDREG_Esi(reg)     ((reg).r_esi)
#define BSDREG_Edi(reg)     ((reg).r_edi)
#define BSDREG_Ebp(reg)     ((reg).r_ebp)
#define BSDREG_Eax(reg)     ((reg).r_eax)
#define BSDREG_Eip(reg)     ((reg).r_eip)
#define BSDREG_SegCs(reg)   ((reg).r_cs)
#define BSDREG_EFlags(reg)  ((reg).r_eflags)
#define BSDREG_Esp(reg)     ((reg).r_esp)
#define BSDREG_SegSs(reg)   ((reg).r_ss)

#endif // HAVE_BSD_REGS_T


#define ASSIGN_CONTROL_REGS \
        ASSIGN_REG(Ebp)     \
        ASSIGN_REG(Eip)     \
        ASSIGN_REG(SegCs)   \
        ASSIGN_REG(EFlags)  \
        ASSIGN_REG(Esp)     \
        ASSIGN_REG(SegSs)   \

#define ASSIGN_INTEGER_REGS \
        ASSIGN_REG(Edi)     \
        ASSIGN_REG(Esi)     \
        ASSIGN_REG(Ebx)     \
        ASSIGN_REG(Edx)     \
        ASSIGN_REG(Ecx)     \
        ASSIGN_REG(Eax)     \

#define ASSIGN_ALL_REGS     \
        ASSIGN_CONTROL_REGS \
        ASSIGN_INTEGER_REGS \


/*++
Function:
  CONTEXT_GetRegisters

Abstract
  retrieve the machine registers value of the indicated process.

Parameter
  processId: process ID
  registers: reg structure in which the machine registers value will be returned.
Return
 returns TRUE if it succeeds, FALSE otherwise
--*/
BOOL CONTEXT_GetRegisters(DWORD processId, ucontext_t *registers)
{
#if HAVE_BSD_REGS_T
    int regFd = -1;
#endif  // HAVE_BSD_REGS_T
    BOOL bRet = FALSE;

    if (processId == GetCurrentProcessId()) 
    {
#if HAVE_GETCONTEXT
        if (getcontext(registers) != 0)
        {
            ASSERT("getcontext() failed %d (%s)\n", errno, strerror(errno));
            return FALSE;
        }
#elif HAVE_BSD_REGS_T
        char buf[MAX_PATH];
        struct reg bsd_registers;

        sprintf(buf, "/proc/%d/regs", processId);

        if ((regFd = open(buf, O_RDONLY)) == -1) 
        {
          ASSERT("open() failed %d (%s) \n", errno, strerror(errno));
          return FALSE;
        }

        if (lseek(regFd, 0, 0) == -1)
        {
            ASSERT("lseek() failed %d (%s)\n", errno, strerror(errno));
            goto EXIT;
        }

        if (read(regFd, &bsd_registers, sizeof(bsd_registers)) != sizeof(bsd_registers))
        {
            ASSERT("read() failed %d (%s)\n", errno, strerror(errno));
            goto EXIT;
        }

#define ASSIGN_REG(reg) MCREG_##reg(registers->uc_mcontext) = BSDREG_##reg(bsd_registers);
        ASSIGN_ALL_REGS
#undef ASSIGN_REG

#else
#error "Don't know how to get current context on this platform!"
#endif
    }
    else
    {
#if HAVE_PT_REGS
        struct pt_regs ptrace_registers;
#elif HAVE_BSD_REGS_T
        struct reg ptrace_registers;
#endif

        if (DBGAttachProcess(processId) == FALSE)
        {
            goto EXIT;
        }

        if (ptrace(PT_GETREGS, processId, (caddr_t) &ptrace_registers, 0) == -1)
        {
            ASSERT("Failed ptrace(PT_GETREGS, processId:%d) errno:%d (%s)\n",
                   processId, errno, strerror(errno));
        }

#if HAVE_PT_REGS
#define ASSIGN_REG(reg) MCREG_##reg(registers->uc_mcontext) = PTREG_##reg(ptrace_registers);
#elif HAVE_BSD_REGS_T
#define ASSIGN_REG(reg) MCREG_##reg(registers->uc_mcontext) = BSDREG_##reg(ptrace_registers);
#endif
        ASSIGN_ALL_REGS
#undef ASSIGN_REG

        if (!DBGDetachProcess(processId))
        {            
            goto EXIT;
        }
    }
    
    bRet = TRUE;
EXIT:
#if HAVE_BSD_REGS_T
    if (regFd != -1)
    {
        close(regFd);
    }
#endif  // HAVE_BSD_REGS_T
    return bRet;
}

/*++
Function:
  GetThreadContext

See MSDN doc.
--*/
BOOL
CONTEXT_GetThreadContext(
         HANDLE hThread,
         LPCONTEXT lpContext)
{    
    BOOL ret = FALSE;
    DWORD processId;
    ucontext_t registers;

    if (lpContext == NULL)
    {
        ERROR("Invalid lpContext parameter value\n");
        SetLastError(ERROR_NOACCESS);
        goto EXIT;
    }
    
    /* How to consider the case when hThread is different from the current
       thread of its owner process. Machine registers values could be retreived
       by a ptrace(pid, ...) call or from the "/proc/%pid/reg" file content. 
       Unfortunately, these two methods only depend on process ID, not on 
       thread ID. */
    if (!(processId = THREADGetThreadProcessId(hThread)))
    {
        ERROR("Couldn't retrieve the process owner of hThread:%p\n", hThread);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto EXIT;
    }

    if (processId == GetCurrentProcessId())
    {
        THREAD *thread;

        thread = (THREAD *) HMGRLockHandle2(hThread, HOBJ_THREAD);

        if ((thread == NULL) || (thread->dwThreadId != GetCurrentThreadId()))
        {
            DWORD flags;
            // There aren't any APIs for this. We can potentially get the
            // context of another thread by using per-thread signals, but
            // on FreeBSD signal handlers that are called as a result
            // of signals raised via pthread_kill don't get a valid
            // sigcontext or ucontext_t. But we need this to return TRUE
            // to avoid an assertion in the CLR in code that manages to
            // cope reasonably well without a valid thread context.
            // Given that, we'll zero out our structure and return TRUE.
            ERROR("GetThreadContext on a thread other than the current "
                  "thread is returning TRUE\n");
            flags = lpContext->ContextFlags;
            memset(lpContext, 0, sizeof(*lpContext));
            lpContext->ContextFlags = flags;
            HMGRUnlockHandle(hThread, &(thread->objHeader));
            ret = TRUE;
            goto EXIT;
        }

        HMGRUnlockHandle(hThread, &(thread->objHeader));
    }

    if (lpContext->ContextFlags & 
        (CONTEXT_CONTROL | CONTEXT_INTEGER))
    {        
        if (CONTEXT_GetRegisters(processId, &registers) == FALSE)
        {
            SetLastError(ERROR_INTERNAL_ERROR);
            goto EXIT;
        }

#define ASSIGN_REG(reg) lpContext->reg = MCREG_##reg(registers.uc_mcontext);
        if (lpContext->ContextFlags & CONTEXT_CONTROL)
        {
            ASSIGN_CONTROL_REGS
        }
        if (lpContext->ContextFlags & CONTEXT_INTEGER)
        {
            ASSIGN_INTEGER_REGS
        }
#undef ASSIGN_REG
    }

    ret = TRUE;

EXIT:
    return ret;
}

/*++
Function:
  SetThreadContext

See MSDN doc.
--*/
BOOL
CONTEXT_SetThreadContext(
           IN HANDLE hThread,
           IN CONST CONTEXT *lpContext)
{
    BOOL ret = FALSE;
    DWORD processId;

#if HAVE_PT_REGS
    struct pt_regs ptrace_registers;
#elif HAVE_BSD_REGS_T
    struct reg ptrace_registers;
#endif

    if (lpContext == NULL)
    {
        ERROR("Invalid lpContext parameter value\n");
        SetLastError(ERROR_NOACCESS);
        goto EXIT;
    }
    
    /* How to consider the case when hThread is different from the current
       thread of its owner process. Machine registers values could be retreived
       by a ptrace(pid, ...) call or from the "/proc/%pid/reg" file content. 
       Unfortunately, these two methods only depend on process ID, not on 
       thread ID. */
    if (!(processId = THREADGetThreadProcessId(hThread)))
    {
        ASSERT("Couldn't retrieve the process owner of hThread:%p\n", hThread);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto EXIT;
    }
        
    if (processId == GetCurrentProcessId())
    {
        ASSERT("SetThreadContext should be called for cross-process only.\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto EXIT;
    }
    
    if (lpContext->ContextFlags  & 
        (CONTEXT_CONTROL | CONTEXT_INTEGER))
    {   
        if (ptrace(PT_GETREGS, processId, (caddr_t) &ptrace_registers, 0) == -1)
        {
            ASSERT("Failed ptrace(PT_GETREGS, processId:%d) errno:%d (%s)\n",
                   processId, errno, strerror(errno));
             SetLastError(ERROR_INTERNAL_ERROR);
             goto EXIT;
        }

#if HAVE_PT_REGS
#define ASSIGN_REG(reg) PTREG_##reg(ptrace_registers) = lpContext->reg;
#elif HAVE_BSD_REGS_T
#define ASSIGN_REG(reg) BSDREG_##reg(ptrace_registers) = lpContext->reg;
#endif
        if (lpContext->ContextFlags & CONTEXT_CONTROL)
        {
            ASSIGN_CONTROL_REGS
        }
        if (lpContext->ContextFlags & CONTEXT_INTEGER)
        {
            ASSIGN_INTEGER_REGS
        }
#undef ASSIGN_REG
        
        if (ptrace(PT_SETREGS, processId, (caddr_t) &ptrace_registers, 0) == -1)
        {
            ASSERT("Failed ptrace(PT_SETREGS, processId:%d) errno:%d (%s)\n",
                   processId, errno, strerror(errno));
            SetLastError(ERROR_INTERNAL_ERROR);
            goto EXIT;
        }
    }

    ret = TRUE;
   EXIT:
     return ret;
}

/*++
Function:
  DBG_DebugBreak: same as DebugBreak

See MSDN doc.
--*/
VOID
DBG_DebugBreak()
{
    __asm__ __volatile__("int $3");
}


/*++
Function:
  DBG_FlushInstructionCache: processor-specific portion of 
  FlushInstructionCache

See MSDN doc.
--*/
BOOL
DBG_FlushInstructionCache(
                          IN LPCVOID lpBaseAddress,
                          IN SIZE_T dwSize)
{
    // Intel x86 hardware has cache coherency, so nothing needs to be done.
    return TRUE;
}

/*++
Function :
    CONTEXTToNativeContext
    
    Converts a CONTEXT record to a native context.

Parameters :
    CONST CONTEXT *lpContext : CONTEXT to convert
    native_context_t *native : native context to fill in
    ULONG contextFlags : flags that determine which registers are valid in
                         lpContext and which ones to set in native

Return value :
    None

--*/
void CONTEXTToNativeContext(CONST CONTEXT *lpContext, native_context_t *native,
                            ULONG contextFlags)
{
    if (contextFlags != (CONTEXT_CONTROL | CONTEXT_INTEGER))
    {
        ASSERT("Invalid contextFlags in CONTEXTToNativeContext!");
    }
    
#define ASSIGN_REG(reg) MCREG_##reg(native->uc_mcontext) = lpContext->reg;
    ASSIGN_ALL_REGS
#undef ASSIGN_REG
}

/*++
Function :
    CONTEXTFromNativeContext
    
    Converts a native context to a CONTEXT record.

Parameters :
    const native_context_t *native : native context to convert
    LPCONTEXT lpContext : CONTEXT to fill in
    ULONG contextFlags : flags that determine which registers are valid in
                         native and which ones to set in lpContext

Return value :
    None

--*/
void CONTEXTFromNativeContext(const native_context_t *native, LPCONTEXT lpContext,
                              ULONG contextFlags)
{
    if (contextFlags != (CONTEXT_CONTROL | CONTEXT_INTEGER))
    {
        ASSERT("Invalid contextFlags in CONTEXTFromNativeContext!");
    }
    lpContext->ContextFlags = contextFlags;

#define ASSIGN_REG(reg) lpContext->reg = MCREG_##reg(native->uc_mcontext);
    ASSIGN_ALL_REGS
#undef ASSIGN_REG
}

/*++
Function :
    CONTEXTGetPC
    
    Returns the program counter from the native context.

Parameters :
    const native_context_t *native : native context

Return value :
    The program counter from the native context.

--*/
LPVOID CONTEXTGetPC(const native_context_t *context)
{
    return (LPVOID) MCREG_Eip(context->uc_mcontext);
}

/*++
Function :
    CONTEXTGetExceptionCodeForSignal
    
    Translates signal and context information to a Win32 exception code.

Parameters :
    const siginfo_t *siginfo : signal information from a signal handler
    const native_context_t *context : context information

Return value :
    The Win32 exception code that corresponds to the signal and context
    information.

--*/
#ifdef ILL_ILLOPC
// If si_code values are available for all signals, use those.
DWORD CONTEXTGetExceptionCodeForSignal(const siginfo_t *siginfo,
                                       const native_context_t *context)
{
    switch (siginfo->si_signo)
    {
        case SIGILL:
            switch (siginfo->si_code)
            {
                case ILL_ILLOPC:    // Illegal opcode
                case ILL_ILLOPN:    // Illegal operand
                case ILL_ILLADR:    // Illegal addressing mode
                case ILL_ILLTRP:    // Illegal trap
                case ILL_COPROC:    // Co-processor error
                    return EXCEPTION_ILLEGAL_INSTRUCTION;
                case ILL_PRVOPC:    // Privileged opcode
                case ILL_PRVREG:    // Privileged register
                    return EXCEPTION_PRIV_INSTRUCTION;
                case ILL_BADSTK:    // Internal stack error
                    return EXCEPTION_STACK_OVERFLOW;
                default:
                    break;
            }
            break;
        case SIGFPE:
            switch (siginfo->si_code)
            {
                case FPE_INTDIV:
                    return EXCEPTION_INT_DIVIDE_BY_ZERO;
                case FPE_INTOVF:
                    return EXCEPTION_INT_OVERFLOW;
                case FPE_FLTDIV:
                    return EXCEPTION_FLT_DIVIDE_BY_ZERO;
                case FPE_FLTOVF:
                    return EXCEPTION_FLT_OVERFLOW;
                case FPE_FLTUND:
                    return EXCEPTION_FLT_UNDERFLOW;
                case FPE_FLTRES:
                    return EXCEPTION_FLT_INEXACT_RESULT;
                case FPE_FLTINV:
                    return EXCEPTION_FLT_INVALID_OPERATION;
                case FPE_FLTSUB:
                    return EXCEPTION_FLT_INVALID_OPERATION;
                default:
                    break;
            }
            break;
        case SIGSEGV:
            switch (siginfo->si_code)
            {
                case SI_USER:       // User-generated signal, sometimes sent
                                    // for SIGSEGV under normal circumstances
                case SEGV_MAPERR:   // Address not mapped to object
                case SEGV_ACCERR:   // Invalid permissions for mapped object
                    return EXCEPTION_ACCESS_VIOLATION;
                default:
                    break;
            }
            break;
        case SIGBUS:
            switch (siginfo->si_code)
            {
                case BUS_ADRALN:    // Invalid address alignment
                    return EXCEPTION_DATATYPE_MISALIGNMENT;
                case BUS_ADRERR:    // Non-existent physical address
                    return EXCEPTION_ACCESS_VIOLATION;
                case BUS_OBJERR:    // Object-specific hardware error
                default:
                    break;
            }
        case SIGTRAP:
            switch (siginfo->si_code)
            {
                case SI_KERNEL:
                case SI_USER:
                case TRAP_BRKPT:    // Process breakpoint
                    return EXCEPTION_BREAKPOINT;
                case TRAP_TRACE:    // Process trace trap
                    return EXCEPTION_SINGLE_STEP;
                default:
                    // We don't want to use ASSERT here since it raises SIGTRAP and we
                    // might again end up here resulting in an infinite loop! 
                    // so, we print out an error message and return 
                    DBG_PRINTF(DLI_ASSERT, defdbgchan, TRUE) 
                    ("Got unknown SIGTRAP signal (%d) with code %d\n", SIGTRAP, siginfo->si_code);

                    return EXCEPTION_ILLEGAL_INSTRUCTION;
            }
        default:
            break;
    }
    ASSERT("Got unknown signal number %d with code %d\n",
           siginfo->si_signo, siginfo->si_code);
    return EXCEPTION_ILLEGAL_INSTRUCTION;
}
#else   // ILL_ILLOPC
DWORD CONTEXTGetExceptionCodeForSignal(const siginfo_t *siginfo,
                                       const native_context_t *context)
{
    int trap;

    if (siginfo->si_signo == SIGFPE)
    {
        // Floating point exceptions are mapped by their si_code.
        switch (siginfo->si_code)
        {
            case FPE_INTDIV :
                TRACE("Got signal SIGFPE:FPE_INTDIV; raising "
                      "EXCEPTION_INT_DIVIDE_BY_ZERO\n");
                return EXCEPTION_INT_DIVIDE_BY_ZERO;
                break;
            case FPE_INTOVF :
                TRACE("Got signal SIGFPE:FPE_INTOVF; raising "
                      "EXCEPTION_INT_OVERFLOW\n");
                return EXCEPTION_INT_OVERFLOW;
                break;
            case FPE_FLTDIV :
                TRACE("Got signal SIGFPE:FPE_FLTDIV; raising "
                      "EXCEPTION_FLT_DIVIDE_BY_ZERO\n");
                return EXCEPTION_FLT_DIVIDE_BY_ZERO;
                break;
            case FPE_FLTOVF :
                TRACE("Got signal SIGFPE:FPE_FLTOVF; raising "
                      "EXCEPTION_FLT_OVERFLOW\n");
                return EXCEPTION_FLT_OVERFLOW;
                break;
            case FPE_FLTUND :
                TRACE("Got signal SIGFPE:FPE_FLTUND; raising "
                      "EXCEPTION_FLT_UNDERFLOW\n");
                return EXCEPTION_FLT_UNDERFLOW;
                break;
            case FPE_FLTRES :
                TRACE("Got signal SIGFPE:FPE_FLTRES; raising "
                      "EXCEPTION_FLT_INEXACT_RESULT\n");
                return EXCEPTION_FLT_INEXACT_RESULT;
                break;
            case FPE_FLTINV :
                TRACE("Got signal SIGFPE:FPE_FLTINV; raising "
                      "EXCEPTION_FLT_INVALID_OPERATION\n");
                return EXCEPTION_FLT_INVALID_OPERATION;
                break;
            case FPE_FLTSUB :/* subscript out of range */
                TRACE("Got signal SIGFPE:FPE_FLTSUB; raising "
                      "EXCEPTION_FLT_INVALID_OPERATION\n");
                return EXCEPTION_FLT_INVALID_OPERATION;
                break;
            default:
                ASSERT("Got unknown signal code %d\n", siginfo->si_code);
                break;
        }
    }

    trap = context->uc_mcontext.mc_trapno;
    switch (trap)
    {
        case T_PRIVINFLT : /* privileged instruction */
            TRACE("Trap code T_PRIVINFLT mapped to EXCEPTION_PRIV_INSTRUCTION\n");
            return EXCEPTION_PRIV_INSTRUCTION; 
        case T_BPTFLT :    /* breakpoint instruction */
            TRACE("Trap code T_BPTFLT mapped to EXCEPTION_BREAKPOINT\n");
            return EXCEPTION_BREAKPOINT;
        case T_ARITHTRAP : /* arithmetic trap */
            TRACE("Trap code T_ARITHTRAP maps to floating point exception...\n");
            return 0;      /* let the caller pick an exception code */
#ifdef T_ASTFLT
        case T_ASTFLT :    /* system forced exception : ^C, ^\. SIGINT signal 
                              handler shouldn't be calling this function, since
                              it doesn't need an exception code */
            ASSERT("Trap code T_ASTFLT received, shouldn't get here\n");
            return 0;
#endif  // T_ASTFLT
        case T_PROTFLT :   /* protection fault */
            TRACE("Trap code T_PROTFLT mapped to EXCEPTION_ACCESS_VIOLATION\n");
            return EXCEPTION_ACCESS_VIOLATION; 
        case T_TRCTRAP :   /* debug exception (sic) */
            TRACE("Trap code T_TRCTRAP mapped to EXCEPTION_SINGLE_STEP\n");
            return EXCEPTION_SINGLE_STEP;
        case T_PAGEFLT :   /* page fault */
            TRACE("Trap code T_PAGEFLT mapped to EXCEPTION_ACCESS_VIOLATION\n");
            return EXCEPTION_ACCESS_VIOLATION;
        case T_ALIGNFLT :  /* alignment fault */
            TRACE("Trap code T_ALIGNFLT mapped to EXCEPTION_DATATYPE_MISALIGNMENT\n");
            return EXCEPTION_DATATYPE_MISALIGNMENT;
        case T_DIVIDE :
            TRACE("Trap code T_DIVIDE mapped to EXCEPTION_INT_DIVIDE_BY_ZERO\n");
            return EXCEPTION_INT_DIVIDE_BY_ZERO;
        case T_NMI :       /* non-maskable trap */
            TRACE("Trap code T_NMI mapped to EXCEPTION_ILLEGAL_INSTRUCTION\n");
            return EXCEPTION_ILLEGAL_INSTRUCTION;
        case T_OFLOW :
            TRACE("Trap code T_OFLOW mapped to EXCEPTION_INT_OVERFLOW\n");
            return EXCEPTION_INT_OVERFLOW;
        case T_BOUND :     /* bound instruction fault */
            TRACE("Trap code T_BOUND mapped to EXCEPTION_ARRAY_BOUNDS_EXCEEDED\n");
            return EXCEPTION_ARRAY_BOUNDS_EXCEEDED; 
        case T_DNA :       /* device not available fault */
            TRACE("Trap code T_DNA mapped to EXCEPTION_ILLEGAL_INSTRUCTION\n");
            return EXCEPTION_ILLEGAL_INSTRUCTION; 
        case T_DOUBLEFLT : /* double fault */
            TRACE("Trap code T_DOUBLEFLT mapped to EXCEPTION_ILLEGAL_INSTRUCTION\n");
            return EXCEPTION_ILLEGAL_INSTRUCTION; 
        case T_FPOPFLT :   /* fp coprocessor operand fetch fault */
            TRACE("Trap code T_FPOPFLT mapped to EXCEPTION_FLT_INVALID_OPERATION\n");
            return EXCEPTION_FLT_INVALID_OPERATION; 
        case T_TSSFLT :    /* invalid tss fault */
            TRACE("Trap code T_TSSFLT mapped to EXCEPTION_ILLEGAL_INSTRUCTION\n");
            return EXCEPTION_ILLEGAL_INSTRUCTION; 
        case T_SEGNPFLT :  /* segment not present fault */
            TRACE("Trap code T_SEGNPFLT mapped to EXCEPTION_ACCESS_VIOLATION\n");
            return EXCEPTION_ACCESS_VIOLATION; 
        case T_STKFLT :    /* stack fault */
            TRACE("Trap code T_STKFLT mapped to EXCEPTION_STACK_OVERFLOW\n");
            return EXCEPTION_STACK_OVERFLOW; 
        case T_MCHK :      /* machine check trap */
            TRACE("Trap code T_MCHK mapped to EXCEPTION_ILLEGAL_INSTRUCTION\n");
            return EXCEPTION_ILLEGAL_INSTRUCTION; 
        case T_RESERVED :  /* reserved (unknown) */
            TRACE("Trap code T_RESERVED mapped to EXCEPTION_ILLEGAL_INSTRUCTION\n");
            return EXCEPTION_ILLEGAL_INSTRUCTION; 
        default:
            ASSERT("Got unknown trap code %d\n", trap);
            break;
    }
    return EXCEPTION_ILLEGAL_INSTRUCTION;
}
#endif  // ILL_ILLOPC

#if (__GNUC__ > 3 ||                                            \
     (__GNUC__ == 3 && __GNUC_MINOR__ > 2))
/*++
Function:
  HijackForUnwind: Set the context for executing thread from arg1, and simulate a call to arg2

  */
VOID
HijackForUnwind(IN LPCONTEXT context, IN LPVOID target)
{
    ULONG retAddr;

    if (context == NULL)
        ASSERT("Context was null");

    if (target == NULL)
        ASSERT("target was null");

    // ideally retAddr needs to point to the instruction after the
    // faulting instruction
    // The hope is that the pointer just needs to point somewhere
    // between the two instructions, i.e., it need not be exact.
    retAddr = context->Eip + 1;
    
    __asm__ __volatile__ (
        // move context argument into fixed register
        "mov %12, %%eax\n\t"
        
        // Note on inline asm: the 'c' modifier tells gcc to omit the
        // '$' that it usually adds on to an immediate constant.

        // Load new stack pointer from context
        "mov (%c10)(%%eax), %%esp\n\t"
        
        // save return address on stack
        "push %14\n\t"

        // save target address on stack
        "push %13\n\t"

        // load all registers in from context
        "mov (%c0)(%%eax), %%edi\n\t"
        "mov (%c1)(%%eax), %%esi\n\t"
        "mov (%c2)(%%eax), %%ebx\n\t"
        "mov (%c3)(%%eax), %%edx\n\t"
        "mov (%c4)(%%eax), %%ecx\n\t"
        // %5: eax later
        "mov (%c6)(%%eax), %%ebp\n\t"
        // %7: eip not needed (return at the end here takes care of that)
        // %8: cs not needed (we're in the same code segment as target)

        // %9: EFlags has to be moved in this special way
        "push (%c9)(%%eax)\n\t"
        "popf\n\t"

        // %10: esp is done
        // %11: ss again special
        "push (%c11)(%%eax)\n\t"
        "pop %%ss\n\t"

        // eax has to be last
        "mov (%c5)(%%eax), %%eax\n\t"

        // target IP is first thing on stack now
        // that's the address we "return" to
        "ret\n\t"
        :
        : "i" (OffsetOf(CONTEXT, Edi)),		// 0
          "i" (OffsetOf(CONTEXT, Esi)),		// 1
          "i" (OffsetOf(CONTEXT, Ebx)),		// 2
          "i" (OffsetOf(CONTEXT, Edx)),		// 3
          "i" (OffsetOf(CONTEXT, Ecx)),		// 4
          "i" (OffsetOf(CONTEXT, Eax)),		// 5
          "i" (OffsetOf(CONTEXT, Ebp)),		// 6
          "i" (OffsetOf(CONTEXT, Eip)),		// 7
          "i" (OffsetOf(CONTEXT, SegCs)),	// 8
          "i" (OffsetOf(CONTEXT, EFlags)),	// 9
          "i" (OffsetOf(CONTEXT, Esp)),		// 10
          "i" (OffsetOf(CONTEXT, SegSs)),	// 11
          "r" (context), "r" (target),		// 12, 13
          "r" (retAddr)				// 14
        : "eax"
       );
}
#endif
