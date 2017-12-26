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
// OBJECT.CPP
//
// Definitions of a Com+ Object
//

#include "common.h"

#include "vars.hpp"
#include "class.h"
#include "object.h"
#include "threads.h"
#include "excep.h"
#include "eeconfig.h"
#include "gc.h"
#include "remoting.h"
#include "field.h"

void* CompressedStackObject::GetUnmanagedCompressedStack()
{
    LEAF_CONTRACT;  
    return ((m_compressedStackHandle != NULL)?m_compressedStackHandle->GetHandle():NULL);
}

SVAL_IMPL(INT32, ArrayBase, s_arrayBoundsZero);

// follow the necessary rules to get a new valid hashcode for an object
DWORD Object::ComputeHashCode()
{
    DWORD hashCode;
   
    // note that this algorithm now uses at most HASHCODE_BITS so that it will
    // fit into the objheader if the hashcode has to be moved back into the objheader
    // such as for an object that is being frozen
    do
    {
        // we use the high order bits in this case because they're more random
        hashCode = GetThread()->GetNewHashCode() >> (32-HASHCODE_BITS);
    }
    while (hashCode == 0);   // need to enforce hashCode != 0

    // verify that it really fits into HASHCODE_BITS
     _ASSERTE((hashCode & ((1<<HASHCODE_BITS)-1)) == hashCode);

    return hashCode;
}

BOOL Object::ValidateObjectWithPossibleAV()
{
    CANNOT_HAVE_CONTRACT;
    
    MethodTable *pMT = PTR_MethodTable((TADDR) m_pMethTab);

    return pMT->ValidateWithPossibleAV();
}


#ifndef DACCESS_COMPILE

MethodTable *Object::GetTrueMethodTable()
{
    CONTRACT(MethodTable*)
    {
        MODE_COOPERATIVE;
        GC_NOTRIGGER;
        NOTHROW;
        SO_TOLERANT;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // make certain object layout in corjit.h is consistant with
    // what is in object.h
    C_ASSERT(offsetof(Object, m_pMethTab) == offsetof(CORINFO_Object, methTable));
    C_ASSERT(offsetof(I1Array, m_Array) == offsetof(CORINFO_Array, i1Elems));
    C_ASSERT(offsetof(PTRArray, m_Array) == offsetof(CORINFO_RefArray, refElems));

    MethodTable *mt = GetMethodTable();
    if (mt->IsThunking())
    {
        if(mt->IsTransparentProxyType())
        {
            mt = CTPMethodTable::GetClassBeingProxied(ObjectToOBJECTREF(this)).GetMethodTable();
        }
        _ASSERTE(!mt->IsThunking());
    }
    RETURN mt;

}

TypeHandle Object::GetTrueTypeHandle()
{
    WRAPPER_CONTRACT;

    if (m_pMethTab->IsArray())
        return ((ArrayBase*) this)->GetTypeHandle();
    else
        return TypeHandle(GetTrueMethodTable());
}

TypeHandle Object::GetTypeHandle()
{
    WRAPPER_CONTRACT;

    if (m_pMethTab->IsArray())
        return ((ArrayBase*) this)->GetTypeHandle();
    else 
        return TypeHandle(m_pMethTab);
}


/* static */ BOOL Object::SupportsInterface(OBJECTREF pObj, MethodTable* pInterfaceMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pInterfaceMT));
        PRECONDITION(pObj->GetTrueMethodTable()->IsRestored());
    }
    CONTRACTL_END

    BOOL bSupportsItf = FALSE;

    THROW_BAD_FORMAT_MAYBE(pInterfaceMT->IsInterface(), BFA_EXPECTED_INTERFACE, pInterfaceMT);

    GCPROTECT_BEGIN(pObj)
    {
        // Make sure the interface method table has been restored.
        pInterfaceMT->CheckRestore();

        // Check to see if the static class definition indicates we implement the interface.
        if (pObj->GetTrueMethodTable()->CanCastToInterface(pInterfaceMT))
        {
            bSupportsItf = TRUE;
        }
    }
    GCPROTECT_END();

    return bSupportsItf;
}

TypeHandle ArrayBase::GetTypeHandle() const
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    // This ensures that we can always get the typehandle for an object in hand
    // without triggering the noisy parts of the loader.
    //
    // The debugger can cause this routine to be called on an unmanaged thread
    // so this really is important.
    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    TypeHandle elemType = GetArrayElementTypeHandle();
    CorElementType kind = GetMethodTable()->GetInternalCorElementType();
    unsigned rank = GetArrayClass()->GetRank();

    CONTRACT_VIOLATION(SOToleranceViolation);
    // == FailIfNotLoadedOrNotRestored
    TypeHandle arrayType = ClassLoader::LoadArrayTypeThrowing(elemType, kind, rank, ClassLoader::DontLoadTypes);  
    CONSISTENCY_CHECK(!arrayType.IsNull()); 
    return(arrayType);
}

Assembly *AssemblyBaseObject::GetAssembly()
{
    WRAPPER_CONTRACT;
    return m_pAssembly->GetAssembly();
}

#ifdef _DEBUG
// Object::DEBUG_SetAppDomain specified DEBUG_ONLY in the contract to disable SO-tolerance
// checking for paths that are DEBUG-only.
void Object::DEBUG_SetAppDomain(AppDomain *pDomain)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        DEBUG_ONLY;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pDomain));
    }
    CONTRACTL_END;

    SetAppDomain(pDomain);
}
#endif

void Object::SetAppDomain(AppDomain *pDomain)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pDomain));
    }
    CONTRACTL_END;

#ifndef _DEBUG
    if (!GetMethodTable()->IsDomainNeutral())
    {
        //
        // If we have a per-app-domain method table, we can 
        // infer the app domain from the method table, so 
        // there is no reason to mark the object.
        //
        // But we don't do this in a debug build, because
        // we want to be able to detect the case when the
        // domain was unloaded from underneath an object (and
        // the MethodTable will be toast in that case.)
        //

        _ASSERTE(pDomain == GetMethodTable()->GetDomain());
    }
    else
#endif
    {
        ADIndex index = pDomain->GetIndex();
        GetHeader()->SetAppDomainIndex(index);
    }

    _ASSERTE(GetHeader()->GetAppDomainIndex().m_dwIndex != 0);
}


AppDomain *Object::GetAppDomain()
{
    WRAPPER_CONTRACT;
#ifndef _DEBUG
    if (!GetMethodTable()->IsDomainNeutral())
        return (AppDomain*) GetMethodTable()->GetDomain();
#endif

    ADIndex index = GetHeader()->GetAppDomainIndex();

    if (index.m_dwIndex == 0)
        return NULL;

    AppDomain *pDomain = SystemDomain::TestGetAppDomainAtIndex(index);

#if CHECK_APP_DOMAIN_LEAKS
    if (! g_pConfig->AppDomainLeaks())
        return pDomain;

    if (IsAppDomainAgile())
        return NULL;

    //
    // If an object has an index of an unloaded domain (its ok to be of a 
    // domain where an unload is in progress through), go ahead
    // and make it agile. If this fails, we have an invalid reference
    // to an unloaded domain.  If it succeeds, the object is no longer
    // contained in that app domain so we can continue.
    //

    if (pDomain == NULL)
    {
        if (SystemDomain::IndexOfAppDomainBeingUnloaded() == index) {
            // if appdomain is unloading but still alive and is valid to have instances
            // in that domain, then use it.
            AppDomain *tmpDomain = SystemDomain::AppDomainBeingUnloaded();
            if (tmpDomain && tmpDomain->ShouldHaveInstances())
                pDomain = tmpDomain;
        }
        if (!pDomain && ! TrySetAppDomainAgile(FALSE))
        {
            _ASSERTE(!"Attempt to reference an object belonging to an unloaded domain");
        }
    }
#endif

    return pDomain;
}

