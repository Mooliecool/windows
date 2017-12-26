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

#ifndef __ASSEMBLYHASH_H__
#define __ASSEMBLYHASH_H__

#include "wincrypt.h"
#include "ex.h"



class AssemblyFileHash
{
public:    
    enum HashContentType
    {
        HASH_CONTENT_FULL,
    } ;
protected:
    BOOL m_bDataOwner;
    PBYTE m_pbData;
    DWORD m_cbData;
    PBYTE m_pbHash;
    DWORD m_cbHash;
    HRESULT HashData(HCRYPTHASH);
    HashContentType m_ContentType;
    HANDLE m_hFile;
    BOOL    m_NeedToReadData;

    HRESULT ReadData();
public:

    HRESULT SetFileName(LPCWSTR wszFileName);
    HRESULT ReleaseFileHandle()
    {
        WRAPPER_CONTRACT;
        return SetFileHandle(INVALID_HANDLE_VALUE);
    };
    HRESULT SetFileHandle(HANDLE hFile)
    {
        LEAF_CONTRACT;
        m_hFile=hFile;
        return S_OK;
    };

    HRESULT SetData(PBYTE pbData, DWORD cbData) // Owned by owners context. Will not be deleted
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;

        m_pbData = pbData;
        m_cbData = cbData;
        m_bDataOwner = FALSE;
        m_NeedToReadData=FALSE;
        return S_OK;
    }

    PBYTE GetHash() { LEAF_CONTRACT; _ASSERTE(!m_NeedToReadData);return m_pbHash; }
    DWORD GetHashSize() { LEAF_CONTRACT;_ASSERTE(!m_NeedToReadData); return m_cbHash; }

    HRESULT CalculateHash(DWORD algid);

    AssemblyFileHash(HashContentType contentType)
        : m_pbData( NULL ),
          m_cbData( 0 ),
          m_pbHash( NULL ),
          m_cbHash( 0 ),
          m_ContentType(contentType),
          m_hFile(INVALID_HANDLE_VALUE),
          m_NeedToReadData(TRUE)
    {
        CONTRACTL
        {
            CONSTRUCTOR_CHECK;
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;
    }

    ~AssemblyFileHash()
    {
        delete [] m_pbHash;
        if (m_bDataOwner)
            delete [] m_pbData;
        if (m_hFile!=INVALID_HANDLE_VALUE)
            CloseHandle(m_hFile);
        m_hFile=INVALID_HANDLE_VALUE;
    }
};

#endif
