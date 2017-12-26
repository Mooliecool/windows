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
// File: Method.CPP
//

#include "common.h"
#include "comvariant.h"
#include "remoting.h"
#include "security.h"
#include "verifier.hpp"
#include "excep.h"
#include "dbginterface.h"
#include "ecall.h"
#include "eeconfig.h"
#include "mlinfo.h"
#include "dllimport.h"
#include "generics.h"
#include "genericdict.h"
#include "typedesc.h"
#include "typestring.h"
#include "virtualcallstub.h"
#include "jitinterface.h"
#include "runtimehandles.h"



// Verify that the structure sizes of our MethodDescs support proper
// aligning for atomic stub replacement.
//
C_ASSERT((sizeof(MethodDescChunk)       & (METHOD_ALIGN - 1)) == 0);
C_ASSERT((sizeof(MethodDesc)            & (METHOD_ALIGN - 1)) == 0);
C_ASSERT((sizeof(FCallMethodDesc)       & (METHOD_ALIGN - 1)) == 0);
C_ASSERT((sizeof(NDirectMethodDesc)     & (METHOD_ALIGN - 1)) == 0);
C_ASSERT((sizeof(EEImplMethodDesc)      & (METHOD_ALIGN - 1)) == 0);
C_ASSERT((sizeof(ArrayMethodDesc)       & (METHOD_ALIGN - 1)) == 0);
C_ASSERT((sizeof(DynamicMethodDesc)     & (METHOD_ALIGN - 1)) == 0);

static const unsigned s_ClassificationSizeTable[] = {
    sizeof(MethodDesc),                 // mcIL
    sizeof(FCallMethodDesc),            // mcFCall
    sizeof(NDirectMethodDesc),          // mcNDirect
    sizeof(EEImplMethodDesc),           // mcEEImpl
    sizeof(ArrayMethodDesc),            // mcArray
    sizeof(InstantiatedMethodDesc),     // mcInstantiated
        0,
    sizeof(DynamicMethodDesc),          // mcDynamic
};

MethodDescCallSite::MethodDescCallSite(MethodDesc* pMD, SignatureNative* pSigNative, BOOL bCanCacheTargetAndCrackedSig, OBJECTREF* porProtectedThis, TypeHandle ownerType, BOOL fCriticalCall) :
    m_pMD(pMD),
    m_pbCallTarget(NULL),
    m_methodSig(pMD, pSigNative, bCanCacheTargetAndCrackedSig),
    m_fCriticalCall(fCriticalCall)
{
    WRAPPER_CONTRACT;
#ifndef DACCESS_COMPILE
    m_pbCallTarget = (BYTE*)pSigNative->GetCallTarget();
    DefaultInit(porProtectedThis, ownerType);       
    if (bCanCacheTargetAndCrackedSig)
        pSigNative->CacheCallTarget((void*)m_pbCallTarget);
#endif
}

//*******************************************************************************
/* static */ SIZE_T MethodDescChunk::GetMethodDescSize(DWORD classification)
{
    LEAF_CONTRACT;

    C_ASSERT(NumItems(s_ClassificationSizeTable) == mdcClassificationCount);
    SIZE_T size = METHOD_PREPAD + s_ClassificationSizeTable[classification & mdcClassification];

    // MethodImpl immediately follows the method desc
    if (classification & mdcMethodImpl)
        size += sizeof(MethodImpl);

    return size;
}

//*******************************************************************************
BOOL MethodDesc::IsIntrospectionOnly()
{
    WRAPPER_CONTRACT;
    return GetModule()->GetAssembly()->IsIntrospectionOnly();
}

//*******************************************************************************
VOID MethodDesc::EnsureActive()
{
    WRAPPER_CONTRACT;
    GetMethodTable()->EnsureInstanceActive();
    TypeHandle* methodInst=NULL;
    if(GetNumGenericMethodArgs()>0)
        methodInst=GetMethodInstantiation();
    for (DWORD i=0;i<GetNumGenericMethodArgs();i++)
        methodInst[i].GetMethodTable()->EnsureInstanceActive();
}

//*******************************************************************************
CHECK MethodDesc::CheckActivated()
{
    WRAPPER_CONTRACT;
    CHECK(GetModule()->CheckActivated());
    CHECK_OK;
}

//*******************************************************************************
BaseDomain *MethodDesc::GetDomain()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    if (HasMethodInstantiation() && !IsGenericMethodDefinition())
    {
        return BaseDomain::ComputeBaseDomain(GetMethodTable()->GetDomain(),
                                             GetNumGenericMethodArgs(),
                                             GetMethodInstantiation());
    }
    else
    {
        return GetMethodTable()->GetDomain();
    }
}

//*******************************************************************************
BaseDomain *MethodDesc::GetDomainForAllocation()
{
    if (IsLCGMethod())
        return GetAppDomain();
    else
        return GetDomain();
}

//*******************************************************************************
LPCUTF8 MethodDesc::GetName(USHORT slot)
{
    // MethodDesc::GetDeclMethodDesc can throw.
    WRAPPER_CONTRACT;
    MethodDesc *pDeclMD = GetDeclMethodDesc((UINT32)slot);
    CONSISTENCY_CHECK(IsInterface() || !pDeclMD->IsInterface());
    return pDeclMD->GetName();
}

//*******************************************************************************
LPCUTF8 MethodDesc::GetName()
{
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS; // MethodImpl::FindMethodDesc can throw.
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }CONTRACTL_END;

    g_IBCLogger.LogMethodDescAccess(this);

    if (GetMethodTable()->IsArray())
    {
        // Array classes don't have metadata tokens
        return ((DPTR(ArrayMethodDesc))PTR_HOST_TO_TADDR(this))->GetMethodName();
    }
    else if (IsNoMetadata())
    {
        // Array classes don't have metadata tokens
        return ((DPTR(DynamicMethodDesc))PTR_HOST_TO_TADDR(this))->GetMethodName();
    }
    else
    {
        // Get the metadata string name for this method
        LPCUTF8 result = NULL;

        // This probes only if we have a thread, in which case it is OK to throw the SO.
        BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(COMPlusThrowSO());

        result = GetMDImport()->GetNameOfMethodDef(GetMemberDef());

        END_SO_INTOLERANT_CODE;

        return(result);
    }
}

//*******************************************************************************
void MethodDesc::GetSig(PCCOR_SIGNATURE *ppSig, DWORD *pcSig)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    if (HasStoredSig())
    {
        StoredSigMethodDesc *pSMD =
            PTR_StoredSigMethodDesc(PTR_HOST_TO_TADDR(this));
        if (pSMD->HasStoredMethodSig() || GetClassification()==mcDynamic)
        {
            *ppSig = pSMD->GetStoredMethodSig(pcSig);
            PREFIX_ASSUME(*ppSig != NULL);
            return;
        }
    }

    *ppSig = GetMDImport()->GetSigOfMethodDef(GetMemberDef(), pcSig);
    PREFIX_ASSUME(*ppSig != NULL);
}

//*******************************************************************************
PCCOR_SIGNATURE MethodDesc::GetSig()
{
    WRAPPER_CONTRACT;

    PCCOR_SIGNATURE pSig;
    DWORD           cSig;

    GetSig(&pSig, &cSig);

    PREFIX_ASSUME(pSig != NULL);

    return pSig;
}

//*******************************************************************************
PTR_MethodDesc MethodDesc::GetDeclMethodDesc(UINT32 slotNumber)
{
    CONTRACTL {
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        INSTANCE_CHECK;
    } CONTRACTL_END;

    MethodDesc *pMDResult = this;

    // If the MethodDesc is not itself a methodImpl, but it is not in its native
    // slot, then someone (perhaps itself) must have overridden a methodImpl
    // in a parent, which causes the method to get put into all of the methodImpl
    // slots. So, the MethodDesc is implicitly a methodImpl without containing
    // the data. To find the real methodImpl MethodDesc, climb the inheritance
    // hierarchy checking the native slot on the way.
    if ((UINT32)pMDResult->GetSlot() != slotNumber)
    {
        while (!pMDResult->IsMethodImpl()
              )
        {
            CONSISTENCY_CHECK(CheckPointer(pMDResult->GetMethodTable()->GetParentMethodTable()));
            CONSISTENCY_CHECK(slotNumber < pMDResult->GetMethodTable()->GetParentMethodTable()->GetNumVirtuals());
            pMDResult = pMDResult->GetMethodTable()->GetParentMethodTable()->GetMethodDescForSlot(slotNumber);
        }

        {
            CONSISTENCY_CHECK(pMDResult->IsMethodImpl());
            MethodImpl *pImpl = pMDResult->GetMethodImpl();
            pMDResult = pImpl->FindMethodDesc(slotNumber, PTR_MethodDesc(pMDResult));
        }

    }

    CONSISTENCY_CHECK(CheckPointer(pMDResult));
    CONSISTENCY_CHECK((UINT32)pMDResult->GetSlot() == slotNumber);
    return PTR_MethodDesc(pMDResult);
}

//*******************************************************************************
// Returns a hash for the method.
// The hash will be the same for the method across multiple process runs.
COUNT_T MethodDesc::GetStableHash()
{
    WRAPPER_CONTRACT;
    _ASSERTE(IsRestored());
    
    COUNT_T hash = HashStringA(GetModule()->GetSimpleName());
    
    hash = HashCOUNT_T(hash, GetMemberDef());

    if (HasClassInstantiation() && !GetMethodTable()->IsGenericTypeDefinition())
    {
        DWORD nargs = GetNumGenericClassArgs();
        TypeHandle * pInst = GetClassInstantiation();
        for (DWORD i = 0; i < nargs; i++)
        {
            MethodTable * pMT = pInst[i].GetMethodTable();
            if (pMT)
                hash = HashCOUNT_T(hash, pMT->GetCl());
        }
    }

    if (HasMethodInstantiation() && !IsGenericMethodDefinition())
    {
        DWORD nargs = GetNumGenericMethodArgs();
        TypeHandle * pInst = GetMethodInstantiation();
        for (DWORD i = 0; i < nargs; i++)
        {
            MethodTable * pMT = pInst[i].GetMethodTable();
            if (pMT)
                hash = HashCOUNT_T(hash, pMT->GetCl());
        }
    }

    return hash;
}

//*******************************************************************************
// Get the number of type parameters to a generic method
// Currently we go via the metadata which might be slow, so we only do
// this for methods that may really be generic.
DWORD MethodDesc::GetNumGenericMethodArgs()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    if (GetClassification() == mcInstantiated)
    {
        InstantiatedMethodDesc *pIMD = AsInstantiatedMethodDesc();
        return pIMD->m_wNumGenericArgs;
    }
    else return 0;
}


//*******************************************************************************
TypeHandle *MethodDesc::GetExactClassInstantiation(TypeHandle possibleObjType)
    {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END


    return (possibleObjType.IsNull()
            ? GetClassInstantiation()
            : possibleObjType.GetInstantiationOfParentClass(GetMethodTable()));
}


//*******************************************************************************
BOOL MethodDesc::HasNonObjectClassOrMethodInstantiation()
{
    WRAPPER_CONTRACT;
    PRECONDITION(IsRestored());

    TypeHandle *classInst = GetMethodTable()->GetInstantiation();
    if (classInst != NULL)
    {
        for (DWORD i = 0; i < GetNumGenericClassArgs(); i++)
        {
            if (g_pHiddenMethodTableClass != classInst[i].GetMethodTable())
                return TRUE;
        }
    }

    if (IsGenericMethodDefinition())
        return TRUE;

    TypeHandle *methodInst = GetMethodInstantiation();
    if (methodInst != NULL)
    {
        for (DWORD i = 0; i < GetNumGenericMethodArgs(); i++)
        {
            if (g_pHiddenMethodTableClass != methodInst[i].GetMethodTable())
                return TRUE;
        }
    }

    return FALSE;
}