STRINGREF AllocateString(SString sstr)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;
    
    COUNT_T length = sstr.GetCount(); // count of WCHARs excluding terminating NULL
    STRINGREF strObj = AllocateString(length + 1);
    strObj->SetStringLength(length);
    wcsncpy_s(strObj->GetBuffer(), length + 1, sstr.GetUnicode(), _TRUNCATE);

    return strObj;
}

#if CHECK_APP_DOMAIN_LEAKS

BOOL Object::IsAppDomainAgile()
{
    WRAPPER_CONTRACT;
    DEBUG_ONLY_FUNCTION;

    SyncBlock *psb = PassiveGetSyncBlock();

    if (psb)
    {
        if (psb->IsAppDomainAgile())
            return TRUE;
        if (psb->IsCheckedForAppDomainAgile())
            return FALSE;
        //special case: thread statics array is considered to be agile
        if (psb->IsThreadStaticsArray ())
        {
            //thread static array is an array of object array
            MethodTable * pMT = GetGCSafeMethodTable ();
            _ASSERTE (pMT->IsArray () 
                && (pMT->GetArrayElementType() == ELEMENT_TYPE_CLASS || pMT->GetArrayElementType() == ELEMENT_TYPE_SZARRAY));
            return TRUE;
        }
    }
    return CheckAppDomain(NULL);
}

BOOL Object::TrySetAppDomainAgile(BOOL raiseAssert)
{
    LEAF_CONTRACT;
    FAULT_NOT_FATAL();
    DEBUG_ONLY_FUNCTION;

    BOOL ret = TRUE;

    EX_TRY
    {
        ret = SetAppDomainAgile(raiseAssert);
    }
    EX_CATCH{}
    EX_END_CATCH(SwallowAllExceptions);

    return ret;
}


BOOL Object::ShouldCheckAppDomainAgile (BOOL raiseAssert, BOOL *pfResult)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    DEBUG_ONLY_FUNCTION;

    if (!g_pConfig->AppDomainLeaks())
    {
        *pfResult = TRUE;
        return FALSE;
    }

    if (this == NULL)
    {
        *pfResult = TRUE;
        return FALSE;
    }

    if (IsAppDomainAgile())
    {
        *pfResult = TRUE;
        return FALSE;
    }

    // if it's not agile and we've already checked it, just bail early
    if (IsCheckedForAppDomainAgile())
    {
        *pfResult = FALSE;
        return FALSE;
    }

    if (IsTypeNeverAppDomainAgile())
    {
        if (raiseAssert)
            _ASSERTE(!"Attempt to reference a domain bound object from an agile location");
        *pfResult = FALSE;
        return FALSE;
    }

    //
    // Do not allow any object to be set to be agile unless we 
    // are compiling field access checking into the class.  This
    // will help guard against unintentional "agile" propagation
    // as well.
    //

    if (!IsTypeAppDomainAgile() && !IsTypeCheckAppDomainAgile()) 
    {
        if (raiseAssert)
            _ASSERTE(!"Attempt to reference a domain bound object from an agile location");
        *pfResult = FALSE;
        return FALSE;
    }

    return TRUE;
}


BOOL Object::SetAppDomainAgile(BOOL raiseAssert, SetAppDomainAgilePendingTable *pTable)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        DEBUG_ONLY;
    }
    CONTRACTL_END;
    BEGIN_DEBUG_ONLY_CODE;
    BOOL fResult;
    if (!this->ShouldCheckAppDomainAgile(raiseAssert, &fResult))
        return fResult;

    //
    // If a SetAppDomainAgilePendingTable is provided, then SetAppDomainAgile
    // was called via SetAppDomainAgile.  Simply store this object in the
    // table, and let the calling SetAppDomainAgile process it later in a
    // non-recursive manner.
    //

    if (pTable == NULL)
    {
        pTable = (SetAppDomainAgilePendingTable *)ClrFlsGetValue(TlsIdx_AppDomainAgilePendingTable);
    }
    if (pTable)
    {
        //
        // If the object is already being checked (on this thread or another),
        // don't duplicate the effort.  Return TRUE to tell the caller to
        // continue processing other references.  Since we're just testing
        // the bit we don't need to take the spin lock.
        //
        
        ObjHeader* pOh = this->GetHeader();
        _ASSERTE(pOh);

        if (pOh->GetBits() & BIT_SBLK_AGILE_IN_PROGRESS)
        {
            return TRUE;
    }

        pTable->PushReference(this);
    }
    else
    {
        //
        // Initialize the table of pending objects
        //
        
        SetAppDomainAgilePendingTable table;
        class ResetPendingTable
        {
        public:
            ResetPendingTable(SetAppDomainAgilePendingTable *pTable)
            {
                ClrFlsSetValue(TlsIdx_AppDomainAgilePendingTable, pTable);
            }
            ~ResetPendingTable()
            {
                ClrFlsSetValue(TlsIdx_AppDomainAgilePendingTable, NULL);
            }
        };

        ResetPendingTable resetPendingTable(&table);

        //
        // Iterate over the table, processing all referenced objects until the
        // entire graph has its sync block marked, or a non-agile object is
        // found.  The loop will start with the current object, as though we
        // just removed it from the table as a pending reference.
        //

        Object *pObject = this;

        do
        {
            //
            // Mark the object to identify recursion.
            // ~SetAppDomainAgilePendingTable will clean up
            // BIT_SBLK_AGILE_IN_PROGRESS, so attempt to push the object first
            // in case it needs to throw an exception.
            //

            table.PushParent(pObject);

            ObjHeader* pOh = pObject->GetHeader();
            _ASSERTE(pOh);

            bool fInProgress = false;

            pOh->EnterSpinLock();
            {
                if (pOh->GetBits() & BIT_SBLK_AGILE_IN_PROGRESS)
                {
                    fInProgress = true;
                }
                else
                {
                    pOh->SetBit(BIT_SBLK_AGILE_IN_PROGRESS);
                }
            }
            pOh->ReleaseSpinLock();

            if (fInProgress)
            {
                //
                // Object is already being processed, so just remove it from
                // the table and look for another object.
                //

                bool fReturnedToParent;
                Object *pLastObject = table.GetPendingObject(&fReturnedToParent);
                CONSISTENCY_CHECK(pLastObject == pObject && fReturnedToParent);
            }
            else
            {
                
                //
                // Finish processing this object.  Any references will be added to
                // the table.
        //

                if (!pObject->SetAppDomainAgileWorker(raiseAssert, &table))
            return FALSE;
            }

        //
            // Find the next object to explore.
        //

            for (;;)
        {
                bool fReturnedToParent;
                pObject = table.GetPendingObject(&fReturnedToParent);

                //
                // No more objects in the table?
                //

                if (!pObject)
                    break;

                //
                // If we've processed all objects reachable through an object,
                // then clear BIT_SBLK_AGILE_IN_PROGRESS, and look for another
                // object in the table.
                //

                if (fReturnedToParent)
            {
                    pOh = pObject->GetHeader();
                    _ASSERTE(pOh);

                    pOh->EnterSpinLock();
                    pOh->ClrBit(BIT_SBLK_AGILE_IN_PROGRESS);
                    pOh->ReleaseSpinLock();
            }
            else
            {
                    //
                    // Re-check whether we should explore through this reference.
                    //

                    if (pObject->ShouldCheckAppDomainAgile(raiseAssert, &fResult))
                        break;
                    
                    if (!fResult)
                    return FALSE;
            }
        }
    }
        while (pObject);
    }
    END_DEBUG_ONLY_CODE;
    return TRUE;
}


