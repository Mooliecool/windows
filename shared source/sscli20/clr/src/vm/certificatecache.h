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

#ifndef _CERTIFICATECACHE_H_
#define _CERTIFICATECACHE_H_

#include "corpermp.h"
#include "crst.h"

#define MAX_CACHED_CERTIFICATES 10

enum EnumCertificateAdditionFlags {
    Success         = 0,
    CacheSaturated  = 1,
    AlreadyExists   = 2
};

class CertificateCache {
public:
    EnumCertificateAdditionFlags AddEntry (COR_TRUST* pCertificate, DWORD* pIndex);
    COR_TRUST* GetEntry (DWORD index);
    BOOL Contains (COR_TRUST* pCertificate);

    CertificateCache ();
    ~CertificateCache ();

private:
    DWORD       m_dwNumEntries;
    COR_TRUST*  m_Entry [MAX_CACHED_CERTIFICATES];
    CrstStatic  m_CertificateCacheCrst;

    DWORD FindEntry (COR_TRUST* pCertificate);
};

#endif //_CERTIFICATECACHE_H_
