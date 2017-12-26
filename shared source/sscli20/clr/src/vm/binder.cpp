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

#include "binder.h"
#include "ecall.h"

#include "field.h"
#include "excep.h"

#include "message.h"
#include "comnumber.h"
#include "eeconfig.h"
#include "rwlock.h"
#include "runtimehandles.h"
#include "customattribute.h"
#include "debugdebugger.h"

#ifndef DACCESS_COMPILE

void Binder::Init(Module *pModule,
                  const ClassDescription *pClassDescriptions,
                  DWORD cClassDescriptions,
                  const MethodDescription *pMethodDescriptions,
                  DWORD cMethodDescriptions,
                  const FieldDescription *pFieldDescriptions,
                  DWORD cFieldDescriptions,
                  const TypeDescription *pTypeDescriptions,
                  DWORD cTypeDescriptions)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    m_pModule = pModule;
    pModule->m_pBinder = this;

    m_classDescriptions = pClassDescriptions;
    m_methodDescriptions = pMethodDescriptions;
    m_fieldDescriptions = pFieldDescriptions;
    m_typeDescriptions = pTypeDescriptions;

    //
    // Initialize Class RID array
    //

    _ASSERTE(cClassDescriptions < USHRT_MAX);
    m_cClassRIDs = (USHORT) cClassDescriptions;
    m_pClassRIDs = new USHORT [m_cClassRIDs];
    ZeroMemory(m_pClassRIDs, m_cClassRIDs * sizeof(*m_pClassRIDs));

    //
    // Initialize Method RID array
    //

    _ASSERTE(cMethodDescriptions < USHRT_MAX);
    m_cMethodMDs = (USHORT) cMethodDescriptions;
    m_pMethodMDs = new MethodDesc * [m_cMethodMDs];
    ZeroMemory(m_pMethodMDs, m_cMethodMDs * sizeof(*m_pMethodMDs));

    
    //
    // Initialize Field RID array
    //

    _ASSERTE(cFieldDescriptions < USHRT_MAX);
    m_cFieldRIDs = (USHORT) cFieldDescriptions;
    m_pFieldRIDs = new USHORT [m_cFieldRIDs];
    ZeroMemory(m_pFieldRIDs, m_cFieldRIDs * sizeof(*m_pFieldRIDs));

    //
    // Initialize TypeHandle array
    //

    _ASSERTE(cTypeDescriptions < USHRT_MAX);
    m_cTypeHandles = (USHORT) cTypeDescriptions;
    m_pTypeHandles = new TypeHandle [m_cTypeHandles];
    ZeroMemory(m_pTypeHandles, m_cTypeHandles * sizeof(*m_pTypeHandles));
}

void Binder::Destroy()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END;
    
    if (m_pClassRIDs != NULL
        && !m_pModule->IsPersistedObject(m_pClassRIDs))
        delete [] m_pClassRIDs;

    if (m_pMethodMDs != NULL
        && !m_pModule->IsPersistedObject(m_pMethodMDs))
        delete [] m_pMethodMDs;

    if (m_pFieldRIDs != NULL
        && !m_pModule->IsPersistedObject(m_pFieldRIDs))
        delete [] m_pFieldRIDs;

    if (m_pTypeHandles != NULL)
        delete [] m_pTypeHandles;
}


#endif // #ifndef DACCESS_COMPILE

mdTypeDef Binder::GetTypeDef(BinderClassID id) 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());

        PRECONDITION(id != CLASS__NIL);
        PRECONDITION(id <= m_cClassRIDs);
    }
    CONTRACTL_END;

    if (m_pClassRIDs[id-1] == 0)
        LookupClass(id);
    return TokenFromRid(m_pClassRIDs[id-1], mdtTypeDef);
}

mdMethodDef Binder::GetMethodDef(BinderMethodID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());

        PRECONDITION(id != METHOD__NIL);
        PRECONDITION(id <= m_cMethodMDs);
    }
    CONTRACTL_END;

    if (m_pMethodMDs[id-1] == NULL)
        LookupMethod(id);
    return m_pMethodMDs[id-1]->GetMemberDef();
}

mdFieldDef Binder::GetFieldDef(BinderFieldID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());

        PRECONDITION(id != FIELD__NIL);
        PRECONDITION(id <= m_cFieldRIDs);
    }
    CONTRACTL_END;
    
    if (m_pFieldRIDs[id-1] == 0)
        LookupField(id);
    return TokenFromRid(m_pFieldRIDs[id-1], mdtFieldDef);
}

//
// Raw retrieve structures from ID.  Use these when 
// you don't care about class Restore or .cctors.
//

MethodTable *Binder::RawGetClass(BinderClassID id, ClassLoadLevel level)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(m_pModule));
        PRECONDITION(id != CLASS__NIL);
        PRECONDITION(id <= m_cClassRIDs);        
    }
    CONTRACTL_END;

    TypeHandle th = m_pModule->LookupTypeDef(TokenFromRid(m_pClassRIDs[id-1], mdtTypeDef), level);
    _ASSERTE(!th.IsNull());
    _ASSERTE(th.IsUnsharedMT());
    return th.AsMethodTable();
}

MethodDesc *Binder::RawGetMethod(BinderMethodID id)
{
    CONTRACT(MethodDesc*)
    {
        NOTHROW;            
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(m_pModule));
        PRECONDITION(id != METHOD__NIL);
        PRECONDITION(id <= m_cMethodMDs);

        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    MethodDesc *pMD = m_pMethodMDs[id - 1];

    RETURN pMD;
}

