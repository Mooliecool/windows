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

#ifndef _METHODTABLE_INL_ 
#define _METHODTABLE_INL_

#include "methodtable.h"
#include "genericdict.h"

//==========================================================================================
inline EEClass* MethodTable::GetClass_NoLogging()
{
    LEAF_CONTRACT;
    return m_pEEClass;
}

//==========================================================================================
inline EEClass* MethodTable::GetClass()
{
    LEAF_CONTRACT;

    // the only time a MT can have a NULL EEClass, and normal callers will never encounter this case.
    // the above PREFIX_ASSUME slows down the checked build (see CHECK class in inc\check.h)
    // this is a frequent call site, it needs faster asserts
    _ASSERTE_IMPL(m_wFlags2 != enum_flag2_IsAsyncPin);
    _ASSERTE_IMPL(m_pEEClass != NULL);

    return m_pEEClass;
}

//==========================================================================================
inline EEClass* MethodTable::GetClassIfPresent()
{
    LEAF_CONTRACT;
    PREFIX_ASSUME(m_pEEClass != NULL || m_wFlags2 == enum_flag2_IsAsyncPin);

    return m_pEEClass;
}

//==========================================================================================
inline Assembly * MethodTable::GetAssembly()
{
    WRAPPER_CONTRACT;
    return GetModule()->GetAssembly();
}

//==========================================================================================
// DO NOT ADD ANY ASSERTS OR ANY OTHER CODE TO THIS METHOD.
// DO NOT USE THIS METHOD.
// Yes folks, for better or worse the debugger pokes supposed object addresses
// to try to see if objects are valid, possibly firing an AccessViolation or
// worse.  Thus it is "correct" behaviour for this to AV, and incorrect
// behaviour for it to assert if called on an invalid pointer.
inline EEClass* MethodTable::GetClassWithPossibleAV()
{
    CANNOT_HAVE_CONTRACT;
    return m_pEEClass;
}

//==========================================================================================
// Does this immediate item live in an NGEN module?
inline BOOL MethodTable::IsZapped()
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return (m_wFlags2 & enum_flag2_IsZapped);
}

//==========================================================================================
// For types that are part of an ngen-ed assembly this gets the
// Module* that contains this methodtable.
inline PTR_Module MethodTable::GetZapModule()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    PTR_Module zapModule = NULL;
    if (IsZapped())
    {
        zapModule = m_pLoaderModule;
    }

    return zapModule;
}


//==========================================================================================
inline Module* MethodTable::GetLoaderModule() 
{
    LEAF_CONTRACT;
    return m_pLoaderModule;
}

#ifndef DACCESS_COMPILE 
//==========================================================================================
inline void MethodTable::SetLoaderModule(Module* pModule) 
{
    LEAF_CONTRACT;
    m_pLoaderModule = pModule;
}
#endif

//==========================================================================================
inline WORD MethodTable::GetNumInstanceFields()
{
    WRAPPER_CONTRACT;
    return (GetClass()->GetNumInstanceFields());
}

//==========================================================================================
inline WORD MethodTable::GetNumStaticFields()
{
    WRAPPER_CONTRACT;
    return (GetClass()->GetNumStaticFields());
}

//==========================================================================================
// <BUG> @dsyme It appears that this MUST be available even from an
// unrestored MethodTable, which may have a bad EEClass pointer - see GcScanRoots
// in siginfo.cpp - at least if ENREGISTERED_PARAMTYPE_MAXSIZE
// is defined.  This may cause failures on amd64 etc.</BUG>
inline DWORD MethodTable::GetNumInstanceFieldBytes()
{
    WRAPPER_CONTRACT;
    return(GetClass()->GetNumInstanceFieldBytes());
}

//==========================================================================================
inline WORD MethodTable::GetNumIntroducedInstanceFields()
{
    WRAPPER_CONTRACT;
    return(GetClass()->GetNumIntroducedInstanceFields());
}

//==========================================================================================
inline DWORD MethodTable::GetAlignedNumInstanceFieldBytes()
{
    WRAPPER_CONTRACT;
    return(GetClass()->GetAlignedNumInstanceFieldBytes());
}

//==========================================================================================
inline WORD MethodTable::GetNumGCPointerSeries()
{
    WRAPPER_CONTRACT;
    return GetClass()->GetNumGCPointerSeries();
}