//*******************************************************************************
TypeHandle *MethodDesc::LoadMethodInstantiation()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    if (IsGenericMethodDefinition() && !IsTypicalMethodDefinition())
    {
        return LoadTypicalMethodDefinition()->GetMethodInstantiation();
    }
    else
        return GetMethodInstantiation();
}

//*******************************************************************************
Module *MethodDesc::GetDefiningModuleForOpenMethod()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    Module *pModule = GetMethodTable()->GetDefiningModuleForOpenType();
    if (pModule != NULL)
        return pModule;

    if (IsGenericMethodDefinition())
        return GetModule();

    TypeHandle* inst = GetMethodInstantiation();
    for (DWORD i = 0; i < GetNumGenericMethodArgs(); i++)
    {
        // Encoded types are never open
        if (!inst[i].IsEncodedFixup())
        {
            pModule = inst[i].GetDefiningModuleForOpenType();
            if (pModule != NULL)
                return pModule;
        }
    }

    return NULL;
}


//*******************************************************************************
BOOL MethodDesc::ContainsGenericVariables()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(IsRestored());
    }
    CONTRACTL_END

    // If this is a method of a generic type, does the type have
    // non-instantiated type arguments

    if (TypeHandle(GetMethodTable()).ContainsGenericVariables())
        return TRUE;

    if (IsGenericMethodDefinition())
        return TRUE;

    // If this is an instantiated generic method, are there are any generic type variables
    if (GetNumGenericMethodArgs() != 0)
    {
        TypeHandle *methodInst = GetMethodInstantiation();
        PREFIX_ASSUME(methodInst != NULL);
        for (DWORD i = 0; i < GetNumGenericMethodArgs(); i++)
        {
            if (methodInst[i].ContainsGenericVariables())
                return TRUE;
        }
    }

    return FALSE;
}


//*******************************************************************************
Stub *MethodDesc::GetStub()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END



    if (!HasPrecode())
        return NULL;

    Precode* pPrecode = GetPrecode();

    // Fetch target just once to avoid races
    TADDR target = pPrecode->GetTarget();

    // Note that this can return true for remoting precode (different from Precode::IsPoitingToNativeCode)
    if (pPrecode->IsPointingTo(target, pPrecode->GetNativeCode()))
        return NULL;

    if (pPrecode->IsPointingToPrestub(target))
        return ThePreStub();

    return Stub::RecoverStub(target);


}

#ifndef DACCESS_COMPILE
//*******************************************************************************
void MethodDesc::Destruct()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        FORBID_FAULT;
    }
    CONTRACTL_END

    if (!IsRestored())
        return;

    Stub *pStub = GetStub();
    if (pStub != NULL && pStub != ThePreStub()) {
        pStub->DecRef();
    }

    if (IsNDirect())
    {
        MLHeader *pMLHeader = ((NDirectMethodDesc*)this)->GetMLHeader();
        Module *pModule = GetModule();
        PREFIX_ASSUME(pModule != NULL);
        if (pMLHeader != NULL && !pModule->IsPersistedObject(pMLHeader))
        {
            Stub *pMLStub = Stub::RecoverStub((TADDR)pMLHeader);
            pMLStub->DecRef();
        }
    }


#ifndef HAS_REMOTING_PRECODE
    EEClass *pClass = GetClass();
    if(pClass->IsMarshaledByRef() || (pClass == g_pObjectClass->GetClass()))
    {
        // Destroy the thunk generated to intercept calls for remoting
        CRemotingServices::DestroyThunk(this);
    }
#endif // HAS_REMOTING_PRECODE
}

//*******************************************************************************
BOOL MethodDesc::InterlockedReplaceStub(Stub** ppStub, Stub *pNewStub)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE(ppStub != NULL);
    _ASSERTE(((DWORD_PTR)ppStub&(sizeof(void*)-1)) == 0);

    Stub *pPrevStub = (Stub*)FastInterlockCompareExchangePointer((void**)ppStub, (void*) pNewStub,
                                                          NULL);

    // Return TRUE if we succeeded.
    return (pPrevStub == NULL);
}


//*******************************************************************************
HRESULT MethodDesc::Verify(COR_ILMETHOD_DECODER* ILHeader,
                            BOOL fThrowException,
                            BOOL fForceVerify)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    _ASSERTE(!"EE Verification is disabled, should never get here");
    return E_FAIL;
}

//*******************************************************************************

BOOL MethodDesc::IsVerifiable()
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    if (IsVerified())
        return (m_wFlags & mdcVerifiable);

    if (!IsTypicalMethodDefinition())
    {
        // We cannot verify concrete instantiation (eg. List<int>.Add()).
        // We have to verify the typical instnatiation (eg. List<T>.Add()).
        MethodDesc * pGenMethod = LoadTypicalMethodDefinition();
        BOOL isVerifiable = pGenMethod->IsVerifiable();

        // Propagate the result from the typical instantiation to the
        // concrete instantiation
        SetIsVerified(isVerifiable);

        return isVerifiable;
    }

    COR_ILMETHOD_DECODER::DecoderStatus status;
    COR_ILMETHOD_DECODER header(GetILHeader(), GetMDImport(), &status);
    if (status != COR_ILMETHOD_DECODER::SUCCESS ||
        !GetModule()->CheckIL(GetRVA(), header.GetHeaderSize()))
    {
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_BAD_IL_RANGE);
    }


    UnsafeJitFunction(this, &header, CORJIT_FLG_IMPORT_ONLY);
    _ASSERTE(IsVerified());

    return (IsVerified() && (m_wFlags & mdcVerifiable));
}

//*******************************************************************************
// Update flags in a thread safe manner.
WORD MethodDesc::InterlockedUpdateFlags(WORD wMask, BOOL fSet)
{
    LEAF_CONTRACT;

    WORD    wOldState = m_wFlags;
    DWORD   dwMask = wMask;

    // We need to make this operation atomic (multiple threads can play with the flags field at the same time). But the flags field
    // is a word and we only have interlock operations over dwords. So we round down the flags field address to the nearest aligned
    // dword (along with the intended bitfield mask). Note that we make the assumption that the flags word is aligned itself, so we
    // only have two possibilites: the field already lies on a dword boundary or it's precisely one word out.
    DWORD* pdwFlags = (DWORD*)((ULONG_PTR)&m_wFlags - (offsetof(MethodDesc, m_wFlags) & 0x3));


#if BIGENDIAN
    if ((offsetof(MethodDesc, m_wFlags) & ~0x3) == 0) {
#else // !BIGENDIAN
    if ((offsetof(MethodDesc, m_wFlags) & ~0x3) != 0) {
#endif // !BIGENDIAN
        C_ASSERT(sizeof(m_wFlags) == 2);
        dwMask <<= 16;
    }

    g_IBCLogger.LogMethodDescWriteAccess(this);

    if (fSet)
        FastInterlockOr(pdwFlags, dwMask);
    else
        FastInterlockAnd(pdwFlags, ~dwMask);

    return wOldState;
}

#endif // !DACCESS_COMPILE

//*******************************************************************************
// Returns the address of the native code. The native code can be one of:
// - fcall entrypoint if IsFCall()
// - jitted code if !IsFCall() && !IsPreImplemented()
// - ngened code if !IsFCall() && IsPreImplemented()
// 
// Methods which have no native code are either implemented by stubs or not jitted yet.
// For example, NDirectMethodDesc's have no native code.  They are treated as 
// implemented by stubs.  On WIN64, these stubs are IL stubs, which DO have native code.
//
// This function returns null if the method has no native code.
TADDR MethodDesc::GetNativeCode()
{
    WRAPPER_CONTRACT;

    if (!HasStableEntryPoint())
        return NULL;

    if (HasPrecode())
        return GetPrecode()->GetNativeCode();

    return GetStableEntryPoint();
}

//*******************************************************************************
TADDR MethodDesc::GetPreImplementedCode()
{
    WRAPPER_CONTRACT;

    return NULL;
}

//*******************************************************************************
BOOL MethodDesc::IsVoid()
{
    WRAPPER_CONTRACT;

    MetaSig sig(this);
    return sig.IsReturnTypeVoid();
}

//*******************************************************************************
BOOL MethodDesc::HasRetBuffArg()
{
    WRAPPER_CONTRACT;

    MetaSig sig(this);
    return sig.HasRetBuffArg();
}

//*******************************************************************************
// This returns the offset of the IL.
// The offset is relative either to the base of the IL image, or to the base
// of the IL contents in the ngen image. Module::GetIL(RVA) will interpret
// it correctly. Just dont use the return value directly as you do not
// know which case is applicable.
ULONG MethodDesc::GetRVA()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END


    if (IsRuntimeSupplied())
    {
        return 0;
    }

    if (GetMemberDef() & 0x00FFFFFF)
    {
        Module *pModule = GetModule();
        PREFIX_ASSUME(pModule != NULL);

#ifndef DACCESS_COMPILE
        DWORD dwOverride;
        if (pModule->GetRVAOverrideForMethod(this, &dwOverride))
            return dwOverride;
#endif

        DWORD dwDescrOffset;
        DWORD dwImplFlags;
        pModule->GetMDImport()->GetMethodImplProps(GetMemberDef(), &dwDescrOffset, &dwImplFlags);
        BAD_FORMAT_NOTHROW_ASSERT(IsNDirect() || IsMiIL(dwImplFlags) || IsMiOPTIL(dwImplFlags) || dwDescrOffset == 0);
        return dwDescrOffset;
    }

    return 0;
}

//*******************************************************************************
BOOL MethodDesc::IsVarArg()
{
    WRAPPER_CONTRACT;
    PCCOR_SIGNATURE pSig = GetSig();
    PREFIX_ASSUME(pSig != NULL);
    return MetaSig::IsVarArg(GetModule(), pSig);
}

//*******************************************************************************
COR_ILMETHOD* MethodDesc::GetILHeader()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
        PRECONDITION(IsIL());
        PRECONDITION(!IsUnboxingStub());
    }
    CONTRACTL_END

    Module *pModule = GetModule();

#ifdef DACCESS_COMPILE
    return DacGetIlMethod((TADDR)pModule->GetIL((RVA)GetRVA()));
#else // !DACCESS_COMPILE
    return PTR_COR_ILMETHOD((TADDR)pModule->GetIL((RVA)GetRVA()));
#endif // !DACCESS_COMPILE
}

//*******************************************************************************
MetaSig::RETURNTYPE MethodDesc::ReturnsObject(
#ifdef _DEBUG
    bool supportStringConstructors
#endif
    )
{
    CONTRACTL
    {
#ifdef _DEBUG
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
#else
        NOTHROW;
#endif        
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    MetaSig sig(this);
    MetaSig::RETURNTYPE rt = sig.GetReturnObjectKind();
    if (rt != MetaSig::RETNONOBJ)
        return rt;

    // String constructors return objects.  We should not have any ecall string
    // constructors, except when called from gc coverage codes (which is only
    // done under debug).  We will therefore optimize the retail version of this
    // method to not support string constructors.

#ifdef _DEBUG
    if (IsCtor() && GetClass()->HasVarSizedInstances())
    {
        _ASSERTE(supportStringConstructors);
        return MetaSig::RETOBJ;
    }
#endif // _DEBUG

#ifdef ENREGISTERED_RETURNTYPE_MAXSIZE
#endif // ENREGISTERED_RETURNTYPE_MAXSIZE

    return MetaSig::RETNONOBJ;
}


//*******************************************************************************
DWORD MethodDesc::GetAttrs()
{
    WRAPPER_CONTRACT;

    if (IsArray())
        return ((DPTR(ArrayMethodDesc))PTR_HOST_TO_TADDR(this))->GetAttrs();
    else if (IsNoMetadata())
        return ((DPTR(DynamicMethodDesc))PTR_HOST_TO_TADDR(this))->GetAttrs();;

    return GetMDImport()->GetMethodDefProps(GetMemberDef());
}

//*******************************************************************************
DWORD MethodDesc::GetImplAttrs()
{
    WRAPPER_CONTRACT;

    ULONG RVA;
    DWORD props;
    GetMDImport()->GetMethodImplProps(GetMemberDef(), &RVA, &props);
    return props;
}


//*******************************************************************************
Module* MethodDesc::GetLoaderModule()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    if (HasMethodInstantiation() && !IsGenericMethodDefinition())
    {
        return ClassLoader::ComputeLoaderModule(GetMethodTable(),
                                                GetMemberDef(),
                                                GetMethodInstantiation(),
                                                GetNumGenericMethodArgs());
    }
    else
    {
        return GetMethodTable()->GetLoaderModule();
    }
}