FieldDesc *Binder::RawGetField(BinderFieldID id)
{
    CONTRACT(FieldDesc*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
        
        PRECONDITION(CheckPointer(m_pModule));
        PRECONDITION(id != FIELD__NIL);
        PRECONDITION(id <= m_cFieldRIDs);    

        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
        
    const FieldDescription *f = m_fieldDescriptions + (id - 1);
    PREFIX_ASSUME(f != NULL);
    MethodTable *pMT = RawGetClass(f->classID, CLASS_LOAD_UNRESTOREDTYPEKEY);
    PREFIX_ASSUME(pMT != NULL);

    // Can't do this because the class may not be restored yet.
    // _ASSERTE(m_pFieldRIDs[id-1]-1 < (pMT->GetClass()->GetNumStaticFields() 
    //                                  + pMT->GetNumIntroducedInstanceFields()));

    g_IBCLogger.LogFieldDescsAccess(pMT->GetClass());


    FieldDesc *pFD = pMT->GetClass()->GetFieldDescListPtr() +
        (m_pFieldRIDs[id-1] - 1);
    
    RETURN pFD;
}

TypeHandle Binder::RawGetType(BinderTypeID id)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(m_pModule));
        PRECONDITION(id != TYPE__NIL);
        PRECONDITION(id <= m_cTypeHandles);
    }
    CONTRACTL_END;
    
    TypeHandle th = m_pTypeHandles[id-1];
    _ASSERTE(!th.IsNull());
    return th;
}

//
// Inline function to check a class for init & restore
//

inline void Binder::CheckInit(MethodTable *pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    if (!pMT->IsClassInited())
    {
#ifndef DACCESS_COMPILE
        InitClass(pMT);
#else
        DacNotImpl();
#endif
    }
}

//
// Fetch retrieve structures from ID - doesn't
// trigger class init
//

MethodTable *Binder::FetchClass(BinderClassID id, BOOL fLoad/* = TRUE*/, ClassLoadLevel level/* = CLASS_LOADED*/)
{
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) GC_NOTRIGGER; else GC_TRIGGERS;
        MODE_ANY;
        if (fLoad) SO_INTOLERANT; else SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(id != CLASS__NIL);
    _ASSERTE(id <= m_cClassRIDs);

    MethodTable *pMT;

    if (m_pClassRIDs[id-1] == 0)
    {
        pMT = LookupClass(id, fLoad, level);

        // If we're looking up the class because it wasn't in the class RID cache, then
        // it is possible that we won't load the type. While by definition we will always
        // have String and CriticalFinalizerObject loaded (we load them in
        // AppDomain::LoadBaseSystemClasses), new cases could be introduced so this is
        // just the safe thing to do.
        if (pMT == NULL)
        {
            return NULL;
        }

        // handle special classes
        switch (id) {
        case CLASS__STRING:
            // Strings are not "normal" objects, so we need to mess with their method table a bit
            // so that the GC can figure out how big each string is...
            pMT->SetBaseSize(ObjSizeOf(StringObject));
            pMT->SetComponentSize(2);
            break;

        case CLASS__CRITICAL_FINALIZER_OBJECT:
            // To introduce a class with a critical finalizer,
            // we'll explicitly load CriticalFinalizerObject and set the bit
            // here.
            pMT->SetHasCriticalFinalizer();
            break;

        default:
            break;
        }
    }
    else
    {
        pMT = RawGetClass(id, CLASS_LOAD_UNRESTOREDTYPEKEY);
        PREFIX_ASSUME(pMT != NULL);
        if (fLoad)
            ClassLoader::EnsureLoaded(pMT, level);
        g_IBCLogger.LogMethodTableAccess(pMT);
    }

    return pMT;
}

MethodDesc *Binder::FetchMethod(BinderMethodID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

    _ASSERTE(id != METHOD__NIL);
    _ASSERTE(id <= m_cMethodMDs);

    MethodDesc *pMD;

    if (m_pMethodMDs[id-1] == 0)
        pMD = LookupMethod(id);
    else
    {
        pMD = RawGetMethod(id);
        pMD->CheckRestore();

        g_IBCLogger.LogMethodDescAccess(pMD);
    }

    if (m_methodDescriptions[id-1].sig != NULL)
    {
        // Initialize the sig here where it's safe.  (Otherwise it would typically happen
        // during a MethodDesc::Call.)
        PCCOR_SIGNATURE pSig;
        DWORD cbSigSize;
        m_methodDescriptions[id-1].sig->GetBinarySig(&pSig, &cbSigSize);
    }

    return pMD;
}

FieldDesc *Binder::FetchField(BinderFieldID id)
{
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED() ) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED() ) GC_NOTRIGGER; else GC_TRIGGERS;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED() ) FORBID_FAULT; else INJECT_FAULT(ThrowOutOfMemory());
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(id != FIELD__NIL);
    _ASSERTE(id <= m_cFieldRIDs);

    FieldDesc *pFD;

    if (m_pFieldRIDs[id-1] == 0)
        pFD = LookupField(id);
    else
    {
        pFD = RawGetField(id);
        pFD->GetEnclosingMethodTable()->CheckRestore();
    }

    return pFD;
}

TypeHandle Binder::FetchType(BinderTypeID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());

        PRECONDITION(id != TYPE__NIL);
        PRECONDITION(id <= m_cTypeHandles);
    }
    CONTRACTL_END;
        
    TypeHandle th = m_pTypeHandles[id-1];
    if (th.IsNull())
        th = LookupType(id);

    return th;
}

//
// Normal retrieve structures from ID
//

MethodTable *Binder::GetClass(BinderClassID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE
    _ASSERTE (GetThread ());
    TRIGGERSGC ();
#ifdef STRESS_HEAP
    // Force a GC here because GetClass could trigger GC nondeterminsticly
    if (g_pConfig->GetGCStressLevel() != 0)
    {
        Thread * pThread = GetThread ();
        BOOL bInCoopMode = pThread->PreemptiveGCDisabled ();
        GCX_COOP ();
        if (bInCoopMode)
        {
            pThread->PulseGCMode ();
        }
    }
#endif //STRESS_HEAP
#endif //DACCESS_COMPILE
    MethodTable *pMT = FetchClass(id);

    CheckInit(pMT);

    return pMT;
}

