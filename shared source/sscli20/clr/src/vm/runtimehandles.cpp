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
#include "corhdr.h"
#include "runtimehandles.h"
#include "object.h"
#include "class.h"
#include "class.inl"
#include "method.hpp"
#include "typehandle.h"
#include "field.h"
#include "comstring.h"
#include "siginfo.hpp"
#include "clsload.hpp"
#include "typestring.h"
#include "typeparse.h"
#include "holder.h"
#include "codeman.h"
#include "corhlpr.h"
#include "jitinterface.h"
#include "stackprobe.h"
#include "eeconfig.h"
#include "eehash.h"
#include "objecthandle.h"
#include "interoputil.h"
#include "typedesc.h"
#include "virtualcallstub.h"
#include "contractimpl.h"
#include "dynamicmethod.h"
#include "peimagelayout.inl"
#include "security.h"
#include "comnlsinfo.h"

FCIMPL3(FC_BOOL_RET, Utf8String::EqualsCaseInsensitive, LPCUTF8 szLhs, LPCUTF8 szRhs, INT32 stringNumBytes)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(szLhs));
        PRECONDITION(CheckPointer(szRhs));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Important: the string in pSsz isn't null terminated so the length must be used
    // when performing operations on the string.
    
    BOOL fStringsEqual = FALSE;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    {
        HELPER_METHOD_FRAME_BEGIN_RET_0();

        // At this point, both the left and right strings are guaranteed to have the
        // same length. 
        SString lhs(SString::Utf8, szLhs, stringNumBytes);
        SString rhs(SString::Utf8, szRhs, stringNumBytes);

        if ( RunningOnWinNT() || (lhs.IsASCII() && rhs.IsASCII()))
        {
            // We can use SString for simple case insensitive compares

            fStringsEqual = lhs.EqualsCaseInsensitive(rhs);
        }
        else
        {
            // Must use NLS helper for nontrivial case insensitive comparison on 9x machines.


            InternalCasingHelper::InitTable();

            fStringsEqual 
              = (InternalCasingHelper::InvariantCaseInsensitiveCompare(
                            lhs.GetUnicode(), lhs.GetCount(),
                            rhs.GetUnicode(), rhs.GetCount()) == 0);

        }

        HELPER_METHOD_FRAME_END();

    }
    END_SO_INTOLERANT_CODE;

    FC_RETURN_BOOL(fStringsEqual);
}
FCIMPLEND

FCIMPL3(FC_BOOL_RET, Utf8String::EqualsCaseSensitive, LPCUTF8 szLhs, LPCUTF8 szRhs, INT32 stringNumBytes)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(szLhs));
        PRECONDITION(CheckPointer(szRhs));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Important: the string in pSsz isn't null terminated so the length must be used
    // when performing operations on the string.

    // At this point, both the left and right strings are guaranteed to have the
    // same length.
    FC_RETURN_BOOL(strncmp(szLhs, szRhs, stringNumBytes) == 0);
}
FCIMPLEND

static BOOL IsVisibleFromXXX(MethodDesc* pTargetMD, MethodTable* sourceMT, Module* pModule)
{
    MethodTable* pTargetMT = pTargetMD->GetMethodTable();
    DWORD dwAttr = pTargetMD->GetAttrs();
    
    if (!(IsTdPublic(pTargetMT->GetClass()->GetProtection())      // If parent class is public
         && !pTargetMT->GetClass()->IsNested()                  // and is not nested
         && !pTargetMT->GetClass()->HasInstantiation()          // and has no instantiation
         && (IsMdPublic(dwAttr)                                 // and the method itself is public but has no instantiation
             && (pTargetMD!=NULL) 
             && !pTargetMD->HasMethodInstantiation())))
    {
        return ClassLoader::CanAccess(
            NULL, sourceMT, pModule->GetAssembly(), 
            pTargetMT, pTargetMT->GetAssembly(), 
            sourceMT, dwAttr, pTargetMD, NULL);
    }

    return TRUE;
}

FCIMPL2(FC_BOOL_RET, RuntimeMethodHandle::IsVisibleFromModule, MethodDesc **ppMethod, Module* pModule)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;

    BOOL bResult = TRUE;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        bResult = IsVisibleFromXXX(*ppMethod, NULL, pModule); 
    }
    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(bResult);
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, RuntimeMethodHandle::IsVisibleFromType, MethodDesc **ppMethod, EnregisteredTypeHandle enregTypeHandle)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;

    BOOL bResult = TRUE;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        TypeHandle sourceHandle = TypeHandle::FromPtr(enregTypeHandle);
        bResult = IsVisibleFromXXX(*ppMethod, sourceHandle.GetMethodTable(), sourceHandle.GetModule()); 
    }
    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(bResult);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeMethodHandle::IsILStub, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMethod = *ppMethod;

    FC_RETURN_BOOL(pMethod->IsILStub());
}
FCIMPLEND

FCIMPL3(void, RuntimeMethodHandle::CheckLinktimeDemands, MethodDesc **ppMethod, Module* pModule, INT32 tkToken) 
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMethod = *ppMethod;

    if (!pMethod->RequiresLinktimeCheck())
        return;
    
    OBJECTREF Throwable = NULL;
    HELPER_METHOD_FRAME_BEGIN_1(Throwable);
    {
        if (!Security::LinktimeCheckMethod(pModule->GetAssembly(), pMethod, &Throwable))
            COMPlusThrow(Throwable);
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND
    
FCIMPL1(MethodDescChunk*, MethodDescChunkHandle::GetNextMethodDescChunk, MethodDescChunk **ppChunk) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppChunk));
        PRECONDITION(CheckPointer(*ppChunk));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDescChunk *pChunk = *ppChunk;

    if (pChunk == NULL)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    return pChunk->GetNextChunk();
}
FCIMPLEND

FCIMPL2(MethodDesc*, MethodDescChunkHandle::GetMethodAt, MethodDescChunk **ppChunk, INT32 index) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppChunk));
        PRECONDITION(CheckPointer(*ppChunk));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDescChunk *pChunk = *ppChunk;

    if (pChunk == NULL)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    MethodDesc *pMethod = pChunk->GetMethodDescAt(index);
    if (!pMethod->IsRestored()) {
        HELPER_METHOD_FRAME_BEGIN_RET_0();
        pMethod->CheckRestore();
        HELPER_METHOD_FRAME_END();
    }
    return pMethod;
}
FCIMPLEND

FCIMPL1(INT32, MethodDescChunkHandle::GetMethodCount, MethodDescChunk **ppChunk) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppChunk));
        PRECONDITION(CheckPointer(*ppChunk));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDescChunk *pChunk = *ppChunk;

    if (pChunk == NULL)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    return (INT32)pChunk->GetCount();
}
FCIMPLEND

FCIMPL3(void, FastArray::SetValueAt, PtrArray* pPtrArrayUNSAFE, INT32 index, Object* objectUNSAFE) { 
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    PTRARRAYREF pPtrArray = (PTRARRAYREF)pPtrArrayUNSAFE;
    OBJECTREF object = (OBJECTREF)objectUNSAFE;
    
    _ASSERTE(index < (INT32)pPtrArray->GetNumComponents());
    
    pPtrArray->SetAt(index, object);
}
FCIMPLEND

/*
FCIMPL2(void, FastArray::CopyArray, PtrArray* pSrc, PtrArray* pDst) { 
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    BYTE* src = pSrc->GetDataPtr();
    BYTE* dst = pDst->GetDataPtr();
    INT32 size = pSrc->GetNumComponents();
    m_memmove(dst, src, size * sizeof(VOID*));

    GCHeap::GetGCHeap()->SetCardsAfterBulkCopy((Object**)dst, size * sizeof(VOID*));
}
FCIMPLEND
*/

FCIMPL1(EnregisteredTypeHandle, RuntimeTypeHandle::GetTrueTypeHandle, Object* objectUNSAFE) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (NULL == objectUNSAFE)
        FCThrow(kArgumentNullException);

    OBJECTREF object = (OBJECTREF)objectUNSAFE;
    TypeHandle th = object->GetTrueTypeHandle();
    return th.AsPtr();
}
FCIMPLEND

FCIMPL1(Object*, RuntimeTypeHandle::GetRuntimeType, void* th) { 
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    OBJECTREF refType = NULL;

    TypeHandle typeHandle = TypeHandle::FromPtr(th);
    TypeHandle* pTypeHandle = &typeHandle;

    _ASSERTE(CheckPointer(pTypeHandle));
    _ASSERTE(CheckPointer(pTypeHandle->AsPtr(), NULL_OK));

    if (pTypeHandle->AsPtr() == NULL)
        return NULL;

    refType = pTypeHandle->GetManagedClassObjectIfExists();

    if (refType != NULL)
        return OBJECTREFToObject(refType);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refType);
    
    refType = pTypeHandle->GetManagedClassObject();
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refType);
}
FCIMPLEND

FCIMPL1(LPVOID, RuntimeTypeHandle::TypeHandle_get, Object* o) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // This is only a dummy method we use to have an intrinsic. Nobody is expected to go thru here.
    // Remove when intrinsics can live with out fcalls
    FCThrowArgument(NULL, NULL);
}
FCIMPLEND

