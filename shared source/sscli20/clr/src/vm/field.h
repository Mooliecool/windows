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
// COM+ Data Field Abstraction
// 

#ifndef _FIELD_H_
#define _FIELD_H_

#include "objecthandle.h"
#include "excep.h"

// Temporary values stored in FieldDesc m_dwOffset during loading
// The high 5 bits must be zero (because in field.h we steal them for other uses), so we must choose values > 0
#define FIELD_OFFSET_MAX              ((1<<27)-1)
#define FIELD_OFFSET_UNPLACED         FIELD_OFFSET_MAX
#define FIELD_OFFSET_UNPLACED_GC_PTR  (FIELD_OFFSET_MAX-1)
#define FIELD_OFFSET_VALUE_CLASS      (FIELD_OFFSET_MAX-2)
#define FIELD_OFFSET_NOT_REAL_FIELD   (FIELD_OFFSET_MAX-3)

// Offset to indicate an EnC added field. They don't have offsets as aren't placed in the object.
#define FIELD_OFFSET_NEW_ENC          (FIELD_OFFSET_MAX-4)
#define FIELD_OFFSET_BIG_RVA          (FIELD_OFFSET_MAX-5)
#define FIELD_OFFSET_LAST_REAL_OFFSET (FIELD_OFFSET_MAX-6)    // real fields have to be smaller than this


//
// This describes a field - one of this is allocated for every field, so don't make this structure any larger.
//
// @GENERICS: 
// Field descriptors for fields in instantiated types may be shared between compatible instantiations
// Hence for reflection it's necessary to pair a field desc with the exact owning type handle
class FieldDesc
{
    friend class MethodTableBuilder;

  protected:
    PTR_MethodTable m_pMTOfEnclosingClass;  // This is used to hold the log2 of the field size temporarily during class loading.  Yuck.

    // struct {
        unsigned m_mb               : 24;

        // 8 bits...
        unsigned m_isStatic         : 1;
        unsigned m_isThreadLocal    : 1;
        unsigned m_isContextLocal   : 1;
        unsigned m_isRVA            : 1;
        unsigned m_prot             : 3;
        unsigned m_isDangerousAppDomainAgileField : 1; // Note: this is used in checked only
    // };

    // struct {
        // Note: this has been as low as 22 bits in the past & seemed to be OK.
        // we can steal some more bits here if we need them.
        unsigned m_dwOffset         : 27;
        unsigned m_type             : 5;
    // };

#ifdef _DEBUG
    LPUTF8 m_debugName;
#endif

    // Allocated by special heap means, don't construct me
    FieldDesc() {};

  public:
#ifdef _DEBUG
public:
    inline LPUTF8 GetDebugName()
    {
        LEAF_CONTRACT;
        return m_debugName;
    }
#endif // _DEBUG
    
#ifndef DACCESS_COMPILE
    // This should be called.  It was added so that Reflection
    // can create FieldDesc's for the static primitive fields that aren't
    // stored with the EEClass.
    void SetMethodTable(MethodTable* mt)
    {
        LEAF_CONTRACT;
        m_pMTOfEnclosingClass = mt;
    }
#endif

    VOID Init(mdFieldDef mb, 
              CorElementType FieldType, 
              DWORD dwMemberAttrs, 
              BOOL fIsStatic, 
              BOOL fIsRVA, 
              BOOL fIsThreadLocal, 
              BOOL fIsContextLocal, 
              LPCSTR pszFieldName);

    mdFieldDef GetMemberDef()
    {
        WRAPPER_CONTRACT;

        return TokenFromRid(m_mb, mdtFieldDef);
    }

    CorElementType GetFieldType()
    {
        LEAF_CONTRACT;

        return (CorElementType) m_type;
    }

    DWORD GetFieldProtection()
    {
        LEAF_CONTRACT;

        return m_prot;
    }

        // Please only use this in a path that you have already guarenteed
        // the assert is true
    DWORD GetOffsetUnsafe()
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;