MethodDesc *Binder::GetMethod(BinderMethodID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE
    _ASSERTE (GetThread ());
    TRIGGERSGC ();
#ifdef STRESS_HEAP
    // Force a GC here because GetMethod could trigger GC nondeterminsticly
    if (g_pConfig->GetGCStressLevel() != 0)
    {
        Thread * pThread = GetThread ();
        BOOL bInCoopMode = pThread->PreemptiveGCDisabled ();
        GCX_COOP ();
        if (bInCoopMode)
        {
            pThread->PulseGCMode ();
        }
    }
#endif //STRESS_HEAP
#endif //DACCESS_COMPILE

    MethodDesc *pMD = FetchMethod(id);

    CheckInit(pMD->GetMethodTable());

    // Record this method desc if required
    g_IBCLogger.LogMethodDescAccess(pMD);

    return pMD;
}

FieldDesc *Binder::GetField(BinderFieldID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE
    _ASSERTE (GetThread ());
    TRIGGERSGC ();
#ifdef STRESS_HEAP
    // Force a GC here because GetField could trigger GC nondeterminsticly
    if (g_pConfig->GetGCStressLevel() != 0)
    {
        Thread * pThread = GetThread ();
        BOOL bInCoopMode = pThread->PreemptiveGCDisabled ();
        GCX_COOP ();
        if (bInCoopMode)
        {
            pThread->PulseGCMode ();
        }
    }
#endif //STRESS_HEAP
#endif //DACCESS_COMPILE

    FieldDesc *pFD = FetchField(id);

    CheckInit(pFD->GetEnclosingMethodTable());

    return pFD;
}

TypeHandle Binder::GetType(BinderTypeID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE
    _ASSERTE (GetThread ());
    TRIGGERSGC ();
#ifdef STRESS_HEAP
    // Force a GC here because GetType could trigger GC nondeterminsticly
    if (g_pConfig->GetGCStressLevel() != 0)
    {
        Thread * pThread = GetThread ();
        BOOL bInCoopMode = pThread->PreemptiveGCDisabled ();
        GCX_COOP ();
        if (bInCoopMode)
        {
            pThread->PulseGCMode ();
        }
    }
#endif //STRESS_HEAP
#endif //DACCESS_COMPILE

    TypeHandle th = FetchType(id);

    return th;
}

//
// Method address - these could conceivably be implemented
// more efficiently than accessing the Desc info.
// 

const BYTE *Binder::GetMethodAddress(BinderMethodID id)
{ 
    WRAPPER_CONTRACT;
    
    return GetMethod(id)->GetAddrofCode(); 
}

//
// Offsets - these could conceivably be implemented
// more efficiently than accessing the Desc info.
// 

DWORD Binder::GetFieldOffset(BinderFieldID id)
{ 
    WRAPPER_CONTRACT;

    return FetchField(id)->GetOffset(); 
}

BOOL Binder::IsClass(MethodTable *pMT, BinderClassID id)
{
    CONTRACTL
    {
        GC_NOTRIGGER; 
        NOTHROW;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_pClassRIDs[id-1] == 0)
        return PTR_HOST_TO_TADDR(LookupClass(id, FALSE, CLASS_LOAD_UNRESTOREDTYPEKEY)) ==
            PTR_HOST_TO_TADDR(pMT);
    else
        return PTR_HOST_TO_TADDR(RawGetClass(id, CLASS_LOAD_UNRESTOREDTYPEKEY)) ==
            PTR_HOST_TO_TADDR(pMT);
}

BOOL Binder::IsType(TypeHandle th, BinderTypeID id)
{
    CONTRACTL
    {
        GC_NOTRIGGER; 
        NOTHROW;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (RawGetType(id) == th)
        return TRUE;
    if (!m_pTypeHandles[id-1].IsNull())
        return FALSE;
    else
        return LookupType(id, FALSE) == th;
}

#ifndef DACCESS_COMPILE

void Binder::InitClass(MethodTable *pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMT));
    }
    CONTRACTL_END;

    // Switch to cooperative GC mode before we manipulate any OBJECTREF's.
    GCX_COOP();
    pMT->CheckRunClassInitThrowing();
}

#endif // #ifndef DACCESS_COMPILE

MethodTable *Binder::LookupClass(BinderClassID id, BOOL fLoad, ClassLoadLevel level/* = CLASS_LOADED*/)
{
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;

        PRECONDITION(CheckPointer(m_pModule));
        PRECONDITION(id != CLASS__NIL);
        PRECONDITION(id <= m_cClassRIDs);
    }
    CONTRACTL_END;


    MethodTable *pMT;

    const ClassDescription *d = m_classDescriptions + (id - 1);
    PREFIX_ASSUME(d != NULL);
    if (!fLoad)
    {
        ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

        pMT = ClassLoader::LoadTypeByNameThrowing(m_pModule->GetAssembly(), d->namesp, d->name, 
                                                  ClassLoader::ReturnNullIfNotFound, 
                                                  // == FailIfNotLoadedOrNotRestored
                                                  ClassLoader::DontLoadTypes,
                                                  level).AsMethodTable();
        if (pMT == NULL)
            return NULL;
    }
    else
    {
        pMT = ClassLoader::LoadTypeByNameThrowing(m_pModule->GetAssembly(), d->namesp, d->name,
                                                  ClassLoader::ThrowIfNotFound, 
                                                  ClassLoader::LoadTypes,
                                                  level).AsMethodTable();
    }

    if (pMT == NULL)
        return NULL; // Not needed, but makes prefast happy


    _ASSERTE(pMT->GetModule() == m_pModule);

    mdTypeDef td = pMT->GetCl();

    _ASSERTE(!IsNilToken(td));

    _ASSERTE(RidFromToken(td) <= USHRT_MAX);
    m_pClassRIDs[id-1] = (USHORT) RidFromToken(td);

    return pMT;
}

MethodDesc *Binder::LookupMethod(BinderMethodID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(ThrowOutOfMemory());

        PRECONDITION(CheckPointer(m_pModule));
        PRECONDITION(id != METHOD__NIL);
        PRECONDITION(id <= m_cMethodMDs);
    }
    CONTRACTL_END;
    

    const MethodDescription *d = m_methodDescriptions + (id - 1);

    MethodTable *pMT = FetchClass(d->classID);

    MethodDesc *pMD = (d->sig != NULL) ? 
        pMT->GetClass()->FindMethod(d->name, d->sig) :
        pMT->GetClass()->FindMethodByName(d->name);

    PREFIX_ASSUME_MSGF(pMD != NULL, ("EE expects method to exist: %s:%s\n", pMT->GetDebugClassName(), d->name));

    _ASSERTE(pMD->GetSlot()+1 <= USHRT_MAX);