FCIMPL1(void, RuntimeTypeHandle::PrepareMemberInfoCache, MethodTable *pMemberInfoCache) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    TypeHandle thMemberInfoCache(pMemberInfoCache);
    _ASSERTE(thMemberInfoCache.GetNumGenericArgs() == 1);
    _ASSERTE(!thMemberInfoCache.IsGenericTypeDefinition());
    TypeHandle *pthInstantiation = thMemberInfoCache.GetInstantiation();

    // Not being a generic type definition and having 1 generic arg means
    // that this must have an instantiation.
    PREFIX_ASSUME(pthInstantiation != NULL);

    // Have to lookup method to prepare in a slightly roundabout manner since
    // the binder doesn't support nested classes. The caller passes us in the
    // type handle for the MemberInfoCache class and we lookup the insert method
    // manually. Cache this since we'll come here for each instantiation (but
    // only use the cache for reference based instantiations, the rest won't
    // shared a method desc).
    static MethodDesc  *pCachedMD = NULL;
    MethodDesc         *pMD = NULL;

    if (pCachedMD == NULL || !CorTypeInfo::IsObjRef(pthInstantiation->GetInternalCorElementType())) {

        // Need to look for the method on the canonical method table (all the
        // rest don't have the methods present).
        MethodTable::MethodIterator it(thMemberInfoCache.AsMethodTable()->GetCanonicalMethodTable());
        for (; it.IsValid(); it.Next()) {
            if (it.IsVirtual())
                continue;
            if (strcmp(it.GetMethodDesc()->GetName(), "Insert") == 0) {
                pMD = it.GetMethodDesc();
                if (CorTypeInfo::IsObjRef(pthInstantiation->GetInternalCorElementType()))
                    pCachedMD = pMD;
#ifdef _DEBUG
                // Check for overloads.
                for (it.Next(); it.IsValid(); it.Next())
                    _ASSERTE(strcmp(it.GetMethodDesc()->GetName(), "Insert"));
#endif
                break;
            }
        }
        _ASSERTE(pMD != NULL);
    } else
        pMD = pCachedMD;

    SigTypeContext sTypeContext(1, pthInstantiation, 0, NULL);
    MethodCallGraphPreparer mcgp(pMD, &sTypeContext, true, true);
    mcgp.Run();

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(void*, RuntimeTypeHandle::GetMethodDescChunk, TypeHandle *pTypeHandle) {
    CONTRACTL {
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    TypeHandle typeHandle = *pTypeHandle;
    
    if (typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
        
    if (typeHandle.IsTypeDesc()) {
        if (!typeHandle.IsArray()) 
            return NULL;
    }

    MethodTable* pMT = typeHandle.GetMethodTable();
    if (!pMT)
        return NULL;
    
    return (void*)pMT->GetClass()->GetChunks();
}
FCIMPLEND

FCIMPL1(INT32, RuntimeTypeHandle::GetCorElementType, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    return pTypeHandle->GetSignatureCorElementType();
}
FCIMPLEND

FCIMPL1(DomainAssembly*, RuntimeTypeHandle::GetAssemblyHandle, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    DomainFile *pDomainFile = NULL;
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    {
        Module *pModule = pTypeHandle->GetAssembly()->GetManifestModule();

            pDomainFile = pModule->FindDomainFile(GetAppDomain());
        
        if (pDomainFile == NULL)
        {
            HELPER_METHOD_FRAME_BEGIN_RET_0();
            
            pDomainFile = GetAppDomain()->LoadDomainNeutralModuleDependency(pModule, FILE_LOADED);

            HELPER_METHOD_FRAME_END();
        }
    }
    END_SO_INTOLERANT_CODE;

    return (DomainAssembly *) pDomainFile;
}
FCIMPLEND


FCIMPL1(FC_BOOL_RET, RuntimeFieldHandle::AcquiresContextFromThis, FieldDesc **ppField)
{
    CONTRACTL {
        DISABLED(NOTHROW);
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppField));
        PRECONDITION(CheckPointer(*ppField));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    FC_RETURN_BOOL((*ppField)->IsSharedByGenericInstantiations());

}
FCIMPLEND


FCIMPL1(Module*, RuntimeTypeHandle::GetModuleHandle, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    Module *result;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));

    result = pTypeHandle->GetModule();

    END_SO_INTOLERANT_CODE;

    return result;
}
FCIMPLEND

FCIMPL1(EnregisteredTypeHandle, RuntimeTypeHandle::GetBaseTypeHandle, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
        
    if (typeHandle.IsTypeDesc()) {
        if (!typeHandle.IsArray()) 
            return NULL;
    }
    
    return typeHandle.GetParent().AsPtr();
}
FCIMPLEND
     
FCIMPL1(EnregisteredTypeHandle, RuntimeTypeHandle::GetElementType, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    if (!pTypeHandle->IsTypeDesc())
        return 0;   

    if (pTypeHandle->IsGenericVariable())
        return 0;

    if (pTypeHandle->IsArray()) 
        return pTypeHandle->AsArray()->GetArrayElementTypeHandle().AsPtr();

    return pTypeHandle->AsTypeDesc()->GetTypeParam().AsPtr();
}
FCIMPLEND
            
FCIMPL1(INT32, RuntimeTypeHandle::GetArrayRank, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        PRECONDITION(pTypeHandle->IsArray());
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return (INT32)pTypeHandle->AsArray()->GetRank();   
}
FCIMPLEND

FCIMPL1(INT32, RuntimeTypeHandle::GetNumVtableSlots, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);

    if (typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
    
    MethodTable *pMT = typeHandle.GetMethodTable();

    if (pMT) 
        return (INT32)pMT->GetNumVirtuals();
    else
        return 0;
}
FCIMPLEND

FCIMPL1(INT32, RuntimeTypeHandle::GetInterfaceMethodSlots, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        PRECONDITION(pTypeHandle->IsInterface());
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);

    if (typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
    
    return (INT32)typeHandle.GetMethodTable()->GetNumMethods();
}
FCIMPLEND

FCIMPL2(void*, RuntimeTypeHandle::GetMethodAt, TypeHandle *pTypeHandle, INT32 slot) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    void* pRet = NULL;

    if (typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    if (slot < 0 || slot >= (INT32)typeHandle.GetMethodTable()->GetNumMethods())
        FCThrowRes(kArgumentException, L"Arg_ArgumentOutOfRangeException");      

    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    {
        pRet = typeHandle.GetMethodTable()->GetMethodDescForSlot((DWORD)slot);
    }
    END_SO_INTOLERANT_CODE;

    return pRet;
}
FCIMPLEND

FCIMPL3(FC_BOOL_RET, RuntimeTypeHandle::GetFields, TypeHandle *pTypeHandle, INT32 **result, INT32 *pCount) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    if (!pCount || !result)
        FCThrow(kArgumentNullException);

    if (typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
    
    if (typeHandle.IsTypeDesc()) {
        *pCount = 0;
        FC_RETURN_BOOL(TRUE);
    }

    MethodTable *pMT= typeHandle.GetMethodTable();
    if (!pMT)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
    
    BOOL retVal = FALSE;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    ApproxFieldDescIterator fdIterator(pMT, ApproxFieldDescIterator::ALL_FIELDS, TRUE);
    INT32 count = (INT32)fdIterator.Count();

    if (count > *pCount) 
    {
        *pCount = count;
    } 
    else 
    {
        for(INT32 i = 0; i < count; i ++)
            result[i] = (INT32*)fdIterator.Next();
        
        *pCount = count;
        retVal = TRUE;
    }
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

FCIMPL1(StringObject*, RuntimeMethodHandle::ConstructInstantiation, MethodDesc** ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMethod = *ppMethod;
    STRINGREF refName = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    StackSString ss;
    TypeString::AppendInst(ss, pMethod->GetNumGenericMethodArgs(), 
        pMethod->LoadMethodInstantiation(), TypeString::FormatBasic);    
    refName = COMString::NewString(ss);
    HELPER_METHOD_FRAME_END();
    
    return (StringObject*)( OBJECTREFToObject( refName ));
}
FCIMPLEND

FCIMPL4(StringObject*, RuntimeTypeHandle::ConstructName, TypeHandle *pTypeHandle, CLR_BOOL fNamespace, CLR_BOOL fFullInst, CLR_BOOL fAssembly) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
      
    StringObject *refName = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refName);
    {
        refName = TypeString::ConstructName(*pTypeHandle, (fNamespace ? TypeString::FormatNamespace : 0) |
                                                          (fFullInst ? TypeString::FormatFullInst : 0) |
                                                          (fAssembly ? TypeString::FormatAssembly : 0));
    }
    HELPER_METHOD_FRAME_END();

    return refName;
}
FCIMPLEND

FCIMPL1(ArrayBase*, RuntimeTypeHandle::GetConstraints, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    if (!typeHandle.IsTypeDesc())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
    
    if (!typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
    
  
    INT32 dwCount = 0; 
  
    BASEARRAYREF refRetVal  = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);
    {
        TypeHandle* constraints = NULL;

        TypeVarTypeDesc* pGenericVariable = typeHandle.AsGenericVariable();              
        constraints = pGenericVariable->GetConstraints((DWORD*)&dwCount);

        // Allocate the array
        TypeHandle thRuntimeTypeHandle = TypeHandle(g_Mscorlib.GetClass(CLASS__TYPE_HANDLE));
        TypeHandle arrayHandle = ClassLoader::LoadArrayTypeThrowing(thRuntimeTypeHandle, ELEMENT_TYPE_SZARRAY);
        refRetVal = (BASEARRAYREF)AllocateArrayEx(arrayHandle, &dwCount, 1);
    
        if (dwCount > 0)
        {
            // populate type array
            TypeHandle *pElem = (TypeHandle*)refRetVal->GetDataPtr();        

            for (INT32 i = 0; i < dwCount; i++) {
                pElem[i] = constraints[i];
                _ASSERTE(!pElem[i].IsNull());
            }
        }
    }
    HELPER_METHOD_FRAME_END();

    return (ArrayBase*)OBJECTREFToObject(refRetVal);   
}
FCIMPLEND

FCIMPL1(ArrayBase*, RuntimeTypeHandle::GetInterfaces, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
  
  if (typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    INT32 ifaceCount = 0; 
  
    BASEARRAYREF refRetVal  = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);
    {
        if (typeHandle.IsTypeDesc())
        {
            if (typeHandle.IsArray())
            {
                ifaceCount = typeHandle.GetMethodTable()->GetNumInterfaces();            
            }
            else
            {
                ifaceCount = 0;
            }
        }
        else
        {
            ifaceCount = typeHandle.GetMethodTable()->GetNumInterfaces();
        }

        // Allocate the array
        if (ifaceCount > 0)
        {            
            TypeHandle thRuntimeTypeHandle = TypeHandle(g_Mscorlib.GetClass(CLASS__TYPE_HANDLE));
            TypeHandle arrayHandle = ClassLoader::LoadArrayTypeThrowing(thRuntimeTypeHandle, ELEMENT_TYPE_SZARRAY);
            refRetVal = (BASEARRAYREF)AllocateArrayEx(arrayHandle, &ifaceCount, 1);
        
            if (ifaceCount > 0)
            {
                // populate type array
                TypeHandle *pElem = (TypeHandle*)refRetVal->GetDataPtr();        

                MethodTable::InterfaceMapIterator it = typeHandle.GetMethodTable()->IterateInterfaceMap();
                while (it.Next())
                {
                    TypeHandle iface(it.GetInterface());
                    pElem[it.GetIndex()] = iface;
                    _ASSERTE(!pElem[it.GetIndex()].IsNull());
                }
            }
        }
    }
    HELPER_METHOD_FRAME_END();

    return (ArrayBase*)OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(INT32, RuntimeTypeHandle::GetAttributes, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
      
    if (typeHandle.IsTypeDesc()) {

        if (typeHandle.IsGenericVariable()) {
            return tdPublic;        
        }
    
        if (!typeHandle.IsArray()) 
            return 0;
    }


    INT32 ret = 0;
    
    ret = (INT32)typeHandle.GetClass()->GetAttrClass();
    return ret;
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeTypeHandle::IsContextful, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;       
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsGenericVariable())
        FC_RETURN_BOOL(FALSE);

    if (typeHandle.IsTypeDesc()) {
        if (!typeHandle.IsArray()) 
            FC_RETURN_BOOL(FALSE);
    }  
    
    MethodTable* pMT= typeHandle.GetMethodTable();
    
    if (!pMT)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    FC_RETURN_BOOL(pMT->IsContextful());
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeTypeHandle::IsVisible, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;       
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);

    BOOL bIsExternallyVisible = FALSE;
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    {
        bIsExternallyVisible = typeHandle.IsExternallyVisible();
    }
    END_SO_INTOLERANT_CODE;

    FC_RETURN_BOOL(bIsExternallyVisible);
}
FCIMPLEND
    
