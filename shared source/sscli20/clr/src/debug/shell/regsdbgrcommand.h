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
/*------------------------------------------------------------------------- *
 * RegsDbgrCommand.h: com debugger shell functions
 * ------------------------------------------------------------------------- */

#ifndef __REGSDBGRCOMMAND_H__
#define __REGSDBGRCOMMAND_H__

#include "stdafx.h"
#include "palclr.h"

#include "cordbpriv.h"
#include <log.h>


#include "internalonly.h"



enum DispRegRegisters
{
#ifdef _X86_
    REGISTER_X86_EFL = REGISTER_X86_FPSTACK_7 +1,
    REGISTER_X86_CS,
    REGISTER_X86_DS,
    REGISTER_X86_ES,
    REGISTER_X86_FS,
    REGISTER_X86_GS,
    REGISTER_X86_SS,
    REGISTER_X86_EFLAGS_CY,
    REGISTER_X86_EFLAGS_PE,
    REGISTER_X86_EFLAGS_AC,
    REGISTER_X86_EFLAGS_ZR,
    REGISTER_X86_EFLAGS_PL,
    REGISTER_X86_EFLAGS_EI,
    REGISTER_X86_EFLAGS_UP,
    REGISTER_X86_EFLAGS_OV,
#endif // _X86_
    INVALID_REGISTER
};


#ifdef _X86_

#define X86_EFLAGS_CY   SETBITULONG64(0)    //Carry Set
#define X86_EFLAGS_PE   SETBITULONG64(2)    //Parity Even?
#define X86_EFLAGS_AC   SETBITULONG64(4)    //Aux. Carry
#define X86_EFLAGS_ZR   SETBITULONG64(6)    //Zero Set
#define X86_EFLAGS_PL   SETBITULONG64(7)    //Sign positive
#define X86_EFLAGS_EI   SETBITULONG64(9)    //Enabled Interrupt
#define X86_EFLAGS_UP   SETBITULONG64(10)   //Direction increment
#define X86_EFLAGS_OV   SETBITULONG64(11)   //Overflow Set

#define NUMREGNAMES (REGISTER_X86_EFLAGS_OV+1)
static WCHAR *g_RegNames[NUMREGNAMES] = {
                                  L"EIP", L"ESP", L"EBP", L"EAX",
                                  L"ECX", L"EDX", L"EBX", L"ESI",
                                  L"EDI", L"ST0", L"ST1", L"ST2",
                                  L"ST3", L"ST4", L"ST5", L"ST6",
                                  L"ST7", L"EFL", L"CS", L"DS",
                                  L"ES", L"FS", L"GS", L"SS",
                                  L"CY", L"PE", L"AC", L"ZR",
                                  L"PL", L"EI", L"UP", L"OV" };
#elif defined(_PPC_)

#define NUMREGNAMES 3
static WCHAR *g_RegNames[NUMREGNAMES] = {
                               // IP,       SP,     FP
                                  L"IAR",   L"GPR1", L"GPR30" };
#endif


class RegistersDebuggerCommand : public DebuggerCommand
{
private:
    int m_nBase;

public:
    RegistersDebuggerCommand(const WCHAR *name, int minMatchLength = 0)
        : DebuggerCommand(name, minMatchLength)
    {
    }

    void Do(Shell *shell, const WCHAR *args);

    virtual void Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args);

    virtual void InternalDo(DebuggerShell* shell, DebuggerUnmanagedThread* ut,
                            CONTEXT* pContext, ICorDebugRegisterSet *pIRS,
                            const WCHAR* szReg, bool fPrinAll);

    void WriteRegSection(DebuggerShell* pShell, CONTEXT* pContext, ICorDebugRegisterSet *pIRS,
                         int regsPerLine, int lowerBound, int upperBound, int* pNumRegsWritten);

    int LookupRegisterIndexByName(const WCHAR *wszReg);

    bool WriteReg(UINT iReg,
                  CONTEXT *pContext,
                  DebuggerShell *shell,
                  ICorDebugRegisterSet *pIRS = NULL);


    // Provide help specific to this command
    void Help(Shell *shell)
    {
        ShellCommand::Help(shell);
        shell->Write(L"\n");
        shell->Write(L"Displays the contents of the registers for\n");
        shell->Write(L"the current thread.\n");
        shell->Write(L"\n");
    }

    const WCHAR *ShortHelp(Shell *shell)
    {
        return L"Display CPU registers for current thread";
    }
}; //RegistersDebuggerCommand


#endif // __REGSDBGRCOMMAND_H__
