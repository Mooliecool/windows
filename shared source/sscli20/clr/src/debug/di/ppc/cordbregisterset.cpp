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

    (*pAvailable) = SETBITULONG64( REGISTER_INSTRUCTION_POINTER );
    return S_OK;
}


HRESULT CordbRegisterSet::GetRegisters(ULONG64 mask, ULONG32 regCount,
                                       CORDB_REGISTER regBuffer[])
{ 
    FAIL_IF_NEUTERED(this);
    UINT iRegister = 0;

    VALIDATE_POINTER_TO_OBJECT_ARRAY(regBuffer, CORDB_REGISTER, regCount, true, true);
    
    if( mask &  SETBITULONG64(REGISTER_INSTRUCTION_POINTER) )
    {
        regBuffer[iRegister++] = m_rd->PC;
    }

    _ASSERTE( iRegister <= regCount );
    return S_OK;
}


HRESULT CordbRegisterSet::GetRegistersAvailable(ULONG32 regCount, 
                                                BYTE    pAvailable[])
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(pAvailable, CORDB_REGISTER, regCount, true, true);
    
    return E_NOTIMPL;
}


HRESULT CordbRegisterSet::GetRegisters(ULONG32 maskCount, BYTE mask[], 
                                       ULONG32 regCount, CORDB_REGISTER regBuffer[])
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(regBuffer, CORDB_REGISTER, regCount, true, true);

    return E_NOTIMPL;
}


// This is just a convenience function to convert a regdisplay into a Context.
// Since a context has more info than a regdisplay, the conversion isn't perfect
// and the context can't be fully accurate.
void CordbRegisterSet::InternalCopyRDToContext(CONTEXT *pInputContext)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess());
    _ASSERTE(pInputContext);

    if((pInputContext->ContextFlags & CONTEXT_INTEGER)==CONTEXT_INTEGER)
    {
        pInputContext->Gpr1 = m_rd->SP;
        pInputContext->Gpr30 = m_rd->FP;
    }


    if((pInputContext->ContextFlags & CONTEXT_CONTROL)==CONTEXT_CONTROL)
    {
        pInputContext->Iar = m_rd->PC;
    }
}
