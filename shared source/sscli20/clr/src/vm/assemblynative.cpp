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
/*============================================================
**
** Header:  AssemblyNative.cpp
**
** Purpose: Implements AssemblyNative (loader domain) architecture
**
** Date:  Dec 1, 1998
**
===========================================================*/

#include "common.h"

#include <shlwapi.h>
#include <stdlib.h>
#include "assemblynative.hpp"
#include "field.h"
#include "comstring.h"
#include "assemblyname.hpp"
#include "eeconfig.h"
#include "security.h"
#include "securitydescriptor.h"
#include "comreflectioncommon.h"
#include "strongname.h"
#include "interoputil.h"
#include "frames.h"
#include "typeparse.h"
#include "comnlsinfo.h"
#include "appdomainhelper.h"
#include "stackprobe.h"


//
// NOTE: this macro must be used prior to entering a helper method frame
//
#define GET_ASSEMBLY(pThis) \
    (((ASSEMBLYREF)ObjectToOBJECTREF(pThis))->GetDomainAssembly())

FCIMPL7(Object*, AssemblyNative::Load, AssemblyNameBaseObject* assemblyNameUNSAFE, 
                                       StringObject* codeBaseUNSAFE, 
                                       Object* securityUNSAFE, 
                                       AssemblyBaseObject* locationHintUNSAFE,
                                       StackCrawlMark* stackMark,
                                       CLR_BOOL fThrowOnFileNotFound,
                                       CLR_BOOL fForIntrospection)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    struct _gc
    {
        ASSEMBLYNAMEREF assemblyName;
        STRINGREF       codeBase;
        ASSEMBLYREF     locationHint;
        OBJECTREF       security;
        ASSEMBLYREF     rv;
    } gc;

    gc.assemblyName    = (ASSEMBLYNAMEREF) assemblyNameUNSAFE;
    gc.codeBase        = (STRINGREF)       codeBaseUNSAFE;
    gc.locationHint    = (ASSEMBLYREF)     locationHintUNSAFE;
    gc.security        = (OBJECTREF)       securityUNSAFE;
    gc.rv              = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    if (gc.assemblyName == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_AssemblyName");

    if (fForIntrospection) {
        if (!GetThread()->GetDomain()->IsVerificationDomain())
            GetThread()->GetDomain()->SetIllegalVerificationDomain();
    }

    Thread *pThread = GetThread();
    CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease

    IAssembly *pRefIAssembly = NULL;
    if(gc.assemblyName->GetSimpleName() == NULL)
    {
        if(gc.codeBase == NULL )
            COMPlusThrow(kArgumentException, L"Format_StringZeroLength");
        if((!fForIntrospection) && CorHost2::IsLoadFromBlocked())
            COMPlusThrow(kFileLoadException, FUSION_E_LOADFROM_BLOCKED);
    }
    else if (!fForIntrospection) {
        // Compute parent assembly
        Assembly *pRefAssembly;
        if (gc.locationHint == NULL) {
            pRefAssembly = SystemDomain::GetCallersAssembly(stackMark);
        
            // Cross-appdomain callers aren't allowed as the parent
            if (pRefAssembly &&
                (pRefAssembly->GetDomain() != pThread->GetDomain()))
                pRefAssembly = NULL;
        }
        else
            pRefAssembly = gc.locationHint->GetAssembly();
        
        // Shared assemblies should not be used for the parent in the
        // late-bound case.
        if (pRefAssembly && (!pRefAssembly->IsDomainNeutral()))
            pRefIAssembly = pRefAssembly->GetFusionAssembly();
    }

    // Initialize spec
    AssemblySpec spec;
    spec.InitializeSpec(&(pThread->m_MarshalAlloc), 
                        &gc.assemblyName,
                        FALSE,
                        fForIntrospection);


    if (gc.codeBase != NULL)
        spec.SetCodeBase(&(pThread->m_MarshalAlloc), &gc.codeBase);

    Assembly *pAssembly;
    {
        GCX_PREEMP();
        spec.GetCodeBase()->SetParentAssembly(pRefIAssembly);
    }
    pAssembly = spec.LoadAssembly(FILE_LOADED, NULL, &gc.security, FALSE, fThrowOnFileNotFound, FALSE /*fRaisePrebindEvents*/, stackMark);


    if (pAssembly != NULL)
        gc.rv = (ASSEMBLYREF) pAssembly->GetExposedObject();
    
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.rv);
}
FCIMPLEND


