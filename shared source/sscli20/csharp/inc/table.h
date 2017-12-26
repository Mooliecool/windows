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
// File: table.h
//
// ===========================================================================

#ifndef _TABLE_H_
#define _TABLE_H_

#include "csiface.h"
#include "name.h"
#include "mempool.h"

////////////////////////////////////////////////////////////////////////////////
// CTableImpl

class CTableImpl
{
    static CMemPool *m_pPool;         // Memory pool for nodes in all tables

protected:
    // Each node in a bucket chain is one of these
    struct HASHNODE
    {
        HASHNODE    *pNext;
        NAME        *pName;
        void        *pData;
    };

    CComPtr<ICSNameTable>   m_spNameTable;
    HASHNODE                **m_ppBuckets;
    DWORD                   m_dwBuckets;
    DWORD                   m_iCount;

    CTableImpl (ICSNameTable *pNameTable);
    virtual ~CTableImpl();


    // Basic find/add/remove operations
    HASHNODE    *Hash_Find (NAME *pName);
    HRESULT     Hash_Add (NAME *pName, HASHNODE **ppNode);
    BOOL        Hash_Remove (NAME *pName);
    void        Hash_CopyContents (void **ppData, NAME **ppNames);
    DWORD       Hash_BucketIndex (DWORD dwHash);

    // This is called when an item is removed -- overridden in the template.
    virtual void    Hash_OnRemove (HASHNODE *pNode) = 0;

    // Text-to-NAME conversions.
    HRESULT     Hash_Add (PCWSTR pszName, NAME **ppName) { return m_spNameTable->Add (pszName, ppName); }
    HRESULT     Hash_Add (PCWSTR pszName, long iLen, NAME **ppName) { return m_spNameTable->AddLen (pszName, iLen, ppName); }
    HRESULT     Hash_Lookup (PCWSTR pszName, NAME **ppName) { return m_spNameTable->Lookup (pszName, ppName); }
    HRESULT     Hash_Lookup (PCWSTR pszName, long iLen, NAME **ppName) { return m_spNameTable->LookupLen (pszName, iLen, ppName); }
    HRESULT     Hash_LookupNoCase (PCWSTR pszName, bool fIsFileName, NAME **ppName);
    HRESULT     Hash_AddNoCase (PCWSTR pszName, bool fIsFileName, NAME **ppName);

public:
    // Call Init() and Shutdown() to initialize and free the static members.
    static HRESULT Init();
    static void    Shutdown();

    // If not provided at construction time, use this to set the name table for
    // this table.  If you don't set one, you crash.  Life is tough.
    void            SetNameTable (ICSNameTable *pTable) { ASSERT (m_spNameTable == NULL); m_spNameTable = pTable; }
    ICSNameTable *  GetNameTable() { ASSERT (m_spNameTable != NULL); return m_spNameTable; }

    // Non-template-specific functions
    long        Count () { return m_iCount; }
    void        RemoveAll ();
    HRESULT     CopyContentsFrom (CTableImpl *pTable);

    // In case a name table isn't handy...
    HRESULT     AddName (PCWSTR pszName, NAME **ppName) { return Hash_Add (pszName, ppName); }
    HRESULT     AddNameNoCase (PCWSTR pszName, bool fIsFileName, NAME **ppName) { return Hash_AddNoCase (pszName, fIsFileName, ppName); }
    HRESULT     LookupName (PCWSTR pszName, NAME **ppName) { return Hash_Lookup (pszName, ppName); }
    HRESULT     LookupNameNoCase (PCWSTR pszName, bool fIsFileName, NAME **ppName) { return Hash_LookupNoCase (pszName, fIsFileName, ppName); }
};


////////////////////////////////////////////////////////////////////////////////
// CTable
//
// This is a templatized implementation of CTableImpl.  You must use this one;
// you cannot instanciate a CTableImpl alone.
//
// A CTable object stores pointers to T's.  You can perform special handling on
// addition and removal of items by overriding OnAdd() and OnRemove().
//
// In order to be case insensitive, the caller must use the *NoCase functions
// to create NAME *'s, and only use those in lookups/adds.  The table will not
// match two names with different case, period.

template <class T>
class CTable : public CTableImpl
{
protected:
    void    Hash_OnRemove (HASHNODE *pNode) { if (pNode->pData != NULL) OnRemove ((T *)pNode->pData); }

public:
    CTable (ICSNameTable *pTable) : CTableImpl(pTable) {}
    ~CTable () { RemoveAll (); }

    // Simple lookup.  Returns the pointer stored on the original add.
    bool Contains(PCWSTR pszName)
    {
        return Find(pszName) != NULL;
    }