        g_IBCLogger.LogFieldDescsAccess(m_pMTOfEnclosingClass->GetClass_NoLogging());
        _ASSERTE(m_dwOffset <= FIELD_OFFSET_LAST_REAL_OFFSET);
        return m_dwOffset;
    }

    DWORD GetOffset()
    {
        WRAPPER_CONTRACT;

        g_IBCLogger.LogFieldDescsAccess(m_pMTOfEnclosingClass->GetClass_NoLogging());
        return GetOffset_NoLogging();
    }

    // During class load m_pMTOfEnclosingClass has the field size in it, so it has to use this version of
    // GetOffset during that time
    DWORD GetOffset_NoLogging()
    {
        WRAPPER_CONTRACT;

        // Note FieldDescs are no longer on "hot" paths so the optimized code here
        // does not look necessary.

        if (m_dwOffset != FIELD_OFFSET_BIG_RVA) {
            return m_dwOffset;
        }

        return OutOfLine_BigRVAOffset();
    }

    DWORD OutOfLine_BigRVAOffset()
    {
        LEAF_CONTRACT;

        DWORD   rva;

        _ASSERTE(m_dwOffset == FIELD_OFFSET_BIG_RVA);

        // <NICE>I'm discarding a potential error here.  According to the code in MDInternalRO.cpp,
        // we won't get an error if we initially found the RVA.  So I'm going to just
        // assert it never happens.
        //
        // This is a small sin, but I don't see a good alternative. --cwb.</NICE>
        HRESULT hr;
        hr = GetMDImport()->GetFieldRVA(GetMemberDef(), &rva); 
        _ASSERTE(SUCCEEDED(hr));

        return rva;
    }

    HRESULT SetOffset(DWORD dwOffset)
    {
        LEAF_CONTRACT;

        //
        // value class fields must be aligned to pointer-sized boundaries
        //
        //
        // This is commented out because it isn't valid in all cases.
        // This is still here because it is useful for finding alignment
        // problems on IA64.
        //
        //_ASSERTE((dwOffset > FIELD_OFFSET_LAST_REAL_OFFSET)  ||
        //         (ELEMENT_TYPE_VALUETYPE != GetFieldType()) ||
        //         (IS_ALIGNED(dwOffset, sizeof(void*))));

        m_dwOffset = dwOffset;
        return((dwOffset > FIELD_OFFSET_LAST_REAL_OFFSET) ? COR_E_TYPELOAD : S_OK);
    }

    // Okay, we've stolen too many bits from FieldDescs.  In the RVA case, there's no
    // reason to believe they will be limited to 22 bits.  So use a sentinel for the
    // huge cases, and recover them from metadata on-demand.
    HRESULT SetOffsetRVA(DWORD dwOffset)
    {
        LEAF_CONTRACT;

        m_dwOffset = (dwOffset > FIELD_OFFSET_LAST_REAL_OFFSET)
                      ? FIELD_OFFSET_BIG_RVA
                      : dwOffset;
        return S_OK;
    }

    BOOL IsRVAFieldWithLessThanBigOffset()
    {
        WRAPPER_CONTRACT;
        return (IsRVA() 
                    && GetModule()->GetFile()->IsILOnly()
                    && m_dwOffset != FIELD_OFFSET_BIG_RVA);
    }
    
    DWORD   IsStatic()
    {
        LEAF_CONTRACT;

        return m_isStatic;
    }

    BOOL   IsSpecialStatic()
    {
        LEAF_CONTRACT;

        return m_isStatic && (m_isRVA || m_isThreadLocal || m_isContextLocal);
    }

#if CHECK_APP_DOMAIN_LEAKS
    BOOL   IsDangerousAppDomainAgileField()
    {
        LEAF_CONTRACT;

        return m_isDangerousAppDomainAgileField;
    }

    void    SetDangerousAppDomainAgileField()
    {
        LEAF_CONTRACT;

        m_isDangerousAppDomainAgileField = TRUE;
    }
