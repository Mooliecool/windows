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
 * RegsDbgrCommand.cpp: platform-specific methods for the 
 *                      RegistersDebuggerCommand class
 * ------------------------------------------------------------------------- */

#include "stdafx.h"
#include "../regsdbgrcommand.h"


void RegistersDebuggerCommand::InternalDo(DebuggerShell* pShell, DebuggerUnmanagedThread* pDUThread, 
                                          CONTEXT* pContext, ICorDebugRegisterSet *pIRS, 
                                          const WCHAR* wszReg, bool fPrintAll)
{
    HRESULT hr = S_OK;

    int nRegsWritten = 1;

    // Write out all the registers, unless we were given a
    // specific register to print out.
    if (fPrintAll)
    {
        // Print the thread ID
        DWORD id;

        if (pShell->m_currentThread)
        {
            hr = pShell->m_currentThread->GetID(&id);
        
            if (FAILED(hr))
            {
                pShell->ReportError(hr);
                return;
            }
        }
        else
            id = (DWORD) pDUThread->GetId();
        
        // Output thread ID
        pShell->Write(L"Thread 0x%x:\n", id);

        for (int i = REGISTER_INSTRUCTION_POINTER; i <= REGISTER_FRAME_POINTER; i++)
        {
            WriteReg(i, pContext, pShell);

            pShell->Write(L" ");
            nRegsWritten++;
        }
    }
    else
    {
        if (!WriteReg(LookupRegisterIndexByName(wszReg), pContext, pShell))
            pShell->Write(L"Register %s unknown or unprintable\n", wszReg);
    }
    pShell->Write(L"\n");
}


bool RegistersDebuggerCommand::WriteReg(UINT iReg,
                                        CONTEXT *pContext,
                                        DebuggerShell *shell, 
                                        ICorDebugRegisterSet *pIRS)
{
    WCHAR wszTemp[30];

    _ASSERTE( pContext != NULL );
    _ASSERTE(sizeof (double) == sizeof (CORDB_REGISTER));
    _ASSERTE(m_nBase == 16 || m_nBase == 10);

#define WRITE_REG(_shell, _val, _name, _tmp, _base) \
    (_shell)->Write(L"%s = %08s", (_name), _ui64tow((_val), (_tmp), (_base)));
                    
    switch( iReg )
    {
        case REGISTER_INSTRUCTION_POINTER:
            WRITE_REG(shell, (ULONG)GetIP(pContext), g_RegNames[iReg], wszTemp, m_nBase);
            break;
        case REGISTER_STACK_POINTER:
            WRITE_REG(shell, (ULONG)GetSP(pContext), g_RegNames[iReg], wszTemp, m_nBase);
            break;
        case REGISTER_FRAME_POINTER:
            WRITE_REG(shell, (ULONG)GetFP(pContext), g_RegNames[iReg], wszTemp, m_nBase);
            break;

        default:
            {
                return false;
            }
    }
    return true;
}