//*******************************************************************************
Module *MethodDesc::GetModule()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    Module *pModule = GetModule_NoLogging();
    g_IBCLogger.LogMethodDescAccess(this);

    return pModule;
}

//*******************************************************************************
Module *MethodDesc::GetModule_NoLogging()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    MethodTable* pMT = GetMethodDescChunk()->GetMethodTable();
    return pMT->GetModule();
}

//*******************************************************************************
// See method.hpp for details
BOOL MethodDesc::IsUnboxingStub()
{
    WRAPPER_CONTRACT;

    return (m_bFlags2 & enum_flag2_IsUnboxingStub) != 0;
}

//*******************************************************************************
// Is this an instantiating stub for generics?  This does not include those
// BoxedEntryPointStubs which call an instantiating stub.
BOOL MethodDesc::IsInstantiatingStub()
{
    WRAPPER_CONTRACT;

    return
        (GetClassification() == mcInstantiated)
         && !IsUnboxingStub()
         && AsInstantiatedMethodDesc()->IMD_IsWrapperStubWithInstantiations();
}

//*******************************************************************************
BOOL MethodDesc::IsWrapperStub()
{
    WRAPPER_CONTRACT;
    return (IsUnboxingStub() || IsInstantiatingStub());
}

#ifndef DACCESS_COMPILE
//*******************************************************************************

MethodDesc *MethodDesc::GetWrappedMethodDesc()
{
    WRAPPER_CONTRACT;
    
    _ASSERTE(IsWrapperStub());
    
    if (IsUnboxingStub())
    {
        return this->GetMethodTable()->GetUnboxedEntryPointMD(this);
    }

    if (IsInstantiatingStub())
    {
        MethodDesc *pRet = AsInstantiatedMethodDesc()->IMD_GetWrappedMethodDesc();
#ifdef _DEBUG
        MethodDesc *pAltMD  =
               MethodDesc::FindOrCreateAssociatedMethodDesc(this,
                                                            this->GetMethodTable(),
                                                            FALSE, /* no unboxing entrypoint */
                                                            this->GetNumGenericMethodArgs(),
                                                            this->GetMethodInstantiation(),
                                                            TRUE /* get shared code */ );
        _ASSERTE(pAltMD == pRet);
#endif // _DEBUG
        return pRet;
    }
    return NULL;
}


MethodDesc *MethodDesc::GetExistingWrappedMethodDesc()
{
    WRAPPER_CONTRACT;
    
    _ASSERTE(IsWrapperStub());
    
    if (IsUnboxingStub())
    {
        return this->GetMethodTable()->GetExistingUnboxedEntryPointMD(this);
    }

    if (IsInstantiatingStub())
    {
        MethodDesc *pRet = AsInstantiatedMethodDesc()->IMD_GetWrappedMethodDesc();
        return pRet;
    }
    return NULL;
}



#endif // !DACCESS_COMPILE

//*******************************************************************************
BOOL MethodDesc::IsSharedByGenericInstantiations()
{
    WRAPPER_CONTRACT;

    if (IsWrapperStub())
        return FALSE;
    else if (GetMethodTable()->IsSharedByGenericInstantiations())
        return TRUE;
    else return IsSharedByGenericMethodInstantiations();
}

//*******************************************************************************
BOOL MethodDesc::IsSharedByGenericMethodInstantiations()
{
    WRAPPER_CONTRACT;

    if (GetClassification() == mcInstantiated)
        return AsInstantiatedMethodDesc()->IMD_IsSharedByGenericMethodInstantiations();
    else return FALSE;
}

//*******************************************************************************
// Does this method require an extra MethodTable argument for instantiation information?
// This is the case for
// * per-inst static methods in shared-code instantiated generic classes (e.g. static void MyClass<string>::m())
// * shared-code instance methods in instantiated generic structs (e.g. void MyValueType<string>::m())
BOOL MethodDesc::RequiresInstMethodTableArg() {
    WRAPPER_CONTRACT;

    return
        IsSharedByGenericInstantiations() &&
        !HasMethodInstantiation() &&
        (IsStatic() || GetMethodTable()->IsValueClass());
}

//*******************************************************************************
// Does this method require an extra InstantiatedMethodDesc argument for instantiation information?
// This is the case for
// * shared-code instantiated generic methods
BOOL MethodDesc::RequiresInstMethodDescArg() {
    WRAPPER_CONTRACT;

    return IsSharedByGenericInstantiations()&&
        HasMethodInstantiation();
}

//*******************************************************************************
BOOL MethodDesc::IsRuntimeMethodHandle()
{
    WRAPPER_CONTRACT;

    return (!HasMethodInstantiation() || !IsSharedByGenericMethodInstantiations());
}

//*******************************************************************************
// Strip off method and class instantiation if present e.g.
// C1<int>.m1<string> -> C1.m1
// C1<int>.m2 -> C1.m2
// C2.m2<int> -> C2.m2
// C2.m2 -> C2.m2
MethodDesc* MethodDesc::LoadTypicalMethodDefinition()
{
    CONTRACT(MethodDesc*)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(RETVAL->IsTypicalMethodDefinition());
    }
    CONTRACT_END

#ifndef DACCESS_COMPILE
    if (HasClassOrMethodInstantiation())
    {
        MethodTable *pMT = GetMethodTable();
        if (!pMT->IsTypicalTypeDefinition())
            pMT = ClassLoader::LoadTypeDefThrowing(pMT->GetModule(), 
                                                   pMT->GetCl(),
                                                   ClassLoader::ThrowIfNotFound,
                                                   ClassLoader::PermitUninstDefOrRef).GetMethodTable();
        CONSISTENCY_CHECK(TypeHandle(pMT).CheckFullyLoaded());
        MethodDesc *resultMD = pMT->GetMethodDescForSlot(GetSlot());
        PREFIX_ASSUME(resultMD != NULL);
        RETURN (resultMD);
    }
    else
#endif // !DACCESS_COMPILE
        RETURN(this);
}

//*******************************************************************************
BOOL MethodDesc::IsTypicalMethodDefinition()
{
    WRAPPER_CONTRACT;

    if (HasMethodInstantiation() && !IsGenericMethodDefinition())
        return FALSE;

    if (HasClassInstantiation() && !GetMethodTable()->IsGenericTypeDefinition())
        return FALSE;

    return TRUE;
}

//*******************************************************************************
BOOL MethodDesc::AcquiresInstMethodTableFromThis() {
    WRAPPER_CONTRACT;

    return
        IsSharedByGenericInstantiations()  &&
        !HasMethodInstantiation() &&
        !IsStatic() &&
        !GetMethodTable()->IsValueClass();
}

//*******************************************************************************
UINT MethodDesc::SizeOfVirtualFixedArgStack()
{
    WRAPPER_CONTRACT;
    return MetaSig(this).SizeOfVirtualFixedArgStack(IsStatic());
}

//*******************************************************************************
UINT MethodDesc::SizeOfActualFixedArgStack()
{
    WRAPPER_CONTRACT;
    return MetaSig(this).SizeOfActualFixedArgStack(IsStatic());
}

//*******************************************************************************
UINT MethodDesc::CbStackPop()
{
    WRAPPER_CONTRACT;
    return MetaSig(this).CbStackPop(IsStatic());
}

#ifndef DACCESS_COMPILE

//*******************************************************************************
void MethodDesc::SetRVA(ULONG rva)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    // Store RVA in lookaside table on module
    GetModule()->SetRVAOverrideForMethod(this, rva);
}