#ifndef DACCESS_COMPILE
    m_pMethodMDs[id-1] = pMD;
#endif

    return pMD;
}

FieldDesc *Binder::LookupField(BinderFieldID id)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(ThrowOutOfMemory());
        
        PRECONDITION(CheckPointer(m_pModule));
        PRECONDITION(id != FIELD__NIL);
        PRECONDITION(id <= m_cFieldRIDs);
    }
    CONTRACTL_END;

    const FieldDescription *d = m_fieldDescriptions + (id - 1);
    PREFIX_ASSUME(d != NULL);
    MethodTable *pMT = FetchClass(d->classID);

    FieldDesc *pFD;

    pFD = pMT->GetClass()->FindField(d->name, NULL, 0, NULL);

#ifndef DACCESS_COMPILE
    PREFIX_ASSUME_MSGF(pFD != NULL, ("EE expects field to exist: %s:%s\n", pMT->GetDebugClassName(), d->name));

    _ASSERTE(pFD - pMT->GetApproxFieldDescListRaw() >= 0);
    _ASSERTE(pFD - pMT->GetApproxFieldDescListRaw() < (pMT->GetNumStaticFields() 
                                                             + pMT->GetNumIntroducedInstanceFields()));
    _ASSERTE(pFD - pMT->GetApproxFieldDescListRaw() + 1 < USHRT_MAX);

    USHORT index = (USHORT)(pFD - pMT->GetApproxFieldDescListRaw());

    m_pFieldRIDs[id-1] = index+1;
#endif

    return pFD;
}

TypeHandle Binder::LookupType(BinderTypeID id, BOOL fLoad)
{
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        if (! fLoad) SO_TOLERANT; else SO_INTOLERANT;
        MODE_ANY;

        PRECONDITION(CheckPointer(m_pModule));
        PRECONDITION(id != TYPE__NIL);
        PRECONDITION(id <= m_cTypeHandles);
    }
    CONTRACTL_END;
    
    const TypeDescription *d = m_typeDescriptions + (id - 1);

    TypeHandle th = m_pTypeHandles[id-1];

    if (!th.IsNull())
        return th;

    if (!fLoad)
    {

        MethodTable *mt = LookupClass(d->classID, FALSE);

        if (mt == NULL)
            return TypeHandle();

        {
            ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();
            th = ClassLoader::LoadArrayTypeThrowing(TypeHandle(mt), d->type, d->rank, ClassLoader::LoadTypes);
        }

    }
    else 
    {
        th = ClassLoader::LoadArrayTypeThrowing(TypeHandle(GetClass(d->classID)), d->type, d->rank);
    }
    if (th.IsNull())
        return TypeHandle();

#ifndef DACCESS_COMPILE
    m_pTypeHandles[id-1] = th;
#endif

    return th;

}

BOOL Binder::IsException(MethodTable *pMT, RuntimeExceptionKind kind)
{
    WRAPPER_CONTRACT;
    return IsClass(pMT, (BinderClassID) (kind + CLASS__MSCORLIB_COUNT));
}

MethodTable *Binder::GetException(RuntimeExceptionKind kind)
{
    WRAPPER_CONTRACT;
    
    return GetClass((BinderClassID) (kind + CLASS__MSCORLIB_COUNT));
}

MethodTable *Binder::FetchException(RuntimeExceptionKind kind)
{
    WRAPPER_CONTRACT;
    return FetchClass((BinderClassID) (kind + CLASS__MSCORLIB_COUNT));
}

LPCUTF8 Binder::GetExceptionName(RuntimeExceptionKind kind)
{ 
    WRAPPER_CONTRACT;
    return GetClassName((BinderClassID) (kind + CLASS__MSCORLIB_COUNT));
}


BOOL Binder::IsElementType(MethodTable *pMT, CorElementType type)
{
    WRAPPER_CONTRACT;
    BinderClassID id = (BinderClassID) (type + CLASS__MSCORLIB_COUNT + kLastException);

    _ASSERTE(GetClassName(id) != NULL);

    return IsClass(pMT, id);
}

MethodTable *Binder::GetElementType(CorElementType type)
{
    WRAPPER_CONTRACT;
    
    BinderClassID id = (BinderClassID) (type + CLASS__MSCORLIB_COUNT + kLastException);

    _ASSERTE(GetClassName(id) != NULL);

    return GetClass(id);
}

MethodTable *Binder::FetchElementType(CorElementType type, BOOL fLoad/* = TRUE*/, ClassLoadLevel level/* = CLASS_LOADED*/)
{
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED() || !fLoad) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END;
    
    BinderClassID id = (BinderClassID) (type + CLASS__MSCORLIB_COUNT + kLastException);

    _ASSERTE(GetClassName(id) != NULL);

    return FetchClass(id, fLoad, level);
}

MethodTable *Binder::LookupElementType(CorElementType type, ClassLoadLevel level /* = CLASS_LOADED */)
{
    WRAPPER_CONTRACT;

    BinderClassID id = (BinderClassID) (type + CLASS__MSCORLIB_COUNT + kLastException);

    return LookupClass(id, FALSE, level);
}


LPCUTF8 Binder::GetElementTypeName(CorElementType type)
{ 
    WRAPPER_CONTRACT;

    return GetClassName((BinderClassID) (type + CLASS__MSCORLIB_COUNT + kLastException));
}

///////////////////////////////////////////////////////////////////////////////
// Mscorlib:
///////////////////////////////////////////////////////////////////////////////

// For the dac compile we only need the array size

#define CountOfMscorlibClassDescriptions                                        \
    ((CLASS__MSCORLIB_COUNT - 1) + kLastException + ELEMENT_TYPE_MAX)