#ifndef DACCESS_COMPILE 
//==========================================================================================
inline FieldDesc *MethodTable::GetApproxFieldDescListRaw()
{
    WRAPPER_CONTRACT;
    // Careful about using this method. If it's possible that fields may have been added via EnC, then
    // must use the FieldDescIterator as any fields added via EnC won't be in the raw list

    return GetClass()->GetApproxFieldDescListRaw();
}
#endif // !DACCESS_COMPILE


//==========================================================================================
inline BOOL MethodTable::IsSerializable()
{
    WRAPPER_CONTRACT;
    return GetClass()->IsSerializable();
}

//==========================================================================================
inline BOOL MethodTable::ContainsStackPtr()
{
    WRAPPER_CONTRACT;
    return GetClass()->ContainsStackPtr();
}

//==========================================================================================
inline BOOL MethodTable::CannotBeBlittedByObjectCloner()
{
    WRAPPER_CONTRACT;
    return GetClass()->CannotBeBlittedByObjectCloner();
}

//==========================================================================================
inline BOOL MethodTable::HasFieldsWhichMustBeInited()
{
    WRAPPER_CONTRACT;
    return GetClass()->HasFieldsWhichMustBeInited();
}

//==========================================================================================
inline BOOL MethodTable::IsAbstract()
{
    WRAPPER_CONTRACT;
    return GetClass()->IsAbstract();
}

//==========================================================================================
inline BOOL MethodTable::HasRemotableMethodInfo()
{
    WRAPPER_CONTRACT;
    return (IsMarshaledByRef() || IsInterface() || this == g_pObjectClass || g_pObjectClass == NULL);
}

//==========================================================================================
inline void MethodTable::SetHasRemotingVtsInfo()
{
    LEAF_CONTRACT;
    m_wFlags |= enum_flag_HasRemotingVtsInfo;
}

//==========================================================================================
inline BOOL MethodTable::HasRemotingVtsInfo()
{
    LEAF_CONTRACT;
    return m_wFlags & enum_flag_HasRemotingVtsInfo;
}

//==========================================================================================
inline PTR_RemotingVtsInfo MethodTable::GetRemotingVtsInfo()
{
    WRAPPER_CONTRACT;
    _ASSERTE(HasRemotingVtsInfo());
    return *GetRemotingVtsInfoPtr();
}

//==========================================================================================
// The following two methods produce correct results only if this type is
// marked Serializable (verified by assert in checked builds) and the field
// in question was introduced in this type (the index is the FieldDesc
// index).
inline BOOL MethodTable::IsFieldNotSerialized(DWORD dwFieldIndex)
{
    LEAF_CONTRACT;
    _ASSERTE(IsSerializable());

    if (!HasRemotingVtsInfo())
        return FALSE;

    return GetRemotingVtsInfo()->IsNotSerialized(dwFieldIndex);
}

//==========================================================================================
inline BOOL MethodTable::IsFieldOptionallySerialized(DWORD dwFieldIndex)
{
    LEAF_CONTRACT;
    _ASSERTE(IsSerializable());

    if (!HasRemotingVtsInfo())
        return FALSE;

    return GetRemotingVtsInfo()->IsOptionallySerialized(dwFieldIndex);
}


//==========================================================================================
// Is pParentMT either System.ValueType or System.Enum?  Returns
// FALSE if pParentMT is System.ValueType and the global for System.Enum
// has not yet been set, i.e. during startup class loading, TRUE for pParentMT at other times.
// If called on System.Enum at other times besides class loading
// then the caller should check if the the immediate child being tested is System.Enum.
//
/* static  */
inline BOOL MethodTable::IsChildValueType(MethodTable *pParentMT)
{
    LEAF_CONTRACT;
    // There is one exception to the rule that you are a value class
    // if you inherit from g_pValueTypeClass, namely System.Enum.
    // we detect that we are System.Enum because g_pEnumClass has
    // not been set
    return
        pParentMT != NULL
        && ((g_pEnumClass != NULL && pParentMT == g_pValueTypeClass) ||
             pParentMT == g_pEnumClass);
}

//==========================================================================================
// Is pParentMT System.Enum?  Returns
// FALSE for System.Enum during startup, TRUE at other times
/* static */
inline BOOL MethodTable::IsChildEnum(MethodTable *pParentMT)
{
    LEAF_CONTRACT;
    // keep in sync with IsEnum
    return (pParentMT != NULL && (pParentMT == g_pEnumClass));
}