FCIMPL5(Object*, AssemblyNative::LoadImage, U1Array* PEByteArrayUNSAFE,
        U1Array* SymByteArrayUNSAFE, Object* securityUNSAFE,
        StackCrawlMark* stackMark, CLR_BOOL fForIntrospection)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;


    if((!fForIntrospection) && CorHost2::IsLoadFromBlocked())
        FCThrowEx(kFileLoadException, FUSION_E_LOADFROM_BLOCKED,0,0,0);

    struct _gc
    {
        U1ARRAYREF PEByteArray;
        U1ARRAYREF SymByteArray;
        OBJECTREF  security;
        OBJECTREF Throwable;
        OBJECTREF refRetVal;
    } gc;

    gc.PEByteArray  = (U1ARRAYREF) PEByteArrayUNSAFE;
    gc.SymByteArray = (U1ARRAYREF) SymByteArrayUNSAFE;
    gc.security     = (OBJECTREF)  securityUNSAFE;
    gc.Throwable = NULL;
    gc.refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);
    {
    if (gc.PEByteArray == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Array");

    if (fForIntrospection) {
        if (!GetThread()->GetDomain()->IsVerificationDomain())
            GetThread()->GetDomain()->SetIllegalVerificationDomain();
    }

    // Get caller's assembly so we can extract their codebase and propagate it
    // into the new assembly (which obviously doesn't have one of its own).

    AppDomain *pCallersDomain = NULL;
    MethodDesc* pCallerMD = SystemDomain::GetCallersMethod (stackMark, &pCallersDomain);
    Assembly *pCallersAssembly = (pCallerMD ? pCallerMD->GetAssembly() : NULL);
    BOOL fPropagateCallersIdentity = ((!fForIntrospection) && (gc.security == NULL));

    if (pCallersAssembly == NULL) {
        pCallersAssembly = SystemDomain::System()->SystemAssembly();
    } else {
        // If no evidence was provided to the Assembly.Load(byte[]) call, 
        // we want to inherit the caller assembly's evidence.
        if (fPropagateCallersIdentity) {
            AssemblySecurityDescriptor *pSecDesc = pCallersAssembly->GetSecurityDescriptor(pCallersDomain);
            ENTER_DOMAIN_PTR(Security::GetDomain(pSecDesc),ADV_RUNNINGIN)
            {
                if (Security::IsEvidenceComputed(pSecDesc))
                    gc.security = Security::GetAdditionalEvidence(pSecDesc);
                else
                    gc.security = Security::GetEvidence(pSecDesc);
            }
            END_DOMAIN_TRANSITION;

            // Caller may be in another appdomain context, in which case we'll
            // need to marshal/unmarshal the evidence across.
            if (pCallersDomain != GetAppDomain())
                gc.security = AppDomainHelper::CrossContextCopyFrom(pCallersDomain->GetId(), &(gc.security));
        }
    }

    // Pin byte array for loading
    Wrapper<OBJECTHANDLE, DoNothing, DestroyPinningHandle> handle(
        GetAppDomain()->CreatePinningHandle(gc.PEByteArray));

    const BYTE *pbImage = gc.PEByteArray->GetDirectConstPointerToNonObjectElements();
    DWORD cbImage = gc.PEByteArray->GetNumComponents();

    //dtor of PEAssemblyHolder will trigger GC
    PEAssemblyHolder pFile(PEAssembly::OpenMemory(pCallersAssembly->GetManifestFile(),
                                                  pbImage, cbImage, fForIntrospection));
    handle.Release();

    Assembly *pAssembly = NULL;

    fPropagateCallersIdentity = (fPropagateCallersIdentity && pCallersDomain && pCallersAssembly);
    pAssembly = GetPostPolicyAssembly(pFile, &gc.security, fForIntrospection, FALSE, &fPropagateCallersIdentity);

    if (pAssembly && fForIntrospection) {
        IAssemblyName *pIAssemblyName = pAssembly->GetFusionAssemblyName();

        AppDomain::AssemblyIterator i = GetAppDomain()->IterateAssembliesEx( (AssemblyIterationFlags)(kIncludeLoaded | kIncludeIntrospection) );
        while (i.Next()) {
            Assembly *pCachedAssembly = i.GetDomainAssembly()->GetAssembly();
            IAssemblyName *pCachedAssemblyName = pCachedAssembly->GetFusionAssemblyName(); 
            if (pAssembly != pCachedAssembly && S_OK == (pCachedAssemblyName->IsEqual(pIAssemblyName, ASM_CMPF_IL_ALL)))
                COMPlusThrow(kFileLoadException, IDS_EE_REFLECTIONONLY_LOADFROM, L"");
        }
    }

    if (pAssembly && fPropagateCallersIdentity) {
        // Propagate the caller's granted permissions into the loaded assembly.
        struct _localGC {
            OBJECTREF granted;
            OBJECTREF denied;
        } localGC;
        ZeroMemory(&localGC, sizeof(localGC));

        GCPROTECT_BEGIN(localGC);

        AssemblySecurityDescriptor *pCallersSecDesc = pCallersAssembly->GetSecurityDescriptor(pCallersDomain);
        AssemblySecurityDescriptor *pLoadedSecDesc = pAssembly->GetSecurityDescriptor();
        localGC.granted = Security::GetGrantedPermissionSet(pCallersSecDesc, &(localGC.denied));
        // Caller may be in another appdomain context, in which case we'll
        // need to marshal/unmarshal the grant and deny sets across.
        if (pCallersDomain != GetAppDomain()) {
            localGC.granted = AppDomainHelper::CrossContextCopyFrom(pCallersDomain->GetId(), &(localGC.granted));
            if (localGC.denied != NULL)
                localGC.denied = AppDomainHelper::CrossContextCopyFrom(pCallersDomain->GetId(), &(localGC.denied));
        }
        Security::SetGrantedPermissionSet(pLoadedSecDesc, localGC.granted, localGC.denied, Security::GetSpecialFlags(pCallersSecDesc));

        // perform necessary Transparency checks for this Load(byte[]) call (based on the calling method).
        if (pCallerMD)
            SecurityTransparent::PerformTransparencyChecksForLoadByteArray(pCallerMD, pLoadedSecDesc);

        GCPROTECT_END();
    }

    LOG((LF_CLASSLOADER, 
         LL_INFO100, 
         "\tLoaded in-memory module\n"));

    if (pAssembly) {
#ifdef DEBUGGING_SUPPORTED
        // If we were given symbols and we need to track JIT info for
        // the debugger, load them now.
        PBYTE pbSyms;
        DWORD cbSyms;

        if ((gc.SymByteArray != NULL) &&
            CORDebuggerTrackJITInfo(pAssembly->GetManifestModule()->GetDebuggerInfoBits())) {
            Security::CopyByteArrayToEncoding(&gc.SymByteArray,
                                                    &pbSyms, &cbSyms);

            HRESULT hr = pAssembly->GetManifestModule()->SetSymbolBytes(pbSyms, cbSyms);
            delete[] pbSyms;

            if (FAILED(hr))
                COMPlusThrowHR(hr);
        }
#endif // DEBUGGING_SUPPORTED
        //caution: we have to protect refRetVal because dtor of PEAssemblyHolder will trigger GC
        gc.refRetVal = pAssembly->GetExposedObject();
    }
}
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.refRetVal);
}
FCIMPLEND


FCIMPL2(Object*, AssemblyNative::LoadFile, StringObject* pathUNSAFE, Object* securityUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    struct _gc {
        OBJECTREF refRetVal;
        OBJECTREF refSecurity;
        STRINGREF strPath;
    } gc;
    
    if(CorHost2::IsLoadFromBlocked())
        FCThrowEx(kFileLoadException, FUSION_E_LOADFROM_BLOCKED,0,0,0);
    
    gc.refRetVal = NULL;
    gc.refSecurity = ObjectToOBJECTREF(securityUNSAFE);
    gc.strPath = ObjectToSTRINGREF(pathUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);
{
    if (pathUNSAFE == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Path");

    StackSString path;
    gc.strPath->GetSString(path);

    SafeComHolder<IAssembly> pFusionAssembly;
    SafeComHolder<IAssembly> pNativeFusionAssembly;
    SafeComHolder<IFusionBindLog> pFusionLog;

    IfFailThrow(ExplicitBind(path, GetAppDomain()->GetFusionContext(), 
                             EXPLICITBIND_FLAGS_NON_BINDABLE,
                             NULL, &pFusionAssembly, &pNativeFusionAssembly, &pFusionLog));

    PEAssemblyHolder pFile(PEAssembly::Open(pFusionAssembly, pNativeFusionAssembly, NULL, FALSE, FALSE));

    BOOL fDelayPolicyResolution = FALSE;
    Assembly *pAssembly = GetPostPolicyAssembly(pFile, &gc.refSecurity, FALSE, TRUE, &fDelayPolicyResolution);

    LOG((LF_CLASSLOADER, 
         LL_INFO100, 
         "\tLoaded assembly from a file\n"));

    if (pAssembly)
        gc.refRetVal = (ASSEMBLYREF) pAssembly->GetExposedObject();
    }
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.refRetVal);
}
FCIMPLEND

