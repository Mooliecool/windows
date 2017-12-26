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
// File: table.cpp
//
// ===========================================================================

#include "pch.h"
#include "table.h"
#include "unilib.h"
// <ROTORTODO>: Waiting for reply to see if it is OK to replace wcscpy with string safe function.
#undef wcscpy
#ifdef PLATFORM_UNIX
#define wcscpy        PAL_wcscpy
#endif // PLATFORM_UNIX

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::m_pool

CMemPool  *CTableImpl::m_pPool = NULL;

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::CTableImpl

CTableImpl::CTableImpl (ICSNameTable *pTable) :
    m_ppBuckets(NULL),
    m_dwBuckets(0),
    m_iCount(0)
{
    ASSERT (m_pPool);
    // Start out with a small number of buckets -- must be even power of 2
    m_dwBuckets = 32;
    m_iCount = 0;
    m_ppBuckets = (HASHNODE **)VSAllocZero (m_dwBuckets * sizeof (HASHNODE));
    m_spNameTable = pTable;
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::~CTableImpl

CTableImpl::~CTableImpl ()
{
    RemoveAll();
    VSFree (m_ppBuckets);
    ASSERT (m_pPool);
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::Init

HRESULT CTableImpl::Init()
{
    if (!m_pPool) {
        m_pPool = new CMemPool (sizeof (CTableImpl::HASHNODE));
    }
    return m_pPool ? S_OK : E_OUTOFMEMORY;
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::Shutdown

void CTableImpl::Shutdown()
{
    delete m_pPool;
    m_pPool = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::Hash_Find

CTableImpl::HASHNODE *CTableImpl::Hash_Find (NAME *pName)
{
    // If we haven't allocated anything yet, or the name given is NULL, we'll
    // never find it...
    if (m_ppBuckets == NULL || pName == NULL)
        return NULL;

    long        iBucket = Hash_BucketIndex (pName->hash);
    HASHNODE    *pRun;

    for (pRun = m_ppBuckets[iBucket]; pRun != NULL; pRun = pRun->pNext)
        if (pRun->pName == pName)
            break;

    return pRun;
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::Hash_BucketIndex

DWORD CTableImpl::Hash_BucketIndex (DWORD dwHash) 
{ 
    return dwHash & (m_dwBuckets - 1); 
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::Hash_Add
//

HRESULT CTableImpl::Hash_Add (NAME *pName, HASHNODE **ppNode)
{
    if (m_ppBuckets == NULL)
        return E_OUTOFMEMORY;

    DWORD       dwBucket = Hash_BucketIndex (pName->hash);
    HASHNODE    *pRun;

    for (pRun = m_ppBuckets[dwBucket]; pRun != NULL; pRun = pRun->pNext)
    {
        if (pRun->pName == pName)
        {
            // This name exists in the table.  If the data is NULL, it is a
            // previously Detach()'d or recently FindOrAdd()'d entry,
            // so pretend we just added it.
            *ppNode = pRun;
            return (pRun->pData != NULL) ? S_FALSE : S_OK;
        }
    }

    if (m_iCount >= m_dwBuckets * 2)
    {
        // We have more than 2 entries per bucket (average).  Double the size
        // of the bucket array, and then rifle through and re-bucket the  table
        ASSERT (m_ppBuckets);
        HASHNODE    **ppNewBuckets = (HASHNODE **)VSRealloc (m_ppBuckets, (m_dwBuckets * 2) * sizeof (HASHNODE *));
        if (ppNewBuckets == NULL) {
            *ppNode = NULL;
            return E_OUTOFMEMORY;
        }
        ZeroMemory (ppNewBuckets + m_dwBuckets, m_dwBuckets * sizeof (HASHNODE *));

        DWORD   dwOldBuckets = m_dwBuckets;

        m_dwBuckets *= 2;
        m_ppBuckets = ppNewBuckets;

        for (DWORD i=0; i<dwOldBuckets; i++)
        {
            HASHNODE    **pp = m_ppBuckets + i;

            while (*pp != NULL)
            {
                DWORD   dwNewBucket = Hash_BucketIndex ((*pp)->pName->hash);

                if (dwNewBucket != (DWORD)i)
                {
                    // If it no longer belongs to this bucket, it must belong
                    // to this bucket + old bucket size!
                    ASSERT (dwNewBucket == (DWORD)(i + dwOldBuckets));
                    HASHNODE    *pNode = *pp;
                    *pp = pNode->pNext;
                    pNode->pNext = m_ppBuckets[dwNewBucket];
                    m_ppBuckets[dwNewBucket] = pNode;
                }
                else
                {
                    // This one stays put...
                    pp = &(*pp)->pNext;
                }
            }
        }

        // Now that we've rebucketed, pName might fall into a new one...
        dwBucket = Hash_BucketIndex (pName->hash);
    }

    // Allocate a new hash node and slam it into the bucket, right in front.
    pRun = (HASHNODE *)m_pPool->Allocate();
    *ppNode = pRun;
    if (pRun == NULL)
        return E_OUTOFMEMORY;
    pRun->pNext = m_ppBuckets[dwBucket];
    m_ppBuckets[dwBucket] = pRun;
    pRun->pName = pName;
    pRun->pData = NULL;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::Hash_Remove

BOOL CTableImpl::Hash_Remove (NAME *pName)
{
    if (m_ppBuckets == NULL)
        return FALSE;

    DWORD       dwBucket = Hash_BucketIndex (pName->hash);
    HASHNODE    **ppBack;

    // Must start at the beginning of the bucket and search for pNode...
    for (ppBack = m_ppBuckets + dwBucket; *ppBack != NULL; ppBack = &(*ppBack)->pNext)
    {
        if ((*ppBack)->pName == pName)
        {
            HASHNODE    *pNode = *ppBack;
            *ppBack = pNode->pNext;
            if (pNode->pData != NULL)
                m_iCount--;
            Hash_OnRemove (pNode);
            m_pPool->Free (pNode);
            return TRUE;
        }
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::RemoveAll

void CTableImpl::RemoveAll ()
{
    if (m_ppBuckets == NULL)
        return;

    for (DWORD i = 0; i<m_dwBuckets; i++)
    {
        while (m_ppBuckets[i] != NULL)
        {
            HASHNODE    *pTmp = m_ppBuckets[i];
            m_ppBuckets[i] = m_ppBuckets[i]->pNext;
            Hash_OnRemove (pTmp);
            m_pPool->Free (pTmp);
        }
    }

    m_iCount = 0;
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::Hash_CopyContents

void CTableImpl::Hash_CopyContents (void **ppData, NAME **ppNames)
{
    if (m_ppBuckets == NULL)
        return;

    DWORD       i, iIndex = 0;

    for (i = 0; i<m_dwBuckets; i++)
    {
        for (HASHNODE *pRun = m_ppBuckets[i]; pRun != NULL; pRun = pRun->pNext)
        {
            // Only copy out non-NULL values...
            if (pRun->pData != NULL)
            {
                if (ppData != NULL)
                    ppData[iIndex] = pRun->pData;
                if (ppNames != NULL)
                    ppNames[iIndex] = pRun->pName;
                iIndex++;
            }
        }
    }
    ASSERT(iIndex <= m_iCount);
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::Hash_AddNoCase

HRESULT CTableImpl::Hash_AddNoCase (PCWSTR pszName, bool fIsFileName, NAME **ppName)
{
    long    iLenPlusNull = (long)wcslen (pszName) + 1;
    PWSTR   pszCopy = STACK_ALLOC (WCHAR, iLenPlusNull);

    if (fIsFileName)
    {
        StringCchCopyW (pszCopy, iLenPlusNull, pszName);
        FilenameToLowerCaseInPlace (pszCopy);
    }
    else
        ToLowerCase (pszName, pszCopy, (size_t)-1);

    return Hash_Add (pszCopy, ppName);
}

////////////////////////////////////////////////////////////////////////////////
// CTableImpl::Hash_LookupNoCase

HRESULT CTableImpl::Hash_LookupNoCase (PCWSTR pszName, bool fIsFileName, NAME **ppName)
{
    long    iLenPlusNull = (long)wcslen (pszName) + 1;
    PWSTR   pszCopy = STACK_ALLOC (WCHAR, iLenPlusNull);

    if (fIsFileName)
    {
        StringCchCopyW (pszCopy, iLenPlusNull, pszName);
        FilenameToLowerCaseInPlace (pszCopy);
    }
    else
        ToLowerCase (pszName, pszCopy, (size_t)-1);

    return Hash_Lookup (pszCopy, ppName);
}