//==========================================================================================
inline BOOL MethodTable::IsValueClass()
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    return GetClass()->IsValueClass();
}

//==========================================================================================
inline BOOL MethodTable::IsEnum()
{
    WRAPPER_CONTRACT;
    // If we haven't yet restored the parent method table pointer then
    // we have to go to the EEClass.  During the process of restoration
    // EEClass gets restored first, and then the parent method table pointer.
    // In between we still want to be able to call IsEnum().
    //
    // For hardbound or non-cross-module pointers we can go straight
    // to the parent mathod table.
    //
#ifndef DACCESS_COMPILE 
    // We should not be calling this before at least our EEClass pointer
    // is valid .
    _ASSERTE(!CORCOMPILE_IS_TOKEN_TAGGED((SIZE_T) this));
    _ASSERTE(!CORCOMPILE_IS_TOKEN_TAGGED((SIZE_T) m_pEEClass));

    if(m_pParentMethodTable && CORCOMPILE_IS_TOKEN_TAGGED((SIZE_T) m_pParentMethodTable))
    {
        return GetClass()->IsEnum();
    }
#endif // !DACCESS_COMPILE
    BOOL res = IsChildEnum(GetParentMethodTable());
    // Check that the result of the above is the same as what EEClass gives.
    _ASSERTE((res == 0) == (GetClass()->IsEnum() == 0));
    return res;
}

//==========================================================================================
inline BOOL MethodTable::IsValueType()
{
    WRAPPER_CONTRACT;
    // This gets used by
    //    MethodTable::Restore -->
    //    Pending::Pending(...) -->
    //    TypeHandle.GetSignatureCorElementType () -->
    //    MethodTable::IsValueType()
    // early during the process of restoring
    // a method table.  Thus we cannot assume that the parent method table is
    // restored or is even yet a valid pointer.
    //
    // If we haven't yet restored the parent method table pointer then
    // we have to go to the EEClass.  During the process of restoration then
    // EEClass gets restored first, and then the parent method table pointer.
    // In between we still want to be able to call IsValueType().
    //
    // For hardbound or non-cross-module pointers we can go straight
    // to the parent mathod table.
    //

#ifndef DACCESS_COMPILE 
    // We should not be calling this before at least our EEClass pointer
    // is valid.
    _ASSERTE(!CORCOMPILE_IS_TOKEN_TAGGED((SIZE_T) this));
    _ASSERTE(!CORCOMPILE_IS_TOKEN_TAGGED((SIZE_T) m_pEEClass));

    g_IBCLogger.LogMethodTableAccess(this);

    if(CORCOMPILE_IS_TOKEN_TAGGED((SIZE_T) m_pParentMethodTable))
    {
        return GetClass()->IsValueClass();
    }
#endif // !DACCESS_COMPILE
    // See note on IsChildValueType for why we need to check "this != g_pEnumClass"
    BOOL res = IsChildValueType(GetParentMethodTable()) && (this != g_pEnumClass);
    // Check that the result of the above is the same as what EEClass says.
    _ASSERTE((res == 0) == (GetClass()->IsValueClass() == 0));
    return res;
}

//==========================================================================================
inline TypeHandle MethodTable::GetApproxArrayElementTypeHandle()
{
    WRAPPER_CONTRACT;

    _ASSERTE (IsArray());
    return PTR_ArrayClass(PTR_HOST_TO_TADDR(GetClass()))->GetApproxArrayElementTypeHandle();
}

//==========================================================================================
inline CorElementType MethodTable::GetArrayElementType()
{
    WRAPPER_CONTRACT;

    _ASSERTE (IsArray());
    return PTR_ArrayClass(PTR_HOST_TO_TADDR(GetClass()))->GetArrayElementType();
}

//==========================================================================================
inline TypeHandle MethodTable::GetApproxArrayElementTypeHandle_NoLogging()
{
    WRAPPER_CONTRACT;

    _ASSERTE (IsArray());
    return PTR_ArrayClass(PTR_HOST_TO_TADDR(GetClass()))->GetApproxArrayElementTypeHandle_NoLogging();
}

//==========================================================================================
inline DWORD MethodTable::GetRank()
{
    WRAPPER_CONTRACT;

    _ASSERTE (IsArray());
    return PTR_ArrayClass(PTR_HOST_TO_TADDR(GetClass()))->GetRank();
}