#define CountOfMscorlibMethodDescriptions   (METHOD__MSCORLIB_COUNT - 1)
#define CountOfMscorlibFieldDescriptions    (FIELD__MSCORLIB_COUNT - 1)
#define CountOfMscorlibTypeDescriptions     (TYPE__MSCORLIB_COUNT - 1)

#ifndef DACCESS_COMPILE

#include "nativeoverlapped.h"

const Binder::ClassDescription Binder::MscorlibClassDescriptions[] =
{
#define DEFINE_CLASS(i,n,s)        { PTR_CSTR((TADDR) g_ ## n ## NS ),  PTR_CSTR((TADDR) # s ) },
#include "mscorlib.h"

    // Include all exception types here
#define EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr) { PTR_CSTR((TADDR)ns) , PTR_CSTR((TADDR) # reKind ) },
#define EXCEPTION_ADD_HR(hr)
#define EXCEPTION_END_DEFINE()
#include "rexcep.h"
#undef EXCEPTION_BEGIN_DEFINE
#undef EXCEPTION_ADD_HR
#undef EXCEPTION_END_DEFINE

    // Now include all signature types

#define TYPEINFO(e,ns,c,s,g,ia,ip,if,im)   { PTR_CSTR((TADDR)ns) , PTR_CSTR((TADDR)c) },
#include "cortypeinfo.h"
#undef TYPEINFO
};

#define gsig_NoSig (*(char*)NULL)

const Binder::MethodDescription Binder::MscorlibMethodDescriptions[] =
{
#define DEFINE_METHOD(c,i,s,g)          { CLASS__ ## c , PTR_CUTF8((TADDR) # s ), PTR_HARDCODEDMETASIG((TADDR) & gsig_ ## g ) },
#include "mscorlib.h"
} ;


const Binder::FieldDescription Binder::MscorlibFieldDescriptions[] =
{
#define DEFINE_FIELD(c,i,s)           { CLASS__ ## c , PTR_CUTF8((TADDR) # s ) },
#ifdef _DEBUG
#define DEFINE_FIELD_U(c,i,s,uc,uf)   { CLASS__ ## c , PTR_CUTF8((TADDR) # s ) },
#endif // _DEBUG
#include "mscorlib.h"
};

const Binder::TypeDescription Binder::MscorlibTypeDescriptions[] = 
{
    { CLASS__BYTE,          ELEMENT_TYPE_SZARRAY,       1 },
    { CLASS__OBJECT,        ELEMENT_TYPE_SZARRAY,       1 },
    { CLASS__VARIANT,       ELEMENT_TYPE_SZARRAY,       1 },
    { CLASS__VOID,          ELEMENT_TYPE_PTR,           0 },
};

#endif // DACCESS_COMPILE

#ifndef DACCESS_COMPILE
#ifdef _DEBUG

const Binder::FieldOffsetCheck Binder::MscorlibFieldOffsets[] =
{
#define DEFINE_FIELD_U(c,i,s,uc,uf)  { FIELD__ ## c ## __ ## i, offsetof(uc, uf), sizeof(((uc*)1)->uf) },
#include "mscorlib.h"
    { (BinderFieldID) 0 }
};

const Binder::ClassSizeCheck Binder::MscorlibClassSizes[] =
{
#define DEFINE_CLASS_U(i,n,s,uc)  { CLASS__ ## i, sizeof(uc) },
#include "mscorlib.h"
    { (BinderClassID) 0 }
};

//
// check the basic consistency between mscorlib and mscorwks
//
void Binder::CheckMscorlib()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

    const FieldOffsetCheck     *pOffsets = MscorlibFieldOffsets;

    while (pOffsets->fieldID != FIELD__NIL)
    {
        FieldDesc *pFD = g_Mscorlib.FetchField(pOffsets->fieldID);
        DWORD offset = pFD->GetOffset();

        if (!pFD->IsFieldOfValueType())
        {
            offset += Object::GetOffsetOfFirstField();
        }

        CONSISTENCY_CHECK_MSGF(offset == pOffsets->expectedOffset, 
            ("Managed class field offset does not match unmanaged class field offset\n"
             "man: 0x%x, unman: 0x%x, Name: %s\n", offset, pOffsets->expectedOffset, pFD->GetName()));

        DWORD size = pFD->LoadSize();

        CONSISTENCY_CHECK_MSGF(size == pOffsets->expectedSize, 
            ("Managed class field size does not match unmanaged class field size\n"
             "man: 0x%x, unman: 0x%x, Name: %s\n", size, pOffsets->expectedSize, pFD->GetName()));

        pOffsets++;
    }

    const ClassSizeCheck     *pSizes = MscorlibClassSizes;

    while (pSizes->classID != CLASS__NIL)
    {
        MethodTable *pMT = g_Mscorlib.FetchClass(pSizes->classID);

        // hidden size of the type that participates in the allignment calculation
        DWORD hiddenSize = pMT->IsValueType() ? sizeof(MethodTable*) : 0;

        DWORD size = pMT->GetBaseSize() - (sizeof(ObjHeader)+hiddenSize);

        DWORD expectedsize = (DWORD)ALIGN_UP(pSizes->expectedSize + (sizeof(ObjHeader) + hiddenSize),
            DATA_ALIGNMENT) - (sizeof(ObjHeader) + hiddenSize);

        CONSISTENCY_CHECK_MSGF(size == expectedsize,
            ("Managed object size does not match unmanaged object size\n"
            "man: 0x%x, unman: 0x%x, Name: %s\n", size, expectedsize, pMT->GetDebugClassName()));
        pSizes++;
    }
}

//
// check consistency of the unmanaged and managed fcall signatures
//
/* static */ FCSigCheck* FCSigCheck::g_pFCSigCheck;