FCIMPL2(FC_BOOL_RET, RuntimeTypeHandle::IsVisibleFromModule, TypeHandle *pTypeHandle, Module* pModule)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;

    BOOL bResult = TRUE;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        // Note that pTypeHandle is the this argument for an instance method on
        // a valuetype (other than those inherited from ValueType and Object), 
        // and is thus guaranteed to be non-null.
        MethodTable* pTargetMT = pTypeHandle->GetMethodTable();

        // And since TypeHandle is an unshared type, it has an unshared MT,
        // and so GetMethodTable always returns non-null.
        PREFIX_ASSUME(pTargetMT != NULL);

        bResult = ClassLoader::CanAccessClass(
            NULL, NULL, pModule->GetAssembly(),
            pTargetMT, pTargetMT->GetAssembly());
    }
    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(bResult);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeTypeHandle::HasProxyAttribute, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsGenericVariable())
        FC_RETURN_BOOL(FALSE);
        
    if (typeHandle.IsTypeDesc()) {
        if (!typeHandle.IsArray()) 
            FC_RETURN_BOOL(FALSE);
    }  
    
    MethodTable* pMT= typeHandle.GetMethodTable();
    
    if (!pMT) 
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    FC_RETURN_BOOL(pMT->GetClass()->HasRemotingProxyAttribute());
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, RuntimeTypeHandle::IsComObject, TypeHandle *pTypeHandle, CLR_BOOL isGenericCOM) {
    CONTRACTL {
        DISABLED(NOTHROW);
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
    }
    CONTRACTL_END;
    FCUnique(0x37);
    FC_RETURN_BOOL(FALSE);
}
FCIMPLEND

FCIMPL1(LPCUTF8, RuntimeTypeHandle::GetUtf8Name, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    INT32 tkTypeDef = mdTypeDefNil;
    LPCUTF8 szName = NULL;

    if (typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
        
    if (typeHandle.IsTypeDesc()) 
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    MethodTable* pMT= typeHandle.GetMethodTable();
    
    if (!pMT) 
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    tkTypeDef = (INT32)pMT->GetCl();
    
    if (IsNilToken(tkTypeDef))
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    pMT->GetMDImport()->GetNameOfTypeDef(tkTypeDef, &szName, NULL);

    _ASSERTE(CheckPointer(szName, NULL_OK));
    
    return szName;
}
FCIMPLEND

FCIMPL1(INT32, RuntimeTypeHandle::GetToken, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    INT32 tkTypeDef = mdTypeDefNil;

    if (typeHandle.IsGenericVariable())
    {
        tkTypeDef = typeHandle.AsGenericVariable()->GetToken();
        
        _ASSERTE(!IsNilToken(tkTypeDef) && TypeFromToken(tkTypeDef) == mdtGenericParam);

        return tkTypeDef;
    }
        
    if (typeHandle.IsTypeDesc()) 
        return mdTypeDefNil;

    if (typeHandle.HasInstantiation())
    {
        HELPER_METHOD_FRAME_BEGIN_RET_0();
        {            
            typeHandle = ClassLoader::LoadTypeDefThrowing(
                typeHandle.GetModule(), typeHandle.GetMethodTable()->GetCl(), ClassLoader::ThrowIfNotFound, ClassLoader::PermitUninstDefOrRef);
        }
        HELPER_METHOD_FRAME_END();
    }

    MethodTable* pMT= typeHandle.GetMethodTable();
    
    if (!pMT) 
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    tkTypeDef = (INT32)pMT->GetCl();
    
    if (IsNilToken(tkTypeDef))
        return mdTypeDefNil; 

    return tkTypeDef;
}
FCIMPLEND

FCIMPL2(LPVOID, RuntimeTypeHandle::GetGCHandle, TypeHandle *pTypeHandle, int handleType) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    OBJECTHANDLE objHandle = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    if((objHandle = typeHandle.GetDomain()->CreateTypedHandle(NULL, handleType)) == NULL)
        COMPlusThrowOM();
    
    HELPER_METHOD_FRAME_END();
    return objHandle;
}
FCIMPLEND

FCIMPL2(void, RuntimeTypeHandle::FreeGCHandle, TypeHandle *pTypeHandle, OBJECTHANDLE handle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer((void*)handle));
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();
    DestroyTypedHandle(handle);
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL2(LPVOID, RuntimeTypeHandle::GetMethodFromToken, TypeHandle *pTypeHandle, INT32 methodDef) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); 
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);

    if (typeHandle.IsTypeDesc()) {
        if (!typeHandle.IsArray()) 
            FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
    }

    LPVOID pMethod = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    pMethod = (void*)typeHandle.GetMethodTable()->GetClass()->FindMethod((UINT32)methodDef);
    HELPER_METHOD_FRAME_END();
    return pMethod;
}
FCIMPLEND

FCIMPL2(INT32, RuntimeTypeHandle::GetFirstSlotForInterface, TypeHandle *pTypeHandle, EnregisteredTypeHandle enregIfaceHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle = *pTypeHandle;
    TypeHandle ifaceHandle = TypeHandle::FromPtr(enregIfaceHandle);

    if (typeHandle.IsNull() || ifaceHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    if (typeHandle.IsGenericVariable())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    INT32 retVal = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    
    if (typeHandle.IsTypeDesc()) {
        if (!typeHandle.IsArray())
            COMPlusThrow(kArgumentException, L"Arg_NotFoundIFace");
    }

    if (typeHandle.IsInterface())
        COMPlusThrow(kArgumentException, L"Argument_InterfaceMap");

    if (!ifaceHandle.IsInterface())
        COMPlusThrow(kArgumentException, L"Arg_MustBeInterface");

    if (!typeHandle.GetMethodTable()->ImplementsInterface(ifaceHandle.AsMethodTable()))
        COMPlusThrow(kArgumentException, L"Arg_NotFoundIFace");
    retVal = (INT32)(-1);

    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND

FCIMPL3(INT32, RuntimeTypeHandle::GetInterfaceMethodImplementationSlot, TypeHandle *pTypeHandle, void *owner, void *md) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(md));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    } CONTRACTL_END;

    UINT32 slotNumber = (UINT32)(-1);
    TypeHandle typeHandle = *pTypeHandle;
    TypeHandle thOwnerOfMD = TypeHandle::FromPtr(owner);
    MethodDesc *pMD = (MethodDesc *) md;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        DispatchSlot slot(typeHandle.GetMethodTable()->FindDispatchSlotForInterfaceMD(thOwnerOfMD, pMD));
        if (!slot.IsNull()) {
            slotNumber = slot.GetMethodDesc()->GetSlot();
        }
    }
    HELPER_METHOD_FRAME_END();
    return (INT32) slotNumber;
}
FCIMPLEND
    
FCIMPL1(MethodDesc*, RuntimeTypeHandle::GetDefaultConstructor, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);

    if (typeHandle.IsTypeDesc())
        return NULL;
    
    if (typeHandle.IsGenericVariable())
        return NULL;
    
    MethodDesc* pCtor = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        MethodTable* pMethodTable = typeHandle.AsMethodTable();
        if (pMethodTable->HasDefaultConstructor())
            pCtor = pMethodTable->GetDefaultConstructor();
    }
    HELPER_METHOD_FRAME_END();

    return pCtor ;
}
FCIMPLEND

FCIMPL1(MethodDesc*, RuntimeTypeHandle::GetDeclaringMethod, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);

    if (!typeHandle.IsTypeDesc())
        return NULL;
    
    if (!typeHandle.IsGenericVariable())
        return NULL;

    TypeVarTypeDesc* pGenericVariable = typeHandle.AsGenericVariable();
    mdToken defToken = pGenericVariable->GetTypeOrMethodDef();
    
    if (TypeFromToken(defToken) != mdtMethodDef)
        return NULL;

    return pGenericVariable->GetModule()->LookupMethodDef(defToken);
}
FCIMPLEND

FCIMPL1(EnregisteredTypeHandle, RuntimeTypeHandle::GetDeclaringType, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    TypeHandle retTypeHandle;

    BOOL fThrowException = FALSE;
    LPCWSTR argName = L"Arg_InvalidHandle";
    RuntimeExceptionKind reKind = kArgumentNullException;

    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    MethodTable* pMT = NULL;
    mdTypeDef tkTypeDef = mdTokenNil;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    if (typeHandle.IsTypeDesc()) {

        if (typeHandle.IsGenericVariable()) {
            TypeVarTypeDesc* pGenericVariable = typeHandle.AsGenericVariable();
            mdToken defToken = pGenericVariable->GetTypeOrMethodDef();
            
            if (TypeFromToken(defToken) == mdtMethodDef)
                defToken = pGenericVariable->GetModule()->LookupMethodDef(defToken)->GetMethodTable()->GetCl();

            // Try the fast way first (if the declaring type has been loaded already).
            retTypeHandle = pGenericVariable->GetModule()->LookupTypeDef(defToken);
            if (!retTypeHandle.IsNull())
                goto Exit;

            // OK, need to go the slow way and load the type first.
            HELPER_METHOD_FRAME_BEGIN_RET_0();
            {
                retTypeHandle = ClassLoader::LoadTypeDefThrowing(pGenericVariable->GetModule(), defToken, 
                                                                 ClassLoader::ThrowIfNotFound, 
                                                                 ClassLoader::PermitUninstDefOrRef);
            }
            HELPER_METHOD_FRAME_END();
            goto Exit;
        }
        if (!typeHandle.IsArray())
        {
            retTypeHandle = TypeHandle();
            goto Exit;
        }
    }  
    
    pMT = typeHandle.GetMethodTable();

    if (!pMT) 
    {
        fThrowException = TRUE;
        goto Exit;
    }

    if(!pMT->GetClass()->IsNested())
    {
        retTypeHandle = TypeHandle();
        goto Exit;
    }

    tkTypeDef = pMT->GetCl();

    typeHandle.GetModule()->GetMDImport()->GetNestedClassProps(tkTypeDef, &tkTypeDef);

    // Try the fast way first (if the declaring type has been loaded already).
    retTypeHandle = typeHandle.GetModule()->LookupTypeDef(tkTypeDef);
    if (retTypeHandle.IsNull())
    { 
         // OK, need to go the slow way and load the type first.
        HELPER_METHOD_FRAME_BEGIN_RET_0();
        {
            retTypeHandle = ClassLoader::LoadTypeDefThrowing(typeHandle.GetModule(), tkTypeDef, 
                                                             ClassLoader::ThrowIfNotFound, 
                                                             ClassLoader::PermitUninstDefOrRef);
        }
        HELPER_METHOD_FRAME_END();
    }
Exit:

    END_SO_INTOLERANT_CODE;

    if (fThrowException)
    {
        FCThrowRes(reKind, argName);
    }
    return retTypeHandle.AsPtr();
    
  }
