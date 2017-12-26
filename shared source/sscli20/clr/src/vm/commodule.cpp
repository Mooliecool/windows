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
#include "commodule.h"
#include "comdynamic.h"
#include "reflectclasswriter.h"
#include "class.h"
#include "corpolicy.h"
#include "security.h"
#include "gcscan.h"
#include "ceesectionstring.h"
#include "comvariant.h"
#include <cor.h>
#include "assemblynative.hpp"
#include "typekey.h"

//
// NOTE: this macro must be used prior to entering a helper method frame
//

#define GET_MODULE(pThis) \
    ((Module*)((REFLECTMODULEBASEREF)ObjectToOBJECTREF(pThis))->GetData())

#define STATE_EMPTY 0
#define STATE_ARRAY 1

//SIG_* are defined in DescriptorInfo.cs and must be kept in sync.
#define SIG_BYREF        0x0001
#define SIG_DEFAULTVALUE 0x0002
#define SIG_IN           0x0004
#define SIG_INOUT        0x0008
#define SIG_STANDARD     0x0001
#define SIG_VARARGS      0x0002

// This function will help clean up after a ISymUnmanagedWriter (if it can't
// clean up on it's own
void CleanUpAfterISymUnmanagedWriter(void * data)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY; 
        FORBID_FAULT;
    }
    CONTRACTL_END;

    CGrowableStream * s = (CGrowableStream*)data;
    s->Release();
}// CleanUpAfterISymUnmanagedWriter
    

//inline Module *COMModule::ValidateThisRef(REFLECTMODULEBASEREF pThis)
//{
//    CONTRACTL 
//    {
//        THROWS;
//    }
//    CONTRACTL_END;
//
//    if (pThis == NULL)
//        COMPlusThrow(kNullReferenceException, L"NullReference_This");
//
//    Module* pModule = (Module*) pThis->GetData();
//    _ASSERTE(pModule);  
//    return pModule;
//}    

//****************************************
// This function creates a dynamic module underneath the current assembly.
//****************************************
FCIMPL4(Object*, COMModule::DefineDynamicModule, AssemblyBaseObject* containingAssemblyUNSAFE, CLR_BOOL emitSymbolInfo, StringObject* filenameUNSAFE, StackCrawlMark* stackMark)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    ASSEMBLYREF containingAssembly  = (ASSEMBLYREF) containingAssemblyUNSAFE;
    STRINGREF   filename            = (STRINGREF)   filenameUNSAFE;
    OBJECTREF   refModule           = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, containingAssembly, filename);

    Assembly        *pAssembly;
    ReflectionModule *mod;

    _ASSERTE(containingAssembly);
    pAssembly = containingAssembly->GetAssembly();
    _ASSERTE(pAssembly);

    // always create a dynamic module. Note that the name conflict
    // checking is done in managed side.

    SString name;
    if (filename != NULL)
        filename->GetSString(name);

    mod = pAssembly->CreateDynamicModule(name);

    mod->SetCreatingAssembly( SystemDomain::GetCallersAssembly( stackMark ) );

    // get the corresponding managed ModuleBuilder class
    refModule = (OBJECTREF) mod->GetExposedObject();  
    _ASSERTE(refModule);    

    // If we need to emit symbol info, we setup the proper symbol
    // writer for this module now.
    if (emitSymbolInfo)
    {
        WCHAR* filenameTemp = NULL;
        
        if ((filename != NULL) &&
            (filename->GetStringLength() > 0))
            filenameTemp = filename->GetBuffer();
        
        _ASSERTE(mod->IsReflection());
        ReflectionModule *rm = mod->GetReflectionModule();
        
        // Create a stream for the symbols to be emitted into. This
        // lives on the Module for the life of the Module.
        CGrowableStream *pStream = new CGrowableStream();
        //pStream->AddRef(); // The Module will keep a copy for it's own use.
        mod->SetInMemorySymbolStream(pStream);

        // Create an ISymUnmanagedWriter and initialize it with the
        // stream and the proper file name. This symbol writer will be
        // replaced with new ones periodically as the symbols get
        // retrieved by the debugger.
        ISymUnmanagedWriter *pWriter;
        
        HRESULT hr = FakeCoCreateInstance(CLSID_CorSymWriter_SxS,
                                          IID_ISymUnmanagedWriter,
                                          (void**)&pWriter);

        if (SUCCEEDED(hr))
        {
            // The other reference is given to the Sym Writer
            // But, the writer takes it's own reference.
            hr = pWriter->Initialize(mod->GetEmitter(),
                                     filenameTemp,
                                     (IStream*)pStream,
                                     TRUE);

            if (SUCCEEDED(hr))
            {
                // Send along some cleanup information
                HelpForInterfaceCleanup *hlp = new HelpForInterfaceCleanup;
                hlp->pData = pStream;
                hlp->pFunction = CleanUpAfterISymUnmanagedWriter;
            
                rm->SetISymUnmanagedWriter(pWriter, hlp);

                // Remember the address of where we've got our
                // ISymUnmanagedWriter stored so we can pass it over
                // to the managed symbol writer object that most of
                // reflection emit will use to write symbols.
                REFLECTMODULEBASEREF ro = (REFLECTMODULEBASEREF)refModule;
                ro->SetInternalSymWriter(rm->GetISymUnmanagedWriterAddr());
            }
        }
        else
        {
            COMPlusThrowHR(hr);
        }
    }
    
    HELPER_METHOD_FRAME_END();

    // Assign the return value  
    return OBJECTREFToObject(refModule);
}
FCIMPLEND


