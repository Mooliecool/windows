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
#include "certificatecache.h"

CertificateCache::CertificateCache () {
    WRAPPER_CONTRACT;
    for (DWORD i=0; i < MAX_CACHED_CERTIFICATES; i++) {
        m_Entry[i] = NULL;
    }
    m_CertificateCacheCrst.Init("Certificate Cache", CrstPublisherCertificate, CRST_DEFAULT);
}

CertificateCache::~CertificateCache () {
    // Let the OS collect the memory allocated for the cached certificates.
}

COR_TRUST* CertificateCache::GetEntry (DWORD index) {
    LEAF_CONTRACT;
    if (index < 0 || index >= MAX_CACHED_CERTIFICATES)
        return NULL;
    return m_Entry[index];
}

EnumCertificateAdditionFlags CertificateCache::AddEntry (COR_TRUST* pCertificate, DWORD* pIndex) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        PRECONDITION(pIndex != NULL);
    } CONTRACTL_END;

    *pIndex = FindEntry(pCertificate);
    if (*pIndex < MAX_CACHED_CERTIFICATES)
        return AlreadyExists; // the certificate is already cached.
    if (m_dwNumEntries >= MAX_CACHED_CERTIFICATES)
        return CacheSaturated; // the cache is full

    CrstHolder csh(&m_CertificateCacheCrst);
    if (m_dwNumEntries >= MAX_CACHED_CERTIFICATES)
        return CacheSaturated;

    // check again now that we have the lock.
    *pIndex = FindEntry(pCertificate);
    if (*pIndex < MAX_CACHED_CERTIFICATES)
        return AlreadyExists;

    *pIndex = m_dwNumEntries;
    m_Entry[m_dwNumEntries++] = pCertificate;
    return Success;
}

BOOL CertificateCache::Contains (COR_TRUST* pCertificate) {
    WRAPPER_CONTRACT;
    DWORD index = FindEntry(pCertificate);
    return (index < MAX_CACHED_CERTIFICATES && index >= 0);
}

DWORD CertificateCache::FindEntry (COR_TRUST* pCertificate) {
    CONTRACTL 
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
    }CONTRACTL_END;

    for (DWORD i=0; i < MAX_CACHED_CERTIFICATES; i++) {
        if (m_Entry[i] != NULL) {
            if ((pCertificate->cbSigner == m_Entry[i]->cbSigner) &&
                (memcmp(pCertificate->pbSigner, m_Entry[i]->pbSigner, m_Entry[i]->cbSigner) == 0))
                return i;
        }
    }
    return 0xFFFFFFFF;
}