FCIMPLEND

FCIMPL2(FC_BOOL_RET, RuntimeTypeHandle::CanCastTo, TypeHandle *pTypeHandle, EnregisteredTypeHandle pToHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        PRECONDITION(CheckPointer(pToHandle));
        SO_TOLERANT;                
    }
    CONTRACTL_END;
    
    TypeHandle fromHandle = *pTypeHandle;
    TypeHandle toHandle = TypeHandle::FromPtr(pToHandle);
    
    if (fromHandle.IsNull() || toHandle.IsNull()) 
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    BOOL iRetVal = 0;

    if (fromHandle.IsGenericVariable()) {       
        HELPER_METHOD_FRAME_BEGIN_RET_0();
        fromHandle.AsGenericVariable()->LoadConstraints();        
        iRetVal = fromHandle.CanCastTo(toHandle);
        HELPER_METHOD_FRAME_END();
    }
    else
    {
        BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));

        TypeHandle::CastResult r = fromHandle.CanCastToNoGC(toHandle);
        if (r == TypeHandle::MaybeCast)
        {
            HELPER_METHOD_FRAME_BEGIN_RET_0();

            TypeHandle rootElementType = fromHandle;
            while (rootElementType.IsTypeDesc()) 
            {
                if (rootElementType.IsGenericVariable() || rootElementType.IsFnPtrType())
                    break;

                if (rootElementType.IsArray()) 
                {
                    rootElementType = rootElementType.AsArray()->GetArrayElementTypeHandle();
                }
                else
                {
                    rootElementType = rootElementType.AsTypeDesc()->GetTypeParam();
                }
            }

            if (rootElementType.IsGenericVariable())
                rootElementType.AsGenericVariable()->LoadConstraints();        

            iRetVal = fromHandle.CanCastTo(toHandle);
            HELPER_METHOD_FRAME_END();
        }
        else
        {
            iRetVal = (r == TypeHandle::CanCast);
        }

        // We allow T to be cast to Nullable<T>
        if (!iRetVal && Nullable::IsNullableForType(toHandle, fromHandle.GetMethodTable())) 
        {
            iRetVal = TRUE;
        }

        END_SO_INTOLERANT_CODE;
    }
        
    FC_RETURN_BOOL(iRetVal);
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, RuntimeTypeHandle::SupportsInterface, TypeHandle *pTypeHandle, Object* objUNSAFE) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    }
    CONTRACTL_END;
    
    BOOL iRetVal = FALSE;
    TypeHandle typeHandle(*pTypeHandle);
    OBJECTREF obj = (OBJECTREF) objUNSAFE;

    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
        
    if (obj == NULL)
        FCThrowRes(kArgumentNullException, L"ArgumentNull_Obj");

    if (typeHandle.IsTypeDesc()) {
        if (!typeHandle.IsArray()) 
            FC_RETURN_BOOL(FALSE);
    }

    MethodTable* pMT = typeHandle.GetMethodTable();

    if (!pMT) // Defensive test
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    HELPER_METHOD_FRAME_BEGIN_RET_1(obj);

    iRetVal = Object::SupportsInterface(obj, pMT);
    
    HELPER_METHOD_FRAME_END();
    
    FC_RETURN_BOOL(iRetVal);
}
FCIMPLEND


FCIMPL2(EnregisteredTypeHandle, RuntimeTypeHandle::GetTypeByNameUsingCARules, StringObject* classNameUNSAFE, Module* pModule) 
{
    STRINGREF sRef = (STRINGREF) classNameUNSAFE;
    TypeHandle typeHandle;
    
    HELPER_METHOD_FRAME_BEGIN_RET_1(classNameUNSAFE);
    {
        if (!sRef)
            COMPlusThrowArgumentNull(L"className",L"ArgumentNull_String");

        typeHandle = TypeName::GetTypeUsingCASearchRules(sRef->GetBuffer(), pModule->GetAssembly());
    }
    HELPER_METHOD_FRAME_END();

    return typeHandle.AsPtr();
}
FCIMPLEND

FCIMPL6(EnregisteredTypeHandle, RuntimeTypeHandle::GetTypeByName, 
    StringObject* classNameUNSAFE, CLR_BOOL bThrowOnError, CLR_BOOL bIgnoreCase, CLR_BOOL bReflectionOnly, StackCrawlMark* pStackMark, CLR_BOOL bLoadTypeFromPartialNameHack) 
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    STRINGREF sRef = (STRINGREF) classNameUNSAFE;
    TypeHandle typeHandle;
    
    HELPER_METHOD_FRAME_BEGIN_RET_1(sRef);
    {
        if (!sRef)
            COMPlusThrowArgumentNull(L"className",L"ArgumentNull_String");

        typeHandle = TypeName::GetTypeManaged(sRef->GetBuffer(), NULL, bThrowOnError, bIgnoreCase, bReflectionOnly, /*bProhibitAsmQualifiedName =*/ FALSE, pStackMark, bLoadTypeFromPartialNameHack);        
    }
    HELPER_METHOD_FRAME_END();

    return typeHandle.AsPtr();
}
FCIMPLEND

FCIMPL4(FC_BOOL_RET, RuntimeTypeHandle::SatisfiesConstraints, TypeHandle *pGenericParameter, ArrayBase* arrayTypeContext, ArrayBase* arrayMethodContext, EnregisteredTypeHandle ethGenericArgument);
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(arrayTypeContext, NULL_OK));
        PRECONDITION(CheckPointer(arrayMethodContext, NULL_OK));
    }
    CONTRACTL_END;
    
    TypeHandle thGenericParameter(*pGenericParameter);
    TypeHandle thGenericArgument = TypeHandle::FromPtr(ethGenericArgument);
    BOOL bResult = FALSE; 
    SigTypeContext typeContext;

    DWORD classInstCount = 0;    
    TypeHandle *classInst = NULL;
    DWORD methInstCount = 0;    
    TypeHandle *methInst = NULL;

    if (arrayTypeContext != NULL)
    {
        classInstCount = arrayTypeContext->GetNumComponents();    
        classInst = (TypeHandle*)arrayTypeContext->GetDataPtr();
    }
    
    if (arrayMethodContext != NULL)
    {
        methInstCount = arrayMethodContext->GetNumComponents();    
        methInst = (TypeHandle*)arrayMethodContext->GetDataPtr();
    }

    SigTypeContext::InitTypeContext(classInstCount, classInst, methInstCount, methInst, &typeContext);

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        bResult = thGenericParameter.AsGenericVariable()->SatisfiesConstraints(&typeContext, thGenericArgument);
    }
    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(bResult);      
}
FCIMPLEND

FCIMPL1(ArrayBase*, RuntimeTypeHandle::GetInstantiation, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    BASEARRAYREF refRetVal  = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);
    {
    TypeHandle* inst = typeHandle.GetInstantiation();
        
    if (inst != NULL) {
        // Allocate the array
        INT32 n = (INT32)typeHandle.GetNumGenericArgs();
        MethodTable *pMT = g_Mscorlib.GetClass(CLASS__TYPE_HANDLE);
        TypeHandle arrayHandle = ClassLoader::LoadArrayTypeThrowing(TypeHandle(pMT), ELEMENT_TYPE_SZARRAY);
        refRetVal = (BASEARRAYREF) AllocateArrayEx(arrayHandle, &n, 1);

        // populate type array
        TypeHandle *pElem = (TypeHandle*)refRetVal->GetDataPtr();
        for (INT32 i = 0; i < n; i++) {
            pElem[i] = inst[i];
            _ASSERTE(!pElem[i].IsNull());
        }
    }
    }
    HELPER_METHOD_FRAME_END();

    return (ArrayBase*)OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL2(EnregisteredTypeHandle, RuntimeTypeHandle::MakeArray, TypeHandle* pTypeHandle, INT32 rank) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    TypeHandle arrayHandle;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();   
    {
        arrayHandle = pTypeHandle->MakeArray(rank);
    }
    HELPER_METHOD_FRAME_END();
    
    return arrayHandle.AsPtr();
}
FCIMPLEND

FCIMPL1(EnregisteredTypeHandle, RuntimeTypeHandle::MakeSZArray, TypeHandle* pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle arrayHandle;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();   
    {
        arrayHandle = pTypeHandle->MakeSZArray();
    }
    HELPER_METHOD_FRAME_END();

    return arrayHandle.AsPtr();    
}
FCIMPLEND

FCIMPL1(EnregisteredTypeHandle, RuntimeTypeHandle::MakePointer, TypeHandle* pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle pointerHandle;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();   
    {
        pointerHandle = pTypeHandle->MakePointer();
    }
    HELPER_METHOD_FRAME_END();
    
    return pointerHandle.AsPtr();    
}
FCIMPLEND

FCIMPL1(EnregisteredTypeHandle, RuntimeTypeHandle::MakeByRef, TypeHandle* pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle byRefHandle;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();   
    {
        byRefHandle = pTypeHandle->MakeByRef();
    }
    HELPER_METHOD_FRAME_END();
    
    return byRefHandle.AsPtr();    
}
FCIMPLEND

FCIMPL2(EnregisteredTypeHandle, RuntimeTypeHandle::Instantiate, TypeHandle *pTypeHandle, ArrayBase *pInstArray) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    TypeHandle type;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        if (pInstArray == NULL) 
            COMPlusThrowArgumentNull(L"inst", L"ArgumentNull_Array");

        DWORD cInstArray = pInstArray->GetNumComponents();
        TypeHandle *pElem = (TypeHandle*)pInstArray->GetDataPtr();

        size_t size = cInstArray * sizeof(TypeHandle);
        if ((size / sizeof(TypeHandle)) != cInstArray) // uint over/underflow
            COMPlusThrow(kArgumentException);
        TypeHandle *inst = (TypeHandle*) _alloca(size);
    
        for (DWORD i = 0; i < cInstArray; i++) {
            if (pElem[i].IsNull())  
                COMPlusThrowArgumentNull(L"inst", L"ArgumentNull_ArrayElement");

            inst[i] = pElem[i];
        }
    
        type = typeHandle.Instantiate(inst, cInstArray);
    }
    HELPER_METHOD_FRAME_END();

    return type.AsPtr();
}
FCIMPLEND

