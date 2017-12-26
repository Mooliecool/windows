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
// File: SymBinder.h
// 
// ===========================================================================

#ifndef SYMBINDER_H_
#define SYMBINDER_H_

/* ------------------------------------------------------------------------- *
 * SymBinder class
 * ------------------------------------------------------------------------- */

class SymBinder : ISymUnmanagedBinder
{
// ctor/dtor
public:
    SymBinder()
    {   
    m_refCount = 0;
    }
    static HRESULT NewSymBinder( REFCLSID clsid, void** ppObj );

// IUnknown methods
public:

    //-----------------------------------------------------------
    // IUnknown support
    //-----------------------------------------------------------
    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (InterlockedIncrement((LONG *) &m_refCount));
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        long refCount = InterlockedDecrement((LONG *) &m_refCount);
        if (refCount == 0)
            DELETE(this);

        return (refCount);
    }
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);

// ISymUnmanagedBinder
public:

    STDMETHOD(GetReaderForFile)( IUnknown *importer,
                                 const WCHAR *fileName,
                                 const WCHAR *searchPath,
                                 ISymUnmanagedReader **pRetVal);
    STDMETHOD(GetReaderFromStream)(IUnknown *importer,
                    IStream *pstream,
                    ISymUnmanagedReader **pRetVal);

private:
    SIZE_T      m_refCount;

};
#endif  
