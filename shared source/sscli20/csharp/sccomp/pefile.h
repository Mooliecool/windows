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
// ===========================================================================
// File: pefile.h
//
// Defines the structures used to create executable files.
// ===========================================================================

#ifndef __pefile_h__
#define __pefile_h__

#include <iceefilegen.h>

class ASSEMBLYEMITTER; // So these guys can get at these fields
class EMITTER;

class PEFile : public IMapToken {
public:
    STDMETHOD_(ULONG, AddRef) () { return 1; }
    STDMETHOD_(ULONG, Release) () { return 1; }
    STDMETHOD(QueryInterface) (REFIID riid, LPVOID* obp) {
        if (obp == NULL)
            return E_POINTER;
        if (riid == IID_IMapToken) {
            *obp = (IMapToken*)this;
        } else if (riid != IID_IUnknown) {
            *obp = (IUnknown*)this;
        } else {
            return E_NOINTERFACE;
        }
        return S_OK;
    }
    STDMETHOD(Map)(mdToken tkOld, mdToken tkNew);

    PEFile();
    virtual ~PEFile();
    virtual void Term();

    virtual bool BeginOutputFile(COMPILER * cmp, OUTFILESYM * pOutFile);
    virtual void EndOutputFile(bool bWriteFile);
    virtual void SetAttributes(bool fDll);
    virtual void *AllocateRVABlock(ULONG cb, ULONG alignment, ULONG *codeRVA) {
        return AllocateRVABlockCore(cb, alignment, codeRVA, NULL);
    }
    virtual void WriteCryptoKey();
    virtual bool CalcResource(PRESFILESYM pRes);
    virtual bool AllocResourceBlob();
    virtual bool AddResource(PRESFILESYM pRes);

    static bool ReplaceFileExtension(__inout_ecount(cchFilename) PWSTR lpszFilename, size_t cchFilename, PCWSTR lpszExt); // Note that lpszExt must contain the dot
    static NAME * GetModuleName(POUTFILESYM pOutFile, COMPILER * compiler); // Gets the module name, do not free!

    // accessors
    IMetaDataEmit2         * GetEmit()               { return metaemit; }
    IMetaDataAssemblyEmit  * GetAssemblyEmitter()    { return assememit; }
    OUTFILESYM             * GetOutFile()            { return outfile; }
    ISymUnmanagedWriter    * GetDebugEmit()          { return debugemit; }

    static bool GetPDBFileName(OUTFILESYM * pOutFile, __out_ecount(cchFilename) PWSTR filename, size_t cchFilename);

protected:
    CComPtr<IMetaDataEmit2> metaemit;
    CComPtr<ISymUnmanagedWriter> debugemit;
    CComPtr<IMetaDataAssemblyEmit> assememit;

    ICeeFileGen * ceefilegen;
    OUTFILESYM * outfile;
    COMPILER * compiler;

    HCEEFILE                ceeFile;
    HCEESECTION             ilSection, rdataSection;

    DWORD                   m_dwBaseResRva;
    DWORD                   m_dwResSize;
    void                    *m_pResBuffer;
    DWORD                   m_dwOffset;

    void CheckHR(HRESULT hr);
    void CheckHR(int errid, HRESULT hr);
    void MetadataFailure(HRESULT hr);
    void MetadataFailure(int errid, HRESULT hr);

    ULONG GetRVAOfOffset(ULONG offset);
    void *AllocateRVABlockCore(ULONG cb, ULONG alignment, ULONG *codeRVA, ULONG *offset);

    static bool GetOutputPath(OUTFILESYM * pOutFile, DWORD cchDirName, __out_ecount(cchDirName) PWSTR wszDirName);
    static HRESULT CreateSymbolWriter(ISymUnmanagedWriter ** pSymwriter);
    static HRESULT CreateSymbolReader(ISymUnmanagedReader ** pSymreader);
};


#endif // __pefile_h__