FCIMPL1(EnregisteredTypeHandle, RuntimeTypeHandle::GetGenericTypeDefinition, TypeHandle *pTypeHandle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle genericType(*pTypeHandle);
    
    if (genericType.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    if (!genericType.HasInstantiation())
        return NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {            
        genericType = ClassLoader::LoadTypeDefThrowing(genericType.GetModule(), 
                                                       genericType.GetMethodTable()->GetCl(),
                                                       ClassLoader::ThrowIfNotFound,
                                                       ClassLoader::PermitUninstDefOrRef);
    }
    HELPER_METHOD_FRAME_END();
    
    return genericType.AsPtr();
}
FCIMPLEND

FCIMPL1(void*, RuntimeTypeHandle::GetCanonicalHandle, TypeHandle *pTypeHandle)
{
    CONTRACTL {
        DISABLED(NOTHROW);
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    return pTypeHandle->GetCanonicalMethodTable();
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeTypeHandle::HasInstantiation, TypeHandle *pTypeHandle)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    FC_RETURN_BOOL(typeHandle.HasInstantiation());
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeTypeHandle::IsGenericTypeDefinition, TypeHandle *pTypeHandle)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    FC_RETURN_BOOL(typeHandle.IsGenericTypeDefinition());
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeTypeHandle::IsGenericVariable, TypeHandle *pTypeHandle)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    FC_RETURN_BOOL(typeHandle.IsGenericVariable());
}
FCIMPLEND

FCIMPL1(INT32, RuntimeTypeHandle::GetGenericVariableIndex, TypeHandle *pTypeHandle)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    if (!typeHandle.IsGenericVariable())
        FCThrow(kArgumentException);
    
    return (INT32)typeHandle.AsGenericVariable()->GetIndex();
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeTypeHandle::ContainsGenericVariables, TypeHandle *pTypeHandle)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle typeHandle(*pTypeHandle);
    
    if (typeHandle.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    FC_RETURN_BOOL(typeHandle.ContainsGenericVariables());
}
FCIMPLEND

//***********************************************************************************
//***********************************************************************************
//***********************************************************************************

FCIMPL1(void*, RuntimeMethodHandle::GetFunctionPointer, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
        
    void* funcPtr = 0;
    
    if (pMethod) {

        _ASSERTE(!pMethod->IsGenericMethodDefinition());

        HELPER_METHOD_FRAME_BEGIN_RET_0();
        funcPtr = (void*)pMethod->GetMultiCallableAddrOfCode();
        HELPER_METHOD_FRAME_END();
    }

    return funcPtr;
}
FCIMPLEND
    
FCIMPL1(LPCUTF8, RuntimeMethodHandle::GetUtf8Name, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    LPCUTF8 szName = NULL;
    
    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
           
    szName = pMethod->GetName();

    _ASSERTE(CheckPointer(szName, NULL_OK));
    
    return szName;
}
FCIMPLEND

FCIMPL1(StringObject*, RuntimeMethodHandle::GetName, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
        
    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
        
    STRINGREF refName = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);
    refName = COMString::NewString(pMethod->GetName());
    HELPER_METHOD_FRAME_END();            
    
    return (StringObject*)OBJECTREFToObject(refName);
}
FCIMPLEND

FCIMPL1(INT32, RuntimeMethodHandle::GetAttributes, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    INT32 retVal = 0;        
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    retVal = (INT32)pMethod->GetAttrs();
    END_SO_INTOLERANT_CODE;
    return retVal;
}
FCIMPLEND

FCIMPL1(INT32, RuntimeMethodHandle::GetImplAttributes, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDesc* pMethod = *ppMethod;
    INT32 attributes = 0;

    if (!pMethod)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    if (IsNilToken(pMethod->GetMemberDef()))
        return attributes;
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    {
        attributes = (INT32)pMethod->GetImplAttrs();
    }
    END_SO_INTOLERANT_CODE;

    return attributes;
}
FCIMPLEND
    
FCIMPL1(EnregisteredTypeHandle, RuntimeMethodHandle::GetDeclaringType, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
        
    MethodTable *pMT = pMethod->GetMethodTable();
    TypeHandle declType(pMT);
    if (pMT->IsArray()) 
    {
        HELPER_METHOD_FRAME_BEGIN_RET_0();   
        
        // Load the TypeDesc for the array type.  Note the returned type is approximate, i.e.
        // if shared between reference array types then we will get object[] back.
        DWORD rank = pMT->GetRank();
        TypeHandle elemType = pMT->GetApproxArrayElementTypeHandle();
        declType = ClassLoader::LoadArrayTypeThrowing(elemType, pMT->GetInternalCorElementType(), rank);
        HELPER_METHOD_FRAME_END();            
    }
    return declType.AsPtr();
}
FCIMPLEND

FCIMPL1(INT32, RuntimeMethodHandle::GetSlot, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
        
    return (INT32)pMethod->GetSlot();
}
FCIMPLEND

FCIMPL4(void, SignatureNative::GetCustomModifiers, SignatureNative* pSignature, 
    INT32 parameter, ArrayBase** REFRequired, ArrayBase** REFOptional)
{   
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        PRECONDITION(CheckPointer(REFRequired));
        PRECONDITION(CheckPointer(REFOptional));
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BASEARRAYREF* SAFERequired = (BASEARRAYREF*)REFRequired;
    BASEARRAYREF* SAFEOptional = (BASEARRAYREF*)REFOptional;

    HELPER_METHOD_FRAME_BEGIN_0();
    {
        
        BYTE callConv = *(BYTE*)pSignature->GetCorSig();
        SigTypeContext typeContext;
        pSignature->GetTypeContext(&typeContext);
        MetaSig sig(pSignature->GetCorSig(), 
                    pSignature->GetCorSigSize(),
                    pSignature->GetModule(), 
                    &typeContext,
                    FALSE, 
                    (callConv & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_FIELD ? MetaSig::sigField : MetaSig::sigMember);
        _ASSERTE(callConv == sig.GetCallingConventionInfo());                 

        SigPointer argument = NULL; 

        PRECONDITION(sig.GetCallingConvention() != IMAGE_CEE_CS_CALLCONV_FIELD || parameter == 1);

        if (parameter == 0) 
        {
            argument = sig.GetReturnProps();
        }
        else
        {
            for(INT32 i = 0; i < parameter; i++)
                sig.NextArg();

            argument = sig.GetArgProps();
        }
        
        //if (parameter < 0 || parameter > (INT32)sig.NumFixedArgs())
        //    FCThrowResVoid(kArgumentNullException, L"Arg_ArgumentOutOfRangeException"); 
        
        SigPointer sp = argument;
        Module* pModule = sig.GetModule();
        INT32 cRequired = 0;
        INT32 cOptional = 0;    
        CorElementType cmodType;
        
        // Discover the number of required and optional custom modifiers.   
        while(TRUE)
        {
            BYTE data;
            IfFailThrow(sp.GetByte(&data));
            cmodType = (CorElementType)data;

            if (cmodType == ELEMENT_TYPE_CMOD_OPT) 
            {
                cOptional ++;
            }        
            else if (cmodType == ELEMENT_TYPE_CMOD_REQD) 
            {
                cRequired ++;
            }        
            else if (cmodType != ELEMENT_TYPE_SENTINEL) 
            {
                break;        
            }
            
            IfFailThrow(sp.GetToken(NULL));
        }

        // Reset sp and populate the arrays for the required and optional custom 
        // modifiers now that we know how long they should be. 
        sp = argument;

        MethodTable *pMT = g_Mscorlib.GetClass(CLASS__TYPE_HANDLE);
        TypeHandle arrayHandle = ClassLoader::LoadArrayTypeThrowing(TypeHandle(pMT), ELEMENT_TYPE_SZARRAY);

        *SAFERequired = (BASEARRAYREF) AllocateArrayEx(arrayHandle, &cRequired, 1);
        *SAFEOptional = (BASEARRAYREF) AllocateArrayEx(arrayHandle, &cOptional, 1);    

        while(cOptional + cRequired != 0)
        {
            BYTE data;
            IfFailThrow(sp.GetByte(&data));
            cmodType = (CorElementType)data;

            mdToken token;
            IfFailThrow(sp.GetToken(&token));

            TypeHandle th = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pModule, token, 
                                                                        &typeContext,
                                                                        ClassLoader::ThrowIfNotFound, 
                                                                        ClassLoader::FailIfUninstDefOrRef);        
        
            if (cmodType == ELEMENT_TYPE_CMOD_OPT) 
            {
                ((TypeHandle*)(*SAFEOptional)->GetDataPtr())[--cOptional] = th;
            }        
            else 
            {
                ((TypeHandle*)(*SAFERequired)->GetDataPtr())[--cRequired] = th;
            }        
        }    
    }  
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(INT32, RuntimeMethodHandle::GetMethodDef, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    if (pMethod->HasMethodInstantiation())
    {
        HELPER_METHOD_FRAME_BEGIN_RET_0();
        {         
            pMethod = pMethod->StripMethodInstantiation();
        }
        HELPER_METHOD_FRAME_END();
    }

    INT32 tkMethodDef = (INT32)pMethod->GetMemberDef();
    _ASSERTE(TypeFromToken(tkMethodDef) == mdtMethodDef);
    
    if (IsNilToken(tkMethodDef) || TypeFromToken(tkMethodDef) != mdtMethodDef)
        return mdMethodDefNil;
    
    return tkMethodDef;
}
FCIMPLEND
    
FCIMPL6(void, SignatureNative::GetSignature,
    SignatureNative* pSignatureNative, 
    PCCOR_SIGNATURE pCorSig, DWORD cCorSig,
    FieldDesc *pFieldDesc, MethodDesc *pMethod, EnregisteredTypeHandle thDeclaringType) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(thDeclaringType || pMethod->IsDynamicMethod());
        PRECONDITION(CheckPointer(pCorSig, NULL_OK));
        PRECONDITION(CheckPointer(pMethod, NULL_OK));
        PRECONDITION(CheckPointer(pFieldDesc, NULL_OK));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // for dynamic method, see precondition
    if (thDeclaringType == NULL)
        thDeclaringType = pMethod->GetMethodTable();
    TypeHandle declType = TypeHandle::FromPtr(thDeclaringType);

    HELPER_METHOD_FRAME_BEGIN_0();
    {        
        Module* pModule = declType.GetModule();
        
        if (pMethod)
            pMethod->GetSig(&pCorSig, &cCorSig);
        else if (pFieldDesc)
            pFieldDesc->GetSig(&pCorSig, &cCorSig);
        
        pSignatureNative->m_sig = pCorSig;    
        pSignatureNative->m_cSig = cCorSig;    
        pSignatureNative->m_pMethod = pMethod;    
        pSignatureNative->m_declaringType = declType;    

        PREFIX_ASSUME(pCorSig!= NULL);
        BYTE callConv = *(BYTE*)pCorSig;
        SigTypeContext typeContext;
        if (pMethod)
            SigTypeContext::InitTypeContext(
                pMethod, declType.GetClassOrArrayInstantiation(), pMethod->LoadMethodInstantiation(), &typeContext);
        else
            SigTypeContext::InitTypeContext(declType, &typeContext);
        MetaSig msig(pCorSig, cCorSig, pModule, &typeContext,
            FALSE, (callConv & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_FIELD ? MetaSig::sigField : MetaSig::sigMember);

        _ASSERTE(callConv == msig.GetCallingConventionInfo());                 
        if (callConv == IMAGE_CEE_CS_CALLCONV_FIELD)
        {            
            msig.NextArgNormalized();            
            pSignatureNative->SetReturnType(msig.GetLastTypeHandleThrowing());          
        }
        else
        {
            pSignatureNative->SetCallingConvention(msig.GetCallingConventionInfo());
            pSignatureNative->SetReturnType(msig.GetRetTypeHandleThrowing());
            
            if (pMethod)
            {
                pSignatureNative->m_numVirtualFixedArgs = msig.NumVirtualFixedArgs(IsMdStatic(pMethod->GetAttrs()));
                if (msig.HasRetBuffArg())
                    pSignatureNative->SetHasRetBuffArg();
            }
            
            INT32 nArgs = msig.NumFixedArgs();
            MethodTable *pMT = g_Mscorlib.GetClass(CLASS__TYPE_HANDLE);
            TypeHandle arrayHandle = ClassLoader::LoadArrayTypeThrowing(TypeHandle(pMT), ELEMENT_TYPE_SZARRAY);
            pSignatureNative->m_arguments = (BASEARRAYREF) AllocateArrayEx(arrayHandle, &nArgs, 1);
                           
            msig.Reset();
            for (INT32 i = 0; i < nArgs; i++) 
            {
                msig.NextArg();
                pSignatureNative->SetArgument(i, msig.GetLastTypeHandleThrowing());
            }

            _ASSERTE(pSignatureNative->m_returnType.AsPtr());
        }
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, SignatureNative::CompareSig, SignatureNative* pLhs, SignatureNative* pRhs)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    INT32 ret = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        ret = MetaSig::CompareMethodSigs(
            pLhs->GetCorSig(), pLhs->GetCorSigSize(), pLhs->GetModule(), NULL, 
            pRhs->GetCorSig(), pRhs->GetCorSigSize(), pRhs->GetModule(), NULL);    
    }
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(ret);
}
FCIMPLEND

