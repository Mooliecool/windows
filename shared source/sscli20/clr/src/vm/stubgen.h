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
#ifndef __STUBGEN_H__
#define __STUBGEN_H__

#include "mlgen.h"
#include "stublink.h"


typedef void* ILStubLinker;
typedef void* ILCodeStream;



enum StubType { StubTypeIL, StubTypeML, StubTypeILTearOff };


//
// The purpose of this class is to make it easy to phase out MLStubLinker in favor of ILStubLinker
// It contains forwarders for all the ILStubLinker and MLStubLinker methods that are used in
// marshaler.h and ILMarshalers.h
//
class InteropStubLinker
{
public:
    InteropStubLinker(StubType type) :  // the type parameter isn't strictly needed, but it makes it easier to find usages of ML stubs
        m_type(type),
        m_pslML(NULL),
        m_pslIL(NULL)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;
        
        //
        // only use this constructor for ML stubs
        //
        _ASSERTE(StubTypeML == m_type);
        m_pslML = new MLStubLinker();
    }

    InteropStubLinker(NDirectStubLinker* psl) :
        m_type(StubTypeILTearOff),
        m_pslML(NULL),
        m_pslIL((ILStubLinker*)psl)
    {
    }

    ~InteropStubLinker()
    {
        LEAF_CONTRACT;

        if (StubTypeIL == m_type)
        {
            delete m_pslIL;
        }
        else if (StubTypeML == m_type)
        {
            delete m_pslML;
        }
        else if (StubTypeILTearOff == m_type)
        {
            // don't delete!
        }
    }

    //
    // common functions
    //
    BOOL EmitsIL() 
    { 
        LEAF_CONTRACT;
            
        if (StubTypeIL == m_type ||
            StubTypeILTearOff == m_type)
        {
            return TRUE; 
        }
        else
        {
            _ASSERTE(StubTypeML == m_type);
            return FALSE;
        }
    }

    //
    // StubLinker functions
    //
    VOID EmitBytes(const BYTE *pBytes, UINT numBytes)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(StubTypeML == m_type);
        _ASSERTE(NULL != m_pslML);
        m_pslML->EmitBytes(pBytes, numBytes);
    }
    VOID Emit8 (unsigned __int8  u8)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(StubTypeML == m_type);
        _ASSERTE(NULL != m_pslML);
        m_pslML->Emit8(u8);
    }
    VOID Emit16(unsigned __int16 u16)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(StubTypeML == m_type);
        _ASSERTE(NULL != m_pslML);
        m_pslML->Emit16(u16);
    }
    VOID Emit32(unsigned __int32 u32)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(StubTypeML == m_type);
        _ASSERTE(NULL != m_pslML);
        m_pslML->Emit32(u32);
    }
    VOID Emit64(unsigned __int64 u64)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(StubTypeML == m_type);
        _ASSERTE(NULL != m_pslML);
        m_pslML->Emit64(u64);
    }
    VOID EmitPtr(const VOID *pval)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(StubTypeML == m_type);
        _ASSERTE(NULL != m_pslML);
        m_pslML->EmitPtr(pval);
    }

    //
    // MLStubLinker functions
    //
    Stub* Link()
    {
        CONTRACT (Stub*)
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(StubTypeML == m_type);
            PRECONDITION(CheckPointer(m_pslML));
            POSTCONDITION(CheckPointer(RETVAL));
        }
        CONTRACT_END;

        RETURN m_pslML->Link();
    }
    
    VOID MLEmit(MLCode opcode)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(StubTypeML == m_type);
            PRECONDITION(CheckPointer(m_pslML));
        }
        CONTRACTL_END;

        m_pslML->MLEmit(opcode);
    }
    
    VOID MLEmitSpace(UINT cb)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(StubTypeML == m_type);
            PRECONDITION(CheckPointer(m_pslML));
        }
        CONTRACTL_END;
        
        m_pslML->MLEmitSpace(cb);
    }
    
    UINT16 MLNewLocal(UINT16 numBytes)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(StubTypeML == m_type);
            PRECONDITION(CheckPointer(m_pslML));
        }
        CONTRACTL_END;
        
        return m_pslML->MLNewLocal(numBytes);
    }
    
    UINT16 GetLocalSize()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(StubTypeML == m_type);
            PRECONDITION(CheckPointer(m_pslML));
        }
        CONTRACTL_END;
        
        return m_pslML->GetLocalSize();
    }

    //
    // for ILStubLinker, we don't support the world, 
    // just give back the various ILStubLinker 
    // derivatives
    //
    ILStubLinker* GetILStubLinker()
    {
        LEAF_CONTRACT;
        
        _ASSERTE(StubTypeIL == m_type || StubTypeILTearOff == m_type);
        _ASSERTE(NULL != m_pslIL);
        return m_pslIL;
    }

    NDirectStubLinker* GetNDirectStubLinker()
    {
        CONTRACT (NDirectStubLinker*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(StubTypeIL == m_type || StubTypeILTearOff == m_type);
            PRECONDITION(CheckPointer(m_pslIL));
        }
        CONTRACT_END;

        RETURN ((NDirectStubLinker*)m_pslIL);
    }


private:
    StubType        m_type;     // used only for clarity; will eventually be removed along with this class once we use IL stubs exclusively
    MLStubLinker*   m_pslML;
    ILStubLinker*   m_pslIL;
};


#define TOKEN_ILSTUB_TARGET_SIG (TokenFromRid(0xFFFFFF, mdtSignature))

void* JitILStub(MethodDesc* pStubMD);

#endif  // __STUBGEN_H__