    T       *Find (PCWSTR pszName)          
    { 
        NAME *p; 
        if (Hash_Lookup (pszName, &p) == S_OK) 
            return Find (p); 
        return NULL; 
    }
    T       *FindNoCase (PCWSTR pszName, bool fIsFileName)    
    { 
        NAME *p; 
        if (Hash_LookupNoCase (pszName, fIsFileName, &p) == S_OK) 
            return Find (p); 
        return NULL; 
    }
    T       *Find (NAME *pName)
    {
        HASHNODE *p = Hash_Find (pName);
        if (p)
            return (T *)p->pData;
        return NULL;
    }

    // Lookup returning a cookie, useful for replacing the value stored using SetData() or ChangeData().
    HRESULT Find (PCWSTR pszName, DWORD_PTR *pdwCookie)
    { 
        NAME *p; 
        if (Hash_Lookup (pszName, &p) == S_OK) 
            return Find (p, pdwCookie); 
        return E_FAIL; 
    }
    HRESULT FindNoCase (PCWSTR pszName, bool fIsFileName, DWORD_PTR *pdwCookie)
    { 
        NAME *p; 
        if (Hash_LookupNoCase (pszName, fIsFileName, &p) == S_OK) 
            return Find (p, pdwCookie); 
        return E_FAIL; 
    }
    HRESULT Find (NAME *pName, DWORD_PTR *pdwCookie)
    {
        HASHNODE *p = Hash_Find (pName);
        if (p == NULL)
            return E_FAIL;
        *pdwCookie = (DWORD_PTR)p;
        return S_OK;
    }

    // Simple add.  Returns S_OK if added, S_FALSE if already present
    HRESULT Add (PCWSTR pszName, T *pData)          
    { 
        NAME *p; 
        HRESULT hr; 
        if (SUCCEEDED (hr = Hash_Add (pszName, &p))) 
            hr = Add (p, pData); 
        return hr; 
    }
    HRESULT AddNoCase (PCWSTR pszName, bool fIsFileName, T *pData)
    { 
        NAME *p; 
        HRESULT hr; 
        if (SUCCEEDED (hr = Hash_AddNoCase (pszName, fIsFileName, &p))) 
            hr = Add (p, pData); 
        return hr; 
    }
    HRESULT Add (NAME *pName, T *pData)
    {
        ASSERT (pData != NULL);     // Can't add NULL values to these tables!

        HASHNODE    *p;
        HRESULT     hr = Hash_Add (pName, &p);

        if (hr == S_OK)
        {
            ASSERT (p != NULL);
            p->pData = pData;
            if (pData)
            {
                OnAdd (pData);
                m_iCount++;
            }
        }
        return hr;
    }

    // Lookup, adding a placeholder if not already present.  S_OK return indicates
    // the name was already there; S_FALSE means a new entry was placed in the table.
    // dwCookie represents the data or the placeholder; data must be added later with
    // SetData() if it's a placeholder (S_FALSE), or can be retrieved with GetData() if
    // it's the actual data.
    HRESULT FindOrAdd (PCWSTR pszName, DWORD_PTR *pdwCookie)
    { 
        NAME *p; 
        HRESULT hr;  
        if (SUCCEEDED (hr = Hash_Add (pszName, &p))) 
            hr = FindOrAdd (p, pdwCookie); 
        return hr; 
    }
    HRESULT FindOrAddNoCase (PCWSTR pszName, bool fIsFileName, DWORD_PTR *pdwCookie)
    { 
        NAME *p; 
        HRESULT hr;  
        if (SUCCEEDED (hr = Hash_AddNoCase (pszName, fIsFileName, &p))) 
            hr = FindOrAdd (p, pdwCookie); 
        return hr; 
    }
    HRESULT FindOrAdd (NAME *pName, DWORD_PTR *pdwCookie)
    {
        HASHNODE    *p;
        HRESULT     hr = Hash_Add (pName, &p);

        if (SUCCEEDED (hr))
        {
            ASSERT (p != NULL);
            *pdwCookie = (DWORD_PTR)p;
            return hr == S_OK ? S_FALSE : S_OK;
        }

        return hr;
    }

    // Given a cookie, store a value in the table.  This is equivalent to doing a REMOVE, followed by
    // an ADD -- so OnRemove and OnAdd will be called appropriately.
    void    SetData (DWORD_PTR dwCookie, T *pData)
    {
        HASHNODE    *p = (HASHNODE *)dwCookie;

        if ((T *)p->pData)
        {
            OnRemove ((T *)p->pData);
            m_iCount--;
        }

        p->pData = pData;

        if (pData)
        {
            OnAdd (pData);
            m_iCount++;
        }
    }