FCIMPL1(ArrayBase*, RuntimeMethodHandle::GetMethodInstantiation, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    BASEARRAYREF refArrTypes   = NULL;
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);
    {
        TypeHandle* inst = pMethod->LoadMethodInstantiation();
        if (inst != NULL) {
            // Allocate the array
            INT32 n = (INT32)pMethod->GetNumGenericMethodArgs();
            MethodTable *pMT = g_Mscorlib.GetClass(CLASS__TYPE_HANDLE);
            TypeHandle arrayHandle = ClassLoader::LoadArrayTypeThrowing(TypeHandle(pMT), ELEMENT_TYPE_SZARRAY);
            refArrTypes = (BASEARRAYREF) AllocateArrayEx(arrayHandle, &n, 1);
            
            // Create type array
            TypeHandle *pElem = (TypeHandle*)refArrTypes->GetDataPtr();
            for (INT32 i = 0; i < n; i++) {
                pElem[i] = inst[i];
                _ASSERTE(!pElem[i].IsNull());
            }
        }
    }
   HELPER_METHOD_FRAME_END();

   return (ArrayBase*)OBJECTREFToObject(refArrTypes);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeMethodHandle::HasMethodInstantiation, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    FC_RETURN_BOOL(pMethod->HasMethodInstantiation());
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeMethodHandle::IsGenericMethodDefinition, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    FC_RETURN_BOOL(pMethod->IsGenericMethodDefinition());
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeMethodHandle::IsDynamicMethod, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    FC_RETURN_BOOL(pMethod->IsNoMetadata());
}
FCIMPLEND

FCIMPL1(Object*, RuntimeMethodHandle::GetResolver, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    OBJECTREF resolver = NULL;
    if (pMethod->IsLCGMethod())
    {
        resolver = ObjectFromHandle(pMethod->GetDynamicMethodDesc()->GetLCGMethodResolver()->m_managedResolver);
    }
    return OBJECTREFToObject(resolver);
}
FCIMPLEND

FCIMPL1(void, RuntimeMethodHandle::Destroy, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMethod = *ppMethod;
    _ASSERTE(pMethod != NULL);
    _ASSERTE(pMethod->IsDynamicMethod());

    DynamicMethodDesc* pDynamicMethodDesc = (DynamicMethodDesc*)pMethod;

    // Destroy should be called only if the managed part is gone.
    _ASSERTE(ObjectFromHandle(pDynamicMethodDesc->GetLCGMethodResolver()->m_managedResolver) == NULL);

    HELPER_METHOD_FRAME_BEGIN_0();
    pDynamicMethodDesc->Destroy(TRUE);
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(MethodDesc*, RuntimeMethodHandle::GetTypicalMethodDefinition, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDesc* pMethod = *ppMethod;
    
    if (!pMethod)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    if (pMethod->IsTypicalMethodDefinition())
        return pMethod;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {            
        pMethod = pMethod->LoadTypicalMethodDefinition();
    }
    HELPER_METHOD_FRAME_END();
    
    return pMethod;
}
FCIMPLEND

FCIMPL1(MethodDesc*, RuntimeMethodHandle::StripMethodInstantiation, MethodDesc **ppMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    MethodDesc* pMethod = *ppMethod;
    MethodDesc* pResult = NULL;
    
    if (!pMethod)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {            
        pResult = pMethod->StripMethodInstantiation();
    }
    HELPER_METHOD_FRAME_END();

    return pResult;
}
FCIMPLEND

//
// This method is called to create an InstantiatedMethodDesc for a method in a generic class or a generic method.
// However this method may return the same MethodDesc if, from a reflection perspective, an InstantiatedMethodDesc
// is not needed.
// 
// There are two cases when we need to instantiate a method desc
// - when calling BindGenericArguments() on a generic method 
// - when initializing particular MethodInfos for a generic type 
// the determination of which case is which is made off instArray being null.
// Notice that this function may be called for a method that does not need an InstatiatedMethodDesc in which
// case it simply returns the original MethodDesc
//
FCIMPL3(MethodDesc*, RuntimeMethodHandle::GetInstantiatingStub, 
    MethodDesc **ppMethod, EnregisteredTypeHandle enregTypehandle, ArrayBase* instArray) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle instType = TypeHandle::FromPtr(enregTypehandle);
    MethodDesc* pMethod = *ppMethod;
    MethodDesc *pInstMD = pMethod;
    
    // error conditions
    if (!pMethod)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");

    if (instType.IsNull())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    if (instType.IsTypeDesc())
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
    
    if (instType.IsGenericVariable())
        FCThrow(kArgumentException);
    
    _ASSERTE(instType.IsUnsharedMT());

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    if (instArray != NULL) 
    {
        // method.BindGenericParameters() was called and we need to retrieve an instantiating stub

        _ASSERTE(pMethod->IsGenericMethodDefinition());
        DWORD ntypars = instArray->GetNumComponents();    

        if (ntypars != pMethod->GetNumGenericMethodArgs())
            COMPlusThrow(kArgumentException);

        TypeHandle *pElem = (TypeHandle*)instArray->GetDataPtr();

        size_t size = ntypars * sizeof(TypeHandle);
        if ((size / sizeof(TypeHandle)) != ntypars) // uint over/underflow
            COMPlusThrow(kArgumentException);
        TypeHandle *inst = (TypeHandle*) _alloca(size);        

        for (DWORD i = 0; i < ntypars; i++) 
        {
            TypeHandle instTh = pElem[i];

            if (instTh.IsNull())  
                COMPlusThrowArgumentNull(L"inst", L"ArgumentNull_ArrayElement");
            inst[i] = instTh;
        }
        // we base the creation of an unboxing stub on whether the original method was one already
        // that keeps the reflection logic the same for value types
        pInstMD = MethodDesc::FindOrCreateAssociatedMethodDesc(
            pMethod,
            instType.GetMethodTable(),
            pMethod->IsUnboxingStub(), 
            ntypars,
            inst,
            FALSE, /* no allowInstParam */
            TRUE   /* force remotable method (i.e. inst wrappers for non-generic methods on generic interfaces) */);

    }
    else if (!pMethod->IsGenericMethodDefinition() && (pMethod->IsStatic() || instType.IsValueType() || instType.IsInterface()))
    {
        // 
        // called at MethodInfos cache creation needs an instantiating stub if 
        // - non generic static method on a generic class
        // - non generic instance method on a struct
        // - non generic method on a generic interface
        //

        // we base the creation of an unboxing stub on whether the original method was one already
        // that keeps the reflection logic the same for value types
        pInstMD = MethodDesc::FindOrCreateAssociatedMethodDesc(
            pMethod,
            instType.GetMethodTable(),
            pMethod->IsUnboxingStub(), 
            0,
            NULL,
            FALSE, /* no allowInstParam */
            TRUE   /* force remotable method (i.e. inst wrappers for non-generic methods on generic interfaces) */);
    }

    HELPER_METHOD_FRAME_END();
    
    return pInstMD;
}
FCIMPLEND

FCIMPL2(MethodDesc*, RuntimeMethodHandle::GetMethodFromCanonical, MethodDesc **ppMethod, EnregisteredTypeHandle enregTypeHandle)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypeHandle instType = TypeHandle::FromPtr(enregTypeHandle);
    MethodDesc* pMethod = *ppMethod;
    MethodDesc* pMDescInCanonMT = NULL;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    {
        pMDescInCanonMT = instType.GetMethodTable()->GetCanonicalMethodTable()->GetMethodDescForSlot(pMethod->GetSlot());
    }
    END_SO_INTOLERANT_CODE;

    return pMDescInCanonMT;
}
FCIMPLEND


