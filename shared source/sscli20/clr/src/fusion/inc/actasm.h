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
#ifndef __ACTASM_H_
#define __ACTASM_H_

#include "list.h"
#include "helpers.h"
#include "dbglog.h"

#define DEPENDENCY_HASH_TABLE_SIZE                     47

class CNativeImageAssembly;

class CActivatedAssembly {
    public:
        CActivatedAssembly(IAssemblyName *pName, IUnknown *pAsm);
        ~CActivatedAssembly();

    public:
        IAssemblyName       *_pName;
        IUnknown            *_pAsm;
        CNativeImageAssembly           *_pAsmNI;
        BOOL                 _bNativeImageProbed;
};

class CLoadContext {
    public:
        CLoadContext(LOADCTX_TYPE ctxType);
        ~CLoadContext();

        static HRESULT Create(CLoadContext **ppLoadContext, LOADCTX_TYPE ctxType);

        ULONG AddRef();
        ULONG Release();

        LOADCTX_TYPE GetContextType();

        // Other methods

        HRESULT CheckActivated(IAssemblyName *pName, IUnknown **ppAsm, CDebugLog *pdbglog);
        HRESULT AddActivation(IUnknown *pAsm, IUnknown **ppAsmActivated);
        HRESULT RemoveActivation(IUnknown *pAsm);

        STDMETHODIMP Lock();
        STDMETHODIMP Unlock();

        // Native image related.
        HRESULT SetNativeImage(
                IAssembly *pAsm, 
                CNativeImageAssembly *pAsmNI,
                CNativeImageAssembly **ppAsmNIActivation);
        HRESULT GetNativeImage(
                IAssembly *pAsm, 
                CNativeImageAssembly **ppAsmNI, 
                LPBOOL pbNativeImageProbed);

    private:
        HRESULT Init();

    private:
        LOADCTX_TYPE                          _ctxType;
        CRITSEC_COOKIE                        _cs;
        LONG                                  _cRef;
        List<CActivatedAssembly *>            _hashDependencies[DEPENDENCY_HASH_TABLE_SIZE];
};

#endif  // __ACTASM_H_