/* static */
Assembly* AssemblyNative::GetPostPolicyAssembly(PEAssembly *pFile,
                                                OBJECTREF *pSecurity,
                                                BOOL fForIntrospection,
                                                BOOL fSetAsExtraEvidence,
                                                BOOL *pfDelayPolicyResolution)
{
    CONTRACT(Assembly*)
    {
        MODE_COOPERATIVE;
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pfDelayPolicyResolution));
        PRECONDITION(CheckPointer(pFile));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    if (!fForIntrospection) {
        DWORD   dwSize = 0;
        // if strongly named
        if (pFile->GetFusionAssemblyName()->GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, NULL, &dwSize) == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {

            SafeComHolder<IAssemblyName> pPostPolicyName(NULL);
            HRESULT hr = PreBindAssembly(GetAppDomain()->GetFusionContext(),
                                         pFile->GetFusionAssemblyName(), 
                                         NULL,  // pAsmParent
                                         &pPostPolicyName,
                                         NULL); // pvReserved
            if (FAILED(hr)) {
                if (hr == FUSION_E_REF_DEF_MISMATCH) {
                    // Policy redirects to another version
                    AssemblySpec spec;
                    spec.InitializeSpec(pPostPolicyName, FALSE);
                    *pfDelayPolicyResolution = FALSE;
                    RETURN spec.LoadAssembly(FILE_LOADED,
                                             fSetAsExtraEvidence ? NULL : pSecurity,
                                             fSetAsExtraEvidence ? pSecurity : NULL,
                                             *pfDelayPolicyResolution);
                }
                else
                    ThrowHR(hr);
            }
            else {
                SafeComHolder<IAssemblyCache> pIAsmCache (NULL);
                IfFailThrow(CreateAssemblyCache(&pIAsmCache, 0));
                
                DWORD dwFlags = ASM_DISPLAYF_FULL;

                SString sourceDisplay;
                FusionBind::GetAssemblyNameDisplayName(pFile->GetFusionAssemblyName(), sourceDisplay, dwFlags);
                hr = pIAsmCache->QueryAssemblyInfo(0, sourceDisplay, NULL);
                if (SUCCEEDED(hr)) {
                    // It's in the GAC
                    AssemblySpec spec;
                    spec.InitializeSpec(pFile->GetFusionAssemblyName(), FALSE);
                    *pfDelayPolicyResolution = FALSE;
                    RETURN spec.LoadAssembly(FILE_LOADED,
                                             fSetAsExtraEvidence ? NULL : pSecurity,
                                             fSetAsExtraEvidence ? pSecurity : NULL,
                                             *pfDelayPolicyResolution);
                }
                else if (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                    ThrowHR(hr);
            }
        }
    }

    RETURN GetAppDomain()->LoadAssembly(NULL, pFile, FILE_LOADED,
                                        fSetAsExtraEvidence ? NULL : pSecurity,
                                        fSetAsExtraEvidence ? pSecurity : NULL,
                                        *pfDelayPolicyResolution);
}


FCIMPL5(Object*, AssemblyNative::LoadModuleImage, Object* refThisUNSAFE, StringObject* moduleNameUNSAFE, U1Array* PEByteArrayUNSAFE, U1Array* SymByteArrayUNSAFE, Object* securityUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    if(CorHost2::IsLoadFromBlocked())
        FCThrowEx(kFileLoadException, FUSION_E_LOADFROM_BLOCKED,0,0,0);

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);

    struct _gc
    {
        STRINGREF   moduleName;
        U1ARRAYREF  PEByteArray;
        U1ARRAYREF  SymByteArray;
        OBJECTREF   security;
        OBJECTREF   rv;
    } gc;
    
    gc.moduleName   = (STRINGREF)   moduleNameUNSAFE;
    gc.PEByteArray  = (U1ARRAYREF)  PEByteArrayUNSAFE;
    gc.SymByteArray = (U1ARRAYREF)  SymByteArrayUNSAFE;
    gc.security     = (OBJECTREF)   securityUNSAFE;
    gc.rv           = NULL;
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    if (gc.moduleName == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_FileName");
    
    if (gc.PEByteArray == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Array");

    LPCSTR psModuleName = NULL;
    WCHAR* pModuleName = NULL;
    WCHAR* pString;
    int    iString;
    CQuickBytes qb;
    CQuickBytes qbLC;

    RefInterpretGetStringValuesDangerousForGC(gc.moduleName, &pString, &iString);
    if(iString == 0)
        COMPlusThrow(kArgumentException, L"Argument_EmptyFileName");
    
    pModuleName = (LPWSTR) qb.AllocThrows((++iString) * sizeof(WCHAR));
    memcpy(pModuleName, pString, iString*sizeof(WCHAR));
    
    MAKE_UTF8PTR_FROMWIDE(pName, pModuleName);
    psModuleName = pName;

    // Need to perform case insensitive lookup.
    {
        UTF8_TO_LOWER_CASE(psModuleName, qbLC);
        psModuleName = (LPUTF8) qbLC.Ptr();
    }

    HashDatum datum;
    mdFile kFile = NULL;
    if (pAssembly->GetAssembly()->m_pAllowedFiles->GetValue(psModuleName, &datum))
        kFile = (mdFile)(size_t)datum;

    // If the name doesn't match one of the File def names, don't load this module.
    // If this name matches the manifest file (datum was NULL), don't load either.
    if (!kFile)
        COMPlusThrow(kArgumentException, L"Arg_InvalidFileName");

    const BYTE *pbImage = gc.PEByteArray->GetDirectConstPointerToNonObjectElements();
    DWORD cbImage = gc.PEByteArray->GetNumComponents();

    // Pin byte array for loading
    Wrapper<OBJECTHANDLE, DoNothing, DestroyPinningHandle> handle(
        GetAppDomain()->CreatePinningHandle(gc.PEByteArray));

    PEModuleHolder pFile(PEModule::OpenMemory(pAssembly->GetFile(), kFile,
                                              pbImage, cbImage));

    handle.Release();

    DomainModule *pDomainModule = GetAppDomain()->LoadDomainModule(pAssembly->GetDomainAssembly(),
                                                                   pFile, FILE_LOADED); 
    Module *pModule = pDomainModule->GetModule();

    if (!pFile->Equals(pModule->GetFile()))

            COMPlusThrow(kArgumentException, L"Argument_ModuleAlreadyLoaded");

    LOG((LF_CLASSLOADER, 
         LL_INFO100, 
         "\tLoaded in-memory module\n"));

    if (pModule) {
#ifdef DEBUGGING_SUPPORTED
        if (!pModule->IsResource()) {
            // If we were given symbols and we need to track JIT info for
            // the debugger, load them now.
            PBYTE pbSyms;
            DWORD cbSyms;
            
            if ((gc.SymByteArray != NULL) &&
                CORDebuggerTrackJITInfo(pModule->GetDebuggerInfoBits())) {
                Security::CopyByteArrayToEncoding(&gc.SymByteArray,
                                                        &pbSyms, &cbSyms);
                
                HRESULT hr = pModule->SetSymbolBytes(pbSyms, cbSyms);
                delete [] pbSyms;
                
                if (FAILED(hr))
                    COMPlusThrowHR(hr);
            }
        }

#endif // DEBUGGING_SUPPORTED
        gc.rv = pModule->GetExposedObject();
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.rv);
}
FCIMPLEND

FCIMPL1(Object*, AssemblyNative::GetLocation, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    STRINGREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    if (pAssembly->IsSystem()) {
        refRetVal = COMString::NewString(SystemDomain::System()->BaseLibrary());
    } else {
        refRetVal = COMString::NewString(pAssembly->GetFile()->GetPath()); 
    }
    
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND



FCIMPL1(FC_BOOL_RET, AssemblyNative::Reflection, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    FC_RETURN_BOOL(pAssembly->IsIntrospectionOnly());
}
FCIMPLEND


FCIMPL4(Object*, AssemblyNative::GetType, Object* refThisUNSAFE, StringObject* nameUNSAFE, CLR_BOOL bThrowOnError, CLR_BOOL bIgnoreCase)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    STRINGREF name      = (STRINGREF) nameUNSAFE;
    OBJECTREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, name);

    // Load the class from this module (fail if it is in a different one).
    refRetVal = (OBJECTREF)GetTypeInner(pAssembly, &name, bThrowOnError, bIgnoreCase);

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

extern MethodTable *GetCallersType(StackCrawlMark *stackMark, void *returnIP);
extern Assembly *GetCallersAssembly(StackCrawlMark *stackMark, void *returnIP);

Object* AssemblyNative::GetTypeInner(DomainAssembly *pAssembly,
                                     STRINGREF *refClassName, 
                                     BOOL bThrowOnError, 
                                     BOOL bIgnoreCase)
{
    CONTRACT(Object*)
    {
        MODE_COOPERATIVE;
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pAssembly));
        POSTCONDITION(!(bThrowOnError && RETVAL == NULL));
    }
    CONTRACT_END;

    STRINGREF sRef = *refClassName;

    if (!sRef)
        COMPlusThrowArgumentNull(L"typeName", L"ArgumentNull_String");

    TypeHandle typeHnd = TypeName::GetTypeManaged(sRef->GetBuffer(), pAssembly, bThrowOnError, bIgnoreCase, pAssembly->IsIntrospectionOnly(), /*prohibitAsmQualifiedName = */ TRUE, NULL, FALSE);

    // There one case were this may return null: if typeHnd
    // represents the Transparent proxy.
    if (typeHnd.IsNull()) {
        if (bThrowOnError) {
            MAKE_UTF8PTR_FROMWIDE(szTypeName, sRef->GetBuffer());
            pAssembly->GetAssembly()->ThrowTypeLoadException(szTypeName, IDS_CLASSLOAD_GENERAL);
        }

        RETURN NULL;
    }

    RETURN (OBJECTREFToObject(typeHnd.GetManagedClassObject()));
}

