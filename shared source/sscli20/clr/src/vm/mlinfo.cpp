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
#include "ml.h"
#include "marshaler.h"
#include "mlinfo.h"
#include "dllimport.h"
#include "sigformat.h"
#include "eeconfig.h"
#include "eehash.h"
#include "../dlls/mscorrc/resource.h"
#include "mdaassistantsptr.h"
#include "ndpversion.h"
#include "typeparse.h"





#ifndef lengthof
    #define lengthof(rg)    (sizeof(rg)/sizeof(rg[0]))
#endif





#define INITIAL_NUM_CMHELPER_HASHTABLE_BUCKETS 32
#define INITIAL_NUM_CMINFO_HASHTABLE_BUCKETS 32
#define DEBUG_CONTEXT_STR_LEN 2000


BOOL CheckForPrimitiveType(CorElementType elemType, CQuickArray<WCHAR> *pStrPrimitiveType);

//-------------------------------------------------------------------------------------
// Return the copy ctor for a VC class (if any exists)
//-------------------------------------------------------------------------------------
void FindCopyCtor(Module *pModule, MethodTable *pMT, MethodDesc **pMDOut)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;    // CompareTypeTokens may trigger GC
        MODE_ANY;
    }
    CONTRACTL_END;
    
    *pMDOut = NULL;

    mdMethodDef tk;
    mdTypeDef cl = pMT->GetCl();
    TypeHandle th = TypeHandle(pMT);
    SigTypeContext typeContext(th); 

    IMDInternalImport *pInternalImport = pModule->GetMDImport();
    MDEnumHolder      hEnumMethod(pInternalImport);

    //
    // First try for the new syntax: <MarshalCopy>
    //
    HRESULT hr = pInternalImport->EnumInit(mdtMethodDef, cl, &hEnumMethod);
    if (FAILED(hr))
        COMPlusThrowHR(hr);

    while (pInternalImport->EnumNext(&hEnumMethod, &tk))
    {
        _ASSERTE(TypeFromToken(tk) == mdtMethodDef);
        DWORD dwMemberAttrs = pInternalImport->GetMethodDefProps(tk);
        
        if (IsMdSpecialName(dwMemberAttrs))
        {
            ULONG cSig;
            PCCOR_SIGNATURE pSig;
            LPCSTR pName = pInternalImport->GetNameAndSigOfMethodDef(tk, &pSig, &cSig);     
            const char *pBaseName = "<MarshalCopy>";
            int ncBaseName = (int)strlen(pBaseName);
            int nc = (int)strlen(pName);
            if (nc >= ncBaseName && 0 == strcmp(pName + nc - ncBaseName, pBaseName))
            {
                MetaSig msig(pSig, cSig, pModule, &typeContext);
                
                // Looking for the prototype   void <MarshalCopy>(Ptr VC, Ptr VC);
                if (msig.NumFixedArgs() == 2)
                {
                    if (msig.GetReturnType() == ELEMENT_TYPE_VOID)
                    {
                        if (msig.NextArg() == ELEMENT_TYPE_PTR)
                        {
                            SigPointer sp1 = msig.GetArgProps();
                            IfFailThrow(sp1.GetElemType(NULL));
                            CorElementType eType;
                            IfFailThrow(sp1.GetElemType(&eType));
                            if (eType == ELEMENT_TYPE_VALUETYPE)
                            {
                                mdToken tk1;
                                IfFailThrow(sp1.GetToken(&tk1));
                                hr = CompareTypeTokensNT(tk1, cl, pModule, pModule);
                                if (FAILED(hr))
                                {
                                    pInternalImport->EnumClose(&hEnumMethod);
                                    COMPlusThrowHR(hr);
                                }

                                if (hr == S_OK)
                                {
                                    if (msig.NextArg() == ELEMENT_TYPE_PTR)
                                    {
                                        SigPointer sp2 = msig.GetArgProps();
                                        IfFailThrow(sp2.GetElemType(NULL));
                                        IfFailThrow(sp2.GetElemType(&eType));
                                        if (eType == ELEMENT_TYPE_VALUETYPE)
                                        {
                                            mdToken tk2;
                                            IfFailThrow(sp2.GetToken(&tk2));
                                            
                                            hr = (tk2 == tk1) ? S_OK : CompareTypeTokensNT(tk2, cl, pModule, pModule);
                                            if (hr == S_OK)
                                            {
                                                *pMDOut = pModule->LookupMethodDef(tk);
                                                return;                                 
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }        
    }

    //
    // Next try the old syntax: global .__ctor
    //
    hr = pInternalImport->EnumGlobalFunctionsInit( &hEnumMethod );
    if (FAILED(hr))
        COMPlusThrowHR(hr);   

    while (pInternalImport->EnumNext(&hEnumMethod, &tk))
    {
        _ASSERTE(TypeFromToken(tk) == mdtMethodDef);
        DWORD dwMemberAttrs = pInternalImport->GetMethodDefProps(tk);
        
        if (IsMdSpecialName(dwMemberAttrs))
        {
            ULONG cSig;
            PCCOR_SIGNATURE pSig;
            LPCSTR pName = pInternalImport->GetNameAndSigOfMethodDef(tk, &pSig, &cSig);     
            const char *pBaseName = ".__ctor";
            int ncBaseName = (int)strlen(pBaseName);
            int nc = (int)strlen(pName);
            if (nc >= ncBaseName && 0 == strcmp(pName + nc - ncBaseName, pBaseName))
            {

                MetaSig msig(pSig, cSig, pModule, &typeContext);
                
                // Looking for the prototype   Ptr VC __ctor(Ptr VC, ByRef VC);
                if (msig.NumFixedArgs() == 2)
                {
                    if (msig.GetReturnType() == ELEMENT_TYPE_PTR)
                    {
                        SigPointer spret = msig.GetReturnProps();
                        IfFailThrow(spret.GetElemType(NULL));
                        CorElementType eType;
                        IfFailThrow(spret.GetElemType(&eType));
                        if (eType == ELEMENT_TYPE_VALUETYPE)
                        {
                            mdToken tk0;
                            IfFailThrow(spret.GetToken(&tk0));
                            hr = CompareTypeTokensNT(tk0, cl, pModule, pModule);
                            if (FAILED(hr))
                            {
                                pInternalImport->EnumClose(&hEnumMethod);
                                COMPlusThrowHR(hr);
                            }
                            
                            if (hr == S_OK)
                            {
                                if (msig.NextArg() == ELEMENT_TYPE_PTR)
                                {
                                    SigPointer sp1 = msig.GetArgProps();
                                    IfFailThrow(sp1.GetElemType(NULL));
                                    IfFailThrow(sp1.GetElemType(&eType));
                                    if (eType == ELEMENT_TYPE_VALUETYPE)
                                    {
                                        mdToken tk1;
                                        IfFailThrow(sp1.GetToken(&tk1));
                                        hr = (tk1 == tk0) ? S_OK : CompareTypeTokensNT(tk1, cl, pModule, pModule);
                                        if (FAILED(hr))
                                        {
                                            pInternalImport->EnumClose(&hEnumMethod);
                                            COMPlusThrowHR(hr);
                                        }

                                        if (hr == S_OK)
                                        {
                                            if (msig.NextArg() == ELEMENT_TYPE_PTR &&
                                                msig.GetArgProps().HasCustomModifier(pModule, "Microsoft.VisualC.IsCXXReferenceModifier", ELEMENT_TYPE_CMOD_OPT))
                                            {
                                                SigPointer sp2 = msig.GetArgProps();
                                                IfFailThrow(sp2.GetElemType(NULL));
                                                IfFailThrow(sp2.GetElemType(&eType));
                                                if (eType == ELEMENT_TYPE_VALUETYPE)
                                                {
                                                    mdToken tk2;
                                                    IfFailThrow(sp2.GetToken(&tk2));
                                                    
                                                    hr = (tk2 == tk0) ? S_OK : CompareTypeTokensNT(tk2, cl, pModule, pModule);
                                                    if (hr == S_OK)
                                                    {
                                                        *pMDOut = pModule->LookupMethodDef(tk);
                                                        return;                                 
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


//-------------------------------------------------------------------------------------
// Return the destructor for a VC class (if any exists)
//-------------------------------------------------------------------------------------
void FindDtor(Module *pModule, MethodTable *pMT, MethodDesc **pMDOut)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;    // CompareTypeTokens may trigger GC
        MODE_ANY;
    }
    CONTRACTL_END;

    
    *pMDOut = NULL;

    mdMethodDef tk;
    mdTypeDef cl = pMT->GetCl();
    TypeHandle th = TypeHandle(pMT);
    SigTypeContext typeContext(th);

    IMDInternalImport *pInternalImport = pModule->GetMDImport();
    MDEnumHolder       hEnumMethod(pInternalImport);
   
    //
    // First try for the new syntax: <MarshalDestroy>
    //
    HRESULT hr = pInternalImport->EnumInit(mdtMethodDef, cl, &hEnumMethod);
    if (FAILED(hr))
        COMPlusThrowHR(hr);

    while (pInternalImport->EnumNext(&hEnumMethod, &tk))
    {
        _ASSERTE(TypeFromToken(tk) == mdtMethodDef);
        DWORD dwMemberAttrs = pInternalImport->GetMethodDefProps(tk);
        
        if (IsMdSpecialName(dwMemberAttrs))
        {
            ULONG cSig;
            PCCOR_SIGNATURE pSig;
            LPCSTR pName = pInternalImport->GetNameAndSigOfMethodDef(tk, &pSig, &cSig);     
            const char *pBaseName = "<MarshalDestroy>";
            int ncBaseName = (int)strlen(pBaseName);
            int nc = (int)strlen(pName);
            if (nc >= ncBaseName && 0 == strcmp(pName + nc - ncBaseName, pBaseName))
            {
                MetaSig msig(pSig, cSig, pModule, &typeContext);
                
                // Looking for the prototype   void <MarshalDestroy>(Ptr VC);
                if (msig.NumFixedArgs() == 1)
                {
                    if (msig.GetReturnType() == ELEMENT_TYPE_VOID)
                    {
                        if (msig.NextArg() == ELEMENT_TYPE_PTR)
                        {
                            SigPointer sp1 = msig.GetArgProps();
                            IfFailThrow(sp1.GetElemType(NULL));
                            CorElementType eType;
                            IfFailThrow(sp1.GetElemType(&eType));
                            if (eType == ELEMENT_TYPE_VALUETYPE)
                            {
                                mdToken tk1;
                                IfFailThrow(sp1.GetToken(&tk1));
                                
                                hr = CompareTypeTokensNT(tk1, cl, pModule, pModule);
                                if (FAILED(hr))
                                    COMPlusThrowHR(hr);
                                
                                if (hr == S_OK)
                                {
                                    *pMDOut = pModule->LookupMethodDef(tk);
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }        
    }


    //
    // Next try the old syntax: global .__dtor
    //
    hr = pInternalImport->EnumGlobalFunctionsInit( &hEnumMethod );
    if (FAILED(hr))
        COMPlusThrowHR(hr);
    

    while (pInternalImport->EnumNext(&hEnumMethod, &tk))
    {
        _ASSERTE(TypeFromToken(tk) == mdtMethodDef);
        ULONG cSig;
        PCCOR_SIGNATURE pSig;
        LPCSTR pName = pInternalImport->GetNameAndSigOfMethodDef(tk, &pSig, &cSig);     
        const char *pBaseName = ".__dtor";
        int ncBaseName = (int)strlen(pBaseName);
        int nc = (int)strlen(pName);
        if (nc >= ncBaseName && 0 == strcmp(pName + nc - ncBaseName, pBaseName))
        {
            MetaSig msig(pSig, cSig, pModule, &typeContext);
            
            // Looking for the prototype   void __dtor(Ptr VC);
            if (msig.NumFixedArgs() == 1)
            {
                if (msig.GetReturnType() == ELEMENT_TYPE_VOID)
                {
                    if (msig.NextArg() == ELEMENT_TYPE_PTR)
                    {
                        SigPointer sp1 = msig.GetArgProps();
                        IfFailThrow(sp1.GetElemType(NULL));
                        CorElementType eType;
                        IfFailThrow(sp1.GetElemType(&eType));
                        if (eType == ELEMENT_TYPE_VALUETYPE)
                        {
                            mdToken tk1;
                            IfFailThrow(sp1.GetToken(&tk1));
                            hr = CompareTypeTokensNT(tk1, cl, pModule, pModule);
                            if (FAILED(hr))
                            {
                                pInternalImport->EnumClose(&hEnumMethod);
                                COMPlusThrowHR(hr);
                            }
                            
                            if (hr == S_OK)
                            {
                                *pMDOut = pModule->LookupMethodDef(tk);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}

//==========================================================================
// Set's up the custom marshaler information.
//==========================================================================
CustomMarshalerHelper *SetupCustomMarshalerHelper(LPCUTF8 strMarshalerTypeName, DWORD cMarshalerTypeNameBytes, LPCUTF8 strCookie, DWORD cCookieStrBytes, Assembly *pAssembly, TypeHandle hndManagedType)
{
    CONTRACT (CustomMarshalerHelper*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pAssembly));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    EEMarshalingData *pMarshalingData = NULL;

    // Retrieve the marshalling data for the current app domain.
    if (pAssembly->IsDomainNeutral())
    {
        // If the assembly is shared, then it should only reference other domain neutral assemblies.
        // This assumption MUST be true for the current custom marshaling scheme to work.
        // This implies that the type of the managed parameter must be a shared type.
        _ASSERTE(hndManagedType.GetAssembly()->IsDomainNeutral());

        // The assembly is shared so we need to use the system domain's marshaling data.
        pMarshalingData = SystemDomain::System()->GetMarshalingData();
    }
    else
    {
        // The assembly is not shared so we use the current app domain's marshaling data.
        pMarshalingData = GetThread()->GetDomain()->GetMarshalingData();
    }

    // Retrieve the custom marshaler helper from the EE marshaling data.
    RETURN pMarshalingData->GetCustomMarshalerHelper(pAssembly, hndManagedType, strMarshalerTypeName, cMarshalerTypeNameBytes, strCookie, cCookieStrBytes);
}

//==========================================================================
// Return: S_OK if there is valid data to compress
//         S_FALSE if at end of data block
//         E_FAIL if corrupt data found
//==========================================================================
HRESULT CheckForCompressedData(PCCOR_SIGNATURE pvNativeTypeStart, PCCOR_SIGNATURE pvNativeType, ULONG cbNativeType)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
       
    if ( ((ULONG)(pvNativeType - pvNativeTypeStart)) == cbNativeType )
    {
        return S_FALSE;  //no more data
    }
    
    PCCOR_SIGNATURE pvProjectedEnd = pvNativeType + CorSigUncompressedDataSize(pvNativeType);
    if (pvProjectedEnd <= pvNativeType || ((ULONG)(pvProjectedEnd - pvNativeTypeStart)) > cbNativeType)
        return E_FAIL; //corrupted data

    return S_OK;
}

//==========================================================================
// Parse and validate the NATIVE_TYPE_ metadata.
// Note! NATIVE_TYPE_ metadata is optional. If it's not present, this
// routine sets NativeTypeParamInfo->m_NativeType to NATIVE_TYPE_DEFAULT. 
//==========================================================================
BOOL ParseNativeTypeInfo(NativeTypeParamInfo* pParamInfo, PCCOR_SIGNATURE pvNativeType, ULONG cbNativeType);

BOOL ParseNativeTypeInfo(mdToken                    token,
                         IMDInternalImport*         pScope,
                         NativeTypeParamInfo*       pParamInfo)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    PCCOR_SIGNATURE pvNativeType;
    ULONG           cbNativeType;

    if (token == mdParamDefNil || pScope->GetFieldMarshal(token, &pvNativeType, &cbNativeType) != S_OK)
        return TRUE;

    return ParseNativeTypeInfo(pParamInfo, pvNativeType, cbNativeType);
}

BOOL ParseNativeTypeInfo(NativeTypeParamInfo* pParamInfo,
                         PCCOR_SIGNATURE pvNativeType,
                         ULONG cbNativeType)
{
    HRESULT hr;

    PCCOR_SIGNATURE pvNativeTypeStart = pvNativeType;

    if (cbNativeType == 0)
        return FALSE;  // Zero-length NATIVE_TYPE block

    pParamInfo->m_NativeType = (CorNativeType)*(pvNativeType++);
    int strLen = 0;

    // Retrieve any extra information associated with the native type.
    switch (pParamInfo->m_NativeType)
    {
            
        case NATIVE_TYPE_FIXEDARRAY:
            
            if (S_OK != CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType))
                return FALSE; 

            pParamInfo->m_Additive = CorSigUncompressData(pvNativeType);
            
            if (S_OK != CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType))
                return TRUE;

            pParamInfo->m_ArrayElementType = (CorNativeType)CorSigUncompressData(pvNativeType);                
            break;
            
        case NATIVE_TYPE_FIXEDSYSSTRING:
            if (S_OK != CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType))
                return FALSE; 

            pParamInfo->m_Additive = CorSigUncompressData(pvNativeType);
            break;
            

        case NATIVE_TYPE_ARRAY:
            hr = CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType);
            if (FAILED(hr))
                return FALSE;

            if (hr == S_OK)
                pParamInfo->m_ArrayElementType = (CorNativeType) (CorSigUncompressData(/*modifies*/pvNativeType));

            // Check for "sizeis" param index
            hr = CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType);
            if (FAILED(hr))
                return FALSE;

            if (hr == S_OK)
            {
                pParamInfo->m_SizeIsSpecified = TRUE;
                pParamInfo->m_CountParamIdx = (UINT16)(CorSigUncompressData(/*modifies*/pvNativeType));

                // If an "sizeis" param index is present, the defaults for multiplier and additive change
                pParamInfo->m_Multiplier = 1;
                pParamInfo->m_Additive   = 0;

                // Check for "sizeis" additive
                hr = CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType);
                if (FAILED(hr))
                    return FALSE;

                if (hr == S_OK)
                {
                    // Extract the additive.
                    pParamInfo->m_Additive = (DWORD)CorSigUncompressData(/*modifies*/pvNativeType);

                    // Check to see if the flags field is present. 
                    hr = CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType);
                    if (FAILED(hr))
                        return FALSE;

                    if (hr == S_OK)
                    {
                        // If the param index specified flag isn't set then we need to reset the
                        // multiplier to 0 to indicate no size param index was specified.
                        NativeTypeArrayFlags flags = (NativeTypeArrayFlags)CorSigUncompressData(/*modifies*/pvNativeType);;
                        if (!(flags & ntaSizeParamIndexSpecified))
                            pParamInfo->m_Multiplier = 0;
                    }
                }
            }

            break;

        case NATIVE_TYPE_CUSTOMMARSHALER:
            // Skip the typelib guid.
            if (S_OK != CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType))
                return FALSE;

            strLen = CPackedLen::GetLength(pvNativeType, (void const **)&pvNativeType);

            if (pvNativeType + strLen < pvNativeType ||
                pvNativeType + strLen > pvNativeTypeStart + cbNativeType)
                return FALSE;

            pvNativeType += strLen;
            _ASSERTE((ULONG)(pvNativeType - pvNativeTypeStart) < cbNativeType);                

            // Skip the name of the native type.
            if (S_OK != CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType))
                return FALSE;

            strLen = CPackedLen::GetLength(pvNativeType, (void const **)&pvNativeType);
            if (pvNativeType + strLen < pvNativeType ||
                pvNativeType + strLen > pvNativeTypeStart + cbNativeType)
                return FALSE;

            pvNativeType += strLen;
            _ASSERTE((ULONG)(pvNativeType - pvNativeTypeStart) < cbNativeType);

            // Extract the name of the custom marshaler.
            if (S_OK != CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType))
                return FALSE;

            strLen = CPackedLen::GetLength(pvNativeType, (void const **)&pvNativeType);
            if (pvNativeType + strLen < pvNativeType ||
                pvNativeType + strLen > pvNativeTypeStart + cbNativeType)
                return FALSE;

            pParamInfo->m_strCMMarshalerTypeName = (LPUTF8)pvNativeType;
            pParamInfo->m_cCMMarshalerTypeNameBytes = strLen;
            pvNativeType += strLen;
            _ASSERTE((ULONG)(pvNativeType - pvNativeTypeStart) < cbNativeType);

            // Extract the cookie string.
            if (S_OK != CheckForCompressedData(pvNativeTypeStart, pvNativeType, cbNativeType))
                return FALSE;

            strLen = CPackedLen::GetLength(pvNativeType, (void const **)&pvNativeType);
            if (pvNativeType + strLen < pvNativeType ||
                pvNativeType + strLen > pvNativeTypeStart + cbNativeType)
                return FALSE;

            pParamInfo->m_strCMCookie = (LPUTF8)pvNativeType;
            pParamInfo->m_cCMCookieStrBytes = strLen;
            _ASSERTE((ULONG)(pvNativeType + strLen - pvNativeTypeStart) == cbNativeType);
            break;

        default:
            break;
    }

    return TRUE;
}

VOID ThrowInteropParamException(UINT resID, UINT paramIdx)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    SString paramString;
    if (paramIdx == 0)
        paramString.Set(L"return value");
    else
        paramString.Printf(L"parameter #%u", paramIdx);

    SString errorString(L"Unknown error.");
    errorString.LoadResource(resID);
    
    COMPlusThrow(kMarshalDirectiveException, IDS_EE_BADMARSHAL_ERROR_MSG, paramString.GetUnicode(), errorString.GetUnicode());
}

//===================================================================================
// Post-patches ML stubs for the sizeis feature.
//===================================================================================
VOID PatchMLStubForSizeIs(BYTE *pMLCode, UINT numArgs, MarshalInfo *pMLInfo)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    INT16 *offsets = (INT16*)_alloca(numArgs * sizeof(INT16));
    
    INT16 srcofs = 0;
    UINT i;
    for (i = 0; i < numArgs; i++) 
    {
        offsets[i] = srcofs;
        srcofs += StackElemSize(pMLInfo[i].GetNativeArgSize());
    }

    BYTE *pMLWalk = pMLCode;
    for (i = 0; i < numArgs; i++) 
    {
        if (pMLInfo[i].GetMarshalType() == MarshalInfo::MARSHAL_TYPE_NATIVEARRAY)
        {
            MLCode mlcode;
            while ((mlcode = *(pMLWalk++)) != ML_CREATE_MARSHALER_CARRAY)
            {
                _ASSERTE(mlcode != ML_END && mlcode != ML_INTERRUPT);
                pMLWalk += gMLInfo[mlcode].m_numOperandBytes;
            }

            ML_CREATE_MARSHALER_CARRAY_OPERANDS *pmops = (ML_CREATE_MARSHALER_CARRAY_OPERANDS*)pMLWalk;
            pMLWalk += gMLInfo[mlcode].m_numOperandBytes;            
            if (pmops->multiplier != 0) 
            {            
                UINT16 countParamIdx = pmops->countParamIdx;
                if (countParamIdx >= numArgs)
                    COMPlusThrow(kIndexOutOfRangeException, IDS_EE_SIZECONTROLOUTOFRANGE);

                pmops->offsetbump = offsets[countParamIdx] - offsets[i];
                switch (pMLInfo[countParamIdx].GetMarshalType())
                {
                    case MarshalInfo::MARSHAL_TYPE_GENERIC_1:
                    case MarshalInfo::MARSHAL_TYPE_GENERIC_U1:
                        pmops->countSize = 1;
                        break;

                    case MarshalInfo::MARSHAL_TYPE_GENERIC_2:
                    case MarshalInfo::MARSHAL_TYPE_GENERIC_U2:
                        pmops->countSize = 2;
                        break;

                    case MarshalInfo::MARSHAL_TYPE_GENERIC_4:
                        pmops->countSize = 4;
                        break;

                    case MarshalInfo::MARSHAL_TYPE_GENERIC_8:
                        pmops->countSize = 8;
                        break;

                    default:
                        COMPlusThrow(kMarshalDirectiveException, IDS_EE_SIZECONTROLBADTYPE);
                }
            }
        }
    }
}

//===============================================================
// Collects paraminfo's in an indexed array so that:
//
//   aParams[0] == param token for return value
//   aParams[1] == param token for argument #1...
//   aParams[numargs] == param token for argument #n...
//
// If no param token exists, the corresponding array element
// is set to mdParamDefNil.
//
// Inputs:
//    pInternalImport  -- ifc for metadata api
//    md       -- token of method. If token is mdMethodNil,
//                all aParam elements will be set to mdParamDefNil.
//    numargs  -- # of arguments in mdMethod
//    aParams  -- uninitialized array with numargs+1 elements.
//                on exit, will be filled with param tokens.
//===============================================================
VOID CollateParamTokens(IMDInternalImport *pInternalImport, mdMethodDef md, ULONG numargs, mdParamDef *aParams)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    for (ULONG i = 0; i < numargs + 1; i++)
        aParams[i] = mdParamDefNil;

    if (md != mdMethodDefNil)
    {
        MDEnumHolder hEnumParams(pInternalImport);
        HRESULT hr = pInternalImport->EnumInit(mdtParamDef, md, &hEnumParams);
        if (FAILED(hr))
        {
            // no param info: nothing left to do here
        }
        else
        {
            mdParamDef CurrParam = mdParamDefNil;
            while (pInternalImport->EnumNext(&hEnumParams, &CurrParam))
            {
                USHORT usSequence;
                DWORD dwAttr;
                pInternalImport->GetParamDefProps(CurrParam, &usSequence, &dwAttr);

                _ASSERTE(usSequence <= numargs);
                _ASSERTE(aParams[usSequence] == mdParamDefNil);
                aParams[usSequence] = CurrParam; 
            }
        }
    }
}

//===================================================================================
// Support routines for storing ML stubs in prejit files
//===================================================================================




EEMarshalingData::EEMarshalingData(BaseDomain *pDomain, LoaderHeap *pHeap, CrstBase *pCrst) :
    m_pHeap(pHeap),
    m_pDomain(pDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    LockOwner lock = {pCrst, IsOwnerOfCrst};
    m_CMHelperHashtable.Init(INITIAL_NUM_CMHELPER_HASHTABLE_BUCKETS, &lock);
    m_SharedCMHelperToCMInfoMap.Init(INITIAL_NUM_CMINFO_HASHTABLE_BUCKETS, &lock);
}


EEMarshalingData::~EEMarshalingData()
{
    WRAPPER_CONTRACT;
    
    CustomMarshalerInfo *pCMInfo;


    // Walk through the linked list and delete all the custom marshaler info's.
    while ((pCMInfo = m_pCMInfoList.RemoveHead()) != NULL)
        delete pCMInfo;
}


void *EEMarshalingData::operator new(size_t size, LoaderHeap *pHeap)
{
    CONTRACT (void*)
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pHeap));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    void* mem = pHeap->AllocMem(sizeof(EEMarshalingData));

    RETURN mem;
}


void EEMarshalingData::operator delete(void *pMem)
{
    LEAF_CONTRACT;
    // Instances of this class are always allocated on the loader heap so
    // the delete operator has nothing to do.
}


CustomMarshalerHelper *EEMarshalingData::GetCustomMarshalerHelper(Assembly *pAssembly, TypeHandle hndManagedType, LPCUTF8 strMarshalerTypeName, DWORD cMarshalerTypeNameBytes, LPCUTF8 strCookie, DWORD cCookieStrBytes)
{
    CONTRACT (CustomMarshalerHelper*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pAssembly));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    CustomMarshalerHelper *pCMHelper = NULL;
    CustomMarshalerHelper* pNewCMHelper = NULL;
    NewHolder<CustomMarshalerInfo> pNewCMInfo(NULL);
    
    BOOL bSharedHelper = pAssembly->IsDomainNeutral();
    TypeHandle hndCustomMarshalerType;

    // Create the key that will be used to lookup in the hashtable.
    EECMHelperHashtableKey Key(cMarshalerTypeNameBytes, strMarshalerTypeName, cCookieStrBytes, strCookie, bSharedHelper);

    // Lookup the custom marshaler helper in the hashtable.
    if (m_CMHelperHashtable.GetValue(&Key, (HashDatum*)&pCMHelper))
        RETURN pCMHelper;

    {
        GCX_COOP();

        // Validate the arguments.
        _ASSERTE(strMarshalerTypeName && strCookie && !hndManagedType.IsNull());

        // Append a NULL terminator to the marshaler type name.
        CQuickArray<char> strCMMarshalerTypeName;
        strCMMarshalerTypeName.ReSizeThrows(cMarshalerTypeNameBytes + 1);
        memcpy(strCMMarshalerTypeName.Ptr(), strMarshalerTypeName, cMarshalerTypeNameBytes);
        strCMMarshalerTypeName[cMarshalerTypeNameBytes] = 0;

        // Load the custom marshaler class. 
        BOOL fNameIsAsmQualified = FALSE;
        hndCustomMarshalerType = TypeName::GetTypeUsingCASearchRules(strCMMarshalerTypeName.Ptr(), pAssembly, &fNameIsAsmQualified);
        
        // Set the assembly to null to indicate that the custom marshaler name is assembly
        // qualified.        
        if (fNameIsAsmQualified)
            pAssembly = NULL;


        if (bSharedHelper)
        {
            // Create the custom marshaler helper in the specified heap.
            pNewCMHelper = new (m_pHeap) SharedCustomMarshalerHelper(pAssembly, hndManagedType, strMarshalerTypeName, cMarshalerTypeNameBytes, strCookie, cCookieStrBytes);
        }
        else
        {
            // Create the custom marshaler info in the specified heap.
            pNewCMInfo = new (m_pHeap) CustomMarshalerInfo(m_pDomain, hndCustomMarshalerType, hndManagedType, strCookie, cCookieStrBytes);

            // Create the custom marshaler helper in the specified heap.
            pNewCMHelper = new (m_pHeap) NonSharedCustomMarshalerHelper(pNewCMInfo);
        }
    }

    // Take the app domain lock before we insert the custom marshaler info into the hashtable.
    {
        BaseDomain::LockHolder lh(m_pDomain);

        // Verify that the custom marshaler helper has not already been added by another thread.
        if (m_CMHelperHashtable.GetValue(&Key, (HashDatum*)&pCMHelper))
        {
            lh.Release();
            RETURN pCMHelper;
        }

        // Add the custom marshaler helper to the hash table.
        m_CMHelperHashtable.InsertValue(&Key, pNewCMHelper, FALSE);

        // If we create the CM info, then add it to the linked list.
        if (pNewCMInfo)
        {
            m_pCMInfoList.InsertHead(pNewCMInfo);
            pNewCMInfo.SuppressRelease();
        }

        // Release the lock and return the custom marshaler info.
    }

    RETURN pNewCMHelper;
}

CustomMarshalerInfo *EEMarshalingData::GetCustomMarshalerInfo(SharedCustomMarshalerHelper *pSharedCMHelper)
{
    CONTRACT (CustomMarshalerInfo*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    CustomMarshalerInfo *pCMInfo = NULL;
    NewHolder<CustomMarshalerInfo> pNewCMInfo(NULL);
    TypeHandle hndCustomMarshalerType;

    // Lookup the custom marshaler helper in the hashtable.
    if (m_SharedCMHelperToCMInfoMap.GetValue(pSharedCMHelper, (HashDatum*)&pCMInfo))
        RETURN pCMInfo;

    // Append a NULL terminator to the marshaler type name.
    CQuickArray<char> strCMMarshalerTypeName;
    DWORD strLen = pSharedCMHelper->GetMarshalerTypeNameByteCount();
    strCMMarshalerTypeName.ReSizeThrows(pSharedCMHelper->GetMarshalerTypeNameByteCount() + 1);
    memcpy(strCMMarshalerTypeName.Ptr(), pSharedCMHelper->GetMarshalerTypeName(), strLen);
    strCMMarshalerTypeName[strLen] = 0;
    
    // Load the custom marshaler class. 
    hndCustomMarshalerType = TypeName::GetTypeUsingCASearchRules(strCMMarshalerTypeName.Ptr(), pSharedCMHelper->GetAssembly());

    // Create the custom marshaler info in the specified heap.
    pNewCMInfo = new (m_pHeap) CustomMarshalerInfo(m_pDomain, 
                                                   hndCustomMarshalerType, 
                                                   pSharedCMHelper->GetManagedType(), 
                                                   pSharedCMHelper->GetCookieString(), 
                                                   pSharedCMHelper->GetCookieStringByteCount());

    {
        // Take the app domain lock before we insert the custom marshaler info into the hashtable.
        BaseDomain::LockHolder lh(m_pDomain);

        // Verify that the custom marshaler info has not already been added by another thread.
        if (m_SharedCMHelperToCMInfoMap.GetValue(pSharedCMHelper, (HashDatum*)&pCMInfo))
        {
            lh.Release();
            RETURN pCMInfo;
        }

        // Add the custom marshaler helper to the hash table.
        m_SharedCMHelperToCMInfoMap.InsertValue(pSharedCMHelper, pNewCMInfo, FALSE);

        // Add the custom marshaler into the linked list.
        m_pCMInfoList.InsertHead(pNewCMInfo);

        // Release the lock and return the custom marshaler info.
    }

    pNewCMInfo.SuppressRelease();
    RETURN pNewCMInfo;
}


//==========================================================================
// Constructs MarshalInfo. 
//==========================================================================
MarshalInfo::MarshalInfo(Module* pModule,
                         SigPointer sig,
                         mdToken token,
                         MarshalScenario ms,
                         BYTE nlType,
                         BYTE nlFlags,
                         BOOL isParam,
                         UINT paramidx,   // parameter # for use in error messages (ignored if not parameter)
                         BOOL BestFit,
                         BOOL ThrowOnUnmappableChar,
                         BOOL fEmitsIL,
                         MethodDesc* pMD,
                         BOOL fLoadCustomMarshal
#ifdef _DEBUG
                         ,
                         LPCUTF8 pDebugName,
                         LPCUTF8 pDebugClassName,
                         LPCUTF8 pDebugNameSpace,
                         UINT    argidx  // 0 for return value, -1 for field
#endif
)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    SigTypeContext emptyTypeContext; // an empty type context is sufficient: all methods should be non-generic

    HRESULT hr;
    NativeTypeParamInfo ParamInfo;

    // we expect a 1-based paramidx, but we like to use a 0-based paramidx
    m_paramidx                      = paramidx - 1;

    // if no one overwrites this with a better message, we'll still at least say something    
    m_resID                         = IDS_EE_BADMARSHAL_GENERIC;

    // flag for uninitialized type
    m_type                          = MARSHAL_TYPE_UNKNOWN;

    CorNativeType nativeType        = NATIVE_TYPE_DEFAULT;
    Assembly *pAssembly             = pModule->GetAssembly();
    BOOL fNeedsCopyCtor             = FALSE;
    m_BestFit                       = BestFit;
    m_ThrowOnUnmappableChar         = ThrowOnUnmappableChar;
    m_ms                            = ms;
    m_fAnsi                         = (ms == MARSHAL_SCENARIO_NDIRECT) && (nlType == nltAnsi);
    m_comArgSize                    = 0;
    m_nativeArgSize                 = 0;
    m_pCMHelper                     = NULL;
    m_CMVt                          = VT_EMPTY;
    m_args.m_pMarshalInfo           = this;
    m_args.m_pMT                    = NULL;
    m_pModule                       = pModule;
    CorElementType mtype            = ELEMENT_TYPE_END;
    CorElementType corElemType      = ELEMENT_TYPE_END;
    m_pMT                        = NULL;




#ifdef _DEBUG

    CHAR achDbgContext[DEBUG_CONTEXT_STR_LEN] = "";
    if (!pDebugName)
    {
        strncpy_s(achDbgContext, COUNTOF(achDbgContext), "<Unknown>", _TRUNCATE);
    }
    else
    {
        if (pDebugNameSpace)
        {
            strncpy_s(achDbgContext, COUNTOF(achDbgContext), pDebugNameSpace, _TRUNCATE);
            strncat_s(achDbgContext, COUNTOF(achDbgContext), NAMESPACE_SEPARATOR_STR, _TRUNCATE);
        }
        strncat_s(achDbgContext, COUNTOF(achDbgContext), pDebugClassName, _TRUNCATE);
        strncat_s(achDbgContext, COUNTOF(achDbgContext), NAMESPACE_SEPARATOR_STR, _TRUNCATE);
        strncat_s(achDbgContext, COUNTOF(achDbgContext), pDebugName, _TRUNCATE);
        strncat_s(achDbgContext, COUNTOF(achDbgContext), " ", _TRUNCATE);
        switch (argidx)
        {
            case -1:
                strncat_s(achDbgContext, COUNTOF(achDbgContext), "field", _TRUNCATE);
                break;
            case 0:
                strncat_s(achDbgContext, COUNTOF(achDbgContext), "return value", _TRUNCATE);
                break;
            default:
            {
                char buf[30];
                sprintf_s(buf, COUNTOF(buf), "param #%lu", (ULONG)argidx);
                strncat_s(achDbgContext, COUNTOF(achDbgContext), buf, _TRUNCATE);
            }
        }
    }

    m_strDebugMethName = pDebugName;
    m_strDebugClassName = pDebugClassName;
    m_strDebugNameSpace = pDebugNameSpace;
    m_iArg = argidx;

    m_in = m_out = FALSE;
    m_byref = TRUE;
#endif



    // Retrieve the native type for the current parameter.
    if (!ParseNativeTypeInfo(token, pModule->GetMDImport(), &ParamInfo))
    {
        IfFailGoto(E_FAIL, lFail);
    }
   
    nativeType = ParamInfo.m_NativeType;
    corElemType = (CorElementType) sig.PeekElemTypeNormalized(pModule,&emptyTypeContext); 
    mtype       = corElemType;

    // Parse ET_BYREF signature
    if (mtype == ELEMENT_TYPE_BYREF)
    {
        m_byref = TRUE;
        SigPointer sigtmp = sig;
        IfFailGoto(sig.GetElemType(NULL), lFail);
        mtype = (CorElementType) sig.PeekElemTypeNormalized(pModule,&emptyTypeContext); 

        // Check for Copy Constructor Modifier
        if (mtype == ELEMENT_TYPE_VALUETYPE) 
        {
            // Skip ET_BYREF
            IfFailGoto(sigtmp.GetByte(NULL), lFail);
            
            if (sigtmp.HasCustomModifier(pModule, "Microsoft.VisualC.NeedsCopyConstructorModifier", ELEMENT_TYPE_CMOD_REQD) ||
                sigtmp.HasCustomModifier(pModule, "System.Runtime.CompilerServices.IsCopyConstructed", ELEMENT_TYPE_CMOD_REQD) )
            {
                fNeedsCopyCtor = TRUE;
                m_byref = FALSE;
            }
        }
    }
    else
    {
        m_byref = FALSE;
    }


    // Check for valid ET_PTR signature
    if (mtype == ELEMENT_TYPE_PTR)
    {
        SigPointer sigtmp = sig;
        IfFailGoto(sigtmp.GetElemType(NULL), lFail);
        CorElementType mtype2 = (CorElementType) sigtmp.PeekElemTypeNormalized(pModule,&emptyTypeContext);

        if (mtype2 == ELEMENT_TYPE_VALUETYPE) 
        {

            TypeHandle th = sigtmp.GetTypeHandleThrowing(pModule,&emptyTypeContext);
            _ASSERTE(!th.IsNull());

            // It should be blittable
            if (!th.IsBlittable())
            {
                m_resID = IDS_EE_BADMARSHAL_PTRNONBLITTABLE;
                IfFailGoto(E_FAIL, lFail);
            }

            // Check for Copy Constructor Modifier
            if (sigtmp.HasCustomModifier(pModule, "Microsoft.VisualC.NeedsCopyConstructorModifier", ELEMENT_TYPE_CMOD_REQD) ||
                sigtmp.HasCustomModifier(pModule, "System.Runtime.CompilerServices.IsCopyConstructed", ELEMENT_TYPE_CMOD_REQD) )
            {
                // Skip ET_PTR
                IfFailGoto(sig.GetElemType(NULL), lFail);
                
                mtype = (CorElementType) sig.PeekElemTypeNormalized(pModule,NULL); // Nb. NULL instantiations OK: non-generic signature
                fNeedsCopyCtor = TRUE;
                m_byref = FALSE;
            }
        }
        else
        {
            if (!(mtype2 != ELEMENT_TYPE_CLASS &&
                  mtype2 != ELEMENT_TYPE_STRING &&
                  mtype2 != ELEMENT_TYPE_CLASS &&
                  mtype2 != ELEMENT_TYPE_OBJECT &&
                  mtype2 != ELEMENT_TYPE_SZARRAY))
            {
                m_resID = IDS_EE_BADMARSHAL_PTRSUBTYPE;
                IfFailGoto(E_FAIL, lFail);
            }
        }
    }


    if (m_byref && ParamInfo.m_SizeIsSpecified)
    {
        m_resID = IDS_EE_PINVOKE_NOREFFORSIZEIS;
        IfFailGoto(E_FAIL, lFail);
    }

    
    if (mtype == ELEMENT_TYPE_VALUETYPE)
    {
        TypeHandle thnd = sig.GetTypeHandleThrowing(pModule, &emptyTypeContext);
        if (!(thnd.IsNull()))
        {  
            CorElementType realmtype = thnd.GetInternalCorElementType();

            if (CorTypeInfo::IsPrimitiveType(realmtype))
                mtype = realmtype;
        }
    }

    if (nativeType == NATIVE_TYPE_CUSTOMMARSHALER)
    {

        switch (mtype)
        {
            case ELEMENT_TYPE_VAR:
            case ELEMENT_TYPE_CLASS:
            case ELEMENT_TYPE_OBJECT:
                m_CMVt = VT_UNKNOWN;
                break;

            case ELEMENT_TYPE_STRING:
            case ELEMENT_TYPE_SZARRAY:
            case ELEMENT_TYPE_ARRAY:
                m_CMVt = VT_I4;
                break;

            default:    
                m_resID = IDS_EE_BADMARSHAL_CUSTOMMARSHALER;
                IfFailGoto(E_FAIL, lFail);
        }

        // Set m_type to MARSHAL_TYPE_UNKNOWN in case SetupCustomMarshalerHelper throws.
        m_type = MARSHAL_TYPE_UNKNOWN; 

        if (fLoadCustomMarshal)
        {
            // Set up the custom marshaler info.
            TypeHandle hndManagedType = sig.GetTypeHandle(pModule, &emptyTypeContext, NULL);

            if (!fEmitsIL)
            {
                m_pCMHelper = SetupCustomMarshalerHelper(ParamInfo.m_strCMMarshalerTypeName, 
                                                        ParamInfo.m_cCMMarshalerTypeNameBytes,
                                                        ParamInfo.m_strCMCookie, 
                                                        ParamInfo.m_cCMCookieStrBytes,
                                                        pAssembly,
                                                        hndManagedType);
            }
        }

        // Specify which custom marshaler to use.
        m_type = MARSHAL_TYPE_REFERENCECUSTOMMARSHALER;

        goto lExit;
    }
   
    switch (mtype)
    {
        case ELEMENT_TYPE_BOOLEAN:
            switch (nativeType)
            {
                case NATIVE_TYPE_BOOLEAN:
                    m_type = MARSHAL_TYPE_WINBOOL;
                    break;


                case NATIVE_TYPE_U1:
                case NATIVE_TYPE_I1:
                    m_type = MARSHAL_TYPE_CBOOL;
                    break;

                case NATIVE_TYPE_DEFAULT:
                    {
                        m_type = MARSHAL_TYPE_WINBOOL;
                    }
                    break;

                default:
                    m_resID = IDS_EE_BADMARSHAL_BOOLEAN;
                    IfFailGoto(E_FAIL, lFail);
            }
            break;

        case ELEMENT_TYPE_CHAR:
            switch (nativeType)
            {
                case NATIVE_TYPE_I1: //fallthru
                case NATIVE_TYPE_U1:
                    m_type = MARSHAL_TYPE_ANSICHAR;
                    break;

                case NATIVE_TYPE_I2: //fallthru
                case NATIVE_TYPE_U2:
                    m_type = MARSHAL_TYPE_GENERIC_U2;
                    break;

                case NATIVE_TYPE_DEFAULT:
                    m_type = ( (m_ms == MARSHAL_SCENARIO_NDIRECT && m_fAnsi) ? MARSHAL_TYPE_ANSICHAR : MARSHAL_TYPE_GENERIC_U2 );
                    break;

                default:
                    m_resID = IDS_EE_BADMARSHAL_CHAR;
                    IfFailGoto(E_FAIL, lFail);

            }
            break;

        case ELEMENT_TYPE_I1:
            if (!(nativeType == NATIVE_TYPE_I1 || nativeType == NATIVE_TYPE_U1 || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_I1;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_GENERIC_1;
            break;

        case ELEMENT_TYPE_U1:
            if (!(nativeType == NATIVE_TYPE_U1 || nativeType == NATIVE_TYPE_I1 || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_I1;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_GENERIC_U1;
            break;

        case ELEMENT_TYPE_I2:
            if (!(nativeType == NATIVE_TYPE_I2 || nativeType == NATIVE_TYPE_U2 || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_I2;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_GENERIC_2;
            break;

        case ELEMENT_TYPE_U2:
            if (!(nativeType == NATIVE_TYPE_U2 || nativeType == NATIVE_TYPE_I2 || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_I2;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_GENERIC_U2;
            break;

        case ELEMENT_TYPE_I4:
            switch (nativeType)
            {
                case NATIVE_TYPE_I4:
                case NATIVE_TYPE_U4:
                case NATIVE_TYPE_DEFAULT:
                    break;


                default:
                m_resID = IDS_EE_BADMARSHAL_I4;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_GENERIC_4;
            break;

        case ELEMENT_TYPE_U4:
            switch (nativeType)
            {
                case NATIVE_TYPE_I4:
                case NATIVE_TYPE_U4:
                case NATIVE_TYPE_DEFAULT:
                    break;


                default:
                m_resID = IDS_EE_BADMARSHAL_I4;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_GENERIC_4;
            break;

        case ELEMENT_TYPE_I8:
            if (!(nativeType == NATIVE_TYPE_I8 || nativeType == NATIVE_TYPE_U8 || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_I8;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_GENERIC_8;
            break;

        case ELEMENT_TYPE_U8:
            if (!(nativeType == NATIVE_TYPE_U8 || nativeType == NATIVE_TYPE_I8 || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_I8;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_GENERIC_8;
            break;

        case ELEMENT_TYPE_I:
            if (!(nativeType == NATIVE_TYPE_INT || nativeType == NATIVE_TYPE_UINT || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_I;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = (sizeof(LPVOID) == 4 ? MARSHAL_TYPE_GENERIC_4 : MARSHAL_TYPE_GENERIC_8);
            break;

        case ELEMENT_TYPE_U:
            if (!(nativeType == NATIVE_TYPE_UINT || nativeType == NATIVE_TYPE_INT || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_I;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = (sizeof(LPVOID) == 4 ? MARSHAL_TYPE_GENERIC_4 : MARSHAL_TYPE_GENERIC_8);
            break;


        case ELEMENT_TYPE_R4:
            if (!(nativeType == NATIVE_TYPE_R4 || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_R4;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_FLOAT;
            break;

        case ELEMENT_TYPE_R8:
            if (!(nativeType == NATIVE_TYPE_R8 || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_R8;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = MARSHAL_TYPE_DOUBLE;
            break;

        case ELEMENT_TYPE_PTR:
            if (nativeType != NATIVE_TYPE_DEFAULT)
            {
                m_resID = IDS_EE_BADMARSHAL_PTR;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = ( (sizeof(void*)==4) ? MARSHAL_TYPE_GENERIC_4 : MARSHAL_TYPE_GENERIC_8 );
            break;

        case ELEMENT_TYPE_FNPTR:
            if (!(nativeType == NATIVE_TYPE_FUNC || nativeType == NATIVE_TYPE_DEFAULT))
            {
                m_resID = IDS_EE_BADMARSHAL_FNPTR;
                IfFailGoto(E_FAIL, lFail);
            }
            m_type = ( (sizeof(void*)==4) ? MARSHAL_TYPE_GENERIC_4 : MARSHAL_TYPE_GENERIC_8 );
            break;

        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_VAR:
        {                

            TypeHandle sigTH = sig.GetTypeHandleThrowing(pModule, &emptyTypeContext);

            // Disallow marshaling generic types.
            if (sigTH.HasInstantiation())
            {
                m_resID = IDS_EE_BADMARSHAL_GENERICS_RESTRICTION;
                IfFailGoto(E_FAIL, lFail);
            }


            m_pMT = sigTH.GetMethodTable();
            if (m_pMT == NULL)
                IfFailGoto(COR_E_TYPELOAD, lFail);

            {
                bool builder = false;
                if (sig.IsStringTypeThrowing(pModule, &emptyTypeContext) 
                    || ((builder = true), 0)
                    || sig.IsClassThrowing(pModule, g_StringBufferClassName)
                    )
                {

                    StringType      stype = enum_WSTR;
       
                    switch ( nativeType )
                    {
                        case NATIVE_TYPE_LPWSTR:
                            stype = enum_WSTR;
                            break;
        
                        case NATIVE_TYPE_LPSTR:
                            stype = enum_CSTR;
                            break;
    
                        case NATIVE_TYPE_LPTSTR:
                        {
                            {
                                static BOOL init = FALSE;
                                static BOOL onUnicode;
    
                                if (!init)
                                {
                                    onUnicode = NDirectOnUnicodeSystem();
                                    init = TRUE;
                                }
    
                                    if (onUnicode)
                                        stype = enum_WSTR;
                                    else
                                        stype = enum_CSTR;
                            }

                            break;
                        }

    
                        case NATIVE_TYPE_DEFAULT:
                        {
                                stype = m_fAnsi ? enum_CSTR : enum_WSTR;

                            break;
                        }
    
                        default:
                            m_resID = builder ? IDS_EE_BADMARSHALPARAM_STRINGBUILDER : IDS_EE_BADMARSHALPARAM_STRING;
                            IfFailGoto(E_FAIL, lFail);
                            break;
                    }
        
                    {
                        _ASSERTE(MARSHAL_TYPE_LPWSTR + enum_WSTR == (int) MARSHAL_TYPE_LPWSTR);
                        _ASSERTE(MARSHAL_TYPE_LPWSTR + enum_CSTR == (int) MARSHAL_TYPE_LPSTR);
                        _ASSERTE(MARSHAL_TYPE_LPWSTR_BUFFER + enum_WSTR == (int) MARSHAL_TYPE_LPWSTR_BUFFER);
                        _ASSERTE(MARSHAL_TYPE_LPWSTR_BUFFER + enum_CSTR == (int) MARSHAL_TYPE_LPSTR_BUFFER);
                        
                        if (builder)
                            m_type = (MarshalType) (MARSHAL_TYPE_LPWSTR_BUFFER + stype);
                        else
                            m_type = (MarshalType) (MARSHAL_TYPE_LPWSTR + stype);
                    }
                }
                else if (sigTH.CanCastTo(TypeHandle(g_Mscorlib.GetClass(CLASS__SAFE_HANDLE))))
                {
                    if (!(nativeType == NATIVE_TYPE_DEFAULT))
                    {
                        m_resID = IDS_EE_BADMARSHAL_SAFEHANDLE;
                        IfFailGoto(E_FAIL, lFail);
                    }
                    m_args.m_pMT = m_pMT;
                    m_type = MARSHAL_TYPE_SAFEHANDLE;
                }
                else if (sigTH.CanCastTo(TypeHandle(g_Mscorlib.GetClass(CLASS__CRITICAL_HANDLE))))
                {
                    if (!(nativeType == NATIVE_TYPE_DEFAULT))
                    {
                        m_resID = IDS_EE_BADMARSHAL_CRITICALHANDLE;
                        IfFailGoto(E_FAIL, lFail);
                    }
                    m_args.m_pMT = m_pMT;
                    m_type = MARSHAL_TYPE_CRITICALHANDLE;
                }
                else if (COMDelegate::IsDelegate(m_pMT))
                {
                    m_args.m_pMT = m_pMT;
                    switch (nativeType)
                    {
                        case NATIVE_TYPE_FUNC:
                            m_type = MARSHAL_TYPE_DELEGATE;
                            break;

                        case NATIVE_TYPE_DEFAULT:
                                m_type = MARSHAL_TYPE_DELEGATE;

                            break;

                        default:
                        m_resID = IDS_EE_BADMARSHAL_DELEGATE;
                        IfFailGoto(E_FAIL, lFail);
                            break;
                    }
                }
                else if (m_pMT->IsBlittable())
                {
                    if (!(nativeType == NATIVE_TYPE_DEFAULT || nativeType == NATIVE_TYPE_LPSTRUCT))
                    {
                        m_resID = IDS_EE_BADMARSHAL_CLASS;
                        IfFailGoto(E_FAIL, lFail);
                    }
                    m_type = MARSHAL_TYPE_BLITTABLEPTR;
                    m_args.m_pMT = m_pMT;
                }
                else if (m_pMT->HasLayout())
                {
                    if (!(nativeType == NATIVE_TYPE_DEFAULT || nativeType == NATIVE_TYPE_LPSTRUCT))
                    {
                        m_resID = IDS_EE_BADMARSHAL_CLASS;
                        IfFailGoto(E_FAIL, lFail);
                    }
                    m_type = MARSHAL_TYPE_LAYOUTCLASSPTR;
                    m_args.m_pMT = m_pMT;
                }    

                else if (m_pMT->IsObjectClass())
                {
                    switch(nativeType)
                    {
                        case NATIVE_TYPE_IUNKNOWN:
                            m_type = MARSHAL_TYPE_INTERFACE;
                            break;

                        case NATIVE_TYPE_ASANY:
                            m_type = m_fAnsi ? MARSHAL_TYPE_ASANYA : MARSHAL_TYPE_ASANYW;
                            break;

                        default:
                            m_resID = IDS_EE_BADMARSHAL_OBJECT;
                            IfFailGoto(E_FAIL, lFail);
                    }
                }
               

                else if (!m_pMT->IsValueClass())
                {
                    m_resID = IDS_EE_BADMARSHAL_NOLAYOUT;
                    IfFailGoto(E_FAIL, lFail);
                }

                else
                {
                    _ASSERTE(m_pMT->IsValueClass());
                    goto lValueClass;
                }
            }
            break;
        }

    
        case ELEMENT_TYPE_VALUETYPE:
        lValueClass:
        {

            if (sig.IsClassThrowing(pModule, g_DecimalClassName))
            {
                switch (nativeType)
                {
                    case NATIVE_TYPE_DEFAULT:
                    case NATIVE_TYPE_STRUCT:
                        m_type = MARSHAL_TYPE_DECIMAL;
                        break;

                    case NATIVE_TYPE_LPSTRUCT:
                        m_type = MARSHAL_TYPE_DECIMAL_PTR;
                        break;

                    case NATIVE_TYPE_CURRENCY:
                        m_type = MARSHAL_TYPE_CURRENCY;
                        break;

                    default:
                        m_resID = IDS_EE_BADMARSHAL_DECIMAL;
                        IfFailGoto(E_FAIL, lFail);
                }
            }
            else if (sig.IsClassThrowing(pModule, g_GuidClassName))
            {
                switch (nativeType)
                {
                    case NATIVE_TYPE_DEFAULT:
                    case NATIVE_TYPE_STRUCT:
                        m_type = MARSHAL_TYPE_GUID;
                        break;

                    case NATIVE_TYPE_LPSTRUCT:
                        m_type = MARSHAL_TYPE_GUID_PTR;
                        break;

                    default:
                        m_resID = IDS_EE_BADMARSHAL_GUID;
                        IfFailGoto(E_FAIL, lFail);
                }
            }
            else if (sig.IsClassThrowing(pModule, g_DateClassName))
            {
                if (!(nativeType == NATIVE_TYPE_DEFAULT || nativeType == NATIVE_TYPE_STRUCT))
                {
                    m_resID = IDS_EE_BADMARSHAL_DATETIME;
                    IfFailGoto(E_FAIL, lFail);
                }
                m_type = MARSHAL_TYPE_DATE;
            }
            else if (sig.IsClassThrowing(pModule, "System.Runtime.InteropServices.ArrayWithOffset"))
            {
                if (!(nativeType == NATIVE_TYPE_DEFAULT))
                {
                    IfFailGoto(E_FAIL, lFail);
                }
                m_type = MARSHAL_TYPE_ARRAYWITHOFFSET;
            }
            else if (sig.IsClassThrowing(pModule, "System.Runtime.InteropServices.HandleRef"))
            {
                if (!(nativeType == NATIVE_TYPE_DEFAULT))
                {
                    IfFailGoto(E_FAIL, lFail);
                }
                m_type = MARSHAL_TYPE_HANDLEREF;
            }
            else if (sig.IsClassThrowing(pModule, "System.ArgIterator"))
            {
                if (!(nativeType == NATIVE_TYPE_DEFAULT))
                {
                    IfFailGoto(E_FAIL, lFail);
                }
                m_type = MARSHAL_TYPE_ARGITERATOR;
            }
            else
            {
                SigTypeContext typeContext; // An empty SigTypeContext is OK - loads the generic type
                m_pMT = sig.GetTypeHandleThrowing(pModule, &typeContext).GetMethodTable();
                if (m_pMT == NULL)
                    break;

                if (m_pMT->HasInstantiation())
                {
                    m_resID = IDS_EE_BADMARSHAL_GENERICS_RESTRICTION;
                    IfFailGoto(E_FAIL, lFail);
                }

                UINT managedSize = m_pMT->GetAlignedNumInstanceFieldBytes();
                UINT  nativeSize = m_pMT->GetNativeSize();
                
                if ( nativeSize > 0xfff0 ||
                    managedSize > 0xfff0)
                {
                    m_resID = IDS_EE_STRUCTTOOCOMPLEX;
                    IfFailGoto(E_FAIL, lFail);
                }

                if (m_pMT->IsBlittable())
                {

                    if (!(nativeType == NATIVE_TYPE_DEFAULT || nativeType == NATIVE_TYPE_STRUCT))
                    {
                        m_resID = IDS_EE_BADMARSHAL_VALUETYPE;
                        IfFailGoto(E_FAIL, lFail);
                    }

                    if (m_byref && !isParam)
                    {
                        // Override the prohibition on byref returns so that IJW works
                        m_byref = FALSE;
                        m_type = ( (sizeof(void*)==4) ? MARSHAL_TYPE_GENERIC_4 : MARSHAL_TYPE_GENERIC_8 );
                    }
                    else
                    {
                        if (fNeedsCopyCtor)
                        {
                            MethodDesc *pCopyCtor;
                            MethodDesc *pDtor;
                            FindCopyCtor(pModule, m_pMT, &pCopyCtor);
                            FindDtor(pModule, m_pMT, &pDtor);

                            m_args.mm.m_pMT = m_pMT;
                            m_args.mm.m_pCopyCtor = pCopyCtor;
                            m_args.mm.m_pDtor = pDtor;
                            m_type = MARSHAL_TYPE_BLITTABLEVALUECLASSWITHCOPYCTOR;
                        }
                        // Permit register-sized structs as return values
                        else if (   !m_byref
                                 && !isParam
                                 && !IsManagedValueTypeReturnedByRef(managedSize)
                                 && !IsUnmanagedValueTypeReturnedByRef(nativeSize)
                                 && managedSize <= sizeof(void*)
                                 && nativeSize <= sizeof(void*))
                        {
                            m_type = ( (sizeof(void*)==4) ? MARSHAL_TYPE_GENERIC_4 : MARSHAL_TYPE_GENERIC_8 );
                            m_args.m_pMT = m_pMT;
                        }
                        else
                        {
                            m_args.m_pMT = m_pMT;
                            m_type = MARSHAL_TYPE_BLITTABLEVALUECLASS;
                        }
                    }
                }
                else if (m_pMT->HasLayout())
                {
                    if (!(nativeType == NATIVE_TYPE_DEFAULT || nativeType == NATIVE_TYPE_STRUCT))
                    {
                        m_resID = IDS_EE_BADMARSHAL_VALUETYPE;
                        IfFailGoto(E_FAIL, lFail);
                    }

                    m_args.m_pMT = m_pMT;
                    m_type = MARSHAL_TYPE_VALUECLASS;
                }
            }
            break;
        }
    
        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_ARRAY:
        {
            // Get class info from array.
            TypeHandle arrayTypeHnd = sig.GetTypeHandleThrowing(pModule, &emptyTypeContext);
            _ASSERTE(!arrayTypeHnd.IsNull());

            ArrayTypeDesc* asArray = arrayTypeHnd.AsArray();
            if (asArray == NULL)
                IfFailGoto(E_FAIL, lFail);

            TypeHandle thElement = asArray->GetTypeParam();

            if (thElement.HasInstantiation())
            {
                m_resID = IDS_EE_BADMARSHAL_GENERICS_RESTRICTION;
                IfFailGoto(E_FAIL, lFail);
            }

            unsigned ofs = 0;
            if (arrayTypeHnd.GetMethodTable())
            {
                ofs = ArrayBase::GetDataPtrOffset(arrayTypeHnd.GetMethodTable());
                if (ofs > 0xffff)
                {
                    ofs = 0;   // can't represent it, so pass on magic value (which causes fallback to regular ML code)
                }
            }

            // Handle retrieving the information for the array type.
            IfFailGoto(HandleArrayElemType(&ParamInfo, (UINT16)ofs, thElement, asArray->GetRank(), mtype == ELEMENT_TYPE_SZARRAY, isParam, pAssembly), lFail);
            break;
        }
        
        default:
            m_resID = IDS_EE_BADMARSHAL_BADMANAGED;
    }

lExit:
    if (m_byref && !isParam)
    {
        m_type = MARSHAL_TYPE_UNKNOWN;
        goto lReallyExit;
    }
   
    //---------------------------------------------------------------------
    // Now, figure out the IN/OUT status.
    //---------------------------------------------------------------------
    if (m_type != MARSHAL_TYPE_UNKNOWN && (GetFlags(m_type, fEmitsIL) & MarshalerFlag_InOnly) && !m_byref)
    {
        // If we got here, the parameter is something like an "int" where
        // [in] is the only semantically valid choice. Since there is no
        // possible way to interpret an [out] for such a type, we will ignore
        // the metadata and force the bits to "in". We could have defined
        // it as an error instead but this is less likely to cause problems
        // with metadata autogenerated from typelibs and poorly
        // defined C headers.
        // 
        m_in = TRUE;
        m_out = FALSE;
    }
    else
    {

        // Capture and save away "In/Out" bits. If none is present, set both to FALSE (they will be properly defaulted downstream)
        if (TypeFromToken(token) != mdtParamDef || token == mdParamDefNil)
        {
            m_in = FALSE;
            m_out = FALSE;
        }
        else
        {
            IMDInternalImport *pInternalImport = pModule->GetMDImport();
            USHORT             usSequence;
            DWORD              dwAttr;
        
            pInternalImport->GetParamDefProps(token, &usSequence, &dwAttr);
            m_in = IsPdIn(dwAttr) != 0;
            m_out = IsPdOut(dwAttr) != 0;
        }
        
        // If neither IN nor OUT are true, this signals the URT to use the default
        // rules.
        if (!m_in && !m_out)
        {
            if (m_byref || 
                 (mtype == ELEMENT_TYPE_CLASS 
                  && !(sig.IsStringType(pModule, NULL))   // Nb. null instantiations OK - sig is non-generic                                    
                  && sig.IsClass(pModule, g_StringBufferClassName)))
            {
                m_in = TRUE;
                m_out = TRUE;
            }
            else
            {
                m_in = TRUE;
                m_out = FALSE;
            }
        
        }
    }


lReallyExit:

#ifdef _DEBUG
    DumpMarshalInfo(pModule, sig, token, ms, nlType, nlFlags); 
#endif
    return;


  lFail:
    // We got here because of an illegal ELEMENT_TYPE/NATIVE_TYPE combo.
    m_type = MARSHAL_TYPE_UNKNOWN;
    //_ASSERTE(!"Invalid ELEMENT_TYPE/NATIVE_TYPE combination");
    goto lExit;
}


VOID MarshalInfo::GetInteropParamException(OBJECTREF* pThrowable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    EX_TRY
    {
        ThrowInteropParamException(m_resID, m_paramidx);
    }
    EX_CATCH
    {
        *pThrowable = GET_THROWABLE();
    }
    EX_END_CATCH(RethrowTransientExceptions)
}

VOID MarshalInfo::EmitOrThrowInteropParamException(InteropStubLinker* psl, UINT resID, UINT paramIdx)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;


    ThrowInteropParamException(resID, paramIdx);

}


HRESULT MarshalInfo::HandleArrayElemType(NativeTypeParamInfo *pParamInfo, UINT16 optbaseoffset,  TypeHandle thElement, int iRank, BOOL fNoLowerBounds, BOOL isParam, Assembly *pAssembly)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pParamInfo));
    }
    CONTRACTL_END;

    ArrayMarshalInfo arrayMarshalInfo(amiRuntime);
    

    //
    // Store rank and bound information.
    //

    m_iArrayRank = iRank;
    m_nolowerbounds = fNoLowerBounds;


    //
    // Determine which type of marshaler to use.
    //

    if (pParamInfo->m_NativeType == NATIVE_TYPE_ARRAY)
    {
        m_type = MARSHAL_TYPE_NATIVEARRAY;
    }
    else if (pParamInfo->m_NativeType == NATIVE_TYPE_DEFAULT)
    {
        {
            m_type = MARSHAL_TYPE_NATIVEARRAY;
        }
    }
    else
    {
        m_resID = IDS_EE_BADMARSHAL_ARRAY;
        return E_FAIL;
    }

    {
        _ASSERTE(m_type == MARSHAL_TYPE_NATIVEARRAY);
        arrayMarshalInfo.InitForNativeArray(m_ms, thElement, pParamInfo->m_ArrayElementType, m_fAnsi);
    }

    // Make sure the marshalling information is valid.
    if (!arrayMarshalInfo.IsValid())
    {
        m_resID = arrayMarshalInfo.GetErrorResourceId();
        return E_FAIL;
    }

    // Set the array type handle and VARTYPE to use for marshalling.
    m_hndArrayElemType = arrayMarshalInfo.GetElementTypeHandle();
    m_arrayElementType = arrayMarshalInfo.GetElementVT();

    if (m_type == MARSHAL_TYPE_NATIVEARRAY)
    {
        // Retrieve the extra information associated with the native array marshaling.
        m_args.na.m_vt  = m_arrayElementType;
        m_args.na.m_pMT = m_hndArrayElemType.IsUnsharedMT() ? m_hndArrayElemType.AsMethodTable() : NULL;   
        m_args.na.m_optionalbaseoffset = optbaseoffset;
        m_countParamIdx = pParamInfo->m_CountParamIdx;
        m_multiplier    = pParamInfo->m_Multiplier;
        m_additive      = pParamInfo->m_Additive;
    }

    return S_OK;
}

static inline MLCode GetOpcodeForPlatform(MLCode mlop, UINT16 cbType, BOOL fManagedToNative)
{
    MLCode codeReturn = mlop;
    if (fManagedToNative || (cbType == STACK_ELEM_SIZE))
    {
        //
        // ia64 needs to zero/sign extend register operands.
        // non-ia64 can just copy sizeof(void*) bytes
        //
        codeReturn = ML_COPY4;
    }

    return codeReturn;
}

static inline UINT16 GetOperandSizeForPlatform(UINT16 cbType, BOOL fManagedToNative)
{
    if (fManagedToNative)
    {
        return cbType;
    }
    else
    {
        return StackElemSize(cbType);
    }
}


void MarshalInfo::GenerateArgumentML(InteropStubLinker* psl,
                                     InteropStubLinker* pslPost,
                                     int argOffset,
                                     BOOL comToNative)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL fEmitsIL = psl ? psl->EmitsIL() : FALSE;
    
    if (m_type == MARSHAL_TYPE_UNKNOWN)
    {
        EmitOrThrowInteropParamException(psl, m_resID, m_paramidx + 1); // m_paramidx is 0-based, but the user wants to see a 1-based index
        return;
    }

    {
        //
        // Use simple copy opcodes if possible.
        //
        if (!m_byref && m_type <= MARSHAL_TYPE_DOUBLE)
        {
            UINT16 size = 0;
            int opcode = 0;

            switch (m_type)
            {
                case MARSHAL_TYPE_GENERIC_1:
                    opcode = GetOpcodeForPlatform(ML_COPYI1, 1, comToNative);
                    size   = GetOperandSizeForPlatform(1, comToNative);
                    break;

                case MARSHAL_TYPE_GENERIC_U1:
                    opcode = GetOpcodeForPlatform(ML_COPYU1, 1, comToNative);
                    size   = GetOperandSizeForPlatform(1, comToNative);
                    break;

                case MARSHAL_TYPE_GENERIC_2:
                    opcode = GetOpcodeForPlatform(ML_COPYI2, 2, comToNative);
                    size   = GetOperandSizeForPlatform(2, comToNative);
                    break;

                case MARSHAL_TYPE_GENERIC_U2:
                    opcode = GetOpcodeForPlatform(ML_COPYU2, 2, comToNative);
                    size   = GetOperandSizeForPlatform(2, comToNative);
                    break;
                    
                case MARSHAL_TYPE_GENERIC_4:
                    opcode = GetOpcodeForPlatform(ML_COPYI4, 4, comToNative);
                    size   = GetOperandSizeForPlatform(4, comToNative);
                    break;

                case MARSHAL_TYPE_GENERIC_U4:
                    opcode = GetOpcodeForPlatform(ML_COPYU4, 4, comToNative);
                    size   = GetOperandSizeForPlatform(4, comToNative);
                    break;

                case MARSHAL_TYPE_GENERIC_8:
                    opcode = ML_COPY8;
                    size   = 8;
                    break;

                case MARSHAL_TYPE_FLOAT:
                    {

#if defined(TRACK_FLOATING_POINT_REGISTERS)
                        opcode = comToNative ? ML_COPYR4_C2N : ML_COPYR4_N2C;
#else
                        opcode = ML_COPY4;
#endif // !TRACK_FLOATING_POINT_REGISTERS

                        size = 4;
                    }
                    break;

                case MARSHAL_TYPE_DOUBLE:
                    {

#if defined(TRACK_FLOATING_POINT_REGISTERS)
                        opcode = comToNative ? ML_COPYR8_C2N : ML_COPYR8_N2C;
#else
                        opcode = ML_COPY8;
#endif // !TRACK_FLOATING_POINT_REGISTERS

                        size = 8;
                    }
                    break;

                case MARSHAL_TYPE_CBOOL:
                    opcode = comToNative ? ML_CBOOL_C2N : ML_CBOOL_N2C;
                    size = 1;

                    default:
                        break;
            }


            if (size != 0)
            {
                if (psl)
                    psl->MLEmit(opcode);

                m_comArgSize = StackElemSize(size);
                m_nativeArgSize = StackElemSize(size);
                return;
            }
        }
    }

    if (m_byref)
    {
        m_comArgSize = StackElemSize(sizeof(void*));
        m_nativeArgSize = StackElemSize(sizeof(void*));
    }
    else
    {
        m_comArgSize = StackElemSize(GetComSize(m_type, m_ms));
        m_nativeArgSize = StackElemSize(GetNativeSize(m_type, m_ms));
    }

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
    if (m_fComArgImplicitByref = (m_comArgSize > ENREGISTERED_PARAMTYPE_MAXSIZE))
        m_comArgSize = StackElemSize(sizeof(void*));

    if (m_fNativeArgImplicitByref = (m_nativeArgSize > ENREGISTERED_PARAMTYPE_MAXSIZE))
        m_nativeArgSize = StackElemSize(sizeof(void*));
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

    if (! psl)
        return;

    MarshalerOverrideStatus amostat;
    UINT resID = IDS_EE_BADMARSHAL_RESTRICTION;
    amostat = (GetArgumentOverrideProc(m_type, fEmitsIL)) (psl,
                                             pslPost,
                                             m_byref,
                                             m_in,
                                             m_out,
                                             comToNative,
                                             &m_args,
                                             &resID,
                                             m_paramidx);

    if (amostat == OVERRIDDEN)
    {
        return;
    }
    
    if (amostat == DISALLOWED)
    {
        EmitOrThrowInteropParamException(psl, resID, m_paramidx + 1); // m_paramidx is 0-based, but the user wants to see a 1-based index
        return;
    }

    CONSISTENCY_CHECK(amostat == HANDLEASNORMAL);

    {
        //
        // Emit marshaler creation opcode
        //
        UINT16 localMarshaler = EmitCreateOpcode(psl);

        //
        // Emit Marshal opcode
        //
        EmitMarshalOpcode(psl, comToNative, localMarshaler);

        //
        // Emit Unmarshal opcode
        //
        EmitUnmarshalOpcode(pslPost, comToNative, localMarshaler);
    }
}

void MarshalInfo::GenerateReturnML(InteropStubLinker* psl,
                                   InteropStubLinker* pslPost,
                                   BOOL comToNative,
                                   BOOL retval)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    MarshalerOverrideStatus amostat;
    UINT resID = IDS_EE_BADMARSHAL_RESTRICTION;

    BOOL fEmitsIL = psl ? psl->EmitsIL() : FALSE;

    if (m_type == MARSHAL_TYPE_UNKNOWN)
    {
        amostat = HANDLEASNORMAL;
    }
    else
    {
        amostat = (GetReturnOverrideProc(m_type, fEmitsIL)) (psl,
                                                   pslPost,
                                                   comToNative,
                                                   retval,
                                                   &m_args,
                                                   &resID);
    }

    if (amostat == DISALLOWED)
    {
        EmitOrThrowInteropParamException(psl, resID, 0);
        return;
    }
        
    if (amostat == OVERRIDDEN)
    {
        _ASSERTE(NULL != psl);
        {
            if (retval)
                m_nativeArgSize = StackElemSize(sizeof(void *));

            if (GetFlags(m_type, psl->EmitsIL()) & MarshalerFlag_ReturnsComByref)
                m_comArgSize = StackElemSize(sizeof(void *));
        }
    }
    else
    {
        _ASSERTE(amostat == HANDLEASNORMAL);

        if (m_type == MARSHAL_TYPE_UNKNOWN || m_type == MARSHAL_TYPE_NATIVEARRAY)
        {
            EmitOrThrowInteropParamException(psl, m_resID, 0);
            return;
        }
    
    
        //
        // Use simple copy opcodes if possible.
        //
    
        if (m_type <= MARSHAL_TYPE_DOUBLE)
        {
            if (retval)
            {
                if (comToNative)
                {
    
                    // Calling from COM to Native: getting returnval thru buffer.
                    _ASSERTE(comToNative && retval);
    
                    int pushOpcode = ML_END;
                    int copyOpcode = 0;
    
                    switch (m_type)
                    {
                    case MARSHAL_TYPE_GENERIC_1:
                        pushOpcode = ML_PUSHRETVALBUFFER1;
                        copyOpcode = ML_COPYI1;
                        break;
    
                    case MARSHAL_TYPE_GENERIC_U1:
                        pushOpcode = ML_PUSHRETVALBUFFER1;
                        copyOpcode = ML_COPYU1;
                        break;
    
                    case MARSHAL_TYPE_GENERIC_2:
                        pushOpcode = ML_PUSHRETVALBUFFER2;
                        copyOpcode = ML_COPYI2;
                        break;
    
                    case MARSHAL_TYPE_GENERIC_U2:
                        pushOpcode = ML_PUSHRETVALBUFFER2;
                        copyOpcode = ML_COPYU2;
                        break;
    
                    case MARSHAL_TYPE_GENERIC_4:
                        pushOpcode = ML_PUSHRETVALBUFFER4;
                        copyOpcode = ML_COPY4;
                        break;
    
                    case MARSHAL_TYPE_GENERIC_8:
                        pushOpcode = ML_PUSHRETVALBUFFER8;
                        copyOpcode = ML_COPY8;
                        break;
    
                    case MARSHAL_TYPE_WINBOOL:
                        pushOpcode = ML_PUSHRETVALBUFFER4;
                        copyOpcode = ML_BOOL_N2C;
                        break;
    
                    case MARSHAL_TYPE_CBOOL:
                        pushOpcode = ML_PUSHRETVALBUFFER1;
                        copyOpcode = ML_CBOOL_N2C;
                        break;
    
    
                    default:
                        break;
                    }
    
                    if (pushOpcode != ML_END)
                    {
                        if (psl)
                        {
                        psl->MLEmit(pushOpcode);
                        UINT16 local = psl->MLNewLocal(sizeof(RetValBuffer));
    
                        pslPost->MLEmit(ML_SETSRCTOLOCAL);
                        pslPost->Emit16(local);
                        pslPost->MLEmit(copyOpcode);
                        }
    
                        m_nativeArgSize = StackElemSize(sizeof(void*));
    
                        return;
                    }
                }
                else
                {
    
                    // Calling from Native to COM: getting returnval thru buffer.
                    _ASSERTE(!comToNative && retval);
    
                    int copyOpcode = ML_END;
    
                    switch (m_type)
                    {
                        case MARSHAL_TYPE_GENERIC_4:
                            copyOpcode = ML_COPY4;
                            break;
        
                        case MARSHAL_TYPE_GENERIC_8:
                            copyOpcode = ML_COPY8;
                            break;
        
                        case MARSHAL_TYPE_FLOAT:
                            copyOpcode = ML_COPY4;
                            break;
        
                        case MARSHAL_TYPE_DOUBLE:
                            copyOpcode = ML_COPY8;
                            break;
        
                        case MARSHAL_TYPE_CBOOL:
                            copyOpcode = ML_CBOOL_C2N;
                            break;
        
                        default:
                            break;
                    }
    
                    if (copyOpcode != ML_END)
                    {
                        if (pslPost)
                        pslPost->MLEmit(copyOpcode);
    
                        m_nativeArgSize = StackElemSize(sizeof(void*));
    
                        return;
                    }
                }
            }
            else if (!retval)
            {
                // Getting return value thru eax:edx. This code path handles
                // both COM->Native && Native->COM.
                _ASSERTE(!retval);
    
                if (!psl)
                    return;
    
                switch (m_type)
                {
    
    #ifdef WRONGCALLINGCONVENTIONHACK
                    case MARSHAL_TYPE_GENERIC_1:
                        pslPost->MLEmit(comToNative ? ML_COPYI1 : ML_COPY4);
                        return;
        
                    case MARSHAL_TYPE_GENERIC_U1:
                        pslPost->MLEmit(comToNative ? ML_COPYU1 : ML_COPY4);
                        return;
        
                    case MARSHAL_TYPE_GENERIC_2:
                        pslPost->MLEmit(comToNative ? ML_COPYI2 : ML_COPY4);
                        return;
        
                    case MARSHAL_TYPE_GENERIC_U2:
                        pslPost->MLEmit(comToNative ? ML_COPYU2 : ML_COPY4);
                        return;
    
    #else
                    case MARSHAL_TYPE_GENERIC_1:
                    case MARSHAL_TYPE_GENERIC_U1:
                    case MARSHAL_TYPE_GENERIC_2:
                    case MARSHAL_TYPE_GENERIC_U2:
                        pslPost->MLEmit(ML_COPY4);
                        return;
    #endif
    
                    case MARSHAL_TYPE_WINBOOL:
                        pslPost->MLEmit(comToNative ? ML_BOOL_N2C : ML_BOOL_C2N);
                        return;
        
                    case MARSHAL_TYPE_CBOOL:
                        pslPost->MLEmit(comToNative ? ML_CBOOL_N2C : ML_CBOOL_C2N);
                        return;
        
                    case MARSHAL_TYPE_GENERIC_4:
                        pslPost->MLEmit(ML_COPY4);
                        return;
        
                    case MARSHAL_TYPE_GENERIC_8:
                        pslPost->MLEmit(ML_COPY8);
                        return;
        
                    case MARSHAL_TYPE_FLOAT:
                        pslPost->MLEmit(ML_COPY4);
                        return;
        
                    case MARSHAL_TYPE_DOUBLE:
                        pslPost->MLEmit(ML_COPY8);
                        return;

                    default:
                        break;
                }
            }
        }
    
        //
        // Compute sizes
        //
        if (retval)
            m_nativeArgSize = StackElemSize(sizeof(void *));

        if (GetFlags(m_type, fEmitsIL) & MarshalerFlag_ReturnsComByref)
            m_comArgSize = StackElemSize(sizeof(void *));
    
        if (!psl)
            return;

        //
        // Emit marshaler creation opcode
        //
        UINT16 local = EmitCreateOpcode(psl);

        //
        // Emit prereturn opcode, if necessary
        //
        EmitPreReturnOpcode(psl, comToNative, local, retval);
    
        //
        // Emit return opcode
        //
        EmitReturnOpcode(psl, pslPost, comToNative, local, retval);
    }
}

void MarshalInfo::GenerateSetterML(InteropStubLinker* psl)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(psl, NULL_OK));
    }
    CONTRACTL_END;
    
    {
        if (m_type == MARSHAL_TYPE_UNKNOWN)
        {
            EmitOrThrowInteropParamException(psl, m_resID, m_paramidx + 1); // m_paramidx is 0-based, but the user wants to see a 1-based index
            return;
        }

        if (psl)
        {
            EmitCreateOpcode(psl);
            psl->MLEmit(ML_SET_COM);
        }

        m_nativeArgSize = StackElemSize(GetNativeSize(m_type, m_ms));
    }
}

void MarshalInfo::GenerateGetterML(InteropStubLinker* psl)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(psl, NULL_OK));
    }
    CONTRACTL_END;
    
    {
        if (m_type == MARSHAL_TYPE_UNKNOWN)
        {
            EmitOrThrowInteropParamException(psl, m_resID, 0);
            return;
        }

        if (psl)
        {
            EmitCreateOpcode(psl);
            psl->MLEmit(ML_PREGET_COM_RETVAL);
        }
        m_nativeArgSize = StackElemSize(sizeof(void*));
    }
}

UINT16 MarshalInfo::EmitCreateOpcode(InteropStubLinker* psl)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    UINT16  local    = 0;
    BOOL    fEmitsIL = psl ? psl->EmitsIL() : FALSE;

    MLCode createopcode = ML_CREATE_MARSHALER_GENERIC_1 + m_type;

    // These marshalers should never be created (see RunML in ml.cpp)
    _ASSERTE(ML_CREATE_MARSHALER_ARRAYWITHOFFSET != createopcode);
    _ASSERTE(ML_CREATE_MARSHALER_BLITTABLEVALUECLASS != createopcode);
    _ASSERTE(ML_CREATE_MARSHALER_VALUECLASS != createopcode);
    _ASSERTE(ML_CREATE_MARSHALER_ARGITERATOR != createopcode);
    _ASSERTE(ML_CREATE_MARSHALER_BLITTABLEVALUECLASSWITHCOPYCTOR != createopcode);

    psl->MLEmit(createopcode);
    local = psl->MLNewLocal(GetLocalSize(m_type, fEmitsIL));

    switch (m_type)
    {
        default:
            break;

        // Fall through to include the mapping info.
        case MARSHAL_TYPE_ANSICHAR:
        case MARSHAL_TYPE_LPSTR:
        case MARSHAL_TYPE_LPSTR_BUFFER:
            psl->Emit8((UINT8) (m_BestFit));
            psl->Emit8((UINT8) (m_ThrowOnUnmappableChar));
            break;
    

        case MARSHAL_TYPE_INTERFACE:
        {
            ItfMarshalInfo itfInfo;
            GetItfMarshalInfo(TypeHandle(m_pMT), m_fDispItf, &itfInfo);
            psl->EmitPtr(itfInfo.thClass.GetMethodTable());
            psl->EmitPtr(itfInfo.thItf.GetMethodTable());
            psl->Emit8((UINT8)(itfInfo.dwFlags & ItfMarshalInfo::ITF_MARSHAL_DISP_ITF));
            psl->Emit8((UINT8)(itfInfo.dwFlags & ItfMarshalInfo::ITF_MARSHAL_CLASS_IS_HINT));
            psl->Emit8((UINT8)(itfInfo.dwFlags & ItfMarshalInfo::ITF_MARSHAL_USE_BASIC_ITF));
            break;
        }

        case MARSHAL_TYPE_NATIVEARRAY:
            ML_CREATE_MARSHALER_CARRAY_OPERANDS mops;
            GetMops(&mops);
            psl->EmitBytes((const BYTE*)&mops, sizeof(mops));
            break;

        case MARSHAL_TYPE_BLITTABLEPTR:
        case MARSHAL_TYPE_LAYOUTCLASSPTR:
        case MARSHAL_TYPE_DELEGATE:
            psl->EmitPtr(m_pMT);
            break;

        case MARSHAL_TYPE_REFERENCECUSTOMMARSHALER:
            psl->EmitPtr(m_pCMHelper);
            break;

        case MARSHAL_TYPE_UNKNOWN:
            _ASSERTE(!"This should never be hit since MARSHAL_TYPE_UNKNOWN should already have been handled by callers of EmitCreateOpcode!");
            COMPlusThrow(kMarshalDirectiveException, IDS_EE_COM_UNSUPPORTED_SIG);
    }
    
    return local;
}

void MarshalInfo::EmitMarshalOpcode(InteropStubLinker* psl, BOOL comToNative, UINT16 local)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    BYTE marshal = (comToNative ? ML_MARSHAL_C2N : ML_MARSHAL_N2C);
    if (m_byref)
        marshal += 2;

    if (!m_in)
        marshal++;

    psl->MLEmit(marshal);
}

UINT16 MarshalInfo::GetLocalSize(MarshalType mtype, BOOL useILMarshalers)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    static const BYTE localSizes[] =
    {
        #define DEFINE_MARSHALER_TYPE(mt, mclass) sizeof(mclass),
        #include "mtypes.h"
    };

    _ASSERTE(!useILMarshalers);

    return localSizes[mtype];
}

UINT16 MarshalInfo::GetComSize(MarshalType mtype, MarshalScenario ms)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    static const BYTE comSizes[]=
    {
        #define DEFINE_MARSHALER_TYPE(mt, mclass) mclass::c_CLRSize,
        #include "mtypes.h"
    };

    BYTE comSize = comSizes[mtype];

    if (comSize == VARIABLESIZE)
    {
        switch (mtype)
        {

            case MARSHAL_TYPE_BLITTABLEVALUECLASS:
            case MARSHAL_TYPE_VALUECLASS:
                return (UINT16) StackElemSize( m_pMT->GetAlignedNumInstanceFieldBytes() );
                break;

            default:
                _ASSERTE(0);
        }
    }

    return StackElemSize((UINT16)comSize);
}

UINT16 MarshalInfo::GetNativeSize(MarshalType mtype, MarshalScenario ms)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    static const BYTE nativeSizes[]=
    {
        #define DEFINE_MARSHALER_TYPE(mt, mclass) mclass::c_nativeSize,
        #include "mtypes.h"
    };

    BYTE nativeSize = nativeSizes[mtype];

    if (nativeSize == VARIABLESIZE)
    {
        switch (mtype)
        {
            case MARSHAL_TYPE_BLITTABLEVALUECLASS:
            case MARSHAL_TYPE_VALUECLASS:
            case MARSHAL_TYPE_BLITTABLEVALUECLASSWITHCOPYCTOR:
                return (UINT16) StackElemSize( m_pMT->GetNativeSize() );
                break;

            default:
                _ASSERTE(0);
        }
    }

    {
        return StackElemSize((UINT16)nativeSize);
    }
}

BYTE MarshalInfo::GetFlags(MarshalType mtype, BOOL useILMarshalers)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    static const BYTE flags[] =
    {
        #define DEFINE_MARSHALER_TYPE(mt, mclass) \
            (mclass::c_fReturnsCLRByref ? MarshalerFlag_ReturnsComByref : 0) | \
            (mclass::c_fReturnsNativeByref ? MarshalerFlag_ReturnsNativeByref : 0) | \
            (mclass::c_fInOnly ? MarshalerFlag_InOnly : 0) ,

    #include "mtypes.h"
    };

    return flags[mtype];
}

OVERRIDEPROC MarshalInfo::GetArgumentOverrideProc(MarshalType mtype, BOOL useILMarshalers)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    {
        static const OVERRIDEPROC gArgumentMLOverride[] =
        {
            #define DEFINE_MARSHALER_TYPE(mt, mclass) mclass::ArgumentMLOverride,
            #include "mtypes.h"
        };

        return gArgumentMLOverride[mtype];
    }
}

RETURNOVERRIDEPROC MarshalInfo::GetReturnOverrideProc(MarshalType mtype, BOOL useILMarshalers)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    {
        static const RETURNOVERRIDEPROC gReturnMLOverride[] =
        {
            #define DEFINE_MARSHALER_TYPE(mt, mclass) mclass::ReturnMLOverride,
            #include "mtypes.h"
        };

        return gReturnMLOverride[mtype];
    }
}

BYTE MarshalInfo::GetUnmarshalFlagsN2C(MarshalType mtype, BOOL useILMarshalers)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
#define UNMARSHAL_NATIVE_TO_COM_NEEDED(c)               \
    (NEEDS_UNMARSHAL_NATIVE_TO_CLR_IN(c)                \
     | (NEEDS_UNMARSHAL_NATIVE_TO_CLR_OUT(c) << 1)      \
     | (NEEDS_UNMARSHAL_NATIVE_TO_CLR_IN_OUT(c) << 2)   \
     | (NEEDS_UNMARSHAL_NATIVE_TO_CLR_BYREF_IN(c) << 3) \
     | (1 << 4)                                         \
     | (1 << 5))


    static const BYTE unmarshalN2CNeeded[] =
    {
        #define DEFINE_MARSHALER_TYPE(mt, mclass) UNMARSHAL_NATIVE_TO_COM_NEEDED(mclass),
        #include "mtypes.h"
    };

    return unmarshalN2CNeeded[mtype];
}

void MarshalInfo::GetItfMarshalInfo(ItfMarshalInfo* pInfo)
{
    GetItfMarshalInfo(TypeHandle(m_pMT), m_fDispItf, pInfo);
}

void MarshalInfo::GetItfMarshalInfo(TypeHandle th, BOOL fDispItf, ItfMarshalInfo *pInfo)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pInfo));
        PRECONDITION(!th.IsNull());
        PRECONDITION(th.IsUnsharedMT());
    }
    CONTRACTL_END;

    if (!th.IsInterface())
        pInfo->thClass = th;
    else
        pInfo->thItf = th;
}

HRESULT MarshalInfo::TryGetItfMarshalInfo(TypeHandle th, BOOL fDispItf, ItfMarshalInfo *pInfo)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(!th.IsNull());
        PRECONDITION(CheckPointer(pInfo));
    }
    CONTRACTL_END;

    GCX_COOP();

    HRESULT hr = S_OK;
    OBJECTREF pThrowable = NULL;

    GCPROTECT_BEGIN(pThrowable)
    {
        EX_TRY
        {
            GetItfMarshalInfo(th, fDispItf, pInfo);
        }
        EX_CATCH
        {
            pThrowable = GETTHROWABLE();
        }
        EX_END_CATCH(RethrowTerminalExceptions);

        if (pThrowable != NULL)
            hr = SetupErrorInfo(pThrowable);
    }
    GCPROTECT_END();
    
    return hr;
}

BYTE MarshalInfo::GetUnmarshalFlagsC2N(MarshalType mtype, BOOL useILMarshalers)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
#define UNMARSHAL_COM_TO_NATIVE_NEEDED(c)                \
    (NEEDS_UNMARSHAL_CLR_TO_NATIVE_IN(c)                 \
     | (NEEDS_UNMARSHAL_CLR_TO_NATIVE_OUT(c) << 1)       \
     | (NEEDS_UNMARSHAL_CLR_TO_NATIVE_IN_OUT(c) << 2)    \
     | (NEEDS_UNMARSHAL_CLR_TO_NATIVE_BYREF_IN(c) << 3)  \
     | (1 << 4)                                          \
     | (1 << 5))

    static const BYTE unmarshalC2NNeeded[] =
    {
        #define DEFINE_MARSHALER_TYPE(mt, mclass) UNMARSHAL_COM_TO_NATIVE_NEEDED(mclass),
        #include "mtypes.h"
    };

    return unmarshalC2NNeeded[mtype];
}


#ifdef _DEBUG
VOID MarshalInfo::DumpMarshalInfo(Module* pModule, SigPointer sig, mdToken token, MarshalScenario ms, BYTE nlType, BYTE nlFlags)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    SigTypeContext emptyTypeContext; // an empty type context is sufficient: all methods should be non-generic

    if (LoggingOn(LF_MARSHALER, LL_INFO10))
    {
        GCX_COOP();

        SString logbuf;
        StackScratchBuffer scratch;

        IMDInternalImport *pInternalImport = pModule->GetMDImport();

        logbuf.AppendASCII("------------------------------------------------------------\n");
        LOG((LF_MARSHALER, LL_INFO10, logbuf.GetANSI(scratch)));
        logbuf.Clear();
        
        logbuf.AppendASCII("Managed type: ");
        if (m_byref)
            logbuf.AppendASCII("Byref ");

        SigFormat sigfmt;
        TypeHandle th;

        OBJECTREF throwable = NULL;
        GCPROTECT_BEGIN(throwable);
            th = sig.GetTypeHandle(pModule, &emptyTypeContext, &throwable);
            if (throwable != NULL)
                logbuf.AppendASCII("<error>");
            else
            {
                sigfmt.AddType(th);
                logbuf.AppendUTF8(sigfmt.GetCString());
            }
        GCPROTECT_END();

        logbuf.AppendASCII("\n");
        LOG((LF_MARSHALER, LL_INFO10, logbuf.GetANSI(scratch)));
        logbuf.Clear();

        logbuf.AppendASCII("NativeType  : ");
        PCCOR_SIGNATURE pvNativeType;
        ULONG           cbNativeType;
        if (token == mdParamDefNil
            || pInternalImport->GetFieldMarshal(token,
                                                 &pvNativeType,
                                                 &cbNativeType) != S_OK)
        {
            logbuf.AppendASCII("<absent>");
        }
        else
        {

            while (cbNativeType--)
            {
                char num[100];
                sprintf_s(num, COUNTOF(num), "0x%lx ", (ULONG)*pvNativeType);
                logbuf.AppendASCII(num);
                switch (*(pvNativeType++))
                {
#define XXXXX(nt) case nt: logbuf.AppendASCII("(" #nt ")"); break;

                    XXXXX(NATIVE_TYPE_BOOLEAN)     
                    XXXXX(NATIVE_TYPE_I1)          

                    XXXXX(NATIVE_TYPE_U1)
                    XXXXX(NATIVE_TYPE_I2)          
                    XXXXX(NATIVE_TYPE_U2)          
                    XXXXX(NATIVE_TYPE_I4)          

                    XXXXX(NATIVE_TYPE_U4)
                    XXXXX(NATIVE_TYPE_I8)          
                    XXXXX(NATIVE_TYPE_U8)          
                    XXXXX(NATIVE_TYPE_R4)          

                    XXXXX(NATIVE_TYPE_R8)

                    XXXXX(NATIVE_TYPE_LPSTR)
                    XXXXX(NATIVE_TYPE_LPWSTR)      
                    XXXXX(NATIVE_TYPE_LPTSTR)      
                    XXXXX(NATIVE_TYPE_FIXEDSYSSTRING)

                    XXXXX(NATIVE_TYPE_STRUCT)      

                    XXXXX(NATIVE_TYPE_INT)         
                    XXXXX(NATIVE_TYPE_FIXEDARRAY)

                    XXXXX(NATIVE_TYPE_UINT)
                
                    XXXXX(NATIVE_TYPE_FUNC)
                
                    XXXXX(NATIVE_TYPE_ASANY)

                    XXXXX(NATIVE_TYPE_ARRAY)
                    XXXXX(NATIVE_TYPE_LPSTRUCT)

                    XXXXX(NATIVE_TYPE_IUNKNOWN)


#undef XXXXX

                    
                    case NATIVE_TYPE_CUSTOMMARSHALER:
                    {
                        int strLen = 0;
                        logbuf.AppendASCII("(NATIVE_TYPE_CUSTOMMARSHALER)");

                        // Skip the typelib guid.
                        logbuf.AppendASCII(" ");

                        strLen = CPackedLen::GetLength(pvNativeType, (void const **)&pvNativeType);
                        if (strLen)
                        {
                            BYTE* p = (BYTE*)logbuf.OpenANSIBuffer(strLen);
                            memcpyNoGCRefs(p, pvNativeType, strLen);
                            logbuf.CloseBuffer();
                            logbuf.AppendASCII("\0");

                            pvNativeType += strLen;
                            cbNativeType -= strLen + 1;

                            // Skip the name of the native type.
                            logbuf.AppendASCII(" ");
                        }
                        
                        
                        strLen = CPackedLen::GetLength(pvNativeType, (void const **)&pvNativeType);
                        if (strLen)
                        {
                            BYTE* p = (BYTE*)logbuf.OpenANSIBuffer(strLen);
                            memcpyNoGCRefs(p, pvNativeType, strLen);
                            logbuf.CloseBuffer();
                            logbuf.AppendASCII("\0");
                            
                            pvNativeType += strLen;
                            cbNativeType -= strLen + 1;
                            
                            // Extract the name of the custom marshaler.
                            logbuf.AppendASCII(" ");
                        }
                        
                        
                        strLen = CPackedLen::GetLength(pvNativeType, (void const **)&pvNativeType);
                        if (strLen)
                        {
                            BYTE* p = (BYTE*)logbuf.OpenANSIBuffer(strLen);
                            memcpyNoGCRefs(p, pvNativeType, strLen);
                            logbuf.CloseBuffer();
                            logbuf.AppendASCII("\0");
                        
                            pvNativeType += strLen;
                            cbNativeType -= strLen + 1;
        
                            // Extract the cookie string.
                            logbuf.AppendASCII(" ");
                        }
                        
                        strLen = CPackedLen::GetLength(pvNativeType, (void const **)&pvNativeType);
                        if (strLen)
                        {
                            BYTE* p = (BYTE*)logbuf.OpenANSIBuffer(strLen);
                            memcpyNoGCRefs(p, pvNativeType, strLen);
                            logbuf.CloseBuffer();
                            logbuf.AppendASCII("\0");

                            pvNativeType += strLen;
                            cbNativeType -= strLen + 1;
                        }
                        
                        break;
                    }

                    default:
                        logbuf.AppendASCII("(?)");
                }

                logbuf.AppendASCII("   ");
            }
        }
        logbuf.AppendASCII("\n");
        LOG((LF_MARSHALER, LL_INFO10, logbuf.GetANSI(scratch)));
        logbuf.Clear();

        logbuf.AppendASCII("MarshalType : ");
        {
            char num[100];
            sprintf_s(num, COUNTOF(num), "0x%lx ", (ULONG)m_type);
            logbuf.AppendASCII(num);
        }
        switch (m_type)
        {
            #define DEFINE_MARSHALER_TYPE(mt, mc) case mt: logbuf.AppendASCII( #mt " (" #mc ")"); break;
            #include "mtypes.h"
            #undef DEFINE_MARSHALER_TYPE

            case MARSHAL_TYPE_UNKNOWN:
                logbuf.AppendASCII("MARSHAL_TYPE_UNKNOWN (illegal combination)");
                break;
                
            default:
                logbuf.AppendASCII("MARSHAL_TYPE_???");
                break;
        }

        logbuf.AppendASCII("\n");


        logbuf.AppendASCII("Metadata In/Out     : ");
        if (TypeFromToken(token) != mdtParamDef || token == mdParamDefNil)
            logbuf.AppendASCII("<absent>");

        else
        {
            DWORD dwAttr = 0;
            USHORT usSequence;
            pInternalImport->GetParamDefProps(token, &usSequence, &dwAttr);
            if (IsPdIn(dwAttr))
                logbuf.AppendASCII("In ");

            if (IsPdOut(dwAttr))
                logbuf.AppendASCII("Out ");
        }

        logbuf.AppendASCII("\n");

        logbuf.AppendASCII("Effective In/Out     : ");
        if (m_in)
            logbuf.AppendASCII("In ");

        if (m_out)
            logbuf.AppendASCII("Out ");

        logbuf.AppendASCII("\n");

        LOG((LF_MARSHALER, LL_INFO10, logbuf.GetANSI(scratch)));
        logbuf.Clear();
    }
}
#endif


void MarshalInfo::EmitUnmarshalOpcode(InteropStubLinker* psl, BOOL comToNative, UINT16 localMarshaler)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    int index = 0;
    if (m_byref)
        index += 3;
    if (m_out)
    {
        index++;
        if (m_in)
            index++;
    }

    // 0: IN
    // 1: OUT
    // 2: IN_OUT
    // 3: BYREF_IN
    // 4: BYREF_OUT
    // 5: BYREF_IN_OUT

    BOOL fEmitsIL = psl ? psl->EmitsIL() : FALSE;

    if (comToNative
        ? ((GetUnmarshalFlagsC2N(m_type, fEmitsIL))&(1<<index))
        : ((GetUnmarshalFlagsN2C(m_type, fEmitsIL))&(1<<index)))
    {
        BYTE unmarshal = (comToNative ? ML_UNMARSHAL_C2N_IN : ML_UNMARSHAL_N2C_IN) + index;

        psl->MLEmit(unmarshal);
        psl->Emit16(localMarshaler);
    }
}

void MarshalInfo::EmitPreReturnOpcode(InteropStubLinker* psl, BOOL comToNative, UINT16 local, BOOL retval)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL fEmitsIL = psl ? psl->EmitsIL() : FALSE;
    
    if (retval || (GetFlags(m_type, fEmitsIL) & MarshalerFlag_ReturnsComByref))
    {
        BYTE prereturn = comToNative ? ML_PRERETURN_C2N : ML_PRERETURN_N2C;
        if (retval)
            prereturn++;

        psl->MLEmit(prereturn);
    }
}

void MarshalInfo::EmitReturnOpcode(InteropStubLinker* pslPre, InteropStubLinker* pslPost, 
    BOOL comToNative, UINT16 local, BOOL retval)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    BYTE return_ = comToNative ? ML_RETURN_C2N : ML_RETURN_N2C;
    if (retval)
        return_++;
    pslPost->MLEmit(return_);
    pslPost->Emit16(local);
}



VOID MarshalInfo::MarshalTypeToString(SString& strMarshalType, BOOL fSizeIsSpecified)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    LPWSTR strRetVal;

    // Some MarshalTypes have extra information and require special handling
    if (m_type == MARSHAL_TYPE_INTERFACE)
    {
        strRetVal = L"IUnknown";
    }
    else if (m_type == MARSHAL_TYPE_NATIVEARRAY)
    {
        SString strVarType;
        VarTypeToString(m_arrayElementType, strVarType);

        if (!fSizeIsSpecified)
        {
            strMarshalType.Printf(L"native array of %s (size not specified by a parameter)",
                                  strVarType.GetUnicode());
        }
        else
        {
            strMarshalType.Printf(L"native array of %s (size specified by parameter %i)",
                                  strVarType.GetUnicode(), m_countParamIdx);
        }

        return;
    }
    else if (m_type == MARSHAL_TYPE_REFERENCECUSTOMMARSHALER)
    {        
        GCX_COOP();
        
        OBJECTHANDLE objHandle = m_pCMHelper->GetCustomMarshalerInfo()->GetCustomMarshaler();
        {
            OBJECTREF pObjRef = ObjectFromHandle(objHandle);
            DefineFullyQualifiedNameForClassW();

            strMarshalType.Printf(L"custom marshaler (%s)",
                                  GetFullyQualifiedNameForClassW(pObjRef->GetClass()));
        }        
        
        return;
    }
    else
    {
        // All other MarshalTypes with no special handling
        switch (m_type)
        {
            case MARSHAL_TYPE_GENERIC_1:
                strRetVal = L"BYTE";
                break;
            case MARSHAL_TYPE_GENERIC_U1:
                strRetVal = L"unsigned BYTE";
                break;
            case MARSHAL_TYPE_GENERIC_2:
                strRetVal = L"WORD";
                break;
            case MARSHAL_TYPE_GENERIC_U2:
                strRetVal = L"unsigned WORD";
                break;
            case MARSHAL_TYPE_GENERIC_4:
                strRetVal = L"DWORD";
                break;
            case MARSHAL_TYPE_GENERIC_8:
                strRetVal = L"QUADWORD";
                break;
            case MARSHAL_TYPE_WINBOOL:
                strRetVal = L"Windows Bool";
                break;
            case MARSHAL_TYPE_ANSICHAR:
                strRetVal = L"Ansi character";
                break;
            case MARSHAL_TYPE_CBOOL:
                strRetVal = L"CBool";
                break;
            case MARSHAL_TYPE_FLOAT:
                strRetVal = L"float";
                break;
            case MARSHAL_TYPE_DOUBLE:
                strRetVal = L"double";
                break;
            case MARSHAL_TYPE_CURRENCY:
                strRetVal = L"CURRENCY";
                break;
            case MARSHAL_TYPE_DECIMAL:
                strRetVal = L"DECIMAL";
                break;
            case MARSHAL_TYPE_DECIMAL_PTR:
                strRetVal = L"DECIMAL pointer";
                break;
            case MARSHAL_TYPE_GUID:
                strRetVal = L"GUID";
                break;
            case MARSHAL_TYPE_GUID_PTR:
                strRetVal = L"GUID pointer";
                break;
            case MARSHAL_TYPE_DATE:
                strRetVal = L"DATE";
                break;
            case MARSHAL_TYPE_LPWSTR:
                strRetVal = L"LPWSTR";
                break;
            case MARSHAL_TYPE_LPSTR:
                strRetVal = L"LPSTR";
                break;
            case MARSHAL_TYPE_LPWSTR_BUFFER:
                strRetVal = L"LPWSTR buffer";
                break;
            case MARSHAL_TYPE_LPSTR_BUFFER:
                strRetVal = L"LPSTR buffer";
                break;
            case MARSHAL_TYPE_ASANYA:
                strRetVal = L"AsAnyA";
                break;
            case MARSHAL_TYPE_ASANYW:
                strRetVal = L"AsAnyW";
                break;
            case MARSHAL_TYPE_DELEGATE:
                strRetVal = L"Delegate";
                break;
            case MARSHAL_TYPE_BLITTABLEPTR:
                strRetVal = L"blittable pointer";
                break;
            case MARSHAL_TYPE_LAYOUTCLASSPTR:
                strRetVal = L"Layout class pointer";
                break;
            case MARSHAL_TYPE_ARRAYWITHOFFSET:
                strRetVal = L"ArrayWithOffset";
                break;
            case MARSHAL_TYPE_BLITTABLEVALUECLASS:
                strRetVal = L"blittable value class";
                break;
            case MARSHAL_TYPE_VALUECLASS:
                strRetVal = L"value class";
                break;
            case MARSHAL_TYPE_ARGITERATOR:
                strRetVal = L"ArgIterator";
                break;
            case MARSHAL_TYPE_BLITTABLEVALUECLASSWITHCOPYCTOR:
                strRetVal = L"blittable value class with copy constructor";
                break;
            case MARSHAL_TYPE_HANDLEREF:
                strRetVal = L"HandleRef";
                break;
            default:
                strRetVal = L"<UNKNOWN>";
                break;
        }
    }

    strMarshalType.Set(strRetVal);
    return;
}


VOID MarshalInfo::VarTypeToString(VARTYPE vt, SString& strVarType)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    
    LPWSTR strRetVal;
    
    switch(vt)
    {
        case VT_I2:
            strRetVal = L"2-byte signed int";
            break;
        case VT_I4:
            strRetVal = L"4-byte signed int";
            break;
        case VT_R4:
            strRetVal = L"4-byte real";
            break;
        case VT_R8:
            strRetVal = L"8-byte real";
            break;
        case VT_CY:
            strRetVal = L"currency";
            break;
        case VT_DATE:
            strRetVal = L"date";
            break;
        case VT_BSTR:
            strRetVal = L"binary string";
            break;
        case VT_DISPATCH:
            strRetVal = L"IDispatch *";
            break;
        case VT_ERROR:
            strRetVal = L"Scode";
            break;
        case VT_BOOL:
            strRetVal = L"boolean";
            break;
        case VT_VARIANT:
            strRetVal = L"VARIANT *";
            break;
        case VT_UNKNOWN:
            strRetVal = L"IUnknown *";
            break;
        case VT_DECIMAL:
            strRetVal = L"16-byte fixed point";
            break;
        case VT_RECORD:
            strRetVal = L"user defined structure";
            break;
        case VT_I1:
            strRetVal = L"signed char";
            break;
        case VT_UI1:
            strRetVal = L"unsigned char";
            break;
        case VT_UI2:
            strRetVal = L"unsigned short";
            break;
        case VT_UI4:
            strRetVal = L"unsigned short";
            break;
        case VT_INT:
            strRetVal = L"signed int";
            break;
        case VT_UINT:
            strRetVal = L"unsigned int";
            break;
        case VT_LPSTR:
            strRetVal = L"LPSTR";
            break;
        case VT_LPWSTR:
            strRetVal = L"LPWSTR";
            break;
        case VT_HRESULT:
            strRetVal = L"HResult";
            break;
        case VT_I8:
            strRetVal = L"8-byte signed int";
            break;
        case VT_NULL:
            strRetVal = L"null";
            break;
        case VT_UI8:
            strRetVal = L"8-byte unsigned int";
            break;
        case VT_VOID:
            strRetVal = L"void";
            break;
        case VTHACK_WINBOOL:
            strRetVal = L"boolean";
            break;
        case VTHACK_ANSICHAR:
            strRetVal = L"char";
            break;
        default:
            strRetVal = L"unknown";
            break;
    }

    strVarType.Set(strRetVal);    
    return;
}

#define ReportInvalidArrayMarshalInfo(resId)    \
    do                                          \
    {                                           \
        m_vtElement = VT_EMPTY;                 \
        m_errorResourceId = resId;              \
        m_thElement = TypeHandle();             \
        goto LExit;                             \
    }                                           \
    while (0)                                   

void ArrayMarshalInfo::InitForNativeArray(MarshalInfo::MarshalScenario ms, TypeHandle thElement, CorNativeType ntElement, BOOL isAnsi)
{
    WRAPPER_CONTRACT;        
    InitElementInfo(NATIVE_TYPE_ARRAY, ms, thElement, ntElement, isAnsi);
}

void ArrayMarshalInfo::InitForFixedArray(TypeHandle thElement, CorNativeType ntElement, BOOL isAnsi)
{
    WRAPPER_CONTRACT;        
    InitElementInfo(NATIVE_TYPE_FIXEDARRAY, MarshalInfo::MARSHAL_SCENARIO_FIELD, thElement, ntElement, isAnsi);
}


void ArrayMarshalInfo::InitElementInfo(CorNativeType arrayNativeType, MarshalInfo::MarshalScenario ms, TypeHandle thElement, CorNativeType ntElement, BOOL isAnsi)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(!thElement.IsNull());
        POSTCONDITION(!IsValid() || !m_thElement.IsNull());
    }
    CONTRACT_END;

    CorElementType etElement = ELEMENT_TYPE_END;    
    
    //
    // IMPORTANT: The error resource IDs used in this function must not contain any placeholders!
    // 
    // Also please maintain the standard of using IDS_EE_BADMARSHAL_XXX when defining new error
    // message resource IDs.
    //

    if (thElement.IsArray())
        ReportInvalidArrayMarshalInfo(IDS_EE_BADMARSHAL_NESTEDARRAY);

    m_thElement = thElement;

    if (m_thElement.IsPointer())
    {
        m_flags = (ArrayMarshalInfoFlags)(m_flags | amiIsPtr);
        m_thElement = ((ParamTypeDesc*)m_thElement.AsTypeDesc())->GetModifiedType();
    }
    
    etElement = m_thElement.GetSignatureCorElementType();

    if (IsAMIPtr(m_flags) && (etElement > ELEMENT_TYPE_R8))
    {
        ReportInvalidArrayMarshalInfo(IDS_EE_BADMARSHAL_UNSUPPORTED_SIG);        
    }

    if (etElement == ELEMENT_TYPE_CHAR)
    {
        switch (ntElement)
        {
            case NATIVE_TYPE_I1: //fallthru
            case NATIVE_TYPE_U1:
                m_vtElement = VTHACK_ANSICHAR;
                break;

            case NATIVE_TYPE_I2: //fallthru
            case NATIVE_TYPE_U2:
                m_vtElement = VT_UI2;
                break;

            // Compat: If the native type doesn't make sense, we need to ignore it and not report an error.
            case NATIVE_TYPE_DEFAULT: //fallthru
            default:
                    m_vtElement = isAnsi ? VTHACK_ANSICHAR : VT_UI2;                    
        }
    }
    else if (etElement == ELEMENT_TYPE_BOOLEAN)
    {
        switch (ntElement)
        {
            case NATIVE_TYPE_BOOLEAN:
                m_vtElement = VTHACK_WINBOOL;
                break;


            // Compat: if the native type doesn't make sense, we need to ignore it and not report an error.
            case NATIVE_TYPE_DEFAULT: //fallthru
            default:
                    m_vtElement = VTHACK_WINBOOL;
                break;
        }                
    }
    else if (etElement == ELEMENT_TYPE_I)
    {
        m_vtElement = (GetPointerSize() == 4) ? VT_I4 : VT_I8;
    }
    else if (etElement == ELEMENT_TYPE_U)
    {
        m_vtElement = (GetPointerSize() == 4) ? VT_UI4 : VT_UI8;
    }
    else if (etElement <= ELEMENT_TYPE_R8)
    {
        static const BYTE map [] =
        {
            VT_NULL,    // ELEMENT_TYPE_END
            VT_NULL,    // ELEMENT_TYPE_VOID
            VT_NULL,    // ELEMENT_TYPE_BOOLEAN
            VT_NULL,    // ELEMENT_TYPE_CHAR
            VT_I1,      // ELEMENT_TYPE_I1
            VT_UI1,     // ELEMENT_TYPE_U1
            VT_I2,      // ELEMENT_TYPE_I2
            VT_UI2,     // ELEMENT_TYPE_U2
            VT_I4,      // ELEMENT_TYPE_I4
            VT_UI4,     // ELEMENT_TYPE_U4
            VT_I8,      // ELEMENT_TYPE_I8
            VT_UI8,     // ELEMENT_TYPE_U8
            VT_R4,      // ELEMENT_TYPE_R4
            VT_R8       // ELEMENT_TYPE_R8

        };

        _ASSERTE(map[etElement] != VT_NULL);        
        m_vtElement = map[etElement];
    }
    else
    {
        if (m_thElement == TypeHandle(g_pStringClass))
        {           
            switch (ntElement)
            {
                case NATIVE_TYPE_DEFAULT:
                        m_vtElement = isAnsi ? VT_LPSTR : VT_LPWSTR;
                    break;
                case NATIVE_TYPE_LPSTR:
                    m_vtElement = VT_LPSTR;
                    break;
                case NATIVE_TYPE_LPWSTR:
                    m_vtElement = VT_LPWSTR;
                    break;
                case NATIVE_TYPE_LPTSTR:
                {
                    {
                        static BOOL init = FALSE;
                        static BOOL onUnicode;
            
                        if (!init)
                        {
                            onUnicode = NDirectOnUnicodeSystem();
                            init = TRUE;
                        }
            
                        if (onUnicode)
                            m_vtElement = VT_LPWSTR;
                        else
                            m_vtElement = VT_LPSTR;
                    }
                    break;
                }

                default:
                    ReportInvalidArrayMarshalInfo(IDS_EE_BADMARSHAL_STRINGARRAY);
            }
        }
        else if (m_thElement == TypeHandle(g_pObjectClass))
        {
            switch (ntElement)
            {
                case NATIVE_TYPE_IUNKNOWN:
                    m_vtElement = VT_UNKNOWN;
                    break;

                default:
                    ReportInvalidArrayMarshalInfo(IDS_EE_BADMARSHAL_UNSUPPORTED_SIG);
            }
        }
        else if (m_thElement.CanCastTo(TypeHandle(g_Mscorlib.GetClass(CLASS__SAFE_HANDLE))))
        {
            // Array's of SAFEHANDLEs are not supported.
            ReportInvalidArrayMarshalInfo(IDS_EE_BADMARSHAL_SAFEHANDLEARRAY);
        }
        else if (m_thElement.CanCastTo(TypeHandle(g_Mscorlib.GetClass(CLASS__CRITICAL_HANDLE))))
        {
            // Array's of CRITICALHANDLEs are not supported.
            ReportInvalidArrayMarshalInfo(IDS_EE_BADMARSHAL_CRITICALHANDLEARRAY);
        }
        else if (etElement == ELEMENT_TYPE_VALUETYPE) 
        {
            if (m_thElement == TypeHandle(g_Mscorlib.GetClass(CLASS__DATE_TIME)))
            {
                if (ntElement == NATIVE_TYPE_STRUCT || ntElement == NATIVE_TYPE_DEFAULT)
                    m_vtElement = VT_DATE;
                else
                    ReportInvalidArrayMarshalInfo(IDS_EE_BADMARSHAL_DATETIMEARRAY);
            }
            else if (m_thElement == TypeHandle(g_Mscorlib.GetClass(CLASS__DECIMAL)))
            {
                if (ntElement == NATIVE_TYPE_STRUCT || ntElement == NATIVE_TYPE_DEFAULT)
                    m_vtElement = VT_DECIMAL;
                else
                    ReportInvalidArrayMarshalInfo(IDS_EE_BADMARSHAL_DECIMALARRAY);                
            }
            else
            {
                // When exporting, we need to handle enums specially.
                if (IsAMIExport(m_flags) && m_thElement.IsEnum())
                {
                    // Get the element type of the underlying type.
                    CorElementType et = m_thElement.GetInternalCorElementType();
                    
                    // If it is not a 32-bit type, convert as the underlying type.
                    if ((et == ELEMENT_TYPE_I4) || (et == ELEMENT_TYPE_U4))
                        m_vtElement = VT_RECORD;             
                    else
                        m_vtElement = OleVariant::GetVarTypeForTypeHandle(m_thElement);
                }             
                else
                {                   
                    m_vtElement = OleVariant::GetVarTypeForTypeHandle(m_thElement);
                }
            }
        }
        else
        {
            ReportInvalidArrayMarshalInfo(IDS_EE_BADMARSHAL_UNSUPPORTED_SIG);
        }
    }


    // If we are exporting, we need to substitute the VTHACK_* VARTYPE with the actual
    // types as expressed in the type library.
    if (IsAMIExport(m_flags))
    {
        if (m_vtElement == VTHACK_ANSICHAR)
            m_vtElement = VT_UI1;
        else if (m_vtElement == VTHACK_WINBOOL)
            m_vtElement = VT_I4;
    }

LExit:;
    
    RETURN;
}

bool IsUnsupportedValueTypeReturn(MetaSig& msig)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END
    
    CorElementType type = msig.GetReturnTypeNormalized();
    
    if (type == ELEMENT_TYPE_VALUETYPE || type == ELEMENT_TYPE_TYPEDBYREF)
    {
        return true;
    }

    return false;
}

