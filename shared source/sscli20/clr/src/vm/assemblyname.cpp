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
** Header:  AssemblyName.cpp
**
** Purpose: Implements AssemblyName (loader domain) architecture
**
** Date:  August 10, 1999
**
===========================================================*/

#include "common.h"

#include <stdlib.h>
#include <shlwapi.h>

#include "assemblyname.hpp"
#include "comstring.h"
#include "security.h"
#include "field.h"
#include "fusion.h"
#include "strongname.h"
#include "eeconfig.h"

FCIMPL1(Object*, AssemblyNameNative::GetFileInformation, StringObject* filenameUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    struct _gc
    {
        ASSEMBLYNAMEREF result;
        STRINGREF       filename;
        OBJECTREF       assemblyObj;
    } gc;

    gc.result   = NULL;
    gc.filename = (STRINGREF) filenameUNSAFE;
    gc.assemblyObj = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    if (gc.filename == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_FileName");

    if (gc.filename->GetStringLength() == 0)
        COMPlusThrow(kArgumentException, L"Argument_EmptyFileName");

    Thread *pThread = GetThread();
    CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease

    AssemblySpec spec;
    spec.SetCodeBase(&(pThread->m_MarshalAlloc), &gc.filename);
    spec.SetIntrospectionOnly(TRUE);

    AppDomain *pAppDomain = GetAppDomain();

    PEFileHolder pFile;
    SafeComHolder<IAssembly> pIAssembly;  
    if (pAppDomain->IsCached(&spec)) {
        pFile = pAppDomain->FindCachedFile(&spec);
        pFile->AddRef();
    }
    else {
        // Don't call BindAssemblySpec, as it will cache the result and we
        // don't want to hold the file open.

        SafeComHolder<IHostAssembly> pIHostAssembly;
        SafeComHolder<IFusionBindLog> pFusionLog;
        BOOL fIsWellKnown = spec.FindAssemblyFile(pAppDomain, TRUE,
                                                  &pIAssembly, &pIHostAssembly, NULL,
                                                  &pFusionLog);

        if (fIsWellKnown &&
            pAppDomain->GetRootAssembly() &&
            pIAssembly == pAppDomain->GetRootAssembly()->GetFusionAssembly()) {
            // This is a shortcut to avoid opening another copy of the process exe.
            // In fact, we have other similar cases where we've called
            // ExplicitBind() rather than normal binding, which aren't covered here.
            
            
            pFile = pAppDomain->GetRootAssembly()->GetManifestFile();
            pFile->AddRef();
        }

        if (pIHostAssembly)
            pFile = PEAssembly::Open(pIHostAssembly, spec.IsMscorlibSatellite(),
                                     spec.IsIntrospectionOnly());
    }

    if (pFile) {
        pFile->MakeMDImportPersistent();// we're going to kill the object soon
        DomainAssembly assembly(GetAppDomain(), pFile, NULL, NULL);

        gc.assemblyObj = assembly.GetExposedAssemblyObject();

        MethodDescCallSite getName(METHOD__ASSEMBLY__GET_NAME, &gc.assemblyObj);

        ARG_SLOT args[1] = {
            ObjToArgSlot(gc.assemblyObj)
        };

        gc.result = (ASSEMBLYNAMEREF) getName.Call_RetOBJECTREF(args);
    }
    else {
        StackSString wszPath;
        if (pIAssembly)
            FusionBind::GetAssemblyManifestModulePath(pIAssembly, wszPath);
        //else                                       

        if (!wszPath.IsEmpty()) {

            PEImageHolder pImage(PEImage::OpenImage(wszPath));

            if(pImage == NULL)
                IfFailThrow(COR_E_FILENOTFOUND);

            pFile = PEFile::Open(pImage);

            if((pFile == NULL)||(!pFile->HasMetadata()))
                IfFailThrow(COR_E_ASSEMBLYEXPECTED);

            SafeComHolder<IMDInternalImport> pIMDI;
            pIMDI = pFile->GetMDImportWithRef();

            SafeComHolder<IMetaDataAssemblyImport> pMDAsmImport;
            IfFailThrow(GetMetaDataPublicInterfaceFromInternal(pIMDI, IID_IMetaDataAssemblyImport, (void**)&pMDAsmImport));
            
            mdAssembly mdAssembly;
            IfFailThrow(pMDAsmImport->GetAssemblyFromScope(&mdAssembly));

            DWORD cbPublicKeyOrToken = 0;
            DWORD cbAssemblyName = 0;
            ASSEMBLYMETADATA context;
            ZeroMemory(&context, sizeof(context));

            pMDAsmImport->GetAssemblyProps(mdAssembly,
                                           NULL,
                                           &cbPublicKeyOrToken,
                                           NULL, 
                                           NULL,
                                           0,
                                           &cbAssemblyName,
                                           &context,
                                           NULL);

            CQuickBytes qb;
            const void *pbPublicKeyOrToken = qb.AllocThrows(cbPublicKeyOrToken);

            CQuickBytes qb2;
            LPWSTR szAssemblyName = (LPWSTR) qb2.AllocThrows(cbAssemblyName * sizeof(WCHAR));

            context.szLocale = (LPWSTR) _alloca(context.cbLocale * sizeof(WCHAR));

            DWORD dwHashAlg;
            DWORD dwFlags;
            IfFailThrow(pMDAsmImport->GetAssemblyProps(mdAssembly,
                                                       &pbPublicKeyOrToken,
                                                       &cbPublicKeyOrToken,
                                                       &dwHashAlg, 
                                                       szAssemblyName,
                                                       cbAssemblyName,
                                                       &cbAssemblyName,
                                                       &context,
                                                       &dwFlags));
            if (cbPublicKeyOrToken != 0)
                dwFlags |= afPublicKey;
            
            // Determine processor architecture and pass it to the flags
            dwFlags &= ~afPA_FullMask;   // afPA_FullMask=0xF0 (CorHdr.h)
            if(pIMDI->GetMetadataStreamVersion() > MD_STREAM_VER_1X)
            {
                dwFlags |= PAFlag(pImage->GetFusionProcessorArchitecture()); // Macro PAFlag is defined in CorHdr.h
            }

            MAKE_UTF8PTR_FROMWIDE(pName, (cbAssemblyName ? szAssemblyName : L""));
            MAKE_UTF8PTR_FROMWIDE(pCulture, (context.cbLocale ? context.szLocale : L""));

            PEAssembly::PathToUrl(wszPath);

            MethodTable* pAsmNameClass = g_Mscorlib.GetClass(CLASS__ASSEMBLY_NAME);
            gc.result = (ASSEMBLYNAMEREF) AllocateObject(pAsmNameClass);

            AssemblyNameInit(&gc.result,
                             pName,
                             (PBYTE) pbPublicKeyOrToken,
                             cbPublicKeyOrToken,
                             context.usMajorVersion,
                             context.usMinorVersion,
                             context.usBuildNumber,
                             context.usRevisionNumber,
                             pCulture,
                             wszPath,
                             dwFlags,
                             dwHashAlg);
        } // end if (!wszPath.IsEmpty()) 
    } // end if (pFile) -- else


    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(gc.result);
}
FCIMPLEND

FCIMPL1(Object*, AssemblyNameNative::ToString, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF pObj          = NULL;
    ASSEMBLYNAMEREF pThis   = (ASSEMBLYNAMEREF) (OBJECTREF) refThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, pThis);

    if (pThis == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    Thread *pThread = GetThread();

    CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease

    AssemblySpec spec;
    spec.InitializeSpec(&(pThread->m_MarshalAlloc), (ASSEMBLYNAMEREF*) &pThis, FALSE, FALSE); 

    StackSString name;
    spec.GetFileOrDisplayName(0, name);

    pObj = (OBJECTREF) COMString::NewString(name);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(pObj);
}
FCIMPLEND

FCIMPL1(Object*, AssemblyNameNative::GetPublicKeyToken, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF orOutputArray = NULL;
    OBJECTREF refThis       = (OBJECTREF) refThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refThis);

    if (refThis == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    ASSEMBLYNAMEREF orThis = (ASSEMBLYNAMEREF)refThis;
    U1ARRAYREF orPublicKey = orThis->GetPublicKey();

    if (orPublicKey != NULL) {
        DWORD cb = orPublicKey->GetNumComponents();
        StrongNameHolder pbToken;

        if (cb) {    
            CQuickBytes qb;
            BYTE *pbKey = (BYTE*) qb.AllocThrows(cb);
            memcpy(pbKey, orPublicKey->GetDataPtr(), cb);

            {
                GCX_PREEMP();
                if (!StrongNameTokenFromPublicKey(pbKey, cb, &pbToken, &cb))
                    COMPlusThrowHR(StrongNameErrorInfo());
            }
        }

        Security::CopyEncodingToByteArray(pbToken, cb, &orOutputArray);
    }

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(orOutputArray);
}
FCIMPLEND


FCIMPL1(Object*, AssemblyNameNative::EscapeCodeBase, StringObject* filenameUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    STRINGREF rv        = NULL;
    STRINGREF filename  = (STRINGREF) filenameUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, filename);

    LPWSTR pCodeBase = NULL;
    DWORD  dwCodeBase = 0;
    CQuickBytes qb;

    if (filename != NULL) {
        WCHAR* pString;
        int    iString;
        RefInterpretGetStringValuesDangerousForGC(filename, &pString, &iString);
        dwCodeBase = (DWORD) iString;
        pCodeBase = (LPWSTR) qb.AllocThrows((++dwCodeBase) * sizeof(WCHAR));
        memcpy(pCodeBase, pString, dwCodeBase*sizeof(WCHAR));
    }

    if(pCodeBase) {
        CQuickBytes qb2;
        DWORD dwEscaped = 1;
        UrlEscape(pCodeBase, (LPWSTR) qb2.Ptr(), &dwEscaped, 0);

        LPWSTR result = (LPWSTR)qb2.AllocThrows((++dwEscaped) * sizeof(WCHAR));
        HRESULT hr = UrlEscape(pCodeBase, result, &dwEscaped, 0);

        if (SUCCEEDED(hr))
            rv = COMString::NewString(result);
        else
            COMPlusThrowHR(hr);
    }

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(rv);
}
FCIMPLEND

