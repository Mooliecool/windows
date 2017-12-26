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
#include "common.h" 
#include "reflectclasswriter.h"

// Forward declaration.
STDAPI  GetMetaDataInternalInterfaceFromPublic(
    IUnknown    *pv,                    // [IN] Given interface.
    REFIID      riid,                   // [IN] desired interface
    void        **ppv);                 // [OUT] returned interface

//******************************************************
//*
//* constructor for RefClassWriter
//*
//******************************************************
HRESULT RefClassWriter::Init(ICeeGen *pCeeGen, IUnknown *pUnk)
{
    CONTRACT(HRESULT) {
        NOTHROW;
        GC_NOTRIGGER;
        // we know that the com implementation is ours so we use mode-any to simplify
        // having to switch mode 
        MODE_ANY; 
        INJECT_FAULT(CONTRACT_RETURN(E_OUTOFMEMORY));
        
        PRECONDITION(CheckPointer(pCeeGen));
        PRECONDITION(CheckPointer(pUnk));

        POSTCONDITION(SUCCEEDED(RETVAL) ? CheckPointer(m_emitter) : TRUE);
        POSTCONDITION(SUCCEEDED(RETVAL) ? CheckPointer(m_importer) : TRUE);
        POSTCONDITION(SUCCEEDED(RETVAL) ? CheckPointer(m_pEmitHelper) : TRUE);
        POSTCONDITION(SUCCEEDED(RETVAL) ? CheckPointer(m_internalimport) : TRUE);
    }
    CONTRACT_END;

    // Initialize the Import and Emitter interfaces
    m_emitter = NULL;
    m_importer = NULL;
    m_internalimport = NULL;
    m_pCeeFileGen = NULL;
    m_ceeFile = NULL;
    m_ulResourceSize = 0;
    m_tkFile = mdFileNil;

    m_pCeeGen = pCeeGen;
    pCeeGen->AddRef();

    // Get the interfaces
    HRESULT hr = pUnk->QueryInterface(IID_IMetaDataEmit2, (void**)&m_emitter);
    if (FAILED(hr))
        RETURN(hr);

    hr = pUnk->QueryInterface(IID_IMetaDataImport, (void**)&m_importer);
    if (FAILED(hr))
        RETURN(hr);

    hr = pUnk->QueryInterface(IID_IMetaDataEmitHelper, (void**)&m_pEmitHelper);
    if (FAILED(hr))
        RETURN(hr);

    hr = GetMetaDataInternalInterfaceFromPublic(pUnk, IID_IMDInternalImport, (void**)&m_internalimport);
    if (FAILED(hr))
        RETURN(hr);

    hr = m_emitter->SetModuleProps(L"Default Dynamic Module");
    if (FAILED(hr))
        RETURN(hr);



    RETURN(S_OK);
}


//******************************************************
//*
//* destructor for RefClassWriter
//*
//******************************************************
RefClassWriter::~RefClassWriter()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        // we know that the com implementation is ours so we use mode-any to simplify
        // having to switch mode 
        MODE_ANY; 
        FORBID_FAULT;
    }
    CONTRACTL_END;

    if (m_emitter) {
        m_emitter->Release();
    }

    if (m_importer) {
        m_importer->Release();
    }

    if (m_pEmitHelper) {
        m_pEmitHelper->Release();
    }

    if (m_internalimport) {
        m_internalimport->Release();
    }

    if (m_pCeeGen) {
        m_pCeeGen->Release();
        m_pCeeGen = NULL;
    }

    if (m_pOnDiskEmitter) {
        m_pOnDiskEmitter->Release();
        m_pOnDiskEmitter = NULL;
    }

    DestroyCeeFileGen();
}

//******************************************************
//*
//* Make sure that CeeFileGen for this module is created for emitting to disk
//*
//******************************************************
HRESULT RefClassWriter::EnsureCeeFileGenCreated(DWORD corhFlags, DWORD peFlags)
{
    CONTRACT(HRESULT) {
        NOTHROW;
        GC_TRIGGERS;
        // we know that the com implementation is ours so we use mode-any to simplify
        // having to switch mode 
        MODE_ANY; 
        INJECT_FAULT(CONTRACT_RETURN(E_OUTOFMEMORY));
        
        POSTCONDITION(SUCCEEDED(RETVAL) ? CheckPointer(m_pCeeFileGen) : (int)(m_pCeeFileGen == NULL));
        POSTCONDITION(SUCCEEDED(RETVAL) ? CheckPointer(m_ceeFile) : (int)(m_pCeeFileGen == NULL));
    }
    CONTRACT_END;

    HRESULT     hr = NOERROR;

    if (m_pCeeFileGen == NULL)
    {
        EX_TRY
        {
            hr = CreateICeeFileGen(&m_pCeeFileGen);
            if (SUCCEEDED(hr))
            {
                hr = m_pCeeFileGen->CreateCeeFileFromICeeGen(m_pCeeGen, &m_ceeFile, peFlags);
            }
            if (SUCCEEDED(hr))
            {
                hr = m_pCeeFileGen->ClearComImageFlags(m_ceeFile, COMIMAGE_FLAGS_ILONLY);
            }
            if (SUCCEEDED(hr))
            {
                hr = m_pCeeFileGen->SetComImageFlags(m_ceeFile, corhFlags);
            }
        }
        EX_CATCH
        {
            hr = GET_EXCEPTION()->GetHR();
        }
        EX_END_CATCH(SwallowAllExceptions);
        
        if (FAILED(hr))
        {
            DestroyCeeFileGen();
        }
    }    

    RETURN(hr);
}


//******************************************************
//*
//* Destroy the instance of CeeFileGen that we created
//*
//******************************************************
HRESULT RefClassWriter::DestroyCeeFileGen()
{
    CONTRACT(HRESULT) {
        NOTHROW;
        GC_TRIGGERS;
        // we know that the com implementation is ours so we use mode-any to simplify
        // having to switch mode 
        MODE_ANY; 
        FORBID_FAULT;
        
        POSTCONDITION(m_pCeeFileGen == NULL);
        POSTCONDITION(m_ceeFile == NULL);
    }
    CONTRACT_END;

    HRESULT     hr = NOERROR;

    if (m_pCeeFileGen) 
    {
        //Cleanup the HCEEFILE.  
        if (m_ceeFile) 
        {
            hr= m_pCeeFileGen->DestroyCeeFile(&m_ceeFile);
            _ASSERTE( SUCCEEDED(hr) || "Destory CeeFile" );
            m_ceeFile = NULL;
        }

        //Cleanup the ICeeFileGen.
        {
            // We intentionally use GetModuleHandleA here.  WszGetModuleHandle may
            // allocate memory.
            // We have created ICeeFileGen object which means that mscorpe.dll has been loaded.
            // DestroyICeeFileGen will not fail for delay load.
            CONTRACT_VIOLATION(ThrowsViolation);
            hr = DestroyICeeFileGen(&m_pCeeFileGen);
        }
        _ASSERTE( SUCCEEDED(hr) || "Destroy ICeeFileGen" );
        m_pCeeFileGen = NULL;
    }

    RETURN(hr);
}