#endif

    BOOL   IsRVA()                     // Has an explicit RVA associated with it
    { 
        LEAF_CONTRACT;

        return m_isRVA;
    }

    BOOL   IsThreadStatic()            // Static relative to a thread
    {
        LEAF_CONTRACT;

        return m_isThreadLocal;
    }

    DWORD   IsContextStatic()           // Static relative to a context
    {
        LEAF_CONTRACT;

        return m_isContextLocal;
    }

    void SetEnCNew() 
    {
        WRAPPER_CONTRACT;

        SetOffset(FIELD_OFFSET_NEW_ENC);
    }

    BOOL IsEnCNew() 
    {
        WRAPPER_CONTRACT;

        return GetOffset() == FIELD_OFFSET_NEW_ENC;
    }

    BOOL IsByValue()
    {
        WRAPPER_CONTRACT;

        return GetFieldType() == ELEMENT_TYPE_VALUETYPE;
    }

    BOOL IsPrimitive()
    {
        WRAPPER_CONTRACT;

        return (CorIsPrimitiveType(GetFieldType()) != FALSE);
    }

    BOOL IsObjRef();


    UINT LoadSize();

    // Return -1 if the type isn't loaded yet (i.e. if LookupFieldTypeHandle() would return null)
    UINT GetSize();

    void    GetInstanceField(OBJECTREF o, VOID * pOutVal)
    {
        WRAPPER_CONTRACT;
        GetInstanceField(*(LPVOID*)&o, pOutVal);
    }

    void    SetInstanceField(OBJECTREF o, const VOID * pInVal)
    {
        WRAPPER_CONTRACT;
        SetInstanceField(*(LPVOID*)&o, pInVal);
    }

    // These routines encapsulate the operation of getting and setting
    // fields.
    void    GetInstanceField(LPVOID o, VOID * pOutVal);
    void    SetInstanceField(LPVOID o, const VOID * pInVal);



        // Get the address of a field within object 'o'
    void*   GetAddress(void *o);
    void*   GetAddressNoThrowNoGC(void *o);
    void*   GetAddressGuaranteedInHeap(void *o, BOOL doValidate=TRUE);

    void*   GetValuePtr(OBJECTREF o);
    VOID    SetValuePtr(OBJECTREF o, void* pValue);
    DWORD   GetValue32(OBJECTREF o);
    VOID    SetValue32(OBJECTREF o, DWORD dwValue);
    OBJECTREF GetRefValue(OBJECTREF o);
    VOID    SetRefValue(OBJECTREF o, OBJECTREF orValue);
    USHORT  GetValue16(OBJECTREF o);
    VOID    SetValue16(OBJECTREF o, DWORD dwValue);  
    BYTE    GetValue8(OBJECTREF o);               
    VOID    SetValue8(OBJECTREF o, DWORD dwValue);  
    __int64 GetValue64(OBJECTREF o);               
    VOID    SetValue64(OBJECTREF o, __int64 value);  

    MethodTable *GetApproxEnclosingMethodTable()
    {
        WRAPPER_CONTRACT;
        g_IBCLogger.LogFieldDescsAccess(m_pMTOfEnclosingClass->GetClass_NoLogging());
        return m_pMTOfEnclosingClass;
    }

    MethodTable *GetEnclosingMethodTable()
    {
        WRAPPER_CONTRACT;
        _ASSERTE(!IsSharedByGenericInstantiations());
        g_IBCLogger.LogFieldDescsAccess(m_pMTOfEnclosingClass->GetClass_NoLogging());
        return m_pMTOfEnclosingClass;
    }

    // FieldDesc can be shared between generic instantiations. So List<String>._items
    // is really the  same as List<__Canon>._items. Hence, the FieldDesc itself
    // cannot know the exact enclosing type. You need to provide the exact owner
    // like List<String> or a subtype like MyInheritedList<String>.
    TypeHandle GetExactDeclaringType(TypeHandle ownerOrSubType);

    BOOL IsSharedByGenericInstantiations()
    {
        WRAPPER_CONTRACT;
        return (!IsStatic()) && GetApproxEnclosingMethodTable()->IsSharedByGenericInstantiations();
    }

    BOOL IsFieldOfValueType()
    {
        WRAPPER_CONTRACT;
        return GetApproxEnclosingMethodTable()->IsValueType();
    }

    // returns true if accesses to this field should make the assembly non-verifiable because
    // of dangerous overlap in an explicit type.
    BOOL IsNonVerifiableExplicitField();

    DWORD GetNumGenericClassArgs()
    {
        WRAPPER_CONTRACT;
        return GetApproxEnclosingMethodTable()->GetNumGenericArgs();
    }


    EEClass *GetEnclosingClass()
    {
        WRAPPER_CONTRACT;
        return  GetApproxEnclosingMethodTable()->GetClass();
    }

    TADDR GetBaseInDomainLocalModule(DomainLocalModule* pLocalModule)
    {
        WRAPPER_CONTRACT;

        if (GetFieldType() == ELEMENT_TYPE_CLASS || GetFieldType() == ELEMENT_TYPE_VALUETYPE)
        {
            return pLocalModule->GetGCStaticsBasePointer(GetEnclosingMethodTable());
        }
        else
        {
            return pLocalModule->GetNonGCStaticsBasePointer(GetEnclosingMethodTable());
        }
    }

    TADDR GetBase(bool bRunCCtor = true)
    {
        CONTRACTL
        {
          THROWS;
          GC_TRIGGERS;
          INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        MethodTable *pMT = GetEnclosingMethodTable();

        if (bRunCCtor && pMT->IsDomainNeutral() && !IsRVA())
        {
            pMT->CheckRunClassInitThrowing();
        }

        return GetBaseInDomainLocalModule(pMT->GetDomainLocalModule());
    }

    TADDR GetBaseInDomain(AppDomain* appDomain)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;

        Module *pModule = GetEnclosingMethodTable()->GetModuleForStatics();
        if (pModule == NULL)
            return NULL;

        DomainLocalModule *pLocalModule = pModule->GetDomainLocalModule(appDomain);
        if (pLocalModule == NULL)
            return NULL;

        return GetBaseInDomainLocalModule(pLocalModule);
    }

    // returns the address of the field
    void* GetStaticAddress(void *base);

    // In all cases except Value classes, the AddressHandle is
    // simply the address of the static.  For the case of value
    // types, however, it is the address of OBJECTREF that holds
    // the boxed value used to hold the value type.  This is needed
    // because the OBJECTREF moves, and the JIT needs to embed something
    // in the code that does not move.  Thus the jit has to 
    // dereference and unbox before the access.  
    void* GetStaticAddressHandle(void *base);

    // if pModule != 0, type constructor won't be run! pModule == 0 is only
    // used for SetupSharedStatics, where play a nasty trick to make an object
    // be the same instance for all app domains
    OBJECTREF GetStaticOBJECTREF(bool bRunCCtor = false)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());

        OBJECTREF *pObjRef = NULL;
        if (IsContextStatic()) 
            pObjRef = (OBJECTREF*)Context::GetStaticFieldAddress(this);
        else if (IsThreadStatic()) 
            pObjRef = (OBJECTREF*)Thread::GetStaticFieldAddress(this);
        else {
            TADDR base = 0;
            if (!IsRVA()) // for RVA the base is ignored
            {
                base = GetBase(bRunCCtor);
            }
                
            pObjRef = (OBJECTREF*)GetStaticAddressHandle((void*)base); 
        }
        _ASSERTE(pObjRef);
        return *pObjRef;
    }

    VOID SetStaticOBJECTREF(OBJECTREF objRef)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());

        OBJECTREF *pObjRef = NULL;
        GCPROTECT_BEGIN(objRef);
        if (IsContextStatic()) 
            pObjRef = (OBJECTREF*)Context::GetStaticFieldAddress(this);
        else if (IsThreadStatic()) 
            pObjRef = (OBJECTREF*)Thread::GetStaticFieldAddress(this);
        else {
            TADDR base = 0;
            if (!IsRVA()) // for RVA the base is ignored
            {
                base = GetBase(); 
            }
            pObjRef = (OBJECTREF*)GetStaticAddress((void*)base); 
        }
        _ASSERTE(pObjRef);
        GCPROTECT_END();
        SetObjectReference(pObjRef, objRef, GetAppDomain());
    }

    void*   GetStaticValuePtr()               
    { 
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());
        return *(void**)GetPrimitiveOrValueClassStaticAddress();
    }
    
    VOID    SetStaticValuePtr(void *value)  
    { 
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());

        void **pLocation = (void**)GetPrimitiveOrValueClassStaticAddress();
        _ASSERTE(pLocation);
        *pLocation = value;
    }

    DWORD   GetStaticValue32()               
    { 
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());
        return *(DWORD*)GetPrimitiveOrValueClassStaticAddress(); 
    }
    
    VOID    SetStaticValue32(DWORD dwValue)  
    { 
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());
        *(DWORD*)GetPrimitiveOrValueClassStaticAddress() = dwValue; 
    }

    USHORT  GetStaticValue16()               
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());
        return *(USHORT*)GetPrimitiveOrValueClassStaticAddress(); 
    }
    
    VOID    SetStaticValue16(DWORD dwValue)  
    { 
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());
        *(USHORT*)GetPrimitiveOrValueClassStaticAddress() = (USHORT)dwValue; 
    }

    BYTE    GetStaticValue8()               
    { 
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());
        return *(BYTE*)GetPrimitiveOrValueClassStaticAddress(); 
    }
    
    VOID    SetStaticValue8(DWORD dwValue)  
    { 
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());
        *(BYTE*)GetPrimitiveOrValueClassStaticAddress() = (BYTE)dwValue; 
    }

    __int64 GetStaticValue64()
    { 
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());
        return *(__int64*)GetPrimitiveOrValueClassStaticAddress();
    }
    
    VOID    SetStaticValue64(__int64 qwValue)  
    { 
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());
        *(__int64*)GetPrimitiveOrValueClassStaticAddress() = qwValue;
    }

    void* GetPrimitiveOrValueClassStaticAddress()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END

        _ASSERTE(IsStatic());

        if (IsContextStatic()) 
            return Context::GetStaticFieldAddress(this);
        else if (IsThreadStatic()) 
            return Thread::GetStaticFieldAddress(this);
        else {
            TADDR base = 0;
            if (!IsRVA()) // for RVA the base is ignored
                base = GetBase(); 
            return GetStaticAddress((void*)base); 
        }
    }

    Module *GetModule()
    {
        WRAPPER_CONTRACT;

        return GetApproxEnclosingMethodTable()->GetModule();
    }

    Module *GetZapModule();

    Module *GetLoaderModule()
    {
        WRAPPER_CONTRACT;

        // Field Desc's are currently always saved into the same module as their 
        // corresponding method table.
        return GetApproxEnclosingMethodTable()->GetLoaderModule();
    }

    void GetSig(PCCOR_SIGNATURE *ppSig, DWORD *pcSig)
    {
        WRAPPER_CONTRACT;

        *ppSig = GetMDImport()->GetSigOfFieldDef(GetMemberDef(), pcSig);
    }

    //                     This is slow, don't use it!
    LPCUTF8  GetName()
    {
        WRAPPER_CONTRACT;

        return GetMDImport()->GetNameOfFieldDef(GetMemberDef());
    }

    //                     This is slow, don't use it!
    DWORD   GetAttributes()
    {
        WRAPPER_CONTRACT;

        g_IBCLogger.LogFieldDescsAccess(GetEnclosingClass());

        return GetMDImport()->GetFieldDefProps(GetMemberDef());
    }

    // Mini-Helpers
    DWORD   IsPublic()
    {
        WRAPPER_CONTRACT;

        return IsFdPublic(GetFieldProtection());
    }

    DWORD   IsPrivate()
    {
        WRAPPER_CONTRACT;

        return IsFdPrivate(GetFieldProtection());
    }

    BOOL IsNotSerialized()
    {
        WRAPPER_CONTRACT;

        MethodTable *pMT = GetApproxEnclosingMethodTable();
        if (pMT->IsSerializable() && !IsStatic())
            return pMT->IsFieldNotSerialized(pMT->GetIndexForFieldDesc(this));
        return IsFdNotSerialized(GetAttributes());
    }

    // Only safe to call this for non-static fields on serializable types.
    BOOL IsOptionallySerialized()
    {
        WRAPPER_CONTRACT;

        _ASSERTE(!IsStatic() && GetApproxEnclosingMethodTable()->IsSerializable());

        MethodTable *pMT = GetApproxEnclosingMethodTable();
        return pMT->IsFieldOptionallySerialized(pMT->GetIndexForFieldDesc(this));
    }

    IMDInternalImport *GetMDImport()
    {
        WRAPPER_CONTRACT;

        return GetModule()->GetMDImport();
    }

