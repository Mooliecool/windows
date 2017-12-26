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
//
// pendingload.h
//
#ifndef _H_PENDINGLOAD
#define _H_PENDINGLOAD

#include "crst.h"
#include "class.h"
#include "typekey.h"
#include "typehash.h"
#include "vars.hpp"
#include "shash.h"
#include "typestring.h"

//
// A temporary structure used when loading and resolving classes
//
class PendingTypeLoadEntry
{
    friend class ClassLoader;
    friend class LockHolder;
    friend class PendingTypeLoadEntry_LockHolder;
public:
    //---------------------------------------------------------------------------
    // The PendingTypeLoadEntry() lock has a higher level than UnresolvedClassLock.
    // But whenever we create one, we have to acquire it while holding the UnresolvedClassLock.
    // This is safe since we're the ones that created the lock and are guaranteed to acquire
    // it without blocking. But to prevent the crstlevel system from asserting, we
    // must acquire it as part of the ctor and pass it the CRST_NO_LEVEL_CHECK flag to disable
    // the assert.
    //---------------------------------------------------------------------------
    PendingTypeLoadEntry(TypeKey typeKey, TypeHandle typeHnd)
        : m_Crst("PendingTypeLoadEntry",
                 CrstPendingTypeLoadEntry,
                 CrstFlags(CRST_REENTRANCY|CRST_HOST_BREAKABLE|CRST_UNSAFE_SAMELEVEL),
                 Crst::CRST_ACQUIRE_IMMEDIATELY
#ifdef _DEBUG
                 ,Crst::CRST_NO_LEVEL_CHECK
#endif

                 ),
        m_typeKey(typeKey)
          
    {
        WRAPPER_CONTRACT;

        m_typeHandle = typeHnd;
        m_dwWaitCount = 1;
        m_hrResult = E_FAIL;
        m_pException = NULL;
#ifdef _DEBUG
    if (LoggingOn(LF_CLASSLOADER, LL_INFO10000))
    {
        SString name;
        TypeString::AppendTypeKeyDebug(name, &m_typeKey);
        LOG((LF_CLASSLOADER, LL_INFO10000, "PHASEDLOAD: Creating loading entry for type %S\n", name.GetUnicode()));
    }
#endif

    }

    ~PendingTypeLoadEntry() 
    {
        LEAF_CONTRACT;

        if (m_pException && !m_pException->IsPreallocatedException()) {
            delete m_pException;
        }
    }

#ifdef _DEBUG
    BOOL HasLock()
    {
        LEAF_CONTRACT;
        return m_Crst.OwnedByCurrentThread();
    }
#endif

    VOID DECLSPEC_NORETURN ThrowException()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM(););
        }
        CONTRACTL_END;

        if (m_pException)
            PAL_CPP_THROW(Exception *, m_pException->Clone());

        // If we got here, we're probably attempting a recursive load.

        if (m_hrResult == E_FAIL)
            m_hrResult = COR_E_TYPELOAD;

#ifndef DACCESS_COMPILE
        if (m_hrResult == COR_E_TYPELOAD)
        {
            TypeKey typeKey = GetTypeKey();
            ClassLoader::ThrowTypeLoadException(&typeKey,
                                                IDS_CLASSLOAD_GENERAL);

        }
        else
#endif //DACCESS_COMPILE
            EX_THROW(EEMessageException, (m_hrResult));
    }

    void SetException(Exception *pException)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FORBID_FAULT;

        m_typeHandle = TypeHandle();
        m_hrResult = COR_E_TYPELOAD;

        // we don't care if this fails
        // we already know the HRESULT so if we can't store
        // the details - so be it
        EX_TRY
        {
            FAULT_NOT_FATAL();
            SetExceptionThrowing(pException);
        }
        EX_CATCH
        {
            m_pException=NULL;
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
    
    void SetResult(TypeHandle typeHnd)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FORBID_FAULT;
        PRECONDITION(CheckPointer(typeHnd));
        PRECONDITION(HasLock());

        m_typeHandle = typeHnd;
        m_hrResult = S_OK;
    }

    TypeKey GetTypeKey()
    {
        LEAF_CONTRACT;
        return m_typeKey;
    }
    
    void SetExceptionThrowing(Exception *pException);

private:
    Crst                m_Crst;

 public:
    // Result of loading; this is first created in the CREATE stage of class loading
    TypeHandle          m_typeHandle;   

 private:

    // Type that we're loading
    TypeKey             m_typeKey;

    // Number of threads waiting for this type
    DWORD               m_dwWaitCount;

    // Error result, propagated to all threads loading this class
    HRESULT             m_hrResult;

    // Exception object to throw
    Exception          *m_pException;
};

class PendingTypeLoadEntry_LockHolder : public CrstPreempHolder
{
 public:
    PendingTypeLoadEntry_LockHolder(PendingTypeLoadEntry *pLE = NULL, BOOL Take = TRUE)
        : CrstPreempHolder(&pLE->m_Crst, Take)
        {
            LEAF_CONTRACT;
        }
    FORCEINLINE void Assign(PendingTypeLoadEntry *pLE)
        {
            LEAF_CONTRACT;
            CrstPreempHolder::Assign(&pLE->m_Crst);
        }

    FORCEINLINE void AssignPreaquiredLock(PendingTypeLoadEntry *pLE)
    {
        LEAF_CONTRACT;

#ifndef DACCESS_COMPILE

        _ASSERTE( pLE->HasLock());
        _ASSERTE( m_value == NULL );
        _ASSERTE( !m_acquired );

        m_value = &pLE->m_Crst;
        m_acquired = TRUE;
#endif
    }
};


class PendingTypeLoadTable
{
protected:
    struct TableEntry
    {
        TableEntry* pNext;
        DWORD                 dwHashValue;
        PendingTypeLoadEntry* pData;
    };

    TableEntry     **m_pBuckets;    // Pointer to first entry for each bucket
    DWORD           m_dwNumBuckets;

public:

#ifdef _DEBUG
    DWORD           m_dwDebugMemory;
#endif

    static PendingTypeLoadTable *Create(LoaderHeap *pHeap, DWORD dwNumBuckets, AllocMemTracker *pamTracker);

private:
    // These functions don't actually exist - declared private to prevent bypassing PendingTypeLoadTable::Create
    void *          operator new(size_t size);
    void            operator delete(void *p);

    PendingTypeLoadTable();
    ~PendingTypeLoadTable();

public:
    BOOL            InsertValue(PendingTypeLoadEntry* pEntry);
    BOOL            DeleteValue(TypeKey *pKey);
    PendingTypeLoadEntry* GetValue(TypeKey *pKey);
    TableEntry* AllocNewEntry();
#ifdef _DEBUG
    void            Dump();
#endif

private:
    TableEntry* FindItem(TypeKey *pKey);
};


#endif // _H_PENDINGLOAD