BOOL Object::SetAppDomainAgileWorker(BOOL raiseAssert, SetAppDomainAgilePendingTable *pTable)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    DEBUG_ONLY_FUNCTION;

    BOOL ret = TRUE;

        if (! IsTypeAppDomainAgile() && ! SetFieldsAgile(raiseAssert, pTable))
        {
            SetIsCheckedForAppDomainAgile();

            ret = FALSE;
        }
    
    if (ret)
    {
        SetSyncBlockAppDomainAgile();
    }

    return ret;
}


SetAppDomainAgilePendingTable::SetAppDomainAgilePendingTable ()
    : m_Stack(sizeof(PendingEntry))
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    DEBUG_ONLY_FUNCTION;
}


SetAppDomainAgilePendingTable::~SetAppDomainAgilePendingTable ()
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    DEBUG_ONLY_FUNCTION;

    while (TRUE)
    {
        Object *pObj;
        bool fObjMarked;
        pObj = GetPendingObject(&fObjMarked);
        if (pObj == NULL)
        {
            break;
        }
    
        if (fObjMarked)
        {
            ObjHeader* pOh = pObj->GetHeader();
            _ASSERTE(pOh);

            pOh->EnterSpinLock();
            pOh->ClrBit(BIT_SBLK_AGILE_IN_PROGRESS);
            pOh->ReleaseSpinLock();
        }
}
}


void Object::SetSyncBlockAppDomainAgile()
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    DEBUG_ONLY_FUNCTION;

    SyncBlock *psb = PassiveGetSyncBlock();
    if (! psb)
    {
        psb = GetSyncBlock();
    }
    psb->SetIsAppDomainAgile();
}

BOOL Object::CheckAppDomain(AppDomain *pAppDomain)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    DEBUG_ONLY_FUNCTION;

    if (!g_pConfig->AppDomainLeaks())
        return TRUE;

    if (this == NULL)
        return TRUE;

    if (IsAppDomainAgileRaw())
        return TRUE;

#ifndef _DEBUG
    MethodTable *pMT = GetGCSafeMethodTable();

    if (!pMT->IsDomainNeutral())
        return pAppDomain == pMT->GetDomain();
#endif

    ADIndex index = GetHeader()->GetAppDomainIndex();

    _ASSERTE(index.m_dwIndex != 0);

    return (pAppDomain != NULL && index == pAppDomain->GetIndex());
}

BOOL Object::IsTypeAppDomainAgile()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    DEBUG_ONLY_FUNCTION;

    MethodTable *pMT = GetGCSafeMethodTable();

    if (pMT->IsArray())
    {
        TypeHandle th = pMT->GetApproxArrayElementTypeHandle();
        return th.IsArrayOfElementsAppDomainAgile();
    }
    else
        return pMT->GetClass()->IsAppDomainAgile();
}

BOOL Object::IsTypeCheckAppDomainAgile()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    DEBUG_ONLY_FUNCTION;

    MethodTable *pMT = GetGCSafeMethodTable();

    if (pMT->IsArray())
    {
        TypeHandle th = pMT->GetApproxArrayElementTypeHandle();
        return th.IsArrayOfElementsCheckAppDomainAgile();
    }
    else
        return pMT->GetClass()->IsCheckAppDomainAgile();
}

BOOL Object::IsTypeNeverAppDomainAgile()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    DEBUG_ONLY_FUNCTION;

    return !IsTypeAppDomainAgile() && !IsTypeCheckAppDomainAgile();
}

BOOL Object::IsTypeTypesafeAppDomainAgile()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    DEBUG_ONLY_FUNCTION;

    return IsTypeAppDomainAgile() && !IsTypeCheckAppDomainAgile();
}

BOOL Object::TryAssignAppDomain(AppDomain *pAppDomain, BOOL raiseAssert)
{
    LEAF_CONTRACT;
    FAULT_NOT_FATAL();
    DEBUG_ONLY_FUNCTION;

    BOOL ret = TRUE;

    EX_TRY
    {
        ret = AssignAppDomain(pAppDomain,raiseAssert);
    }
    EX_CATCH{}
    EX_END_CATCH(SwallowAllExceptions);

    return ret;
}

BOOL Object::AssignAppDomain(AppDomain *pAppDomain, BOOL raiseAssert)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    DEBUG_ONLY_FUNCTION;

    if (!g_pConfig->AppDomainLeaks())
        return TRUE;

    if (CheckAppDomain(pAppDomain))
        return TRUE;

    //
    // App domain does not match; try to make this object agile
    //

    if (IsTypeNeverAppDomainAgile())
    {
        if (raiseAssert)
        {
            if (pAppDomain == NULL)
                _ASSERTE(!"Attempt to reference a domain bound object from an agile location");
            else
                _ASSERTE(!"Attempt to reference a domain bound object from a different domain");
        }
        return FALSE;
    }
    else
    {
        //
        // Make object agile
        //

        if (! IsTypeAppDomainAgile() && ! SetFieldsAgile(raiseAssert))
        {
            SetIsCheckedForAppDomainAgile();
            return FALSE;
        }

        SetSyncBlockAppDomainAgile();

        return TRUE;        
    }
}

BOOL Object::AssignValueTypeAppDomain(MethodTable *pMT, void *base, AppDomain *pAppDomain, BOOL raiseAssert)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    DEBUG_ONLY_FUNCTION;

    if (!g_pConfig->AppDomainLeaks())
        return TRUE;

    if (pMT->GetClass()->IsAppDomainAgile())
        return TRUE;

    if (pAppDomain == NULL)
    {
        //
        // Do not allow any object to be set to be agile unless we 
        // are compiling field access checking into the class.  This
        // will help guard against unintentional "agile" propagation
        // as well.
        //

        if (pMT->GetClass()->IsNeverAppDomainAgile())
        {
            _ASSERTE(!"Attempt to reference a domain bound object from an agile location");
            return FALSE;
        }

        return SetClassFieldsAgile(pMT, base, TRUE/*=baseIsVT*/, raiseAssert);
    }
    else
    {
        return ValidateClassFields(pMT, base, TRUE/*=baseIsVT*/, pAppDomain, raiseAssert);
    }
}

BOOL Object::SetFieldsAgile(BOOL raiseAssert, SetAppDomainAgilePendingTable *pTable)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        DEBUG_ONLY;
    }
    CONTRACTL_END;

    BOOL result = TRUE;

    MethodTable *pMT= GetGCSafeMethodTable();

    if (pMT->IsArray())
    {
        switch (pMT->GetArrayElementType())
        {
        case ELEMENT_TYPE_CLASS:
            {
                PtrArray *pArray = (PtrArray *) this;

                DWORD n = pArray->GetNumComponents();
                OBJECTREF *p = (OBJECTREF *) 
                  (((BYTE*)pArray) + ArrayBase::GetDataPtrOffset(GetGCSafeMethodTable()));

                for (DWORD i=0; i<n; i++)
                {
                    if (!p[i]->SetAppDomainAgile(raiseAssert, pTable))
                        result = FALSE;
                }

                break;
            }
        case ELEMENT_TYPE_VALUETYPE:
            {
                ArrayBase *pArray = (ArrayBase *) this;

                MethodTable *pElemMT = pMT->GetApproxArrayElementTypeHandle().GetMethodTable();

                BYTE *p = ((BYTE*)pArray) + ArrayBase::GetDataPtrOffset(GetGCSafeMethodTable());
                unsigned size = pArray->GetComponentSize();
                DWORD n = pArray->GetNumComponents();

                for (DWORD i=0; i<n; i++)
                    if (!SetClassFieldsAgile(pElemMT, p + i*size, TRUE/*=baseIsVT*/, raiseAssert, pTable))
                        result = FALSE;

                break;
            }
            
        default:
            _ASSERTE(!"Unexpected array type");
        }
    }
    else
    {
        if (pMT->GetClass()->IsNeverAppDomainAgile())
        {
            _ASSERTE(!"Attempt to reference a domain bound object from an agile location");
            return FALSE;
        }

        while (pMT != NULL && !pMT->GetClass()->IsTypesafeAppDomainAgile())
        {
            if (!SetClassFieldsAgile(pMT, this, FALSE/*=baseIsVT*/, raiseAssert, pTable))
                result = FALSE;

            pMT = pMT->GetParentMethodTable();

            if (pMT->GetClass()->IsNeverAppDomainAgile())
            {
                _ASSERTE(!"Attempt to reference a domain bound object from an agile location");
                return FALSE;
            }
        }
    }

    return result;
}