MethodDesc* MethodDesc::StripMethodInstantiation()
{
    CONTRACT(MethodDesc*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    MethodTable *pMT = GetMethodTable()->GetCanonicalMethodTable();
    MethodDesc *resultMD = pMT->GetMethodDescForSlot(GetSlot());
    _ASSERTE(resultMD->IsGenericMethodDefinition() || !resultMD->HasMethodInstantiation());
    RETURN(resultMD);
}

//*******************************************************************************
MethodDescChunk *MethodDescChunk::CreateChunk(LoaderHeap *pHeap, DWORD methodDescCount, 
    DWORD classification, BYTE tokrange, MethodTable *pInitialMT, AllocMemTracker *pamTracker)
{
    CONTRACT(MethodDescChunk *)
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(ThrowOutOfMemory());

        PRECONDITION(CheckPointer(pHeap));
        PRECONDITION(methodDescCount <= GetMaxMethodDescs(classification));
        PRECONDITION(methodDescCount > 0);
        PRECONDITION(CheckPointer(pInitialMT, NULL_OK));
        PRECONDITION(CheckPointer(pamTracker));

        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    SIZE_T mdSize  = GetMethodDescSize(classification);
    SIZE_T presize = sizeof(MethodDescChunk);

    MethodDescChunk *mdChunk = (MethodDescChunk *)
        pamTracker->Track(
            pHeap->AllocAlignedMem(presize + mdSize * methodDescCount,
                                   MethodDesc::ALIGNMENT,
                                   NULL)
                         );

    mdChunk->m_count = methodDescCount;

    _ASSERTE((mdSize & MethodDesc::ALIGNMENT_MASK) == 0);
    mdChunk->m_unit = mdSize >> MethodDesc::ALIGNMENT_SHIFT;

    _ASSERTE((classification & ~CHUNK_KIND_MASK) == 0);
    mdChunk->m_kind = classification;

    mdChunk->m_tokrange    = tokrange;
    mdChunk->m_methodTable = pInitialMT;

    _ASSERTE(mdChunk->GetMethodDescSize() == mdSize);

    for (DWORD i = 0; i < methodDescCount; i++)
    {
        MethodDesc* pMD = mdChunk->GetMethodDescAt(i);

        pMD->SetSizeClassification(classification);
        pMD->SetChunkIndex(mdChunk, i);
    }

    RETURN mdChunk;
}

//*******************************************************************************
// <NICE> This should not be needed! </NICE>
void MethodDescChunk::SetMethodTable(MethodTable *pMT)
{
    WRAPPER_CONTRACT;

    _ASSERTE(m_methodTable == NULL);
    m_methodTable = pMT;
    pMT->GetClass()->AddChunk(this);
}

//--------------------------------------------------------------------
// Virtual Resolution on Objects
//
// Given a MethodDesc and an Object, return the target address
// and/or the target MethodDesc and/or make a call.
//
// Some of the implementation of this logic is in
// MethodTable::GetTargetFromMethodDescAndServer
// and MethodTable::GetMethodDescForInterfaceMethodAndServer.
// Those functions should really be moved here.
//--------------------------------------------------------------------

//*******************************************************************************
// The following resolve virtual dispatch for the given method on the given
// object down to an actual address to call, including any
// handling of context proxies and other thunking layers.
MethodDesc* MethodDesc::ResolveGenericVirtualMethod(OBJECTREF *orThis)
{
    CONTRACT(MethodDesc *)
    {
        THROWS;
        GC_TRIGGERS;

        PRECONDITION(IsVtableMethod());
        PRECONDITION(IsRestored());
        PRECONDITION(HasMethodInstantiation());
        PRECONDITION(!ContainsGenericVariables());
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(RETVAL->HasMethodInstantiation());
    }
    CONTRACT_END;

    // Method table of target (might be instantiated)
    // Deliberately use GetMethodTable -- not GetTrueMethodTable
    MethodTable *pObjMT = (*orThis)->GetMethodTable();

    // This is the static method descriptor describing the call.
    // It is not the destination of the call, which we must compute.
    MethodDesc* pStaticMD = this;

    if (pObjMT->IsThunking())
    {
        // For transparent proxies get the client's view of the server type
        // unless we're calling through an interface (in which case we let the
        // server handle the resolution).
        if (pStaticMD->IsInterface())
            RETURN(pStaticMD);
        pObjMT = (*orThis)->GetTrueMethodTable();
    }

    // Strip off the method instantiation if present
    MethodDesc* pStaticMDWithoutGenericMethodArgs = pStaticMD->StripMethodInstantiation();

    // Compute the target, though we have not yet applied the type arguments.
    MethodDesc *pTargetMDBeforeGenericMethodArgs =
        pStaticMD->IsInterface()
        ? MethodTable::GetMethodDescForInterfaceMethodAndServer(TypeHandle(pStaticMD->GetMethodTable()),
                                                                pStaticMDWithoutGenericMethodArgs,orThis)
        : pObjMT->GetMethodDescForSlot(pStaticMDWithoutGenericMethodArgs->GetSlot());

    pTargetMDBeforeGenericMethodArgs->CheckRestore();

    // The actual destination may lie anywhere in the inheritance hierarchy.
    // between the static descriptor and the target object.
    // So now compute where we are really going!  This may be an instantiated
    // class type if the generic virtual lies in a generic class.
    MethodTable *pTargetMT = pTargetMDBeforeGenericMethodArgs->GetMethodTable();

    // No need to find/create a new generic instantiation if the target is the
    // same as the static, i.e. the virtual method has not been overriden.
    if (!pTargetMT->IsSharedByGenericInstantiations() && !pTargetMT->IsValueClass() &&
        pTargetMDBeforeGenericMethodArgs == pStaticMDWithoutGenericMethodArgs)
        RETURN(pStaticMD);

    if (pTargetMT->IsSharedByGenericInstantiations())
    {
        pTargetMT = ClassLoader::LoadGenericInstantiationThrowing(pTargetMT->GetModule(),
                                                                  pTargetMT->GetCl(),
                                                                  pTargetMT->GetNumGenericArgs(),
                                                                  pTargetMDBeforeGenericMethodArgs->GetExactClassInstantiation(TypeHandle(pObjMT))).GetMethodTable();
    }

    RETURN(MethodDesc::FindOrCreateAssociatedMethodDesc(
        pTargetMDBeforeGenericMethodArgs,
        pTargetMT,
        (pTargetMT->IsValueClass()), /* get unboxing entry point if a struct*/
        pStaticMD->GetNumGenericMethodArgs(),
        pStaticMD->GetMethodInstantiation(),
        FALSE /* no allowInstParam */ ));
}

//*******************************************************************************
const BYTE* MethodDesc::GetSingleCallableAddrOfVirtualizedCode(OBJECTREF *orThis)
{
    WRAPPER_CONTRACT;
    PRECONDITION(IsVtableMethod());

    if (HasMethodInstantiation())
    {
        CheckRestore();
        MethodDesc *pResultMD = ResolveGenericVirtualMethod(orThis);

        // If we're remoting this call we can't call directly on the returned
        // method desc, we need to go through a stub that guarantees we end up
        // in the remoting handler. The stub we use below is normally just for
        // non-virtual calls on virtual methods (that have the same problem
        // where we could end up bypassing the remoting system), but it serves
        // our purpose here (basically pushes our correctly instantiated,
        // resolved method desc on the stack and calls the remoting code).
        if ((*orThis)->IsThunking())
            if (IsInterface())
                return CRemotingServices::GetStubForInterfaceMethod(pResultMD);
            else
                return (BYTE*)CRemotingServices::GetNonVirtualEntryPointForVirtualMethod(pResultMD);

        return pResultMD->GetAddrofCode();
    }

    // Deliberately use GetMethodTable -- not GetTrueMethodTable
    MethodTable *pMT = (*orThis)->GetMethodTable();

    BYTE *addr = (BYTE *) pMT->GetRestoredSlotForMethod(this);

    return addr;
}

//*******************************************************************************
// The following resolve virtual dispatch for the given method on the given
// object down to an actual address to call, including any
// handling of context proxies and other thunking layers.
const BYTE* MethodDesc::GetMultiCallableAddrOfVirtualizedCode(OBJECTREF *orThis, TypeHandle staticTH)
{
    CONTRACT(const BYTE *)
    {
        THROWS;
        GC_TRIGGERS;

        PRECONDITION(IsRestored());
        PRECONDITION(IsVtableMethod());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // Method table of target (might be instantiated)
    // Deliberately use GetMethodTable -- not GetTrueMethodTable
    MethodTable *pObjMT = (*orThis)->GetMethodTable();

    // This is the static method descriptor describing the call.
    // It is not the destination of the call, which we must compute.
    MethodDesc* pStaticMD = this;
    MethodDesc *pTargetMD;

    if (pStaticMD->HasMethodInstantiation())
    {
        CheckRestore();
        pTargetMD = ResolveGenericVirtualMethod(orThis);

        // If we're remoting this call we can't call directly on the returned
        // method desc, we need to go through a stub that guarantees we end up
        // in the remoting handler. The stub we use below is normally just for
        // non-virtual calls on virtual methods (that have the same problem
        // where we could end up bypassing the remoting system), but it serves
        // our purpose here (basically pushes our correctly instantiated,
        // resolved method desc on the stack and calls the remoting code).
        if (pObjMT->IsThunking())
            if (pStaticMD->IsInterface())
                RETURN(CRemotingServices::GetStubForInterfaceMethod(pTargetMD));
            else
                RETURN((BYTE*)CRemotingServices::GetNonVirtualEntryPointForVirtualMethod(pTargetMD));

        RETURN(pTargetMD->GetMultiCallableAddrOfCode());
    }
    else
    {
        if (pStaticMD->IsInterface())
        {
            pTargetMD = MethodTable::GetMethodDescForInterfaceMethodAndServer(staticTH,pStaticMD,orThis);
        }
        else
        {
            if (pObjMT->IsThunking())
            {
                const BYTE *pTarget = pObjMT->GetSlot(pStaticMD->GetSlot());
                RETURN (pTarget);
            }

            pTargetMD = pObjMT->GetMethodDescForSlot(pStaticMD->GetSlot());
        }
    }
    RETURN (pTargetMD->GetMultiCallableAddrOfCode());
}

#ifdef _DEBUG
//*******************************************************************************
BOOL MethodDescMatchesBinderSig(MethodDesc* pMD, BinderMethodID id)
{
    WRAPPER_CONTRACT;

    PCCOR_SIGNATURE pSig;
    DWORD cSig;
    pMD->GetSig(&pSig, &cSig);

    PCCOR_SIGNATURE pBinarySig;
    DWORD           cBinarySig;
    g_Mscorlib.GetMethodSig(id)->GetBinarySig(&pBinarySig, &cBinarySig);

    return MetaSig::CompareMethodSigs(pBinarySig, cBinarySig, SystemDomain::SystemModule(), NULL,
                                      pSig, cSig, pMD->GetModule(), NULL);
}
#endif // _DEBUG

#ifdef CALLDESCR_REGTYPEMAP
//*******************************************************************************
void FillInRegTypeMap(ArgIterator* argit, BYTE* pMap, int* pRegArgNum, BYTE* pType, UINT32 structSize)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        PRECONDITION(CheckPointer(argit, NULL_NOT_OK));
        PRECONDITION(CheckPointer(pMap, NULL_NOT_OK));
        PRECONDITION(CheckPointer(pRegArgNum, NULL_NOT_OK));
        PRECONDITION(CheckPointer(pType, NULL_NOT_OK));
    }
    CONTRACTL_END;

    // Create a map of the first 8 argument types.  This is used in
    // CallDescrWorkerInternal to load args into general registers or
    // floating point registers.
    //
    // we put these in order from the LSB to the MSB so that we can keep
    // the map in a register and just examine the low byte and then shift
    // right for each arg.


    UINT32 count = ((UINT32)StackElemSize(structSize)) / STACK_ELEM_SIZE;

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
    if (count > ENREGISTERED_PARAMTYPE_MAXSIZE / STACK_ELEM_SIZE)
    {
        count = ENREGISTERED_PARAMTYPE_MAXSIZE / STACK_ELEM_SIZE;
    }
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

    while (count && ((*pRegArgNum) < NUM_ARGUMENT_REGISTERS))
    {
        (pMap)[(*pRegArgNum)++] = (*pType);
        count--;
    }
}
#endif // CALLDESCR_REGTYPEMAP

//*******************************************************************************
const BYTE* MethodDesc::GetCallTarget(OBJECTREF* pThisObj)
{
    CONTRACTL
    {
        THROWS;                 // Resolving a generic virtual method can throw
        GC_TRIGGERS;            
        MODE_COOPERATIVE;
    }
    CONTRACTL_END

    const BYTE* pTarget;

    if (IsVtableMethod() && !GetMethodTable()->IsValueType())
    {
        CONSISTENCY_CHECK(NULL != pThisObj);
        pTarget = GetSingleCallableAddrOfVirtualizedCode(pThisObj);
#ifdef DEBUG
        // For the cases where we ALWAYS want the Prestub, make certain that
        // the address we find in the Vtable actually points at the prestub.
        if (IsComPlusCall() || IsArray() || IsInterceptedForDeclSecurity() || IsRemotingInterceptedViaPrestub() || IsEnCMethod())
        {
            _ASSERTE(GetMethodTable()->GetUnknownMethodDescForSlotAddress(pTarget)->GetPrecode()->GetEntryPoint() == pTarget);
        }
#endif // DEBUG
    }
    else
    {
        pTarget = GetMethodEntryPoint();
    }

    return pTarget;
}

//*******************************************************************************
ARG_SLOT MethodDesc::CallTargetWorker(const BYTE* pTarget,
                                      MetaSig* pMetaSig,
                                      const ARG_SLOT *pArguments,
                                      BOOL fCriticalCall
                                      DEBUG_ARG(BOOL fPermitValueTypes))
{
    //
    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
    //
    // This method needs to have a GC_TRIGGERS contract because it 
    // calls managed code.  However, IT MAY NOT TRIGGER A GC ITSELF 
    // because the argument array is not protected and may contain gc
    // refs.
    //
    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
    //
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckActivated());             // EnsureActive will trigger, so we must already be activated
        PRECONDITION(!pMetaSig->NeedsSigWalk());    // ForceSigWalk will trigger, so we must already have done the walk
    }
    CONTRACTL_END;

    BOOL        fIsStatic;
    {
        // The incoming argument array is not protected!!
        GCX_FORBID();


        fIsStatic = IsStatic();
    }
    return CallDescr(pTarget, pMetaSig, pArguments, fIsStatic, fCriticalCall DEBUG_ARG(fPermitValueTypes));
}

#ifdef _DEBUG
extern int g_fMainThreadApartmentStateSet;
extern int g_fInitializingInitialAD;
extern int g_fInExecuteMainMethod;
#endif

//*******************************************************************************
ARG_SLOT MethodDesc::CallDescr(const BYTE *pTarget,
                               MetaSig* pMetaSigOrig,
                               const ARG_SLOT* pArguments,
                               BOOL fIsStatic,
                               BOOL fCriticalCall
                               DEBUG_ARG(BOOL fPermitValueTypes))
{
    //
    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
    //
    // This method needs to have a GC_TRIGGERS contract because it 
    // calls managed code.  However, IT MAY NOT TRIGGER A GC ITSELF 
    // because the argument array is not protected and may contain gc
    // refs.
    //
    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
    //
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_COOPERATIVE;
        PRECONDITION(GetAppDomain()->CheckCanExecuteManagedCode(this));
        PRECONDITION(CheckActivated());                 // EnsureActive will trigger, so we must already be activated
        PRECONDITION(!pMetaSigOrig->NeedsSigWalk());    // ForceSigWalk will trigger, so we must already have done the walk

        // If we're an exe, then we must either be initializing the first AD, or have already setup the main thread's
        //  COM apartment state.
        // If you hit this assert, then you likely introduced code during startup that could inadvertently 
        //  initialize the COM apartment state of the main thread before we set it based on the user attribute.
        PRECONDITION(g_fInExecuteMainMethod ? (g_fMainThreadApartmentStateSet || g_fInitializingInitialAD) : TRUE);
    }
    CONTRACTL_END;

    // stack allocate the MetaSig here so we can run 
    // the ctor in the GCX_FORBID region.
    MetaSig* pMetaSig = (MetaSig*)_alloca(sizeof(MetaSig));
    LPBYTE pFrameBase;
    UINT   nActualStackBytes;
    UINT   fpReturnSize;