static void FCallCheckSignature(MethodDesc* pMD, LPVOID pImpl, BOOL* pfDuplicateImplementations)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    char* pUnmanagedSig = NULL;

    FCSigCheck* pSigCheck = FCSigCheck::g_pFCSigCheck;
    while (pSigCheck != NULL)
    {
        if (pImpl == pSigCheck->func) {
            if (pUnmanagedSig != NULL)
            {
                // There are multiple fcalls with identical entrypoints. Skip signature check now
                // to give a better assert later.
                *pfDuplicateImplementations = TRUE;
                return;
            }
            pUnmanagedSig = pSigCheck->signature;
        }
        pSigCheck = pSigCheck->next;
    }

    MetaSig msig(pMD);   
    int argIndex = -2; // start with return value
    char* pUnmanagedArg = pUnmanagedSig;
    for (;;)
    {
        CorElementType argType = ELEMENT_TYPE_END;
        TypeHandle argTypeHandle;

        if (argIndex == -2)
        {
            // return value
            argType = msig.GetReturnType();
            if (argType == ELEMENT_TYPE_VALUETYPE)
                argTypeHandle = msig.GetRetTypeHandleThrowing();
        }

        if (argIndex == -1)
        {
            // this ptr
            if (msig.HasThis())
                argType = ELEMENT_TYPE_CLASS;
            else
                argIndex++; // move on to the first argument
        }

        if (argIndex >= 0)
        {
            argType = msig.NextArg();
            if (argType == ELEMENT_TYPE_END)
                break;
            if (argType == ELEMENT_TYPE_VALUETYPE)
                argTypeHandle = msig.GetLastTypeHandleThrowing();
        }

        const char* expectedType = NULL;

        switch (argType)
        {
        case ELEMENT_TYPE_VOID:
            expectedType = pMD->IsCtor() ? NULL : "void";
            break;
        case ELEMENT_TYPE_BOOLEAN:
            expectedType = (argIndex == -2) ? "FC_BOOL_RET" : "CLR_BOOL";
            break;
        case ELEMENT_TYPE_CHAR:
            expectedType = (argIndex == -2) ? "FC_CHAR_RET" : "CLR_CHAR";
            break;
        case ELEMENT_TYPE_I1:
            expectedType = (argIndex == -2) ? "FC_INT8_RET" : "INT8";
            break;
        case ELEMENT_TYPE_U1:
            expectedType = (argIndex == -2) ? "FC_UINT8_RET" : "UINT8";
            break;
        case ELEMENT_TYPE_I2:
            expectedType = (argIndex == -2) ? "FC_INT16_RET" : "INT16";
            break;
        case ELEMENT_TYPE_U2:
            expectedType = (argIndex == -2) ? "FC_UINT16_RET" : "UINT16";
            break;
        // case ELEMENT_TYPE_I4:
        //     expectedType = "INT32";
        //     break;
        // case ELEMENT_TYPE_U4:
        //     expectedType = "UINT32";
        //     break;
        case ELEMENT_TYPE_I8:
            expectedType = (argIndex == -2) ? "INT64" : "VINT64";
            break;
        case ELEMENT_TYPE_U8:
            expectedType = (argIndex == -2) ? "UINT64" : "VUINT64";
            break;
        case ELEMENT_TYPE_R4:
            expectedType = (argIndex == -2) ? "float" : "Vfloat";
            break;
        case ELEMENT_TYPE_R8:
            expectedType = (argIndex == -2) ? "double" : "Vdouble";
            break;
        case ELEMENT_TYPE_VALUETYPE:
            {
                _ASSERTE(!argTypeHandle.IsNull());
                
                StackSString ssArgTypeName;
                argTypeHandle.GetName(ssArgTypeName);

                // These types are special cased as ELEMENT_TYPE_I in class.cpp
                if ((ssArgTypeName.Equals(SL(L"System.RuntimeTypeHandle"))) ||
                    (ssArgTypeName.Equals(SL(L"System.RuntimeArgumentHandle"))) ||
                    (ssArgTypeName.Equals(SL(L"System.RuntimeMethodHandle"))) ||
                    (ssArgTypeName.Equals(SL(L"System.RuntimeFieldHandle"))))
                    break;

                // If the verifier type for a small valuetype is not a valuetype, it means that on
                // all !VALUETYPES_BY_REFERENCE platforms this argument gets passed directly
                // but on this platform it is always BYREF, so there's likely to be a mismatch.
                CONSISTENCY_CHECK_MSGF(
                        (argTypeHandle.GetSize() >= sizeof(ARG_SLOT))
                        || (argTypeHandle.GetVerifierCorElementType() != ELEMENT_TYPE_VALUETYPE),
                    ("The fcall signature contains small value type that is likely to be source of portability problems\n"
                    "Method: %s:%s. Argument: %d\n", pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName, argIndex));
            }
            break;

        default:
            // no checks for other types
            break;
        }

        if (pUnmanagedSig != NULL)
        {
            CONSISTENCY_CHECK_MSGF(pUnmanagedArg != NULL,
                ("Unexpected end of managed fcall signature\n"
                "Method: %s:%s\n", pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName));

            char* pUnmanagedArgEnd = strchr(pUnmanagedArg, ',');

            char* pUnmanagedTypeEnd = (pUnmanagedArgEnd != NULL) ? 
                pUnmanagedArgEnd : (pUnmanagedArg + strlen(pUnmanagedArg));

            if (argIndex != -2)
            {
                // skip argument name
                while(pUnmanagedTypeEnd > pUnmanagedArg) 
                {
                    char c = *(pUnmanagedTypeEnd-1);
                    if ((c != '_') 
                        && ((c < '0') || ('9' < c)) 
                        && ((c < 'a') || ('z' < c)) 
                        && ((c < 'A') || ('Z' < c)))
                        break;
                    pUnmanagedTypeEnd--;
                }
            }

            // skip whitespaces
            while(pUnmanagedTypeEnd > pUnmanagedArg) 
            {
                char c = *(pUnmanagedTypeEnd-1);
                if ((c != 0x20) && (c != '\t') && (c != '\n') && (c != '\r'))
                    break;
                pUnmanagedTypeEnd--;
            }

            if (expectedType != NULL)
            {
                size_t len = pUnmanagedTypeEnd - pUnmanagedArg;
                CONSISTENCY_CHECK_MSGF(strlen(expectedType) == len && SString::_strnicmp(expectedType, pUnmanagedArg, (COUNT_T)len) == 0,
                    ("The managed and unmanaged fcall signatures do not match\n"
                    "Method: %s:%s. Argument: %d Expecting: %s\n", pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName, argIndex, expectedType));
            }
            pUnmanagedArg = (pUnmanagedArgEnd != NULL) ? (pUnmanagedArgEnd+1) : NULL;
        }

        argIndex++;
    }

    if (pUnmanagedSig != NULL)
    {
        if (msig.IsVarArg())
        {
            CONSISTENCY_CHECK_MSGF((pUnmanagedArg != NULL) && strcmp(pUnmanagedArg, "...") == 0, 
                ("Expecting varargs in unmanaged fcall signature\n"
                "Method: %s:%s\n", pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName));
        }
        else
        {
            CONSISTENCY_CHECK_MSGF(pUnmanagedArg == NULL,
                ("Unexpected end of unmanaged fcall signature\n"
                "Method: %s:%s\n", pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName));
        }
    }
}