//**************************************************
// LoadInMemoryTypeByName
// Explicitly loading an in memory type
//**************************************************
FCIMPL2(Object*, COMModule::LoadInMemoryTypeByName, ReflectModuleBaseObject* refThisUNSAFE, StringObject* strFullNameUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    OBJECTREF       ret = NULL;
    STRINGREF           strFullName = (STRINGREF) strFullNameUNSAFE;

    TypeHandle      typeHnd;
    UINT            resId = IDS_CLASSLOAD_GENERAL;
    IMetaDataImport *pImport = NULL;
    RefClassWriter  *pRCW;
    mdTypeDef       td;
    LPCWSTR         wzFullName;
    HRESULT         hr = S_OK;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module*             pThisModule = GET_MODULE(refThisUNSAFE);
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, strFullName);

    if (!pThisModule->IsReflection())
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");

    pRCW = ((ReflectionModule*) pThisModule)->GetClassWriter();
    _ASSERTE(pRCW);

    // it is ok to use public import API because this is a dynamic module anyway. We are also receiving Unicode full name as
    // parameter.
    pImport = pRCW->GetRWImporter();

    wzFullName = strFullName->GetBuffer();
    if (wzFullName == NULL)
        IfFailGo( E_FAIL );

    // look up the handle
    IfFailGo( pImport->FindTypeDefByName(wzFullName, mdTokenNil, &td) );     

    {
        GCX_PREEMP();
        TypeKey typeKey(pThisModule, td);
        typeHnd = pThisModule->GetClassLoader()->LoadTypeHandleForTypeKey(&typeKey, TypeHandle());
    }

    if (!typeHnd.IsNull())
        ret = typeHnd.GetManagedClassObject();

ErrExit:
    if (FAILED(hr) && (hr != CLDB_E_RECORD_NOTFOUND))
        COMPlusThrowHR(hr);

    if (ret == NULL) 
    {
            CQuickBytes bytes;
            LPSTR szClassName;
            DWORD cClassName;

            // Get the UTF8 version of strFullName
            szClassName = GetClassStringVars(strFullName, &bytes, 
                                             &cClassName, true);
        pThisModule->GetAssembly()->ThrowTypeLoadException(szClassName, resId);
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(ret);

}
FCIMPLEND

//**************************************************
// GetClassToken
// This function will return the type token given full qual name. If the type
// is defined locally, we will return the TypeDef token. Or we will return a TypeRef token 
// with proper resolution scope calculated.
//**************************************************
FCIMPL5(mdTypeRef, COMModule::GetClassToken, ReflectModuleBaseObject*   refThisUNSAFE, 
                                             StringObject*              strFullNameUNSAFE, 
                                             ReflectModuleBaseObject*   refedModuleUNSAFE, 
                                             StringObject*              strRefedModuleFileNameUNSAFE,
                                             INT32 tkResolutionArg)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    mdTypeRef               tr              = 0;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module*                 pThisModule     = GET_MODULE(refThisUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_NOPOLL();

    RefClassWriter      *pRCW;
    HRESULT             hr;
    mdToken             tkResolution = mdTokenNil;
    Module              *pRefedModule;
    Assembly            *pThisAssembly;
    Assembly            *pRefedAssembly;
    IMetaDataEmit       *pEmit;
    IMetaDataImport     *pImport;
    IMetaDataAssemblyEmit *pAssemblyEmit = NULL;

    struct _gc
    {
        STRINGREF               strFullName;
        REFLECTMODULEBASEREF    refedModule;
        STRINGREF               strRefedModuleFileName;
    } gc;

    gc.strFullName = NULL;
    gc.refedModule = NULL;
    gc.strRefedModuleFileName = NULL;

    GCPROTECT_BEGIN(gc);

    gc.strFullName              = (STRINGREF)               strFullNameUNSAFE;
    gc.refedModule              = (REFLECTMODULEBASEREF)    refedModuleUNSAFE;
    gc.strRefedModuleFileName   = (STRINGREF)               strRefedModuleFileNameUNSAFE;

    if (!pThisModule->IsReflection())
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");

    pRCW = ((ReflectionModule*) pThisModule)->GetClassWriter();
    _ASSERTE(pRCW);

    pEmit = pRCW->GetEmitter(); 
    pImport = pRCW->GetRWImporter();

    if (gc.strFullName == NULL) {
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_String");
    }    

    _ASSERTE(gc.refedModule);

    pRefedModule = (Module*) gc.refedModule->GetData();
    _ASSERTE(pRefedModule);

    pThisAssembly = pThisModule->GetClassLoader()->GetAssembly();
    pRefedAssembly = pRefedModule->GetClassLoader()->GetAssembly();
    if (pThisModule == pRefedModule)
    {
        // referenced type is from the same module so we must be able to find a TypeDef.
        hr = pImport->FindTypeDefByName(
            gc.strFullName->GetBuffer(),
            RidFromToken(tkResolutionArg) ? tkResolutionArg : mdTypeDefNil,
            &tr); 

        _ASSERTE(SUCCEEDED(hr));
        goto ErrExit;
    }

    if (RidFromToken(tkResolutionArg))
    {
        // reference to nested type
        tkResolution = tkResolutionArg;
    }
    else
    {
        // reference to top level type
        if ( pThisAssembly != pRefedAssembly )
        {
            // Generate AssemblyRef
            IfFailGo( pEmit->QueryInterface(IID_IMetaDataAssemblyEmit, (void **) &pAssemblyEmit) );
            tkResolution = pThisAssembly->AddAssemblyRef(pRefedAssembly, pAssemblyEmit);

            // Add the assembly ref token and the manifest module it is referring to this module's rid map.
            // Don't cache the assembly if it's Save-only
            if( pRefedAssembly->HasRunAccess())
            {
                pThisModule->ForceStoreAssemblyRef(tkResolution, pRefedAssembly);
            }
        }
        else
        {
            _ASSERTE(pThisModule != pRefedModule);
            // Generate ModuleRef
            if (gc.strRefedModuleFileName != NULL)
            {
                IfFailGo(pEmit->DefineModuleRef(gc.strRefedModuleFileName->GetBuffer(), &tkResolution));
            }
            else
            {
                _ASSERTE(!"E_NYI!");
                COMPlusThrow(kInvalidOperationException, L"InvalidOperation_MetaDataError");    
            }
        }
    }

    IfFailGo( pEmit->DefineTypeRefByName(tkResolution, gc.strFullName->GetBuffer(), &tr) );  
ErrExit:
    if (pAssemblyEmit)
        pAssemblyEmit->Release();
    if (FAILED(hr))
    {
        // failed in defining PInvokeMethod
        if (hr == E_OUTOFMEMORY)
            COMPlusThrowOM();
        else
            COMPlusThrowHR(hr);    
    }

    GCPROTECT_END();

    HELPER_METHOD_FRAME_END_POLL();

    return tr;
}
FCIMPLEND