//==========================================================================================
inline BOOL MethodTable::IsTruePrimitive()
{
    WRAPPER_CONTRACT;
    BOOL res = !IsArray() && (m_wFlags & enum_flag_InternalCorElementTypeExtraInfoMask) ==
                             enum_flag_InternalCorElementTypeExtraInfo_IfNotArrayThenTruePrimitive;
    _ASSERTE(!res == !GetClass()->IsTruePrimitive());
    return res;
}

//==========================================================================================
inline BOOL MethodTable::IsBlittable()
{
    WRAPPER_CONTRACT;
#ifndef DACCESS_COMPILE 
    _ASSERTE(GetClass());
    return GetClass()->IsBlittable();
#else // DACCESS_COMPILE
    DacNotImpl();
    return false;
#endif // DACCESS_COMPILE
}

#ifndef DACCESS_COMPILE 

//==========================================================================================
inline BOOL MethodTable::HasDefaultConstructor()
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    return GetClass()->HasDefaultConstructor();
}

//==========================================================================================
inline WORD MethodTable::GetDefaultConstructorSlot()
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    return GetClass()->GetDefaultConstructorSlot();
}

//==========================================================================================
inline void MethodTable::SetDefaultConstructorSlot (WORD wDefaultCtorSlot)
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    GetClass()->SetDefaultConstructorSlot(wDefaultCtorSlot);
}

//==========================================================================================
inline BOOL MethodTable::IsContextful()
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    return GetClass()->IsContextful();
}

//==========================================================================================
inline BOOL MethodTable::HasLayout()
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    return GetClass()->HasLayout();
}

#endif // !DACCESS_COMPILE

//==========================================================================================
inline SLOT* MethodTable::GetAddrOfRestoredSlotForMethod(MethodDesc* method)
{
    WRAPPER_CONTRACT;
    DWORD slot = method->GetSlot();

    //
    // Restore the slot contents if necessary
    //

    GetRestoredSlot(slot);

    //
    // Return the address of the slot
    //

    return(GetSlotPtr(slot));
}

//==========================================================================================
inline MethodDesc* MethodTable::GetUnknownMethodDescForSlot(DWORD slot)
{
    WRAPPER_CONTRACT;
    MethodTable *pMT = this;
    if (slot >= GetNumMethods())
    {
        CONSISTENCY_CHECK(HasInstantiation());
        pMT = GetCanonicalMethodTable();
        CONSISTENCY_CHECK(slot < pMT->GetNumMethods());
    }
    return MethodTable::GetUnknownMethodDescForSlotAddress(pMT->GetRestoredSlot(slot));
}

//==========================================================================================
inline MethodDesc* MethodTable::GetMethodDescForSlot(DWORD slot)
{
    WRAPPER_CONTRACT;
    _ASSERTE(!IsThunking());


    return(GetUnknownMethodDescForSlot(slot));
}

//==========================================================================================
inline MethodDesc* MethodTable::GetDeclMethodDescForSlot(DWORD slot)
{
    WRAPPER_CONTRACT;
    _ASSERTE(!IsThunking());
    return GetMethodDescForSlot(slot)->GetDeclMethodDesc(slot);
}

#ifndef DACCESS_COMPILE 

//==========================================================================================
inline INT32 MethodTable::MethodIterator::GetNumMethods() const
{
    //  assert that number of methods hasn't changed during the iteration
    CONSISTENCY_CHECK( m_pMethodData->GetNumMethods() == static_cast< UINT32 >( m_iMethods ) );
    return m_iMethods;
}

//==========================================================================================
// Returns TRUE if it's valid to request data from the current position
inline BOOL MethodTable::MethodIterator::IsValid() const
{
    LEAF_CONTRACT;
    return m_iCur >= 0 && m_iCur < GetNumMethods();
}

//==========================================================================================
inline BOOL MethodTable::MethodIterator::MoveTo(UINT32 idx)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    m_iCur = (INT32)idx;
    return IsValid();
}

//==========================================================================================
inline BOOL MethodTable::MethodIterator::Prev()
{
    WRAPPER_CONTRACT;
    if (IsValid())
        --m_iCur;
    return (IsValid());
}