#ifdef CALLDESCR_REGTYPEMAP
    UINT64 dwRegTypeMap;
#endif
#ifdef CALLDESCR_RETBUF
    void*   pvRetBuff = 0;
    UINT64  cbRetBuff = 0;
    BYTE    SmallVCBuff[ENREGISTERED_RETURNTYPE_MAXSIZE];
#endif // CALLDESCR_RETBUF

    {
        //
        // the incoming argument array is not gc-protected, so we 
        // may not trigger a GC before we actually call managed code
        //
        GCX_FORBID();

        // 
        // All types must already be loaded. This macro also sets up a FAULT_FORBID region which is
        // also required for critical calls since we cannot inject any failure points between the 
        // caller of MethodDesc::CallDescr and the actual transition to managed code.
        //
        ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();
        
        
        // Record this call if required
        g_IBCLogger.LogMethodDescAccess(this);

        // Make local copy as this function mutates iterator state.
        new (pMetaSig) MetaSig(pMetaSigOrig);

        _ASSERTE(GetAppDomain()->ShouldHaveCode());

        BYTE callingconvention = pMetaSig->GetCallingConvention();
        if (!isCallConv(callingconvention, IMAGE_CEE_CS_CALLCONV_DEFAULT))
        {
            _ASSERTE(!"This calling convention is not supported.");
            COMPlusThrow(kInvalidProgramException);
        }

        if (pMetaSig->GetCallingConventionInfo() & CORINFO_CALLCONV_PARAMTYPE)
        {
            _ASSERTE(!"PARAMTYPE calling convention is not supported.");
            COMPlusThrow(kInvalidProgramException);
        }

#ifdef DEBUGGING_SUPPORTED
        if (CORDebuggerTraceCall())
        {
            g_pDebugInterface->TraceCall(pTarget);
        }
#endif // DEBUGGING_SUPPORTED

#if CHECK_APP_DOMAIN_LEAKS
        if (g_pConfig->AppDomainLeaks())
        {
            // See if we are in the correct domain to call on the object
            if (!fIsStatic && !GetClass()->IsValueClass())
            {
                        CONTRACT_VIOLATION(ThrowsViolation|GCViolation|FaultViolation);
                OBJECTREF pThis = ArgSlotToObj(pArguments[0]);
                if (!pThis->AssignAppDomain(GetAppDomain()))
                    _ASSERTE(!"Attempt to call method on object in wrong domain");
            }
        }
#endif // CHECK_APP_DOMAIN_LEAKS

#ifdef _DEBUG
        {
            // Check to see that any value type args have been loaded and restored.
            // This is because we may be calling a FramedMethodFrame which will use the sig
            // to trace the args, but if any are unloaded we will be stuck if a GC occurs.
            _ASSERTE(IsRestored());
            CorElementType argType;
            while ((argType = pMetaSig->NextArg()) != ELEMENT_TYPE_END)
            {
                if (argType == ELEMENT_TYPE_VALUETYPE)
                {
                    TypeHandle th = pMetaSig->GetLastTypeHandleThrowing(ClassLoader::DontLoadTypes);
                    CONSISTENCY_CHECK(th.CheckFullyLoaded());
                    CONSISTENCY_CHECK(th.IsRestored());
                }
            }
            pMetaSig->Reset();
        }
#endif // _DEBUG

        DWORD   arg = 0;

        nActualStackBytes = pMetaSig->SizeOfFrameArgumentArray(fIsStatic);

        // Create a fake FramedMethodFrame on the stack.
        DWORD dwAllocaSize = 0;
        if (!ClrSafeInt<DWORD>::addition(FramedMethodFrame::GetRawNegSpaceSize(), sizeof(FramedMethodFrame), dwAllocaSize) ||
            !ClrSafeInt<DWORD>::addition(dwAllocaSize, nActualStackBytes, dwAllocaSize)) {
            _ASSERTE(!"Integer overflow calculating number of bytes needed on stack.");
            COMPlusThrow(kInvalidProgramException);
        }

        LPBYTE pAlloc = (LPBYTE)_alloca(dwAllocaSize);

        pFrameBase = pAlloc + FramedMethodFrame::GetRawNegSpaceSize();

#ifdef CALLDESCR_REGTYPEMAP
        dwRegTypeMap            = 0;
        BYTE*   pMap            = (BYTE*)&dwRegTypeMap;
        int     regArgNum       = 0;
#endif // CALLDESCR_REGTYPEMAP

        ArgIterator argit(pFrameBase, pMetaSig, fIsStatic);

        if (!fIsStatic)
        {
            *((LPVOID*) argit.GetThisAddr()) = ArgSlotToPtr(pArguments[arg++]);
#ifdef CALLDESCR_REGTYPEMAP
            regArgNum++;
#endif
        }

        if (pMetaSig->HasRetBuffArg())
        {
#ifdef CALLDESCR_RETBUF
            pvRetBuff = ArgSlotToPtr(pArguments[arg++]);
            cbRetBuff = pMetaSig->GetReturnTypeSize();
#else // !CALLDESCR_RETBUF
            *((LPVOID*) argit.GetRetBuffArgAddr()) = ArgSlotToPtr(pArguments[arg++]);
#endif // !CALLDESCR_RETBUF

#ifdef CALLDESCR_RETBUFMARK
            // the CallDescrWorker callsite for methods with return buffer is
            //  different for RISC CPUs - we pass this information along by setting
            //  the lowest bit in pTarget
            pTarget = (const BYTE *)((UINT_PTR)pTarget | 0x1);
#endif // CALLDESCR_RETBUFMARK

#if defined(CALLDESCR_REGTYPEMAP) && !defined(RETBUF_ARG_SPECIAL_PARAM)
            regArgNum++;
#endif
        }
#ifdef CALLDESCR_RETBUF
        else if (ELEMENT_TYPE_VALUETYPE == pMetaSig->GetReturnTypeNormalized())
        {
            ZeroMemory(SmallVCBuff, sizeof(SmallVCBuff));
            pvRetBuff = SmallVCBuff;
            cbRetBuff = pMetaSig->GetReturnTypeSize();

            _ASSERTE(cbRetBuff <= sizeof(SmallVCBuff));
        }
#endif // CALLDESCR_RETBUF


        BYTE   typ;
        UINT32 structSize;
        int    ofs;
        for( ; 0 != (ofs = argit.GetNextOffsetFaster(&typ, &structSize)); arg++ )
        {
#ifdef _DEBUG
            if (   !fPermitValueTypes
                && ELEMENT_TYPE_VALUETYPE == typ
                && structSize > sizeof(void*))
            {
                TypeHandle th = argit.GetArgType();
                ApproxFieldDescIterator fdIterator(th.GetMethodTable(), ApproxFieldDescIterator::INSTANCE_FIELDS);
                FieldDesc* pFD;

                while ((pFD = fdIterator.Next()) != NULL)
                {
                    CONSISTENCY_CHECK_MSG(!pFD->IsObjRef(),
                    "This might be an unsafe call to a method with a value type parameter.  If the value type contains no object references,"
                    " or if the contents will be protected by the caller when ENREGISTERED_PARAMTYPE_MAXSIZE is defined and the struct size"
                    " is > ENREGISTERED_PARAMTYPE_MAXSIZE, then change the call site to use MethodDesc::CallXXXWithValueTypes instead.\n");
                }
            }
#endif // _DEBUG

            UINT32 stackSize = structSize;

#ifdef CALLDESCR_REGTYPEMAP
            FillInRegTypeMap(&argit, pMap, &regArgNum, &typ, structSize);
#endif

#if CHECK_APP_DOMAIN_LEAKS
            // Make sure the arg is in the right app domain
            if (g_pConfig->AppDomainLeaks() && typ == ELEMENT_TYPE_CLASS) {
                        CONTRACT_VIOLATION(ThrowsViolation|GCViolation|FaultViolation);
                OBJECTREF objRef = ArgSlotToObj(pArguments[arg]);
                if (!objRef->AssignAppDomain(GetAppDomain()))
                    _ASSERTE(!"Attempt to pass object in wrong app domain to method");
            }
#endif // CHECK_APP_DOMAIN_LEAKS

            PVOID pDest = pFrameBase + ofs;

            {
                switch (stackSize)
                {
                    case 1:
                    case 2:
                    case 4:
                        *((INT32*)pDest) = (INT32)pArguments[arg];
                        break;

                    case 8:
#if !defined(_WIN64) && (DATA_ALIGNMENT > 4)
                        // the deregistered arguments are only 4-byte aligned
                        CopyMemory((INT32*)pDest, (INT32*)(&pArguments[arg]), 2 * sizeof(INT32));
#else // !(!defined(_WIN64) && (DATA_ALIGNMENT > 4))
                        *((INT64*)pDest) = pArguments[arg];
#endif // !(!defined(_WIN64) && (DATA_ALIGNMENT > 4))
                        break;

                    default:
                        // The ARG_SLOT contains a pointer to the value-type
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                        if (MetaSig::IsArgPassedByRef(stackSize))
                        {
                            *(PVOID*)pDest = ArgSlotToPtr(pArguments[arg]);
                        }
                        else
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE
                        if (stackSize>sizeof(ARG_SLOT))
                        {
                            CopyMemory(pDest, ArgSlotToPtr(pArguments[arg]), stackSize);
                        }
                        else
                        {
                            CopyMemory(pDest, (LPVOID) (&pArguments[arg]), stackSize);
                        }
                        break;
                }
            }
        }

        fpReturnSize = pMetaSig->GetFPReturnSize();

#ifdef _PPC_
        FramedMethodFrame::Enregister(pFrameBase, pMetaSig, fIsStatic, nActualStackBytes);
#endif

        UpdateFPReturnSizeForHFAReturn(pMetaSig, &fpReturnSize);

    } // END GCX_FORBID & ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE
    
    INT64 retval = 0;
    retval = CallDescrWorkerWithHandler(pFrameBase + sizeof(FramedMethodFrame)
#ifndef CALLDESCR_BOTTOMUP
                             + nActualStackBytes
#endif
                             ,
                             nActualStackBytes / STACK_ELEM_SIZE,
#ifdef CALLDESCR_ARGREGS
                             (ArgumentRegisters*)(pFrameBase + FramedMethodFrame::GetOffsetOfArgumentRegisters()),
#endif
#ifdef CALLDESCR_REGTYPEMAP
                             dwRegTypeMap,
#endif
#ifdef CALLDESCR_RETBUF
                             pvRetBuff,
                             cbRetBuff,
#endif // CALLDESCR_RETBUF
                             fpReturnSize,
                             (LPVOID)pTarget,
                             fCriticalCall);
    {
        GCX_FORBID();

#ifdef CALLDESCR_RETBUF
        if (cbRetBuff <= ENREGISTERED_RETURNTYPE_MAXSIZE)
        {
            memcpyNoGCRefs(GetThread()->m_SmallVCRetVal, SmallVCBuff, sizeof(SmallVCBuff));
        }
#endif // CALLDESCR_RETBUF

#if !defined(_WIN64) && BIGENDIAN
        if (!pMetaSig->Is64BitReturn())
        {
            retval >>= 32;
        }
#endif // !defined(_WIN64) && BIGENDIAN
    } // END GCX_FORBID
    
    return (ARG_SLOT) retval;
}

#endif // !DACCESS_COMPILE