/*=============================GetArrayMethodToken==============================
**Action:
**Returns:
**Arguments: REFLECTMODULEBASEREF refThis
**           U1ARRAYREF     sig
**           STRINGREF      methodName
**           int            tkTypeSpec
**Exceptions:
==============================================================================*/
FCIMPL6(INT32, COMModule::GetArrayMethodToken, ReflectModuleBaseObject* refThisUNSAFE, 
                                               INT32 tkTypeSpec, 
                                               StringObject* methodNameUNSAFE, 
                                               U1Array* signatureUNSAFE,
                                               INT32 sigLength,
                                               INT32 baseToken)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module* pModule = GET_MODULE(refThisUNSAFE);

    RefClassWriter* pRCW;   
    PCCOR_SIGNATURE pvSig;
    LPCWSTR         methName;
    mdMemberRef memberRefE = mdTokenNil; 
    HRESULT hr;

    STRINGREF   methodName  = (STRINGREF)   methodNameUNSAFE;
    U1ARRAYREF  signature   = (U1ARRAYREF)  signatureUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_2(methodName, signature);

    if (!methodName)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_String");
    if (!tkTypeSpec) 
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Type");

    if (!pModule->IsReflection())  
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");   

    pRCW = ((ReflectionModule*) pModule)->GetClassWriter(); 
    _ASSERTE(pRCW); 
    
    //Get the signature.  Because we generated it with a call to GetSignature, it's already in the current scope.
    pvSig = (PCCOR_SIGNATURE)signature->GetDataPtr();
    
    methName = methodName->GetBuffer();

    hr = pRCW->GetEmitter()->DefineMemberRef(tkTypeSpec, methName, pvSig, sigLength, &memberRefE); 
    if (FAILED(hr)) 
    {
        _ASSERTE(!"Failed on DefineMemberRef");
        COMPlusThrowHR(hr);
    }

    HELPER_METHOD_FRAME_END();

    return (INT32)memberRefE;
}
FCIMPLEND


//******************************************************************************
//
// GetMemberRefToken
// This function will return a MemberRef token given a MethodDef token and the module where the MethodDef/FieldDef is defined.
//
//******************************************************************************
FCIMPL4(INT32, COMModule::GetMemberRefToken, ReflectModuleBaseObject* refThisUNSAFE, ReflectModuleBaseObject* refedModuleUNSAFE, INT32 tr, INT32 token)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    mdMemberRef             memberRefE      = 0; 

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module*                 pModule         = GET_MODULE(refThisUNSAFE);
    REFLECTMODULEBASEREF    refedModule     = (REFLECTMODULEBASEREF) refedModuleUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(refedModule);

    HRESULT                 hr; 
    RefClassWriter*         pRCW;   
    WCHAR*                  szName; 
    ULONG                   nameSize;    
    ULONG                   actNameSize     = 0;    
    ULONG                   cbComSig;   
    PCCOR_SIGNATURE         pvComSig;
    CQuickBytes             qbNewSig; 
    ULONG                   cbNewSig;   
    LPCUTF8                 szNameTmp;
    Module*                 pRefedModule;
    CQuickBytes             qbName;
    Assembly*               pRefedAssembly;
    Assembly*               pRefingAssembly;
    IMetaDataAssemblyEmit*  pAssemblyEmit   = NULL;
    mdTypeRef               tref;

    if (!pModule->IsReflection())  
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");   

    pRCW = ((ReflectionModule*) pModule)->GetClassWriter(); 
    _ASSERTE( pRCW ); 

    pRefedModule = (Module *) refedModule->GetData();
    _ASSERTE( pRefedModule );
    
    if (TypeFromToken(token) == mdtMethodDef)
    {
        szNameTmp = pRefedModule->GetMDImport()->GetNameOfMethodDef(token);
        pvComSig = pRefedModule->GetMDImport()->GetSigOfMethodDef(
            token,
            &cbComSig);
    }
    else
    {
        szNameTmp = pRefedModule->GetMDImport()->GetNameOfFieldDef(token);
        pvComSig = pRefedModule->GetMDImport()->GetSigOfFieldDef(
            token,
            &cbComSig);
    }

    // translate the name to unicode string
    nameSize = (ULONG)strlen(szNameTmp);
    IfFailGo( qbName.ReSizeNoThrow((nameSize + 1) * sizeof(WCHAR)) );
    szName = (WCHAR *) qbName.Ptr();
    actNameSize = ::WszMultiByteToWideChar(CP_UTF8, 0, szNameTmp, -1, szName, nameSize + 1);

    // The unicode translation function cannot fail!!
    _ASSERTE(actNameSize);

    // Translate the method sig into this scope 
    //
    pRefedAssembly = pRefedModule->GetAssembly();
    pRefingAssembly = pModule->GetAssembly();
    IfFailGo( pRefingAssembly->GetManifestModule()->GetEmitter()->QueryInterface(IID_IMetaDataAssemblyEmit, (void **) &pAssemblyEmit) );

    IfFailGo( pRefedModule->GetMDImport()->TranslateSigWithScope(
        pRefedAssembly->GetManifestImport(), 
        NULL, 0,        // hash value
        pvComSig, 
        cbComSig, 
        pAssemblyEmit,  // Emit assembly scope.
        pRCW->GetEmitter(), 
        &qbNewSig, 
        &cbNewSig) );  

    if (TypeFromToken(tr) == mdtTypeDef)
    {
        // define a TypeRef using the TypeDef
        IfFailGo(DefineTypeRefHelper(pRCW->GetEmitter(), tr, &tref));
    }
    else 
        tref = tr;

    // Define the memberRef
    IfFailGo( pRCW->GetEmitter()->DefineMemberRef(tref, szName, (PCCOR_SIGNATURE) qbNewSig.Ptr(), cbNewSig, &memberRefE) ); 