//==========================================================================================
inline BOOL MethodTable::MethodIterator::Next()
{
    WRAPPER_CONTRACT;
    if (IsValid())
        ++m_iCur;
    return (IsValid());
}

//==========================================================================================
inline void MethodTable::MethodIterator::MoveToBegin()
{
    WRAPPER_CONTRACT;
    m_iCur = 0;
}

//==========================================================================================
inline void MethodTable::MethodIterator::MoveToEnd()
{
    WRAPPER_CONTRACT;
    m_iCur = GetNumMethods() - 1;
}

//==========================================================================================
inline UINT32 MethodTable::MethodIterator::GetSlotNumber() const {
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(IsValid());
    return (UINT32)m_iCur;
}

//==========================================================================================
inline BOOL MethodTable::MethodIterator::IsVirtual() const {
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(IsValid());
    return m_iCur < (INT32)(GetNumVirtuals());
}

//==========================================================================================
inline UINT32 MethodTable::MethodIterator::GetNumVirtuals() const {
    LEAF_CONTRACT;
    return m_pMethodData->GetNumVirtuals();;
}

//==========================================================================================
inline DispatchSlot MethodTable::MethodIterator::GetTarget() const {
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(IsValid());
    return m_pMethodData->GetImplSlot(m_iCur);
}

//==========================================================================================
inline MethodDesc *MethodTable::MethodIterator::GetMethodDesc() const {
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(IsValid());
    MethodDesc *pMD = m_pMethodData->GetImplMethodDesc(m_iCur);
    CONSISTENCY_CHECK(CheckPointer(pMD));
    return pMD;
}

//==========================================================================================
inline MethodDesc *MethodTable::MethodIterator::GetDeclMethodDesc() const {
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(IsValid());
    MethodDesc *pMD = m_pMethodData->GetDeclMethodDesc(m_iCur);
    CONSISTENCY_CHECK(CheckPointer(pMD));
    CONSISTENCY_CHECK(pMD->GetSlot() == GetSlotNumber());
    return pMD;
}

//==========================================================================================
inline BOOL MethodTable::IntroducedMethodIterator::IsValid() const
{
    LEAF_CONTRACT;
    return m_pMethodDescChunk != NULL;
}

//==========================================================================================
inline MethodDesc * MethodTable::IntroducedMethodIterator::GetMethodDesc() const
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(IsValid());
    MethodDesc *pMD = m_pMethodDescChunk->GetMethodDescAt(m_iCurMethodInChunk);
    CONSISTENCY_CHECK(CheckPointer(pMD));
    return pMD;
}

#endif // !DACCESS_COMPILE

//==========================================================================================
inline UINT32 MethodTable::GetNativeSize()
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    return GetClass()->GetNativeSize();
}

//==========================================================================================
inline void MethodTable::SetNativeSize(UINT32 nativeSize)
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());
    GetClass()->SetNativeSize(nativeSize);
}

//==========================================================================================
inline MethodTable *MethodTable::GetCanonicalMethodTable()
{
    WRAPPER_CONTRACT;
    if (!HasInstantiation())
        return this;

    _ASSERTE(GetClass());
    return (GetClass()->GetMethodTable());
}

//==========================================================================================
inline BOOL MethodTable::IsMarshaledByRef()
{
    LEAF_CONTRACT;

    return (m_wFlags2 & enum_flag2_MarshaledByRef);
}

//==========================================================================================
inline IMDInternalImport* MethodTable::GetMDImport()
{
    WRAPPER_CONTRACT;
    return GetModule()->GetMDImport();
}

//==========================================================================================
inline BOOL MethodTable::IsSealed()
{
    WRAPPER_CONTRACT;
    return GetClass()->IsSealed();
}

//==========================================================================================
inline BOOL MethodTable::IsManagedSequential()
{
    WRAPPER_CONTRACT;
    return GetClass()->IsManagedSequential();
}

//==========================================================================================
inline DWORD MethodTable::GetPerInstInfoSize()
{
    WRAPPER_CONTRACT;
    return GetNumDicts() * sizeof(TypeHandle*);
}

//==========================================================================================
inline EEClassLayoutInfo *MethodTable::GetLayoutInfo()
{
    WRAPPER_CONTRACT;
    PRECONDITION(HasLayout());
    return GetClass()->GetLayoutInfo();
}

