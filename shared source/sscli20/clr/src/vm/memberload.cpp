
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
// File: memberload.CPP
//
//

#include "common.h"
#include "clsload.hpp"
#include "method.hpp"
#include "class.h"
#include "class.inl"
#include "object.h"
#include "field.h"
#include "util.hpp"
#include "excep.h"
#include "siginfo.hpp"
#include "threads.h"
#include "stublink.h"
#include "ecall.h"
#include "dllimport.h"
#include "gcdesc.h"
#include "verifier.hpp"
#include "jitinterface.h"
#include "eeconfig.h"
#include "log.h"
#include "fieldmarshaler.h"
#include "cgensys.h"
#include "gc.h"
#include "security.h"
#include "comstringbuffer.h"
#include "dbginterface.h"
#include "comdelegate.h"
#include "sigformat.h"
#include "remoting.h"
#include "eeprofinterfaces.h"
#include "dllimportcallback.h"
#include "listlock.h"
#include "methodimpl.h"
#include "stackprobe.h"
#include "encee.h"
#include "comsynchronizable.h"
#include "customattribute.h"
#include "virtualcallstub.h"
#include "eeconfig.h"
#include "contractimpl.h"
#include "prettyprintsig.h"
#include "objectclone.h"
#include "listlock.inl"
#include "generics.h"
#include "instmethhash.h"
#include "typestring.h"

#ifndef DACCESS_COMPILE

void DECLSPEC_NORETURN MemberLoader::ThrowMissingFieldException(MethodTable* pMT, LPCSTR szMember)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMT, NULL_OK));
        PRECONDITION(CheckPointer(szMember,NULL_OK));
    }
    CONTRACTL_END;

    LPCUTF8 szClassName;

    DefineFullyQualifiedNameForClass();
    if (pMT)
    {
        szClassName = GetFullyQualifiedNameForClass(pMT->GetClass());
    }
    else
    {
        szClassName = "?";
    };


    LPUTF8 szFullName;
    MAKE_FULLY_QUALIFIED_MEMBER_NAME(szFullName, NULL, szClassName, (szMember?szMember:"?"), NULL);
    PREFIX_ASSUME(szFullName!=NULL);
    MAKE_WIDEPTR_FROMUTF8(szwFullName, szFullName);
    EX_THROW(EEMessageException, (kMissingFieldException, IDS_EE_MISSING_FIELD, szwFullName));
}

void DECLSPEC_NORETURN MemberLoader::ThrowMissingMethodException(MethodTable* pMT, LPCSTR szMember, Module *pModule, PCCOR_SIGNATURE pSig,DWORD cSig,const SigTypeContext *pTypeContext)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMT,NULL_OK));
        PRECONDITION(CheckPointer(szMember,NULL_OK));
        PRECONDITION(CheckPointer(pSig,NULL_OK));
        PRECONDITION(CheckPointer(pModule,NULL_OK));
        PRECONDITION(CheckPointer(pTypeContext,NULL_OK));
    }
    CONTRACTL_END;
    LPCUTF8 szClassName;

    DefineFullyQualifiedNameForClass();
    if (pMT)
    {
        szClassName = GetFullyQualifiedNameForClass(pMT->GetClass());
    }
    else
    {
        szClassName = "?";
    };

    if (pSig && pModule && SUCCEEDED(validateSigCompression(0, pSig, cSig)))
    {
        MetaSig tmp(pSig, cSig, pModule, pTypeContext);
        SigFormat sf(tmp, szMember ? szMember : "?", szClassName, NULL);
        MAKE_WIDEPTR_FROMUTF8(szwFullName, sf.GetCString());
        EX_THROW(EEMessageException, (kMissingMethodException, IDS_EE_MISSING_METHOD, szwFullName));
    }
    else
    {
        EX_THROW(EEMessageException, (kMissingMethodException, IDS_EE_MISSING_METHOD, L"?"));
    }
}