BOOL Object::SetClassFieldsAgile(MethodTable *pMT, void *base, BOOL baseIsVT, BOOL raiseAssert, SetAppDomainAgilePendingTable *pTable)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;

    BOOL result = TRUE;

    if (pMT->GetClass()->IsNeverAppDomainAgile())
    {
        _ASSERTE(!"Attempt to reference a domain bound object from an agile location");
        return FALSE;
    }

    // This type approximation is OK since we are only checking some layout information 
    // and all compatible instantiations share the same GC characteristics
    ApproxFieldDescIterator fdIterator(pMT, ApproxFieldDescIterator::INSTANCE_FIELDS);
    FieldDesc* pField;

    while ((pField = fdIterator.Next()) != NULL)
    {
        if (pField->IsDangerousAppDomainAgileField())
        {
            if (pField->GetFieldType() == ELEMENT_TYPE_CLASS)
            {
                OBJECTREF ref;

                if (baseIsVT)
                    ref = *(OBJECTREF*) pField->GetAddressNoThrowNoGC(base);
                else
                    ref = *(OBJECTREF*) pField->GetAddressGuaranteedInHeap(base, FALSE);

                if (ref != 0 && !ref->IsAppDomainAgile())
                {
                    if (!ref->SetAppDomainAgile(raiseAssert, pTable))
                        result = FALSE;
                }
            }
            else if (pField->GetFieldType() == ELEMENT_TYPE_VALUETYPE)
            {
                // Be careful here - we may not have loaded a value
                // type field of a class under prejit, and we don't
                // want to trigger class loading here.

                TypeHandle th = pField->LookupFieldTypeHandle();
                if (!th.IsNull())
                {
                    void *nestedBase;

                    if (baseIsVT)
                        nestedBase = pField->GetAddressNoThrowNoGC(base);
                    else
                        nestedBase = pField->GetAddressGuaranteedInHeap(base, FALSE);

                    if (!SetClassFieldsAgile(th.GetMethodTable(),
                                             nestedBase,
                                             TRUE/*=baseIsVT*/,
                                             raiseAssert,
                                             pTable))
                    {
                        result = FALSE;
                    }
                }
            }
            else
            {
                _ASSERTE(!"Bad field type");
            }
        }
    }

    return result;
}

BOOL Object::ValidateAppDomain(AppDomain *pAppDomain)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;


    if (!g_pConfig->AppDomainLeaks())
        return TRUE;

    if (this == NULL)
        return TRUE;

    if (CheckAppDomain())
        return ValidateAppDomainFields(pAppDomain);

    return AssignAppDomain(pAppDomain);
}

BOOL Object::ValidateAppDomainFields(AppDomain *pAppDomain)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;

    BOOL result = TRUE;

    MethodTable *pMT = GetGCSafeMethodTable();

    while (pMT != NULL && !pMT->GetClass()->IsTypesafeAppDomainAgile())
    {
        if (!ValidateClassFields(pMT, this, FALSE/*=baseIsVT*/, pAppDomain))
            result = FALSE;

        pMT = pMT->GetParentMethodTable();
    }

    return result;
}

BOOL Object::ValidateValueTypeAppDomain(MethodTable *pMT, void *base, AppDomain *pAppDomain, BOOL raiseAssert)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;

    if (!g_pConfig->AppDomainLeaks())
        return TRUE;

    if (pAppDomain == NULL)
    {
        if (pMT->GetClass()->IsTypesafeAppDomainAgile())
            return TRUE;
        else if (pMT->GetClass()->IsNeverAppDomainAgile())
        {
            if (raiseAssert)
                _ASSERTE(!"Value type cannot be app domain agile");
            return FALSE;
        }
    }

    return ValidateClassFields(pMT, base, TRUE/*=baseIsVT*/, pAppDomain, raiseAssert);
}

BOOL Object::ValidateClassFields(MethodTable *pMT, void *base, BOOL baseIsVT, AppDomain *pAppDomain, BOOL raiseAssert)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;

    BOOL result = TRUE;

    // This type approximation is OK since we are only checking some layout information 
    // and all compatible instantiations share the same GC characteristics
    ApproxFieldDescIterator fdIterator(pMT, ApproxFieldDescIterator::INSTANCE_FIELDS);
    FieldDesc* pField;

    while ((pField = fdIterator.Next()) != NULL)
    {
        if (!pMT->GetClass()->IsCheckAppDomainAgile() 
            || pField->IsDangerousAppDomainAgileField())
        {
            if (pField->GetFieldType() == ELEMENT_TYPE_CLASS)
            {
                OBJECTREF ref;

                if (baseIsVT)
                    ref = ObjectToOBJECTREF(*(Object**) pField->GetAddressNoThrowNoGC(base));
                else
                    ref = ObjectToOBJECTREF(*(Object**) pField->GetAddressGuaranteedInHeap(base, FALSE));

                if (ref != 0 && !ref->AssignAppDomain(pAppDomain, raiseAssert))
                    result = FALSE;
            }
            else if (pField->GetFieldType() == ELEMENT_TYPE_VALUETYPE)
            {
                // Be careful here - we may not have loaded a value
                // type field of a class under prejit, and we don't
                // want to trigger class loading here.

                TypeHandle th = pField->LookupFieldTypeHandle();
                if (!th.IsNull())
                {
                    void *nestedBase;

                    if (baseIsVT)
                        nestedBase = pField->GetAddressNoThrowNoGC(base);
                    else
                        nestedBase = pField->GetAddressGuaranteedInHeap(base, FALSE);

                    if (!ValidateValueTypeAppDomain(th.GetMethodTable(),
                                                    nestedBase,
                                                    pAppDomain,
                                                    raiseAssert
                                                    ))
                        result = FALSE;

                }
            }
        }
    }

    return result;
}

#endif // CHECK_APP_DOMAIN_LEAKS

void Object::ValidatePromote(ScanContext *sc, DWORD flags)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;


#if defined (VERIFY_HEAP)
    Validate();
#endif

#if CHECK_APP_DOMAIN_LEAKS
    // Do app domain integrity checking here
    if (g_pConfig->AppDomainLeaks())
    {
        AppDomain *pDomain = GetAppDomain();


        if ((flags & GC_CALL_CHECK_APP_DOMAIN)
            && pDomain != NULL 
            && !pDomain->ShouldHaveRoots() 
            && !TrySetAppDomainAgile(FALSE))    
        {
            _ASSERTE(!"Found GC object which should have been purged during app domain unload.");
        }
    }
#endif
}

void Object::ValidateHeap(Object *from)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

#if defined (VERIFY_HEAP)
    //no need to verify next object's header in this case
    //since this is called in verify_heap, which will verfiy every object anyway
    Validate(TRUE, FALSE); 
#endif

