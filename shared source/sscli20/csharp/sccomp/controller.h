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
// File: controller.h
//
// ===========================================================================

#ifndef __controller_h__
#define __controller_h__

////////////////////////////////////////////////////////////////////////////////
// CController
//
// This is the "controller" for compiler objects. The controller is the object
// that exposes/implements ICSCompiler for external consumption.  Compiler
// options are configured through this object, and for an actual compilation,
// this object instanciates a COMPILER, feeds it the appropriate information,
// tells it to compile, and then destroys it.

class CController :
    public CComObjectRootMT,
    public ICSCompiler,
    public ALLOCHOST
{
private:
    NAMEMGR                     *m_pNameMgr;            // NOTE:  Referenced pointer!
    PAGEHEAP                    *m_pPageHeap;           // Page heap for this controller. All controllers and the name manager share a single page heap (most efficient that way).
    MEMHEAP                     m_StdHeap;              // Standard heap for this controller (NOT used by COMPILER, it has its own)
    CComPtr<ICSCompilerHost>    m_spHost;               // Our host
    CTinyLock                   m_lockOptions;          // Protect the options
    COptionData                 m_OptionData;           // Our options
    CInputSet                   *m_pInputSets;          // List of input sets
    CInputSet                   **m_ppNextInputSet;     // Next slot in input set list
    CErrorContainer             *m_pCompilerErrors;     // Container for reporting errors
    DWORD                       m_dwFlags;              // Creation flags
    void                        *m_pExceptionAddr;      // Address of exception
    long                        m_iErrorsReported;      // Number of non-warning errors reported to host
    long                        m_iWarnAsErrorsReported; // Number of warnings that have been reported as errors due to /warnaserror+ 
    bool m_fSuppressErrors;

public:
    BEGIN_COM_MAP(CController)
        COM_INTERFACE_ENTRY(ICSCompiler)
    END_COM_MAP()

    CController ();
    ~CController ();

    HRESULT     Initialize (DWORD dwFlags, ICSCompilerHost *pHost, ICSNameTable *pNameTable);

    ICSCompilerHost *GetHost () { return m_spHost; }
    NAMEMGR         *GetNameMgr() { return m_pNameMgr; }
    void            RemoveSetFromList (CInputSet *pSet);
    long            ErrorsReported () { return m_iErrorsReported; }
    long            WarnAsErrorsReported() { return m_iWarnAsErrorsReported; }
    void            SetConfiguration (COptionData *pData);
    BOOL            CheckFlags (DWORD dwFlags) { return m_dwFlags & dwFlags; }
    BOOL            CheckDisplayWarning(long iErrorIndex, int warnLevel);
    void CloneOptions(COptionData & options);
    void CloneCoreOptions(CCoreOptionData & options);

    // The following methods comprise the error handling/COMPILER-to-CController hosting
    HRESULT         CreateError(long iErrorIndex, VarArgList args, CError **ppError, bool warnOverride = false);
    void            SubmitError (CError *pError);
    void            ReportErrorsToHost (ICSErrorContainer *pErrors);

    void            HandleException (DWORD dwException);
    void            SetExceptionData (EXCEPTION_POINTERS *pExceptionInfo) { m_pExceptionAddr = pExceptionInfo->ExceptionRecord->ExceptionAddress; }
    void            *GetExceptionAddress () { return m_pExceptionAddr; }
    WatsonOperationKindEnum GetWatsonFlags() {
        return m_OptionData.m_howToReportWatsons;
    }

    void SuppressErrors(bool fSuppress) { m_fSuppressErrors = fSuppress; }
    bool FErrorsSuppressed() { return m_fSuppressErrors; }

    // ICSCompiler
    STDMETHOD(CreateSourceModule)(ICSSourceText *pText, ICSSourceModule **ppModule);
    STDMETHOD(GetNameTable)(ICSNameTable **ppNameTable);
    STDMETHOD(Shutdown)();
    STDMETHOD(GetConfiguration)(ICSCompilerConfig **ppConfig);
    STDMETHOD(AddInputSet)(ICSInputSet **ppInputSet);
    STDMETHOD(RemoveInputSet)(ICSInputSet *pInputSet);
    STDMETHOD(Compile)(ICSCompileProgress *pProgress);
    STDMETHOD(BuildForEnc)(ICSCompileProgress *pProgress, ICSEncProjectServices *pEncService, IUnknown * punkPE);
    STDMETHOD(GetOutputFileName)(PCWSTR *ppszFileName);
    STDMETHOD(CreateParser)(ICSParser **ppParser);

    // ICSCompiler Helpers
    HRESULT RunCompiler(ICSCompileProgress *pProgress, COMPILER **ppCompiler, ICompileCallback* pBindCallback = NULL);
    HRESULT Cleanup();


    // ALLOCHOST
    __declspec(noreturn) void NoMemory ();
    MEMHEAP     *GetStandardHeap ();
    PAGEHEAP    *GetPageHeap ();
};

#endif //__controller_h__