ErrExit:
    if (pAssemblyEmit)
        pAssemblyEmit->Release();

    if (FAILED(hr))
    {
        _ASSERTE(!"GetMemberRefToken failed!"); 
        COMPlusThrowHR(hr);    
    }

    HELPER_METHOD_FRAME_END();

    // assign output parameter
    return (INT32)memberRefE;
}
FCIMPLEND


//******************************************************************************
//
// Return a TypeRef token given a TypeDef token from the same emit scope
//
//******************************************************************************
HRESULT COMModule::DefineTypeRefHelper(
    IMetaDataEmit       *pEmit,         // given emit scope
    mdTypeDef           td,             // given typedef in the emit scope
    mdTypeRef           *ptr)           // return typeref
{
    CONTRACT(HRESULT) {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY; 
        INJECT_FAULT(CONTRACT_RETURN(E_OUTOFMEMORY));

        PRECONDITION(CheckPointer(pEmit));
        PRECONDITION(CheckPointer(ptr));
    }
    CONTRACT_END;

    IMetaDataImport     *pImport = NULL;
    CQuickBytes qb;
    WCHAR* szTypeDef = (WCHAR*) qb.AllocNoThrow((MAX_CLASSNAME_LENGTH+1) * sizeof(WCHAR));
    if(szTypeDef == NULL)
        return E_OUTOFMEMORY;
    mdToken             rs;             // resolution scope
    DWORD               dwFlags;
    HRESULT             hr;

    IfFailGo( pEmit->QueryInterface(IID_IMetaDataImport, (void **)&pImport) );
    IfFailGo( pImport->GetTypeDefProps(td, szTypeDef, MAX_CLASSNAME_LENGTH, NULL, &dwFlags, NULL) );
    if ( IsTdNested(dwFlags) )
    {
        mdToken         tdNested;
        IfFailGo( pImport->GetNestedClassProps(td, &tdNested) );
        IfFailGo( DefineTypeRefHelper( pEmit, tdNested, &rs) );
    }
    else
        rs = TokenFromRid( 1, mdtModule );

    IfFailGo( pEmit->DefineTypeRefByName( rs, szTypeDef, ptr) );

ErrExit:
    if (pImport)
        pImport->Release();
    RETURN(hr);
}   // DefineTypeRefHelper


//******************************************************************************
//
// Return a MemberRef token given a RuntimeMethodInfo
//
//******************************************************************************
FCIMPL3(INT32, COMModule::GetMemberRefTokenOfMethodInfo, ReflectModuleBaseObject* refThisUNSAFE, INT32 tr, MethodDesc *pMeth)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    mdMemberRef             memberRefE      = 0; 

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module*                 pModule         = GET_MODULE(refThisUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    HRESULT         hr; 
    RefClassWriter  *pRCW;   
    WCHAR           *szName; 
    ULONG           nameSize;    
    ULONG           actNameSize = 0;    
    ULONG           cbComSig;   
    PCCOR_SIGNATURE pvComSig;
    CQuickBytes     qbNewSig; 
    ULONG           cbNewSig;   
    LPCUTF8         szNameTmp;
    CQuickBytes     qbName;
    Assembly        *pRefedAssembly;
    Assembly        *pRefingAssembly;
    IMetaDataAssemblyEmit *pAssemblyEmit = NULL;

    if (!pMeth)  
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Obj");

    if (!pModule->IsReflection())  
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");   

    pRCW = ((ReflectionModule*) pModule)->GetClassWriter(); 
    _ASSERTE(pRCW); 

    // Otherwise, we want to return memberref token.
    if (pMeth->IsArray())
    {    
        _ASSERTE(!"Should not have come here!");
        COMPlusThrow(kNotSupportedException);    
    }
    if (pMeth->GetMethodTable())
    {
        if (pMeth->GetMethodTable()->GetModule() == pModule)
        {
            // If the passed in method is defined in the same module, just return the MethodDef token           
            memberRefE = pMeth->GetMemberDef();
            goto lExit;
        }
    }

    szNameTmp = pMeth->GetMDImport()->GetNameOfMethodDef(pMeth->GetMemberDef());
    pvComSig = pMeth->GetMDImport()->GetSigOfMethodDef(
        pMeth->GetMemberDef(),
        &cbComSig);

    // Translate the method sig into this scope 
    pRefedAssembly = pMeth->GetModule()->GetAssembly();
    pRefingAssembly = pModule->GetAssembly();
    IfFailGo( pRefingAssembly->GetManifestModule()->GetEmitter()->QueryInterface(IID_IMetaDataAssemblyEmit, (void **) &pAssemblyEmit) );

    IfFailGo( pMeth->GetMDImport()->TranslateSigWithScope(
        pRefedAssembly->GetManifestImport(), 
        NULL, 0,        // hash blob value
        pvComSig, 
        cbComSig, 
        pAssemblyEmit,  // Emit assembly scope.
        pRCW->GetEmitter(), 
        &qbNewSig, 
        &cbNewSig) );  

    // translate the name to unicode string
    nameSize = (ULONG)strlen(szNameTmp);
    IfFailGo( qbName.ReSizeNoThrow((nameSize + 1) * sizeof(WCHAR)) );
    szName = (WCHAR *) qbName.Ptr();
    actNameSize = ::WszMultiByteToWideChar(CP_UTF8, 0, szNameTmp, -1, szName, nameSize + 1);

    // The unicode translation function cannot fail!!
    _ASSERTE(actNameSize);

    // Define the memberRef
    IfFailGo( pRCW->GetEmitter()->DefineMemberRef(tr, szName, (PCCOR_SIGNATURE) qbNewSig.Ptr(), cbNewSig, &memberRefE) ); 

ErrExit:
    if (pAssemblyEmit)
        pAssemblyEmit->Release();

    if (FAILED(hr))
    {
        _ASSERTE(hr == E_OUTOFMEMORY || !"GetMemberRefTokenOfMethodInfo Failed!"); 
        COMPlusThrowHR(hr);    
    }

lExit: ;
    HELPER_METHOD_FRAME_END();

    // assign output parameter
    return memberRefE;
}
FCIMPLEND