void* MemberLoader::GetDescFromMemberDefOrRefThrowing(Module *pModule,
                                                 mdMemberRef MemberRef,
                                                 BOOL *pfIsMethod,
                                                 const SigTypeContext *pTypeContext,
                                                 BOOL strictMetadataChecks /* = TRUE */,  // Throw an exception if no generic method args given for a generic method, otherwise return the 'generic' instantiation
                                                 DWORD nGenericMethodArgs,  // Used when building an instantiated generic method
                                                 TypeHandle *genericMethodArgs,
                                                 BOOL allowInstParam,
                                                 ClassLoadLevel level)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pfIsMethod));
        PRECONDITION(level == CLASS_LOAD_APPROXPARENTS || level == CLASS_LOAD_LEVEL_FINAL);
    }
    CONTRACTL_END;

    LPCUTF8     szMember;
    PCCOR_SIGNATURE pSig = NULL;
    DWORD       cSig;
    mdToken tk  = TypeFromToken(MemberRef);
    PVOID       pRet = NULL;

    *pfIsMethod = TRUE;

    if (tk == mdtMemberRef)
    {
        Module      *pReference = pModule;

        if (!pTypeContext || (pTypeContext->m_classInst == NULL && pTypeContext->m_methInst == NULL))
        {
            // In lookup table?
            void *pDatum = pModule->LookupMemberRef(MemberRef, pfIsMethod);

            if (pDatum != NULL)
            {
                if (*pfIsMethod)
                    ((MethodDesc*)pDatum)->CheckRestore(level);
                return pDatum;
            }
        }

        // No, so do it the long way
        IMDInternalImport *pInternalImport;

        pInternalImport = pModule->GetMDImport();

        szMember = pInternalImport->GetNameAndSigOfMemberRef(
            MemberRef,
            &pSig,
            &cSig
        );

        if (pSig == NULL)
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);

        *pfIsMethod = !isCallConv(MetaSig::GetCallingConventionInfo(pModule, pSig),
                                  IMAGE_CEE_CS_CALLCONV_FIELD);

        mdTypeRef parent = pInternalImport->GetParentOfMemberRef(MemberRef);

        // If parent is a method def, then this is a varargs method and the
        // desc lives in the same module.
        if (TypeFromToken(parent) == mdtMethodDef)
        {
            MethodDesc *pDatumDef = pModule->LookupMethodDef(parent);
            if (pDatumDef)
            {
                pDatumDef->CheckRestore(level);
                return pDatumDef;
            }
            else   // There is no value for this def so we haven't yet loaded the class.
            {
                // Get the parent of the MethodDef
                mdTypeDef typeDef;
                {
                    HRESULT hr = pInternalImport->GetParentToken(parent, &typeDef);
                    if (FAILED(hr))
                        COMPlusThrowHR(hr);
                }
                // Make sure it is a typedef
                if (TypeFromToken(typeDef) != mdtTypeDef)
                {
                    COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_METHODDEF_WO_TYPEDEF_PARENT);
                }

                // load the class
                
                ClassLoader::PermitUninstantiatedFlag permitUninstantiatedFlag;
                permitUninstantiatedFlag = strictMetadataChecks ? ClassLoader::FailIfUninstDefOrRef
                                                                : ClassLoader::PermitUninstDefOrRef;
                
                TypeHandle th = ClassLoader::LoadTypeDefThrowing(pModule, typeDef, 
                                                                 ClassLoader::ThrowIfNotFound,
                                                                 permitUninstantiatedFlag);

                // the class has been loaded and the method should be in the rid map!
                pDatumDef = pModule->LookupMethodDef(parent);
                if (pDatumDef && !pDatumDef->HasClassOrMethodInstantiation())
                {
                    pDatumDef->CheckRestore(level);
                    return pDatumDef;
                }
                else
                    ThrowMissingMethodException(th.GetMethodTable(),szMember,pModule,pSig,cSig,pTypeContext);
            }
        }

        else if (TypeFromToken(parent) == mdtModuleRef)
        {
            // Fix up method refs whose parent is the module to have the fake class COR_GLOBAL_PARENT_TOKEN as a parent
            // Global function/variable
            DomainFile *pFile = pModule->LoadModule(GetAppDomain(), parent);
            pModule = pFile->GetCurrentModule();
            parent = COR_GLOBAL_PARENT_TOKEN;
        }
        else if (TypeFromToken(parent) != mdtTypeRef &&
                 TypeFromToken(parent) != mdtTypeDef &&
                 TypeFromToken(parent) != mdtTypeSpec)
        {
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
        }

        // Now load the parent of the method ref
        TypeHandle typeHnd;
        if (TypeFromToken(parent) == mdtTypeSpec && level == CLASS_LOAD_APPROXPARENTS)
        {
            PCCOR_SIGNATURE sigInst;
            typeHnd = ClassLoader::LoadApproxTypeThrowing(pModule, parent, &sigInst, pTypeContext);
        }
        else
        {
            typeHnd = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pModule, parent, pTypeContext, 
                                                                  ClassLoader::ThrowIfNotFound,
                                                                  (strictMetadataChecks) ? ClassLoader::FailIfUninstDefOrRef : 
                                                                                           ClassLoader::PermitUninstDefOrRef);
        }
        MethodTable *pMT = typeHnd.GetMethodTable();

        // pMT will be null if typeHnd is a variable type
        if (!pMT)
        {
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_METHODDEF_PARENT_NO_MEMBERS);
        }
        PREFIX_ASSUME(pMT!=NULL);

        if (!*pfIsMethod)
        {
            FieldDesc *pFD= pMT->GetClass()->FindField(szMember, pSig, cSig, pReference);

            if (pFD == NULL)
                ThrowMissingFieldException(pMT, szMember);

            if (pFD->IsStatic() && pMT->HasGenericsStaticsInfo())
            {
                //
                // <NICE> this is duplicated logic GetFieldDescByIndex </NICE>
                //
                INDEBUG(mdFieldDef token = pFD->GetMemberDef();)

                DWORD pos = pFD - (pMT->GetApproxFieldDescListRaw() + pMT->GetNumIntroducedInstanceFields());
                _ASSERTE(pos >= 0 && pos < pMT->GetNumStaticFields());

                pFD = pMT->GetGenericsStaticFieldDescs() + pos;
                _ASSERTE(pFD->GetMemberDef() == token);
                _ASSERTE(!pFD->IsSharedByGenericInstantiations());
                _ASSERTE(pFD->GetEnclosingMethodTable() == pMT);
            }


            if (!pMT->HasInstantiation())
              pReference->StoreMemberRef(MemberRef, pFD);
            return pFD;

        }
        else
        {

            // For array method signatures, the caller's signature contains "actual" types whereas the callee's signature has
            // formals (ELEMENT_TYPE_VAR 0 wherever the element type of the array occurs). So we need to pass in a substitution
            // built from the signature of the element type.
            Substitution sigSubst(pModule, NULL, NULL);
            if (typeHnd.IsArray() && TypeFromToken(parent) == mdtTypeSpec)
            {
                ULONG cSigTmp;
                PCCOR_SIGNATURE pSigTmp;
                pInternalImport->GetTypeSpecFromToken(parent, &pSigTmp, &cSigTmp);
                SigPointer sigptr = SigPointer(pSigTmp);
                CorElementType type;
                IfFailThrow(sigptr.GetElemType(&type));
                THROW_BAD_FORMAT_MAYBE(type == ELEMENT_TYPE_SZARRAY || type == ELEMENT_TYPE_ARRAY || type == ELEMENT_TYPE_VALUEARRAY, BFA_NOT_AN_ARRAY, pModule);
                PCCOR_SIGNATURE pInst = sigptr.GetPtr();
                sigSubst = Substitution(pModule, pInst, NULL);
            }

            // Lookup the method in the class.
            MethodDesc *pMD = pMT->GetClass()->FindMethod(szMember, pSig, cSig, pModule, NULL, EEClass::FM_Default, &sigSubst);
            if (pMD == NULL)
                ThrowMissingMethodException(pMT,szMember,pModule,pSig,cSig, pTypeContext);

            pMD->CheckRestore(level);

            // This method might be inherited, in which case typeHnd won't always give the correct instantiation for the declaring type
            if (pMD->HasClassInstantiation() && pMD->GetClass() != typeHnd.GetClass())
            {
                typeHnd = ClassLoader::LoadGenericInstantiationThrowing(pMD->GetModule(),
                                                                        pMD->GetMethodTable()->GetCl(),
                                                                        pMD->GetNumGenericClassArgs(),
                                                                        pMD->GetExactClassInstantiation(typeHnd),
                                                                        ClassLoader::LoadTypes,
                                                                        level);
            }

            // For generic code it will return an instantiating stub where needed.  If the method
            // is a generic method then instantiate it with the given parameters.
            // For non-generic code this will just return pMD
            pMD = MethodDesc::FindOrCreateAssociatedMethodDesc(pMD,
                                                               pMT,
                                                               FALSE /* don't get unboxing entry point */,
                                                               (strictMetadataChecks ? nGenericMethodArgs : pMD->GetNumGenericMethodArgs()),
                                                               (strictMetadataChecks ? genericMethodArgs : pMD->LoadMethodInstantiation()),
                                                               allowInstParam,
                                                               FALSE,
                                                               TRUE,
                                                               level);

            if (!pMD->HasClassOrMethodInstantiation() && !typeHnd.IsArray())
                pReference->StoreMemberRef(MemberRef, pMD);

            return pMD;
        }
    }
    else if (tk == mdtMethodDef)
    {
        *pfIsMethod = TRUE;

        // In lookup table?
        MethodDesc *pDatum = pModule->LookupMethodDef(MemberRef);
        if (!pDatum)
        {
            // No, so do it the long way
            //
            // Notes on methodDefs to generic things
            //
            // For internal purposes we wish to resolve MethodDef from generic classes or for generic methods to
            // the corresponding fully uninstantiated descriptor.  For example, for
            //     class C<T> { void m(); }
            // then then MethodDef for m resolves to a method descriptor for C<T>.m().  This is the
            // descriptor that gets stored in the RID map.
            //
            // Normal IL code that uses generic code cannot use MethodDefs in this way: all calls
            // to generic code must be emitted as MethodRefs and MethodSpecs.  However, at other
            // points in tthe codebase we need to resolve MethodDefs to generic uninstantiated
            // method descriptors, and this is the best place to implement that.
            //
            mdTypeDef typeDef;
            {
                HRESULT hr = pModule->GetMDImport()->GetParentToken(MemberRef, &typeDef);
                if (FAILED(hr))     // GetParentToken should never return ThreadAbort ot OutOfMemory
                    return NULL;
            }

            ClassLoader::PermitUninstantiatedFlag permitUninstantiatedFlag;
            permitUninstantiatedFlag = strictMetadataChecks ? ClassLoader::FailIfUninstDefOrRef
                                                            : ClassLoader::PermitUninstDefOrRef;
            
            TypeHandle th = ClassLoader::LoadTypeDefThrowing(pModule, typeDef, 
                                                             ClassLoader::ThrowIfNotFound,
                                                             permitUninstantiatedFlag);

            // The RID map should have been filled out if we fully loaded the class
            pDatum = pModule->LookupMethodDef(MemberRef);

            if (pDatum == NULL)
            {
                pSig = pModule->GetMDImport()->GetSigOfMethodDef(MemberRef, &cSig);
                szMember = pModule->GetMDImport()->GetNameOfMethodDef(MemberRef);
                pDatum = th.GetClass()->FindMethod(szMember, pSig, cSig, pModule);
                if (!pDatum || th.GetNumGenericArgs() != 0)
                {
                    ThrowMissingMethodException(th.GetMethodTable(),szMember,pModule,pSig,cSig,pTypeContext);
                }
            }
        }

        pDatum->CheckRestore(level);

        // Apply the method instantiation if any.  If not applying strictMetadataChecks we
        // generate the "generic" instantiation - this is used by FuncEval.
        //
        // For generic code this call will return an instantiating stub where needed.  If the method
        // is a generic method then instantiate it with the given parameters.
        // For non-generic code this will just return pDatum
        pDatum = MethodDesc::FindOrCreateAssociatedMethodDesc(pDatum,
                                                              pDatum->GetMethodTable(),
                                                              FALSE /* don't get unboxing entry point */,
                                                              (strictMetadataChecks ? nGenericMethodArgs : pDatum->GetNumGenericMethodArgs()),
                                                              (strictMetadataChecks ? genericMethodArgs : pDatum->LoadMethodInstantiation()),
                                                              allowInstParam,
                                                              FALSE,
                                                              TRUE,
                                                              level);
        pRet = pDatum;
    }
    else if (tk == mdtFieldDef)
    {
        *pfIsMethod = FALSE;

        // In lookup table?
        FieldDesc *pDatum = pModule->LookupFieldDef(MemberRef);
        if (!pDatum)
        {
            // No, so do it the long way
            mdTypeDef   typeDef;
            {
                HRESULT hr = pModule->GetMDImport()->GetParentToken(MemberRef, &typeDef);
                if (FAILED(hr))
                    COMPlusThrowHR(hr);
            }

            // Load the class - that should set the desc in the rid map
            // Field defs to generic things resolve to the formal instantiation
            // without taking the type context into account.  They are only valid internally.
            
            ClassLoader::PermitUninstantiatedFlag permitUninstantiatedFlag;
            permitUninstantiatedFlag = strictMetadataChecks ? ClassLoader::FailIfUninstDefOrRef
                                                            : ClassLoader::PermitUninstDefOrRef;
            
            TypeHandle th = ClassLoader::LoadTypeDefThrowing(pModule, typeDef, 
                                                             ClassLoader::ThrowIfNotFound,
                                                             permitUninstantiatedFlag);

            pDatum = pModule->LookupFieldDef(MemberRef);
            if (!pDatum || nGenericMethodArgs != 0)
            {
                pSig = pModule->GetMDImport()->GetSigOfFieldDef(MemberRef, &cSig);
                szMember = pModule->GetMDImport()->GetNameOfFieldDef(MemberRef);
                ThrowMissingFieldException(th.GetMethodTable(),szMember);
            }
        }

        ClassLoader::EnsureLoaded(pDatum->GetApproxEnclosingMethodTable(), level);


        if (pDatum->GetNumGenericClassArgs() != 0 && strictMetadataChecks)
        {
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_MUST_USE_MEMBERREF_W_FIELD_GEN);
        }
        pRet = pDatum;
    }
    else
    {
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
    }

    return pRet;
}