FCIMPL1(FC_BOOL_RET, AssemblyNative::IsDynamic, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");
    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    FC_RETURN_BOOL(pAssembly->GetFile()->IsDynamic());
}
FCIMPLEND

FCIMPL5(void, AssemblyNative::GetVersion, Object* refThisUNSAFE, INT32* pMajorVersion, INT32* pMinorVersion, INT32*pBuildNumber, INT32* pRevisionNumber)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");


    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);

    UINT16 major=0xffff, minor=0xffff, build=0xffff, revision=0xffff;
    if (pAssembly->IsLoaded())
    {
        BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException))
        pAssembly->GetFile()->GetVersion(&major, &minor, &build, &revision);
        END_SO_INTOLERANT_CODE        
    }
    else
    {
        // half baked assembly, can trigger GC
        HELPER_METHOD_FRAME_BEGIN_0();
        pAssembly->GetFile()->GetVersion(&major, &minor, &build, &revision);
        HELPER_METHOD_FRAME_END();
    }
    *pMajorVersion = major;
    *pMinorVersion = minor;
    *pBuildNumber = build;
    *pRevisionNumber = revision; 

}
FCIMPLEND

FCIMPL1(Object*, AssemblyNative::GetPublicKey, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    OBJECTREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    DWORD cbPublicKey;
    const void *pbPublicKey = pAssembly->GetFile()->GetPublicKey(&cbPublicKey);

    Security::CopyEncodingToByteArray((PBYTE)pbPublicKey,
                                            cbPublicKey,
                                            &refRetVal);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(Object*, AssemblyNative::GetSimpleName, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    STRINGREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);
    
    refRetVal = COMString::NewString(pAssembly->GetSimpleName());
    HELPER_METHOD_FRAME_END();

    return (Object*) OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(Object*, AssemblyNative::GetLocale, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    STRINGREF refRetVal = NULL;
    
    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);
    LPCUTF8 pLocale = pAssembly->GetFile()->GetLocale();
    if(pLocale)
        refRetVal = COMString::NewString(pLocale);
    HELPER_METHOD_FRAME_END();
    
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL2(Object*, AssemblyNative::GetCodeBase, Object* refThisUNSAFE, CLR_BOOL fCopiedName)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    STRINGREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    StackSString codebase;

    if (pAssembly->IsSystem()) {
        codebase.Set(SystemDomain::System()->BaseLibrary());
        PEAssembly::PathToUrl(codebase);
    }
    else {
        pAssembly->GetFile()->GetCodeBase(codebase, fCopiedName && pAssembly->GetAppDomain()->IsShadowCopyOn());
    }

    refRetVal = COMString::NewString(codebase);
        
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(INT32, AssemblyNative::GetHashAlgorithm, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");
    INT32 retVal=0;
    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    HELPER_METHOD_FRAME_BEGIN_RET_0()
    retVal = pAssembly->GetFile()->GetHashAlgId();
    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND

FCIMPL1(INT32,  AssemblyNative::GetFlags, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");
    INT32 retVal=0;
    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    HELPER_METHOD_FRAME_BEGIN_RET_0()
    retVal = pAssembly->GetFile()->GetFlags();
    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND

FCIMPL5(BYTE*, AssemblyNative::GetResource, Object* refThisUNSAFE, StringObject* nameUNSAFE, UINT64* length, StackCrawlMark* stackMark, CLR_BOOL skipSecurityCheck)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly       = GET_ASSEMBLY(refThisUNSAFE);
    PBYTE       pbInMemoryResource  = NULL;
    STRINGREF   name                = ObjectToSTRINGREF(nameUNSAFE);

    _ASSERTE(length != NULL);


    if (name == NULL)
        FCThrowRes(kArgumentNullException, L"ArgumentNull_String");
        
    // Get the name in UTF8
    HELPER_METHOD_FRAME_BEGIN_RET_1(name);
    CQuickBytes bytes;
    LPSTR szName;
    DWORD cName;

    
    szName = GetClassStringVars((STRINGREF)name, &bytes, &cName);
    if (!cName)
        COMPlusThrow(kArgumentException, L"Format_StringZeroLength");

    DWORD  cbResource;

    if (pAssembly->GetResource(szName, &cbResource,
                               &pbInMemoryResource, NULL, NULL,
                               NULL, stackMark, skipSecurityCheck, FALSE))
        *length = cbResource;
    HELPER_METHOD_FRAME_END();

    // Can return null if resource file is zero-length
    return pbInMemoryResource;
}
FCIMPLEND


FCIMPL5(INT32, AssemblyNative::GetManifestResourceInfo, Object* refThisUNSAFE, StringObject* nameUNSAFE, OBJECTREF* pAssemblyRef, STRINGREF* pFileNameOUT, StackCrawlMark* stackMark)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    INT32 rv = -1;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    STRINGREF name      = (STRINGREF) nameUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(name);

    if (name == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_String");
        
    // Get the name in UTF8
    CQuickBytes bytes;
    LPSTR szName;
    DWORD cName;

    szName = GetClassStringVars(name, &bytes, &cName);
    if (!cName)
        COMPlusThrow(kArgumentException, L"Format_StringZeroLength");

    *pFileNameOUT   = NULL;
    *pAssemblyRef   = NULL;

    LPCSTR pFileName = NULL;
    DWORD dwLocation = 0;
    DomainAssembly *pReferencedAssembly = NULL;

    if (pAssembly->GetResource(szName, NULL, NULL, &pReferencedAssembly, &pFileName,
                               &dwLocation, stackMark, FALSE, FALSE)) {
        if (pFileName)
            *((STRINGREF*) (&(*pFileNameOUT))) = COMString::NewString(pFileName);
        if (pReferencedAssembly)
            *((OBJECTREF*) (&(*pAssemblyRef))) = pReferencedAssembly->GetExposedAssemblyObject();

        rv = dwLocation;
}

    HELPER_METHOD_FRAME_END();

    return rv;
}
FCIMPLEND

FCIMPL3(Object*, AssemblyNative::GetModules, Object* refThisUNSAFE, CLR_BOOL fLoadIfNotFound, CLR_BOOL fGetResourceModules)
{
    CONTRACTL 
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);

    struct _gc {
        PTRARRAYREF ModArray;
        PTRARRAYREF nArray;
        OBJECTREF Throwable;
        PTRARRAYREF rv;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    MethodTable *pModuleClass = g_Mscorlib.GetClass(CLASS__MODULE);

    HENUMInternalHolder phEnum(pAssembly->GetMDImport());

    phEnum.EnumInit(mdtFile, mdTokenNil);

    DWORD dwElements = pAssembly->GetMDImport()->EnumGetCount(&phEnum) + 1;
    
    gc.ModArray = (PTRARRAYREF) AllocateObjectArray(dwElements,pModuleClass);
    
    int iCount = 0;

    mdFile  mdFile;
    OBJECTREF o = pAssembly->GetExposedModuleObjectAsModule();
    gc.ModArray->SetAt(0, o);
    
    ReflectionModule *pOnDiskManifest = NULL;
    if (pAssembly->GetAssembly()->NeedsToHideManifestForEmit())
        pOnDiskManifest = pAssembly->GetAssembly()->GetOnDiskManifestModule();

    for(int i = 1;
        pAssembly->GetMDImport()->EnumNext(&phEnum, &mdFile);
        i++) {
            
        DomainFile *pModule;
        pModule = pAssembly->GetModule()->LoadModule(GetAppDomain(), mdFile, fGetResourceModules, !fLoadIfNotFound);

        if (pModule && pModule->GetModule() != pOnDiskManifest) {
            OBJECTREF objRef = (OBJECTREF) pModule->GetExposedModuleObject();
            gc.ModArray->SetAt(i, objRef);
        }
        else
            iCount++;
    }
    
    if(iCount) {
        gc.nArray = (PTRARRAYREF) AllocateObjectArray(dwElements - iCount, pModuleClass);
        DWORD index = 0;
        for(DWORD ii = 0; ii < dwElements; ii++) {
            if(gc.ModArray->GetAt(ii) != NULL) {
                _ASSERTE(index < dwElements - iCount);
                gc.nArray->SetAt(index, gc.ModArray->GetAt(ii));
                index++;
            }
        }
        
        gc.rv = gc.nArray;
    }
    else 
        gc.rv = gc.ModArray;
    
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(gc.rv);
}
FCIMPLEND


FCIMPL2(Object*, AssemblyNative::GetModule, Object* refThisUNSAFE, StringObject* strFileNameUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    OBJECTREF rv            = NULL;
    STRINGREF strFileName   = (STRINGREF) strFileNameUNSAFE;
    CQuickBytes qbLC;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, strFileName);

    if (strFileName == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_FileName");
    if(*(strFileName->GetBuffer()) == L'\0')
        COMPlusThrow(kArgumentException, L"Argument_EmptyFileName");
    
    DomainFile *pModule;
    MAKE_UTF8PTR_FROMWIDE(szModuleName, strFileName->GetBuffer());

    // Need to perform case insensitive lookup.
    {
        UTF8_TO_LOWER_CASE(szModuleName, qbLC);
        szModuleName = (LPUTF8) qbLC.Ptr();
    }

    HashDatum datum;
    if (pAssembly->GetAssembly()->m_pAllowedFiles->GetValue(szModuleName, &datum)) {
        if (datum) { 
            // internal module
            mdFile  tokFile = (mdFile)(UINT_PTR)datum;
                
            pModule = pAssembly->GetModule()->LoadModule(GetAppDomain(), tokFile);
        }
        else // manifest module
            pModule = pAssembly->GetDomainAssembly();
            
        rv = pModule->GetExposedModuleObject();
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND

static Object* __fastcall GetExportedTypesInternal(Assembly* pAssembly);

FCIMPL1(Object*, AssemblyNative::GetExportedTypes, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    OBJECTREF   rv      = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, rv);

    rv = ObjectToOBJECTREF(GetExportedTypesInternal(pAssembly->GetAssembly()));

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND

static Object* __fastcall GetExportedTypesInternal(Assembly* pAssembly)
{
    CONTRACTL 
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
    }
    CONTRACTL_END;

    PTRARRAYREF  rv         = NULL;
    MethodTable *pTypeClass = g_Mscorlib.GetClass(CLASS__TYPE);
    
    IMDInternalImport *pImport = pAssembly->GetManifestImport();

    HENUMInternalHolder phCTEnum(pImport);
    HENUMTypeDefInternalHolder phTDEnum(pImport);

    phCTEnum.EnumInit(mdtExportedType, mdTokenNil);
    DWORD dwElements = pImport->EnumGetCount(&phCTEnum);

    phTDEnum.EnumTypeDefInit();
    dwElements += pImport->EnumGetCount(&phTDEnum);

    mdExportedType mdCT;
    mdTypeDef mdTD;
    LPCSTR pszNameSpace;
    LPCSTR pszClassName;
    DWORD dwFlags;
    int iCount = 0;
    
    struct _gc {
        PTRARRAYREF TypeArray;
        PTRARRAYREF nArray;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);
    
    gc.TypeArray = (PTRARRAYREF) AllocateObjectArray(dwElements, pTypeClass);
        
    while(pImport->EnumNext(&phTDEnum, &mdTD)) {
        mdTypeDef mdEncloser;
        TypeHandle typeHnd;
        
        pImport->GetTypeDefProps(mdTD,
                                 &dwFlags,
                                 NULL);
        mdEncloser = mdTD;
        
        // nested type
        while (SUCCEEDED(pImport->GetNestedClassProps(mdEncloser, &mdEncloser)) &&
               IsTdNestedPublic(dwFlags)) {
            pImport->GetTypeDefProps(mdEncloser,
                                     &dwFlags,
                                     NULL);
        }
        
        if (IsTdPublic(dwFlags)) {
            typeHnd = ClassLoader::LoadTypeDefThrowing(pAssembly->GetManifestModule(), mdTD, 
                                                       ClassLoader::ThrowIfNotFound, 
                                                       ClassLoader::PermitUninstDefOrRef);
            
            OBJECTREF o = typeHnd.GetManagedClassObject();
            gc.TypeArray->SetAt(iCount, o);
            iCount++;
        }
    }
    
    
    // Now get the ExportedTypes that don't have TD's in the manifest file
    while(pImport->EnumNext(&phCTEnum, &mdCT)) {
        mdToken mdImpl;
        TypeHandle typeHnd;
        pImport->GetExportedTypeProps(mdCT,
                                      &pszNameSpace,
                                      &pszClassName,
                                      &mdImpl,
                                      NULL, //binding
                                      &dwFlags);
            
        // nested type
        while ((TypeFromToken(mdImpl) == mdtExportedType) &&
               (mdImpl != mdExportedTypeNil) &&
               IsTdNestedPublic(dwFlags))           
            pImport->GetExportedTypeProps(mdImpl,
                                          NULL, //namespace
                                          NULL, //name
                                          &mdImpl,
                                          NULL, //binding
                                          &dwFlags);
            
        if ((TypeFromToken(mdImpl) == mdtFile) &&
            (mdImpl != mdFileNil) &&
            IsTdPublic(dwFlags)) {
            
            NameHandle typeName(pszNameSpace, pszClassName);
            typeName.SetTypeToken(pAssembly->GetManifestModule(), mdCT);
            typeHnd = pAssembly->GetLoader()->LoadTypeHandleThrowIfFailed(&typeName);
            
            OBJECTREF o = typeHnd.GetManagedClassObject();
            gc.TypeArray->SetAt(iCount, o);
            iCount++;
        }
    }
    
    gc.nArray = (PTRARRAYREF) AllocateObjectArray(iCount, pTypeClass);
    for(int i = 0; i < iCount; i++)
        gc.nArray->SetAt(i, gc.TypeArray->GetAt(i));
    
    rv = gc.nArray;
        
    GCPROTECT_END();
    
    return OBJECTREFToObject(rv);
}

FCIMPL1(Object*, AssemblyNative::GetResourceNames, Object* refThisUNSAFE)
{
    CONTRACTL 
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    PTRARRAYREF rv = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, rv);

    IMDInternalImport *pImport = pAssembly->GetMDImport();

    HENUMInternalHolder phEnum(pImport);
    DWORD dwCount;

    phEnum.EnumInit(mdtManifestResource, mdTokenNil);
        dwCount = pImport->EnumGetCount(&phEnum);

    PTRARRAYREF ItemArray = (PTRARRAYREF) AllocateObjectArray(dwCount, g_pStringClass);

    mdManifestResource mdResource;

    GCPROTECT_BEGIN(ItemArray);
    for(DWORD i = 0;  i < dwCount; i++) {
        pImport->EnumNext(&phEnum, &mdResource);
        LPCSTR pszName = NULL;
        
        pImport->GetManifestResourceProps(mdResource,
                                          &pszName, // name
                                          NULL, // linkref
                                          NULL, // offset
                                          NULL); //flags
           
        OBJECTREF o = (OBJECTREF) COMString::NewString(pszName);
        ItemArray->SetAt(i, o);
    }
     
    rv = ItemArray;
    GCPROTECT_END();
    
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND

FCIMPL1(Object*, AssemblyNative::GetReferencedAssemblies, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    PTRARRAYREF rv  = NULL;   

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, rv);

    rv = GetReferencedAssembliesInternal(pAssembly->GetAssembly());

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND

PTRARRAYREF AssemblyNative::GetReferencedAssembliesInternal(Assembly *pAssembly)
{
    STATIC_CONTRACT_THROWS;

    PTRARRAYREF rv = NULL;   

    IMDInternalImport *pImport = pAssembly->GetManifestImport();

    MethodTable* pAsmNameClass = g_Mscorlib.GetClass(CLASS__ASSEMBLY_NAME);

    HENUMInternal phEnum;
    DWORD dwCount = 0;

    if (pImport->EnumInit(mdtAssemblyRef,
                          mdTokenNil,
                          &phEnum) == S_OK)
        dwCount = pImport->EnumGetCount(&phEnum);
    else
        dwCount = 0;
    
    mdAssemblyRef mdAssemblyRef;

    struct _gc {
        PTRARRAYREF ItemArray;
        ASSEMBLYNAMEREF pObj;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    
    GCPROTECT_BEGIN(gc);
 
    EX_TRY_FOR_FINALLY
    {
        
        gc.ItemArray = (PTRARRAYREF) AllocateObjectArray(dwCount, pAsmNameClass);
        
        for(DWORD i = 0; i < dwCount; i++) 
        {
            pImport->EnumNext(&phEnum, &mdAssemblyRef);
            
            LPCSTR pszName;
            const void *pbPublicKeyOrToken;
            DWORD cbPublicKeyOrToken;
            DWORD dwAssemblyRefFlags;
            AssemblyMetaDataInternal context;
            const void *pbHashValue;
            DWORD cbHashValue;
            
            ZeroMemory(&context, sizeof(context));
            pImport->GetAssemblyRefProps(mdAssemblyRef,        // [IN] The AssemblyRef for which to get the properties.        
                                         &pbPublicKeyOrToken,  // [OUT] Pointer to the public key or token.
                                         &cbPublicKeyOrToken,  // [OUT] Count of bytes in the public key or token.
                                         &pszName,             // [OUT] Buffer to fill with name.                              
                                         &context,             // [OUT] Assembly MetaData.                                     
                                         &pbHashValue,         // [OUT] Hash blob.                                             
                                         &cbHashValue,         // [OUT] Count of bytes in the hash blob.                       
                                         &dwAssemblyRefFlags); // [OUT] Flags.                                             
            
            gc.pObj = (ASSEMBLYNAMEREF) AllocateObject(pAsmNameClass);
            AssemblyNameNative::AssemblyNameInit(&gc.pObj,
                                                 pszName,
                                                 pbPublicKeyOrToken,
                                                 cbPublicKeyOrToken,
                                                 context.usMajorVersion,
                                                 context.usMinorVersion,
                                                 context.usBuildNumber,
                                                 context.usRevisionNumber,
                                                 context.szLocale,
                                                 NULL, //codebase
                                                 dwAssemblyRefFlags,
                                                 pAssembly->GetHashAlgId());
            
            gc.ItemArray->SetAt(i, (OBJECTREF) gc.pObj);
        }
        
        rv = gc.ItemArray;
    }
    EX_FINALLY 
    {
        pImport->EnumClose(&phEnum);
    } 
    EX_END_FINALLY 
    
    GCPROTECT_END();
    pImport->EnumClose(&phEnum);

    return rv;
}

FCIMPL1(MethodDesc*, AssemblyNative::GetEntryPoint, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    MethodDesc* pMeth = NULL;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    HELPER_METHOD_FRAME_BEGIN_RET_NOPOLL();

    pMeth = pAssembly->GetAssembly()->GetEntryPoint();

    HELPER_METHOD_FRAME_END();
    return pMeth;
}
FCIMPLEND

// prepare saving manifest to disk
FCIMPL2(void,  AssemblyNative::PrepareSavingManifest, Object* refThisUNSAFE, ReflectModuleBaseObject* moduleUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    DomainAssembly*      pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    REFLECTMODULEBASEREF pReflect  = (REFLECTMODULEBASEREF) moduleUNSAFE;
    ReflectionModule     *pModule  = NULL;

    HELPER_METHOD_FRAME_BEGIN_1(pReflect);

    if (pReflect != NULL) {
        pModule = (ReflectionModule*) pReflect->GetData();
        _ASSERTE(pModule);
    }

    pAssembly->GetAssembly()->PrepareSavingManifest(pModule);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// add a file name to the file list of this assembly. On disk only.
FCIMPL2(mdFile, AssemblyNative::AddFileList, Object* refThisUNSAFE, StringObject* strFileNameUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly*   pAssembly   = GET_ASSEMBLY(refThisUNSAFE);
    STRINGREF   strFileName = (STRINGREF) strFileNameUNSAFE;
    mdFile      retVal = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_1(strFileName);
    
    retVal = pAssembly->GetAssembly()->AddFileList(strFileName->GetBuffer());
    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND

// set the hash value on a file.
FCIMPL3(void, AssemblyNative::SetHashValue, Object* refThisUNSAFE, INT32 tkFile, StringObject* strFullFileNameUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    _ASSERTE(strFullFileNameUNSAFE != NULL);

    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    STRINGREF strFullFileName = (STRINGREF) strFullFileNameUNSAFE;


    HELPER_METHOD_FRAME_BEGIN_1(strFullFileName);
    
    pAssembly->GetAssembly()->SetHashValue(tkFile, strFullFileName->GetBuffer());
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// add a Type name to COMType table. On disk only.
FCIMPL5(mdExportedType,  AssemblyNative::AddExportedType, Object* refThisUNSAFE, StringObject* strCOMTypeNameUNSAFE, INT32 ar, INT32 tkTypeDef, INT32 flags)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly*       pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    STRINGREF       strCOMTypeName  = (STRINGREF) strCOMTypeNameUNSAFE;
    mdExportedType  retVal = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_1(strCOMTypeName);
    retVal = pAssembly->GetAssembly()->AddExportedType(strCOMTypeName->GetBuffer(), ar, tkTypeDef, (CorTypeAttr)flags);
    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND

// add a Stand alone resource to ManifestResource table
FCIMPL5(void, AssemblyNative::AddStandAloneResource, Object* refThisUNSAFE, StringObject* strNameUNSAFE, StringObject* strFileNameUNSAFE, StringObject* strFullFileNameUNSAFE, INT32 iAttribute)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);

    struct _gc
    {
        STRINGREF strName;
        STRINGREF strFileName;
        STRINGREF strFullFileName;
    } gc;

    gc.strName          = (STRINGREF) strNameUNSAFE;
    gc.strFileName      = (STRINGREF) strFileNameUNSAFE;
    gc.strFullFileName  = (STRINGREF) strFullFileNameUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc);
    
    StackSString name;
    StackSString filename;
    StackSString fullfilename;
    gc.strName->GetSString (name);
    gc.strFileName->GetSString (filename);
    gc.strFullFileName->GetSString (fullfilename);

    pAssembly->GetAssembly()->AddStandAloneResource(
        name.GetUnicode (), 
        NULL,
        NULL,
        filename.GetUnicode (),
        fullfilename.GetUnicode (),        
        iAttribute); 

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// Save security permission requests.
FCIMPL4(void, AssemblyNative::SavePermissionRequests, Object* refThisUNSAFE, U1Array* requiredUNSAFE, U1Array* optionalUNSAFE, U1Array* refusedUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);

    struct _gc
    {
        U1ARRAYREF required;
        U1ARRAYREF optional;
        U1ARRAYREF refused;
    } gc;

    gc.required = (U1ARRAYREF) requiredUNSAFE;
    gc.optional = (U1ARRAYREF) optionalUNSAFE;
    gc.refused  = (U1ARRAYREF) refusedUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc);

    pAssembly->GetAssembly()->SavePermissionRequests(gc.required, gc.optional, gc.refused);

    HELPER_METHOD_FRAME_END_POLL();
}
FCIMPLEND

// save the manifest to disk!
extern void ManagedBitnessFlagsToUnmanagedBitnessFlags(
    INT32 portableExecutableKind, INT32 imageFileMachine,
    DWORD* pPeFlags, DWORD* pCorhFlags);

FCIMPL6(void, AssemblyNative::SaveManifestToDisk, Object* refThisUNSAFE, StringObject* strManifestFileNameUNSAFE, INT32 entrypoint, INT32 fileKind, INT32 portableExecutableKind, INT32 imageFileMachine)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    DWORD peFlags = 0, corhFlags = 0;
    HELPER_METHOD_FRAME_BEGIN_NOPOLL();
    ManagedBitnessFlagsToUnmanagedBitnessFlags(portableExecutableKind, imageFileMachine, &peFlags, &corhFlags);   

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    
    // Make a copy of the file name, GC could move strManifestFileName

    CQuickBytes qb;
    LPWSTR pwszFileName = (LPWSTR) qb.AllocThrows((strManifestFileNameUNSAFE->GetStringLength() + 1) * sizeof(WCHAR));

    memcpyNoGCRefs(pwszFileName, strManifestFileNameUNSAFE->GetBuffer(),
            (strManifestFileNameUNSAFE->GetStringLength() + 1) * sizeof(WCHAR));

    pAssembly->GetAssembly()->SaveManifestToDisk(pwszFileName, entrypoint, fileKind, corhFlags, peFlags);

    HELPER_METHOD_FRAME_END_POLL();
}
FCIMPLEND

// Add a file entry into the in memory file list of this manifest
FCIMPL3(void, AssemblyNative::AddFileToInMemoryFileList, Object* refThisUNSAFE, StringObject* strModuleFileNameUNSAFE, Object* refModuleUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    REFLECTMODULEBASEREF    pReflect;
    Module                  *pModule = NULL;
    pReflect = (REFLECTMODULEBASEREF) ObjectToOBJECTREF(refModuleUNSAFE);
    _ASSERTE(pReflect);

    pModule = (Module*) pReflect->GetData();
    _ASSERTE(pModule);
    
    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    pAssembly->GetAssembly()->AddFileToInMemoryFileList(strModuleFileNameUNSAFE->GetBuffer(), pModule);

    HELPER_METHOD_FRAME_END_POLL();
}
FCIMPLEND


FCIMPL1(Object*, AssemblyNative::GetStringizedName, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    STRINGREF   refRetVal   = NULL;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly*   pAssembly   = GET_ASSEMBLY(refThisUNSAFE);

    // If called by Object.ToString(), pAssembly may be NULL.
    if (pAssembly) {
        HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

        StackSString name;
        pAssembly->GetFile()->GetDisplayName(name);
    
        refRetVal = COMString::NewString(name);
    
        HELPER_METHOD_FRAME_END();
    }

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


FCIMPL1(Object*, AssemblyNative::GetExecutingAssembly, StackCrawlMark* stackMark)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    ASSEMBLYREF refRetVal = NULL;
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    Assembly* pAssembly = SystemDomain::GetCallersAssembly(stackMark);

    if(pAssembly)
        refRetVal = (ASSEMBLYREF) pAssembly->GetExposedObject();

    HELPER_METHOD_FRAME_END();
    
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


FCIMPL0(Object*, AssemblyNative::GetEntryAssembly)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    OBJECTREF rv       = NULL;
    AppDomain *pDomain = GetAppDomain();

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, rv);

    if(pDomain->m_pRootAssembly)
        rv = pDomain->m_pRootAssembly->GetExposedObject();
    
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND

FCIMPL2(Object*, AssemblyNative::CreateQualifiedName, StringObject* strAssemblyNameUNSAFE, StringObject* strTypeNameUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    OBJECTREF   rv              = NULL;

    STRINGREF   strAssemblyName = (STRINGREF) strAssemblyNameUNSAFE;
    STRINGREF   strTypeName     = (STRINGREF) strTypeNameUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, strAssemblyName, strTypeName);

    LPWSTR pTypeName = NULL;
    DWORD  dwTypeName = 0;
    LPWSTR pAssemblyName = NULL;
    DWORD  dwAssemblyName = 0;
    CQuickBytes qb;
    
    if(strTypeName != NULL) {
        pTypeName = strTypeName->GetBuffer();
        dwTypeName = strTypeName->GetStringLength();
    }

    if(strAssemblyName != NULL) {
        pAssemblyName = strAssemblyName->GetBuffer();
        dwAssemblyName = strAssemblyName->GetStringLength();
    }

    DWORD length = dwTypeName + dwAssemblyName + ASSEMBLY_SEPARATOR_LEN + 1;
    LPWSTR result = (LPWSTR) qb.AllocThrows(length * sizeof(WCHAR));

    if(ns::MakeAssemblyQualifiedName(result,
                                     length,
                                     pTypeName,
                                     dwTypeName,
                                     pAssemblyName,
                                     dwAssemblyName)) 
        rv = (OBJECTREF) COMString::NewString(result);

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND

FCIMPL3(void, AssemblyNative::GetGrantSet, Object* refThisUNSAFE, OBJECTREF* ppGranted, OBJECTREF* ppDenied)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_0();

    AssemblySecurityDescriptor *pSecDesc = pAssembly->GetSecurityDescriptor();

    Security::Resolve(pSecDesc);

    OBJECTREF granted = Security::GetGrantedPermissionSet(pSecDesc, ppDenied);
    *ppGranted = granted;

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// return the on disk assembly module for reflection emit. This only works for dynamic assembly.
FCIMPL1(Object*, AssemblyNative::GetOnDiskAssemblyModule, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE); 
    OBJECTREF rv = NULL; 

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, rv);

    ReflectionModule *mod;

    mod = pAssembly->GetAssembly()->GetOnDiskManifestModule();
    _ASSERTE(mod);

    // Assign the return value  
    rv = (OBJECTREF) mod->GetExposedObject();     

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND

// return the in memory assembly module for reflection emit. This only works for dynamic assembly.
FCIMPL1(Object*, AssemblyNative::GetInMemoryAssemblyModule, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    OBJECTREF rv = NULL; 

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, rv);

    // get the corresponding managed ModuleBuilder class
    rv = pAssembly->GetExposedModuleObject();

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND



FCIMPL1(FC_BOOL_RET, AssemblyNative::GlobalAssemblyCache, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);

    BOOL rv = FALSE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(refThisUNSAFE);
    rv = pAssembly->GetFile()->IsSourceGAC();
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(rv);
}
FCIMPLEND