    // Given a cookie, replace the data value stored there.  Does NOT call OnAdd or OnRemove, but
    // maintains the count correctly.
    void    ChangeData (DWORD_PTR dwCookie, T *pData)
    {
        HASHNODE    *p = (HASHNODE *)dwCookie;

        if ((T *)p->pData)
            m_iCount--;

        p->pData = pData;

        if (pData)
            m_iCount++;
    }

    // Return the value stored at the given cookie
    T       *GetData (DWORD_PTR dwCookie) { return (T *)(((HASHNODE *)dwCookie)->pData); }

    // Detach the value connected to this cookie.  Does NOT call OnRemove.
    T       *Detach (DWORD_PTR dwCookie) { HASHNODE *p = (HASHNODE *)dwCookie; T *t = (T *)p->pData; p->pData = NULL; if (t) m_iCount--; return t; }

    // Removal.  Note that there's no mechanism for removing based on data, since
    // we have no way to search other than linearly.
    BOOL    Remove (PCWSTR pszName)         
    { 
        NAME *p; 
        if (Hash_Lookup (pszName, &p) == S_OK) 
            return Remove (p); 
        return FALSE; 
    }
    BOOL    RemoveNoCase (PCWSTR pszName, bool fIsFileName)   
    { 
        NAME *p; 
        if (Hash_LookupNoCase (pszName, fIsFileName, &p) == S_OK) 
            return Remove (p); 
        return FALSE; 
    }
    BOOL    Remove (NAME *pName)            { return Hash_Remove (pName); }
    void    Remove (DWORD_PTR dwCookie)     { Hash_Remove (((HASHNODE *)dwCookie)->pName); }

    // Table enumeration is done by copying the values into an array (optionally the names as well).
    // Caller must allocate the arrays to the appropriate size first -- assumptions are made!
    HRESULT CopyContents (T **ppData, size_t iDataCount, NAME **ppNames = NULL, size_t iNameCount = 0)
    {
        // the caller needs to allocate arrays large enough to hold the data and names
        // so verify that here. In theory we wont use it all since some entries may
        // be NULL, but the caller can't know this so we'll need a full allocation
        if (ppData)
        {
            if (iDataCount < m_iCount)
            {
                ASSERT(FALSE);
                return E_INVALIDARG;
            }
        }

        if (ppNames)
        {
            if (iNameCount < m_iCount)
            {
                ASSERT(FALSE);
                return E_INVALIDARG;
            }
        }

        Hash_CopyContents ((void **)ppData, ppNames);
        return S_OK;
    }

    // Use this to copy the contents from the given table to this table
    HRESULT CopyContentsFrom (CTable<T> *pTable)
    {
        if (pTable->m_ppBuckets == NULL)
            return S_OK;

        HRESULT     hr;

        for (DWORD i=0; i<pTable->m_dwBuckets; i++)
        {
            for (HASHNODE *p = pTable->m_ppBuckets[i]; p != NULL; p = p->pNext)
            {
                if (p->pData)
                {
                    if (FAILED (hr = Add (p->pName, (T *)p->pData)))
                        return hr;
                }
            }
        }

        return S_OK;
    }



    // Override these to perform special handling on add/remove
    virtual void    OnAdd (T *pData) {}
    virtual void    OnRemove (T *pData) {}
};

////////////////////////////////////////////////////////////////////////////////
// CAutoRefTable
//
// A derivation of CTable that automatically calls AddRef()/Release() through
// the data stored in it.

template <class T>
class CAutoRefTable : public CTable<T>
{
public:
    CAutoRefTable (ICSNameTable *pTable) : CTable<T> (pTable) {}
    ~CAutoRefTable() { this->RemoveAll(); }   // Must do this to ensure our override of OnRemove is called
    void    OnAdd (T *pData) { pData->AddRef(); }
    void    OnRemove (T *pData) { pData->Release(); }
};

////////////////////////////////////////////////////////////////////////////////
// CAutoDelTable
//
// A derivation of CTable that automatically deletes the data when removed.  Use
// this only for data that is allocated with 'new'!

template <class T>
class CAutoDelTable : public CTable<T>
{
public:
    CAutoDelTable (ICSNameTable *pTable) : CTable<T> (pTable) {}
    ~CAutoDelTable() { this->RemoveAll(); }   // Must do this to ensure our override of OnRemove is called
    void    OnRemove (T *pData) { delete pData; }
};

////////////////////////////////////////////////////////////////////////////////
// CIdentTable
//
// This is a simple table of names, keyed by themselves.

class CIdentTable : public CTable<NAME>
{
public:
    CIdentTable (ICSNameTable *pTable) : CTable<NAME> (pTable) {}
};

#endif  // _TABLE_H_