//==========================================================================================
// These come after the pointers to the generic dictionaries (if any)
inline DWORD MethodTable::GetInterfaceMapSize()
{
    LEAF_CONTRACT;
    DWORD cbIMap = GetNumInterfaces() * sizeof(InterfaceInfo_t);
    return cbIMap;
}

//==========================================================================================
// These are the generic dictionaries themselves and are come after
//  the interface map.  In principle they need not be inline in the method table.
inline DWORD MethodTable::GetInstAndDictSize()
{
    LEAF_CONTRACT;
    return HasInstantiation() ? GetClass()->GetInstAndDictSize() : 0;
}

//==========================================================================================
inline BOOL MethodTable::IsCanonicalMethodTable()
{
    LEAF_CONTRACT;
    return ((m_wFlags2 & enum_flag2_GenericsMask) != enum_flag2_GenericsMask_NonCanonInst);
}

//==========================================================================================
__forceinline BOOL MethodTable::HasInstantiation()
{
    LEAF_CONTRACT;

    g_IBCLogger.LogMethodTableAccess(this);

    BOOL result = (m_wFlags2 & enum_flag2_GenericsMask) != enum_flag2_GenericsMask_NonGeneric;
    _ASSERTE_IMPL(!IsRestored() || result == GetClass()->HasInstantiation());
    return result;
}

//==========================================================================================
inline void MethodTable::SetHasInstantiation(BOOL typical)
{
    LEAF_CONTRACT;
    m_wFlags2 |= (typical ? enum_flag2_GenericsMask_TypicalInst : enum_flag2_GenericsMask_CanonInst);
}

//==========================================================================================
inline BOOL MethodTable::IsGenericTypeDefinition()
{
    LEAF_CONTRACT;

    BOOL result = (m_wFlags2 & enum_flag2_GenericsMask) == enum_flag2_GenericsMask_TypicalInst;

    // check the result is the same as the EEClass, though first check if the EEClass is restored.
    _ASSERTE_IMPL(CORCOMPILE_IS_TOKEN_TAGGED((SIZE_T) m_pEEClass)
             || (!result == !GetClass()->IsGenericTypeDefinition()));

    return result;
}

//==========================================================================================
inline PTR_InterfaceInfo MethodTable::GetInterfaceMap()
{
    WRAPPER_CONTRACT;
    if (!HasInterfaceMap())
    {
        _ASSERTE(m_pIMapDEBUG == NULL);
        _ASSERTE(GetNumInterfaces() == 0);
        return NULL;
    }

    PTR_InterfaceInfo pInterfaceInfo = *GetInterfaceMapPtr();

    // Check against our mirror
    CONSISTENCY_CHECK(m_pIMapDEBUG == pInterfaceInfo);
    // If HasInterfaceMap returns true above, then we MUST have an interface map pointer.
    CONSISTENCY_CHECK(pInterfaceInfo != NULL);

    return pInterfaceInfo;
}

//==========================================================================================
// This method is dependent on the declared order of optional members
// If you add or remove an optional member or reorder them please change this method
FORCEINLINE DWORD MethodTable::GetOffsetOfOptionalMember(OptionalMemberId id)
{
    WRAPPER_CONTRACT;
    DWORD offset = sizeof(MethodTable);
    offset += GetTotalSlots()*sizeof(SLOT);

#undef METHODTABLE_OPTIONAL_MEMBER
#define METHODTABLE_OPTIONAL_MEMBER(NAME, TYPE, GETTER) \
    if (id == OptionalMember_##NAME) { \
        return offset; \
    } \
    C_ASSERT(sizeof(TYPE) % sizeof(UINT_PTR) == 0); /* To insure proper alignment */ \
    if (Has##NAME()) { \
        offset += sizeof(TYPE); \
    }

    METHODTABLE_OPTIONAL_MEMBERS()

    _ASSERTE(!"Wrong optional member" || id == OptionalMember_Count);
    return offset;
}