struct FCImplEntry
{
    LPVOID        m_pImplementation;
    MethodDesc *  m_pMD;
};

static int __cdecl fcImplEntryCmp(const void* a_, const void* b_)
{
    LEAF_CONTRACT;

    FCImplEntry *a = (FCImplEntry *)a_;
    FCImplEntry *b = (FCImplEntry *)b_;
    return(int)((size_t)a->m_pImplementation - (size_t)b->m_pImplementation);
}

//
// extended check of consistency between mscorlib and mscorwks:
//  - verifies that all references from mscorlib to mscorwks are present
//  - verifies that all references from mscorwks to mscorlib are present
//  - limited detection of mismatches between managed and unmanaged fcall signatures
//
void Binder::CheckMscorlibExtended()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

    // check the consistency of BCL and VM
    // note: it is not enabled by default because of it is time consuming and 
    // changes the bootstrap sequence of the EE
    if (!g_pConfig->GetConfigDWORD(L"ConsistencyCheck", 0))
        return;

    //
    // VM referencing BCL (mscorlib.h)
    //
    for (BinderClassID cID = (BinderClassID) 1; cID <= g_Mscorlib.m_cClassRIDs; cID = (BinderClassID) (cID + 1)) {
        if (g_Mscorlib.GetClassName(cID) != NULL) // Allow for CorSigElement entries with no classes
            g_Mscorlib.FetchClass(cID);
    }

    for (BinderMethodID mID = (BinderMethodID) 1; mID <= g_Mscorlib.m_cMethodMDs; mID = (BinderMethodID) (mID + 1))
        g_Mscorlib.FetchMethod(mID);

    for (BinderFieldID fID = (BinderFieldID) 1; fID <= g_Mscorlib.m_cFieldRIDs; fID = (BinderFieldID) (fID + 1))
        g_Mscorlib.FetchField(fID);

    //
    // BCL referencing VM (ecall.cpp)
    //
    HRESULT hr = S_OK;
    Module *pModule = g_Mscorlib.m_pModule;
    IMDInternalImport *pInternalImport = pModule->GetMDImport();

    HENUMInternal hEnum;
    LPVOID pImplementation = NULL;    
    const DWORD dwMaxNumEntries = 2000;
    DWORD dwNumEntries = 0; 
    NewArrayHolder<FCImplEntry> pSortedEntries(NULL); 
    BOOL fDuplicateImplementations = FALSE;
    
    // for all methods...
    IfFailGo(pInternalImport->EnumAllInit(mdtMethodDef, &hEnum));

    // Allocate a temporary array to store the static FCall entry points.
    pSortedEntries = new FCImplEntry[dwMaxNumEntries];

    for (;;) {
        mdTypeDef td;
        mdTypeDef tdClass;
        DWORD dwImplFlags;

        if (!pInternalImport->EnumNext(&hEnum, &td))
            break;

        pInternalImport->GetMethodImplProps(td, NULL, &dwImplFlags);

        // ... that are internal calls ...
        if (!IsMiInternalCall(dwImplFlags))
            continue;

        IfFailGo(pInternalImport->GetParentToken(td, &tdClass));

        TypeHandle type;

        HRESULT hr = ClassLoader::LoadTypeDefOrRefNoThrow(pModule, tdClass, &type, NULL, 
                                                          ClassLoader::ThrowIfNotFound, 
                                                          ClassLoader::FailIfUninstDefOrRef);
        if (FAILED(hr) || type.IsNull()) {
            LPCUTF8 pszClassName;
            LPCUTF8 pszNameSpace;
            pInternalImport->GetNameOfTypeDef(tdClass, &pszClassName, &pszNameSpace);
            CONSISTENCY_CHECK_MSGF(false, ("Unable to load class from mscorlib: %s.%s\n", pszNameSpace, pszClassName));
        }      

        MethodDesc *pMD = type.AsMethodTable()->GetClass()->FindMethod(td);
        _ASSERTE(pMD);

        // Get the implementation entrypoint and store it in the array
        pImplementation = ECall::GetStaticFCallImpl(pMD);
        if (pImplementation) {
            if (dwNumEntries >= dwMaxNumEntries) {
                _ASSERTE(!"dwMaxNumEntries too small");
                ThrowHR(E_FAIL);
            }
            pSortedEntries[dwNumEntries].m_pImplementation = pImplementation;
            pSortedEntries[dwNumEntries].m_pMD = pMD;
            ++dwNumEntries;
        }
        
        DWORD id = ECall::GetIDForMethod(pMD);

        // ... check that the method is in the fcall table.
        if (id == 0) {
            LPCUTF8 pszClassName;
            LPCUTF8 pszNameSpace;
            pInternalImport->GetNameOfTypeDef(tdClass, &pszClassName, &pszNameSpace);
            LPCUTF8 pszName = pInternalImport->GetNameOfMethodDef(td);
            CONSISTENCY_CHECK_MSGF(false, ("Unable to find internalcall implementation: %s.%s::%s\n", pszNameSpace, pszClassName, pszName));
        }
        else {
            FCallCheckSignature(pMD, ECall::GetFCallImpl(pMD), &fDuplicateImplementations);
        }
    }

    pInternalImport->EnumClose(&hEnum);

    //
    // Sort the static FCall entrypoints, and check for duplication.
    //
    _ASSERTE(dwNumEntries>=2);    // We should at least find 2 static FCall entries.
    qsort (pSortedEntries,                // start of array
           dwNumEntries,                  // array size in elements
           sizeof(FCImplEntry),           // element size in bytes
           fcImplEntryCmp);               // comparere function
      
    for (DWORD i=1; i<dwNumEntries; i++)
    {
        if (pSortedEntries[i].m_pImplementation == pSortedEntries[i-1].m_pImplementation)
        {
            // The fcall entrypoints has to be at unique addresses. If you get failure here, use the following steps
            // to fix it:
            // 1. Consider merging the offending fcalls into one fcall. Do they really do different things?
            // 2. If it does not make sense to merge the offending fcalls into one,
            // add FCUnique(<a random unique number here>); to one of the offending fcalls.
  
            // Attach debugger to see the names of the offending methods in debug output.
            _ASSERTE(!"Duplicate pImplementation entries found in reverse fcall table");

            MethodDesc* pMD1 = pSortedEntries[i-1].m_pMD;
            LPCUTF8 pszMethodName1 = pMD1->GetName();
            LPCUTF8 pszNamespace1 = 0;
            LPCUTF8 pszName1 = pMD1->GetClass()->GetFullyQualifiedNameInfo(&pszNamespace1);
  
            MethodDesc* pMD2 = pSortedEntries[i].m_pMD;
            LPCUTF8 pszMethodName2 = pMD2->GetName();
            LPCUTF8 pszNamespace2 = 0;
            LPCUTF8 pszName2 = pMD2->GetClass()->GetFullyQualifiedNameInfo(&pszNamespace2);
  
            CONSISTENCY_CHECK_MSGF(false, ("Duplicate implementation found: %s.%s::%s and %s.%s::%s\n", 
                pszNamespace1, pszName1, pszMethodName1, pszNamespace2, pszName2, pszMethodName2));

            fDuplicateImplementations = TRUE;
        }
    }

    // Make sure that nothing slipped through the cracks from FCallCheckSignature
    if (fDuplicateImplementations)
    {
        _ASSERTE(!"Duplicate FCALL implementations");
        ThrowHR(E_FAIL);
    }

    // Verify that there are no unused entries in the ecall table
    if (!ECall::CheckUnusedFCalls())
    {
        ThrowHR(E_FAIL);
    }

    //
    // Stub constants
    //
