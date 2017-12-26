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
/*============================================================
**
** Header:  AssemblySink.hpp
**
** Purpose: Asynchronous call back for loading classes
**
** Date:  June 23, 1999
**
===========================================================*/
#ifndef _ASSEMBLYSINK_H
#define _ASSEMBLYSINK_H

class AppDomain;

class AssemblySink : public FusionSink
{
public:
    AssemblySink(AppDomain* pDomain);
    ~AssemblySink() { LEAF_CONTRACT; };

    void Reset();

    ULONG STDMETHODCALLTYPE Release(void);

    STDMETHODIMP OnProgress(DWORD dwNotification,
                            HRESULT hrNotification,
                            LPCWSTR szNotification,
                            DWORD dwProgress,
                            DWORD dwProgressMax,
                            LPVOID pvBindInfo,
                            IUnknown* punk);

    virtual HRESULT Wait();

    void RequireCodebaseSecurityCheck() {LEAF_CONTRACT;  m_CheckCodebase = TRUE;}
    BOOL DoCodebaseSecurityCheck() {LEAF_CONTRACT;  return m_CheckCodebase;}
    void SetAssemblySpec(AssemblySpec* pSpec) 
    {
        LEAF_CONTRACT; 
        m_pSpec=pSpec;
    }

private:
    ADID m_Domain; // Which domain (index) do I belong to
    AssemblySpec* m_pSpec;
    BOOL m_CheckCodebase;
};

#endif