//==========================================================================================
// this is not the pretties function however I got bitten pretty hard by the computation 
// of the allocation size of a MethodTable done "by hand" in few places.
// Essentially the idea is that this is going to centralize computation of size for optional
// members so the next morons that need to add an optional member will look at this function
// and hopefully be less exposed to code doing size computation behind their back
inline DWORD MethodTable::GetOptionalMembersAllocationSize(BOOL needsInterfaceMap,
                                                           BOOL needsRemotableMethodInfo,
                                                           BOOL needsGenericsStaticsInfo,
                                                           BOOL needsRemotingVtsInfo,
                                                           BOOL needsGenericDictionaryInfo,
                                                           BOOL needsThreadContextStatic)
{
    DWORD size = 0;

    if (needsInterfaceMap)
        size += sizeof(UINT_PTR);
    if (needsRemotableMethodInfo)
        size += sizeof(UINT_PTR);
    if (needsGenericsStaticsInfo)
        size += sizeof(GenericsStaticsInfo);
    if (needsRemotingVtsInfo)
        size += sizeof(UINT_PTR);
    if (needsGenericDictionaryInfo)
        size += sizeof(GenericsDictInfo);
    if (needsThreadContextStatic)
        size += sizeof(UINT_PTR);
    return size;
}

inline DWORD MethodTable::GetOptionalMembersSize()
{
    WRAPPER_CONTRACT;

    return GetEndOffsetOfOptionalMembers() - GetStartOffsetOfOptionalMembers();
}

#ifdef _DEBUG 
//==========================================================================================
inline InterfaceInfo_t** MethodTable::GetIMapDEBUGPtr()
{
    LEAF_CONTRACT;
    return &m_pIMapDEBUG;
}
#endif // _DEBUG


//==========================================================================================
inline TADDR MethodTable::GetNonGCStaticsBasePointer()
{
    WRAPPER_CONTRACT;
    return GetDomainLocalModule()->GetNonGCStaticsBasePointer(this);
}

//==========================================================================================
inline TADDR MethodTable::GetGCStaticsBasePointer()
{
    WRAPPER_CONTRACT;
    return GetDomainLocalModule()->GetGCStaticsBasePointer(this);
}

//==========================================================================================
inline PTR_DomainLocalModule MethodTable::GetDomainLocalModule(AppDomain* pAppDomain /* = NULL */)
{
    WRAPPER_CONTRACT;
    return GetModuleForStatics()->GetDomainLocalModule(pAppDomain);
}

//==========================================================================================
inline OBJECTREF MethodTable::AllocateNoChecks()
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        THROWS;
    }
    CONTRACTL_END;

    // we know an instance of this class already exists in the same appdomain
    // therefore, some checks become redundant.
    // this currently only happens for Delegate.Combine
    CONSISTENCY_CHECK(IsRestored());

    CONSISTENCY_CHECK(CheckInstanceActivated());

    return AllocateObject(this);
}

//==========================================================================================
inline CorElementType  MethodTable::GetInternalCorElementType()
{
    WRAPPER_CONTRACT;
    _ASSERTE(GetClass());


    g_IBCLogger.LogMethodTableAccess(this);

    // At least we can avoid the touch in this case...
    if (IsArray())
    {
        if (m_wFlags & enum_flag_IfArrayThenSzArray)
        {
            _ASSERTE(GetClass()->GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY);
            return ELEMENT_TYPE_SZARRAY;
        }
        else
        {
            _ASSERTE(GetClass()->GetInternalCorElementType() == ELEMENT_TYPE_ARRAY);
            return ELEMENT_TYPE_ARRAY;
        }
    }
    else if (IsInterface() || ((m_wFlags & enum_flag_InternalCorElementTypeExtraInfoMask) == enum_flag_InternalCorElementTypeExtraInfo_IfNotArrayThenClass))
    {
        _ASSERTE(GetClass()->GetInternalCorElementType() == ELEMENT_TYPE_CLASS);
        return ELEMENT_TYPE_CLASS;
    }
    else if ((m_wFlags & enum_flag_InternalCorElementTypeExtraInfoMask) == enum_flag_InternalCorElementTypeExtraInfo_IfNotArrayThenValueType)
    {
        _ASSERTE(GetClass()->GetInternalCorElementType() == ELEMENT_TYPE_VALUETYPE);
        return ELEMENT_TYPE_VALUETYPE;
    }
    else
    {

        // This path should typically only be taken fo a fixed number of inbuilt mscorlib types.
        g_IBCLogger.LogEEClassAndMethodTableAccess(GetClass());

        return GetClass()->GetInternalCorElementType();
    }
}

//==========================================================================================
inline BOOL MethodTable::InterfaceMapIterator::IsRestored()
{
    LEAF_CONTRACT;
    PRECONDITION(m_i >= 0 && m_i < m_count);
    MethodTable *pMT = GetInterfaceInfo()->m_pMethodTable;
    return !CORCOMPILE_IS_TOKEN_TAGGED(SIZE_T(pMT)) && pMT->IsRestored();
}