//*******************************************************************************
// convert arbitrary IP location in jitted code to a MethodDesc
MethodDesc* IP2MethodDesc(const BYTE* IP)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    IJitManager* jitMan = ExecutionManager::FindJitMan((SLOT)IP);
    if (jitMan == 0)
        return(0);
    MethodDesc* pMD;
    jitMan->JitCodeToMethodInfo((SLOT)IP, &pMD);
    return pMD;
}

//*******************************************************************************
const BYTE* MethodDesc::GetMethodEntryPoint()
{
    WRAPPER_CONTRACT;

    return GetSafeAddrofCode();
}



#ifndef DACCESS_COMPILE

//*******************************************************************************
// convert an entry point into a method desc
MethodDesc* Entry2MethodDesc(const BYTE* entryPoint, MethodTable *pMT)
{
    CONTRACT(MethodDesc*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        POSTCONDITION(RETVAL->SanityCheck());
    }
    CONTRACT_END

    MethodDesc* method = IP2MethodDesc(entryPoint);
    if (method)
    {
        RETURN method;
    }

    method = StubManager::MethodDescFromEntry(entryPoint, pMT);
    if (method)
    {
        RETURN method;
    }

    // Is it an FCALL?
    method = ECall::MapTargetBackToMethod((TADDR)entryPoint);
    if (method != 0)
    {
        _ASSERTE(!method->IsPointingToNativeCode() || method->GetNativeCode() == (TADDR)entryPoint);
        RETURN(method);
    }

    // Its a stub
    method = MethodDesc::GetMethodDescFromStubAddr((TADDR)entryPoint);

    RETURN (method);
}

//*******************************************************************************
BOOL MethodDesc::IsFCallOrIntrinsic()
{
    WRAPPER_CONTRACT;
    return (IsFCall() || IsArray());
}

//*******************************************************************************
BOOL MethodDesc::IsPointingToPrestub()
{
    WRAPPER_CONTRACT;

    if (!HasStableEntryPoint())
        return TRUE;

    if (!HasPrecode())
        return FALSE;

    if (!IsRestored())
        return TRUE;

    return GetPrecode()->IsPointingToPrestub();
}

//*******************************************************************************
// Returns true if the code (compiled, prejitted, internal implementation) is ready.
// Else the prestub still needs to be executed to get the code.
BOOL MethodDesc::IsCodeReady()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // For simple (IL/fcall) functions, GetSafeAddrofCode() cannot still point to the prestub
    // (IsIL() || IsFCall())) is used because DoPrestub() uses it. Is it really required?
    if (IsSafeToHandoutJittedCode() && (IsIL() || IsFCall()))
        return TRUE;

    // For functions requiring stubs, GetSafeAddrofCode() may be pointing to
    // the prestub. Hence, we need to inspect the prestub
    if (IsPointingToPrestub())
    {
        return FALSE;
    }

    return TRUE;
}

//*******************************************************************************
void MethodDesc::Reset()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END

    // This method is not thread-safe since we are updating
    // different pieces of data non-atomically.
    // Use this only if you can guarantee thread-safety somehow.

    _ASSERTE(IsEnCMethod() || // The process is frozen by the debugger
             IsDynamicMethod() || // These are used in a very restricted way
             GetLoaderModule()->IsReflection()); // Rental methods                                                                 

    // Reset any flags relevant to the old code
    ClearFlagsOnUpdate();

    if (HasPrecode())
    {
        GetPrecode()->Reset();
    }
    else
    {
        // We should go here only for the rental methods
        _ASSERTE(GetLoaderModule()->IsReflection());

        InterlockedUpdateFlags2(enum_flag2_HasStableEntryPoint | enum_flag2_HasPrecode, FALSE);

        *GetAddrOfSlotUnchecked() = GetTemporaryEntryPoint();
    }

    _ASSERTE(!HasNativeCode());
}

//*******************************************************************************
DWORD MethodDesc::GetSecurityFlagsDuringPreStub()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END


    DWORD dwMethDeclFlags       = 0;
    DWORD dwMethNullDeclFlags   = 0;
    DWORD dwClassDeclFlags      = 0;
    DWORD dwClassNullDeclFlags  = 0;

    if (IsInterceptedForDeclSecurity())
    {
        HRESULT hr;

        BOOL fHasSuppressUnmanagedCodeAccessAttr = HasSuppressUnmanagedCodeAccessAttr();;

        hr = Security::GetDeclarationFlags(GetMDImport(),
                                           GetMemberDef(),
                                           &dwMethDeclFlags,
                                           &dwMethNullDeclFlags,
                                           &fHasSuppressUnmanagedCodeAccessAttr);
        if (FAILED(hr))
            COMPlusThrowHR(hr);

        // We only care about runtime actions, here.
        // Don't add security interceptors for anything else!
        dwMethDeclFlags     &= DECLSEC_RUNTIME_ACTIONS;
        dwMethNullDeclFlags &= DECLSEC_RUNTIME_ACTIONS;
    }

    MethodTable *pMT = GetMethodTable();
    if (!pMT->IsNoSecurityProperties())
    {
        EEClass *pCl = GetClass();
        if (pCl)
        {
            g_IBCLogger.LogEEClassAndMethodTableAccess(pCl);

            PSecurityProperties pSecurityProperties = pCl->GetSecurityProperties();
            _ASSERTE(pSecurityProperties);
            if (pSecurityProperties)
            {
                dwClassDeclFlags    = pSecurityProperties->GetRuntimeActions();
                dwClassNullDeclFlags= pSecurityProperties->GetNullRuntimeActions();
            }
        }
    }
    else
    {
        _ASSERTE( GetClass()->GetSecurityProperties()->GetRuntimeActions() == 0
            && GetClass()->GetSecurityProperties()->GetNullRuntimeActions() == 0 );
    }


    // Build up a set of flags to indicate the actions, if any,
    // for which we will need to set up an interceptor.

    // Add up the total runtime declarative actions so far.
    DWORD dwSecurityFlags = dwMethDeclFlags | dwClassDeclFlags;

    // Add in a declarative demand for NDirect.
    // If this demand has been overridden by a declarative check
    // on a class or method, then the bit won't change. If it's
    // overridden by an empty check, then it will be reset by the
    // subtraction logic below.
    if (IsNDirect())
    {
        dwSecurityFlags |= DECLSEC_UNMNGD_ACCESS_DEMAND;
    }

    if (dwSecurityFlags)
    {
        // If we've found any declarative actions at this point,
        // try to subtract any actions that are empty.

            // Subtract out any empty declarative actions on the method.
        dwSecurityFlags &= ~dwMethNullDeclFlags;

        // Finally subtract out any empty declarative actions on the class,
        // but only those actions that are not also declared by the method.
        dwSecurityFlags &= ~(dwClassNullDeclFlags & ~dwMethDeclFlags);
    }

    return dwSecurityFlags;
}

//*******************************************************************************
DWORD MethodDesc::GetSecurityFlagsDuringClassLoad(IMDInternalImport *pInternalImport,
                                                  mdToken tkMethod,
                                                  mdToken tkClass,
                                                  DWORD *pdwClassDeclFlags,
                                                  DWORD *pdwClassNullDeclFlags,
                                                  DWORD *pdwMethDeclFlags,
                                                  DWORD *pdwMethNullDeclFlags)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END

    HRESULT hr;

    hr = Security::GetDeclarationFlags(pInternalImport,
                                               tkMethod,
                                               pdwMethDeclFlags,
                                               pdwMethNullDeclFlags);
    if (FAILED(hr))
          COMPlusThrowHR(hr);


    if (!IsNilToken(tkClass) && (*pdwClassDeclFlags == 0xffffffff || *pdwClassNullDeclFlags == 0xffffffff))
    {
        hr = Security::GetDeclarationFlags(pInternalImport,
                                           tkClass,
                                           pdwClassDeclFlags,
                                           pdwClassNullDeclFlags);
        if (FAILED(hr))
            COMPlusThrowHR(hr);

    }

    // Build up a set of flags to indicate the actions, if any,
    // for which we will need to set up an interceptor.

    // Add up the total runtime declarative actions so far.
    DWORD dwSecurityFlags = *pdwMethDeclFlags | *pdwClassDeclFlags;

    // Add in a declarative demand for NDirect.
    // If this demand has been overridden by a declarative check
    // on a class or method, then the bit won't change. If it's
    // overridden by an empty check, then it will be reset by the
    // subtraction logic below.
    if (IsNDirect())
    {
        dwSecurityFlags |= DECLSEC_UNMNGD_ACCESS_DEMAND;
    }

    if (dwSecurityFlags)
    {
        // If we've found any declarative actions at this point,
        // try to subtract any actions that are empty.

            // Subtract out any empty declarative actions on the method.
        dwSecurityFlags &= ~*pdwMethNullDeclFlags;

        // Finally subtract out any empty declarative actions on the class,
        // but only those actions that are not also declared by the method.
        dwSecurityFlags &= ~(*pdwClassNullDeclFlags & ~*pdwMethDeclFlags);
    }

    return dwSecurityFlags;
}

//*******************************************************************************
Dictionary* MethodDesc::GetMethodDictionary()
{
    WRAPPER_CONTRACT;

    return
        ((GetClassification() == mcInstantiated) && !IsUnboxingStub())
        ? (Dictionary*) (AsInstantiatedMethodDesc()->IMD_GetMethodInstantiation())
        : NULL;
}

//*******************************************************************************
DictionaryLayout* MethodDesc::GetDictionaryLayout()
{
    WRAPPER_CONTRACT;

    return
        (GetClassification() == mcInstantiated)
        ? AsInstantiatedMethodDesc()->IMD_GetDictionaryLayout()
        : NULL;
}

#endif // !DACCESS_COMPILE

//*******************************************************************************
MethodImpl *MethodDesc::GetMethodImpl()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(IsMethodImpl());
    }
    CONTRACTL_END

    // MethodImpl immediately follows the method desc
    SIZE_T size = s_ClassificationSizeTable[GetClassification()];
    return PTR_MethodImpl(PTR_HOST_TO_TADDR(this) + size);
}

//*******************************************************************************
void MethodDesc::CheckRestore(ClassLoadLevel level)
{
    LEAF_CONTRACT;
}

//*******************************************************************************
BOOL MethodDesc::IsRestored()
{
    LEAF_CONTRACT;
    return TRUE;
}

#ifndef DACCESS_COMPILE
#endif // !DACCESS_COMPILE


#ifdef HAS_COMPACT_ENTRYPOINTS

#if defined(_X86_)

#include <pshpack1.h>
static const struct CentralJumpCode {
    BYTE m_movzxEAX[3];
    BYTE m_shlEAX[3];
    BYTE m_addEAX[1];
    MethodDesc* m_pBaseMD;
    BYTE m_jmp[1];
    INT32 m_rel32;

    inline void Setup(MethodDesc* pMD, TADDR target, BaseDomain* pDomain) {
        LEAF_CONTRACT;
        m_pBaseMD = pMD;
        m_rel32 = rel32UsingJumpStub(&m_rel32, target, pMD, pDomain);
    }

    inline BOOL CheckTarget(TADDR target) {
        LEAF_CONTRACT;
        TADDR addr = rel32Decode(PTR_HOST_MEMBER_TADDR(CentralJumpCode, this, m_rel32));
        return (addr == target);
    }
}
c_CentralJumpCode = { 
    { 0x0F, 0xB6, 0xC0 },            // movzx eax,al
    { 0xC1, 0xE0, 0x03 },            // shl eax, 3
    { 0x05 }, NULL,                  // add eax, pBaseMD
    { 0xE9 }, 0                      // jmp PreStub
};
#include <poppack.h>

#else
#error Unsupported platform
#endif

typedef DPTR(struct CentralJumpCode) PTR_CentralJumpCode;
#define TEP_CENTRAL_JUMP_SIZE   sizeof(c_CentralJumpCode)
C_ASSERT((TEP_CENTRAL_JUMP_SIZE & 1) == 0);