#if CHECK_APP_DOMAIN_LEAKS

    // Do app domain integrity checking here
    if (g_pConfig->AppDomainLeaks())
    {
        AppDomain *pDomain = from->GetAppDomain();

        // 
        // Don't perform check if we're checking for agility, and the containing type is not
        // marked checked agile - this will cover "proxy" type agility 
        // where cross references are allowed
        //

        // And don't perform check if the from object is thread statics array
        SyncBlock *psb = from->PassiveGetSyncBlock();
        if ((!psb || !psb->IsThreadStaticsArray ()) &&              
             (pDomain != NULL || from->GetMethodTable()->GetClass()->IsCheckAppDomainAgile()))
        {
           
            //special case:thread object is allowed to hold a context belonging to current domain
            if (from->GetMethodTable () == g_pThreadClass && 
                      ( this == OBJECTREFToObject(((ThreadBaseObject *)from)->m_ExposedContext)
                       || this == OBJECTREFToObject(((ThreadBaseObject *)from)->m_ExecutionContext)))
            {  
                if (((ThreadBaseObject *)from)->m_InternalThread)
                    _ASSERTE (CheckAppDomain (((ThreadBaseObject *)from)->m_InternalThread->GetDomain ()));
            }
            // special case: Overlapped has a field OverlappedData which may be moved to default domain
            // during AD unload
            else if (GetMethodTable() == g_pOverlappedDataClass && 
                     GetAppDomainIndex() == SystemDomain::System()->DefaultDomain()->GetIndex())
            {
            }
            else
            {
                TryAssignAppDomain(pDomain);
            }
        }

        if (pDomain != NULL
            && (!psb || !psb->IsThreadStaticsArray ())
            && !pDomain->ShouldHaveInstances() 
            && !TrySetAppDomainAgile(FALSE))
            _ASSERTE(!"Found GC object which should have been purged during app domain unload.");
    }
#endif
}


#if defined (VERIFY_HEAP)

//handle faults during concurrent gc.
LONG process_exception (EXCEPTION_POINTERS* ep, LPVOID pv){

    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;



    if (ep->ExceptionRecord->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND)) {
        _ASSERTE(!"Detected use of a corrupted OBJECTREF. Possible GC hole.");
        ::TerminateProcess(GetCurrentProcess(), STATUS_ACCESS_VIOLATION);
    }

    PEXCEPTION_RECORD er = ep->ExceptionRecord;
    if (   er->ExceptionCode == STATUS_BREAKPOINT
        || er->ExceptionCode == STATUS_SINGLE_STEP
        || er->ExceptionCode == STATUS_STACK_OVERFLOW)
        return EXCEPTION_CONTINUE_SEARCH;


    // zap mon will generate priviledged instruction exceptions too, so we have 
    // to check if it's a monitor exception before bailing out here.
    if ( er->ExceptionCode != STATUS_ACCESS_VIOLATION)
        return EXCEPTION_EXECUTE_HANDLER;


    return EXCEPTION_EXECUTE_HANDLER;
}
#endif //VERIFY_HEAP



void Object::SetOffsetObjectRef(DWORD dwOffset, size_t dwValue)
{ 
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    OBJECTREF*  location;
    OBJECTREF   o;

    location = (OBJECTREF *) &GetData()[dwOffset];
    o        = ObjectToOBJECTREF(*(Object **)  &dwValue);

    SetObjectReference( location, o, GetAppDomain() );
}        
/******************************************************************/
/*
 * Write Barrier Helper
 *
 * Use this function to assign an object reference into
 * another object.
 *
 * It will set the appropriate GC Write Barrier data
 */

#if CHECK_APP_DOMAIN_LEAKS
void SetObjectReferenceChecked(OBJECTREF *dst,OBJECTREF ref,AppDomain *pAppDomain)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    BEGIN_DEBUG_ONLY_CODE(GetThread());
    ref->TryAssignAppDomain(pAppDomain);
    return SetObjectReferenceUnchecked(dst,ref);
    END_DEBUG_ONLY_CODE;
}
#endif

void SetObjectReferenceUnchecked(OBJECTREF *dst,OBJECTREF ref)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    // Assign value. We use casting to avoid going thru the overloaded
    // OBJECTREF= operator which in this case would trigger a false
    // write-barrier violation assert.
    *(Object* volatile *)dst = OBJECTREFToObject(ref);
#ifdef _DEBUG
    Thread::ObjectRefAssign(dst);
#endif
    ErectWriteBarrier(dst, ref);
}

/******************************************************************/
    // copies src to dest worrying about write barriers.  
    // Note that it can work on normal objects (but not arrays)
    // if dest, points just after the VTABLE.
#if CHECK_APP_DOMAIN_LEAKS
void CopyValueClassChecked(void* dest, void* src, MethodTable *pMT, AppDomain *pDomain)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    BEGIN_DEBUG_ONLY_CODE(GetThread());
    FAULT_NOT_FATAL();
    EX_TRY
    {
        Object::AssignValueTypeAppDomain(pMT, src, pDomain);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
    CopyValueClassUnchecked(dest,src,pMT);
    END_DEBUG_ONLY_CODE;
}
#endif
    
void __stdcall CopyValueClassUnchecked(void* dest, void* src, MethodTable *pMT) 
{

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_ENTRY_POINT;
    BEGIN_ENTRYPOINT_VOIDRET;

    _ASSERTE(!pMT->IsArray());

    switch (pMT->GetNumInstanceFieldBytes())
    {        
    case 1:
        *(volatile UINT8*)dest = *(UINT8*)src;
        break;
#ifndef ALIGN_ACCESS
        // we can hit an alignment fault if the value type has multiple 
        // smaller fields.  Example: if there are two I4 fields, the 
        // value class can be aligned to 4-byte boundaries, yet the 
        // NumInstanceFieldBytes is 8
    case 2:
        *(volatile UINT16*)dest = *(UINT16*)src;
        break;
    case 4:
        *(volatile UINT32*)dest = *(UINT32*)src;
        break;
    case 8:
        *(volatile UINT64*)dest = *(UINT64*)src;
        break;
#endif // !ALIGN_ACCESS
    default:
        memcpyNoGCRefs(dest, src, pMT->GetNumInstanceFieldBytes());
        break;
    }

        // Tell the GC about any copies.  
    if (pMT->ContainsPointers())
    {   
        CGCDesc* map = CGCDesc::GetCGCDescFromMT(pMT);
        CGCDescSeries* cur = map->GetHighestSeries();
        CGCDescSeries* last = map->GetLowestSeries();
        DWORD size = pMT->GetBaseSize();
        _ASSERTE(cur >= last);
        do                                                                  
        {   
            // offset to embedded references in this series must be
            // adjusted by the VTable pointer, when in the unboxed state.
            size_t offset = cur->GetSeriesOffset() - sizeof(void*);
            OBJECTREF* srcPtr = (OBJECTREF*)(((BYTE*) src) + offset);
            OBJECTREF* destPtr = (OBJECTREF*)(((BYTE*) dest) + offset);
            OBJECTREF* srcPtrStop = (OBJECTREF*)((BYTE*) srcPtr + cur->GetSeriesSize() + size);         
            while (srcPtr < srcPtrStop)                                         
            {   
                SetObjectReferenceUnchecked(destPtr, ObjectToOBJECTREF(*(Object**)srcPtr));
                srcPtr++;
                destPtr++;
            }                                                               
            cur--;                                                              
        } while (cur >= last);                                              
    }
    END_ENTRYPOINT_VOIDRET;

}

#if defined (VERIFY_HEAP)

#include "dbginterface.h"

    // make the checking code goes as fast as possible!
#if defined(_MSC_VER)
#pragma optimize("tgy", on)
#endif

#ifndef _DEBUG
#ifdef _ASSERTE
#undef _ASSERTE
#endif
#define _ASSERTE(c) if (!(c)) DebugBreak()
#endif