#ifndef DACCESS_COMPILE
    IMetaDataImport *GetRWImporter()
    {
        WRAPPER_CONTRACT;

        return GetModule()->GetRWImporter();
    }
#endif // DACCESS_COMPILE

    TypeHandle LookupFieldTypeHandle(ClassLoadLevel level = CLASS_LOADED, BOOL dropGenericArgumentLevel = FALSE);

    TypeHandle LookupApproxFieldTypeHandle()
    {
        WRAPPER_CONTRACT;
        return LookupFieldTypeHandle(CLASS_LOAD_APPROXPARENTS, TRUE);
    }

    // Instance FieldDesc can be shared between generic instantiations. So List<String>._items
    // is really the  same as List<__Canon>._items. Hence, the FieldDesc itself
    // cannot know the exact field type. This function returns the approximate field type.
    // For eg. this will return "__Canon[]" for List<String>._items.
    TypeHandle GetFieldTypeHandleThrowing(ClassLoadLevel level = CLASS_LOADED, BOOL dropGenericArgumentLevel = FALSE);

    TypeHandle GetApproxFieldTypeHandleThrowing()
    {
        WRAPPER_CONTRACT;
        return GetFieldTypeHandleThrowing(CLASS_LOAD_APPROXPARENTS, TRUE);
    }

    // Given a type handle of an object and a method that comes from some 
    // superclass of the class of that object, find the instantiation of 
    // that superclass, i.e. the class instantiation which will be relevant
    // to interpreting the signature of the method.  The type handle of
    // the object does not need to be given in all circumstances, in 
    // particular it is only needed for FieldDescs pFD that
    // return true for pFD->GetApproxEnclosingMethodTable()->IsSharedByGenericInstantiations().
    // In other cases it is allowed to be null and will be ignored.
    // 
    // Will return NULL if the field is not in a generic class.
    TypeHandle *GetExactClassInstantiation(TypeHandle possibleObjType);

    // Instance FieldDesc can be shared between generic instantiations. So List<String>._items
    // is really the  same as List<__Canon>._items. Hence, the FieldDesc itself
    // cannot know the exact field type. You need to specify the owner
    // like List<String> in order to get the exact type which would be "String[]"
    TypeHandle GetExactFieldType(TypeHandle owner);

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
    {
        DAC_ENUM_DTHIS();
    }
#endif

#ifndef DACCESS_COMPILE
#endif // DACCESS_COMPILE

};

#endif // _FIELD_H_