//******************************************************************************
//
// Return a MemberRef token given a RuntimeFieldInfo
//
//******************************************************************************
FCIMPL4(mdMemberRef, COMModule::GetMemberRefTokenOfFieldInfo, ReflectModuleBaseObject* refThisUNSAFE, mdTypeDef tr, void * th, mdFieldDef tkField)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    TypeHandle      typeHandle = TypeHandle::FromPtr(th);
    mdMemberRef     memberRefE  = 0; 

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module*         pModule     = GET_MODULE(refThisUNSAFE);


    HELPER_METHOD_FRAME_BEGIN_RET_0();

    HRESULT         hr; 
    WCHAR           *szName; 
    ULONG           nameSize;    
    RefClassWriter* pRCW;   
    ULONG           actNameSize = 0;    
    ULONG           cbComSig;   
    PCCOR_SIGNATURE pvComSig;
    LPCUTF8         szNameTmp;
    CQuickBytes     qbNewSig;
    ULONG           cbNewSig;   
    CQuickBytes     qbName;
    Assembly        *pRefedAssembly;
    Assembly        *pRefingAssembly;
    IMetaDataAssemblyEmit *pAssemblyEmit = NULL;

    if (!pModule->IsReflection())  
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");   

    pRCW = ((ReflectionModule*) pModule)->GetClassWriter(); 
    _ASSERTE(pRCW);

    if (TypeFromToken(tr) == mdtTypeDef)
    {
        // If the passed in method is defined in the same module, just return the FieldDef token           
        memberRefE = tkField;
        goto lExit;
    }

    // get the field name and sig
    szNameTmp = typeHandle.GetModule()->GetMDImport()->GetNameOfFieldDef(tkField);
    pvComSig = typeHandle.GetModule()->GetMDImport()->GetSigOfFieldDef(tkField, &cbComSig);

    // translate the name to unicode string
    nameSize = (ULONG)strlen(szNameTmp);
    IfFailGo( qbName.ReSizeNoThrow((nameSize + 1) * sizeof(WCHAR)) );
    szName = (WCHAR *) qbName.Ptr();
    actNameSize = ::WszMultiByteToWideChar(CP_UTF8, 0, szNameTmp, -1, szName, nameSize + 1);
    
    // The unicode translation function cannot fail!!
    _ASSERTE(actNameSize);

    pRefedAssembly = typeHandle.GetModule()->GetAssembly();
    pRefingAssembly = pModule->GetAssembly();
    IfFailGo( pRefingAssembly->GetManifestModule()->GetEmitter()->QueryInterface(IID_IMetaDataAssemblyEmit, (void **) &pAssemblyEmit) );

    // Translate the field signature this scope  
    IfFailGo( typeHandle.GetModule()->GetMDImport()->TranslateSigWithScope(
        pRefedAssembly->GetManifestImport(), 
        NULL, 0,            // hash value
        pvComSig, 
        cbComSig, 
        pAssemblyEmit,      // Emit assembly scope.
        pRCW->GetEmitter(), 
        &qbNewSig, 
        &cbNewSig) );  

    IfFailGo( pRCW->GetEmitter()->DefineMemberRef(tr, szName, (PCCOR_SIGNATURE) qbNewSig.Ptr(), cbNewSig, &memberRefE) ); 

ErrExit:
    if (pAssemblyEmit)
        pAssemblyEmit->Release();

    if (FAILED(hr))
    {
        _ASSERTE(!"GetMemberRefTokenOfFieldInfo Failed on Field"); 
        COMPlusThrowHR(hr);    
    }

lExit: ;
    HELPER_METHOD_FRAME_END();

    return memberRefE;  
}
FCIMPLEND

//******************************************************************************
//
// Return a MemberRef token given a Signature
//
//******************************************************************************
FCIMPL5(INT32, COMModule::GetMemberRefTokenFromSignature, ReflectModuleBaseObject* refThisUNSAFE, 
                                                      INT32 tr,
                                                      StringObject* strMemberNameUNSAFE,
                                                      U1Array* signatureUNSAFE,
                                                      INT32 sigLength)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    HRESULT         hr; 
    RefClassWriter* pRCW;   
    mdMemberRef     memberRefE = mdTokenNil; 

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module* pModule = GET_MODULE(refThisUNSAFE);

    STRINGREF               strMemberName   = (STRINGREF)   strMemberNameUNSAFE;
    U1ARRAYREF              signature       = (U1ARRAYREF)  signatureUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_2(strMemberName, signature);

    if (!pModule->IsReflection())  
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");   

    pRCW = ((ReflectionModule*) pModule)->GetClassWriter(); 
    _ASSERTE(pRCW);

    IfFailGo( pRCW->GetEmitter()->DefineMemberRef(tr, 
                                                  strMemberName->GetBuffer(), 
                                                  (PCCOR_SIGNATURE) signature->GetDataPtr(), 
                                                  sigLength, 
                                                  &memberRefE) ); 