VOID Object::Validate(BOOL bDeep, BOOL bVerifyNextHeader)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    int lastTest = 0;
    if (this == NULL)
    {
        return;     // NULL is ok
    }

    if (g_fEEShutDown & ShutDown_Phase2)
    {
        return; // During second phase of shutdown code below is not guaranteed to work.
    }

#ifdef _DEBUG
    {
    Thread *pThread = GetThread();

    if (pThread != NULL && !(pThread->PreemptiveGCDisabled()))
    {
        // Debugger helper threads are special in that they take over for
        // what would normally be a nonEE thread (the RCThread).  If an
        // EE thread is doing RCThread duty, then it should be treated
        // as such.
        //
        // There are some GC threads in the same kind of category.  Note that
        // GetThread() sometimes returns them, if DLL_THREAD_ATTACH notifications
        // have run some managed code.
        if (!dbgOnly_IsSpecialEEThread())
            _ASSERTE(!"OBJECTREF being accessed while thread is in preemptive GC mode.");
    }
    }
#endif

    // this is a hot codepath in the checked builds - avoid going through the slow setjmp
    EXCEPTION_REGISTRATION_RECORD exRecord;
    exRecord.Handler = process_exception;
    INSTALL_EXCEPTION_HANDLING_RECORD(&exRecord);
    {
        MethodTable *pMT = GetGCSafeMethodTable();
        lastTest = 1;

        _ASSERTE(pMT->Validate ());
        lastTest = 2;

        _ASSERTE(GCHeap::GetGCHeap()->IsHeapPointer(this));

        lastTest = 3;

        if (bDeep)
        {
            _ASSERTE (GetHeader()->Validate ());
        }
        
        lastTest = 4;

        if (bDeep && (g_pConfig->GetHeapVerifyLevel() & EEConfig::HEAPVERIFY_GC)) {
            GCHeap::GetGCHeap()->ValidateObjectMember(this);
        }

        lastTest = 5;

        if (GCHeap::GetGCHeap()->IsHeapPointer(this, TRUE)) {
            _ASSERTE (!GCHeap::GetGCHeap()->IsObjectInFixedHeap(this));
        }

        lastTest = 6;

#if CHECK_APP_DOMAIN_LEAKS
        // when it's not safe to verify the fields, it's not safe to verify AppDomain either
        // because the process might try to access fields.
        if (bDeep && g_pConfig->AppDomainLeaks())
        {
            //
            // Check to see that our domain is valid.  This will assert if it has been unloaded.
            //
            SCAN_IGNORE_FAULT;
            GetAppDomain();
        }        
#endif

        lastTest = 7;

        //try to validate next object's header
        if (bDeep 
            && bVerifyNextHeader 
            && CNameSpace::GetGcRuntimeStructuresValid ()
            //NextObj could be very slow if concurrent GC is going on
            && !(GCHeap::IsGCHeapInitialized() && GCHeap::GetGCHeap ()->IsConcurrentGCInProgress ()))
        {
            Object * nextObj = GCHeap::GetGCHeap ()->NextObj (this);
            if (nextObj)
            {
                _ASSERTE(nextObj->GetHeader ()->Validate (FALSE));
            }
        }

        lastTest = 8;

    }
    UNINSTALL_EXCEPTION_HANDLING_RECORD(&exRecord);
}

#ifndef _DEBUG
#undef _ASSERTE
#define _ASSERTE(expr) ((void)0)
#endif   // _DEBUG

#endif   // VERIFY_HEAP


#ifdef USE_CHECKED_OBJECTREFS

//-------------------------------------------------------------
// Default constructor, for non-initializing declarations:
//
//      OBJECTREF or;
//-------------------------------------------------------------
OBJECTREF::OBJECTREF()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    m_asObj = (Object*)POISONC;
    Thread::ObjectRefNew(this);
}

//-------------------------------------------------------------
// Copy constructor, for passing OBJECTREF's as function arguments.
//-------------------------------------------------------------
OBJECTREF::OBJECTREF(const OBJECTREF & objref)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    VALIDATEOBJECTREF(objref.m_asObj);

    // !!! If this assert is fired, there are two possibilities:
    // !!! 1.  You are doing a type cast, e.g.  *(OBJECTREF*)pObj
    // !!!     Instead, you should use ObjectToOBJECTREF(*(Object**)pObj),
    // !!!                          or ObjectToSTRINGREF(*(StringObject**)pObj)
    // !!! 2.  There is a real GC hole here.
    // !!! Either way you need to fix the code.
    _ASSERTE(Thread::IsObjRefValid(&objref));
    if ((objref.m_asObj != 0) &&
        ((GCHeap*)GCHeap::GetGCHeap())->IsHeapPointer( (BYTE*)this ))
    {
        _ASSERTE(!"Write Barrier violation. Must use SetObjectReference() to assign OBJECTREF's into the GC heap!");
    }
    m_asObj = objref.m_asObj;
    
    if (m_asObj != 0) {
        ENABLESTRESSHEAP();
    }

    Thread::ObjectRefNew(this);
}


//-------------------------------------------------------------
// To allow NULL to be used as an OBJECTREF.
//-------------------------------------------------------------
OBJECTREF::OBJECTREF(TADDR nul)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;


    //_ASSERTE(nul == 0);
    m_asObj = (Object*)nul;
    if( m_asObj != NULL)
    {
        VALIDATEOBJECTREF(m_asObj);
        ENABLESTRESSHEAP();
    }
    Thread::ObjectRefNew(this);
}

//-------------------------------------------------------------
// This is for the GC's use only. Non-GC code should never
// use the "Object" class directly. The unused "int" argument
// prevents C++ from using this to implicitly convert Object*'s
// to OBJECTREF.
//-------------------------------------------------------------
OBJECTREF::OBJECTREF(Object *pObject)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    DEBUG_ONLY_FUNCTION;
    
    if ((pObject != 0) &&
        ((GCHeap*)GCHeap::GetGCHeap())->IsHeapPointer( (BYTE*)this ))
    {
        _ASSERTE(!"Write Barrier violation. Must use SetObjectReference() to assign OBJECTREF's into the GC heap!");
    }
    m_asObj = pObject;
    VALIDATEOBJECTREF(m_asObj);
    if (m_asObj != 0) {
        ENABLESTRESSHEAP();
    }
    Thread::ObjectRefNew(this);
}

//-------------------------------------------------------------
// Test against NULL.
//-------------------------------------------------------------
int OBJECTREF::operator!() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    // We don't do any validation here, as we want to allow zero comparison in preemptive mode
    return !m_asObj;
}

//-------------------------------------------------------------
// Compare two OBJECTREF's.
//-------------------------------------------------------------
int OBJECTREF::operator==(const OBJECTREF &objref) const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (objref.m_asObj != NULL) // Allow comparison to zero in preemptive mode
    {
        VALIDATEOBJECTREF(objref.m_asObj);

        // !!! If this assert is fired, there are two possibilities:
        // !!! 1.  You are doing a type cast, e.g.  *(OBJECTREF*)pObj
        // !!!     Instead, you should use ObjectToOBJECTREF(*(Object**)pObj),
        // !!!                          or ObjectToSTRINGREF(*(StringObject**)pObj)
        // !!! 2.  There is a real GC hole here.
        // !!! Either way you need to fix the code.
        _ASSERTE(Thread::IsObjRefValid(&objref));
        VALIDATEOBJECTREF(m_asObj);
        // If this assert fires, you probably did not protect
        // your OBJECTREF and a GC might have occured.  To
        // where the possible GC was, set a breakpoint in Thread::TriggersGC                     
        _ASSERTE(Thread::IsObjRefValid(this));

        if (m_asObj != 0 || objref.m_asObj != 0) {
            ENABLESTRESSHEAP();
        }
    }
    return m_asObj == objref.m_asObj;
}

