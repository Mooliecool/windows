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
// File: RSRegSetCommon.cpp
// Common cross-platform behavior of reg sets.
// Platform specific stuff is in CordbRegisterSet.cpp located in
// the platform sub-dir.
//
//*****************************************************************************
#include "stdafx.h"
#include "primitives.h"

/* ------------------------------------------------------------------------- *
 * Common (cross-platform) Register-Set stuff
 * ------------------------------------------------------------------------- */

CordbRegisterSet::CordbRegisterSet( 
    DebuggerREGDISPLAY *rd, 
    CordbThread *thread,
    bool active, 
    bool quickUnwind
) : CordbBase(thread->GetProcess(), 0, enumCordbRegisterSet)
{
    _ASSERTE( rd != NULL );
    _ASSERTE( thread != NULL );
    m_rd = rd;
    m_thread = thread;
    m_active = active;
    m_quickUnwind = quickUnwind;

    // Add to our parent thread's neuter list.
    thread->GetRefreshStackNeuterList()->Add(this);
}

void CordbRegisterSet::Neuter(NeuterTicket ticket)
{
    m_thread = NULL;
    m_rd = NULL;
    
    CordbBase::Neuter(ticket);
}

CordbRegisterSet::~CordbRegisterSet()
{
    _ASSERTE(this->IsNeutered());
}


HRESULT CordbRegisterSet::QueryInterface(REFIID riid, void **ppInterface)
{
    if (riid == IID_ICorDebugRegisterSet)
    {
        *ppInterface = static_cast<ICorDebugRegisterSet*>(this);
    }
    else if (riid == IID_ICorDebugRegisterSet2)
    {
        *ppInterface = static_cast<ICorDebugRegisterSet2*>(this);
    }
    else if (riid == IID_IUnknown)
    {
        *ppInterface = static_cast<IUnknown*>(static_cast<ICorDebugRegisterSet*>(this));
    }
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//-----------------------------------------------------------------------------
// This is just a convenience function to convert a regdisplay into a Context.
// Since a context has more info than a regdisplay, the conversion isn't perfect
// and the context can't be fully accurate.
//
// Inputs:
//    contextSize - sizeof incoming context buffer  in bytes
//    context - buffer to copy this regdisplay's OS CONTEXT structure into.
//
// Returns S_OK on success. 
//-----------------------------------------------------------------------------
HRESULT CordbRegisterSet::GetThreadContext(ULONG32 contextSize, BYTE context[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    _ASSERTE( m_thread != NULL );
    if( contextSize < sizeof( CONTEXT ))
    {
        return E_INVALIDARG;
    }        

    VALIDATE_POINTER_TO_OBJECT_ARRAY(context, BYTE, contextSize, true, true);

    CONTEXT *pInputContext = reinterpret_cast<CONTEXT *> (context);
    
    // Just to be safe, zero out the buffer we got in while preserving the ContextFlags.  
    // On X64 the ContextFlags field is not the first 4 bytes of the CONTEXT.
    DWORD dwContextFlags = pInputContext->ContextFlags;
    ZeroMemory(context, contextSize);
    pInputContext->ContextFlags = dwContextFlags;

    // Augment the leafmost (active) register w/ information from the current context.
    CONTEXT * pLeafContext = NULL;
    if (m_active)
    {
        m_thread->GetManagedContext( &pLeafContext);

        if (pLeafContext != NULL)
        {
            memmove( pInputContext, pLeafContext, sizeof( CONTEXT) );
        }
    }


    // Now update the registers based on the current frame.
    // This is a very platform specific action.
    InternalCopyRDToContext(pInputContext);
    
    return S_OK;
}

//-----------------------------------------------------------------------------
// Helpers to impl IRegSet2 on top of original IRegSet.
// These are useful on platforms that don't need IRegSet2 (like x86 + amd64). 
// See CorDebug.idl for details.
//
// Inputs:
//   regCount - size of pAvailable buffer in bytes
//   pAvailable - buffer to hold bitvector of available registers. 
//                On success, bit at position CorDebugRegister is 1 iff that
//                register is available.
// Returns S_OK on success.
//-----------------------------------------------------------------------------
HRESULT CordbRegisterSet::GetRegistersAvailableAdapter(
    ULONG32 regCount, 
    BYTE    pAvailable[])
{
    // Defer to call on v1.0 interface
    HRESULT hr = S_OK;

    if (regCount < sizeof(ULONG64))
    {
        return E_INVALIDARG;
    }

    _ASSERTE(pAvailable != NULL);

    ULONG64 availRegs;
    hr = this->GetRegistersAvailable(&availRegs);
    if (FAILED(hr))
    {
        return hr;
    }

    // Nor marshal our 64-bit value into the outgoing byte array.
    for(int iBit = 0; iBit < (int) sizeof(availRegs) * 8; iBit++)
    {
        ULONG64 test = SETBITULONG64(iBit);
        if (availRegs & test)
        {
            SET_BIT_MASK(pAvailable, iBit);
        }
        else
        {
            RESET_BIT_MASK(pAvailable, iBit);
        }
    }
    return S_OK;
}

//-----------------------------------------------------------------------------
// Helpers to impl IRegSet2 on top of original IRegSet.
// These are useful on platforms that don't need IRegSet2 (like x86 + amd64). 
// See CorDebug.idl for details.
//
// Inputs:
//  maskCount - size of mask buffer in bytes.
//  mask - input buffer specifying registers to request
//  regCount - size of regBuffer in bytes
//  regBuffer - output buffer, regBuffer[n] = value of register at n-th active
//              bit in mask.
// Returns S_OK on success.
//-----------------------------------------------------------------------------

// mask input requrest registers, which get written to regCount buffer.
HRESULT CordbRegisterSet::GetRegistersAdapter(
    ULONG32 maskCount, BYTE mask[], 
    ULONG32 regCount, CORDB_REGISTER regBuffer[])
{
    ULONG64 maskOrig = 0;

    for(UINT iBit = 0; iBit < maskCount * 8; iBit++)
    {
        if (IS_SET_BIT_MASK(mask, iBit))
        {
            maskOrig |= SETBITULONG64(iBit);
        }
    }
    
    return this->GetRegisters(maskOrig, 
        regCount, regBuffer);
}