#define TEP_ENTRY_SIZE          4
#define TEP_MAX_BEFORE_INDEX    (1 + (127 / TEP_ENTRY_SIZE))
#define TEP_MAX_BLOCK_INDEX     (TEP_MAX_BEFORE_INDEX + (128 - TEP_CENTRAL_JUMP_SIZE) / TEP_ENTRY_SIZE)
#define TEP_FULL_BLOCK_SIZE     (TEP_MAX_BLOCK_INDEX * TEP_ENTRY_SIZE + TEP_CENTRAL_JUMP_SIZE)

//*******************************************************************************
/* static */ MethodDesc* MethodDescChunk::GetMethodDescFromCompactEntryPoint(TADDR addr, BOOL fSpeculative /*=FALSE*/)
{
    LEAF_CONTRACT;

#ifdef DACCESS_COMPILE
    // Always use speculative checks with DAC
    fSpeculative = TRUE;
#endif

    // Always do consistency check in debug
    if (fSpeculative INDEBUG(|| TRUE))
    {
        if ((addr & 3) != 1 ||
            *PTR_BYTE(addr) != X86_INSTR_MOV_AL ||
            *PTR_BYTE(addr+2) != X86_INSTR_JMP_REL8)
        {
            if (fSpeculative) return NULL;
            _ASSERTE(!"Unexpected code in temporary entrypoint");
        }
    }

    int index = *PTR_BYTE(addr+1);
    TADDR centralJump = addr + 4 + *PTR_SBYTE(addr+3);

    CentralJumpCode* pCentralJumpCode = PTR_CentralJumpCode(centralJump);

    // Always do consistency check in debug
    if (fSpeculative INDEBUG(|| TRUE))
    {
        SIZE_T i;
        for (i = 0; i < TEP_CENTRAL_JUMP_SIZE; i++)
        {
            BYTE b = ((BYTE*)&c_CentralJumpCode)[i];
            if (b != 0 && b != *PTR_BYTE(centralJump+i))
            {
                if (fSpeculative) return NULL;
                _ASSERTE(!"Unexpected code in temporary entrypoint");
            }
        }

        _ASSERTE_IMPL(pCentralJumpCode->CheckTarget((TADDR)ThePreStub()->GetEntryPoint()));
    }

    MethodDesc* pMD = PTR_MethodDesc((TADDR)pCentralJumpCode->m_pBaseMD + index * METHOD_ALIGN);

#ifdef _DEBUG_IMPL
    if (!fSpeculative)
    {
        // Simple validation
        MethodDescChunk* pChunk = pMD->GetMethodDescChunk();
        _ASSERTE_IMPL(pChunk->GetMethodDescAt(pMD->GetMethodDescIndex() / pChunk->GetUnit()) == pMD);
    }
#endif

    return pMD;
}

//*******************************************************************************
SIZE_T MethodDescChunk::SizeOfCompactEntryPoints(int count)
{
    LEAF_CONTRACT;

    int fullBlocks = count / TEP_MAX_BLOCK_INDEX;
    int remainder = count % TEP_MAX_BLOCK_INDEX;

    return 1 + (fullBlocks * TEP_FULL_BLOCK_SIZE) +
        (remainder * TEP_ENTRY_SIZE) + ((remainder != 0) ? TEP_CENTRAL_JUMP_SIZE : 0);
}

#ifndef DACCESS_COMPILE
TADDR MethodDescChunk::AllocateCompactEntryPoints(BaseDomain* pDomain, AllocMemTracker *pamTracker)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    int count = GetCount();

    SIZE_T size = SizeOfCompactEntryPoints(count);

    TADDR temporaryEntryPoints = (TADDR)pamTracker->Track(pDomain->GetHighFrequencyHeap()->AllocMem(size));

    // make the temporary entrypoints unaligned, so they are easy to identify
    BYTE* p = (BYTE*)temporaryEntryPoints + 1;

    int indexInBlock = TEP_MAX_BLOCK_INDEX; // recompute relOffset in first iteration
    int relOffset = 0; // relative offset for the short jump
    int blockStart = 0; // index of the start of the block

    for (int index = 0; index < count; index++)
    {
        if (indexInBlock == TEP_MAX_BLOCK_INDEX)
        {
            relOffset = (min(count - index, TEP_MAX_BEFORE_INDEX) - 1) * TEP_ENTRY_SIZE;
            indexInBlock = 0;
            blockStart = index;
        }

        *(p+0) = X86_INSTR_MOV_AL;
        _ASSERTE(FitsInU1(indexInBlock * GetUnit()));
        *(p+1) = (BYTE)(indexInBlock * GetUnit());

        *(p+2) = X86_INSTR_JMP_REL8;
        _ASSERTE(FitsInI1(relOffset));
        *(p+3) = (BYTE)relOffset;

        p += TEP_ENTRY_SIZE; C_ASSERT(TEP_ENTRY_SIZE == 4);

        if (relOffset == 0)
        {
            CentralJumpCode* pCode = (CentralJumpCode*)p;

            memcpy(pCode, &c_CentralJumpCode, TEP_CENTRAL_JUMP_SIZE);

            pCode->Setup(GetMethodDescAt(blockStart), (TADDR)ThePreStub()->GetEntryPoint(), pDomain);

            p += TEP_CENTRAL_JUMP_SIZE;

            relOffset -= TEP_CENTRAL_JUMP_SIZE;
        }

        relOffset -= TEP_ENTRY_SIZE;
        indexInBlock++;
    }

    _ASSERTE(p == (BYTE*)temporaryEntryPoints + size);

    SetHasCompactEntryPoints();
    return temporaryEntryPoints;
}
#endif // !DACCESS_COMPILE

#endif // HAS_COMPACT_ENTRYPOINTS

//*******************************************************************************
TADDR MethodDescChunk::GetTemporaryEntryPoint(int index)
{
    LEAF_CONTRACT;

    _ASSERTE(HasTemporaryEntryPoints());

#ifdef HAS_COMPACT_ENTRYPOINTS
    if (HasCompactEntryPoints())
    {
        int fullBlocks = index / TEP_MAX_BLOCK_INDEX;
        int remainder = index % TEP_MAX_BLOCK_INDEX;

        return m_temporaryEntryPoints + 1 + (fullBlocks * TEP_FULL_BLOCK_SIZE) +
            (remainder * TEP_ENTRY_SIZE) + ((remainder >= TEP_MAX_BEFORE_INDEX) ? TEP_CENTRAL_JUMP_SIZE : 0);
    }
#endif // HAS_COMPACT_ENTRYPOINTS

    return Precode::GetPrecodeForTemporaryEntryPoint(m_temporaryEntryPoints, index, HasNativeCodeSlots())->GetEntryPoint();
}

#ifndef DACCESS_COMPILE
//*******************************************************************************
void MethodDesc::SetTemporaryEntryPoint(BaseDomain *pDomain, AllocMemTracker *pamTracker)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    GetMethodDescChunk()->EnsureTemporaryEntryPointsCreated(pDomain, pamTracker);

    PTR_TADDR pSlot = GetAddrOfSlotUnchecked();
    _ASSERTE(*pSlot == NULL);
    *pSlot = GetTemporaryEntryPoint();

    if (RequiresStableEntryPoint())
    {
        // The rest of the system assumes that certain methods always have stable entrypoints.
        // Create them now.
        GetOrCreatePrecode();
    }
}

//*******************************************************************************
void MethodDescChunk::CreateTemporaryEntryPoints(BaseDomain *pDomain, AllocMemTracker *pamTracker)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(m_temporaryEntryPoints == NULL);

    m_temporaryEntryPoints = Precode::AllocateTemporaryEntryPoints(this, pDomain, pamTracker);

#ifdef HAS_COMPACT_ENTRYPOINTS
    // Precodes allocated only if they provide more compact representation or if it is required
    if (m_temporaryEntryPoints == NULL)
    {
        m_temporaryEntryPoints = AllocateCompactEntryPoints(pDomain, pamTracker);        
    }
#endif // HAS_COMPACT_ENTRYPOINTS

    _ASSERTE(m_temporaryEntryPoints != NULL);
}

//*******************************************************************************
void MethodDesc::InterlockedUpdateFlags2(BYTE bMask, BOOL fSet)
{
    WRAPPER_CONTRACT;

    ULONG* pLong = (ULONG*)(&m_bFlags2 - 3);
    C_ASSERT(offsetof(MethodDesc, m_bFlags2) % sizeof(LONG) == 3);

    if (fSet)
        FastInterlockOr(pLong, (ULONG)bMask << (3 * 8));
    else
        FastInterlockAnd(pLong, ~((ULONG)bMask << (3 * 8)));
}

//*******************************************************************************
Precode* MethodDesc::GetOrCreatePrecode()
{
    WRAPPER_CONTRACT;

    if (HasPrecode())
    {
        return GetPrecode();
    }

    PTR_TADDR pSlot = GetAddrOfSlotUnchecked();
    TADDR tempEntry = GetTemporaryEntryPoint();

    PrecodeType requiredType = PRECODE_STUB;
    PrecodeType availableType = PRECODE_INVALID;

#ifdef HAS_REMOTING_PRECODE
    if (IsRemotingInterceptedViaPrestub())
    {
        requiredType = PRECODE_REMOTING;
    }
#endif // HAS_REMOTING_PRECODE

    if (!GetMethodDescChunk()->HasCompactEntryPoints())
    {
        availableType = Precode::GetPrecodeFromEntryPoint(tempEntry)->GetType();
    }

    BOOL fMayHaveNativeCode = MayHaveNativeCode();

    // Allocate the precode if necessary
    if (requiredType != availableType || (fMayHaveNativeCode && !GetMethodDescChunk()->HasNativeCodeSlots()))
    {
        AllocMemTracker amt;
        Precode* pPrecode = Precode::Allocate(requiredType, this, fMayHaveNativeCode, GetDomain(), &amt);
        if (FastInterlockCompareExchangePointer((void**)pSlot, (void*) pPrecode->GetEntryPoint(), (void*)tempEntry) == (void*)tempEntry)
            amt.SuppressRelease();
    }

    // Set the flags atomically
    InterlockedUpdateFlags2(enum_flag2_HasStableEntryPoint | enum_flag2_HasPrecode, TRUE);

    return Precode::GetPrecodeFromEntryPoint(*pSlot);
}

//*******************************************************************************
BOOL MethodDesc::SetStableEntryPointInterlocked(TADDR addr)
{
    LEAF_CONTRACT;

    _ASSERTE(!HasPrecode());

    TADDR pExpected = GetTemporaryEntryPoint();
    PTR_TADDR pSlot = GetAddrOfSlotUnchecked();

    BOOL fResult = FastInterlockCompareExchangePointer((void*volatile*)pSlot, (void*)addr, (void*)pExpected) == (void*)pExpected;

    InterlockedUpdateFlags2(enum_flag2_HasStableEntryPoint, TRUE);

    return fResult;
}

#endif // DACCESS_COMPILE


#ifndef DACCESS_COMPILE
//*******************************************************************************
//
// Returns a callable entry point for a function.
// Multiple entry points could be used for a single function.
// ie. this function is not idempotent
//

// We must ensure that GetMultiCallableAddrOfCode works
// correctly for all of the following cases:
// 1.   shared generic method instantiations
// 2.   unshared generic method instantiations
// 3.   instance methods in shared generic classes
// 4.   instance methods in unshared generic classes
// 5.   static methods in shared generic classes.
// 6.   static methods in unshared generic classes.
//
// For case 1 and 5 the methods are implemented using
// an instantiating stub (i.e. IsInstantiatingStub()
// should be true).  These stubs pass on to
// shared-generic-code-which-requires-an-extra-type-context-parameter.
// So whenever we use LDFTN on these we need to give out
// the address of an instantiating stub.  In this case we can just
// give out GetCallablePreStubAddr().
//
// For cases 2, 3, 4 and 6 we can just use the standard technique for LdFtn:
// (for 2 we give out the address of the fake "slot" in InstantiatedMethodDescs)
// (for 3 it doesn't matter if the code is shared between instantiations
// because the instantiation context is picked up from the "this" parameter.)