FCIMPL1(Object*, AssemblyNative::GetImageRuntimeVersion, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    PEFile* pPEFile = NULL;
    
    STRINGREF VersionString = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, VersionString);
    
    // Retrieve the PEFile from the assembly.
    pPEFile = pAssembly->GetFile();
    PREFIX_ASSUME(pPEFile!=NULL);

    LPCSTR pszVersion = NULL;
    HRESULT hr = pPEFile->GetMDImport()->GetVersionString(&pszVersion);
    
    IfFailThrow(hr);

    SString pVersion(SString::Literal, pszVersion);
    
    AdjustImageRuntimeVersion(&pVersion);

    // Allocate a managed string that contains the version and return it.
    VersionString = COMString::NewString(pVersion);

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject((OBJECTREF)VersionString);
}
FCIMPLEND

FCIMPL1(INT64, AssemblyNative::GetHostContext, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    UINT64 Context = 0;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException))

    DomainAssembly* pAssembly = GET_ASSEMBLY(refThisUNSAFE);
    IHostAssembly *pIHostAssembly = pAssembly->GetFile()->GetIHostAssembly();
    if (pIHostAssembly)
        pIHostAssembly->GetAssemblyContext(&Context);
    END_SO_INTOLERANT_CODE;
    return Context;
}
FCIMPLEND