HRESULT MemberLoader::GetFieldDescFromMemberRefNT(Module *pModule,
                                             mdMemberRef MemberRef,
                                             FieldDesc **ppFieldDesc,
                                             OBJECTREF* pThrowable,
                                             const SigTypeContext *pTypeContext)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    HRESULT hr=S_OK;
    BEGIN_EXCEPTION_GLUE(&hr,pThrowable);
    *ppFieldDesc=GetFieldDescFromMemberRefThrowing(pModule,MemberRef,pTypeContext);
    END_EXCEPTION_GLUE;
    return hr;
} ;


MethodDesc * MemberLoader::GetMethodDescFromMemberDefOrRefOrSpecThrowing(
    Module *pModule,
    mdMemberRef MemberRef,
    const SigTypeContext *pTypeContext,
    BOOL strictMetadataChecks,  // Noramlly true - the zapper is one exception.  Throw an exception if no generic method args given for a generic method, otherwise return the 'generic' instantiation
    BOOL allowInstParam)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END;

    DWORD nGenericMethodArgs = 0;
    CQuickBytes qbGenericMethodArgs;
    TypeHandle *genericMethodArgs = NULL;
    mdMemberRef GenericMemberRef = MemberRef;
    IMDInternalImport *pInternalImport = pModule->GetMDImport();
    if(!pInternalImport->IsValidToken(MemberRef))
    {
        THROW_BAD_FORMAT_MAYBE(!"Invalid Method token", BFA_INVALID_METHOD_TOKEN, pModule);
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
    }
    
    if (TypeFromToken(MemberRef) == mdtMethodSpec)
    {
        PCCOR_SIGNATURE pSig;
        ULONG cSig;

        // Get the member def/ref and instantiation signature
        {
            HRESULT hr = pInternalImport->GetMethodSpecProps(MemberRef, &GenericMemberRef, &pSig, &cSig);
            if(FAILED(hr))
            {
                COMPlusThrowHR(hr);
            }
        }

        THROW_BAD_FORMAT_MAYBE(TypeFromToken(GenericMemberRef) == mdtMethodDef || TypeFromToken(GenericMemberRef) == mdtMemberRef, BFA_EXPECTED_METHODDEF_OR_MEMBERREF, pModule);

        // Load the generic method instantiation
        THROW_BAD_FORMAT_MAYBE(*pSig == IMAGE_CEE_CS_CALLCONV_GENERICINST, 0, pModule);
        pSig++;

        SigPointer sp(pSig);

        IfFailThrow(sp.GetData(&nGenericMethodArgs));

        DWORD cbAllocSize = 0;
        if (!ClrSafeInt<DWORD>::multiply(nGenericMethodArgs, sizeof(TypeHandle), cbAllocSize))
        {
            COMPlusThrowHR(COR_E_OVERFLOW);
        }

        genericMethodArgs = reinterpret_cast<TypeHandle *>(qbGenericMethodArgs.AllocThrows(cbAllocSize));

        for (DWORD i = 0; i < nGenericMethodArgs; i++)
        {
            genericMethodArgs[i] = sp.GetTypeHandleThrowing(pModule, pTypeContext);
            _ASSERTE (!genericMethodArgs[i].IsNull());
            sp.SkipExactlyOne();
        }
    }
    // It's a MethodRef, MethodDef, FieldRef or FieldDef
    BOOL fIsMethod;
    MethodDesc *pMethodDesc =
        (MethodDesc*)GetDescFromMemberDefOrRefThrowing(pModule, 
                                                       GenericMemberRef, 
                                                       &fIsMethod, 
                                                       pTypeContext, 
                                                       strictMetadataChecks, 
                                                       nGenericMethodArgs, 
                                                       genericMethodArgs, 
                                                       allowInstParam);
    if (!fIsMethod)
    {
        EX_THROW(EEMessageException, (kMissingMethodException, IDS_EE_MISSING_METHOD, L"?"));
    }

    pMethodDesc->CheckRestore();

    return pMethodDesc;
}

