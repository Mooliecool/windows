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
//*****************************************************************************
// File: CordbRegisterSet.cpp
//
//*****************************************************************************
#include "stdafx.h"
#include "primitives.h"


HRESULT CordbRegisterSet::GetRegistersAvailable(ULONG64 *pAvailable)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pAvailable, ULONG64 *);

    (*pAvailable) = SETBITULONG64( REGISTER_INSTRUCTION_POINTER )
            |   SETBITULONG64( REGISTER_STACK_POINTER )
            |   SETBITULONG64( REGISTER_FRAME_POINTER );

    if (!m_quickUnwind || m_active)
        (*pAvailable) |= SETBITULONG64( REGISTER_X86_EAX )
            |   SETBITULONG64( REGISTER_X86_ECX )
            |   SETBITULONG64( REGISTER_X86_EDX )
            |   SETBITULONG64( REGISTER_X86_EBX )
            |   SETBITULONG64( REGISTER_X86_ESI )
            |   SETBITULONG64( REGISTER_X86_EDI );

    if (m_active)
        (*pAvailable) |= SETBITULONG64( REGISTER_X86_FPSTACK_0 )
            |   SETBITULONG64( REGISTER_X86_FPSTACK_1 )
            |   SETBITULONG64( REGISTER_X86_FPSTACK_2 )
            |   SETBITULONG64( REGISTER_X86_FPSTACK_3 )
            |   SETBITULONG64( REGISTER_X86_FPSTACK_4 )
            |   SETBITULONG64( REGISTER_X86_FPSTACK_5 )
            |   SETBITULONG64( REGISTER_X86_FPSTACK_6 )
            |   SETBITULONG64( REGISTER_X86_FPSTACK_7 );

    return S_OK;
}


#define FPSTACK_FROM_INDEX( _index )  (m_thread->m_floatValues[m_thread->m_floatStackTop -( (REGISTER_X86_FPSTACK_##_index)-REGISTER_X86_FPSTACK_0 ) ] )

HRESULT CordbRegisterSet::GetRegisters(ULONG64 mask, ULONG32 regCount,
                                       CORDB_REGISTER regBuffer[])
{ 
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    UINT iRegister = 0;

    VALIDATE_POINTER_TO_OBJECT_ARRAY(regBuffer, CORDB_REGISTER, regCount, true, true);
    

    // Make sure that the registers are really available
    if ( mask & (       SETBITULONG64( REGISTER_X86_EAX )
                    |   SETBITULONG64( REGISTER_X86_ECX )
                    |   SETBITULONG64( REGISTER_X86_EDX )
                    |   SETBITULONG64( REGISTER_X86_EBX )
                    |   SETBITULONG64( REGISTER_X86_ESI )
                    |   SETBITULONG64( REGISTER_X86_EDI ) ) )
    {
        if (!m_active && m_quickUnwind)
            return E_INVALIDARG;
    }

    for ( int i = REGISTER_INSTRUCTION_POINTER
        ; i<=REGISTER_X86_FPSTACK_7 && iRegister < regCount 
        ; i++)
    {
        if( mask &  SETBITULONG64(i) )
        {
            switch( i )
            {
            case REGISTER_INSTRUCTION_POINTER: 
                regBuffer[iRegister++] = m_rd->PC; break;
            case REGISTER_STACK_POINTER:
                regBuffer[iRegister++] = m_rd->SP; break;
            case REGISTER_FRAME_POINTER:
                regBuffer[iRegister++] = m_rd->FP; break;
            case REGISTER_X86_EAX:
                regBuffer[iRegister++] = m_rd->Eax; break;
            case REGISTER_X86_EBX:
                regBuffer[iRegister++] = m_rd->Ebx; break;
            case REGISTER_X86_ECX:
                regBuffer[iRegister++] = m_rd->Ecx; break;
            case REGISTER_X86_EDX:
                regBuffer[iRegister++] = m_rd->Edx; break;
            case REGISTER_X86_ESI:
                regBuffer[iRegister++] = m_rd->Esi; break;
            case REGISTER_X86_EDI:
                regBuffer[iRegister++] = m_rd->Edi; break;

            case    REGISTER_X86_FPSTACK_0:            
            case    REGISTER_X86_FPSTACK_1:            
            case    REGISTER_X86_FPSTACK_2:            
            case    REGISTER_X86_FPSTACK_3:            
            case    REGISTER_X86_FPSTACK_4:            
            case    REGISTER_X86_FPSTACK_5:            
            case    REGISTER_X86_FPSTACK_6:            
            case    REGISTER_X86_FPSTACK_7:
                return E_INVALIDARG;
            }
        }
    }

    _ASSERTE( iRegister <= regCount );
    return S_OK;
}


HRESULT CordbRegisterSet::GetRegistersAvailable(ULONG32 regCount, 
                                                BYTE    pAvailable[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(pAvailable, CORDB_REGISTER, regCount, true, true);

    // Defer to adapter for v1.0 interface
    return GetRegistersAvailableAdapter(regCount, pAvailable);
}


HRESULT CordbRegisterSet::GetRegisters(ULONG32 maskCount, BYTE mask[], 
                                       ULONG32 regCount, CORDB_REGISTER regBuffer[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(regBuffer, CORDB_REGISTER, regCount, true, true);

    // Defer to adapter for v1.0 interface
    return GetRegistersAdapter(maskCount, mask, regCount, regBuffer);
}


// This is just a convenience function to convert a regdisplay into a Context.
// Since a context has more info than a regdisplay, the conversion isn't perfect
// and the context can't be fully accurate.
void CordbRegisterSet::InternalCopyRDToContext(CONTEXT *pInputContext)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess());
    _ASSERTE(pInputContext);

    //now update the registers based on the current frame
    if((pInputContext->ContextFlags & CONTEXT_INTEGER)==CONTEXT_INTEGER)
    {
        pInputContext->Eax = m_rd->Eax;
        pInputContext->Ebx = m_rd->Ebx;
        pInputContext->Ecx = m_rd->Ecx;
        pInputContext->Edx = m_rd->Edx;
        pInputContext->Esi = m_rd->Esi;
        pInputContext->Edi = m_rd->Edi;
    }


    if((pInputContext->ContextFlags & CONTEXT_CONTROL)==CONTEXT_CONTROL)
    {
        pInputContext->Eip = m_rd->PC;
        pInputContext->Esp = m_rd->SP;
        pInputContext->Ebp = m_rd->FP;
    }
}