//-------------------------------------------------------------
// Compare two OBJECTREF's.
//-------------------------------------------------------------
int OBJECTREF::operator!=(const OBJECTREF &objref) const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (objref.m_asObj != NULL) { // Allow comparison to zero in preemptive mode
        VALIDATEOBJECTREF(objref.m_asObj);

        // !!! If this assert is fired, there are two possibilities:
        // !!! 1.  You are doing a type cast, e.g.  *(OBJECTREF*)pObj
        // !!!     Instead, you should use ObjectToOBJECTREF(*(Object**)pObj),
        // !!!                          or ObjectToSTRINGREF(*(StringObject**)pObj)
        // !!! 2.  There is a real GC hole here.
        // !!! Either way you need to fix the code.
        _ASSERTE(Thread::IsObjRefValid(&objref));
        VALIDATEOBJECTREF(m_asObj);
        // If this assert fires, you probably did not protect
        // your OBJECTREF and a GC might have occured.  To
        // where the possible GC was, set a breakpoint in Thread::TriggersGC                     
        _ASSERTE(Thread::IsObjRefValid(this));

        if (m_asObj != 0 || objref.m_asObj != 0) {
            ENABLESTRESSHEAP();
        }
    }

    return m_asObj != objref.m_asObj;
}


//-------------------------------------------------------------
// Forward method calls.
//-------------------------------------------------------------
Object* OBJECTREF::operator->()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    VALIDATEOBJECTREF(m_asObj);
        // If this assert fires, you probably did not protect
        // your OBJECTREF and a GC might have occured.  To
        // where the possible GC was, set a breakpoint in Thread::TriggersGC                     
    _ASSERTE(Thread::IsObjRefValid(this));

    if (m_asObj != 0) {
        ENABLESTRESSHEAP();
    }

    // if you are using OBJECTREF directly,
    // you probably want an Object *
    return (Object *)m_asObj;
}


//-------------------------------------------------------------
// Forward method calls.
//-------------------------------------------------------------
const Object* OBJECTREF::operator->() const
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    VALIDATEOBJECTREF(m_asObj);
        // If this assert fires, you probably did not protect
        // your OBJECTREF and a GC might have occured.  To
        // where the possible GC was, set a breakpoint in Thread::TriggersGC                     
    _ASSERTE(Thread::IsObjRefValid(this));

    if (m_asObj != 0) {
        ENABLESTRESSHEAP();
    }

    // if you are using OBJECTREF directly,
    // you probably want an Object *
    return (Object *)m_asObj;
}


//-------------------------------------------------------------
// Assignment. We don't validate the destination so as not
// to break the sequence:
//
//      OBJECTREF or;
//      or = ...;
//-------------------------------------------------------------
OBJECTREF& OBJECTREF::operator=(const OBJECTREF &objref)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    VALIDATEOBJECTREF(objref.m_asObj);

    // !!! If this assert is fired, there are two possibilities:
    // !!! 1.  You are doing a type cast, e.g.  *(OBJECTREF*)pObj
    // !!!     Instead, you should use ObjectToOBJECTREF(*(Object**)pObj),
    // !!!                          or ObjectToSTRINGREF(*(StringObject**)pObj)
    // !!! 2.  There is a real GC hole here.
    // !!! Either way you need to fix the code.
    _ASSERTE(Thread::IsObjRefValid(&objref));

    if ((objref.m_asObj != 0) &&
        ((GCHeap*)GCHeap::GetGCHeap())->IsHeapPointer( (BYTE*)this ))
    {
        _ASSERTE(!"Write Barrier violation. Must use SetObjectReference() to assign OBJECTREF's into the GC heap!");
    }
    Thread::ObjectRefAssign(this);

    m_asObj = objref.m_asObj;
    if (m_asObj != 0) {
        ENABLESTRESSHEAP();
    }
    return *this;
}

//-------------------------------------------------------------
// Allows for the assignment of NULL to a OBJECTREF 
//-------------------------------------------------------------

OBJECTREF& OBJECTREF::operator=(TADDR nul)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(nul == 0);
    Thread::ObjectRefAssign(this);
    m_asObj = (Object*)nul;
    if (m_asObj != 0) {
        ENABLESTRESSHEAP();
    }
    return *this;
}
#endif  // DEBUG

#ifdef _DEBUG

void* __cdecl GCSafeMemCpy(void * dest, const void * src, size_t len)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (!(((*(BYTE**)&dest) <  g_lowest_address ) ||
          ((*(BYTE**)&dest) >= g_highest_address)))
    {
        Thread* pThread = GetThread();

        // GCHeap::IsHeapPointer has race when called in preemptive mode. It walks the list of segments
        // that can be modified by GC. Do the check below only if it is safe to do so.
        if (pThread != NULL && pThread->PreemptiveGCDisabled())
        {
            // Note there is memcpyNoGCRefs which will allow you to do a memcpy into the GC
            // heap if you really know you don't need to call the write barrier

            _ASSERTE(!GCHeap::GetGCHeap()->IsHeapPointer((BYTE *) dest) ||
                     !"using memcpy to copy into the GC heap, use CopyValueClass");
        }
    }
    return memcpyNoGCRefs(dest, src, len);
}

#endif // _DEBUG

void StackTraceArray::Append(StackTraceElement const * begin, StackTraceElement const * end)
{
    WRAPPER_CONTRACT;

    // ensure that only one thread can write to the array
    EnsureThreadAffinity();

    size_t newsize = Size() + (end - begin);
    Grow(newsize);
    memcpyNoGCRefs(GetData() + Size(), begin, (end - begin) * sizeof(StackTraceElement));
    MemoryBarrier();  // prevent the newsize from being reordered with the array copy
    SetSize(newsize);

#if defined(_DEBUG)
    CheckState();
#endif
}

void StackTraceArray::AppendSkipLast(StackTraceElement const * begin, StackTraceElement const * end)
{
    WRAPPER_CONTRACT;

    // to skip the last element, we need to replace it with the first element
    // from m_pStackTrace and do it atomically if possible,
    // otherwise we'll create a copy of the entire array, which is bad for performance,
    // and so should not be on the main path
    //

    // ensure that only one thread can write to the array
    EnsureThreadAffinity();

    assert(Size() > 0);

    StackTraceElement & last = GetData()[Size() - 1];
    if (last.PartiallyEqual(*begin))
    {
        // fast path: atomic update
        last.PartialAtomicUpdate(*begin);

        // append the rest
        if (end - begin > 1)
            Append(begin + 1, end);
    }
    else
    {
        // slow path: create a copy and append
        StackTraceArray copy(*this);
        GCPROTECT_BEGIN(copy);
            copy.SetSize(copy.Size() - 1);
            copy.Append(begin, end);
            this->Swap(copy);
        GCPROTECT_END();
    }

#if defined(_DEBUG)
    CheckState();
#endif
}

void StackTraceArray::CheckState() const
{
    WRAPPER_CONTRACT;
    
    if (!m_array)
        return;

    assert(GetObjectThread() == ::GetThread());
    
    size_t size = Size();
    StackTraceElement const * p = GetData();
    for (size_t i = 0; i < size; ++i)
        assert(p[i].pFunc != NULL);
}