HRESULT MemberLoader::GetMethodDescFromMemberDefOrRefOrSpecNT(Module *pModule,
                                              mdMemberRef MemberRef,
                                              MethodDesc** pDesc,
                                              OBJECTREF* pThrowable,
                                              const SigTypeContext *pTypeContext,
                                              BOOL strictMetadataChecks,  // Noramlly true - the zapper is one exception.  Throw an exception if no generic method args given for a generic method, otherwise return the 'generic' instantiation
                                              BOOL allowInstParam)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    HRESULT hr = S_OK;
    BEGIN_EXCEPTION_GLUE(&hr,pThrowable);
    *pDesc = GetMethodDescFromMemberDefOrRefOrSpecThrowing(
        pModule,MemberRef, pTypeContext, strictMetadataChecks, allowInstParam);
    END_EXCEPTION_GLUE;
    return hr;
};


MethodDesc* MemberLoader::GetMethodDescFromMethodDefThrowing(Module *pModule,
                                                        mdMethodDef MethodDef,                // MethodDef token
                                                        DWORD numGenericTypeArgs,             // Number of generic type arguments
                                                        TypeHandle *genericTypeArgs,          // Generic arguments for declaring class
                                                        DWORD nGenericMethodArgs,           // Number of generic method arguments
                                                        TypeHandle *genericMethodArgs)        // Generic arguments for declaring method
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(TypeFromToken(MethodDef) == mdtMethodDef);
    }
    CONTRACTL_END;

    // Get the generic method definition.  The functions above are guaranteed to
    // return the generic definition when given a MethodDef.
    // We pass allowInstParam=TRUE because generic method descs are never instantiating stubs.
    BOOL fIsMethod;
    MethodDesc* pDefMD = (MethodDesc*)GetDescFromMemberDefOrRefThrowing(
        pModule, MethodDef, &fIsMethod, NULL, FALSE /* strictMetadataChecks */, 0, NULL, TRUE);

    if (!fIsMethod)
    {
        EX_THROW(EEMessageException, (kMissingMethodException, IDS_EE_MISSING_METHOD, L"?"));
    }

    pDefMD->CheckRestore();

    // If the class isn't generic then LoadGenericInstantiation just checks that
    // we're not supplying type parameters and then returns us the class as a type handle
    MethodTable *pMT = ClassLoader::LoadGenericInstantiationThrowing(
        pModule, pDefMD->GetClass()->GetCl(), numGenericTypeArgs, genericTypeArgs).GetMethodTable();

    // Apply the instantiations (if any).
    MethodDesc *pMD = MethodDesc::FindOrCreateAssociatedMethodDesc(pDefMD, pMT,
                                                                   FALSE, /* don't get unboxing entry point */
                                                                   nGenericMethodArgs, genericMethodArgs,
                                                                   FALSE /* no allowInstParam */);
    pMD->CheckRestore();
    return pMD;

}




FieldDesc* MemberLoader::GetFieldDescFromMemberRefThrowing(
    Module *pModule, 
    mdMemberRef MemberRef, 
    const SigTypeContext *pTypeContext,
    BOOL strictMetadataChecks  // Noramlly true - reflection is the one exception.  Throw an exception if no generic method args given for a generic field, otherwise return the 'generic' instantiation
    )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    BOOL fIsMethod;
    FieldDesc* pFieldDesc = (FieldDesc*)GetDescFromMemberDefOrRefThrowing(
        pModule, MemberRef, &fIsMethod, pTypeContext, strictMetadataChecks);

    if (fIsMethod)
    {
        COMPlusThrow(kMissingFieldException, L"Arg_MissingFieldException");
    }

    return pFieldDesc;
}

#endif // #ifndef DACCESS_COMPILE