FCIMPL4(HRESULT, AssemblyNameNative::Init, Object* refThisUNSAFE, OBJECTREF *pAssemblyRef, CLR_BOOL fForIntrospection, CLR_BOOL fRaiseResolveEvent)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    ASSEMBLYNAMEREF pThis = (ASSEMBLYNAMEREF) (OBJECTREF) refThisUNSAFE;
    HRESULT hr = S_OK;
    
    HELPER_METHOD_FRAME_BEGIN_RET_1(pThis);
    
    *pAssemblyRef = NULL;

    if (pThis == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    Thread *pThread = GetThread();

    CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease

    AssemblySpec spec;
    hr = spec.InitializeSpec(&(pThread->m_MarshalAlloc), (ASSEMBLYNAMEREF*) &pThis, TRUE, FALSE); 

    if (SUCCEEDED(hr))
    {
        AssemblyNameInit(&pThis,
                 spec.m_pAssemblyName,
                 spec.m_pbPublicKeyOrToken,
                 spec.m_cbPublicKeyOrToken,
                 spec.m_context.usMajorVersion,
                 spec.m_context.usMinorVersion,
                 spec.m_context.usBuildNumber,
                 spec.m_context.usRevisionNumber,
                 spec.m_context.szLocale,
                 NULL, // codebase
                 spec.m_dwFlags,
                 0); // AssemblyHashAlgorithm.None
    }
    else if ((hr == FUSION_E_INVALID_NAME) && fRaiseResolveEvent)
    {
        if (pAssemblyRef != NULL)
        {
            Assembly *pAssembly = GetAppDomain()->RaiseAssemblyResolveEvent(&spec, fForIntrospection, FALSE);

            if (pAssembly == NULL)
                EEFileLoadException::Throw(&spec, hr);
            else
                *((OBJECTREF*) (&(*pAssemblyRef))) = pAssembly->GetExposedObject();
        }
        else
            EEFileLoadException::Throw(&spec, hr);
    }
    else
    {
        ThrowHR(hr);
    }
    
    HELPER_METHOD_FRAME_END();
    return hr;
}
FCIMPLEND