#define ASMCONSTANTS_C_ASSERT(cond)
#define ASMCONSTANTS_RUNTIME_ASSERT(cond) _ASSERTE(cond)
#include "asmconstants.h"



ErrExit:
    _ASSERTE(SUCCEEDED(hr));
}

#endif // _DEBUG

#endif // #ifndef DACCESS_COMPILE

GVAL_IMPL(Binder, g_Mscorlib);

#ifndef DACCESS_COMPILE

void Binder::StartupMscorlib(Module *pModule)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    C_ASSERT(CLASS__MSCORLIB_COUNT < USHRT_MAX);
    C_ASSERT(METHOD__MSCORLIB_COUNT < USHRT_MAX);
    C_ASSERT(FIELD__MSCORLIB_COUNT < USHRT_MAX);
    C_ASSERT(TYPE__MSCORLIB_COUNT < USHRT_MAX);

    C_ASSERT(CountOfMscorlibClassDescriptions == NumItems(MscorlibClassDescriptions));
    C_ASSERT(CountOfMscorlibMethodDescriptions == NumItems(MscorlibMethodDescriptions));
    C_ASSERT(CountOfMscorlibFieldDescriptions == NumItems(MscorlibFieldDescriptions));
    C_ASSERT(CountOfMscorlibTypeDescriptions == NumItems(MscorlibTypeDescriptions));

    {
        g_Mscorlib.Init(pModule,
                        MscorlibClassDescriptions,
                        NumItems(MscorlibClassDescriptions),
                        MscorlibMethodDescriptions,
                        NumItems(MscorlibMethodDescriptions),
                        MscorlibFieldDescriptions,
                        NumItems(MscorlibFieldDescriptions),
                        MscorlibTypeDescriptions,
                        NumItems(MscorlibTypeDescriptions));
    }
}

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void
Binder::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();

    DacEnumMemoryRegion(PTR_TO_TADDR(m_classDescriptions),
                        CountOfMscorlibClassDescriptions * sizeof(ClassDescription));
    DacEnumMemoryRegion(PTR_TO_TADDR(m_methodDescriptions),
                        CountOfMscorlibMethodDescriptions * sizeof(MethodDescription));
    DacEnumMemoryRegion(PTR_TO_TADDR(m_fieldDescriptions),
                        CountOfMscorlibFieldDescriptions * sizeof(FieldDescription));
    DacEnumMemoryRegion(PTR_TO_TADDR(m_typeDescriptions),
                        CountOfMscorlibTypeDescriptions * sizeof(TypeDescription));

    if (m_pModule.IsValid())
    {
        m_pModule->EnumMemoryRegions(flags, true);
    }

    DacEnumMemoryRegion(PTR_TO_TADDR(m_pClassRIDs),
                        m_cClassRIDs * sizeof(USHORT));
    DacEnumMemoryRegion(PTR_TO_TADDR(m_pFieldRIDs),
                        m_cFieldRIDs * sizeof(USHORT));
    DacEnumMemoryRegion(PTR_TO_TADDR(m_pMethodMDs),
                        m_cMethodMDs * sizeof(PTR_MethodDesc));
    DacEnumMemoryRegion(PTR_TO_TADDR(m_pTypeHandles),
                        m_cTypeHandles * sizeof(TypeHandle));
}

#endif // #ifdef DACCESS_COMPILE
