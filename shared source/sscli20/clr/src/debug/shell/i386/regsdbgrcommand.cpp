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
            WriteReg(i, 
                     pContext, 
                     pShell);

            pShell->Write(L" ");
            nRegsWritten++;
        }

        for (int i = REGISTER_FRAME_POINTER+1; i <= REGISTER_X86_EFLAGS_OV; i++)
        {
            WriteReg(i, 
                     pContext, 
                     pShell);

            if (((nRegsWritten++ % 5) == 0) ||
                (i == REGISTER_X86_FPSTACK_7) ||
                (i == REGISTER_X86_EDI))
            {
                nRegsWritten = 1;
                pShell->Write(L"\n");
            }
            else
                pShell->Write(L" ");
        }
    }
    else
    {
        if (!WriteReg(LookupRegisterIndexByName(wszReg),
                      pContext,
                      pShell))
            pShell->Write(L"Register %s unknown or unprintable\n", wszReg);
    }

    pShell->Write(L"\n");

    WCHAR sz[20];


    if (fPrintAll && (pContext->ContextFlags & CONTEXT_FLOATING_POINT))
    {
        pShell->Write(L"ControlWord = %08s ", 
                     _itow(pContext->FloatSave.ControlWord, sz, m_nBase));
        pShell->Write(L"StatusWord = %08s ", 
                     _itow(pContext->FloatSave.StatusWord, sz, m_nBase));
        pShell->Write(L"TagWord = %08s\n",
                     _itow(pContext->FloatSave.TagWord, sz, m_nBase));
        pShell->Write(L"ErrorOffset = %08s ",
                     _itow(pContext->FloatSave.ErrorOffset, sz, m_nBase));
        pShell->Write(L"ErrorSelector = %08s ", 
                     _itow(pContext->FloatSave.ErrorSelector, sz, m_nBase));
        pShell->Write(L"DataOffset = %08s\n",
                     _itow(pContext->FloatSave.DataOffset, sz, m_nBase));
        pShell->Write(L"DataSelector = %08s ", 
                     _itow(pContext->FloatSave.DataSelector, sz, m_nBase));
        pShell->Write(L"Cr0NpxState = %08s\n", 
                     _itow(pContext->FloatSave.Cr0NpxState, sz, m_nBase));
    }
}


#undef WRITE_SPECIAL_BIT_REGISTER
#undef WRITE_SPECIAL_REGISTER

#define WRITE_SPECIAL_REGISTER(shell, pContext, segmentflag, Name, fieldName, nBase, sz) \
        if ((pContext)->ContextFlags & (segmentflag))            \
            (shell)->Write( L###Name L" = %04s",                 \
            _itow((pContext)->fieldName, sz, (nBase)));          \
        else                                                     \
            shell->Write(L###Name L"=<?>");                      

#define WRITE_SPECIAL_BIT_REGISTER( shell, pContext, segmentFlag, fName, Name ) \
            if ( (pContext)->ContextFlags & (segmentFlag))          \
            {                                                       \
                if ( (pContext)->EFlags & (X86_EFLAGS_##fName) )    \
                    shell->Write( L###Name L" = 1"  );              \
                else                                                \
                    shell->Write( L###Name L" = 0"  );              \
            }                                                       \
            else                                                    \
            {                                                       \
                shell->Write( L###Name L"=<?>" );                   \
            }                                                       \

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
        case REGISTER_X86_EAX:
            WRITE_REG(shell, pContext->Eax, g_RegNames[iReg], wszTemp, m_nBase);
            break;
        case REGISTER_X86_EBX:
            WRITE_REG(shell, pContext->Ebx, g_RegNames[iReg], wszTemp, m_nBase);
            break;
        case REGISTER_X86_ECX:
            WRITE_REG(shell, pContext->Ecx, g_RegNames[iReg], wszTemp, m_nBase);
            break;
        case REGISTER_X86_EDX:
            WRITE_REG(shell, pContext->Edx, g_RegNames[iReg], wszTemp, m_nBase);
            break;
        case REGISTER_X86_ESI:
            WRITE_REG(shell, pContext->Esi, g_RegNames[iReg], wszTemp, m_nBase);
            break;
        case REGISTER_X86_EDI:
            WRITE_REG(shell, pContext->Edi, g_RegNames[iReg], wszTemp, m_nBase);
            break;

        case REGISTER_X86_FPSTACK_0:
        case REGISTER_X86_FPSTACK_1:
        case REGISTER_X86_FPSTACK_2:
        case REGISTER_X86_FPSTACK_3:
        case REGISTER_X86_FPSTACK_4:
        case REGISTER_X86_FPSTACK_5:
        case REGISTER_X86_FPSTACK_6:
        case REGISTER_X86_FPSTACK_7:
            {
                shell->Write(L"%s = n/a     ", g_RegNames[iReg]);
                break;
            }

        case REGISTER_X86_EFL:
            {
                WRITE_SPECIAL_REGISTER( shell, pContext, CONTEXT_SEGMENTS, EFL, EFlags, m_nBase, wszTemp )
                break;
            }
        case REGISTER_X86_CS:
            {
                WRITE_SPECIAL_REGISTER( shell, pContext, CONTEXT_SEGMENTS, CS, SegCs, m_nBase, wszTemp )
                break;
            }

        case REGISTER_X86_EFLAGS_CY:
            {
                WRITE_SPECIAL_BIT_REGISTER( shell, pContext,  CONTEXT_CONTROL, CY, CY )
                break;
            }
        case REGISTER_X86_EFLAGS_PE:
            {
                WRITE_SPECIAL_BIT_REGISTER( shell, pContext,  CONTEXT_CONTROL, PE, PE )
                break;
            }
        case REGISTER_X86_EFLAGS_AC:
            {
                WRITE_SPECIAL_BIT_REGISTER( shell, pContext,  CONTEXT_CONTROL, AC, AC )
                break;
            }
        case REGISTER_X86_EFLAGS_ZR:
            {
                WRITE_SPECIAL_BIT_REGISTER( shell, pContext,  CONTEXT_CONTROL, ZR, ZR )
                break;
            }
        case REGISTER_X86_EFLAGS_PL:
            {
                WRITE_SPECIAL_BIT_REGISTER( shell, pContext,  CONTEXT_CONTROL, PL, PL)
                break;
            }
        case REGISTER_X86_EFLAGS_EI:
            {
                WRITE_SPECIAL_BIT_REGISTER( shell, pContext,  CONTEXT_CONTROL, EI, EI )
                break;
            }
        case REGISTER_X86_EFLAGS_UP:
            {
                WRITE_SPECIAL_BIT_REGISTER( shell, pContext,  CONTEXT_CONTROL, UP, UP )
                break;
            }
        case REGISTER_X86_EFLAGS_OV:
            {
                WRITE_SPECIAL_BIT_REGISTER( shell, pContext,  CONTEXT_CONTROL, OV, OV )
                break;
            }
        default:
            {
                return false;
            }
    }
    return true;
}