ErrExit:
    if (FAILED(hr))
        COMPlusThrowHR(hr);    
    HELPER_METHOD_FRAME_END();
    return memberRefE;  
}
FCIMPLEND

//******************************************************************************
//
// SetFieldRVAContent
// This function is used to set the FieldRVA with the content data
//
//******************************************************************************
FCIMPL4(void, COMModule::SetFieldRVAContent, ReflectModuleBaseObject* refThisUNSAFE, INT32 tkField, U1Array* contentUNSAFE, INT32 length)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    RefClassWriter      *pRCW;   
    ICeeGen             *pGen;
    HRESULT             hr;
    DWORD               dwRVA;
    void                *pvBlob;

    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    Module* pModule = GET_MODULE(refThisUNSAFE);

    U1ARRAYREF  content = (U1ARRAYREF) contentUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(content);

    if (!pModule->IsReflection())  
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");   

    pRCW = ((ReflectionModule*) pModule)->GetClassWriter(); 
    _ASSERTE(pRCW); 

    pGen = pRCW->GetCeeGen();

    // Create the .sdata section if not created
    if (((ReflectionModule*) pModule)->m_sdataSection == 0)
        IfFailGo( pGen->GetSectionCreate (".sdata", sdReadWrite, &((ReflectionModule*) pModule)->m_sdataSection) );

    // Get the size of current .sdata section. This will be the RVA for this field within the section
    IfFailGo( pGen->GetSectionDataLen(((ReflectionModule*) pModule)->m_sdataSection, &dwRVA) );
    dwRVA = (dwRVA + sizeof(DWORD)-1) & ~(sizeof(DWORD)-1);         

    // allocate the space in .sdata section
    IfFailGo( pGen->GetSectionBlock(((ReflectionModule*) pModule)->m_sdataSection, length, sizeof(DWORD), (void**) &pvBlob) );

    // copy over the initialized data if specified
    if (content != NULL)
        memcpy(pvBlob, content->GetDataPtr(), length);

    // set FieldRVA into metadata. Note that this is not final RVA in the image if save to disk. We will do another round of fix up upon save.
    IfFailGo( pRCW->GetEmitter()->SetFieldRVA(tkField, dwRVA) );

ErrExit:
    if (FAILED(hr))
    {
        // failed in Setting ResolutionScope
        COMPlusThrowHR(hr);
    }
   
    HELPER_METHOD_FRAME_END();
   
}
FCIMPLEND


//******************************************************************************
//
// GetStringConstant
// If this is a dynamic module, this routine will define a new 
//  string constant or return the token of an existing constant.    
//
//******************************************************************************
FCIMPL2(mdString, COMModule::GetStringConstant, ReflectModuleBaseObject* refThisUNSAFE, StringObject* strValueUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    RefClassWriter* pRCW;   
    mdString strRef = mdTokenNil;   
    HRESULT hr;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    // Verify that the module is a dynamic module...    
    Module* pModule = GET_MODULE(refThisUNSAFE);

    STRINGREF strValue = (STRINGREF) strValueUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(strValue);

    if (!pModule->IsReflection())  
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");   

    // If they didn't pass a String throw...    
    if (!strValue)    
        COMPlusThrow(kArgumentNullException,L"ArgumentNull_String");

    pRCW = ((ReflectionModule*) pModule)->GetClassWriter(); 
    _ASSERTE(pRCW); 
    
    hr = pRCW->GetEmitter()->DefineUserString(strValue->GetBuffer(), 
            strValue->GetStringLength(), &strRef);
    if (FAILED(hr)) {   
        _ASSERTE(hr == E_OUTOFMEMORY || !"Unknown failure in DefineUserString");    
        COMPlusThrowHR(hr);    
    }   

    HELPER_METHOD_FRAME_END();

    return strRef;  
}
FCIMPLEND