FCIMPL2(MethodBody *, RuntimeMethodHandle::GetMethodBody, MethodDesc **ppMethod, EnregisteredTypeHandle enregDeclaringTypeHandle)
{      
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMethod = *ppMethod;
    TypeHandle declaringType = TypeHandle::FromPtr(enregDeclaringTypeHandle);

    if (!pMethod)
        FCThrowRes(kArgumentException, L"Arg_InvalidHandle");
    
    if (!pMethod->IsIL())
        return NULL;

    struct _gc
    {
        METHODBODYREF MethodBodyObj;
        EXCEPTIONHANDLINGCLAUSEREF EHClauseObj;
        LOCALVARIABLEINFOREF LocalVariableInfoObj;
        U1ARRAYREF                  U1Array;
        BASEARRAYREF                TempArray;
    } gc;

    gc.MethodBodyObj = NULL;
    gc.EHClauseObj = NULL;
    gc.LocalVariableInfoObj = NULL;
    gc.U1Array              = NULL;
    gc.TempArray            = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);
    {
        MethodDesc *pMethodIL = pMethod;
        if (pMethod->IsWrapperStub())
            pMethodIL = pMethod->GetWrappedMethodDesc();
        
        COR_ILMETHOD* pILHeader = pMethodIL->GetILHeader();
        
        if (pILHeader)
        {            
            TypeHandle thMethodBody(g_Mscorlib.FetchClass(CLASS__METHOD_BODY));

            TypeHandle thExceptionHandlingClause(g_Mscorlib.FetchClass(CLASS__EH_CLAUSE));
            TypeHandle thEHClauseArray = ClassLoader::LoadArrayTypeThrowing(thExceptionHandlingClause, ELEMENT_TYPE_SZARRAY);

            TypeHandle thLocalVariable(g_Mscorlib.FetchClass(CLASS__LOCAL_VARIABLE_INFO));
            TypeHandle thLocalVariableArray = ClassLoader::LoadArrayTypeThrowing(thLocalVariable, ELEMENT_TYPE_SZARRAY);

            Module* pModule = pMethod->GetModule();
            COR_ILMETHOD_DECODER::DecoderStatus status;
            COR_ILMETHOD_DECODER header(pILHeader, pModule->GetMDImport(), &status);

            if (status != COR_ILMETHOD_DECODER::SUCCESS ||
                !pModule->CheckIL(pMethodIL->GetRVA(), header.GetHeaderSize()))
            {
                if (status == COR_ILMETHOD_DECODER::VERIFICATION_ERROR)
                {
                    // Throw a verification HR
                    COMPlusThrowHR(COR_E_VERIFICATION);
                }
                else
                {
                    COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
                }
            }

            gc.MethodBodyObj = (METHODBODYREF)AllocateObject(thMethodBody.GetMethodTable());
            
            gc.MethodBodyObj->m_maxStackSize = header.GetMaxStack();
            gc.MethodBodyObj->m_initLocals = !!(header.GetFlags() & CorILMethod_InitLocals);

            if (header.IsFat())
                gc.MethodBodyObj->m_localVarSigToken = header.GetLocalVarSigTok();
            else
                gc.MethodBodyObj->m_localVarSigToken = 0;

            // Allocate the array of IL and fill it in from the method header.
            BYTE* pIL = const_cast<BYTE*>(header.Code);
            COUNT_T cIL = header.GetCodeSize();
            gc.U1Array  = (U1ARRAYREF) AllocatePrimitiveArray(ELEMENT_TYPE_U1, cIL);

            SetObjectReference((OBJECTREF*)&gc.MethodBodyObj->m_IL, gc.U1Array, GetAppDomain());
            memcpyNoGCRefs(gc.MethodBodyObj->m_IL->GetDataPtr(), pIL, cIL);

            // Allocate the array of exception clauses.
            INT32 cEh = (INT32)header.EHCount();
            const COR_ILMETHOD_SECT_EH* ehInfo = header.EH;
            gc.TempArray = (BASEARRAYREF) AllocateArrayEx(thEHClauseArray, &cEh, 1);

            SetObjectReference((OBJECTREF*)&gc.MethodBodyObj->m_exceptionClauses, gc.TempArray, GetAppDomain());
            
            for (INT32 i = 0; i < cEh; i++)
            {                    
                COR_ILMETHOD_SECT_EH_CLAUSE_FAT ehBuff; 
                const COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehClause = 
                    (const COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)ehInfo->EHClause(i, &ehBuff); 

                gc.EHClauseObj = (EXCEPTIONHANDLINGCLAUSEREF) AllocateObject(thExceptionHandlingClause.GetMethodTable());

                gc.EHClauseObj->m_flags = ehClause->GetFlags();  
                gc.EHClauseObj->m_tryOffset = ehClause->GetTryOffset();
                gc.EHClauseObj->m_tryLength = ehClause->GetTryLength();
                gc.EHClauseObj->m_handlerOffset = ehClause->GetHandlerOffset();
                gc.EHClauseObj->m_handlerLength = ehClause->GetHandlerLength();
                
                if ((ehClause->GetFlags() & COR_ILEXCEPTION_CLAUSE_FILTER) == 0)
                    gc.EHClauseObj->m_catchToken = ehClause->GetClassToken();
                else
                    gc.EHClauseObj->m_filterOffset = ehClause->GetFilterOffset();
                
                gc.MethodBodyObj->m_exceptionClauses->SetAt(i, (OBJECTREF) gc.EHClauseObj);
                SetObjectReference((OBJECTREF*)&(gc.EHClauseObj->m_methodBody), (OBJECTREF)gc.MethodBodyObj, GetAppDomain());
            }     
           
            PCCOR_SIGNATURE pLocalVarSig = header.LocalVarSig;
            if (pLocalVarSig)
            {
                SigTypeContext sigTypeContext(pMethod, declaringType, pMethod->LoadMethodInstantiation());        
                MetaSig metaSig(pLocalVarSig, 
                                SigParser::LengthOfSig(pLocalVarSig),
                                pModule, 
                                &sigTypeContext, 
                        /* BOOL fConvertSigAsVarArg = */ FALSE, 
                                MetaSig::sigLocalVars,
                        /* BOOL fParamTypeArg = */ FALSE);
                INT32 cLocals = metaSig.NumFixedArgs();
                gc.TempArray  = (BASEARRAYREF) AllocateArrayEx(thLocalVariableArray, &cLocals, 1);
                SetObjectReference((OBJECTREF*)&gc.MethodBodyObj->m_localVariables, gc.TempArray, GetAppDomain());

                for (INT32 i = 0; i < cLocals; i ++)
                {
                    gc.LocalVariableInfoObj = (LOCALVARIABLEINFOREF)AllocateObject(thLocalVariable.GetMethodTable());

                    gc.LocalVariableInfoObj->m_localIndex = i;
                    
                    metaSig.NextArg();

                    CorElementType eType;
                    IfFailThrow(metaSig.GetArgProps().PeekElemType(&eType));
                    if (ELEMENT_TYPE_PINNED == eType)
                        gc.LocalVariableInfoObj->m_bIsPinned = TRUE;

                    TypeHandle  tempType= metaSig.GetArgProps().GetTypeHandleThrowing(pModule, &sigTypeContext);       
                    gc.LocalVariableInfoObj->m_typeHandle =      tempType;
                    _ASSERTE(gc.LocalVariableInfoObj->m_typeHandle != NULL);
                    gc.MethodBodyObj->m_localVariables->SetAt(i, (OBJECTREF) gc.LocalVariableInfoObj);
                }        
            }
            else
            {
                INT32 cLocals = 0;
                gc.TempArray  = (BASEARRAYREF) AllocateArrayEx(thLocalVariableArray, &cLocals, 1);
                SetObjectReference((OBJECTREF*)&gc.MethodBodyObj->m_localVariables, gc.TempArray, GetAppDomain());
            }
        }
    }
    HELPER_METHOD_FRAME_END();

    return (MethodBody*)OBJECTREFToObject(gc.MethodBodyObj);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, RuntimeMethodHandle::IsConstructor, MethodDesc **ppMethod)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppMethod));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BOOL ret = FALSE;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    ret = (BOOL)((*ppMethod)->IsCtor() || (*ppMethod)->IsStaticInitMethod());
    END_SO_INTOLERANT_CODE;
    FC_RETURN_BOOL(ret);
}
FCIMPLEND

//*********************************************************************************************
//*********************************************************************************************
//*********************************************************************************************

FCIMPL1(StringObject*, RuntimeFieldHandle::GetName, FieldDesc **ppField) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppField));
        PRECONDITION(CheckPointer(*ppField));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    FieldDesc *pField = *ppField;
    
    if (!pField)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
        
    STRINGREF refString = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);
    {
        refString = COMString::NewString(pField->GetName());
    }
    HELPER_METHOD_FRAME_END();
    return (StringObject*)OBJECTREFToObject(refString);
}
FCIMPLEND
    
FCIMPL1(LPCUTF8, RuntimeFieldHandle::GetUtf8Name, FieldDesc **ppField) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppField));
        PRECONDITION(CheckPointer(*ppField));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    FieldDesc *pField = *ppField;
    
    if (!pField)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
        
    return pField->GetName();
}
FCIMPLEND
    
FCIMPL1(INT32, RuntimeFieldHandle::GetAttributes, FieldDesc **ppField) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppField));
        PRECONDITION(CheckPointer(*ppField));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    FieldDesc *pField = *ppField;
    
    if (!pField)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    INT32 ret = 0;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));
    ret = (INT32)pField->GetAttributes();
    END_SO_INTOLERANT_CODE;
    return ret;
}
FCIMPLEND
    
FCIMPL1(void*, RuntimeFieldHandle::GetApproxDeclaringType, FieldDesc **ppField) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppField));
        PRECONDITION(CheckPointer(*ppField));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    FieldDesc *pField = *ppField;
    
    if (!pField)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
        
    TypeHandle th = TypeHandle(pField->GetApproxEnclosingMethodTable());  // <BUG> this needs to be checked - see bug 184355 </BUG>
    return th.AsPtr();
}
FCIMPLEND

FCIMPL1(INT32, RuntimeFieldHandle::GetToken, FieldDesc **ppField) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppField));
        PRECONDITION(CheckPointer(*ppField));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    FieldDesc *pField = *ppField;
    
    if (!pField)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    INT32 tkFieldDef = (INT32)pField->GetMemberDef();
    _ASSERTE(!IsNilToken(tkFieldDef) || tkFieldDef == mdFieldDefNil);
    return tkFieldDef;
}
FCIMPLEND

