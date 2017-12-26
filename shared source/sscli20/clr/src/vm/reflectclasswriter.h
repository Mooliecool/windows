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
#ifndef _REFCLASSWRITER_H_
#define _REFCLASSWRITER_H_

#include "iceefilegen.h"

// RefClassWriter
// This will create a Class
class RefClassWriter {
protected:
    friend class COMDynamicWrite;
	IMetaDataEmit2*			m_emitter;			// Emit interface.
	IMetaDataImport*		m_importer;			// Import interface.
	IMDInternalImport*		m_internalimport;	// Scopeless internal import interface
	ICeeGen*				m_pCeeGen;
    ICeeFileGen*            m_pCeeFileGen;
    HCEEFILE                m_ceeFile;
	IMetaDataEmitHelper*	m_pEmitHelper;
	ULONG					m_ulResourceSize;
    mdFile                  m_tkFile;
    IMetaDataEmit*          m_pOnDiskEmitter;

public:
    RefClassWriter() {
        LEAF_CONTRACT;
        m_pOnDiskEmitter = NULL;
    }

	HRESULT		Init(ICeeGen *pCeeGen, IUnknown *pUnk);

	IMetaDataEmit2* GetEmitter() {
        LEAF_CONTRACT;
		return m_emitter;
	}

	IMetaDataEmitHelper* GetEmitHelper() {
        LEAF_CONTRACT;
		return m_pEmitHelper;
	}

	IMetaDataImport* GetRWImporter() {
        LEAF_CONTRACT;
		return m_importer;
	}

	IMDInternalImport* GetMDImport() {
        LEAF_CONTRACT;
		return m_internalimport;
	}

	ICeeGen* GetCeeGen() {
        LEAF_CONTRACT;
		return m_pCeeGen;
	}

	ICeeFileGen* GetCeeFileGen() {
        LEAF_CONTRACT;
		return m_pCeeFileGen;
	}

	HCEEFILE GetHCEEFILE() {
        LEAF_CONTRACT;
		return m_ceeFile;
	}

    IMetaDataEmit* GetOnDiskEmitter() {
        LEAF_CONTRACT;
        return m_pOnDiskEmitter;
    }

    void SetOnDiskEmitter(IMetaDataEmit *pOnDiskEmitter) {
        CONTRACTL {
            NOTHROW;
            GC_TRIGGERS;
            // we know that the com implementation is ours so we use mode-any to simplify
            // having to switch mode 
            MODE_ANY; 
            FORBID_FAULT;
        }
        CONTRACTL_END;
        if (pOnDiskEmitter) 
            pOnDiskEmitter->AddRef();
        if (m_pOnDiskEmitter)
            m_pOnDiskEmitter->Release();
        m_pOnDiskEmitter = pOnDiskEmitter;
    }

    //HRESULT EnsureCeeFileGenCreated(DWORD corhFlags = COMIMAGE_FLAGS_ILONLY, DWORD peFlags = ICEE_CREATE_FILE_PURE_IL);
    HRESULT EnsureCeeFileGenCreated(DWORD corhFlags, DWORD peFlags);
    HRESULT DestroyCeeFileGen();

	~RefClassWriter();
};

#endif	// _REFCLASSWRITER_H_