const BYTE* MethodDesc::GetMultiCallableAddrOfCode(BOOL fAllowInstParam /*=FALSE*/)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    // Record this method desc if required
    g_IBCLogger.LogMethodDescAccess(this);

    if (IsGenericMethodDefinition())
    {
        _ASSERTE(!"Cannot take the address of an uninstantiated generic method.");
        COMPlusThrow(kInvalidProgramException);
    }

    if (!fAllowInstParam)
    {
        // Whenever we use LDFTN on shared-generic-code-which-requires-an-extra-parameter
        // we need to give out the address of an instantiating stub.  This is why we give
        // out GetCallablePreStubAddr() for the IsInstantiatingStub() case: this is
        // safe.  But first we assert that we only use GetMultiCallableAddrOfCode on
        // the instantiating stubs and not on the shared code itself.
        _ASSERTE(!RequiresInstArg());
        _ASSERTE(!IsSharedByGenericMethodInstantiations());
    }

    if (IsWrapperStub() || IsEnCAddedMethod())
        return GetCallablePreStubAddr();

    if (IsRemotingInterceptedViaVirtualDispatch())
        return (BYTE*)CRemotingServices::GetNonVirtualEntryPointForVirtualMethod(this);

    // If the method has already been jitted, we can give out the direct address
    // Note that we may have previously created a FuncPtrStubEntry, but
    // GetMultiCallableAddrOfCode() does not need to be idempotent.

    if (IsSafeToHandoutJittedCode())
        return (BYTE*)GetNativeCode();

    if (HasStableEntryPoint())
        return (BYTE*)GetStableEntryPoint();

    // Force the creation of the precode if we would eventually got one anyway
    if (MayHavePrecode())
        return (BYTE*)GetOrCreatePrecode()->GetEntryPoint();

#ifdef HAS_COMPACT_ENTRYPOINTS
    //
    // We don't have stable entrypoint, we don't have a slot,
    // so get a FuncPtrStubEntry
    //
    return GetDomain()->GetFuncPtrStubs()->GetFuncPtrStub(this);
#else // HAS_COMPACT_ENTRYPOINTS
    //
    // Embed call to the temporary entrypoint into the code. It will be patched 
    // to point to the actual code later.
    //
    INDEBUG(IncPermittedPrestubCalls());
    return (BYTE*)GetTemporaryEntryPoint();
#endif // HAS_COMPACT_ENTRYPOINTS

}

//*******************************************************************************
void NDirectMethodDesc::InterlockedSetNDirectFlags(WORD wFlags)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // Since InterlockedCompareExchange only works on ULONGs, 
    // we'll have to operate on the entire ULONG. Ugh.

    WORD *pFlags = &ndirect.m_wFlags;

    // Make sure that m_flags is aligned on a 4 byte boundry
    _ASSERTE( ( ((size_t) pFlags) & (sizeof(ULONG)-1) ) == 0);

    // Ensure we won't be reading or writing outside the bounds of the NDirectMethodDesc.
    _ASSERTE((BYTE*)pFlags >= (BYTE*)this);
    _ASSERTE((BYTE*)pFlags+sizeof(ULONG) < (BYTE*)(this+1));

    DWORD dwMask = 0;

    // Set the flags in the mask
    ((WORD*)&dwMask)[0] |= wFlags;

    // Now, slam all 32 bits atomically.
    FastInterlockOr((DWORD*)pFlags, dwMask);
}

//*******************************************************************************
BOOL NDirectMethodDesc::InterlockedReplaceMLHeader(MLHeader *pMLHeader, MLHeader *pOldMLHeader)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE(IsNDirect());
    MLHeader *pPrevML = (MLHeader*)FastInterlockCompareExchangePointer( (void**)&GetWriteableData()->m_pMLHeader,
                                                                 (void*)pMLHeader, (void*)pOldMLHeader );
    return pPrevML == pOldMLHeader;
}

//*******************************************************************************
bool NDirectMethodDesc::FindEntryPoint( HINSTANCE hMod, UINT16 numParamBytes, void ** retFunc, SString * retFuncName ) const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    char const * funcName = NULL;
    bool retVal = false;
    
    FARPROC pFunc = NULL, pFuncW = NULL;

#ifndef PLATFORM_UNIX
    // Handle ordinals.
    if (GetEntrypointName()[0] == '#')
    {
        long ordinal = atol(GetEntrypointName()+1);
        *retFunc = (void*) GetProcAddress(hMod, (LPCSTR)(size_t)((UINT16)ordinal));
        return false;
    }
#endif  // !PLATFORM_UNIX

    // Just look for the unmangled name.  If it is unicode fcn, we are going
    // to need to check for the 'W' API because it takes precedence over the
    // unmangled one (on NT some APIs have unmangled ANSI exports).
    pFunc = GetProcAddress(hMod, funcName = GetEntrypointName());
    if ((pFunc != NULL && IsNativeAnsi()) || IsNativeNoMangled())
    {
        *retFunc = (void*) pFunc;
        if ( pFunc && retFuncName )
        {
            *retFuncName = SString(SString::Ansi, funcName);
            retVal = true;
        }

        return retVal;
    }

    // Allocate space for a copy of the entry point name.
    int dstbufsize = (int)(sizeof(char) * (strlen(GetEntrypointName()) + 1 + 20)); // +1 for the null terminator
                                                                         // +20 for various decorations

    // Allocate a single character before the start of this string to enable quickly
    // prepending a '_' character if we look for a stdcall entrypoint later on.
    LPSTR szAnsiEntrypointName = ((LPSTR)_alloca(dstbufsize + 1)) + 1;

    // Copy the name so we can mangle it.
    strcpy_s(szAnsiEntrypointName,dstbufsize,GetEntrypointName());
    DWORD nbytes = (DWORD)(strlen(GetEntrypointName()) + 1);
    szAnsiEntrypointName[nbytes] = '\0'; // Add an extra '\0'.


    // If the program wants the ANSI api or if Unicode APIs are unavailable.
    if (IsNativeAnsi())
    {
        szAnsiEntrypointName[nbytes-1] = 'A';
        pFunc = GetProcAddress(hMod, funcName = szAnsiEntrypointName);
    }
    else
    {
        szAnsiEntrypointName[nbytes-1] = 'W';
        pFuncW = GetProcAddress(hMod, szAnsiEntrypointName);

        // This overrides the unmangled API. See the comment above.
        if (pFuncW != NULL)
        {
            pFunc = pFuncW;
            funcName = szAnsiEntrypointName;
        }
    }

    if (!pFunc)
    {

#ifndef PLATFORM_UNIX
        /* try mangled names only for __stdcalls */

        if (!pFunc && (numParamBytes != 0xffff))
        {
            szAnsiEntrypointName[-1] = '_';
            sprintf_s(szAnsiEntrypointName + nbytes - 1, dstbufsize - (nbytes - 1), "@%ld", (ULONG)numParamBytes);
            pFunc = GetProcAddress(hMod, funcName = szAnsiEntrypointName - 1);
        }
#endif // !PLATFORM_UNIX
    }

    *retFunc = (void*) pFunc;
    if ( pFunc && retFuncName )
    {
        *retFuncName = SString(SString::Ansi, funcName);
        retVal = true;
    }

    return retVal;
}


//*******************************************************************************
void MethodDesc::ComputeSuppressUnmanagedCodeAccessAttr(IMDInternalImport *pImport)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    // We only care about this bit for NDirect and ComPlusCall
    if (!IsNDirect() && !IsComPlusCall())
        return;

    HRESULT hr = pImport->GetCustomAttributeByName(GetMemberDef(),
                                                    COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                    NULL,
                                                    NULL);
    IfFailThrow(hr);

    if (IsNDirect())
        ((NDirectMethodDesc*)this)->SetSuppressUnmanagedCodeAccessAttr(hr == S_OK);

}


//*******************************************************************************
BOOL MethodDesc::HasSuppressUnmanagedCodeAccessAttr()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    if (IsNDirect())
        return ((NDirectMethodDesc*)this)->HasSuppressUnmanagedCodeAccessAttr();
    else
        return FALSE;
}



#endif // !DACCESS_COMPILE


#ifdef DACCESS_COMPILE

//*******************************************************************************
void
MethodDesc::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{

    if (DacHasMethodDescBeenEnumerated(this))
    {
        return;
    }
    
    // Save away the whole MethodDescChunk as in many
    // places RecoverChunk is called on a method desc so
    // the whole chunk must be available.  This also
    // automatically picks up any prestubs and such.
    GetMethodDescChunk()->EnumMemoryRegions(flags);

    if (HasPrecode())
    {
        GetPrecode()->EnumMemoryRegions(flags);
    }

    if (g_pDebugInfoStore.IsValid())
    {
        // Need to save the Debug-Info for this method so that we can see it in a debugger later.
        g_pDebugInfoStore->EnumMemoryRegionsForMethodDebugInfo(flags, this);
    }

    if (!IsNoMetadata() ||IsILStub())
    {
        // The assembling of the string below implicitly dumps the memory we need.
        
        StackSString str;
        TypeString::AppendMethodInternal(str, this, TypeString::FormatSignature|TypeString::FormatNamespace|TypeString::FormatFullInst);

        // The module path is used in the output of !clrstack and !pe if the  
        // module is not available when the minidump is inspected. By retrieving
        // the path here, the required memory is implicitly dumped.
        Module* pModule = GetModule();
        if (pModule)
        {
            pModule->GetPath();
        }
    }

    // Also, call DacValidateMD to dump the memory it needs. !clrstack calls 
    // DacValidateMD before it retrieves the method name. We don't expect 
    // DacValidateMD to fail, but if it does, ignore the failure and try to assemble the 
    // string anyway so that clients that don't validate the MD still work.

    DacValidateMD(this);

    DacSetMethodDescEnumerated(this);
    
}

//*******************************************************************************
void
StoredSigMethodDesc::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    // 'this' already done, see below.
    DacEnumMemoryRegion(m_pSig, m_cSig);
}

//*******************************************************************************
void
MethodDescChunk::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_CHECK_ENUM_THIS();
    EMEM_OUT(("MEM: %p MethodDescChunk\n", PTR_HOST_TO_TADDR(this)));

    DacEnumMemoryRegion(PTR_HOST_TO_TADDR(this), Sizeof());

    if (m_methodTable.IsValid())
    {
        m_methodTable->EnumMemoryRegions(flags);
    }

    if (m_temporaryEntryPoints != NULL)
    {
        SIZE_T size;

#ifdef HAS_COMPACT_ENTRYPOINTS
        if (HasCompactEntryPoints())
        {
            size = SizeOfCompactEntryPoints(GetCount());
        }
        else
#endif // HAS_COMPACT_ENTRYPOINTS
        {
            size = Precode::SizeOfTemporaryEntryPoints(m_temporaryEntryPoints, GetCount(), HasNativeCodeSlots());
        }

        DacEnumMemoryRegion(m_temporaryEntryPoints, size);
    }


    for (UINT i = 0; i < GetCount(); i++)
    {
        PAL_TRY
        {
            MethodDesc* methodDesc = GetMethodDescAt(i);

            if (methodDesc->IsMethodImpl())
            {
                methodDesc->GetMethodImpl()->EnumMemoryRegions(flags);
            }

            if (methodDesc->HasStoredSig())
            {
                StoredSigMethodDesc* sigMd = PTR_StoredSigMethodDesc(PTR_HOST_TO_TADDR(methodDesc));
                sigMd->EnumMemoryRegions(flags);
            }
        }
        PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
        {
        }
        PAL_ENDTRY
    }
}


#endif // DACCESS_COMPILE

#ifndef DACCESS_COMPILE
//*******************************************************************************
MethodDesc *MethodDesc::GetInterfaceMD()
{
    CONTRACT (MethodDesc*) {
        THROWS;
        GC_TRIGGERS;
        INSTANCE_CHECK;
        PRECONDITION(!IsInterface());
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;
    MethodTable *pMT = GetMethodTable();
    RETURN(pMT->ReverseInterfaceMDLookup(GetSlot()));
}
#endif // !DACCESS_COMPILE