/*=============================SetModuleProps==============================
// SetModuleProps
==============================================================================*/
FCIMPL2(void, COMModule::SetModuleProps, ReflectModuleBaseObject* refThisUNSAFE, StringObject* strModuleNameUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    RefClassWriter      *pRCW;
    HRESULT             hr;
    IMetaDataEmit       *pEmit;

    if (refThisUNSAFE == NULL)
        FCThrowResVoid(kNullReferenceException, L"NullReference_This");

    Module*     pModule         = GET_MODULE(refThisUNSAFE);
    STRINGREF   strModuleName   = (STRINGREF)strModuleNameUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(strModuleName);

    if (!pModule->IsReflection())
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");

    pRCW = ((ReflectionModule*) pModule)->GetClassWriter();
    _ASSERTE(pRCW);

    pEmit = pRCW->GetEmitter(); 

    IfFailGo( pEmit->SetModuleProps(strModuleName->GetBuffer()) );

ErrExit:
    if (FAILED(hr))
    {
        // failed in Setting ResolutionScope
        COMPlusThrowHR(hr);    
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

//******************************************************************************
//
// Return a type spec token given a byte array
//
//******************************************************************************
FCIMPL3(mdTypeSpec, COMModule::GetTypeSpecTokenWithBytes, ReflectModuleBaseObject* refThisUNSAFE, U1Array* signatureUNSAFE, INT32 sigLength)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    mdTypeSpec      ts = mdTokenNil;
    RefClassWriter  *pRCW; 
    HRESULT         hr = NOERROR;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module* pModule = GET_MODULE(refThisUNSAFE);

    U1ARRAYREF  signature   = (U1ARRAYREF) signatureUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(signature);

    if (!pModule->IsReflection())  
        COMPlusThrow(kNotSupportedException, L"NotSupported_NonReflectedType");   

    pRCW = ((ReflectionModule*) pModule)->GetClassWriter(); 
    _ASSERTE(pRCW); 

    hr = pRCW->GetEmitter()->GetTokenFromTypeSpec((PCCOR_SIGNATURE)signature->GetDataPtr(), sigLength, &ts);  
    _ASSERTE(SUCCEEDED(hr));

    HELPER_METHOD_FRAME_END();
    return ts;

}
FCIMPLEND


// GetClass
// Given a class name, this method will look for that class
//  with in the module. 
FCIMPL4(Object*, COMModule::GetClass, ReflectModuleBaseObject* refThisUNSAFE, StringObject* refClassNameUNSAFE, CLR_BOOL bThrowOnError, CLR_BOOL bIgnoreCase)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module*     pModule         = GET_MODULE(refThisUNSAFE);
    STRINGREF name = (STRINGREF) refClassNameUNSAFE;
    OBJECTREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, name);
    
    DomainAssembly *pAssembly = pModule->GetDomainAssembly();
    _ASSERTE(pAssembly);
    refRetVal = (OBJECTREF)AssemblyNative::GetTypeInner(pAssembly, &name, bThrowOnError, bIgnoreCase);

    // Verify that it's in 'this' module
    // But, if it's in a different assembly than expected, that's okay, because
    // it just means that it's been type forwarded.
    if (refRetVal != NULL) {
        TypeHandle th = ((REFLECTCLASSBASEREF)refRetVal)->GetType();
        if ( (th.GetModule() != pModule) &&
             (th.GetModule()->GetAssembly() == pModule->GetAssembly()) )
            refRetVal = NULL;
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


// GetName
// This routine will return the name of the module as a String
FCIMPL1(Object*, COMModule::GetName, ReflectModuleBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    STRINGREF modName = NULL;
    LPCSTR    szName = NULL;

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module* pModule = GET_MODULE(refThisUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, modName);

    if (pModule->IsResource())
        pModule->GetAssembly()->GetManifestImport()->GetFileProps(pModule->GetModuleRef(),
                                                                  &szName,
                                                                  NULL,
                                                                  NULL,
                                                                  NULL);
    else {
        if (pModule->GetMDImport()->IsValidToken(pModule->GetMDImport()->GetModuleFromScope()))
            pModule->GetMDImport()->GetScopeProps(&szName,0);
        else
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
    }

    modName = COMString::NewString(szName);

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(modName);
}
FCIMPLEND


/*============================GetFullyQualifiedName=============================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FCIMPL1(Object*, COMModule::GetFullyQualifiedName,  ReflectModuleBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    STRINGREF name=NULL;
    HRESULT hr = S_OK;

    WCHAR wszBuffer[64];

    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module* pModule = GET_MODULE(refThisUNSAFE);
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, name);
    
    if (pModule->IsPEFile()) {
        LPCWSTR fileName = pModule->GetPath();
        if (*fileName != 0) {
            if (pModule->IsSystem())
                name = COMString::NewString(SystemDomain::System()->BaseLibrary());
            else
                name = COMString::NewString(fileName);
        } else {
            hr = UtilLoadStringRC(IDS_EE_NAME_UNKNOWN, wszBuffer, sizeof( wszBuffer ) / sizeof( WCHAR ), true );
            if (SUCCEEDED(hr))
                name = COMString::NewString(wszBuffer);
            else
                COMPlusThrowHR(hr);
        }
    } else {
        hr = UtilLoadStringRC(IDS_EE_NAME_INMEMORYMODULE, wszBuffer, sizeof( wszBuffer ) / sizeof( WCHAR ), true );
        if (SUCCEEDED(hr))
            name = COMString::NewString(wszBuffer);
        else
            COMPlusThrowHR(hr);
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(name);
}
FCIMPLEND

/*===================================GetHINST===================================
**Action:  Returns the hinst for this module.
**Returns:
**Arguments: refThis
**Exceptions: None.
==============================================================================*/
FCIMPL1(HINSTANCE, COMModule::GetHINST, ReflectModuleBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    HMODULE hMod = (HMODULE)0;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module* pModule = GET_MODULE(refThisUNSAFE);

    // This returns the base address - this will work for either HMODULE or HCORMODULES
    // Other modules should have zero base
    PEFile *pPEFile = pModule->GetFile();
    if (!pPEFile->IsDynamic() && !pPEFile->IsResource())
    {
        HELPER_METHOD_FRAME_BEGIN_RET_0();

        hMod = (HMODULE) pModule->GetFile()->GetManagedFileContents();

        HELPER_METHOD_FRAME_END();
    }

    //If we don't have an hMod, set it to -1 so that they know that there's none
    //available
    if (!hMod) {
        hMod = (HMODULE)-1;
    }
    return (HINSTANCE)hMod;
}
FCIMPLEND

static Object* GetClassesInner(Module* pModule, StackCrawlMark* stackMark);

// Get class will return an array contain all of the classes
//  that are defined within this Module.    
FCIMPL2(Object*, COMModule::GetClasses, ReflectModuleBaseObject* refThisUNSAFE, StackCrawlMark* stackMark)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module*     pModule     = GET_MODULE(refThisUNSAFE);
    OBJECTREF   refRetVal   = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    refRetVal = (OBJECTREF) GetClassesInner(pModule, stackMark);

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

Object* GetClassesInner(Module* pModule, StackCrawlMark* stackMark)
{
    CONTRACT(Object*) {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; 
        INJECT_FAULT(COMPlusThrowOM());

        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(stackMark));

        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    HRESULT         hr;
    DWORD           dwNumTypeDefs = 0;
    DWORD           i;
    mdTypeDef*      rgTypeDefs;
    IMDInternalImport *pInternalImport;
    PTRARRAYREF     refArrClasses = NULL;
    PTRARRAYREF     xcept = NULL;
    DWORD           cXcept;
    HENUMInternal   hEnum;
    bool            bSystemAssembly;    // Don't expose transparent proxy
    bool            bCheckedAccess = false;
    bool            bAllowedAccess = false;
    int             AllocSize = 0;
    MethodTable* pMT = NULL;

    if (pModule->IsResource())
    {
        refArrClasses = (PTRARRAYREF) AllocateObjectArray(0, g_Mscorlib.FetchClass(CLASS__TYPE));
        RETURN(OBJECTREFToObject(refArrClasses));
    }

    GCPROTECT_BEGIN(refArrClasses);
    GCPROTECT_BEGIN(xcept);

    pInternalImport = pModule->GetMDImport();

    // Get the count of typedefs
    hr = pInternalImport->EnumTypeDefInit(&hEnum);

    if(FAILED(hr)) {
        _ASSERTE(!"GetCountTypeDefs failed.");
        COMPlusThrowHR(hr);    
    }
    dwNumTypeDefs = pInternalImport->EnumTypeDefGetCount(&hEnum);

    // Allocate an array for all the typedefs
    rgTypeDefs = (mdTypeDef*) _alloca(sizeof(mdTypeDef) * dwNumTypeDefs);

    // Get the typedefs
    for (i=0; pInternalImport->EnumTypeDefNext(&hEnum, &rgTypeDefs[i]); i++);

    pInternalImport->EnumTypeDefClose(&hEnum);

    // Account for types we skipped.
    dwNumTypeDefs = i;

    // Allocate the COM+ array
    bSystemAssembly = (pModule->GetAssembly() == SystemDomain::SystemAssembly());
    AllocSize = (!bSystemAssembly || (bCheckedAccess && !bAllowedAccess)) ? dwNumTypeDefs : dwNumTypeDefs - 1;
    refArrClasses = (PTRARRAYREF) AllocateObjectArray(AllocSize, g_Mscorlib.FetchClass(CLASS__TYPE));

    // Allocate an array to store the references in
    xcept = (PTRARRAYREF) AllocateObjectArray(dwNumTypeDefs,g_pExceptionClass);
    cXcept = 0;
    
    OBJECTREF throwable = 0;
    GCPROTECT_BEGIN(throwable);
    // Now create each COM+ Method object and insert it into the array.
    int curPos = 0;
    for(i = 0; i < dwNumTypeDefs; i++)
    {
        // Get the VM class for the current class token
        TypeHandle curClass;
        ClassLoader::LoadTypeDefOrRefNoThrow(pModule, rgTypeDefs[i], &curClass, &throwable, 
                                             ClassLoader::ThrowIfNotFound, 
                                             ClassLoader::PermitUninstDefOrRef);
        if (bSystemAssembly) {
            if (!curClass.IsNull() && curClass.GetMethodTable()->IsTransparentProxyType())
                continue;
        }
        if (throwable != 0) {
            refArrClasses->ClearAt(i);
            xcept->SetAt(cXcept++, throwable);
            throwable = 0;
        }
        else {
            _ASSERTE("LoadClass failed." && !curClass.IsNull());

            pMT = curClass.GetMethodTable();
            PREFIX_ASSUME(pMT != NULL);
            // Get the COM+ Class object
            OBJECTREF refCurClass = pMT->GetManagedClassObject();
            _ASSERTE("GetManagedClassObject failed." && refCurClass != NULL);

            refArrClasses->SetAt(curPos++, refCurClass);
        }
    }
    GCPROTECT_END();    //throwable

    // check if there were exceptions thrown
    if (cXcept > 0) {
        PTRARRAYREF xceptRet = NULL;
        GCPROTECT_BEGIN(xceptRet);
        
        xceptRet = (PTRARRAYREF) AllocateObjectArray(cXcept,g_pExceptionClass);
        for (i=0;i<cXcept;i++) {
            xceptRet->SetAt(i, xcept->GetAt(i));
        }
        OBJECTREF except = InvokeUtil::CreateClassLoadExcept((OBJECTREF*) &refArrClasses,(OBJECTREF*) &xceptRet);
        COMPlusThrow(except);
        
        GCPROTECT_END();
    }

    // Assign the return value to the COM+ array
    GCPROTECT_END();
    GCPROTECT_END();

    RETURN(OBJECTREFToObject(refArrClasses));
}


FCIMPL1(Object*, COMModule::GetAssembly,  ReflectModuleBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;       
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module*         pModule     = GET_MODULE(refThisUNSAFE);
    ASSEMBLYREF     result      = NULL;
        

    Assembly *pAssembly = pModule->GetAssembly();
    _ASSERTE(pAssembly);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, result);   
    result = (ASSEMBLYREF) pAssembly->GetExposedObject();
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(result);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, COMModule::IsResource, ReflectModuleBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    if (refThisUNSAFE == NULL)
        FCThrowRes(kNullReferenceException, L"NullReference_This");

    Module* pModule = GET_MODULE(refThisUNSAFE);
    FC_RETURN_BOOL(pModule->IsResource());
}
FCIMPLEND

FCIMPL1(INT32, COMModule::GetSigTypeFromClassWrapper, ReflectClassBaseObject* refTypeUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CorElementType sigType = ELEMENT_TYPE_END;
    REFLECTCLASSBASEREF refType = (REFLECTCLASSBASEREF)refTypeUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(refType)
    
    // Find out if this type is a primitive or a class object
    sigType = refType->GetType().GetSignatureCorElementType();
    if (sigType == ELEMENT_TYPE_CLASS)
    {            
        if (g_Mscorlib.IsClass(refType->GetType().AsMethodTable(), CLASS__OBJECT))
            sigType = ELEMENT_TYPE_OBJECT;
        else if (g_Mscorlib.IsClass(refType->GetType().AsMethodTable(), CLASS__STRING))
            sigType = ELEMENT_TYPE_STRING;
    }
    
    HELPER_METHOD_FRAME_END();
    
    return sigType;
}
FCIMPLEND
    