void StackTraceArray::Grow(size_t grow_size)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END;

    size_t raw_size = grow_size * sizeof(StackTraceElement) + sizeof(ArrayHeader);

    if (!m_array)
    {
        SetArray(I1ARRAYREF(AllocatePrimitiveArray(ELEMENT_TYPE_I1, static_cast<DWORD>(raw_size))));
        SetSize(0);
        SetObjectThread();
    }
    else
    {
        if (Capacity() >= raw_size)
            return;

        // allocate a new array, copy the data
        size_t new_capacity = Max(Capacity() * 2, raw_size);

        _ASSERTE(new_capacity >= grow_size * sizeof(StackTraceElement) + sizeof(ArrayHeader));
        
        I1ARRAYREF newarr = (I1ARRAYREF) AllocatePrimitiveArray(ELEMENT_TYPE_I1, static_cast<DWORD>(new_capacity));
        memcpyNoGCRefs(newarr->GetDirectPointerToNonObjectElements(),
                       GetRaw(),
                       Size() * sizeof(StackTraceElement) + sizeof(ArrayHeader));

        SetArray(newarr);
    }
}

void StackTraceArray::EnsureThreadAffinity()
{
    WRAPPER_CONTRACT;

    if (!m_array)
        return;

    if (GetObjectThread() != ::GetThread())
    {
        // object is being changed by a thread different from the one which created it
        // make a copy of the array to prevent a race condition when two different threads try to change it
        StackTraceArray copy(*this);
        this->Swap(copy);
    }
}

StackTraceArray::StackTraceArray(StackTraceArray const & rhs)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END;

    m_array = (I1ARRAYREF) AllocatePrimitiveArray(ELEMENT_TYPE_I1, static_cast<DWORD>(rhs.Capacity()));

    GCPROTECT_BEGIN(m_array);
        volatile size_t size = rhs.Size();
        memcpyNoGCRefs(GetRaw(), rhs.GetRaw(), size * sizeof(StackTraceElement) + sizeof(ArrayHeader));

        SetSize(size);  // set size to the exact value which was used when we copied the data
                        // another thread might have changed it at the time of copying
        SetObjectThread();  // affinitize the newly created array with the current thread
    GCPROTECT_END();
}

#ifdef _DEBUG
//===============================================================================
// Code that insures that our unmanaged version of Nullable is consistant with
// the managed version Nullable<T> for all T.  

void Nullable::CheckFieldOffsets(TypeHandle nullableType) 
{
    LEAF_CONTRACT;

/***
        // The non-instantiated method tables like List<T> that are used
        // by reflection and verification do not have correct field offsets
        // but we never make instances of these anyway.
    if (nullableMT->ContainsGenericVariables())
        return;
***/

    MethodTable* nullableMT = nullableType.GetMethodTable();

        // insure that the managed version of the table is the same as the
        // unmanaged.  Note that we can't do this in mscorlib.h because this
        // class is generic and field layout depends on the instantiation.

    _ASSERTE(nullableMT->GetNumInstanceFields() == 2);
    FieldDesc* field = nullableMT->GetApproxFieldDescListRaw();

    _ASSERTE(strcmp(field->GetDebugName(), "hasValue") == 0);
//     _ASSERTE(field->GetOffset() == offsetof(Nullable, hasValue));
    field++;

    _ASSERTE(strcmp(field->GetDebugName(), "value") == 0);
//     _ASSERTE(field->GetOffset() == offsetof(Nullable, value));
}
#endif

//===============================================================================
// Returns true if nullableMT is Nullable<T> for T == paramMT

BOOL Nullable::IsNullableForTypeHelper(MethodTable* nullableMT, MethodTable* paramMT) 
{
    EEClass* cls = nullableMT->GetClass();
    if (!cls->HasSameTypeDefAs(g_pNullableEEClass))
		return FALSE;

        // we require an exact match of the parameter types 
    return TypeHandle(paramMT) == nullableMT->GetInstantiation()[0];
}
	
//===============================================================================
bool* Nullable::HasValueAddr(MethodTable* nullableMT) {

    WRAPPER_CONTRACT;

    _ASSERTE(strcmp(nullableMT->GetApproxFieldDescListRaw()[0].GetDebugName(), "hasValue") == 0);
    return (bool*) (((BYTE*) this) + nullableMT->GetApproxFieldDescListRaw()[0].GetOffset_NoLogging());
}

//===============================================================================
void* Nullable::ValueAddr(MethodTable* nullableMT) {

    WRAPPER_CONTRACT;

    _ASSERTE(strcmp(nullableMT->GetApproxFieldDescListRaw()[1].GetDebugName(), "value") == 0);
    return (((BYTE*) this) + nullableMT->GetApproxFieldDescListRaw()[1].GetOffset_NoLogging());
}

//===============================================================================
// Special Logic to box a nullable<T> as a boxed<T>

OBJECTREF Nullable::Box(void* srcPtr, MethodTable* nullableMT)
{
    CONTRACTL
    {
        THROWS;
        FAULT_NOT_FATAL();      // FIX_NOW: why do we need this?
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    Nullable* src = (Nullable*) srcPtr;

    _ASSERTE(IsNullableType(nullableMT));
        // We better have a concrete instantiation, or our field offset asserts are not useful
    _ASSERTE(!nullableMT->ContainsGenericVariables());

    if (!*src->HasValueAddr(nullableMT))
        return NULL;

    OBJECTREF obj = 0;
    GCPROTECT_BEGININTERIOR (src);
    MethodTable* argMT = nullableMT->GetInstantiation()[0].GetMethodTable();
    obj = argMT->Allocate();
    CopyValueClass(obj->UnBox(), src->ValueAddr(nullableMT), argMT, obj->GetAppDomain());
    GCPROTECT_END ();

    return obj;
}

//===============================================================================
// Special Logic to unbox a boxed T as a nullable<T>

BOOL Nullable::UnBox(void* destPtr, OBJECTREF boxedVal, MethodTable* destMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    Nullable* dest = (Nullable*) destPtr;

        // We should only get here if we are unboxing a T as a Nullable<T>
    _ASSERTE(IsNullableType(destMT));

        // We better have a concrete instantiation, or our field offset asserts are not useful
    _ASSERTE(!destMT->ContainsGenericVariables());

    if (boxedVal == NULL) 
    {
            // logicall we are doing *dest->HasValueAddr(destMT) = false;
            // We zero out the whole structure becasue it may contain GC references
            // and these need to be initialized to zero.   (could optimize in the non-GC case)
        memset(destPtr, 0, destMT->GetNumInstanceFieldBytes());
    }
    else 
    {
        if (!IsNullableForType(destMT, boxedVal->GetMethodTable())) {
            // For safety's sake, also allow true nullables to be unboxed normally.  
            // This should not happen normally, but we want to be robust
            if (destMT == boxedVal->GetMethodTable()) {
                CopyValueClass(dest, boxedVal->GetData(), destMT, boxedVal->GetAppDomain());
                return TRUE;
            }
            return FALSE;
        }

        *dest->HasValueAddr(destMT) = true;
        CopyValueClass(dest->ValueAddr(destMT), boxedVal->UnBox(), boxedVal->GetMethodTable(), boxedVal->GetAppDomain());
    }
    return TRUE;
}

//===============================================================================
// a boxed Nullable<T> should either be null or a boxed T, but sometimes it is
// useful to have a 'true' boxed Nullable<T> (that is it has two fields).  This
// function returns a 'normalized' version of this pointer.

OBJECTREF Nullable::NormalizeBox(OBJECTREF obj) {
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    if (obj != NULL) {
        MethodTable* retMT = obj->GetMethodTable();
        if (Nullable::IsNullableType(retMT)) 
            obj = Nullable::Box(obj->GetData(), retMT);
    }
    return obj;
}

#endif // #ifndef DACCESS_COMPILE


StackTraceElement const & StackTraceArray::operator[](size_t index) const
{
    WRAPPER_CONTRACT;
    return GetData()[index];
}

StackTraceElement & StackTraceArray::operator[](size_t index)
{
    WRAPPER_CONTRACT;
    return GetData()[index];
}