void AssemblyNameNative::AssemblyNameInit(ASSEMBLYNAMEREF *pAsmName,
                                          LPCSTR szName,
                                          const void *pbPublicKeyOrToken,
                                          DWORD cbPublicKeyOrToken,
                                          USHORT usMajorVersion,
                                          USHORT usMinorVersion,
                                          USHORT usBuildNumber,
                                          USHORT usRevisionNumber,
                                          LPCSTR szLocale,
                                          LPCWSTR wszCodeBase,
                                          DWORD dwFlags,
                                          DWORD dwHashAlgId)
{
    CONTRACTL 
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        PRECONDITION(IsProtectedByGCFrame (pAsmName));
    }
    CONTRACTL_END;
    
    struct _gc {
        OBJECTREF CultureInfo;
        STRINGREF Locale;
        OBJECTREF Version;
        U1ARRAYREF PublicKeyOrToken;
        STRINGREF Name;
        STRINGREF CodeBase;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    
    GCPROTECT_BEGIN(gc);
    
    if ((usMajorVersion != (USHORT) -1) &&
        (usMinorVersion != (USHORT) -1)) {

        MethodTable* pVersion = g_Mscorlib.GetClass(CLASS__VERSION);
    
        // version
        gc.Version = AllocateObject(pVersion);


        MethodDescCallSite ctorMethod(METHOD__VERSION__CTOR);
            
        ARG_SLOT VersionArgs[5] =
        {
            ObjToArgSlot(gc.Version),
            (ARG_SLOT) usMajorVersion,      
            (ARG_SLOT) usMinorVersion,
            (ARG_SLOT) usBuildNumber,
            (ARG_SLOT) usRevisionNumber,
        };
        ctorMethod.Call(VersionArgs);
    }
    
    // cultureinfo
    if (szLocale) {
        
        MethodTable* pCI = g_Mscorlib.GetClass(CLASS__CULTURE_INFO);
        gc.CultureInfo = AllocateObject(pCI);
        
        gc.Locale = COMString::NewString(szLocale);

        MethodDescCallSite strCtor(METHOD__CULTURE_INFO__STR_CTOR);
        
        ARG_SLOT args[2] = 
        {
            ObjToArgSlot(gc.CultureInfo),
            ObjToArgSlot(gc.Locale)
        };
        
        strCtor.Call(args);
    }
    

    // public key or token byte array
    if (pbPublicKeyOrToken)
        Security::CopyEncodingToByteArray((BYTE*) pbPublicKeyOrToken,
                                                cbPublicKeyOrToken,
                                                (OBJECTREF*) &gc.PublicKeyOrToken);

    // simple name
    if(szName)
        gc.Name = COMString::NewString(szName);
    
    if (wszCodeBase)
        gc.CodeBase = COMString::NewString(wszCodeBase);


    MethodDescCallSite init(METHOD__ASSEMBLY_NAME__INIT);
    
    BOOL fPublicKey = dwFlags & afPublicKey;
    ARG_SLOT MethodArgs[] =
    {
        ObjToArgSlot(*pAsmName),
        ObjToArgSlot(gc.Name),
        fPublicKey ? ObjToArgSlot(gc.PublicKeyOrToken) :
        (ARG_SLOT) NULL, // public key
        fPublicKey ? (ARG_SLOT) NULL :
        ObjToArgSlot(gc.PublicKeyOrToken), // public key token
        ObjToArgSlot(gc.Version),
        ObjToArgSlot(gc.CultureInfo),
        (ARG_SLOT) dwHashAlgId,
        (ARG_SLOT) 1, // AssemblyVersionCompatibility.SameMachine
        ObjToArgSlot(gc.CodeBase),
        (ARG_SLOT) dwFlags,
        (ARG_SLOT) NULL // key pair
    };
    
    init.Call(MethodArgs);
    GCPROTECT_END();
}