//==========================================================================================
inline void MethodTable::InterfaceMapIterator::Restore()
{
    WRAPPER_CONTRACT;
    PRECONDITION(m_i >= 0 && m_i < m_count);
#ifndef DACCESS_COMPILE
#endif // !DACCESS_COMPILE
}

//==========================================================================================
inline BOOL MethodTable::HasThreadOrContextStatics()
{
    LEAF_CONTRACT;
    return (enum_flag_ThreadContextStatic & m_wFlags);
}

//==========================================================================================
inline void MethodTable::SetHasThreadOrContextStatics()
{
    LEAF_CONTRACT;
    m_wFlags |= enum_flag_ThreadContextStatic;
}

//==========================================================================================
inline DWORD MethodTable::GetThreadStaticsOffset()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;    
    return GetThreadOrContextStaticsBucket()->m_dwThreadStaticsOffset;
}

//==========================================================================================
inline DWORD MethodTable::GetContextStaticsOffset()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return GetThreadOrContextStaticsBucket()->m_dwContextStaticsOffset;
}

//==========================================================================================
inline WORD MethodTable::GetThreadStaticsSize()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return GetThreadOrContextStaticsBucket()->m_wThreadStaticsSize;
}

//==========================================================================================
inline WORD MethodTable::GetContextStaticsSize()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return GetThreadOrContextStaticsBucket()->m_wContextStaticsSize;
}

//==========================================================================================
inline DWORD MethodTable::AllocateThreadStaticsOffset()
{
    WRAPPER_CONTRACT;
    return AllocateThreadOrContextStaticsOffset(FALSE);
}

//==========================================================================================
inline DWORD MethodTable::AllocateContextStaticsOffset()
{
    WRAPPER_CONTRACT;
    return AllocateThreadOrContextStaticsOffset(TRUE);
}

//==========================================================================================
inline DWORD MethodTable::GetClassIndex()
{
    WRAPPER_CONTRACT;
    return GetClassIndexFromToken(GetCl());
}

//==========================================================================================
// unbox src into dest, making sure src is of the correct type.

inline BOOL MethodTable::UnBoxInto(void *dest, OBJECTREF src) 
{
    WRAPPER_CONTRACT;

    if (Nullable::IsNullableType(TypeHandle(this)))
        return Nullable::UnBox(dest, src, this);
    else  
    {
        if (src == NULL || src->GetMethodTable() != this)
            return FALSE;

        CopyValueClass(dest, src->UnBox(), this, src->GetAppDomain());
    }
    return TRUE;
}

//==========================================================================================
// unbox src into dest, No checks are done

inline void MethodTable::UnBoxIntoUnchecked(void *dest, OBJECTREF src) 
{
    WRAPPER_CONTRACT;

    if (Nullable::IsNullableType(TypeHandle(this))) {
        BOOL ret = Nullable::UnBox(dest, src, this);
        _ASSERTE(ret);
    }
    else  
    {
        _ASSERTE(src->GetMethodTable()->GetNumInstanceFieldBytes() == GetNumInstanceFieldBytes());

        CopyValueClass(dest, src->UnBox(), this, src->GetAppDomain());
    }
}

//==========================================================================================
inline TypeHandle::CastResult MethodTable::CanCastToClassOrInterfaceNoGC(MethodTable *pTargetMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INSTANCE_CHECK;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pTargetMT));
        PRECONDITION(!pTargetMT->IsArray());
    }
    CONTRACTL_END

    if (pTargetMT->IsInterface())
        return CanCastToInterfaceNoGC(pTargetMT);
    else
        return CanCastToClassNoGC(pTargetMT);
}

//==========================================================================================
inline BOOL MethodTable::CanCastToClassOrInterface(MethodTable *pTargetMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INSTANCE_CHECK;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pTargetMT));
        PRECONDITION(!pTargetMT->IsArray());
        PRECONDITION(IsRestored());
    }
    CONTRACTL_END

    if (pTargetMT->IsInterface())
        return CanCastToInterface(pTargetMT);
    else
        return CanCastToClass(pTargetMT);
}

//==========================================================================================

#endif // !_METHODTABLE_INL_