FCIMPL2(FieldDesc*, RuntimeFieldHandle::GetStaticFieldForGenericType, FieldDesc **ppField, EnregisteredTypeHandle declaringTH)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppField));
        PRECONDITION(CheckPointer(*ppField));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    FieldDesc *pField = *ppField;
    TypeHandle declaringType = TypeHandle::FromPtr(declaringTH);
    
    if (!pField)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    if (declaringType.IsNull() || !declaringType.IsUnsharedMT())
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    MethodTable *pMT = declaringType.AsMethodTable();

    _ASSERTE(pField->IsStatic());
    if (pMT->HasGenericsStaticsInfo())
        pField = pMT->GetFieldDescByIndex(pField->GetApproxEnclosingMethodTable()->GetIndexForFieldDesc(pField));
    _ASSERTE(!pField->IsSharedByGenericInstantiations());
    _ASSERTE(pField->GetEnclosingMethodTable() == pMT);

    return pField;
}
FCIMPLEND

FCIMPL1(void*, AssemblyHandle::GetManifestModule, DomainAssembly **ppAssembly) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    DomainAssembly *pAssembly = *ppAssembly;

    if (!pAssembly)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    Assembly* currentAssembly = pAssembly->GetCurrentAssembly();
    return ((currentAssembly == NULL) ? NULL : (LPVOID)(currentAssembly->GetManifestModule()));
}
FCIMPLEND

FCIMPL1(INT32, AssemblyHandle::GetToken, DomainAssembly **ppAssembly) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    DomainAssembly *pAssembly = *ppAssembly;
    mdAssembly token = mdAssemblyNil;
    
    if (!pAssembly)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
        
    IMDInternalImport *mdImport = pAssembly->GetCurrentAssembly()->GetManifestImport();
    
    if (mdImport)
        mdImport->GetAssemblyFromScope(&token);
    
    return token;
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, AssemblyHandle::AptcaCheck, DomainAssembly **ppTargetAssembly, DomainAssembly *pSourceAssembly) 
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    DomainAssembly *pTargetAssembly = *ppTargetAssembly;
    
    if (pTargetAssembly == pSourceAssembly)
        FC_RETURN_BOOL(TRUE);

    BOOL bResult = TRUE;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        bResult = (pTargetAssembly->GetAssembly()->AllowUntrustedCaller() || Security::IsFullyTrusted(pSourceAssembly->GetSecurityDescriptor()));
    }
    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(bResult);
}
FCIMPLEND
    
FCIMPL1(Object*, AssemblyHandle::GetAssembly, DomainAssembly **ppAssembly) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        SO_TOLERANT;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    DomainAssembly *pAssembly = *ppAssembly;
    if (!pAssembly)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    OBJECTREF refAssembly = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);
    refAssembly = (OBJECTREF) pAssembly->GetExposedAssemblyObject();
    _ASSERTE(refAssembly);
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refAssembly);
}
FCIMPLEND

FCIMPL3(void, ModuleHandle::GetPEKind, Module **ppModule, DWORD* pdwPEKind, DWORD* pdwMachine) 
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(*ppModule));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    HELPER_METHOD_FRAME_BEGIN_0();
    Module *pModule = *ppModule;
    PEFile* pFile = pModule->GetFile();
    PEImage* pImage = pFile->GetILimage();
    if(pImage != NULL)
    {
        pImage->GetPEKindAndMachine();
        *pdwPEKind = pImage->GetPEKind();
        *pdwMachine = pImage->GetMachine();
    }
    else
    {
        *pdwPEKind = *pdwMachine = 0;
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(INT32, ModuleHandle::GetMDStreamVersion, Module **ppModule) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(*ppModule));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    Module *pModule = *ppModule;
    
    int iRet = 0;
    
    if (!pModule)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    if (!pModule->IsResource())
        iRet = (int)pModule->GetMDImport()->GetMetadataStreamVersion();
    
    return iRet;
}
FCIMPLEND

FCIMPL1(EnregisteredTypeHandle, ModuleHandle::GetModuleTypeHandle, Module **ppModule) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(*ppModule));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    Module *pModule = *ppModule;
    
    if (!pModule)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    TypeHandle globalTypeHandle = TypeHandle();
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        EX_TRY
        {          
            globalTypeHandle = TypeHandle(pModule->GetGlobalMethodTable());
        }
        EX_SWALLOW_NONTRANSIENT;
    }
    HELPER_METHOD_FRAME_END();

    // May return null
    return (EnregisteredTypeHandle)globalTypeHandle.AsPtr();
}
FCIMPLEND

FCIMPL1(INT32, ModuleHandle::GetToken, Module **ppModule) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(*ppModule));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    Module *pModule = *ppModule;
    
    mdModule token = mdModuleNil;
    
    if (!pModule)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");
    
    if (!pModule->IsResource())
        token = pModule->GetMDImport()->GetModuleFromScope();
    
    return token;
}
FCIMPLEND

FCIMPL1(IMDInternalImport*, ModuleHandle::GetMetadataImport, Module **ppModule) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(*ppModule));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    Module *pModule = *ppModule;
    
    if (!pModule)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    IMDInternalImport *result = NULL;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException));

    if (!pModule->IsResource())
        result = pModule->GetMDImport();

    END_SO_INTOLERANT_CODE;

    return result;
}
FCIMPLEND


FCIMPL6(EnregisteredTypeHandle, ModuleHandle::ResolveType, Module **ppModule, INT32 tkType, TypeHandle *typeArgs, int typeArgsCount, TypeHandle *methodArgs, int methodArgsCount) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(*ppModule));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    Module *pModule = *ppModule;
    
    if (!pModule)
        FCThrowRes(kArgumentNullException, L"Arg_InvalidHandle");

    if (IsNilToken(tkType))
        return NULL;
    
    TypeHandle typeHandle;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        SigTypeContext typeContext(typeArgsCount, typeArgs, methodArgsCount, methodArgs);
        typeHandle = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pModule, tkType, &typeContext, 
                                                          ClassLoader::ThrowIfNotFound, 
                                                          ClassLoader::PermitUninstDefOrRef);

    }
    HELPER_METHOD_FRAME_END();
    return typeHandle.AsPtr();
}
FCIMPLEND

FCIMPL6(MethodDesc*, ModuleHandle::ResolveMethod, Module **ppModule, INT32 tkMemberRef, TypeHandle *typeArgs, int typeArgsCount, TypeHandle *methodArgs, int methodArgsCount) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(*ppModule));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMD = NULL;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        SigTypeContext typeContext(typeArgsCount, typeArgs, methodArgsCount, methodArgs);
        BOOL strictMetadataChecks = (TypeFromToken(tkMemberRef) == mdtMethodSpec);
        pMD = MemberLoader::GetMethodDescFromMemberDefOrRefOrSpecThrowing(*ppModule, tkMemberRef, &typeContext, strictMetadataChecks, FALSE);

        EEClass* pEEClass = pMD->GetClass();
        if (pEEClass->IsValueClass() && !pMD->IsUnboxingStub() && pMD->IsVirtual())
            pMD = MethodDesc::FindOrCreateAssociatedMethodDesc(pMD,
                                                        pMD->GetMethodTable(),
                                                        TRUE /* get unboxing entry point */,
                                                        pMD->GetNumGenericMethodArgs(),
                                                        pMD->GetMethodInstantiation(),
                                                        TRUE /* allowInstParam */ );
    }
    HELPER_METHOD_FRAME_END();

    return pMD;
}
FCIMPLEND

FCIMPL6(FieldDesc*, ModuleHandle::ResolveField, Module **ppModule, INT32 tkMemberRef, TypeHandle *typeArgs, int typeArgsCount, TypeHandle *methodArgs, int methodArgsCount) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(*ppModule));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    FieldDesc* pField = NULL;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        SigTypeContext typeContext(typeArgsCount, typeArgs, methodArgsCount, methodArgs);
        pField = MemberLoader::GetFieldDescFromMemberRefThrowing(*ppModule, tkMemberRef, &typeContext, FALSE);
    }
    HELPER_METHOD_FRAME_END();

    return pField;
}
FCIMPLEND

FCIMPL1(Object*, ModuleHandle::GetModule, Module **ppModule) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppModule));
        PRECONDITION(CheckPointer(*ppModule, NULL_OK));
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    Module *pModule = *ppModule;
    
    if (!pModule)
        return NULL;

    OBJECTREF refModule = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    {
        refModule = (OBJECTREF) pModule->GetExposedObject();
        _ASSERTE(refModule);
    }
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refModule);
}
FCIMPLEND

FCIMPL4(void*, ModuleHandle::GetDynamicMethod, Module *pModule, StringObject *name, U1Array *sig,  Object *resolver) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(name));
        PRECONDITION(CheckPointer(sig));
    }
    CONTRACTL_END;
    
    DynamicMethodDesc *pNewMD = NULL;

    STRINGREF nameRef = (STRINGREF)name;
    OBJECTREF resolverRef = (OBJECTREF)resolver;
    HELPER_METHOD_FRAME_BEGIN_RET_2(resolverRef, nameRef);
    
    DomainFile *pDomainModule = pModule->GetDomainFile();

    U1ARRAYREF dataArray = (U1ARRAYREF)sig;
    DWORD sigSize = dataArray->GetNumComponents();
    NewHolder<BYTE> pSig(new BYTE[sigSize]);
    memcpy(pSig, dataArray->GetDataPtr(), sigSize);

    DWORD length = nameRef->GetStringLength();
    NewHolder<char> pName(new char[(length + 1) * 2]);
    pName[0] = '\0';
    length = WszWideCharToMultiByte(CP_UTF8, 0, nameRef->GetBuffer(), length, pName, (length + 1) * 2 - sizeof(char), NULL, NULL);
    if (length)
        pName[length / sizeof(char)] = '\0';

    DynamicMethodTable *pMTForDynamicMethods = pDomainModule->GetDynamicMethodTable();
    pNewMD = pMTForDynamicMethods->GetDynamicMethod(pSig, sigSize, pName);
    _ASSERTE(pNewMD != NULL);
    // pNewMD now owns pSig and pName.
    pSig.SuppressRelease();
    pName.SuppressRelease();

    // create a handle to hold the resolver objectref
    pNewMD->GetLCGMethodResolver()->m_managedResolver = pDomainModule->GetAppDomain()->CreateLongWeakHandle(resolverRef);
   
    HELPER_METHOD_FRAME_END();

    return pNewMD;
}
FCIMPLEND

FCIMPL1(EnregisteredTypeHandle, ModuleHandle::GetCallerType, StackCrawlMark* stackMark) { 
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodTable *pMT = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    pMT = SystemDomain::GetCallersType(stackMark);
    HELPER_METHOD_FRAME_END();
    return (EnregisteredTypeHandle)pMT;
}
FCIMPLEND