/* static */
FCIMPL2(FC_BOOL_RET, AssemblyNameNative::ReferenceMatchesDefinition, AssemblyNameBaseObject* refUNSAFE, AssemblyNameBaseObject* defUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    struct _gc
    {
        ASSEMBLYNAMEREF pRef;
        ASSEMBLYNAMEREF pDef;
    } gc;
    gc.pRef = (ASSEMBLYNAMEREF)ObjectToOBJECTREF (refUNSAFE);
    gc.pDef = (ASSEMBLYNAMEREF)ObjectToOBJECTREF (defUNSAFE);

    BOOL result = FALSE;
    HELPER_METHOD_FRAME_BEGIN_RET_PROTECT(gc);

    Thread *pThread = GetThread();

    CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease

    AssemblySpec refSpec;
    refSpec.InitializeSpec(&(pThread->m_MarshalAlloc), (ASSEMBLYNAMEREF*) &gc.pRef, TRUE, FALSE);
    SafeComHolder<IAssemblyName> pRefName (NULL);
    IfFailThrow(refSpec.CreateFusionName(&pRefName, FALSE));

    AssemblySpec defSpec;
    defSpec.InitializeSpec(&(pThread->m_MarshalAlloc), (ASSEMBLYNAMEREF*) &gc.pDef, TRUE, FALSE);
    SafeComHolder <IAssemblyName> pDefName (NULL);
    IfFailThrow(defSpec.CreateFusionName(&pDefName, FALSE));

    // Order matters: Ref->IsEqual(Def)
    result = (S_OK == pRefName->IsEqual(pDefName, ASM_CMPF_IL_ALL));

    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(result);
}
FCIMPLEND
