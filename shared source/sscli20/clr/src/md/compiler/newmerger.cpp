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
//*****************************************************************************
// NewMerger.cpp
//
// contains utility code to MD directory
//
//*****************************************************************************
#include "stdafx.h"
#include "newmerger.h"
#include "regmeta.h"
#include "importhelper.h"
#include "rwutil.h"
#include "mdlog.h"
#include <posterror.h>
#include <sstring.h>
#include "ndpversion.h"


#define MODULEDEFTOKEN         TokenFromRid(1, mdtModule)
#define COR_COMPILERSERVICE_NAMESPACE "System.Runtime.CompilerServices"
#define COR_SUPPRESS_MERGE_CHECK_ATTRIBUTE "SuppressMergeCheckAttribute"
#define COR_MISCBITS_ATTRIBUTE "Microsoft.VisualC.MiscellaneousBitsAttribute"

#define COR_SECURITYCRITICAL_ATTRIBUTE_FULL "System.Security.SecurityCriticalAttribute"

#define COR_SECURITYCRITICAL_ATTRIBUTE_FULL_W L"System.Security.SecurityCriticalAttribute"
#define COR_SECURITYTREATASSAFE_ATTRIBUTE_FULL_W L"System.Security.SecurityTreatAsSafeAttribute"

#define COR_SECURITYCRITICAL_ATTRIBUTE_NAMESPACE "System.Security"
#define COR_SECURITYCRITICAL_ATTRIBUTE "SecurityCriticalAttribute" 
#define COR_SECURITYTREATASSAFE_ATTRIBUTE_NAMESPACE "System.Security"
#define COR_SECURITYTREATASSAFE_ATTRIBUTE "SecurityTreatAsSafeAttribute"


static BOOL g_fRefShouldMergeCriticalChecked = FALSE;
static BOOL g_fRefShouldMergeCritical = TRUE;

// check if input scope has an assembly level security critical attribute
BOOL NEWMERGER::CheckInputScopeIsCritical(MergeImportData* pImportData)
{
    mdTypeRef fakeModuleTypeRef = mdTokenNil;
    mdAssemblyRef tkMscorlib = mdTokenNil;

    if (g_fRefShouldMergeCriticalChecked == FALSE)
    {
        // shouldn't require thread safety lock
        g_fRefShouldMergeCritical = (REGUTIL::GetConfigDWORD(L"MergeCriticalAttributes", 1) != 0);
        g_fRefShouldMergeCriticalChecked = TRUE;
    }

    // return no merge needed, if the merge critical attribute setting is not enabled.
    if (!g_fRefShouldMergeCritical) return FALSE;
    
     // get typeref for mscorlib
    BYTE pbMscorlibToken[] = {0xb7, 0x7a, 0x5c, 0x56,0x19,0x34,0xe0,0x89};
    // 
    if (S_OK != ImportHelper::FindAssemblyRef(&pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd,
                                                "mscorlib",
                                                NULL,
                                                pbMscorlibToken,
                                                sizeof(pbMscorlibToken),
                                                asm_rmj,
                                                asm_rmm,
                                                asm_rup,
                                                asm_rpt,
                                                0,
                                                &tkMscorlib))
    {
        // there isn't an mscorlib ref here... we can't have the security critical attribute
        pImportData->m_fContainsSecurityCriticalAttribute = FALSE;
        return FALSE;
    }

    if (S_OK != ImportHelper::FindTypeRefByName(&pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd,
                                            tkMscorlib,
                                            "System.Runtime.CompilerServices",
                                            "AssemblyAttributesGoHere",
                                            &fakeModuleTypeRef))
    {
        // for now let use the fake module ref as the assembly def
        fakeModuleTypeRef = 0x000001;
    }

    // See if the magical security attribute is set in this scope
    if (S_OK == ImportHelper::GetCustomAttributeByName(&pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd,
                                                                                            fakeModuleTypeRef, // This is the assembly def token
                                                                                            COR_SECURITYCRITICAL_ATTRIBUTE_FULL,
                                                                                            NULL,
                                                                                            NULL))
    {
        pImportData->m_fContainsSecurityCriticalAttribute = TRUE;
    }
    else
        pImportData->m_fContainsSecurityCriticalAttribute = FALSE;
    return pImportData->m_fContainsSecurityCriticalAttribute;
}


HRESULT NEWMERGER::MergeSecurityCriticalAttributes()
{    
    CMiniMdRW* emit = GetMiniMdEmit();
    HRESULT hr = S_OK;
    
    // The attribute we want to decorate all of the types with has not been defined.
    mdMemberRef tkSecurityCriticalAttribute = mdTokenNil;
    mdMemberRef tkSecurityTreatAsSafeAttribute = mdTokenNil;

    // get typeref for mscorlib
    BYTE pbMscorlibToken[] = {0xb7, 0x7a, 0x5c, 0x56,0x19,0x34,0xe0,0x89};
    mdAssemblyRef tkMscorlib = mdTokenNil;
    
    if (FAILED(hr = ImportHelper::FindAssemblyRef(emit,
                                            "mscorlib",
                                            NULL,
                                            pbMscorlibToken,
                                            sizeof(pbMscorlibToken),
                                            asm_rmj,
                                            asm_rmm,
                                            asm_rup,
                                            asm_rpt,
                                            0,
                                            &tkMscorlib)))
    {
        _ASSERTE(!"mscorlib not resolved");
        // There isn't an assembly ref for mscorlib in the emitted assembly. This shouldn't happen...
        return hr;
    }

    // Find/define the security enum
    mdTypeRef securityEnum = mdTokenNil;

    if (FAILED(hr = m_pRegMetaEmit->DefineTypeRefByName(tkMscorlib,
                                    L"System.Security.SecurityCriticalScope",
                                    &securityEnum)))
    {
        _ASSERTE(!"Couldn't Emit a Typeref for SecurityCriticalScope");
        return hr;
    }

    mdTypeRef tkSecurityCriticalAttributeType = mdTokenNil;                                                    
    if (FAILED(hr = m_pRegMetaEmit->DefineTypeRefByName(tkMscorlib, COR_SECURITYCRITICAL_ATTRIBUTE_FULL_W, &tkSecurityCriticalAttributeType)))
    {
        _ASSERTE(!"Couldn't Emit a Typeref for SecurityCriticalAttribute");
        return hr;
    }
    // use the constructor that takes SecurityCriticalScope argument
    // Build the sig.

    DWORD dwMaxSigSize = 5 + sizeof(mdTypeRef) * 1;
    BYTE* rgSigBytesSecurityCriticalCtor = (COR_SIGNATURE*)_alloca(dwMaxSigSize);
    BYTE* pCurr = rgSigBytesSecurityCriticalCtor;
    *pCurr++ = IMAGE_CEE_CS_CALLCONV_DEFAULT_HASTHIS;
    *pCurr++ = 1; // one argument to constructor
    *pCurr++ = ELEMENT_TYPE_VOID;
    *pCurr++ = ELEMENT_TYPE_CLASS;
    pCurr += CorSigCompressToken(securityEnum, pCurr);
    DWORD dwSigSize = (DWORD)(pCurr - rgSigBytesSecurityCriticalCtor);
    _ASSERTE(dwSigSize <= dwMaxSigSize);

    if (FAILED(hr = m_pRegMetaEmit->DefineMemberRef(tkSecurityCriticalAttributeType, 
                                         L".ctor", 
                                         rgSigBytesSecurityCriticalCtor, 
                                         dwSigSize, 
                                         &tkSecurityCriticalAttribute)))
    {
        _ASSERTE(!"Couldn't Emit a MemberRef for SecurityCriticalAttribute .ctor");
        return hr;
    }
                                                                  
    mdTypeRef tkSecurityTreatAsSafeAttributeType = mdTokenNil;                                                      
    if (FAILED (hr = m_pRegMetaEmit->DefineTypeRefByName(tkMscorlib, COR_SECURITYTREATASSAFE_ATTRIBUTE_FULL_W, 
                                            &tkSecurityTreatAsSafeAttributeType)))
    {
        _ASSERTE(!"Couldn't Emit a Typeref for TreatAsSafe attribute");
        return hr;
    }
    
    BYTE rgSigBytesTreatAsSafeCtor[] = {IMAGE_CEE_CS_CALLCONV_DEFAULT_HASTHIS, 0x00, ELEMENT_TYPE_VOID};
    if (FAILED(hr = m_pRegMetaEmit->DefineMemberRef(tkSecurityTreatAsSafeAttributeType,
                                                                  L".ctor",
                                                                  rgSigBytesTreatAsSafeCtor, 
                                                                  sizeof(rgSigBytesTreatAsSafeCtor),
                                                                  &tkSecurityTreatAsSafeAttribute)))
    {
        _ASSERTE(!"Couldn't Emit a MemberRef for TreatAsSafe attribute .ctor");
        return hr;
    }
                                                                          
    for (MergeImportData* pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // ignore input scopes that are already marked critical
        if (pImportData->m_fContainsSecurityCriticalAttribute) continue;

        // Run through the scopes that need to have their types decorated with this attribute
        MDTOKENMAP*pCurTKMap = pImportData->m_pMDTokenMap;
        BYTE rgSecurityCriticalCtorValue[] = { 0x01, 0x00 ,0x01, 0x00, 0x00, 0x00 ,0x00, 0x00 };
        BYTE rgTreatAsSafeCtorValue[] = {0x01, 0x00, 0x00 ,0x00};
        
        for (int i = 0; i < pCurTKMap->Count(); i++)
        {
            TOKENREC* pRec = pCurTKMap->Get(i);            
            BOOL fInjectSecurityAttributes = FALSE;            

            // skip empty records
            if (pRec->IsEmpty()) continue;
           
            // check for typedefs, but don't put this on the global typedef
            if ((TypeFromToken(pRec->m_tkTo) == mdtTypeDef) && (pRec->m_tkTo != TokenFromRid(1, mdtTypeDef)))
            {
                // by default we will inject
                fInjectSecurityAttributes = TRUE;
                // except for Enums
                DWORD           dwClassAttrs = 0;
                mdTypeRef       crExtends = mdTokenNil;

                if (FAILED(hr = m_pRegMetaEmit->GetTypeDefProps(pRec->m_tkTo, NULL, NULL, 0 , &dwClassAttrs, &crExtends)))
                {
                }

                // check for Enum types                                                                                                
                if (!IsNilToken(crExtends) && (TypeFromToken(crExtends)==mdtTypeRef))
                {
                    // get the namespace and the name for this token
                    CMiniMdRW   *pMiniMd = GetMiniMdEmit();
                    TypeRefRec  *pTypeRefRec = pMiniMd->getTypeRef(RidFromToken(crExtends));                      
                    LPCSTR  szNamespace = pMiniMd->getNamespaceOfTypeRef(pTypeRefRec);;
                    LPCSTR  szName = pMiniMd->getNameOfTypeRef(pTypeRefRec);
                        // check for System.Enum
                    BOOL bIsEnum = (!strcmp(szNamespace,"System"))&&(!strcmp(szName,"Enum"));
                    if (bIsEnum)
                    {
                        fInjectSecurityAttributes = FALSE;
                    }                                
                }
            }
            else // check for global method defs
            if (TypeFromToken(pRec->m_tkTo) == mdtMethodDef)
            {
                int isGlobal = 0;                                
                if (!FAILED(m_pRegMetaEmit->IsGlobal(pRec->m_tkTo, &isGlobal)))
                {
                    // check for global methods
                    if (isGlobal != 0)
                    {
                        fInjectSecurityAttributes = TRUE;
                    }
                }
            }
            
            if (fInjectSecurityAttributes)
            {    
                // check to see if the token already has a custom attribute
                const void  *pData = NULL;               // [OUT] Put pointer to data here.
                ULONG       cbData = 0;
                // check for dupes??
                if (S_OK != m_pRegMetaEmit->GetCustomAttributeByName(pRec->m_tkTo, COR_SECURITYCRITICAL_ATTRIBUTE_FULL_W, &pData, &cbData))
                {
                
                    hr = m_pRegMetaEmit->DefineCustomAttribute(pRec->m_tkTo, tkSecurityCriticalAttribute,
                                rgSecurityCriticalCtorValue, // Use this if you need specific custom attribute data (presence of the attribute isn't enough)
                                                     sizeof(rgSecurityCriticalCtorValue), // Length of your custom attribute data
                                NULL);
                    hr = m_pRegMetaEmit->DefineCustomAttribute(pRec->m_tkTo, tkSecurityTreatAsSafeAttribute,
                                rgTreatAsSafeCtorValue, // Use this if you need specific custom attribute data (presence of the attribute isn't enough)
                                                    sizeof(rgTreatAsSafeCtorValue), // Length of your custom attribute data
                                NULL);
                }
            }
        }
    }

    return hr;
}


//*****************************************************************************
// Checks to see if the given type is managed or native. We'll key off of the
// Custom Attribute "Microsoft.VisualC.MiscellaneousBitsAttribute". If the third
// byte has the 01000000 bit set then it is an unmanaged type
//*****************************************************************************
HRESULT IsManagedType(CMiniMdRW* pMiniMd,
                          mdTypeDef td,
                          BOOL *fIsManagedType)
{
    // First look for the custom attribute
    HENUMInternal hEnum;
    HRESULT hr = S_OK;
    
    IfFailRet(pMiniMd->CommonEnumCustomAttributeByName(td, COR_MISCBITS_ATTRIBUTE, false, &hEnum));

    // If there aren't any custom attributes here, then this must be a managed type
    if (hEnum.m_ulCount > 0)
    {
        // Let's loop through these, and see if any of them have that magical bit set.
        mdCustomAttribute ca;
        CustomAttributeRec *pRec;
        ULONG cbData = 0;
        
        while(HENUMInternal::EnumNext(&hEnum, &ca))
        {
            const BYTE* pData = NULL;
            
            pRec = pMiniMd->getCustomAttribute(RidFromToken(ca));
            pData = pMiniMd->getValueOfCustomAttribute(pRec, &cbData);

            if (pData != NULL && cbData >=3)
            {
                // See if the magical bit is set to make this an unmanaged type
                if ((*(pData+2)&0x40) > 0)
                {
                    // Yes, this is an unmanaged type
                    HENUMInternal::ClearEnum(&hEnum);
                    *fIsManagedType = FALSE;
                    return S_OK;
                }
            }
        }
                
    }

    // Nope, this isn't an unmanaged type.... must be managed
    HENUMInternal::ClearEnum(&hEnum);
    *fIsManagedType = TRUE;
    return S_OK;
}// IsManagedType


//*****************************************************************************
// "Is CustomAttribute from certain namespace and assembly" check helper
// Returns ptr to TypeRefRec if it is, or NULL otherwise
//*****************************************************************************
TypeRefRec* IsAttributeFromNamespace(CMiniMdRW       *pMiniMd,
                                            mdToken         tk,
                                            LPUTF8          szNamespace,
                                            LPUTF8          szAssembly)
{
    if(TypeFromToken(tk) == mdtMemberRef)
    {
        MemberRefRec    *pMemRefRec = pMiniMd->getMemberRef(RidFromToken(tk));
        tk = pMiniMd->getClassOfMemberRef(pMemRefRec);
    }
    if(TypeFromToken(tk) == mdtTypeRef)
    {
        TypeRefRec      *pTypeRefRec = pMiniMd->getTypeRef(RidFromToken(tk));
        if (strcmp(pMiniMd->getNamespaceOfTypeRef(pTypeRefRec), szNamespace) == 0)
        {
            mdToken tkResTmp = pMiniMd->getResolutionScopeOfTypeRef(pTypeRefRec);
            if (TypeFromToken(tkResTmp) == mdtAssemblyRef)
            {
                AssemblyRefRec  *pAsmRefRec = pMiniMd->getAssemblyRef(RidFromToken(tkResTmp));
                if(SString::_stricmp(pMiniMd->getNameOfAssemblyRef(pAsmRefRec), szAssembly) == 0)
                    return pTypeRefRec;
            }
        }
    }
    return NULL;
}


CMiniMdRW *NEWMERGER::GetMiniMdEmit() 
{
    return &(m_pRegMetaEmit->m_pStgdb->m_MiniMd); 
} // CMiniMdRW *NEWMERGER::GetMiniMdEmit()


//*****************************************************************************
// constructor
//*****************************************************************************
NEWMERGER::NEWMERGER()
 :  m_pRegMetaEmit(0),
    m_pImportDataList(NULL),
    m_optimizeRefToDef(MDRefToDefDefault),
    m_fMergingSecurityCriticalAttribute(FALSE)
{
    m_pImportDataTail = &(m_pImportDataList);
#if _DEBUG
    m_iImport = 0;
#endif // _DEBUG
} // NEWMERGER::NEWMERGER()


//*****************************************************************************
// initializer
//*****************************************************************************
HRESULT NEWMERGER::Init(RegMeta *pRegMeta) 
{
    HRESULT hr = NOERROR;
    MergeTypeData * pMTD;

    m_pRegMetaEmit = pRegMeta;

    // burn an entry so that the RID matches the array index
    IfNullGo(pMTD = m_rMTDs.Append());

    pMTD->m_bSuppressMergeCheck = false;
    pMTD->m_cMethods = 0;
    pMTD->m_cFields = 0;
    pMTD->m_cEvents = 0;
    pMTD->m_cProperties = 0;

ErrExit:
    return hr;
} // HRESULT NEWMERGER::Init()


//*****************************************************************************
// destructor
//*****************************************************************************
NEWMERGER::~NEWMERGER()
{
    if (m_pImportDataList)
    {
        // delete this list and release all AddRef'ed interfaces!
        MergeImportData *pNext;
        for (pNext = m_pImportDataList; pNext != NULL; )
        {
            pNext = m_pImportDataList->m_pNextImportData;
            if (m_pImportDataList->m_pHandler)
                m_pImportDataList->m_pHandler->Release();
            if (m_pImportDataList->m_pHostMapToken)
                m_pImportDataList->m_pHostMapToken->Release();
            if (m_pImportDataList->m_pError)
                m_pImportDataList->m_pError->Release();
            if (m_pImportDataList->m_pMDTokenMap)
                delete m_pImportDataList->m_pMDTokenMap;
            m_pImportDataList->m_pRegMetaImport->Release();
            delete m_pImportDataList;
            m_pImportDataList = pNext;
        }
    }
} // NEWMERGER::~NEWMERGER()


//*****************************************************************************
// Adding a new import
//*****************************************************************************
HRESULT NEWMERGER::AddImport(
    IMetaDataImport2 *pImport,              // [IN] The scope to be merged.
    IMapToken   *pHostMapToken,             // [IN] Host IMapToken interface to receive token remap notification
    IUnknown    *pHandler)                  // [IN] An object to receive error notification.
{
    HRESULT             hr = NOERROR;
    MergeImportData     *pData;

    RegMeta     *pRM = static_cast<RegMeta*>(pImport);

    // Add a MergeImportData to track the information for this import scope
    pData = new (nothrow) MergeImportData;
    IfNullGo( pData );
    pData->m_pRegMetaImport = pRM;
    pData->m_pRegMetaImport->AddRef();
    pData->m_pHostMapToken = pHostMapToken;
    if (pData->m_pHostMapToken)
        pData->m_pHostMapToken->AddRef();
    if (pHandler)
    {
        pData->m_pHandler = pHandler;
        pData->m_pHandler->AddRef();
    }
    else
    {
        pData->m_pHandler = NULL;
    }

    // don't query for IMetaDataError until we need one.
    pData->m_pError = NULL;
    pData->m_pMDTokenMap = NULL;
    pData->m_pNextImportData = NULL;
#if _DEBUG
    pData->m_iImport = ++m_iImport;
#endif // _DEBUG

    // add the newly create node to the tail of the list
    *m_pImportDataTail = pData;
    m_pImportDataTail = &(pData->m_pNextImportData);

ErrExit:

    return hr;
} // HRESULT NEWMERGER::AddImport()

HRESULT NEWMERGER::InitMergeTypeData() 
{
    CMiniMdRW   *pMiniMdEmit;
    ULONG       cTypeDefRecs;
    ULONG       i, j;
    bool        bSuppressMergeCheck;

    ULONG       ridStart, ridEnd;
    RID         ridMap;

    mdToken     tkSuppressMergeCheckCtor = mdTokenNil;
    mdToken     tkCA;
    mdMethodDef mdEmit;
    mdFieldDef  fdEmit;
    mdEvent     evEmit;
    mdProperty  prEmit;

    TypeDefRec  *pTypeDefRec;
    EventMapRec  *pEventMapRec;
    PropertyMapRec *pPropertyMapRec;

    MergeTypeData *pMTD;

    HRESULT     hr = NOERROR;

    pMiniMdEmit = GetMiniMdEmit();

    // cache the SuppressMergeCheckAttribute.ctor token
    ImportHelper::FindCustomAttributeCtorByName(
            pMiniMdEmit, "mscorlib", 
            COR_COMPILERSERVICE_NAMESPACE, COR_SUPPRESS_MERGE_CHECK_ATTRIBUTE, 
            &tkSuppressMergeCheckCtor);

    cTypeDefRecs = pMiniMdEmit->getCountTypeDefs();
    _ASSERTE(m_rMTDs.Count() > 0);  

    for (i = m_rMTDs.Count(); i <= cTypeDefRecs; i++)
    { 
        IfNullGo(pMTD = m_rMTDs.Append());

        pMTD->m_cMethods = 0;
        pMTD->m_cFields = 0;
        pMTD->m_cEvents = 0;
        pMTD->m_cProperties = 0;
        pMTD->m_bSuppressMergeCheck = (tkSuppressMergeCheckCtor != mdTokenNil) &&
            (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdEmit,
                TokenFromRid(i, mdtTypeDef), tkSuppressMergeCheckCtor,
                NULL, 0, &tkCA));

        pTypeDefRec = pMiniMdEmit->getTypeDef(i);

        // Count the number methods
        ridStart = pMiniMdEmit->getMethodListOfTypeDef(pTypeDefRec);
        ridEnd = pMiniMdEmit->getEndMethodListOfTypeDef(pTypeDefRec);

        for (j = ridStart; j < ridEnd; j++)
        {
            mdEmit = pMiniMdEmit->GetMethodRid(j);
            bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
               ((tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdEmit,
                    mdEmit, tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

            if (!bSuppressMergeCheck) 
            {
                pMTD->m_cMethods++;
            }
        }

        // Count the number fields
        ridStart = pMiniMdEmit->getFieldListOfTypeDef(pTypeDefRec);
        ridEnd = pMiniMdEmit->getEndFieldListOfTypeDef(pTypeDefRec);

        for (j = ridStart; j < ridEnd; j++)
        {
            fdEmit = pMiniMdEmit->GetMethodRid(j);
            bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
               ((tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdEmit,
                    fdEmit, tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

            if (!bSuppressMergeCheck) 
            {
                pMTD->m_cFields++;
            }
        }

        // Count the number of events
        ridMap = pMiniMdEmit->FindEventMapFor(i);
        if (!InvalidRid(ridMap)) 
        {
            pEventMapRec = pMiniMdEmit->getEventMap(ridMap);
            ridStart = pMiniMdEmit->getEventListOfEventMap(pEventMapRec);
            ridEnd = pMiniMdEmit->getEndEventListOfEventMap(pEventMapRec);

            for (j = ridStart; j < ridEnd; j++)
            {
                evEmit = pMiniMdEmit->GetEventRid(j);
                bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
                    ((tkSuppressMergeCheckCtor != mdTokenNil) &&
                    (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdEmit,
                        evEmit, tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

                if (!bSuppressMergeCheck) 
                {
                    pMTD->m_cEvents++;
                }
            }
        }

        // Count the number of properties
        ridMap = pMiniMdEmit->FindPropertyMapFor(i);
        if (!InvalidRid(ridMap)) 
        {
            pPropertyMapRec = pMiniMdEmit->getPropertyMap(ridMap);
            ridStart = pMiniMdEmit->getPropertyListOfPropertyMap(pPropertyMapRec);
            ridEnd = pMiniMdEmit->getEndPropertyListOfPropertyMap(pPropertyMapRec);

            for (j = ridStart; j < ridEnd; j++)
            {
                prEmit = pMiniMdEmit->GetPropertyRid(j);
                bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
                    ((tkSuppressMergeCheckCtor != mdTokenNil) &&
                    (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdEmit,
                        prEmit, tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

                if (!bSuppressMergeCheck) 
                {
                    pMTD->m_cProperties++;
                }
            }
        }
    }

ErrExit:
    return hr;
}

//*****************************************************************************
// Merge now
//*****************************************************************************
HRESULT NEWMERGER::Merge(MergeFlags dwMergeFlags, CorRefToDefCheck optimizeRefToDef)
{
    MergeImportData     *pImportData = m_pImportDataList;
    MDTOKENMAP          **pPrevMap = NULL;
    MDTOKENMAP          *pMDTokenMap;
    HRESULT             hr = NOERROR;
    MDTOKENMAP          *pCurTKMap;
    int                 i;

#if _DEBUG
    {
    LOG((LOGMD, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
    LOG((LOGMD, "Merge scope list\n"));
    i = 0;
    for (MergeImportData *pID = m_pImportDataList; pID != NULL; pID = pID->m_pNextImportData)
    {
        WCHAR szScope[1024], szGuid[40];
        GUID mvid;
        ULONG cchScope;
        pID->m_pRegMetaImport->GetScopeProps(szScope, 1024, &cchScope, &mvid);
        szScope[1023] = 0;
        GuidToLPWSTR(mvid, szGuid, 40);
        ++i; // Counter is 1-based.
        LOG((LOGMD, "%3d: %ls : %ls\n", i, szGuid, szScope));
    }
    LOG((LOGMD, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
    }
#endif // _DEBUG
    
    m_dwMergeFlags = dwMergeFlags;
    m_optimizeRefToDef = optimizeRefToDef;

    // check to see if we need to do dup check
    m_fDupCheck = ((m_dwMergeFlags & NoDupCheck) != NoDupCheck);

    while (pImportData)
    {
        // Verify that we have a filter for each import scope.
        IfNullGo( pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd.GetFilterTable() );

        // cache the SuppressMergeCheckAttribute.ctor token for each import scope
        ImportHelper::FindCustomAttributeCtorByName(
                &pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd, "mscorlib", 
                COR_COMPILERSERVICE_NAMESPACE, COR_SUPPRESS_MERGE_CHECK_ATTRIBUTE, 
                &pImportData->m_tkSuppressMergeCheckCtor);

        // check for security critical attribute in the assembly
        if (CheckInputScopeIsCritical(pImportData))
       {
            pImportData->m_fContainsSecurityCriticalAttribute = TRUE;
            m_fMergingSecurityCriticalAttribute = TRUE;
        }
        else
            pImportData->m_fContainsSecurityCriticalAttribute = FALSE;       
        
        // create the tokenmap class to track metadata token remap for each import scope
        pMDTokenMap = new (nothrow) MDTOKENMAP;
        IfNullGo(pMDTokenMap);
        IfFailGo(pMDTokenMap->Init((IMetaDataImport2*)pImportData->m_pRegMetaImport));
        pImportData->m_pMDTokenMap = pMDTokenMap;
        pImportData->m_pMDTokenMap->m_pMap = pImportData->m_pHostMapToken;
        if (pImportData->m_pHostMapToken)
            pImportData->m_pHostMapToken->AddRef();
        pImportData->m_pMDTokenMap->m_pNextMap = NULL;
        if (pPrevMap)
            *pPrevMap = pImportData->m_pMDTokenMap;
        pPrevMap = &(pImportData->m_pMDTokenMap->m_pNextMap);
        pImportData = pImportData->m_pNextImportData;
    }

    // Populate the m_rMTDs with the type info already defined in the emit scope
    IfFailGo( InitMergeTypeData() );

    // 1. Merge Module
    IfFailGo( MergeModule( ) );

    // 2. Merge TypeDef partially (i.e. only name)
    IfFailGo( MergeTypeDefNamesOnly() );

    // 3. Merge ModuleRef property and do ModuleRef to ModuleDef optimization
    IfFailGo( MergeModuleRefs() );

    // 4. Merge AssemblyRef. 
    IfFailGo( MergeAssemblyRefs() );

    // 5. Merge TypeRef with TypeRef to TypeDef optimization
    IfFailGo( MergeTypeRefs() );

    // 6. Merge TypeSpec & MethodSpec
    IfFailGo( MergeTypeSpecs() );

    // 7. Now Merge the remaining of TypeDef records
    IfFailGo( CompleteMergeTypeDefs() );

    // 8. Merge Methods and Fields. Such that Signature translation is respecting the TypeRef to TypeDef optimization.
    IfFailGo( MergeTypeDefChildren() );

    // 9. Merge MemberRef with MemberRef to MethodDef/FieldDef optimization
    IfFailGo( MergeMemberRefs( ) );

    // 10. Merge InterfaceImpl
    IfFailGo( MergeInterfaceImpls( ) );

    // merge all of the remaining in metadata ....

    // 11. constant has dependency on property, field, param
    IfFailGo( MergeConstants() );

    // 12. field marshal has dependency on param and field
    IfFailGo( MergeFieldMarshals() );

    // 13. in ClassLayout, move over the FieldLayout and deal with FieldLayout as well
    IfFailGo( MergeClassLayouts() );

    // 14. FieldLayout has dependency on FieldDef.
    IfFailGo( MergeFieldLayouts() );

    // 15. FieldRVA has dependency on FieldDef.
    IfFailGo( MergeFieldRVAs() );
        
    // 16. MethodImpl has dependency on MemberRef, MethodDef, TypeRef and TypeDef.
    IfFailGo( MergeMethodImpls() );

    // 17. pinvoke depends on MethodDef and ModuleRef
    IfFailGo( MergePinvoke() );

    IfFailGo( MergeStandAloneSigs() );

    IfFailGo( MergeMethodSpecs() );

    IfFailGo( MergeStrings() );

    if (m_dwMergeFlags & MergeManifest)
    {
        // keep the manifest!!
        IfFailGo( MergeAssembly() );
        IfFailGo( MergeFiles() );
        IfFailGo( MergeExportedTypes() );
        IfFailGo( MergeManifestResources() );
    }
    else if (m_dwMergeFlags & ::MergeExportedTypes)
    {
        IfFailGo( MergeFiles() );
        IfFailGo( MergeExportedTypes() );
    }    

    IfFailGo( MergeCustomAttributes() );
    IfFailGo( MergeDeclSecuritys() );


    // Please don't add any MergeXxx() below here.  CustomAttributess must be
    // very late, because custom values are various other types.

    // Fixup list cannot be merged. Linker will need to re-emit them.

    // Now call back to host for the result of token remap
    // 
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // Send token remap information for each import scope
        pCurTKMap = pImportData->m_pMDTokenMap;
        TOKENREC    *pRec;
        if (pImportData->m_pHostMapToken)
        {
            for (i = 0; i < pCurTKMap->Count(); i++)
            {
                pRec = pCurTKMap->Get(i);
                if (!pRec->IsEmpty())
                    pImportData->m_pHostMapToken->Map(pRec->m_tkFrom, pRec->m_tkTo);
            }
        }
    }

    // And last, but not least, let's do Raja's fixups.
    if (m_fMergingSecurityCriticalAttribute)
    {
        MergeSecurityCriticalAttributes();
    }


#if _DEBUG
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // dump the mapping
        LOG((LOGMD, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
        LOG((LOGMD, "Dumping token remap for one import scope!\n"));
        LOG((LOGMD, "This is the %d import scope for merge!\n", pImportData->m_iImport));        

        pCurTKMap = pImportData->m_pMDTokenMap;
        TOKENREC    *pRec;
        for (i = 0; i < pCurTKMap->Count(); i++)
        {
            pRec = pCurTKMap->Get(i);
            if (!pRec->IsEmpty())
            {
                LOG((LOGMD, "   Token 0x%08x  ====>>>> Token 0x%08x\n", pRec->m_tkFrom, pRec->m_tkTo));
            }
        }
        LOG((LOGMD, "End dumping token remap!\n"));
        LOG((LOGMD, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
    }
#endif // _DEBUG

ErrExit:
    return hr;
} // HRESULT NEWMERGER::Merge()


//*****************************************************************************
// Merge ModuleDef
//*****************************************************************************
HRESULT NEWMERGER::MergeModule()
{
    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;
    HRESULT         hr = NOERROR;
    TOKENREC        *pTokenRec;

    // we don't really merge Module information but we create a one to one mapping for each module token into the TokenMap
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        // set the current MDTokenMap

        pCurTkMap = pImportData->m_pMDTokenMap;
        IfFailGo( pCurTkMap->InsertNotFound(TokenFromRid(1, mdtModule), true, TokenFromRid(1, mdtModule), &pTokenRec) );
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeModule()


//*****************************************************************************
// Merge TypeDef but only Names. This is a partial merge to support TypeRef to TypeDef optimization
//*****************************************************************************
HRESULT NEWMERGER::MergeTypeDefNamesOnly()
{
    HRESULT         hr = NOERROR;
    TypeDefRec      *pRecImport = NULL;
    TypeDefRec      *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    mdTypeDef       tdEmit;
    mdTypeDef       tdImport;
    bool            bDuplicate;
    DWORD           dwFlags;
    DWORD           dwExportFlags;
    NestedClassRec  *pNestedRec;
    RID             iNestedRec;
    mdTypeDef       tdNester;
    TOKENREC        *pTokenRec;

    LPCUTF8         szNameImp;
    LPCUTF8         szNamespaceImp;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    MergeTypeData   *pMTD;
    BOOL            bSuppressMergeCheck;
    mdCustomAttribute tkCA;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
        
        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;

        iCount = pMiniMdImport->getCountTypeDefs();

        // Merge the typedefs
        for (i = 1; i <= iCount; i++)
        {
            // only merge those TypeDefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsTypeDefMarked(TokenFromRid(i, mdtTypeDef)) == false)
                continue;

            // compare it with the emit scope
            pRecImport = pMiniMdImport->getTypeDef(i);
            szNameImp = pMiniMdImport->getNameOfTypeDef(pRecImport);
            szNamespaceImp = pMiniMdImport->getNamespaceOfTypeDef(pRecImport);

            // If the class is a Nested class, get the parent token.
            dwFlags = pMiniMdImport->getFlagsOfTypeDef(pRecImport);
            if (IsTdNested(dwFlags))
            {
                iNestedRec = pMiniMdImport->FindNestedClassHelper(TokenFromRid(i, mdtTypeDef));
                if (InvalidRid(iNestedRec))
                {
                    _ASSERTE(!"Bad state!");
                    IfFailGo(META_E_BADMETADATA);
                }
                else
                {
                    pNestedRec = pMiniMdImport->getNestedClass(iNestedRec);
                    tdNester = pMiniMdImport->getEnclosingClassOfNestedClass(pNestedRec);
                    _ASSERTE(!IsNilToken(tdNester));
                    IfFailGo(pCurTkMap->Remap(tdNester, &tdNester));
                }
            }
            else
                tdNester = mdTokenNil;

            bSuppressMergeCheck = (pImportData->m_tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdImport,
                    TokenFromRid(i, mdtTypeDef), pImportData->m_tkSuppressMergeCheckCtor,
                    NULL, 0, &tkCA));
            
            // does this TypeDef already exist in the emit scope?
            if ( ImportHelper::FindTypeDefByName(
                pMiniMdEmit,
                szNamespaceImp,
                szNameImp,
                tdNester,
                &tdEmit) == S_OK )
            {
                // Yes, it does
                bDuplicate = true;

                // Let's look at their accessiblities.
                pRecEmit = pMiniMdEmit->getTypeDef(RidFromToken(tdEmit));
                dwExportFlags = pMiniMdEmit->getFlagsOfTypeDef(pRecEmit);

                // Managed types need to have the same accessiblity
                BOOL fManagedType = FALSE;
                IfFailGo(IsManagedType(pMiniMdImport, TokenFromRid(i, mdtTypeDef), &fManagedType));
                if (fManagedType)
                {
                    if ((dwFlags&tdVisibilityMask) != (dwExportFlags&tdVisibilityMask))
                    {
                        CheckContinuableErrorEx(META_E_MISMATCHED_VISIBLITY, pImportData, TokenFromRid(i, mdtTypeDef));
                    }

                }
                pMTD = m_rMTDs.Get(RidFromToken(tdEmit));
                if (pMTD->m_bSuppressMergeCheck != bSuppressMergeCheck)
                {
                    CheckContinuableErrorEx(META_E_MD_INCONSISTENCY, pImportData, TokenFromRid(i, mdtTypeDef));
                }
            }
            else
            {
                // No, it doesn't. Copy it over.
                bDuplicate = false;
                IfNullGo( pRecEmit = pMiniMdEmit->AddTypeDefRecord((RID *)&tdEmit) );

                // make sure the index matches
                _ASSERTE(((mdTypeDef)m_rMTDs.Count()) == tdEmit);
                
                IfNullGo(pMTD = m_rMTDs.Append());

                pMTD->m_cMethods = 0;
                pMTD->m_cFields = 0;
                pMTD->m_cEvents = 0;
                pMTD->m_cProperties = 0;
                pMTD->m_bSuppressMergeCheck = bSuppressMergeCheck;

                tdEmit = TokenFromRid( tdEmit, mdtTypeDef );

                // Set Full Qualified Name.
                IfFailGo( CopyTypeDefPartially( pRecEmit, pMiniMdImport, pRecImport) );

                // Create a NestedClass record if the class is a Nested class.
                if (! IsNilToken(tdNester))
                {
                    IfNullGo( pNestedRec = pMiniMdEmit->AddNestedClassRecord(&iNestedRec) );

                    // copy over the information
                    IfFailGo( pMiniMdEmit->PutToken(TBL_NestedClass, NestedClassRec::COL_NestedClass,
                                                    pNestedRec, tdEmit));

                    // tdNester has already been remapped above to the Emit scope.
                    IfFailGo( pMiniMdEmit->PutToken(TBL_NestedClass, NestedClassRec::COL_EnclosingClass,
                                                    pNestedRec, tdNester));
                    IfFailGo( pMiniMdEmit->AddNestedClassToHash(iNestedRec) );

                }
            }

            // record the token movement
            tdImport = TokenFromRid(i, mdtTypeDef);
            IfFailGo( pCurTkMap->InsertNotFound(tdImport, bDuplicate, tdEmit, &pTokenRec) );
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeTypeDefNamesOnly()


//*****************************************************************************
// Merge EnclosingType tables
//*****************************************************************************
HRESULT NEWMERGER::CopyTypeDefPartially( 
    TypeDefRec  *pRecEmit,                  // [IN] the emit record to fill
    CMiniMdRW   *pMiniMdImport,             // [IN] the importing scope
    TypeDefRec  *pRecImp)                   // [IN] the record to import

{
    HRESULT     hr;
    LPCUTF8     szNameImp;
    LPCUTF8     szNamespaceImp;
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();

    szNameImp = pMiniMdImport->getNameOfTypeDef(pRecImp);
    szNamespaceImp = pMiniMdImport->getNamespaceOfTypeDef(pRecImp);

    IfFailGo( pMiniMdEmit->PutString( TBL_TypeDef, TypeDefRec::COL_Name, pRecEmit, szNameImp) );
    IfFailGo( pMiniMdEmit->PutString( TBL_TypeDef, TypeDefRec::COL_Namespace, pRecEmit, szNamespaceImp) );

    pRecEmit->SetFlags(pRecImp->GetFlags());

    // Don't copy over the extends until TypeRef's remap is calculated

ErrExit:
    return hr;

} // HRESULT NEWMERGER::CopyTypeDefPartially()


//*****************************************************************************
// Merge ModuleRef tables including ModuleRef to ModuleDef optimization
//*****************************************************************************
HRESULT NEWMERGER::MergeModuleRefs()
{
    HRESULT         hr = NOERROR;
    ModuleRefRec    *pRecImport = NULL;
    ModuleRefRec    *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    mdModuleRef     mrEmit;
    bool            bDuplicate = false;
    TOKENREC        *pTokenRec;
    LPCUTF8         szNameImp;
    bool            isModuleDef;

    MergeImportData *pImportData;
    MergeImportData *pData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountModuleRefs();

        // loop through all ModuleRef
        for (i = 1; i <= iCount; i++)
        {
            // only merge those ModuleRefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsModuleRefMarked(TokenFromRid(i, mdtModuleRef)) == false)
                continue;

            isModuleDef = false;

            // compare it with the emit scope
            pRecImport = pMiniMdImport->getModuleRef(i);
            szNameImp = pMiniMdImport->getNameOfModuleRef(pRecImport);

            // Only do the ModuleRef to ModuleDef optimization if ModuleRef's name is meaningful!
            if ( szNameImp && szNameImp[0] != '\0')
            {

                // Check to see if this ModuleRef has become the ModuleDef token
                for (pData = m_pImportDataList; pData != NULL; pData = pData->m_pNextImportData)
                {
                    CMiniMdRW       *pMiniMd = &(pData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
                    ModuleRec       *pRec;
                    LPCUTF8         szName;

                    pRec = pMiniMd->getModule(MODULEDEFTOKEN);
                    szName = pMiniMd->getNameOfModule(pRec);
                    if (szName && szName[0] != '\0' && strcmp(szNameImp, szName) == 0)
                    {
                        // We found an import Module for merging that has the same name as the ModuleRef
                        isModuleDef = true;
                        bDuplicate = true;
                        mrEmit = MODULEDEFTOKEN;       // set the resulting token to ModuleDef Token
                        break;
                    }
                }
            }

            if (isModuleDef == false)
            {
                // does this ModuleRef already exist in the emit scope?
                hr = ImportHelper::FindModuleRef(pMiniMdEmit,
                                                szNameImp,
                                                &mrEmit);
                if (hr == S_OK)
                {
                    // Yes, it does
                    bDuplicate = true;
                }
                else if (hr == CLDB_E_RECORD_NOTFOUND)
                {
                    // No, it doesn't. Copy it over.
                    bDuplicate = false;
                    IfNullGo( pRecEmit = pMiniMdEmit->AddModuleRefRecord((RID*)&mrEmit) );
                    mrEmit = TokenFromRid(mrEmit, mdtModuleRef);

                    // Set ModuleRef Name.
                    IfFailGo( pMiniMdEmit->PutString(TBL_ModuleRef, ModuleRefRec::COL_Name, pRecEmit, szNameImp) );
                }
                else
                    IfFailGo(hr);
            }

            // record the token movement
            IfFailGo( pCurTkMap->InsertNotFound(
                TokenFromRid(i, mdtModuleRef), 
                bDuplicate,
                mrEmit,
                &pTokenRec) );
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeModuleRefs()


//*****************************************************************************
// Merge AssemblyRef tables
//*****************************************************************************
HRESULT NEWMERGER::MergeAssemblyRefs()
{
    HRESULT         hr = NOERROR;
    AssemblyRefRec  *pRecImport = NULL;
    AssemblyRefRec  *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    mdAssemblyRef   arEmit;
    bool            bDuplicate = false;
    LPCUTF8         szTmp;
    const void      *pbTmp;
    ULONG           cbTmp;
    ULONG           iCount;
    ULONG           i;
    ULONG           iRecord;
    TOKENREC        *pTokenRec;
    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountAssemblyRefs();

        // loope through all the AssemblyRefs.
        for (i = 1; i <= iCount; i++)
        {
            // Compare with the emit scope.
            pRecImport = pMiniMdImport->getAssemblyRef(i);
            pbTmp = pMiniMdImport->getPublicKeyOrTokenOfAssemblyRef(pRecImport, &cbTmp);
            hr = CLDB_E_RECORD_NOTFOUND;
            if (m_fDupCheck)
                hr = ImportHelper::FindAssemblyRef(pMiniMdEmit,
                                               pMiniMdImport->getNameOfAssemblyRef(pRecImport),
                                               pMiniMdImport->getLocaleOfAssemblyRef(pRecImport),
                                               pbTmp, 
                                               cbTmp,
                                               pRecImport->GetMajorVersion(),
                                               pRecImport->GetMinorVersion(),
                                               pRecImport->GetBuildNumber(),
                                               pRecImport->GetRevisionNumber(),
                                               pRecImport->GetFlags(),
                                               &arEmit);
            if (hr == S_OK)
            {
                // Yes, it does
                bDuplicate = true;

            }
            else if (hr == CLDB_E_RECORD_NOTFOUND)
            {
                // No, it doesn't.  Copy it over.
                bDuplicate = false;
                IfNullGo( pRecEmit = pMiniMdEmit->AddAssemblyRefRecord(&iRecord));
                arEmit = TokenFromRid(iRecord, mdtAssemblyRef);

                pRecEmit->Copy(pRecImport);

                pbTmp = pMiniMdImport->getPublicKeyOrTokenOfAssemblyRef(pRecImport, &cbTmp);
                IfFailGo(pMiniMdEmit->PutBlob(TBL_AssemblyRef, AssemblyRefRec::COL_PublicKeyOrToken,
                                            pRecEmit, pbTmp, cbTmp));

                szTmp = pMiniMdImport->getNameOfAssemblyRef(pRecImport);
                IfFailGo(pMiniMdEmit->PutString(TBL_AssemblyRef, AssemblyRefRec::COL_Name,
                                            pRecEmit, szTmp));

                szTmp = pMiniMdImport->getLocaleOfAssemblyRef(pRecImport);
                IfFailGo(pMiniMdEmit->PutString(TBL_AssemblyRef, AssemblyRefRec::COL_Locale,
                                            pRecEmit, szTmp));

                pbTmp = pMiniMdImport->getHashValueOfAssemblyRef(pRecImport, &cbTmp);
                IfFailGo(pMiniMdEmit->PutBlob(TBL_AssemblyRef, AssemblyRefRec::COL_HashValue,
                                            pRecEmit, pbTmp, cbTmp));

            }
            else
                IfFailGo(hr);

            // record the token movement.
            IfFailGo(pCurTkMap->InsertNotFound(
                TokenFromRid(i, mdtAssemblyRef),
                bDuplicate,
                arEmit,
                &pTokenRec));
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeAssemblyRefs()


//*****************************************************************************
// Merge TypeRef tables also performing TypeRef to TypeDef opitimization. ie.
// we will not introduce a TypeRef record if we can optimize it to a TypeDef.
//*****************************************************************************
HRESULT NEWMERGER::MergeTypeRefs()
{
    HRESULT     hr = NOERROR;
    TypeRefRec  *pRecImport = NULL;
    TypeRefRec  *pRecEmit = NULL;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    ULONG       iCount;
    ULONG       i;
    mdTypeRef   trEmit;
    bool        bDuplicate = false;
    TOKENREC    *pTokenRec;
    bool        isTypeDef;

    mdToken     tkResImp;
    mdToken     tkResEmit;
    LPCUTF8     szNameImp;
    LPCUTF8     szNamespaceImp;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountTypeRefs();

        // loop through all TypeRef
        for (i = 1; i <= iCount; i++)
        {
            // only merge those TypeRefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsTypeRefMarked(TokenFromRid(i, mdtTypeRef)) == false)
                continue;

            isTypeDef = false;

            // compare it with the emit scope
            pRecImport = pMiniMdImport->getTypeRef(i);
            tkResImp = pMiniMdImport->getResolutionScopeOfTypeRef(pRecImport);
            szNamespaceImp = pMiniMdImport->getNamespaceOfTypeRef(pRecImport);
            szNameImp = pMiniMdImport->getNameOfTypeRef(pRecImport);
            if (!IsNilToken(tkResImp))
            {
                IfFailGo(pCurTkMap->Remap(tkResImp, &tkResEmit));
            }
            else
            {
                tkResEmit = tkResImp;
            }

            if (IsNilToken(tkResEmit) || tkResEmit == MODULEDEFTOKEN || TypeFromToken(tkResEmit) == mdtTypeDef)
            {
                hr = ImportHelper::FindTypeDefByName(
                    pMiniMdEmit,
                    szNamespaceImp,
                    szNameImp,
                    (TypeFromToken(tkResEmit) == mdtTypeDef) ? tkResEmit : mdTokenNil,
                    &trEmit);
                if (hr == S_OK)
                {
                    isTypeDef = true;

                    // it really does not matter if we set the duplicate to true or false. 
                    bDuplicate = true;
                }
            }

            // If the ResolutionScope was merged as a TypeDef, and this token wasn't found as TypeDef, send the error.
            if (TypeFromToken(tkResEmit) == mdtTypeDef && !isTypeDef)
            {
                // Send the error notification.  Use the "continuable error" callback, but even if linker says it is
                //  ok, don't continue.
                CheckContinuableErrorEx(META_E_TYPEDEF_MISSING, pImportData, TokenFromRid(i, mdtTypeRef));
                IfFailGo(META_E_TYPEDEF_MISSING);
            }

            // If this TypeRef cannot be optmized to a TypeDef or the Ref to Def optimization is turned off, do the following.
            if (!isTypeDef || !((m_optimizeRefToDef & MDTypeRefToDef) == MDTypeRefToDef))
            {
                // does this TypeRef already exist in the emit scope?
                if ( m_fDupCheck && ImportHelper::FindTypeRefByName(
                    pMiniMdEmit,
                    tkResEmit,
                    szNamespaceImp,
                    szNameImp,
                    &trEmit) == S_OK )
                {
                    // Yes, it does
                    bDuplicate = true;
                }
                else
                {
                    // No, it doesn't. Copy it over.
                    bDuplicate = false;
                    IfNullGo( pRecEmit = pMiniMdEmit->AddTypeRefRecord((RID*)&trEmit) );
                    trEmit = TokenFromRid(trEmit, mdtTypeRef);

                    // Set ResolutionScope.  tkResEmit has already been re-mapped.
                    IfFailGo(pMiniMdEmit->PutToken(TBL_TypeRef, TypeRefRec::COL_ResolutionScope,
                                                    pRecEmit, tkResEmit));

                    // Set Name.
                    IfFailGo(pMiniMdEmit->PutString(TBL_TypeRef, TypeRefRec::COL_Name,
                                                    pRecEmit, szNameImp));
                    IfFailGo(pMiniMdEmit->AddNamedItemToHash(TBL_TypeRef, trEmit, szNameImp, 0));
            
                    // Set Namespace.
                    IfFailGo(pMiniMdEmit->PutString(TBL_TypeRef, TypeRefRec::COL_Namespace,
                                                    pRecEmit, szNamespaceImp));
                }
            }

            // record the token movement
            IfFailGo( pCurTkMap->InsertNotFound(
                TokenFromRid(i, mdtTypeRef), 
                bDuplicate,
                trEmit,
                &pTokenRec) );
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeTypeRefs()
 

//*****************************************************************************
// copy over the remaining information of partially merged TypeDef records. Right now only
// extends field is delayed to here. The reason that we delay extends field is because we want
// to optimize TypeRef to TypeDef if possible.
//*****************************************************************************
HRESULT NEWMERGER::CompleteMergeTypeDefs()
{
    HRESULT         hr = NOERROR;
    TypeDefRec      *pRecImport = NULL;
    TypeDefRec      *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    TOKENREC        *pTokenRec;
    mdToken         tkExtendsImp;
    mdToken         tkExtendsEmit;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;

        iCount = pMiniMdImport->getCountTypeDefs();

        // Merge the typedefs
        for (i = 1; i <= iCount; i++)
        {
            // only merge those TypeDefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsTypeDefMarked(TokenFromRid(i, mdtTypeDef)) == false)
                continue;

            if ( !pCurTkMap->Find(TokenFromRid(i, mdtTypeDef), &pTokenRec) )
            {
                _ASSERTE( !"bad state!");
                IfFailGo( META_E_BADMETADATA );
            }

            if (pTokenRec->m_isDuplicate == false)
            {
                // get the extends token from the import
                pRecImport = pMiniMdImport->getTypeDef(i);
                tkExtendsImp = pMiniMdImport->getExtendsOfTypeDef(pRecImport);

                // map the extends token to an merged token
                IfFailGo( pCurTkMap->Remap(tkExtendsImp, &tkExtendsEmit) );

                // set the extends to the merged TypeDef records.
                pRecEmit = pMiniMdEmit->getTypeDef( RidFromToken(pTokenRec->m_tkTo) );
                IfFailGo(pMiniMdEmit->PutToken(TBL_TypeDef, TypeDefRec::COL_Extends, pRecEmit, tkExtendsEmit));                
            }
            else
            {
            }
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::CompleteMergeTypeDefs()


//*****************************************************************************
// merging TypeSpecs
//*****************************************************************************
HRESULT NEWMERGER::MergeTypeSpecs()
{
    HRESULT         hr = NOERROR;
    TypeSpecRec     *pRecImport = NULL;
    TypeSpecRec     *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    TOKENREC        *pTokenRec;
    mdTypeSpec      tsImp;
    mdTypeSpec      tsEmit;
    bool            fDuplicate;
    PCCOR_SIGNATURE pbSig;
    ULONG           cbSig;
    ULONG           cbEmit;
    CQuickBytes     qbSig;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;

        iCount = pMiniMdImport->getCountTypeSpecs();

        // loop through all TypeSpec
        for (i = 1; i <= iCount; i++)
        {
            // only merge those TypeSpecs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsTypeSpecMarked(TokenFromRid(i, mdtTypeSpec)) == false)
                continue;

            // compare it with the emit scope
            pRecImport = pMiniMdImport->getTypeSpec(i);
            pbSig = pMiniMdImport->getSignatureOfTypeSpec(pRecImport, &cbSig);

            // convert tokens contained in signature to new scope
            IfFailGo(ImportHelper::MergeUpdateTokenInFieldSig(
                NULL,                       // Assembly emit scope.
                pMiniMdEmit,                // The emit scope.
                NULL, NULL, 0,              // Import assembly information.
                pMiniMdImport,              // The scope to merge into the emit scope.
                pbSig,                      // signature from the imported scope
                pCurTkMap,                  // Internal token mapping structure.
                &qbSig,                     // [OUT] translated signature
                0,                          // start from first byte of the signature
                0,                          // don't care how many bytes consumed
                &cbEmit));                  // number of bytes write to cbEmit

            hr = CLDB_E_RECORD_NOTFOUND;
            if (m_fDupCheck)
                hr = ImportHelper::FindTypeSpec(
                    pMiniMdEmit,
                    (PCOR_SIGNATURE) qbSig.Ptr(),
                    cbEmit,
                    &tsEmit );

            if ( hr == S_OK )
            {
                // find a duplicate
                fDuplicate = true;
            }
            else
            {
                // copy over
                fDuplicate = false;
                IfNullGo( pRecEmit = pMiniMdEmit->AddTypeSpecRecord((ULONG *)&tsEmit) );
                tsEmit = TokenFromRid(tsEmit, mdtTypeSpec);
                IfFailGo( pMiniMdEmit->PutBlob(
                    TBL_TypeSpec, 
                    TypeSpecRec::COL_Signature, 
                    pRecEmit, 
                    (PCOR_SIGNATURE)qbSig.Ptr(), 
                    cbEmit));
            }
            tsImp = TokenFromRid(i, mdtTypeSpec);

            // Record the token movement
            IfFailGo( pCurTkMap->InsertNotFound(tsImp, fDuplicate, tsEmit, &pTokenRec) );
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeTypeSpecs()


//*****************************************************************************
// merging Children of TypeDefs. This includes field, method, parameter, property, event
//*****************************************************************************
HRESULT NEWMERGER::MergeTypeDefChildren() 
{
    HRESULT         hr = NOERROR;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    mdTypeDef       tdEmit;
    mdTypeDef       tdImport;
    TOKENREC        *pTokenRec;

#if _DEBUG
    TypeDefRec      *pRecImport = NULL;
    LPCUTF8         szNameImp;
    LPCUTF8         szNamespaceImp;
#endif // _DEBUG

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountTypeDefs();

        // loop through all TypeDef again to merge/copy Methods, fields, events, and properties
        // 
        for (i = 1; i <= iCount; i++)
        {
            // only merge those TypeDefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsTypeDefMarked(TokenFromRid(i, mdtTypeDef)) == false)
                continue;

#if _DEBUG
            pRecImport = pMiniMdImport->getTypeDef(i);
            szNameImp = pMiniMdImport->getNameOfTypeDef(pRecImport);
            szNamespaceImp = pMiniMdImport->getNamespaceOfTypeDef(pRecImport);
#endif // _DEBUG

            // check to see if the typedef is duplicate or not
            tdImport = TokenFromRid(i, mdtTypeDef);
            if ( pCurTkMap->Find( tdImport, &pTokenRec) == false)
            {
                _ASSERTE( !"bad state!");
                IfFailGo( META_E_BADMETADATA );
            }
            tdEmit = pTokenRec->m_tkTo;
            if (pTokenRec->m_isDuplicate == false)
            {
                // now move all of the children records over
                IfFailGo( CopyMethods(pImportData, tdImport, tdEmit) );
                IfFailGo( CopyFields(pImportData, tdImport, tdEmit) );

                IfFailGo( CopyEvents(pImportData, tdImport, tdEmit) );

                //  Property has dependency on events
                IfFailGo( CopyProperties(pImportData, tdImport, tdEmit) );

                // Generic Params.
                IfFailGo( CopyGenericParams(pImportData, tdImport, tdEmit) );
            }
            else
            {
                // verify the children records
                IfFailGo( VerifyMethods(pImportData, tdImport, tdEmit) );
                IfFailGo( VerifyFields(pImportData, tdImport, tdEmit) );
                IfFailGo( VerifyEvents(pImportData, tdImport, tdEmit) );

                // property has dependency on events
                IfFailGo( VerifyProperties(pImportData, tdImport, tdEmit) );

                IfFailGo( VerifyGenericParams(pImportData, tdImport, tdEmit) );
            }
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeTypeDefChildren()


//*******************************************************************************
// Helper to copy an Method record
//*******************************************************************************
HRESULT NEWMERGER::CopyMethod(
    MergeImportData *pImportData,           // [IN] import scope
    MethodRec   *pRecImp,                   // [IN] the record to import
    MethodRec   *pRecEmit)                  // [IN] the emit record to fill
{
    HRESULT     hr;
    CMiniMdRW   *pMiniMdImp = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();
    LPCUTF8     szName;
    PCCOR_SIGNATURE pbSig;
    ULONG       cbSig;
    ULONG       cbEmit;
    CQuickBytes qbSig;
    MDTOKENMAP  *pCurTkMap;

    pCurTkMap = pImportData->m_pMDTokenMap;

    // copy over the fix part of the record
    pRecEmit->Copy(pRecImp);

    // copy over the name
    szName = pMiniMdImp->getNameOfMethod(pRecImp);
    IfFailGo(pMiniMdEmit->PutString(TBL_Method, MethodRec::COL_Name, pRecEmit, szName));

    // copy over the signature
    pbSig = pMiniMdImp->getSignatureOfMethod(pRecImp, &cbSig);

    // convert rid contained in signature to new scope
    IfFailGo(ImportHelper::MergeUpdateTokenInSig(    
        NULL,                       // Assembly emit scope.
        pMiniMdEmit,                // The emit scope.
        NULL, NULL, 0,              // Import assembly scope information.
        pMiniMdImp,                 // The scope to merge into the emit scope.
        pbSig,                      // signature from the imported scope
        pCurTkMap,                // Internal token mapping structure.
        &qbSig,                     // [OUT] translated signature
        0,                          // start from first byte of the signature
        0,                          // don't care how many bytes consumed
        &cbEmit));                  // number of bytes write to cbEmit

    IfFailGo(pMiniMdEmit->PutBlob(TBL_Method, MethodRec::COL_Signature, pRecEmit, qbSig.Ptr(), cbEmit));

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyMethod()


//*******************************************************************************
// Helper to copy an field record
//*******************************************************************************
HRESULT NEWMERGER::CopyField(
    MergeImportData *pImportData,           // [IN] import scope
    FieldRec    *pRecImp,                   // [IN] the record to import
    FieldRec    *pRecEmit)                  // [IN] the emit record to fill
{
    HRESULT     hr;
    CMiniMdRW   *pMiniMdImp = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();
    LPCUTF8     szName;
    PCCOR_SIGNATURE pbSig;
    ULONG       cbSig;
    ULONG       cbEmit;
    CQuickBytes qbSig;
    MDTOKENMAP  *pCurTkMap;

    pCurTkMap = pImportData->m_pMDTokenMap;

    // copy over the fix part of the record
    pRecEmit->SetFlags(pRecImp->GetFlags());

    // copy over the name
    szName = pMiniMdImp->getNameOfField(pRecImp);
    IfFailGo(pMiniMdEmit->PutString(TBL_Field, FieldRec::COL_Name, pRecEmit, szName));

    // copy over the signature
    pbSig = pMiniMdImp->getSignatureOfField(pRecImp, &cbSig);

    // convert rid contained in signature to new scope
    IfFailGo(ImportHelper::MergeUpdateTokenInSig(    
        NULL,                       // Emit assembly scope.
        pMiniMdEmit,                // The emit scope.
        NULL, NULL, 0,              // Import assembly scope information.
        pMiniMdImp,                 // The scope to merge into the emit scope.
        pbSig,                      // signature from the imported scope
        pCurTkMap,                  // Internal token mapping structure.
        &qbSig,                     // [OUT] translated signature
        0,                          // start from first byte of the signature
        0,                          // don't care how many bytes consumed
        &cbEmit));                  // number of bytes write to cbEmit

    IfFailGo(pMiniMdEmit->PutBlob(TBL_Field, FieldRec::COL_Signature, pRecEmit, qbSig.Ptr(), cbEmit));

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyField()

//*******************************************************************************
// Helper to copy an field record
//*******************************************************************************
HRESULT NEWMERGER::CopyParam(
    MergeImportData *pImportData,           // [IN] import scope
    ParamRec    *pRecImp,                   // [IN] the record to import
    ParamRec    *pRecEmit)                  // [IN] the emit record to fill
{
    HRESULT     hr;
    CMiniMdRW   *pMiniMdImp = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();
    LPCUTF8     szName;
    MDTOKENMAP  *pCurTkMap;

    pCurTkMap = pImportData->m_pMDTokenMap;

    // copy over the fix part of the record
    pRecEmit->Copy(pRecImp);

    // copy over the name
    szName = pMiniMdImp->getNameOfParam(pRecImp);
    IfFailGo(pMiniMdEmit->PutString(TBL_Param, ParamRec::COL_Name, pRecEmit, szName));

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyParam()

//*******************************************************************************
// Helper to copy an Event record
//*******************************************************************************
HRESULT NEWMERGER::CopyEvent(
    MergeImportData *pImportData,           // [IN] import scope
    EventRec    *pRecImp,                   // [IN] the record to import
    EventRec    *pRecEmit)                  // [IN] the emit record to fill
{
    HRESULT     hr = NOERROR;
    CMiniMdRW   *pMiniMdImp = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();
    mdToken     tkEventTypeImp;
    mdToken     tkEventTypeEmit;            // could be TypeDef or TypeRef
    LPCUTF8     szName;
    MDTOKENMAP  *pCurTkMap;

    pCurTkMap = pImportData->m_pMDTokenMap;

    pRecEmit->SetEventFlags(pRecImp->GetEventFlags());

    //move over the event name
    szName = pMiniMdImp->getNameOfEvent( pRecImp );
    IfFailGo( pMiniMdEmit->PutString(TBL_Event, EventRec::COL_Name, pRecEmit, szName) );

    // move over the EventType
    tkEventTypeImp = pMiniMdImp->getEventTypeOfEvent(pRecImp);
    if ( !IsNilToken(tkEventTypeImp) )
    {
        IfFailGo( pCurTkMap->Remap(tkEventTypeImp, &tkEventTypeEmit) );
        IfFailGo(pMiniMdEmit->PutToken(TBL_Event, EventRec::COL_EventType, pRecEmit, tkEventTypeEmit));
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyEvent()


//*******************************************************************************
// Helper to copy a property record
//*******************************************************************************
HRESULT NEWMERGER::CopyProperty(
    MergeImportData *pImportData,           // [IN] import scope
    PropertyRec *pRecImp,                   // [IN] the record to import
    PropertyRec *pRecEmit)                  // [IN] the emit record to fill
{
    HRESULT     hr = NOERROR;
    CMiniMdRW   *pMiniMdImp = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();
    LPCUTF8     szName;
    PCCOR_SIGNATURE pbSig;
    ULONG       cbSig;
    ULONG       cbEmit;
    CQuickBytes qbSig;
    MDTOKENMAP  *pCurTkMap;

    pCurTkMap = pImportData->m_pMDTokenMap;

    // move over the flag value
    pRecEmit->SetPropFlags(pRecImp->GetPropFlags());

    //move over the property name
    szName = pMiniMdImp->getNameOfProperty( pRecImp );
    IfFailGo( pMiniMdEmit->PutString(TBL_Property, PropertyRec::COL_Name, pRecEmit, szName) );

    // move over the type of the property
    pbSig = pMiniMdImp->getTypeOfProperty( pRecImp, &cbSig );

    // convert rid contained in signature to new scope
    IfFailGo( ImportHelper::MergeUpdateTokenInSig(    
        NULL,                       // Assembly emit scope.
        pMiniMdEmit,                // The emit scope.
        NULL, NULL, 0,              // Import assembly scope information.
        pMiniMdImp,                 // The scope to merge into the emit scope.
        pbSig,                      // signature from the imported scope
        pCurTkMap,                // Internal token mapping structure.
        &qbSig,                     // [OUT] translated signature
        0,                          // start from first byte of the signature
        0,                          // don't care how many bytes consumed
        &cbEmit) );                 // number of bytes write to cbEmit

    IfFailGo(pMiniMdEmit->PutBlob(TBL_Property, PropertyRec::COL_Type, pRecEmit, qbSig.Ptr(), cbEmit));

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyProperty()


//*****************************************************************************
// Copy MethodSemantics for an event or a property
//*****************************************************************************
HRESULT NEWMERGER::CopyMethodSemantics(
    MergeImportData *pImportData, 
    mdToken     tkImport,               // Event or property in the import scope
    mdToken     tkEmit)                 // corresponding event or property in the emitting scope
{
    HRESULT     hr = NOERROR;
    MethodSemanticsRec  *pRecImport = NULL;
    MethodSemanticsRec  *pRecEmit = NULL;
    CMiniMdRW   *pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();
    ULONG       i;
    ULONG       msEmit;                 // MethodSemantics are just index not tokens
    mdToken     tkMethodImp;
    mdToken     tkMethodEmit;
    MDTOKENMAP  *pCurTkMap;
    HENUMInternal hEnum;

    pCurTkMap = pImportData->m_pMDTokenMap;

    // copy over the associates
    IfFailGo( pMiniMdImport->FindMethodSemanticsHelper(tkImport, &hEnum) );
    while (HENUMInternal::EnumNext(&hEnum, (mdToken *) &i))
    {
        pRecImport = pMiniMdImport->getMethodSemantics(i);
        IfNullGo( pRecEmit = pMiniMdEmit->AddMethodSemanticsRecord(&msEmit) );
        pRecEmit->SetSemantic(pRecImport->GetSemantic());

        // set the MethodSemantics
        tkMethodImp = pMiniMdImport->getMethodOfMethodSemantics(pRecImport);
        IfFailGo(  pCurTkMap->Remap(tkMethodImp, &tkMethodEmit) );
        IfFailGo( pMiniMdEmit->PutToken(TBL_MethodSemantics, MethodSemanticsRec::COL_Method, pRecEmit, tkMethodEmit));

        // set the associate
        _ASSERTE( pMiniMdImport->getAssociationOfMethodSemantics(pRecImport) == tkImport );
        IfFailGo( pMiniMdEmit->PutToken(TBL_MethodSemantics, MethodSemanticsRec::COL_Association, pRecEmit, tkEmit));

        // no need to record the movement since it is not a token
        IfFailGo( pMiniMdEmit->AddMethodSemanticsToHash(msEmit) );
    }
ErrExit:
    HENUMInternal::ClearEnum(&hEnum);
    return hr;
} // HRESULT NEWMERGER::CopyMethodSemantics()


//*****************************************************************************
// Copy Methods given a TypeDef
//*****************************************************************************
HRESULT NEWMERGER::CopyMethods(
    MergeImportData *pImportData, 
    mdTypeDef       tdImport, 
    mdTypeDef       tdEmit)
{
    HRESULT     hr = NOERROR;
    MethodRec   *pRecImport = NULL;
    MethodRec   *pRecEmit = NULL;
    TypeDefRec  *pTypeDefRec;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    ULONG       ridStart, ridEnd;
    ULONG       i;
    mdMethodDef mdEmit;
    mdMethodDef mdImp;
    TOKENREC    *pTokenRec;
    MDTOKENMAP  *pCurTkMap;

    MergeTypeData *pMTD;
    BOOL        bSuppressMergeCheck;
    mdCustomAttribute tkCA;

    pMiniMdEmit = GetMiniMdEmit();
    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pCurTkMap = pImportData->m_pMDTokenMap;

    pTypeDefRec = pMiniMdImport->getTypeDef(RidFromToken(tdImport));
    ridStart = pMiniMdImport->getMethodListOfTypeDef(pTypeDefRec);
    ridEnd = pMiniMdImport->getEndMethodListOfTypeDef(pTypeDefRec);

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

    pMTD = m_rMTDs.Get(RidFromToken(tdEmit));

    // make sure we didn't count the methods yet
    _ASSERTE(pMTD->m_cMethods == 0);

    // loop through all Methods
    for (i = ridStart; i < ridEnd; i++)
    {
        // compare it with the emit scope
        mdImp = pMiniMdImport->GetMethodRid(i);

        // only merge those MethodDefs that are marked
        if ( pMiniMdImport->GetFilterTable()->IsMethodMarked(TokenFromRid(mdImp, mdtMethodDef)) == false)
            continue;

        pRecImport = pMiniMdImport->getMethod(mdImp);
        IfNullGo( pRecEmit = pMiniMdEmit->AddMethodRecord((RID *)&mdEmit) );

        // copy the method content over 
        IfFailGo( CopyMethod(pImportData, pRecImport, pRecEmit) );

        IfFailGo( pMiniMdEmit->AddMethodToTypeDef(RidFromToken(tdEmit), mdEmit));

        // record the token movement
        mdImp = TokenFromRid(mdImp, mdtMethodDef);
        mdEmit = TokenFromRid(mdEmit, mdtMethodDef);
        IfFailGo( pMiniMdEmit->AddMemberDefToHash(
            mdEmit, 
            tdEmit) ); 

        IfFailGo( pCurTkMap->InsertNotFound(mdImp, false, mdEmit, &pTokenRec) );

        // copy over the children
        IfFailGo( CopyParams(pImportData, mdImp, mdEmit) );
        IfFailGo( CopyGenericParams(pImportData, mdImp, mdEmit) );

        bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
               ((pImportData->m_tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdImport,
                    mdImp, pImportData->m_tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));


        if (!bSuppressMergeCheck) {
            pMTD->m_cMethods++;
        }
    }

    // make sure we don't count any methods if merge check is suppressed on the type
    _ASSERTE(pMTD->m_cMethods == 0 || !pMTD->m_bSuppressMergeCheck);
ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyMethods()


//*****************************************************************************
// Copy Fields given a TypeDef
//*****************************************************************************
HRESULT NEWMERGER::CopyFields(
    MergeImportData *pImportData, 
    mdTypeDef       tdImport, 
    mdTypeDef       tdEmit)
{
    HRESULT         hr = NOERROR;
    FieldRec        *pRecImport = NULL;
    FieldRec        *pRecEmit = NULL;
    TypeDefRec      *pTypeDefRec;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           ridStart, ridEnd;
    ULONG           i;
    mdFieldDef      fdEmit;
    mdFieldDef      fdImp;
    bool            bDuplicate;
    TOKENREC        *pTokenRec;
    PCCOR_SIGNATURE pvSigBlob;
    ULONG           cbSigBlob;
    MDTOKENMAP      *pCurTkMap;

    MergeTypeData   *pMTD;
    BOOL            bSuppressMergeCheck;
    mdCustomAttribute tkCA;

    pMiniMdEmit = GetMiniMdEmit();
    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pCurTkMap = pImportData->m_pMDTokenMap;

    pTypeDefRec = pMiniMdImport->getTypeDef(RidFromToken(tdImport));
    ridStart = pMiniMdImport->getFieldListOfTypeDef(pTypeDefRec);
    ridEnd = pMiniMdImport->getEndFieldListOfTypeDef(pTypeDefRec);

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

    pMTD = m_rMTDs.Get(RidFromToken(tdEmit));

    // make sure we didn't count the methods yet
    _ASSERTE(pMTD->m_cFields == 0);

    // loop through all FieldDef of a TypeDef
    for (i = ridStart; i < ridEnd; i++)
    {
        // compare it with the emit scope
        fdImp = pMiniMdImport->GetFieldRid(i);

        // only merge those FieldDefs that are marked
        if ( pMiniMdImport->GetFilterTable()->IsFieldMarked(TokenFromRid(fdImp, mdtFieldDef)) == false)
            continue;

        
        pRecImport = pMiniMdImport->getField(fdImp);
        bDuplicate = false;
        IfNullGo( pRecEmit = pMiniMdEmit->AddFieldRecord((RID *)&fdEmit) );

        // copy the field content over 
        IfFailGo( CopyField(pImportData, pRecImport, pRecEmit) );
        
        IfFailGo( pMiniMdEmit->AddFieldToTypeDef(RidFromToken(tdEmit), fdEmit));

        // record the token movement
        fdImp = TokenFromRid(fdImp, mdtFieldDef);
        fdEmit = TokenFromRid(fdEmit, mdtFieldDef);
        pvSigBlob = pMiniMdEmit->getSignatureOfField(pRecEmit, &cbSigBlob);
        IfFailGo( pMiniMdEmit->AddMemberDefToHash(
            fdEmit, 
            tdEmit) ); 

        IfFailGo( pCurTkMap->InsertNotFound(fdImp, false, fdEmit, &pTokenRec) );

        // count the number of fields that didn't suppress merge check
        // non-static fields doesn't inherite the suppress merge check attribute from the type
        bSuppressMergeCheck = 
            (IsFdStatic(pRecEmit->GetFlags()) && pMTD->m_bSuppressMergeCheck) ||
               ((pImportData->m_tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdImport,
                    fdImp, pImportData->m_tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

        if (!bSuppressMergeCheck) {
            pMTD->m_cFields++;
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyFields()


//*****************************************************************************
// Copy Events given a TypeDef
//*****************************************************************************
HRESULT NEWMERGER::CopyEvents(
    MergeImportData *pImportData, 
    mdTypeDef       tdImport, 
    mdTypeDef       tdEmit)
{
    HRESULT     hr = NOERROR;
    CMiniMdRW   *pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();
    RID         ridEventMap;
    EventMapRec *pEventMapRec;  
    EventRec    *pRecImport;
    EventRec    *pRecEmit;
    ULONG       ridStart;
    ULONG       ridEnd;
    ULONG       i;
    mdEvent     evImp;
    mdEvent     evEmit;
    TOKENREC    *pTokenRec;
    ULONG       iEventMap;
    EventMapRec *pEventMap;
    MDTOKENMAP  *pCurTkMap;

    MergeTypeData *pMTD;
    BOOL        bSuppressMergeCheck;
    mdCustomAttribute tkCA;

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

    pCurTkMap = pImportData->m_pMDTokenMap;

    pMTD = m_rMTDs.Get(RidFromToken(tdEmit));

    // make sure we didn't count the events yet
    _ASSERTE(pMTD->m_cEvents == 0);

    ridEventMap = pMiniMdImport->FindEventMapFor(RidFromToken(tdImport));
    if (!InvalidRid(ridEventMap))
    {
        pEventMapRec = pMiniMdImport->getEventMap(ridEventMap);
        ridStart = pMiniMdImport->getEventListOfEventMap(pEventMapRec);
        ridEnd = pMiniMdImport->getEndEventListOfEventMap(pEventMapRec);

        if (ridEnd > ridStart)      
        {
            // If there is any event, create the eventmap record in the emit scope
            // Create new record.
            IfNullGo(pEventMap = pMiniMdEmit->AddEventMapRecord(&iEventMap));

            // Set parent.
            IfFailGo(pMiniMdEmit->PutToken(TBL_EventMap, EventMapRec::COL_Parent, pEventMap, tdEmit));
        }
        
        for (i = ridStart; i < ridEnd; i++)
        {
            // get the real event rid
            evImp = pMiniMdImport->GetEventRid(i);

            // only merge those Events that are marked
            if ( pMiniMdImport->GetFilterTable()->IsEventMarked(TokenFromRid(evImp, mdtEvent)) == false)
                continue;
            
            pRecImport = pMiniMdImport->getEvent(evImp);
            IfNullGo( pRecEmit = pMiniMdEmit->AddEventRecord((RID *)&evEmit) );

            // copy the event record over 
            IfFailGo( CopyEvent(pImportData, pRecImport, pRecEmit) );
            
            // Add Event to the EventMap.
            IfFailGo( pMiniMdEmit->AddEventToEventMap(iEventMap, evEmit) );

            // record the token movement
            evImp = TokenFromRid(evImp, mdtEvent);
            evEmit = TokenFromRid(evEmit, mdtEvent);

            IfFailGo( pCurTkMap->InsertNotFound(evImp, false, evEmit, &pTokenRec) );

            // copy over the method semantics
            IfFailGo( CopyMethodSemantics(pImportData, evImp, evEmit) );

            bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
               ((pImportData->m_tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdImport,
                    evImp, pImportData->m_tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

            if (!bSuppressMergeCheck) {
                pMTD->m_cEvents++;
            }
        }
    }

    // make sure we don't count any events if merge check is suppressed on the type
    _ASSERTE(pMTD->m_cEvents == 0 || !pMTD->m_bSuppressMergeCheck);
ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyEvents()


//*****************************************************************************
// Copy Properties given a TypeDef
//*****************************************************************************
HRESULT NEWMERGER::CopyProperties(
    MergeImportData *pImportData, 
    mdTypeDef       tdImport, 
    mdTypeDef       tdEmit)
{
    HRESULT     hr = NOERROR;
    CMiniMdRW   *pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();
    RID         ridPropertyMap;
    PropertyMapRec *pPropertyMapRec;    
    PropertyRec *pRecImport;
    PropertyRec *pRecEmit;
    ULONG       ridStart;
    ULONG       ridEnd;
    ULONG       i;
    mdProperty  prImp;
    mdProperty  prEmit;
    TOKENREC    *pTokenRec;
    ULONG       iPropertyMap;
    PropertyMapRec  *pPropertyMap;
    MDTOKENMAP  *pCurTkMap;

    MergeTypeData *pMTD;
    BOOL        bSuppressMergeCheck;
    mdCustomAttribute tkCA;

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

    pCurTkMap = pImportData->m_pMDTokenMap;

    pMTD = m_rMTDs.Get(RidFromToken(tdEmit));

    // make sure we didn't count the properties yet
    _ASSERTE(pMTD->m_cProperties == 0);

    ridPropertyMap = pMiniMdImport->FindPropertyMapFor(RidFromToken(tdImport));
    if (!InvalidRid(ridPropertyMap))
    {
        pPropertyMapRec = pMiniMdImport->getPropertyMap(ridPropertyMap);
        ridStart = pMiniMdImport->getPropertyListOfPropertyMap(pPropertyMapRec);
        ridEnd = pMiniMdImport->getEndPropertyListOfPropertyMap(pPropertyMapRec);

        if (ridEnd > ridStart)      
        {
            // If there is any event, create the PropertyMap record in the emit scope
            // Create new record.
            IfNullGo(pPropertyMap = pMiniMdEmit->AddPropertyMapRecord(&iPropertyMap));

            // Set parent.
            IfFailGo(pMiniMdEmit->PutToken(TBL_PropertyMap, PropertyMapRec::COL_Parent, pPropertyMap, tdEmit));
        }

        for (i = ridStart; i < ridEnd; i++)
        {
            // get the property rid
            prImp = pMiniMdImport->GetPropertyRid(i);

            // only merge those Properties that are marked
            if ( pMiniMdImport->GetFilterTable()->IsPropertyMarked(TokenFromRid(prImp, mdtProperty)) == false)
                continue;
            
            
            pRecImport = pMiniMdImport->getProperty(prImp);
            IfNullGo( pRecEmit = pMiniMdEmit->AddPropertyRecord((RID *)&prEmit) );

            // copy the property record over 
            IfFailGo( CopyProperty(pImportData, pRecImport, pRecEmit) );

            // Add Property to the PropertyMap.
            IfFailGo( pMiniMdEmit->AddPropertyToPropertyMap(iPropertyMap, prEmit) );

            // record the token movement
            prImp = TokenFromRid(prImp, mdtProperty);
            prEmit = TokenFromRid(prEmit, mdtProperty);

            IfFailGo( pCurTkMap->InsertNotFound(prImp, false, prEmit, &pTokenRec) );

            // copy over the method semantics
            IfFailGo( CopyMethodSemantics(pImportData, prImp, prEmit) );

            bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
               ((pImportData->m_tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdImport,
                    prImp, pImportData->m_tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

            if (!bSuppressMergeCheck) {
                pMTD->m_cProperties++;
            }
        }
    }

    // make sure we don't count any properties if merge check is suppressed on the type
    _ASSERTE(pMTD->m_cProperties == 0 || !pMTD->m_bSuppressMergeCheck);

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyProperties()


//*****************************************************************************
// Copy Parameters given a TypeDef
//*****************************************************************************
HRESULT NEWMERGER::CopyParams(
    MergeImportData *pImportData, 
    mdMethodDef     mdImport,   
    mdMethodDef     mdEmit)
{
    HRESULT     hr = NOERROR;
    ParamRec    *pRecImport = NULL;
    ParamRec    *pRecEmit = NULL;
    MethodRec   *pMethodRec;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    ULONG       ridStart, ridEnd;
    ULONG       i;
    mdParamDef  pdEmit;
    mdParamDef  pdImp;
    TOKENREC    *pTokenRec;
    MDTOKENMAP  *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pCurTkMap = pImportData->m_pMDTokenMap;


    pMethodRec = pMiniMdImport->getMethod(RidFromToken(mdImport));
    ridStart = pMiniMdImport->getParamListOfMethod(pMethodRec);
    ridEnd = pMiniMdImport->getEndParamListOfMethod(pMethodRec);

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

    // loop through all InterfaceImpl
    for (i = ridStart; i < ridEnd; i++)
    {
        // Get the param rid
        pdImp = pMiniMdImport->GetParamRid(i);

        // only merge those Params that are marked
        if ( pMiniMdImport->GetFilterTable()->IsParamMarked(TokenFromRid(pdImp, mdtParamDef)) == false)
            continue;
            
        
        pRecImport = pMiniMdImport->getParam(pdImp);
        IfNullGo( pRecEmit = pMiniMdEmit->AddParamRecord((RID *)&pdEmit) );

        // copy the Parameter record over 
        IfFailGo( CopyParam(pImportData, pRecImport, pRecEmit) );

        // warning!! warning!!
        // We cannot add paramRec to method list until it is fully set.
        // AddParamToMethod will use the ulSequence in the record
        IfFailGo( pMiniMdEmit->AddParamToMethod(RidFromToken(mdEmit), pdEmit));

        // record the token movement
        pdImp = TokenFromRid(pdImp, mdtParamDef);
        pdEmit = TokenFromRid(pdEmit, mdtParamDef);

        IfFailGo( pCurTkMap->InsertNotFound(pdImp, false, pdEmit, &pTokenRec) );
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyParams()


//*****************************************************************************
// Copy GenericParams given a TypeDef
//*****************************************************************************
HRESULT NEWMERGER::CopyGenericParams(
    MergeImportData *pImportData, 
    mdToken         tkImport, 
    mdToken         tkEmit)
{
    HRESULT         hr = NOERROR;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    TOKENREC        *pTokenRec;
    GenericParamRec *pRecImport = NULL;
    GenericParamRec *pRecEmit = NULL;
    MDTOKENMAP      *pCurTkMap;
    HENUMInternal   hEnum;
    mdGenericParam  gpImport;
    mdGenericParam  gpEmit;
    LPCSTR          szGenericParamName;

    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pMiniMdEmit = GetMiniMdEmit();
    pCurTkMap = pImportData->m_pMDTokenMap;

    IfFailGo( pMiniMdImport->FindGenericParamHelper(tkImport, &hEnum) );
    
    while (HENUMInternal::EnumNext(&hEnum, (mdToken *) &gpImport))
    {
        // Get the import GenericParam record
        _ASSERTE(TypeFromToken(gpImport) == mdtGenericParam);
        pRecImport = pMiniMdImport->getGenericParam(RidFromToken(gpImport));
        
        // Create new emit record.
        IfNullGo( pRecEmit = pMiniMdEmit->AddGenericParamRecord((RID *)&gpEmit) );

        // copy the GenericParam content 
        pRecEmit->SetNumber( pRecImport->GetNumber());
        pRecEmit->SetFlags( pRecImport->GetFlags());
        
        IfFailGo( pMiniMdEmit->PutToken(TBL_GenericParam, GenericParamRec::COL_Owner, pRecEmit, tkEmit));
        
        szGenericParamName = pMiniMdImport->getNameOfGenericParam(pRecImport);
        IfFailGo( pMiniMdEmit->PutString(TBL_GenericParam, GenericParamRec::COL_Name, pRecEmit, szGenericParamName));
        
        // record the token movement
        gpImport = TokenFromRid(gpImport, mdtGenericParam);
        gpEmit = TokenFromRid(gpEmit, mdtGenericParam);

        IfFailGo( pCurTkMap->InsertNotFound(gpImport, false, gpEmit, &pTokenRec) );

        // copy over any constraints
        IfFailGo( CopyGenericParamConstraints(pImportData, gpImport, gpEmit) );
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyGenericParams()


//*****************************************************************************
// Copy GenericParamConstraints given a GenericParam
//*****************************************************************************
HRESULT NEWMERGER::CopyGenericParamConstraints(
    MergeImportData *pImportData, 
    mdGenericParamConstraint tkImport, 
    mdGenericParamConstraint tkEmit)
{
    HRESULT         hr = NOERROR;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    TOKENREC        *pTokenRec;
    GenericParamConstraintRec *pRecImport = NULL;
    GenericParamConstraintRec *pRecEmit = NULL;
    MDTOKENMAP      *pCurTkMap;
    HENUMInternal   hEnum;
    mdGenericParamConstraint  gpImport;
    mdGenericParamConstraint  gpEmit;
    mdToken         tkConstraint;

    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pMiniMdEmit = GetMiniMdEmit();
    pCurTkMap = pImportData->m_pMDTokenMap;

    IfFailGo( pMiniMdImport->FindGenericParamConstraintHelper(tkImport, &hEnum) );
    
    while (HENUMInternal::EnumNext(&hEnum, (mdToken *) &gpImport))
    {
        // Get the import GenericParam record
        _ASSERTE(TypeFromToken(gpImport) == mdtGenericParamConstraint);
        pRecImport = pMiniMdImport->getGenericParamConstraint(RidFromToken(gpImport));
        
        // Translate the constraint before creating new record.
        tkConstraint = pMiniMdImport->getConstraintOfGenericParamConstraint(pRecImport);
        if (pCurTkMap->Find(tkConstraint, &pTokenRec) == false)
        {
            // This should never fire unless the TypeDefs/Refs weren't merged
            // before this code runs.
            _ASSERTE(!"GenericParamConstraint Constraint not found in MERGER::CopyGenericParamConstraints.  Bad state!");
            IfFailGo( META_E_BADMETADATA );
        }
        tkConstraint = pTokenRec->m_tkTo;

        // Create new emit record.
        IfNullGo( pRecEmit = pMiniMdEmit->AddGenericParamConstraintRecord((RID *)&gpEmit) );

        // copy the GenericParamConstraint content 
        IfFailGo( pMiniMdEmit->PutToken(TBL_GenericParamConstraint, GenericParamConstraintRec::COL_Owner, pRecEmit, tkEmit));
        
        IfFailGo( pMiniMdEmit->PutToken(TBL_GenericParamConstraint, GenericParamConstraintRec::COL_Constraint, pRecEmit, tkConstraint));
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyGenericParamConstraints()


//*****************************************************************************
// Verify GenericParams given a TypeDef
//*****************************************************************************
HRESULT NEWMERGER::VerifyGenericParams(
    MergeImportData *pImportData, 
    mdToken         tkImport, 
    mdToken         tkEmit)
{
    HRESULT         hr = NOERROR;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    TOKENREC        *pTokenRec;
    MDTOKENMAP      *pCurTkMap;
    HENUMInternal   hEnumImport;        // Enumerator for import scope.
    HENUMInternal   hEnumEmit;          // Enumerator for emit scope.
    ULONG           cImport, cEmit;     // Count of import & emit records.
    ULONG           i;                  // Enumerating records in import scope.
    ULONG           iEmit;              // Tracking records in emit scope.
    mdGenericParam  gpImport;           // Import scope GenericParam token.
    mdGenericParam  gpEmit;             // Emit scope GenericParam token.
    GenericParamRec *pRecImport = NULL;
    GenericParamRec *pRecEmit = NULL;
    LPCSTR          szNameImport;       // Name of param in import scope.
    LPCSTR          szNameEmit;         // Name of param in emit scope.

    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pMiniMdEmit = GetMiniMdEmit();
    pCurTkMap = pImportData->m_pMDTokenMap;

    // Get enumerators for the input and output scopes.
    IfFailGo( pMiniMdImport->FindGenericParamHelper(tkImport, &hEnumImport) );
    IfFailGo( pMiniMdEmit->FindGenericParamHelper(tkEmit, &hEnumEmit) );
    
    // The counts should be the same.
    HENUMInternal::GetCount(&hEnumImport, &cImport);
    HENUMInternal::GetCount(&hEnumEmit, &cEmit);

    if (cImport != cEmit)
    {
        CheckContinuableErrorEx(META_E_GENERICPARAM_INCONSISTENT, pImportData, tkImport);
        // If we are here, the linker says this error is OK.
    }

    for (i=iEmit=0; i<cImport; ++i)
    {
        // Get the import GenericParam record
        IfFailGo( HENUMInternal::GetElement(&hEnumImport, i, &gpImport));
        _ASSERTE(TypeFromToken(gpImport) == mdtGenericParam);
        pRecImport = pMiniMdImport->getGenericParam(RidFromToken(gpImport));
        
        // Find the emit record.  If the import and emit scopes are ordered the same
        //  this is easy; otherwise go looking for it.
        // Get the "next" emit record.
        if (iEmit < cEmit)
        {
            IfFailGo( HENUMInternal::GetElement(&hEnumEmit, iEmit, &gpEmit));
            _ASSERTE(TypeFromToken(gpEmit) == mdtGenericParam);
            pRecEmit = pMiniMdEmit->getGenericParam(RidFromToken(gpEmit));
        }

        // If the import and emit sequence numbers don't match, go looking.
        //  Also, if we would have walked off end of array, go looking.
        if (iEmit >= cEmit || pRecImport->GetNumber() != pRecEmit->GetNumber())
        {
            for (iEmit=0; iEmit<cEmit; ++iEmit)
            {
                IfFailGo( HENUMInternal::GetElement(&hEnumEmit, iEmit, &gpEmit));
                _ASSERTE(TypeFromToken(gpEmit) == mdtGenericParam);
                pRecEmit = pMiniMdEmit->getGenericParam(RidFromToken(gpEmit));

                // The one we want?
                if (pRecImport->GetNumber() == pRecEmit->GetNumber())
                    break;
            }
            if (iEmit >= cEmit)
                goto Error; // Didn't find it
        }

        // Check that these "n'th" GenericParam records match.

        // Flags.
        if (pRecImport->GetFlags() != pRecEmit->GetFlags())
            goto Error;

        // Name.
        szNameImport = pMiniMdImport->getNameOfGenericParam(pRecImport);
        szNameEmit = pMiniMdEmit->getNameOfGenericParam(pRecEmit);
        if (strcmp(szNameImport, szNameEmit) != 0)
            goto Error;

        // Verify any constraints.
        gpImport = TokenFromRid(gpImport, mdtGenericParam);
        gpEmit = TokenFromRid(gpEmit, mdtGenericParam);
        hr =  VerifyGenericParamConstraints(pImportData, gpImport, gpEmit);

        if (SUCCEEDED(hr))
        {
            // record the token movement
            IfFailGo( pCurTkMap->InsertNotFound(gpImport, true, gpEmit, &pTokenRec) );
        }
        else
        {
Error:
            // inconsistent in GenericParams
            hr = S_OK; // discard old error; new error will be returned from CheckContinuableError
            CheckContinuableErrorEx(META_E_GENERICPARAM_INCONSISTENT, pImportData, tkImport);
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::VerifyGenericParams()


//*****************************************************************************
// Verify GenericParamConstraints given a GenericParam
//*****************************************************************************
HRESULT NEWMERGER::VerifyGenericParamConstraints(
    MergeImportData *pImportData,           // The import scope.
    mdGenericParam  gpImport,               // Import GenericParam.
    mdGenericParam  gpEmit)                 // Emit GenericParam.
{
    HRESULT         hr = NOERROR;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    TOKENREC        *pTokenRec;
    HENUMInternal   hEnumImport;        // Enumerator for import scope.
    HENUMInternal   hEnumEmit;          // Enumerator for emit scope.
    ULONG           cImport, cEmit;     // Count of import & emit records.
    ULONG           i;                  // Enumerating records in import scope.
    ULONG           iEmit;              // Tracking records in emit scope.
    GenericParamConstraintRec *pRecImport = NULL;
    GenericParamConstraintRec *pRecEmit = NULL;
    MDTOKENMAP      *pCurTkMap;
    mdToken         tkConstraintImport = mdTokenNil;
    mdToken         tkConstraintEmit = mdTokenNil;

    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pMiniMdEmit = GetMiniMdEmit();
    pCurTkMap = pImportData->m_pMDTokenMap;

    // Get enumerators for the input and output scopes.
    IfFailGo( pMiniMdImport->FindGenericParamConstraintHelper(gpImport, &hEnumImport) );
    IfFailGo( pMiniMdEmit->FindGenericParamConstraintHelper(gpEmit, &hEnumEmit) );
    
    // The counts should be the same.
    HENUMInternal::GetCount(&hEnumImport, &cImport);
    HENUMInternal::GetCount(&hEnumEmit, &cEmit);

    if (cImport != cEmit)
        IfFailGo(META_E_GENERICPARAM_INCONSISTENT); // Different numbers of constraints.

    for (i=iEmit=0; i<cImport; ++i)
    {
        // Get the import GenericParam record
        IfFailGo( HENUMInternal::GetElement(&hEnumImport, i, &gpImport));
        _ASSERTE(TypeFromToken(gpImport) == mdtGenericParamConstraint);
        pRecImport = pMiniMdImport->getGenericParamConstraint(RidFromToken(gpImport));
        
        // Get the constraint.
        tkConstraintImport = pMiniMdImport->getConstraintOfGenericParamConstraint(pRecImport);
        if (pCurTkMap->Find(tkConstraintImport, &pTokenRec) == false)
        {
            // This should never fire unless the TypeDefs/Refs weren't merged
            // before this code runs.
            _ASSERTE(!"GenericParamConstraint Constraint not found in MERGER::VerifyGenericParamConstraints.  Bad state!");
            IfFailGo( META_E_BADMETADATA );
        }
        tkConstraintImport = pTokenRec->m_tkTo;
        
        // Find the emit record.  If the import and emit scopes are ordered the same
        //  this is easy; otherwise go looking for it.
        // Get the "next" emit record.
        if (iEmit < cEmit)
        {
            IfFailGo( HENUMInternal::GetElement(&hEnumEmit, iEmit, &gpEmit));
            _ASSERTE(TypeFromToken(gpEmit) == mdtGenericParamConstraint);
            pRecEmit = pMiniMdEmit->getGenericParamConstraint(RidFromToken(gpEmit));
            tkConstraintEmit = pMiniMdEmit->getConstraintOfGenericParamConstraint(pRecEmit);
        }

        // If the import and emit constraints don't match, go looking.
        if (iEmit >= cEmit || tkConstraintEmit != tkConstraintImport)
        {
            for (iEmit=0; iEmit<cEmit; ++iEmit)
            {
                IfFailGo( HENUMInternal::GetElement(&hEnumEmit, iEmit, &gpEmit));
                _ASSERTE(TypeFromToken(gpEmit) == mdtGenericParamConstraint);
                pRecEmit = pMiniMdEmit->getGenericParamConstraint(RidFromToken(gpEmit));
                tkConstraintEmit = pMiniMdEmit->getConstraintOfGenericParamConstraint(pRecEmit);

                // The one we want?
                if (tkConstraintEmit == tkConstraintImport)
                    break;
            }
            if (iEmit >= cEmit)
            {
                IfFailGo(META_E_GENERICPARAM_INCONSISTENT); // Didn't find the constraint
            }
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::VerifyGenericParamConstraints()


//*****************************************************************************
// Verify Methods
//*****************************************************************************
HRESULT NEWMERGER::VerifyMethods(
    MergeImportData *pImportData, 
    mdTypeDef       tdImport, 
    mdTypeDef       tdEmit)
{
    HRESULT     hr = NOERROR;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    MethodRec   *pRecImp;
    MethodRec   *pRecEmit;
    ULONG       ridStart;
    ULONG       ridEnd;
    ULONG       i;
    
    TypeDefRec  *pTypeDefRec;
    LPCUTF8     szName;
    PCCOR_SIGNATURE pbSig;
    ULONG       cbSig;
    ULONG       cbEmit;
    CQuickBytes qbSig;
    TOKENREC    *pTokenRec;
    mdMethodDef mdImp;
    mdMethodDef mdEmit;
    MDTOKENMAP  *pCurTkMap;

    MergeTypeData *pMTD;
    BOOL        bSuppressMergeCheck;
    ULONG       cImport = 0;        // count of non-merge check suppressed methods
    mdCustomAttribute tkCA;

    pMiniMdEmit = GetMiniMdEmit();
    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pCurTkMap = pImportData->m_pMDTokenMap;

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

    // Get a count of records in the import scope; prepare to enumerate them.
    pTypeDefRec = pMiniMdImport->getTypeDef(RidFromToken(tdImport));
    ridStart = pMiniMdImport->getMethodListOfTypeDef(pTypeDefRec);
    ridEnd = pMiniMdImport->getEndMethodListOfTypeDef(pTypeDefRec);

    pMTD = m_rMTDs.Get(RidFromToken(tdEmit));

    // loop through all Methods of the TypeDef
    for (i = ridStart; i < ridEnd; i++)
    {
        mdImp = pMiniMdImport->GetMethodRid(i);

        // only verify those Methods that are marked
        if ( pMiniMdImport->GetFilterTable()->IsMethodMarked(TokenFromRid(mdImp, mdtMethodDef)) == false)
            continue;
            
        pRecImp = pMiniMdImport->getMethod(mdImp);

        if (m_fDupCheck == FALSE && tdImport == pImportData->m_pRegMetaImport->m_tdModule) //   TokenFromRid(1, mdtTypeDef))
        {
            // No dup check. This is the scenario that we only have one import scope. Just copy over the
            // globals.
            goto CopyMethodLabel;
        }
          
        szName = pMiniMdImport->getNameOfMethod(pRecImp);
        pbSig = pMiniMdImport->getSignatureOfMethod(pRecImp, &cbSig);

        mdImp = TokenFromRid(mdImp, mdtMethodDef);

        if ( IsMdPrivateScope( pRecImp->GetFlags() ) )
        {
            // Trigger additive merge
            goto CopyMethodLabel;
        }

        // convert rid contained in signature to new scope
        IfFailGo(ImportHelper::MergeUpdateTokenInSig(    
            NULL,                       // Assembly emit scope.
            pMiniMdEmit,                // The emit scope.
            NULL, NULL, 0,              // Import assembly scope information.
            pMiniMdImport,              // The scope to merge into the emit scope.
            pbSig,                      // signature from the imported scope
            pCurTkMap,                // Internal token mapping structure.
            &qbSig,                     // [OUT] translated signature
            0,                          // start from first byte of the signature
            0,                          // don't care how many bytes consumed
            &cbEmit));                  // number of bytes write to cbEmit

        hr = ImportHelper::FindMethod(
            pMiniMdEmit,
            tdEmit,
            szName,
            (const COR_SIGNATURE *)qbSig.Ptr(),
            cbEmit,
            &mdEmit );

        
        bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
               ((pImportData->m_tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdImport,
                    mdImp, pImportData->m_tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

        if (bSuppressMergeCheck || (tdImport == pImportData->m_pRegMetaImport->m_tdModule))
        {
            // global functions! Make sure that we move over the non-duplicate global function
            // declaration
            //
            if (hr == S_OK)
            {
                // found the duplicate
                IfFailGo( VerifyMethod(pImportData, mdImp, mdEmit) );
            }
            else
            {
CopyMethodLabel:
                // not a duplicate! Copy over the 
                IfNullGo( pRecEmit = pMiniMdEmit->AddMethodRecord((RID *)&mdEmit) );

                // copy the method content over
                IfFailGo( CopyMethod(pImportData, pRecImp, pRecEmit) );

                IfFailGo( pMiniMdEmit->AddMethodToTypeDef(RidFromToken(tdEmit), mdEmit));

                // record the token movement
                mdEmit = TokenFromRid(mdEmit, mdtMethodDef);
                IfFailGo( pMiniMdEmit->AddMemberDefToHash(
                    mdEmit, 
                    tdEmit) ); 

                mdImp = TokenFromRid(mdImp, mdtMethodDef);
                IfFailGo( pCurTkMap->InsertNotFound(mdImp, false, mdEmit, &pTokenRec) );

                // copy over the children
                IfFailGo( CopyParams(pImportData, mdImp, mdEmit) );
                IfFailGo( CopyGenericParams(pImportData, mdImp, mdEmit) );

            }
        }
        else
        {
            if (hr == S_OK)
            {
                // Good! We are supposed to find a duplicate
                IfFailGo( VerifyMethod(pImportData, mdImp, mdEmit) );
            }
            else
            {
                // Oops! The typedef is duplicated but the method is not!!
                hr = S_OK; // discard old error; new error will be returned from CheckContinuableError
                CheckContinuableErrorEx(META_E_METHD_NOT_FOUND, pImportData, mdImp);
            }
            
            cImport++;
        }
    }

    // The counts should be the same, unless this is <module>
    if (cImport != pMTD->m_cMethods && tdImport != pImportData->m_pRegMetaImport->m_tdModule)
    {
        CheckContinuableErrorEx(META_E_METHOD_COUNTS, pImportData, tdImport);
        // If we are here, the linker says this error is OK.
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::VerifyMethods()


//*****************************************************************************
// verify a duplicated method
//*****************************************************************************
HRESULT NEWMERGER::VerifyMethod(
    MergeImportData *pImportData, 
    mdMethodDef mdImp,                      // [IN] the emit record to fill
    mdMethodDef mdEmit)                     // [IN] the record to import
{
    HRESULT     hr;
    MethodRec   *pRecImp;
    MethodRec   *pRecEmit;
    TOKENREC    *pTokenRec;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    MDTOKENMAP  *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pCurTkMap = pImportData->m_pMDTokenMap;

    IfFailGo( pCurTkMap->InsertNotFound(mdImp, true, mdEmit, &pTokenRec) );
    
    pRecImp = pMiniMdImport->getMethod(RidFromToken(mdImp));

    // We need to make sure that the impl flags are propagated .
    // Rules are: if the first method has miForwardRef flag set but the new method does not,
    // we want to disable the miForwardRef flag. If the one found in the emit scope does not have
    // miForwardRef set and the second one doesn't either, we want to make sure that the rest of
    // impl flags are the same.
    //
    if ( !IsMiForwardRef( pRecImp->GetImplFlags() ) )
    {
        pRecEmit = pMiniMdEmit->getMethod(RidFromToken(mdEmit));
        if (!IsMiForwardRef(pRecEmit->GetImplFlags()))
        {
            // make sure the rest of ImplFlags are the same
            if (pRecEmit->GetImplFlags() != pRecImp->GetImplFlags())
            {
                // inconsistent in implflags
                CheckContinuableErrorEx(META_E_METHDIMPL_INCONSISTENT, pImportData, mdImp);
            }
        }
        else
        {
            // propagate the importing ImplFlags
            pRecEmit->SetImplFlags(pRecImp->GetImplFlags());
        }
    }

    // verify the children
    IfFailGo( VerifyParams(pImportData, mdImp, mdEmit) );
    IfFailGo( VerifyGenericParams(pImportData, mdImp, mdEmit) );

ErrExit:
    return hr;
} // HRESULT NEWMERGER::VerifyMethod()


//*****************************************************************************
// Verify Fields
//*****************************************************************************
HRESULT NEWMERGER::VerifyFields(
    MergeImportData *pImportData, 
    mdTypeDef       tdImport, 
    mdTypeDef       tdEmit)
{
    HRESULT     hr = NOERROR;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    FieldRec    *pRecImp;
    FieldRec    *pRecEmit;
    mdFieldDef  fdImp;
    mdFieldDef  fdEmit;
    ULONG       ridStart;
    ULONG       ridEnd;
    ULONG       i;

    TypeDefRec  *pTypeDefRec;
    LPCUTF8     szName;
    PCCOR_SIGNATURE pbSig;
    ULONG       cbSig;
    ULONG       cbEmit;
    CQuickBytes qbSig;
    TOKENREC    *pTokenRec;
    MDTOKENMAP  *pCurTkMap;

    MergeTypeData *pMTD;
    BOOL        bSuppressMergeCheck;
    ULONG       cImport = 0;        // count of non-merge check suppressed fields
    mdCustomAttribute tkCA;

    pMiniMdEmit = GetMiniMdEmit();
    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pCurTkMap = pImportData->m_pMDTokenMap;

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );
   
    // Get a count of records in the import scope; prepare to enumerate them.
    pTypeDefRec = pMiniMdImport->getTypeDef(RidFromToken(tdImport));
    ridStart = pMiniMdImport->getFieldListOfTypeDef(pTypeDefRec);
    ridEnd = pMiniMdImport->getEndFieldListOfTypeDef(pTypeDefRec);
    
    pMTD = m_rMTDs.Get(RidFromToken(tdEmit));
 
    // loop through all fields of the TypeDef
    for (i = ridStart; i < ridEnd; i++)
    {
        fdImp = pMiniMdImport->GetFieldRid(i);

        // only verify those fields that are marked
        if ( pMiniMdImport->GetFilterTable()->IsFieldMarked(TokenFromRid(fdImp, mdtFieldDef)) == false)
            continue;

        pRecImp = pMiniMdImport->getField(fdImp);

        if (m_fDupCheck == FALSE && tdImport == pImportData->m_pRegMetaImport->m_tdModule)
        {
            // No dup check. This is the scenario that we only have one import scope. Just copy over the
            // globals.
            goto CopyFieldLabel;
        }

        szName = pMiniMdImport->getNameOfField(pRecImp);
        pbSig = pMiniMdImport->getSignatureOfField(pRecImp, &cbSig);

        if ( IsFdPrivateScope(pRecImp->GetFlags()))
        {
            // Trigger additive merge
            fdImp = TokenFromRid(fdImp, mdtFieldDef);
            goto CopyFieldLabel;
        }

        // convert rid contained in signature to new scope
        IfFailGo(ImportHelper::MergeUpdateTokenInSig(    
            NULL,                       // Assembly emit scope.
            pMiniMdEmit,                // The emit scope.
            NULL, NULL, 0,              // Import assembly scope information.
            pMiniMdImport,              // The scope to merge into the emit scope.
            pbSig,                      // signature from the imported scope
            pCurTkMap,                // Internal token mapping structure.
            &qbSig,                     // [OUT] translated signature
            0,                          // start from first byte of the signature
            0,                          // don't care how many bytes consumed
            &cbEmit));                  // number of bytes write to cbEmit

        hr = ImportHelper::FindField(
            pMiniMdEmit,
            tdEmit,
            szName,
            (const COR_SIGNATURE *)qbSig.Ptr(),
            cbEmit,
            &fdEmit );

        fdImp = TokenFromRid(fdImp, mdtFieldDef);

        bSuppressMergeCheck = 
            (IsFdStatic(pRecImp->GetFlags()) && pMTD->m_bSuppressMergeCheck) ||
               ((pImportData->m_tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdImport,
                    fdImp, pImportData->m_tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

        if (bSuppressMergeCheck || (tdImport == pImportData->m_pRegMetaImport->m_tdModule))
        {
            // global data! Make sure that we move over the non-duplicate global function
            // declaration
            //
            if (hr == S_OK)
            {
                // found the duplicate
                IfFailGo( pCurTkMap->InsertNotFound(fdImp, true, fdEmit, &pTokenRec) );
            }
            else
            {
CopyFieldLabel:
                // not a duplicate! Copy over the 
                IfNullGo( pRecEmit = pMiniMdEmit->AddFieldRecord((RID *)&fdEmit) );

                // copy the field record over 
                IfFailGo( CopyField(pImportData, pRecImp, pRecEmit) );

                IfFailGo( pMiniMdEmit->AddFieldToTypeDef(RidFromToken(tdEmit), fdEmit));

                // record the token movement
                fdEmit = TokenFromRid(fdEmit, mdtFieldDef);
                IfFailGo( pMiniMdEmit->AddMemberDefToHash(
                    fdEmit, 
                    tdEmit) ); 

                fdImp = TokenFromRid(fdImp, mdtFieldDef);
                IfFailGo( pCurTkMap->InsertNotFound(fdImp, false, fdEmit, &pTokenRec) );
            }
        }
        else
        {
            if (hr == S_OK)
            {
                // Good! We are supposed to find a duplicate
                IfFailGo( pCurTkMap->InsertNotFound(fdImp, true, fdEmit, &pTokenRec) );
            }
            else
            {
                // Oops! The typedef is duplicated but the field is not!!
                hr = S_OK; // discard old error; new error will be returned from CheckContinuableError
                CheckContinuableErrorEx(META_E_FIELD_NOT_FOUND, pImportData, fdImp);
            }
            
            cImport++;
        }
    }

    // The counts should be the same, unless this is <module>
    if (cImport != pMTD->m_cFields && tdImport != pImportData->m_pRegMetaImport->m_tdModule)
    {
        CheckContinuableErrorEx(META_E_FIELD_COUNTS, pImportData, tdImport);
        // If we are here, the linker says this error is OK.
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::VerifyFields()


//*****************************************************************************
// Verify Events
//*****************************************************************************
HRESULT NEWMERGER::VerifyEvents(
    MergeImportData *pImportData, 
    mdTypeDef       tdImport, 
    mdTypeDef       tdEmit)
{
    HRESULT     hr = NOERROR;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    RID         ridEventMap, ridEventMapEmit;
    EventMapRec *pEventMapRec;  
    EventRec    *pRecImport;
    ULONG       ridStart;
    ULONG       ridEnd;
    ULONG       i;
    mdEvent     evImport;
    mdEvent     evEmit;
    TOKENREC    *pTokenRec;
    LPCUTF8     szName;
    mdToken     tkType;
    MDTOKENMAP  *pCurTkMap;

    EventMapRec *pEventMapEmit;  
    EventRec    *pRecEmit;
    MergeTypeData *pMTD;
    BOOL        bSuppressMergeCheck;
    ULONG       cImport = 0;        // count of non-merge check suppressed events
    mdCustomAttribute tkCA;

    pMiniMdEmit = GetMiniMdEmit();
    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pCurTkMap = pImportData->m_pMDTokenMap;

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

    ridEventMap = pMiniMdImport->FindEventMapFor(RidFromToken(tdImport));
    if (!InvalidRid(ridEventMap))
    {
        // Get a count of records already in emit scope.
        ridEventMapEmit = pMiniMdEmit->FindEventMapFor(RidFromToken(tdEmit));

        if (InvalidRid(ridEventMapEmit)) {
            // If there is any event, create the eventmap record in the emit scope
            // Create new record.
            IfNullGo(pEventMapEmit = pMiniMdEmit->AddEventMapRecord(&ridEventMapEmit));

            // Set parent.
            IfFailGo(pMiniMdEmit->PutToken(TBL_EventMap, EventMapRec::COL_Parent, pEventMapEmit, tdEmit));
        }

        // Get a count of records in the import scope; prepare to enumerate them.
        pEventMapRec = pMiniMdImport->getEventMap(ridEventMap);
        ridStart = pMiniMdImport->getEventListOfEventMap(pEventMapRec);
        ridEnd = pMiniMdImport->getEndEventListOfEventMap(pEventMapRec);

        pMTD = m_rMTDs.Get(RidFromToken(tdEmit));

        for (i = ridStart; i < ridEnd; i++)
        {
            // get the property rid
            evImport = pMiniMdImport->GetEventRid(i);

            // only verify those Events that are marked
            if ( pMiniMdImport->GetFilterTable()->IsEventMarked(TokenFromRid(evImport, mdtEvent)) == false)
                continue;
            
            pRecImport = pMiniMdImport->getEvent(evImport);
            szName = pMiniMdImport->getNameOfEvent(pRecImport);
            tkType = pMiniMdImport->getEventTypeOfEvent( pRecImport );
            IfFailGo( pCurTkMap->Remap(tkType, &tkType) );
            evImport = TokenFromRid( evImport, mdtEvent);         

            hr = ImportHelper::FindEvent(
                pMiniMdEmit,
                tdEmit,
                szName,
                &evEmit);

            bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
               ((pImportData->m_tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdImport,
                    evImport, pImportData->m_tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));

            if (bSuppressMergeCheck) 
            {

                if (hr == S_OK )
                {
                    // Good. We found the matching event when we have a duplicate typedef
                    IfFailGo( pCurTkMap->InsertNotFound(evImport, true, evEmit, &pTokenRec) );
                }
                else
                {   
                    // not a duplicate! Copy over the 
                    IfNullGo( pRecEmit = pMiniMdEmit->AddEventRecord((RID *)&evEmit) );

                    // copy the event record over 
                    IfFailGo( CopyEvent(pImportData, pRecImport, pRecEmit) );
                    
                    // Add Event to the EventMap.
                    IfFailGo( pMiniMdEmit->AddEventToEventMap(ridEventMapEmit, evEmit) );

                    // record the token movement
                    evEmit = TokenFromRid(evEmit, mdtEvent);

                    IfFailGo( pCurTkMap->InsertNotFound(evImport, false, evEmit, &pTokenRec) );

                    // copy over the method semantics
                    IfFailGo( CopyMethodSemantics(pImportData, evImport, evEmit) );
                }
            }
            else
            {
                if (hr == S_OK )
                {
                    // Good. We found the matching event when we have a duplicate typedef
                    IfFailGo( pCurTkMap->InsertNotFound(evImport, true, evEmit, &pTokenRec) );
                }
                else
                {
                    // Oops! The typedef is duplicated but the event is not!!
                    hr = S_OK; // discard old error; new error will be returned from CheckContinuableError
                    CheckContinuableErrorEx(META_E_EVENT_NOT_FOUND, pImportData, evImport);

                }

                cImport++;
            }
        }

        // The counts should be the same, unless this is <module>
        if (cImport != pMTD->m_cEvents && tdImport != pImportData->m_pRegMetaImport->m_tdModule)
        {
            CheckContinuableErrorEx(META_E_EVENT_COUNTS, pImportData, tdImport);
            // If we are here, the linker says this error is OK.
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::VerifyEvents()


//*****************************************************************************
// Verify Properties
//*****************************************************************************
HRESULT NEWMERGER::VerifyProperties(
    MergeImportData *pImportData, 
    mdTypeDef       tdImport, 
    mdTypeDef       tdEmit)
{
    HRESULT     hr = NOERROR;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    RID         ridPropertyMap, ridPropertyMapEmit;
    PropertyMapRec *pPropertyMapRec;    
    PropertyRec *pRecImport;
    ULONG       ridStart;
    ULONG       ridEnd;
    ULONG       i;
    mdProperty  prImp;
    mdProperty  prEmit;
    TOKENREC    *pTokenRec;
    LPCUTF8     szName;
    PCCOR_SIGNATURE pbSig;
    ULONG       cbSig;
    ULONG       cbEmit;
    CQuickBytes qbSig;
    MDTOKENMAP  *pCurTkMap;

    PropertyMapRec *pPropertyMapEmit;  
    PropertyRec *pRecEmit;
    MergeTypeData *pMTD;
    BOOL        bSuppressMergeCheck;
    ULONG       cImport = 0;        // count of non-merge check suppressed properties
    mdCustomAttribute tkCA;

    pMiniMdEmit = GetMiniMdEmit();
    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pCurTkMap = pImportData->m_pMDTokenMap;

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

    ridPropertyMap = pMiniMdImport->FindPropertyMapFor(RidFromToken(tdImport));
    if (!InvalidRid(ridPropertyMap))
    {
        // Get a count of records already in emit scope.
        ridPropertyMapEmit = pMiniMdEmit->FindPropertyMapFor(RidFromToken(tdEmit));

        if (InvalidRid(ridPropertyMapEmit))      
        {
            // If there is any event, create the PropertyMap record in the emit scope
            // Create new record.
            IfNullGo(pPropertyMapEmit = pMiniMdEmit->AddPropertyMapRecord(&ridPropertyMapEmit));

            // Set parent.
            IfFailGo(pMiniMdEmit->PutToken(TBL_PropertyMap, PropertyMapRec::COL_Parent, pPropertyMapEmit, tdEmit));
        }

        // Get a count of records in the import scope; prepare to enumerate them.
        pPropertyMapRec = pMiniMdImport->getPropertyMap(ridPropertyMap);
        ridStart = pMiniMdImport->getPropertyListOfPropertyMap(pPropertyMapRec);
        ridEnd = pMiniMdImport->getEndPropertyListOfPropertyMap(pPropertyMapRec);

        pMTD = m_rMTDs.Get(RidFromToken(tdEmit));

        for (i = ridStart; i < ridEnd; i++)
        {
            // get the property rid
            prImp = pMiniMdImport->GetPropertyRid(i);

            // only verify those Properties that are marked
            if ( pMiniMdImport->GetFilterTable()->IsPropertyMarked(TokenFromRid(prImp, mdtProperty)) == false)
                continue;
                        
            pRecImport = pMiniMdImport->getProperty(prImp);
            szName = pMiniMdImport->getNameOfProperty(pRecImport);
            pbSig = pMiniMdImport->getTypeOfProperty( pRecImport, &cbSig );
            prImp = TokenFromRid( prImp, mdtProperty);

            // convert rid contained in signature to new scope
            IfFailGo( ImportHelper::MergeUpdateTokenInSig(    
                NULL,                       // Emit assembly.
                pMiniMdEmit,                // The emit scope.
                NULL, NULL, 0,              // Import assembly scope information.
                pMiniMdImport,              // The scope to merge into the emit scope.
                pbSig,                      // signature from the imported scope
                pCurTkMap,                // Internal token mapping structure.
                &qbSig,                     // [OUT] translated signature
                0,                          // start from first byte of the signature
                0,                          // don't care how many bytes consumed
                &cbEmit) );                 // number of bytes write to cbEmit

            hr = ImportHelper::FindProperty(
                pMiniMdEmit,
                tdEmit,
                szName,
                (PCCOR_SIGNATURE) qbSig.Ptr(),
                cbEmit,
                &prEmit);

            bSuppressMergeCheck = pMTD->m_bSuppressMergeCheck ||
               ((pImportData->m_tkSuppressMergeCheckCtor != mdTokenNil) &&
                (S_OK == ImportHelper::FindCustomAttributeByToken(pMiniMdImport,
                    prImp, pImportData->m_tkSuppressMergeCheckCtor, NULL, 0, &tkCA)));
            
            if (bSuppressMergeCheck) 
            {
                if (hr == S_OK)
                {
                    // Good. We found the matching property when we have a duplicate typedef
                    IfFailGo( pCurTkMap->InsertNotFound(prImp, true, prEmit, &pTokenRec) );
                }
                else
                {
                    IfNullGo( pRecEmit = pMiniMdEmit->AddPropertyRecord((RID *)&prEmit) );

                    // copy the property record over 
                    IfFailGo( CopyProperty(pImportData, pRecImport, pRecEmit) );

                    // Add Property to the PropertyMap.
                    IfFailGo( pMiniMdEmit->AddPropertyToPropertyMap(ridPropertyMapEmit, prEmit) );

                    // record the token movement
                    prEmit = TokenFromRid(prEmit, mdtProperty);

                    IfFailGo( pCurTkMap->InsertNotFound(prImp, false, prEmit, &pTokenRec) );

                    // copy over the method semantics
                    IfFailGo( CopyMethodSemantics(pImportData, prImp, prEmit) );
                }
            }
            else
            {
                if (hr == S_OK)
                {
                    // Good. We found the matching property when we have a duplicate typedef
                    IfFailGo( pCurTkMap->InsertNotFound(prImp, true, prEmit, &pTokenRec) );
                }
                else
                {
                    hr = S_OK; // discard old error; new error will be returned from CheckContinuableError
                    CheckContinuableErrorEx(META_E_PROP_NOT_FOUND, pImportData, prImp);   
                }

                cImport++;
            }
        }

        // The counts should be the same, unless this is <module>
        if (cImport != pMTD->m_cProperties && tdImport != pImportData->m_pRegMetaImport->m_tdModule)
        {
            CheckContinuableErrorEx(META_E_PROPERTY_COUNTS, pImportData, tdImport);
            // If we are here, the linker says this error is OK.
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::VerifyProperties()


//*****************************************************************************
// Verify Parameters given a Method
//*****************************************************************************
HRESULT NEWMERGER::VerifyParams(
    MergeImportData *pImportData,   
    mdMethodDef     mdImport,   
    mdMethodDef     mdEmit)
{
    HRESULT     hr = NOERROR;
    ParamRec    *pRecImport = NULL;
    ParamRec    *pRecEmit = NULL;
    MethodRec   *pMethodRec;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    ULONG       ridStart, ridEnd;
    ULONG       ridStartEmit, ridEndEmit;
    ULONG       cImport, cEmit;
    ULONG       i, j;
    mdParamDef  pdEmit = 0;
    mdParamDef  pdImp;
    TOKENREC    *pTokenRec;
    MDTOKENMAP  *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    pCurTkMap = pImportData->m_pMDTokenMap;

    // We know that the filter table is not null here.  Tell PREFIX that we know it.
    PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

    // Get count of params in import scope; prepare to enumerate.
    pMethodRec = pMiniMdImport->getMethod(RidFromToken(mdImport));
    ridStart = pMiniMdImport->getParamListOfMethod(pMethodRec);
    ridEnd = pMiniMdImport->getEndParamListOfMethod(pMethodRec);
    cImport = ridEnd - ridStart;

    // Get count of params in emit scope; prepare to enumerate.
    pMethodRec = pMiniMdEmit->getMethod(RidFromToken(mdEmit));
    ridStartEmit = pMiniMdEmit->getParamListOfMethod(pMethodRec);
    ridEndEmit = pMiniMdEmit->getEndParamListOfMethod(pMethodRec);
    cEmit = ridEndEmit - ridStartEmit;
    
    // The counts should be the same.
    if (cImport != cEmit)
    {
        // That is, unless this is <module>, so get the method's parent.
        mdTypeDef tdImport;
        IfFailGo(pMiniMdImport->FindParentOfMethodHelper(mdImport, &tdImport));
        if (tdImport != pImportData->m_pRegMetaImport->m_tdModule)
            CheckContinuableErrorEx(META_E_PARAM_COUNTS, pImportData, mdImport);
            // If we are here, the linker says this error is OK.
    }

    // loop through all Parameters
    for (i = ridStart; i < ridEnd; i++)
    {
        // Get the importing param row
        pdImp = pMiniMdImport->GetParamRid(i);

        // only verify those Params that are marked
        if ( pMiniMdImport->GetFilterTable()->IsParamMarked(TokenFromRid(pdImp, mdtParamDef)) == false)
            continue;
            

        pRecImport = pMiniMdImport->getParam(pdImp);
        pdImp = TokenFromRid(pdImp, mdtParamDef);

        // It turns out when we merge a typelib with itself, the emit and import scope
        // has different sequence of parameter
        //
        // find the corresponding emit param row
        for (j = ridStartEmit; j < ridEndEmit; j++)
        {
            pdEmit = pMiniMdEmit->GetParamRid(j);
            pRecEmit = pMiniMdEmit->getParam(pdEmit);
            if (pRecEmit->GetSequence() == pRecImport->GetSequence())
                break;
        }

        if (j == ridEndEmit)
        {
            // did not find the corresponding parameter in the emiting scope
            hr = S_OK; // discard old error; new error will be returned from CheckContinuableError
            CheckContinuableErrorEx(META_S_PARAM_MISMATCH, pImportData, pdImp);
        }

        else
        {
            _ASSERTE( pRecEmit->GetSequence() == pRecImport->GetSequence() );

            pdEmit = TokenFromRid(pdEmit, mdtParamDef);
    
            // record the token movement
#ifdef WE_DONT_NEED_TO_CHECK_NAMES__THEY_DONT_AFFECT_ANYTHING
            LPCUTF8     szNameImp = pMiniMdImport->getNameOfParam(pRecImport);
            LPCUTF8     szNameEmit = pMiniMdEmit->getNameOfParam(pRecEmit);
            if (szNameImp && szNameEmit && strcmp(szNameImp, szNameEmit) != 0)
            {
                // parameter name doesn't match
                CheckContinuableErrorEx(META_S_PARAM_MISMATCH, pImportData, pdImp);
            }
#endif
            if (pRecEmit->GetFlags() != pRecImport->GetFlags())
            {
                // flags doesn't match
                CheckContinuableErrorEx(META_S_PARAM_MISMATCH, pImportData, pdImp);
            }

            // record token movement. This is a duplicate.
            IfFailGo( pCurTkMap->InsertNotFound(pdImp, true, pdEmit, &pTokenRec) );
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::VerifyParams()


//*****************************************************************************
// merging MemberRef
//*****************************************************************************
HRESULT NEWMERGER::MergeMemberRefs( ) 
{
    HRESULT         hr = NOERROR;
    MemberRefRec    *pRecImport = NULL;
    MemberRefRec    *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    mdMemberRef     mrEmit;
    mdMemberRef     mrImp;
    bool            bDuplicate = false;
    TOKENREC        *pTokenRec;
    mdToken         tkParentImp;
    mdToken         tkParentEmit;

    LPCUTF8         szNameImp;
    PCCOR_SIGNATURE pbSig;
    ULONG           cbSig;
    ULONG           cbEmit;
    CQuickBytes     qbSig;

    bool            isRefOptimizedToDef;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;

        iCount = pMiniMdImport->getCountMemberRefs();

        // loop through all MemberRef
        for (i = 1; i <= iCount; i++)
        {

            // only merge those MemberRefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsMemberRefMarked(TokenFromRid(i, mdtMemberRef)) == false)
                continue;

            isRefOptimizedToDef = false;

            // compare it with the emit scope
            pRecImport = pMiniMdImport->getMemberRef(i);
            szNameImp = pMiniMdImport->getNameOfMemberRef(pRecImport);
            pbSig = pMiniMdImport->getSignatureOfMemberRef(pRecImport, &cbSig);
            tkParentImp = pMiniMdImport->getClassOfMemberRef(pRecImport);

            IfFailGo( pCurTkMap->Remap(tkParentImp, &tkParentEmit) );

            // convert rid contained in signature to new scope
            IfFailGo(ImportHelper::MergeUpdateTokenInSig(    
                NULL,                       // Assembly emit scope.
                pMiniMdEmit,                // The emit scope.
                NULL, NULL, 0,              // Import assembly information.
                pMiniMdImport,              // The scope to merge into the emit scope.
                pbSig,                      // signature from the imported scope
                pCurTkMap,                // Internal token mapping structure.
                &qbSig,                     // [OUT] translated signature
                0,                          // start from first byte of the signature
                0,                          // don't care how many bytes consumed
                &cbEmit));                  // number of bytes write to cbEmit

            // We want to know if we can optimize this MemberRef to a FieldDef or MethodDef
            if (TypeFromToken(tkParentEmit) == mdtTypeDef && RidFromToken(tkParentEmit) != 0)
            {
                // The parent of this MemberRef has been successfully optimized to a TypeDef. Then this MemberRef should be 
                // be able to optimized to a MethodDef or FieldDef unless one of the parent in the inheritance hierachy
                // is through TypeRef. Then this MemberRef stay as MemberRef. If This is a VarArg calling convention, then 
                // we will remap the MemberRef's parent to a MethodDef or stay as TypeRef.
                //
                mdToken     tkParent = tkParentEmit;
                mdToken     tkMethDefOrFieldDef;
                PCCOR_SIGNATURE pbSigTmp = (const COR_SIGNATURE *) qbSig.Ptr();

                while (TypeFromToken(tkParent) == mdtTypeDef && RidFromToken(tkParent) != 0)
                {
                    TypeDefRec      *pRec;
                    hr = ImportHelper::FindMember(pMiniMdEmit, tkParent, szNameImp, pbSigTmp, cbEmit, &tkMethDefOrFieldDef);
                    if (hr == S_OK)
                    {
                        // We have found a match!!
                        if (isCallConv(CorSigUncompressCallingConv(pbSigTmp), IMAGE_CEE_CS_CALLCONV_VARARG))
                        {
                            // The found MethodDef token will replace this MemberRef's parent token
                            _ASSERTE(TypeFromToken(tkMethDefOrFieldDef) == mdtMethodDef);
                            tkParentEmit = tkMethDefOrFieldDef;
                            break;
                        }
                        else
                        {
                            // The found MethodDef/FieldDef token will replace this MemberRef token and we won't introduce a MemberRef 
                            // record.
                            //
                            mrEmit = tkMethDefOrFieldDef;
                            isRefOptimizedToDef = true;
                            bDuplicate = true;
                            break;
                        }
                    }

                    // now walk up to the parent class of tkParent and try to resolve this MemberRef
                    pRec = pMiniMdEmit->getTypeDef(RidFromToken(tkParent));
                    tkParent = pMiniMdEmit->getExtendsOfTypeDef(pRec);
                }

                // When we exit the loop, there are several possibilities:
                // 1. We found a MethodDef/FieldDef to replace the MemberRef
                // 2. We found a MethodDef matches the MemberRef but the MemberRef is VarArg, thus we want to use the MethodDef in the 
                // parent column but not replacing it.
                // 3. We exit because we run out the TypeDef on the parent chain. If it is because we encounter a TypeRef, this TypeRef will
                // replace the parent column of the MemberRef. Or we encounter nil token! (This can be unresolved global MemberRef or
                // compiler error to put an undefined MemberRef. In this case, we should just use the old tkParentEmit
                // on the parent column for the MemberRef.

                if (TypeFromToken(tkParent) == mdtTypeRef && RidFromToken(tkParent) != 0)
                {
                    // we had walked up the parent's chain to resolve it but we have not been successful and got stopped by a TypeRef.
                    // Then we will use this TypeRef as the parent of the emit MemberRef record
                    //
                    tkParentEmit = tkParent;
                }
            }
            else if ((TypeFromToken(tkParentEmit) == mdtMethodDef &&
                      !isCallConv(CorSigUncompressCallingConv(pbSig), IMAGE_CEE_CS_CALLCONV_VARARG)) ||
                     (TypeFromToken(tkParentEmit) == mdtFieldDef))
            {
                // If the MemberRef's parent is already a non-vararg MethodDef or FieldDef, we can also
                // safely drop the MemberRef
                mrEmit = tkParentEmit;
                isRefOptimizedToDef = true;
                bDuplicate = true;
            }

            // If the Ref cannot be optimized to a Def or MemberRef to Def optmization is turned off, do the following.
            if (isRefOptimizedToDef == false || !((m_optimizeRefToDef & MDMemberRefToDef) == MDMemberRefToDef))
            {
                // does this MemberRef already exist in the emit scope?
                if ( m_fDupCheck && ImportHelper::FindMemberRef(
                    pMiniMdEmit,
                    tkParentEmit,
                    szNameImp,
                    (const COR_SIGNATURE *) qbSig.Ptr(),
                    cbEmit,
                    &mrEmit) == S_OK )
                {
                    // Yes, it does
                    bDuplicate = true;
                }
                else
                {
                    // No, it doesn't. Copy it over.
                    bDuplicate = false;
                    IfNullGo( pRecEmit = pMiniMdEmit->AddMemberRefRecord((RID *)&mrEmit) );
                    mrEmit = TokenFromRid( mrEmit, mdtMemberRef );

                    // Copy over the MemberRef context
                    IfFailGo(pMiniMdEmit->PutString(TBL_MemberRef, MemberRefRec::COL_Name, pRecEmit, szNameImp));
                    IfFailGo(pMiniMdEmit->PutToken(TBL_MemberRef, MemberRefRec::COL_Class, pRecEmit, tkParentEmit));
                    IfFailGo(pMiniMdEmit->PutBlob(TBL_MemberRef, MemberRefRec::COL_Signature, pRecEmit,
                                                qbSig.Ptr(), cbEmit));
                    IfFailGo(pMiniMdEmit->AddMemberRefToHash(mrEmit) );
                }
            }
            // record the token movement
            mrImp = TokenFromRid(i, mdtMemberRef);
            IfFailGo( pCurTkMap->InsertNotFound(mrImp, bDuplicate, mrEmit, &pTokenRec) );
        }
    }


ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeMemberRefs()


//*****************************************************************************
// merge interface impl
//*****************************************************************************
HRESULT NEWMERGER::MergeInterfaceImpls( ) 
{
    HRESULT         hr = NOERROR;
    InterfaceImplRec    *pRecImport = NULL;
    InterfaceImplRec    *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    mdTypeDef       tkParent;
    mdInterfaceImpl iiEmit;
    bool            bDuplicate;
    TOKENREC        *pTokenRec;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountInterfaceImpls();

        // loop through all InterfaceImpl
        for (i = 1; i <= iCount; i++)
        {
            // only merge those InterfaceImpls that are marked
            if ( pMiniMdImport->GetFilterTable()->IsInterfaceImplMarked(TokenFromRid(i, mdtInterfaceImpl)) == false)
                continue;

            // compare it with the emit scope
            pRecImport = pMiniMdImport->getInterfaceImpl(i);
            tkParent = pMiniMdImport->getClassOfInterfaceImpl(pRecImport);

            // does this TypeRef already exist in the emit scope?
            if ( pCurTkMap->Find(tkParent, &pTokenRec) )
            {
                if ( pTokenRec->m_isDuplicate )
                {
                    // parent in the emit scope
                    mdToken     tkParentEmit;
                    mdToken     tkInterface;

                    // remap the typedef token
                    tkParentEmit = pTokenRec->m_tkTo;

                    // remap the implemented interface token
                    tkInterface = pMiniMdImport->getInterfaceOfInterfaceImpl(pRecImport);
                    IfFailGo( pCurTkMap->Remap( tkInterface, &tkInterface) );

                    // Set duplicate flag
                    bDuplicate = true;

                    // find the corresponding interfaceimpl in the emit scope
                    if ( ImportHelper::FindInterfaceImpl(pMiniMdEmit, tkParentEmit, tkInterface, &iiEmit) != S_OK )
                    {
                        // bad state!! We have a duplicate typedef but the interface impl is not the same!!

                        // continuable error
                        CheckContinuableErrorEx(
                            META_E_INTFCEIMPL_NOT_FOUND,
                            pImportData,
                            TokenFromRid(i, mdtInterfaceImpl));

                        iiEmit = mdTokenNil;
                    }
                }
                else
                {
                    // No, it doesn't. Copy it over.
                    bDuplicate = false;
                    IfNullGo( pRecEmit = pMiniMdEmit->AddInterfaceImplRecord((RID *)&iiEmit) );

                    // copy the interfaceimp record over 
                    IfFailGo( CopyInterfaceImpl( pRecEmit, pImportData, pRecImport) );
                }
            }
            else
            {
                _ASSERTE( !"bad state!");
                IfFailGo( META_E_BADMETADATA );
            }

            // record the token movement
            IfFailGo( pCurTkMap->InsertNotFound(
                TokenFromRid(i, mdtInterfaceImpl), 
                bDuplicate, 
                TokenFromRid( iiEmit, mdtInterfaceImpl ), 
                &pTokenRec) );
        }
    }


ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeInterfaceImpls()


//*****************************************************************************
// merge all of the constant for field, property, and parameter
//*****************************************************************************
HRESULT NEWMERGER::MergeConstants() 
{
    HRESULT         hr = NOERROR;
    ConstantRec     *pRecImport = NULL;
    ConstantRec     *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    ULONG           csEmit;                 // constant value is not a token
    mdToken         tkParentImp;
    TOKENREC        *pTokenRec;
    void const      *pValue;
    ULONG           cbBlob;
#if _DEBUG
    ULONG           typeParent;
#endif // _DEBUG

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountConstants();

        // loop through all Constants
        for (i = 1; i <= iCount; i++)
        {
            // compare it with the emit scope
            pRecImport = pMiniMdImport->getConstant(i);
            tkParentImp = pMiniMdImport->getParentOfConstant(pRecImport);

            // only move those constant over if their parents are marked
            // If MDTOKENMAP::Find returns false, we don't need to copy the constant value over
            if ( pCurTkMap->Find(tkParentImp, &pTokenRec) )
            {
                // If the parent is duplicated, no need to move over the constant value
                if ( !pTokenRec->m_isDuplicate )
                {
                    IfNullGo( pRecEmit = pMiniMdEmit->AddConstantRecord(&csEmit) );
                    pRecEmit->SetType(pRecImport->GetType());

                    // set the parent
                    IfFailGo( pMiniMdEmit->PutToken(TBL_Constant, ConstantRec::COL_Parent, pRecEmit, pTokenRec->m_tkTo) );

                    // move over the constant blob value
                    pValue = pMiniMdImport->getValueOfConstant(pRecImport, &cbBlob);
                    IfFailGo( pMiniMdEmit->PutBlob(TBL_Constant, ConstantRec::COL_Value, pRecEmit, pValue, cbBlob) );
                    IfFailGo( pMiniMdEmit->AddConstantToHash(csEmit) );
                }
                else
                {
                }
            }
#if _DEBUG
            // Include this block only under Debug build. The reason is that 
            // the linker chooses all the errors that we report (such as unmatched MethodDef or FieldDef)
            // as a continuable error. It is likely to hit this else while the tkparentImp is marked if there
            // is any error reported earlier!!
            else
            {
                typeParent = TypeFromToken(tkParentImp);
                if (typeParent == mdtFieldDef)
                {
                    // FieldDef should not be marked.
                    if ( pMiniMdImport->GetFilterTable()->IsFieldMarked(tkParentImp) == false)
                        continue;
                }
                else if (typeParent == mdtParamDef)
                {
                    // ParamDef should not be marked.
                    if ( pMiniMdImport->GetFilterTable()->IsParamMarked(tkParentImp) == false)
                        continue;
                }
                else
                {
                    _ASSERTE(typeParent == mdtProperty);
                    // Property should not be marked.
                    if ( pMiniMdImport->GetFilterTable()->IsPropertyMarked(tkParentImp) == false)
                        continue;
                }

                // If we come to here, we have a constant whose parent is marked but we could not
                // find it in the map!! Bad state.

                _ASSERTE(!"Ignore this error if you have seen error reported earlier! Otherwise bad token map or bad metadata!");
            }
#endif // _DEBUG
            // Note that we don't need to record the token movement since constant is not a valid token kind.
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeConstants()


//*****************************************************************************
// Merge field marshal information
//*****************************************************************************
HRESULT NEWMERGER::MergeFieldMarshals() 
{
    HRESULT     hr = NOERROR;
    FieldMarshalRec *pRecImport = NULL;
    FieldMarshalRec *pRecEmit = NULL;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    ULONG       iCount;
    ULONG       i;
    ULONG       fmEmit;                 // FieldMarhsal is not a token 
    mdToken     tkParentImp;
    TOKENREC    *pTokenRec;
    void const  *pValue;
    ULONG       cbBlob;
#if _DEBUG
    ULONG       typeParent;
#endif // _DEBUG

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountFieldMarshals();

        // loop through all TypeRef
        for (i = 1; i <= iCount; i++)
        {
            // compare it with the emit scope
            pRecImport = pMiniMdImport->getFieldMarshal(i);
            tkParentImp = pMiniMdImport->getParentOfFieldMarshal(pRecImport);

            // We want to merge only those field marshals that parents are marked.
            // Find will return false if the parent is not marked
            //
            if ( pCurTkMap->Find(tkParentImp, &pTokenRec) )
            {
                // If the parent is duplicated, no need to move over the constant value
                if ( !pTokenRec->m_isDuplicate )
                {
                    IfNullGo( pRecEmit = pMiniMdEmit->AddFieldMarshalRecord(&fmEmit) );

                    // set the parent
                    IfFailGo( pMiniMdEmit->PutToken(
                        TBL_FieldMarshal, 
                        FieldMarshalRec::COL_Parent, 
                        pRecEmit, 
                        pTokenRec->m_tkTo) );

                    // move over the constant blob value
                    pValue = pMiniMdImport->getNativeTypeOfFieldMarshal(pRecImport, &cbBlob);
                    IfFailGo( pMiniMdEmit->PutBlob(TBL_FieldMarshal, FieldMarshalRec::COL_NativeType, pRecEmit, pValue, cbBlob) );
                    IfFailGo( pMiniMdEmit->AddFieldMarshalToHash(fmEmit) );

                }
                else
                {
                }
            }
#if _DEBUG
            else
            {
                typeParent = TypeFromToken(tkParentImp);

                if (typeParent == mdtFieldDef)
                {
                    // FieldDefs should not be marked
                    if ( pMiniMdImport->GetFilterTable()->IsFieldMarked(tkParentImp) == false)
                        continue;
                }
                else
                {
                    _ASSERTE(typeParent == mdtParamDef);
                    // ParamDefs should not be  marked
                    if ( pMiniMdImport->GetFilterTable()->IsParamMarked(tkParentImp) == false)
                        continue;
                }

                // If we come to here, that is we have a FieldMarshal whose parent is marked and we don't find it
                // in the map!!!

                // either bad lookup map or bad metadata
                _ASSERTE(!"Ignore this assert if you have seen error reported earlier. Otherwise, it is bad state!");
            }
#endif // _DEBUG
        }
        // Note that we don't need to record the token movement since FieldMarshal is not a valid token kind.
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeFieldMarshals()


//*****************************************************************************
// Merge class layout information
//*****************************************************************************
HRESULT NEWMERGER::MergeClassLayouts() 
{
    HRESULT         hr = NOERROR;
    ClassLayoutRec  *pRecImport = NULL;
    ClassLayoutRec  *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    ULONG           iRecord;                    // class layout is not a token
    mdToken         tkParentImp;
    TOKENREC        *pTokenRec;
    RID             ridClassLayout;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountClassLayouts();

        // loop through all TypeRef
        for (i = 1; i <= iCount; i++)
        {
            // compare it with the emit scope
            pRecImport = pMiniMdImport->getClassLayout(i);
            tkParentImp = pMiniMdImport->getParentOfClassLayout(pRecImport);

            // only merge those TypeDefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsTypeDefMarked(tkParentImp) == false)
                continue;

            if ( pCurTkMap->Find(tkParentImp, &pTokenRec) )
            {
                if ( !pTokenRec->m_isDuplicate )
                {
                    // If the parent is not duplicated, just copy over the classlayout information
                    IfNullGo( pRecEmit = pMiniMdEmit->AddClassLayoutRecord(&iRecord) );

                    // copy over the fix part information
                    pRecEmit->Copy(pRecImport);
                    IfFailGo( pMiniMdEmit->PutToken(TBL_ClassLayout, ClassLayoutRec::COL_Parent, pRecEmit, pTokenRec->m_tkTo));
                    IfFailGo( pMiniMdEmit->AddClassLayoutToHash(iRecord) );
                }
                else
                {

                    ridClassLayout = pMiniMdEmit->FindClassLayoutHelper(pTokenRec->m_tkTo);

                    if (InvalidRid(ridClassLayout))
                    {
                        // class is duplicated but not class layout info                        
                        CheckContinuableErrorEx(META_E_CLASS_LAYOUT_INCONSISTENT, pImportData, tkParentImp);
                    }
                    else
                    {
                        pRecEmit = pMiniMdEmit->getClassLayout(RidFromToken(ridClassLayout));
                        if (pMiniMdImport->getPackingSizeOfClassLayout(pRecImport) != pMiniMdEmit->getPackingSizeOfClassLayout(pRecEmit) || 
                            pMiniMdImport->getClassSizeOfClassLayout(pRecImport) != pMiniMdEmit->getClassSizeOfClassLayout(pRecEmit) )
                        {
                            CheckContinuableErrorEx(META_E_CLASS_LAYOUT_INCONSISTENT, pImportData, tkParentImp);
                        }
                    }
                }
            }
            else
            {
                // bad lookup map
                _ASSERTE( !"bad state!");
                IfFailGo( META_E_BADMETADATA );
            }
            // no need to record the index movement. Classlayout is not a token.
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeClassLayouts()

//*****************************************************************************
// Merge field layout information
//*****************************************************************************
HRESULT NEWMERGER::MergeFieldLayouts() 
{
    HRESULT         hr = NOERROR;
    FieldLayoutRec *pRecImport = NULL;
    FieldLayoutRec *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    ULONG           iRecord;                    // field layout2 is not a token.
    mdToken         tkFieldImp;
    TOKENREC        *pTokenRec;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountFieldLayouts();

        // loop through all FieldLayout records.
        for (i = 1; i <= iCount; i++)
        {
            // compare it with the emit scope
            pRecImport = pMiniMdImport->getFieldLayout(i);
            tkFieldImp = pMiniMdImport->getFieldOfFieldLayout(pRecImport);
        
            // only merge those FieldDefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsFieldMarked(tkFieldImp) == false)
                continue;

            if ( pCurTkMap->Find(tkFieldImp, &pTokenRec) )
            {
                if ( !pTokenRec->m_isDuplicate )
                {
                    // If the Field is not duplicated, just copy over the FieldLayout information
                    IfNullGo( pRecEmit = pMiniMdEmit->AddFieldLayoutRecord(&iRecord) );

                    // copy over the fix part information
                    pRecEmit->Copy(pRecImport);
                    IfFailGo( pMiniMdEmit->PutToken(TBL_FieldLayout, FieldLayoutRec::COL_Field, pRecEmit, pTokenRec->m_tkTo));
                    IfFailGo( pMiniMdEmit->AddFieldLayoutToHash(iRecord) );
                }
                else
                {
                }
            }
            else
            {
                // bad lookup map
                _ASSERTE( !"bad state!");
                IfFailGo( META_E_BADMETADATA );
            }
            // no need to record the index movement. fieldlayout2 is not a token.
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeFieldLayouts()


//*****************************************************************************
// Merge field RVAs
//*****************************************************************************
HRESULT NEWMERGER::MergeFieldRVAs() 
{
    HRESULT         hr = NOERROR;
    FieldRVARec     *pRecImport = NULL;
    FieldRVARec     *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    ULONG           iRecord;                    // FieldRVA is not a token.
    mdToken         tkFieldImp;
    TOKENREC        *pTokenRec;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountFieldRVAs();

        // loop through all FieldRVA records.
        for (i = 1; i <= iCount; i++)
        {
            // compare it with the emit scope
            pRecImport = pMiniMdImport->getFieldRVA(i);
            tkFieldImp = pMiniMdImport->getFieldOfFieldRVA(pRecImport);
        
            // only merge those FieldDefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsFieldMarked(TokenFromRid(tkFieldImp, mdtFieldDef)) == false)
                continue;

            if ( pCurTkMap->Find(tkFieldImp, &pTokenRec) )
            {
                if ( !pTokenRec->m_isDuplicate )
                {
                    // If the Field is not duplicated, just copy over the FieldRVA information
                    IfNullGo( pRecEmit = pMiniMdEmit->AddFieldRVARecord(&iRecord) );

                    // copy over the fix part information
                    pRecEmit->Copy(pRecImport);
                    IfFailGo( pMiniMdEmit->PutToken(TBL_FieldRVA, FieldRVARec::COL_Field, pRecEmit, pTokenRec->m_tkTo));
                    IfFailGo( pMiniMdEmit->AddFieldRVAToHash(iRecord) );
                }
                else
                {
                }
            }
            else
            {
                // bad lookup map
                _ASSERTE( !"bad state!");
                IfFailGo( META_E_BADMETADATA );
            }
            // no need to record the index movement. FieldRVA is not a token.
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeFieldRVAs()


//*****************************************************************************
// Merge MethodImpl information
//*****************************************************************************
HRESULT NEWMERGER::MergeMethodImpls() 
{
    HRESULT     hr = NOERROR;
    MethodImplRec   *pRecImport = NULL;
    MethodImplRec   *pRecEmit = NULL;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    ULONG       iCount;
    ULONG       i;
    RID         iRecord;
    mdTypeDef   tkClassImp;
    mdToken     tkBodyImp;
    mdToken     tkDeclImp;
    TOKENREC    *pTokenRecClass;
    mdToken     tkBodyEmit;
    mdToken     tkDeclEmit;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountMethodImpls();

        // loop through all the MethodImpls.
        for (i = 1; i <= iCount; i++)
        {
            // only merge those MethodImpls that are marked.
            if ( pMiniMdImport->GetFilterTable()->IsMethodImplMarked(i) == false)
                continue;

            // compare it with the emit scope
            pRecImport = pMiniMdImport->getMethodImpl(i);
            tkClassImp = pMiniMdImport->getClassOfMethodImpl(pRecImport);
            tkBodyImp = pMiniMdImport->getMethodBodyOfMethodImpl(pRecImport);
            tkDeclImp = pMiniMdImport->getMethodDeclarationOfMethodImpl(pRecImport);

            if ( pCurTkMap->Find(tkClassImp, &pTokenRecClass))
            {
                // If the TypeDef is duplicated, no need to move over the MethodImpl record.
                if ( !pTokenRecClass->m_isDuplicate )
                {
                    // Create a new record and set the data.


                    IfFailGo( pCurTkMap->Remap(tkBodyImp, &tkBodyEmit) );
                    IfFailGo( pCurTkMap->Remap(tkDeclImp, &tkDeclEmit) );
                    IfNullGo( pRecEmit = pMiniMdEmit->AddMethodImplRecord(&iRecord) );
                    IfFailGo( pMiniMdEmit->PutToken(TBL_MethodImpl, MethodImplRec::COL_Class, pRecEmit, pTokenRecClass->m_tkTo) );
                    IfFailGo( pMiniMdEmit->PutToken(TBL_MethodImpl, MethodImplRec::COL_MethodBody, pRecEmit, tkBodyEmit) );
                    IfFailGo( pMiniMdEmit->PutToken(TBL_MethodImpl, MethodImplRec::COL_MethodDeclaration, pRecEmit, tkDeclEmit) );
                    IfFailGo( pMiniMdEmit->AddMethodImplToHash(iRecord) );
                }
                else
                {
                }
                // No need to record the token movement, MethodImpl is not a token.
            }
            else
            {
                // either bad lookup map or bad metadata
                _ASSERTE(!"bad state");
                IfFailGo( META_E_BADMETADATA );
            }
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeMethodImpls()


//*****************************************************************************
// Merge PInvoke
//*****************************************************************************
HRESULT NEWMERGER::MergePinvoke() 
{
    HRESULT         hr = NOERROR;
    ImplMapRec      *pRecImport = NULL;
    ImplMapRec      *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    mdModuleRef     mrImp;
    mdModuleRef     mrEmit;
    mdMethodDef     mdImp;
    RID             mdImplMap;
    TOKENREC        *pTokenRecMR;
    TOKENREC        *pTokenRecMD;

    USHORT          usMappingFlags;
    LPCUTF8         szImportName;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountImplMaps();

        // loop through all ImplMaps
        for (i = 1; i <= iCount; i++)
        {
            // compare it with the emit scope
            pRecImport = pMiniMdImport->getImplMap(i);

            // Get the MethodDef token in the new space.
            mdImp = pMiniMdImport->getMemberForwardedOfImplMap(pRecImport);

            // only merge those MethodDefs that are marked
            if ( pMiniMdImport->GetFilterTable()->IsMethodMarked(mdImp) == false)
                continue;

            // Get the ModuleRef token in the new space.
            mrImp = pMiniMdImport->getImportScopeOfImplMap(pRecImport);

            // map the token to the new scope
            if (pCurTkMap->Find(mrImp, &pTokenRecMR) == false)
            {
                // This should never fire unless the module refs weren't merged
                // before this code ran.
                _ASSERTE(!"Parent ModuleRef not found in MERGER::MergePinvoke.  Bad state!");
                IfFailGo( META_E_BADMETADATA );
            }

            // If the ModuleRef has been remapped to the "module token", we need to undo that
            //  for the pinvokeimpl.  A pinvoke can only have a ModuleRef for the ImportScope.
            mrEmit = pTokenRecMR->m_tkTo;
            if (mrEmit == MODULEDEFTOKEN)
            {   // Yes, the ModuleRef has been remapped to the module token.  So, 
                //  find the ModuleRef in the output scope; if it is not found, add
                //  it.
                ModuleRefRec    *pModRefImport;
                LPCUTF8         szNameImp;
                pModRefImport = pMiniMdImport->getModuleRef(RidFromToken(mrImp));
                szNameImp = pMiniMdImport->getNameOfModuleRef(pModRefImport);

                // does this ModuleRef already exist in the emit scope?
                hr = ImportHelper::FindModuleRef(pMiniMdEmit,
                                                szNameImp,
                                                &mrEmit);

                if (hr == CLDB_E_RECORD_NOTFOUND)
                {   // No, it doesn't. Copy it over.
                    ModuleRefRec    *pModRefEmit;
                    IfNullGo( pModRefEmit = pMiniMdEmit->AddModuleRefRecord((RID*)&mrEmit) );
                    mrEmit = TokenFromRid(mrEmit, mdtModuleRef);

                    // Set ModuleRef Name.
                    IfFailGo( pMiniMdEmit->PutString(TBL_ModuleRef, ModuleRefRec::COL_Name, pModRefEmit, szNameImp) );
                }
                else
                    IfFailGo(hr);
            }


            if (pCurTkMap->Find(mdImp, &pTokenRecMD) == false)
            {
                // This should never fire unless the method defs weren't merged
                // before this code ran.
                _ASSERTE(!"Parent MethodDef not found in MERGER::MergePinvoke.  Bad state!");
                IfFailGo( META_E_BADMETADATA );
            }


            // Get copy of rest of data.
            usMappingFlags = pMiniMdImport->getMappingFlagsOfImplMap(pRecImport);
            szImportName = pMiniMdImport->getImportNameOfImplMap(pRecImport);

            // If the method associated with PInvokeMap is not duplicated, then don't bother to look up the 
            // duplicated PInvokeMap information.
            if (pTokenRecMD->m_isDuplicate == true)
            {
                // Does the correct ImplMap entry exist in the emit scope?
                mdImplMap = pMiniMdEmit->FindImplMapHelper(pTokenRecMD->m_tkTo);
            }
            else
            {
                mdImplMap = mdTokenNil;
            }
            if (!InvalidRid(mdImplMap))
            {
                // Verify that the rest of the data is identical, else it's an error.
                pRecEmit = pMiniMdEmit->getImplMap(mdImplMap);
                _ASSERTE(pMiniMdEmit->getMemberForwardedOfImplMap(pRecEmit) == pTokenRecMD->m_tkTo);
                if (pMiniMdEmit->getImportScopeOfImplMap(pRecEmit) != mrEmit ||
                    pMiniMdEmit->getMappingFlagsOfImplMap(pRecEmit) != usMappingFlags ||
                    strcmp(pMiniMdEmit->getImportNameOfImplMap(pRecEmit), szImportName))
                {
                    // Mismatched p-invoke entries are found.
                    _ASSERTE(!"Mismatched P-invoke entries during merge.  Bad State!");
                    IfFailGo(E_FAIL);
                }
            }
            else
            {
                IfNullGo( pRecEmit = pMiniMdEmit->AddImplMapRecord(&mdImplMap) );

                // Copy rest of data.
                IfFailGo( pMiniMdEmit->PutToken(TBL_ImplMap, ImplMapRec::COL_MemberForwarded, pRecEmit, pTokenRecMD->m_tkTo) );
                IfFailGo( pMiniMdEmit->PutToken(TBL_ImplMap, ImplMapRec::COL_ImportScope, pRecEmit, mrEmit) );
                IfFailGo( pMiniMdEmit->PutString(TBL_ImplMap, ImplMapRec::COL_ImportName, pRecEmit, szImportName) );
                pRecEmit->SetMappingFlags(usMappingFlags);
                IfFailGo( pMiniMdEmit->AddImplMapToHash(mdImplMap) );
            }
        }
    }


ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergePinvoke()


//*****************************************************************************
// Merge StandAloneSigs
//*****************************************************************************
HRESULT NEWMERGER::MergeStandAloneSigs() 
{
    HRESULT         hr = NOERROR;
    StandAloneSigRec    *pRecImport = NULL;
    StandAloneSigRec    *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    TOKENREC        *pTokenRec;
    mdSignature     saImp;
    mdSignature     saEmit;
    bool            fDuplicate;
    PCCOR_SIGNATURE pbSig;
    ULONG           cbSig;
    ULONG           cbEmit;
    CQuickBytes     qbSig;
    PCOR_SIGNATURE  rgSig;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountStandAloneSigs();

        // loop through all Signatures
        for (i = 1; i <= iCount; i++)
        {
            // only merge those Signatures that are marked
            if ( pMiniMdImport->GetFilterTable()->IsSignatureMarked(TokenFromRid(i, mdtSignature)) == false)
                continue;

            // compare it with the emit scope
            pRecImport = pMiniMdImport->getStandAloneSig(i);
            pbSig = pMiniMdImport->getSignatureOfStandAloneSig(pRecImport, &cbSig);

            // This is a signature containing the return type after count of args
            // convert rid contained in signature to new scope
            IfFailGo(ImportHelper::MergeUpdateTokenInSig(    
                NULL,                       // Assembly emit scope.
                pMiniMdEmit,                // The emit scope.
                NULL, NULL, 0,              // Assembly import scope info.
                pMiniMdImport,              // The scope to merge into the emit scope.
                pbSig,                      // signature from the imported scope
                pCurTkMap,                // Internal token mapping structure.
                &qbSig,                     // [OUT] translated signature
                0,                          // start from first byte of the signature
                0,                          // don't care how many bytes consumed
                &cbEmit));                  // number of bytes write to cbEmit
            rgSig = ( PCOR_SIGNATURE ) qbSig.Ptr();

            hr = ImportHelper::FindStandAloneSig(
                pMiniMdEmit,
                rgSig,
                cbEmit,
                &saEmit );
            if ( hr == S_OK )
            {
                // find a duplicate
                fDuplicate = true;
            }
            else
            {
                // copy over
                fDuplicate = false;
                IfNullGo( pRecEmit = pMiniMdEmit->AddStandAloneSigRecord((ULONG *)&saEmit) );
                saEmit = TokenFromRid(saEmit, mdtSignature);
                IfFailGo( pMiniMdEmit->PutBlob(TBL_StandAloneSig, StandAloneSigRec::COL_Signature, pRecEmit, rgSig, cbEmit));
            }
            saImp = TokenFromRid(i, mdtSignature);

            // Record the token movement
            IfFailGo( pCurTkMap->InsertNotFound(saImp, fDuplicate, saEmit, &pTokenRec) );
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeStandAloneSigs()

//*****************************************************************************
// Merge MethodSpecs
//*****************************************************************************
HRESULT NEWMERGER::MergeMethodSpecs() 
{
    HRESULT         hr = NOERROR;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    mdToken         tk;
    ULONG           iRecord;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;

        // Loop through all MethodSpec
        iCount = pMiniMdImport->getCountMethodSpecs();
        for (i=1; i<=iCount; ++i)
        {
            MethodSpecRec   *pRecImport;
            MethodSpecRec   *pRecEmit;
            TOKENREC        *pTokenRecMethod;
            TOKENREC        *pTokenRecMethodNew;
            PCCOR_SIGNATURE pvSig;
            ULONG           cbSig;
            CQuickBytes     qbSig;
            ULONG           cbEmit;
            
            // Only copy marked records.
            if (!pMiniMdImport->GetFilterTable()->IsMethodSpecMarked(i))
                continue;

            pRecImport = pMiniMdImport->getMethodSpec(i);
            tk = pMiniMdImport->getMethodOfMethodSpec(pRecImport);

            // Map the token to the new scope.
            if (pCurTkMap->Find(tk, &pTokenRecMethod) == false)
            {
                // This should never fire unless the TypeDefs/Refs weren't merged
                // before this code runs.
                _ASSERTE(!"MethodSpec method not found in MERGER::MergeGenericsInfo.  Bad state!");
                IfFailGo( META_E_BADMETADATA );
            }
            // Copy to output scope.
            IfNullGo( pRecEmit = pMiniMdEmit->AddMethodSpecRecord(&iRecord) );
            IfFailGo( pMiniMdEmit->PutToken(TBL_MethodSpec, MethodSpecRec::COL_Method, pRecEmit, pTokenRecMethod->m_tkTo));

            // Copy the signature, translating any embedded tokens.
            pvSig = pMiniMdImport->getInstantiationOfMethodSpec(pRecImport, &cbSig);
            
            //  ...convert rid contained in signature to new scope
            IfFailGo(ImportHelper::MergeUpdateTokenInSig(    
                NULL,                       // Assembly emit scope.
                pMiniMdEmit,                // The emit scope.
                NULL, NULL, 0,              // Import assembly scope information.
                pMiniMdImport,              // The scope to merge into the emit scope.
                pvSig,                      // signature from the imported scope
                pCurTkMap,                  // Internal token mapping structure.
                &qbSig,                     // [OUT] translated signature
                0,                          // start from first byte of the signature
                0,                          // don't care how many bytes consumed
                &cbEmit));                  // number of bytes write to cbEmit

            // ...persist the converted signature
            IfFailGo( pMiniMdEmit->PutBlob(TBL_MethodSpec, MethodSpecRec::COL_Instantiation, pRecEmit, qbSig.Ptr(), cbEmit) );
            
            IfFailGo( pCurTkMap->InsertNotFound(TokenFromRid(i, mdtMethodSpec), false,
                                                TokenFromRid(iRecord, mdtMethodSpec), &pTokenRecMethodNew) );
        }
    }

    ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeMethodSpecs()
    
//*****************************************************************************
// Merge DeclSecuritys
//*****************************************************************************
HRESULT NEWMERGER::MergeDeclSecuritys() 
{
    HRESULT         hr = NOERROR;
    DeclSecurityRec *pRecImport = NULL;
    DeclSecurityRec *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    mdToken         tkParentImp;
    TOKENREC        *pTokenRec;
    void const      *pValue;
    ULONG           cbBlob;
    mdPermission    pmImp;
    mdPermission    pmEmit;
    bool            fDuplicate;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountDeclSecuritys();

        // loop through all DeclSecurity
        for (i = 1; i <= iCount; i++)
        {
            // only merge those DeclSecurities that are marked
            if ( pMiniMdImport->GetFilterTable()->IsDeclSecurityMarked(TokenFromRid(i, mdtPermission)) == false)
                continue;
        
            // compare it with the emit scope
            pRecImport = pMiniMdImport->getDeclSecurity(i);
            tkParentImp = pMiniMdImport->getParentOfDeclSecurity(pRecImport);
            if ( pCurTkMap->Find(tkParentImp, &pTokenRec) )
            {
                if ( !pTokenRec->m_isDuplicate )
                {
                    // If the parent is not duplicated, just copy over the custom value
                    goto CopyPermission;
                }
                else
                {
                    // Try to see if the Permission is there in the emit scope or not.
                    // If not, move it over still
                    if ( ImportHelper::FindPermission(
                        pMiniMdEmit,
                        pTokenRec->m_tkTo,
                        pRecImport->GetAction(),
                        &pmEmit) == S_OK )
                    {
                        // found a match
                        fDuplicate = true;
                    }
                    else
                    {
                        // Parent is duplicated but the Permission is not. Still copy over the
                        // Permission.
CopyPermission:
                        fDuplicate = false;
                        IfNullGo( pRecEmit = pMiniMdEmit->AddDeclSecurityRecord((ULONG *)&pmEmit) );
                        pmEmit = TokenFromRid(pmEmit, mdtPermission);

                        pRecEmit->Copy(pRecImport);

                        // set the parent
                        IfFailGo( pMiniMdEmit->PutToken(
                            TBL_DeclSecurity, 
                            DeclSecurityRec::COL_Parent, 
                            pRecEmit, 
                            pTokenRec->m_tkTo) );

                        // move over the CustomAttribute blob value
                        pValue = pMiniMdImport->getPermissionSetOfDeclSecurity(pRecImport, &cbBlob);
                        IfFailGo( pMiniMdEmit->PutBlob(
                            TBL_DeclSecurity, 
                            DeclSecurityRec::COL_PermissionSet, 
                            pRecEmit, 
                            pValue, 
                            cbBlob));
                    }
                }
                pmEmit = TokenFromRid(pmEmit, mdtPermission);
                pmImp = TokenFromRid(i, mdtPermission);

                // Record the token movement
                IfFailGo( pCurTkMap->InsertNotFound(pmImp, fDuplicate, pmEmit, &pTokenRec) );
            }
            else
            {
                // bad lookup map
                _ASSERTE(!"bad state");
                IfFailGo( META_E_BADMETADATA );
            }
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeDeclSecuritys()


//*****************************************************************************
// Merge Strings
//*****************************************************************************
HRESULT NEWMERGER::MergeStrings() 
{
    HRESULT         hr = NOERROR;
    void            *pvStringBlob;
    ULONG           cbBlob;
    ULONG           ulImport = 0;
    ULONG           ulEmit;
    ULONG           ulNext;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    TOKENREC        *pTokenRec;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        ulImport = 0;
        while (ulImport != (ULONG) -1)
        {
            pvStringBlob = pMiniMdImport->GetUserStringNext(ulImport, &cbBlob, &ulNext);
            if (!cbBlob)
            {
                ulImport = ulNext;
                continue;
            }
            if ( pMiniMdImport->GetFilterTable()->IsUserStringMarked(TokenFromRid(ulImport, mdtString)) == false)
            {
                ulImport = ulNext;
                continue;
            }

            IfFailGo(pMiniMdEmit->PutUserString(pvStringBlob, cbBlob, &ulEmit));

            IfFailGo( pCurTkMap->InsertNotFound(
                TokenFromRid(ulImport, mdtString),
                false,
                TokenFromRid(ulEmit, mdtString),
                &pTokenRec) );
            ulImport = ulNext;
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeStrings()

//*****************************************************************************
// Merge CustomAttributes
//*****************************************************************************
HRESULT NEWMERGER::MergeCustomAttributes() 
{
    HRESULT         hr = NOERROR;
    CustomAttributeRec  *pRecImport = NULL;
    CustomAttributeRec  *pRecEmit = NULL;
    CMiniMdRW       *pMiniMdImport;
    CMiniMdRW       *pMiniMdEmit;
    ULONG           iCount;
    ULONG           i;
    mdToken         tkParentImp;            // Token of attributed object (parent).
    TOKENREC        *pTokenRec;             // Parent's remap.
    mdToken         tkType;                 // Token of attribute's type.
    TOKENREC        *pTypeRec;              // Type's remap.
    void const      *pValue;                // The actual value.
    ULONG           cbBlob;                 // Size of the value.
    mdToken         cvImp;
    mdToken         cvEmit;
    bool            fDuplicate;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    TypeRefRec      *pTypeRefRec;  
    ULONG           cTypeRefRecs;  
    mdToken         mrSuppressMergeCheckAttributeCtor = mdTokenNil;

    pMiniMdEmit = GetMiniMdEmit();

    // Find out the TypeRef referring to our library's System.CompilerServices.SuppressMergeCheckAttribute
    cTypeRefRecs = pMiniMdEmit->getCountTypeRefs();

    // Search for the TypeRef.
    for (i = 1; i <= cTypeRefRecs; i++)
    {
        mdToken tkTmp = TokenFromRid(i,mdtTypeRef);   
        if((pTypeRefRec = IsAttributeFromNamespace(pMiniMdEmit,tkTmp,
                                                   COR_COMPILERSERVICE_NAMESPACE,
                                                   (LPUTF8)"mscorlib")) != NULL)
        {
            if (strcmp(pMiniMdEmit->getNameOfTypeRef(pTypeRefRec), COR_SUPPRESS_MERGE_CHECK_ATTRIBUTE) == 0)
            {
                if (S_OK == ImportHelper::FindMemberRef(pMiniMdEmit, tkTmp, COR_CTOR_METHOD_NAME, NULL, 0, &mrSuppressMergeCheckAttributeCtor))
                {
                    break;
                }
            }
        }
    }
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // We know that the filter table is not null here.  Tell PREFIX that we know it.
        PREFIX_ASSUME( pMiniMdImport->GetFilterTable() != NULL );

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountCustomAttributes();

        // loop through all CustomAttribute
        for (i = 1; i <= iCount; i++)
        {
            // compare it with the emit scope
            pRecImport = pMiniMdImport->getCustomAttribute(i);
            tkParentImp = pMiniMdImport->getParentOfCustomAttribute(pRecImport);
            tkType = pMiniMdImport->getTypeOfCustomAttribute(pRecImport);
            pValue = pMiniMdImport->getValueOfCustomAttribute(pRecImport, &cbBlob);

            // only merge those CustomAttributes that are marked
            if ( pMiniMdImport->GetFilterTable()->IsCustomAttributeMarked(TokenFromRid(i, mdtCustomAttribute)) == false)
                continue;

            // Check the type of the CustomAttribute. If it is not marked, then we don't need to move over the CustomAttributes.
            // This will only occur for compiler defined discardable CAs during linking.
            //
            if ( pMiniMdImport->GetFilterTable()->IsTokenMarked(tkType) == false)
                continue;
        
            if ( pCurTkMap->Find(tkParentImp, &pTokenRec) )
            {
                // If the From token type is different from the To token's type, we have optimized the ref to def. 
                // In this case, we are dropping the CA associated with the Ref tokens.
                //
                if (TypeFromToken(tkParentImp) == TypeFromToken(pTokenRec->m_tkTo))
                {

                    // If tkParentImp is a MemberRef and it is also mapped to a MemberRef in the merged scope with a MethodDef
                    // parent, then it is a MemberRef optimized to a MethodDef. We are keeping the MemberRef because it is a
                    // vararg call. So we can drop CAs on this MemberRef.
                    if (TypeFromToken(tkParentImp) == mdtMemberRef)
                    {
                        MemberRefRec    *pTempRec = pMiniMdEmit->getMemberRef(RidFromToken(pTokenRec->m_tkTo));
                        if (TypeFromToken(pMiniMdEmit->getClassOfMemberRef(pTempRec)) == mdtMethodDef)
                            continue;
                    }


                    if (! pCurTkMap->Find(tkType, &pTypeRec) )
                    {
                        _ASSERTE(!"CustomAttribute Type not found in output scope");
                        IfFailGo(META_E_BADMETADATA);
                    }

                    // if it's the SuppressMergeCheckAttribute, don't copy it
                    if ( pTypeRec->m_tkTo == mrSuppressMergeCheckAttributeCtor )
                    {
                        continue;
                    }

                    if ( pTokenRec->m_isDuplicate)
                    {
                        // Try to see if the custom value is there in the emit scope or not.
                        // If not, move it over still
                        hr = ImportHelper::FindCustomAttributeByToken(
                            pMiniMdEmit,
                            pTokenRec->m_tkTo,
                            pTypeRec->m_tkTo,
                            pValue,
                            cbBlob,
                            &cvEmit);
                
                        if ( hr == S_OK )
                        {
                            // found a match
                            fDuplicate = true;
                        }
                        else
                        {
                            // We need to allow additive merge on TypeRef for CustomAttributes because compiler
                            // could build module but not assembly. They are hanging of Assembly level CAs on a bogus
                            // TypeRef.
                            // Also allow additive merge for CAs from CompilerServices and Microsoft.VisualC
                            if (tkParentImp == TokenFromRid(1, mdtModule) 
                                || TypeFromToken(tkParentImp) == mdtTypeRef
                                || (IsAttributeFromNamespace(pMiniMdImport,tkType,
                                                   COR_COMPILERSERVICE_NAMESPACE,
                                                   (LPUTF8)"mscorlib") != NULL)
                                || (IsAttributeFromNamespace(pMiniMdImport,tkType,
                                                   (LPUTF8)"Microsoft.VisualC",
                                                   (LPUTF8)"Microsoft.VisualC") != NULL))
                            {
                                // clear the error
                                hr = NOERROR;

                                // custom value of module token!  Copy over the custom value
                                goto CopyCustomAttribute;
                            }
                            CheckContinuableErrorEx(META_E_MD_INCONSISTENCY, pImportData, TokenFromRid(i, mdtCustomAttribute));
                        }
                    }
                    else
                    {
CopyCustomAttribute:
                        if ((m_dwMergeFlags & DropMemberRefCAs) && TypeFromToken(pTokenRec->m_tkTo) == mdtMemberRef)
                        {
                            // CustomAttributes associated with MemberRef. If the parent of MemberRef is a MethodDef or FieldDef, drop
                            // the custom attribute.
                            MemberRefRec    *pMemberRefRec = pMiniMdEmit->getMemberRef(RidFromToken(pTokenRec->m_tkTo));
                            mdToken         mrParent = pMiniMdEmit->getClassOfMemberRef(pMemberRefRec);
                            if (TypeFromToken(mrParent) == mdtMethodDef || TypeFromToken(mrParent) == mdtFieldDef)
                            {
                                // Don't bother to copy over
                                continue;
                            }
                        }

                        // Parent is duplicated but the custom value is not. Still copy over the
                        // custom value.
                        fDuplicate = false;
                        IfNullGo( pRecEmit = pMiniMdEmit->AddCustomAttributeRecord((ULONG *)&cvEmit) );
                        cvEmit = TokenFromRid(cvEmit, mdtCustomAttribute);

                        // set the parent
                        IfFailGo( pMiniMdEmit->PutToken(TBL_CustomAttribute, CustomAttributeRec::COL_Parent, pRecEmit, pTokenRec->m_tkTo) );
                        // set the type
                        IfFailGo( pMiniMdEmit->PutToken(TBL_CustomAttribute, CustomAttributeRec::COL_Type, pRecEmit, pTypeRec->m_tkTo));

                        // move over the CustomAttribute blob value
                        pValue = pMiniMdImport->getValueOfCustomAttribute(pRecImport, &cbBlob);

                        IfFailGo( pMiniMdEmit->PutBlob(TBL_CustomAttribute, CustomAttributeRec::COL_Value, pRecEmit, pValue, cbBlob));
                        IfFailGo( pMiniMdEmit->AddCustomAttributesToHash(cvEmit) );
                    }
                    cvEmit = TokenFromRid(cvEmit, mdtCustomAttribute);
                    cvImp = TokenFromRid(i, mdtCustomAttribute);

                    // Record the token movement
                    IfFailGo( pCurTkMap->InsertNotFound(cvImp, pTokenRec->m_isDuplicate, cvEmit, &pTokenRec) );
                }
            }
            else
            {

                // either bad lookup map or bad metadata
                _ASSERTE(!"Bad state");
                IfFailGo( META_E_BADMETADATA );
            }
        }
    }

ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeCustomAttributes()


//*******************************************************************************
// Helper to copy an InterfaceImpl record
//*******************************************************************************
HRESULT NEWMERGER::CopyInterfaceImpl(
    InterfaceImplRec    *pRecEmit,          // [IN] the emit record to fill
    MergeImportData     *pImportData,       // [IN] the importing context
    InterfaceImplRec    *pRecImp)           // [IN] the record to import
{
    HRESULT     hr;
    mdToken     tkParent;
    mdToken     tkInterface;
    CMiniMdRW   *pMiniMdEmit = GetMiniMdEmit();
    CMiniMdRW   *pMiniMdImp;
    MDTOKENMAP  *pCurTkMap;

    pMiniMdImp = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

    // set the current MDTokenMap
    pCurTkMap = pImportData->m_pMDTokenMap;

    tkParent = pMiniMdImp->getClassOfInterfaceImpl(pRecImp);
    tkInterface = pMiniMdImp->getInterfaceOfInterfaceImpl(pRecImp);

    IfFailGo( pCurTkMap->Remap(tkParent, &tkParent) );
    IfFailGo( pCurTkMap->Remap(tkInterface, &tkInterface) );

    IfFailGo( pMiniMdEmit->PutToken( TBL_InterfaceImpl, InterfaceImplRec::COL_Class, pRecEmit, tkParent) );
    IfFailGo( pMiniMdEmit->PutToken( TBL_InterfaceImpl, InterfaceImplRec::COL_Interface, pRecEmit, tkInterface) );

ErrExit:
    return hr;
} // HRESULT NEWMERGER::CopyInterfaceImpl()


//*****************************************************************************
// Merge Assembly table
//*****************************************************************************
HRESULT NEWMERGER::MergeAssembly()
{
    HRESULT     hr = NOERROR;
    AssemblyRec *pRecImport = NULL;
    AssemblyRec *pRecEmit = NULL;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    LPCUTF8     szTmp;
    const BYTE  *pbTmp;
    ULONG       cbTmp;
    ULONG       iRecord;
    TOKENREC    *pTokenRec;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        if (!pMiniMdImport->getCountAssemblys())
            goto ErrExit;       // There is no Assembly in the import scope to merge.

        // Copy the Assembly map record to the Emit scope and send a token remap notifcation
        // to the client.  No duplicate checking needed since the Assembly can be present in
        // only one scope and there can be atmost one entry.
        pRecImport = pMiniMdImport->getAssembly(1);
        IfNullGo( pRecEmit = pMiniMdEmit->AddAssemblyRecord(&iRecord));

        pRecEmit->Copy(pRecImport);
    
        pbTmp = pMiniMdImport->getPublicKeyOfAssembly(pRecImport, &cbTmp);
        IfFailGo(pMiniMdEmit->PutBlob(TBL_Assembly, AssemblyRec::COL_PublicKey, pRecEmit,
                                    pbTmp, cbTmp));

        szTmp = pMiniMdImport->getNameOfAssembly(pRecImport);
        IfFailGo(pMiniMdEmit->PutString(TBL_Assembly, AssemblyRec::COL_Name, pRecEmit, szTmp));

        szTmp = pMiniMdImport->getLocaleOfAssembly(pRecImport);
        IfFailGo(pMiniMdEmit->PutString(TBL_Assembly, AssemblyRec::COL_Locale, pRecEmit, szTmp));

        // record the token movement.
        IfFailGo(pCurTkMap->InsertNotFound(
            TokenFromRid(1, mdtAssembly),
            false,
            TokenFromRid(iRecord, mdtAssembly),
            &pTokenRec));
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeAssembly()




//*****************************************************************************
// Merge File table
//*****************************************************************************
HRESULT NEWMERGER::MergeFiles()
{
    HRESULT     hr = NOERROR;
    FileRec     *pRecImport = NULL;
    FileRec     *pRecEmit = NULL;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    LPCUTF8     szTmp;
    const void  *pbTmp;
    ULONG       cbTmp;
    ULONG       iCount;
    ULONG       i;
    ULONG       iRecord;
    TOKENREC    *pTokenRec;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountFiles();

        // Loop through all File records and copy them to the Emit scope.
        // Since there can only be one File table in all the scopes combined,
        // there isn't any duplicate checking that needs to be done.
        for (i = 1; i <= iCount; i++)
        {
            pRecImport = pMiniMdImport->getFile(i);
            IfNullGo( pRecEmit = pMiniMdEmit->AddFileRecord(&iRecord));

            pRecEmit->Copy(pRecImport);

            szTmp = pMiniMdImport->getNameOfFile(pRecImport);
            IfFailGo(pMiniMdEmit->PutString(TBL_File, FileRec::COL_Name, pRecEmit, szTmp));

            pbTmp = pMiniMdImport->getHashValueOfFile(pRecImport, &cbTmp);
            IfFailGo(pMiniMdEmit->PutBlob(TBL_File, FileRec::COL_HashValue, pRecEmit, pbTmp, cbTmp));

            // record the token movement.
            IfFailGo(pCurTkMap->InsertNotFound(
                TokenFromRid(i, mdtFile),
                false,
                TokenFromRid(iRecord, mdtFile),
                &pTokenRec));
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeFiles()


//*****************************************************************************
// Merge ExportedType table
//*****************************************************************************
HRESULT NEWMERGER::MergeExportedTypes()
{
    HRESULT     hr = NOERROR;
    ExportedTypeRec  *pRecImport = NULL;
    ExportedTypeRec  *pRecEmit = NULL;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    LPCUTF8     szTmp;
    mdToken     tkTmp;
    ULONG       iCount;
    ULONG       i;
    ULONG       iRecord;
    TOKENREC    *pTokenRec;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountExportedTypes();

        // Loop through all ExportedType records and copy them to the Emit scope.
        // Since there can only be one ExportedType table in all the scopes combined,
        // there isn't any duplicate checking that needs to be done.
        for (i = 1; i <= iCount; i++)
        {
            pRecImport = pMiniMdImport->getExportedType(i);
            IfNullGo( pRecEmit = pMiniMdEmit->AddExportedTypeRecord(&iRecord));

            pRecEmit->Copy(pRecImport);

            szTmp = pMiniMdImport->getTypeNameOfExportedType(pRecImport);
            IfFailGo(pMiniMdEmit->PutString(TBL_ExportedType, ExportedTypeRec::COL_TypeName, pRecEmit, szTmp));

            szTmp = pMiniMdImport->getTypeNamespaceOfExportedType(pRecImport);
            IfFailGo(pMiniMdEmit->PutString(TBL_ExportedType, ExportedTypeRec::COL_TypeNamespace, pRecEmit, szTmp));

            tkTmp = pMiniMdImport->getImplementationOfExportedType(pRecImport);
            IfFailGo(pCurTkMap->Remap(tkTmp, &tkTmp));
            IfFailGo(pMiniMdEmit->PutToken(TBL_ExportedType, ExportedTypeRec::COL_Implementation,
                                        pRecEmit, tkTmp));


            // record the token movement.
            IfFailGo(pCurTkMap->InsertNotFound(
                TokenFromRid(i, mdtExportedType),
                false,
                TokenFromRid(iRecord, mdtExportedType),
                &pTokenRec));
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeExportedTypes()


//*****************************************************************************
// Merge ManifestResource table
//*****************************************************************************
HRESULT NEWMERGER::MergeManifestResources()
{
    HRESULT     hr = NOERROR;
    ManifestResourceRec *pRecImport = NULL;
    ManifestResourceRec *pRecEmit = NULL;
    CMiniMdRW   *pMiniMdImport;
    CMiniMdRW   *pMiniMdEmit;
    LPCUTF8     szTmp;
    mdToken     tkTmp;
    ULONG       iCount;
    ULONG       i;
    ULONG       iRecord;
    TOKENREC    *pTokenRec;

    MergeImportData *pImportData;
    MDTOKENMAP      *pCurTkMap;

    pMiniMdEmit = GetMiniMdEmit();
    
    for (pImportData = m_pImportDataList; pImportData != NULL; pImportData = pImportData->m_pNextImportData)
    {
        // for each import scope
        pMiniMdImport = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);

        // set the current MDTokenMap
        pCurTkMap = pImportData->m_pMDTokenMap;
        iCount = pMiniMdImport->getCountManifestResources();

        // Loop through all ManifestResource records and copy them to the Emit scope.
        // Since there can only be one ManifestResource table in all the scopes combined,
        // there isn't any duplicate checking that needs to be done.
        for (i = 1; i <= iCount; i++)
        {
            pRecImport = pMiniMdImport->getManifestResource(i);
            IfNullGo( pRecEmit = pMiniMdEmit->AddManifestResourceRecord(&iRecord));

            pRecEmit->Copy(pRecImport);

            szTmp = pMiniMdImport->getNameOfManifestResource(pRecImport);
            IfFailGo(pMiniMdEmit->PutString(TBL_ManifestResource, ManifestResourceRec::COL_Name,
                                        pRecEmit, szTmp));

            tkTmp = pMiniMdImport->getImplementationOfManifestResource(pRecImport);
            IfFailGo(pCurTkMap->Remap(tkTmp, &tkTmp));
            IfFailGo(pMiniMdEmit->PutToken(TBL_ManifestResource, ManifestResourceRec::COL_Implementation,
                                        pRecEmit, tkTmp));

            // record the token movement.
            IfFailGo(pCurTkMap->InsertNotFound(
                TokenFromRid(i, mdtManifestResource),
                false,
                TokenFromRid(iRecord, mdtManifestResource),
                &pTokenRec));
        }
    }
ErrExit:
    return hr;
} // HRESULT NEWMERGER::MergeManifestResources()





//*****************************************************************************
// Error handling. Call back to host to see what they want to do.
//*****************************************************************************
HRESULT NEWMERGER::OnError(
    HRESULT     hrIn,                   // The error HR we're reporting.
    MergeImportData *pImportData,       // The input scope with the error.
    mdToken     token)                  // The token with the error.
{
    // This function does a QI and a Release on every call.  However, it should be 
    //  called very infrequently, and lets the scope just keep a generic handler.
    IMetaDataError  *pIErr = NULL;
    IUnknown        *pHandler = pImportData->m_pHandler;
    CMiniMdRW       *pMiniMd = &(pImportData->m_pRegMetaImport->m_pStgdb->m_MiniMd);
    CQuickArray<WCHAR> rName;           // Name of the TypeDef in unicode.
    LPCUTF8         szTypeName;
    LPCUTF8         szNSName;
    TypeDefRec      *pTypeRec;
    int             iLen;               // Length of a name.
    mdToken         tkParent;
    HRESULT         hr = NOERROR;

    if (pHandler && pHandler->QueryInterface(IID_IMetaDataError, (void**)&pIErr)==S_OK)
    {
        switch (hrIn)
        {
           
            case META_E_PARAM_COUNTS:
            case META_E_METHD_NOT_FOUND:
            case META_E_METHDIMPL_INCONSISTENT:
            {
                LPCUTF8     szMethodName;
                MethodRec   *pMethodRec;

                // Method name.
                _ASSERTE(TypeFromToken(token) == mdtMethodDef);
                pMethodRec = pMiniMd->getMethod(RidFromToken(token));
                szMethodName = pMiniMd->getNameOfMethod(pMethodRec);
                MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzMethodName, szMethodName);
                IfNullGo(wzMethodName);

                // Type and its name.
                IfFailGo( pMiniMd->FindParentOfMethodHelper(token, &tkParent) );
                pTypeRec = pMiniMd->getTypeDef(RidFromToken(tkParent));
                szTypeName = pMiniMd->getNameOfTypeDef(pTypeRec);
                szNSName = pMiniMd->getNamespaceOfTypeDef(pTypeRec);
                // Put namespace + name together.
                iLen = ns::GetFullLength(szNSName, szTypeName);
                IfFailGo(rName.ReSizeNoThrow(iLen+1));
                ns::MakePath(rName.Ptr(), iLen+1, szNSName, szTypeName);

                PostError(hrIn, (LPWSTR) rName.Ptr(), wzMethodName, token);
                break;
            }
            case META_E_FIELD_NOT_FOUND:
            {
                LPCUTF8     szFieldName;
                FieldRec   *pFieldRec;

                // Field name.
                _ASSERTE(TypeFromToken(token) == mdtFieldDef);
                pFieldRec = pMiniMd->getField(RidFromToken(token));
                szFieldName = pMiniMd->getNameOfField(pFieldRec);
                MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzFieldName, szFieldName);
                IfNullGo(wzFieldName);

                // Type and its name.
                IfFailGo( pMiniMd->FindParentOfFieldHelper(token, &tkParent) );
                pTypeRec = pMiniMd->getTypeDef(RidFromToken(tkParent));
                szTypeName = pMiniMd->getNameOfTypeDef(pTypeRec);
                szNSName = pMiniMd->getNamespaceOfTypeDef(pTypeRec);
                // Put namespace + name together.
                iLen = ns::GetFullLength(szNSName, szTypeName);
                IfFailGo(rName.ReSizeNoThrow(iLen+1));
                ns::MakePath(rName.Ptr(), iLen+1, szNSName, szTypeName);

                PostError(hrIn, (LPWSTR) rName.Ptr(), wzFieldName, token);
                break;
            }
            case META_E_EVENT_NOT_FOUND:
            {
                LPCUTF8     szEventName;
                EventRec   *pEventRec;

                // Event name.
                _ASSERTE(TypeFromToken(token) == mdtEvent);
                pEventRec = pMiniMd->getEvent(RidFromToken(token));
                szEventName = pMiniMd->getNameOfEvent(pEventRec);
                MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzEventName, szEventName);
                IfNullGo(wzEventName);

                // Type and its name.
                IfFailGo( pMiniMd->FindParentOfEventHelper(token, &tkParent) );
                pTypeRec = pMiniMd->getTypeDef(RidFromToken(tkParent));
                szTypeName = pMiniMd->getNameOfTypeDef(pTypeRec);
                szNSName = pMiniMd->getNamespaceOfTypeDef(pTypeRec);
                // Put namespace + name together.
                iLen = ns::GetFullLength(szNSName, szTypeName);
                IfFailGo(rName.ReSizeNoThrow(iLen+1));
                ns::MakePath(rName.Ptr(), iLen+1, szNSName, szTypeName);

                PostError(hrIn, (LPWSTR) rName.Ptr(), wzEventName, token);
                break;
            }
            case META_E_PROP_NOT_FOUND:
            {
                LPCUTF8     szPropertyName;
                PropertyRec   *pPropertyRec;

                // Property name.
                _ASSERTE(TypeFromToken(token) == mdtProperty);
                pPropertyRec = pMiniMd->getProperty(RidFromToken(token));
                szPropertyName = pMiniMd->getNameOfProperty(pPropertyRec);
                MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzPropertyName, szPropertyName);
                IfNullGo(wzPropertyName);

                // Type and its name.
                IfFailGo( pMiniMd->FindParentOfPropertyHelper(token, &tkParent) );
                pTypeRec = pMiniMd->getTypeDef(RidFromToken(tkParent));
                szTypeName = pMiniMd->getNameOfTypeDef(pTypeRec);
                szNSName = pMiniMd->getNamespaceOfTypeDef(pTypeRec);
                // Put namespace + name together.
                iLen = ns::GetFullLength(szNSName, szTypeName);
                IfFailGo(rName.ReSizeNoThrow(iLen+1));
                ns::MakePath(rName.Ptr(), iLen+1, szNSName, szTypeName);

                PostError(hrIn, (LPWSTR) rName.Ptr(), wzPropertyName, token);
                break;
            }
            case META_S_PARAM_MISMATCH:
            {
                LPCUTF8     szMethodName;
                MethodRec   *pMethodRec;
                mdToken     tkMethod;

                // Method name.
                _ASSERTE(TypeFromToken(token) == mdtParamDef);
                IfFailGo( pMiniMd->FindParentOfParamHelper(token, &tkMethod) );
                pMethodRec = pMiniMd->getMethod(RidFromToken(tkMethod));
                szMethodName = pMiniMd->getNameOfMethod(pMethodRec);
                MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzMethodName, szMethodName);
                IfNullGo(wzMethodName);

                // Type and its name.
                IfFailGo( pMiniMd->FindParentOfMethodHelper(token, &tkParent) );
                pTypeRec = pMiniMd->getTypeDef(RidFromToken(tkParent));
                szTypeName = pMiniMd->getNameOfTypeDef(pTypeRec);
                szNSName = pMiniMd->getNamespaceOfTypeDef(pTypeRec);
                // Put namespace + name together.
                iLen = ns::GetFullLength(szNSName, szTypeName);
                IfFailGo(rName.ReSizeNoThrow(iLen+1));
                ns::MakePath(rName.Ptr(), iLen+1, szNSName, szTypeName);

                // use the error hresult so that we can post the correct error.
                PostError(META_E_PARAM_MISMATCH, wzMethodName, (LPWSTR) rName.Ptr(), token);
                break;
            }
            case META_E_INTFCEIMPL_NOT_FOUND:
            {
                InterfaceImplRec    *pRec;          // The InterfaceImpl 
                mdToken             tkIface;        // Token of the implemented interface.
                CQuickArray<WCHAR>  rIface;         // Name of the Implemented Interface in unicode.
                TypeRefRec          *pRef;          // TypeRef record when II is a typeref.

                // Get the record.
                _ASSERTE(TypeFromToken(token) == mdtInterfaceImpl);
                pRec = pMiniMd->getInterfaceImpl(RidFromToken(token));
                // Get the name of the class.
                tkParent = pMiniMd->getClassOfInterfaceImpl(pRec);
                pTypeRec = pMiniMd->getTypeDef(RidFromToken(tkParent));
                szTypeName = pMiniMd->getNameOfTypeDef(pTypeRec);
                szNSName = pMiniMd->getNamespaceOfTypeDef(pTypeRec);
                // Put namespace + name together.
                iLen = ns::GetFullLength(szNSName, szTypeName);
                IfFailGo(rName.ReSizeNoThrow(iLen+1));
                ns::MakePath(rName.Ptr(), iLen+1, szNSName, szTypeName);

                // Get the name of the implemented interface.
                tkIface = pMiniMd->getInterfaceOfInterfaceImpl(pMiniMd->getInterfaceImpl(RidFromToken(token)));
                if (TypeFromToken(tkIface) == mdtTypeDef)
                {   // If it is a typedef...
                    pTypeRec = pMiniMd->getTypeDef(RidFromToken(tkIface));
                    szTypeName = pMiniMd->getNameOfTypeDef(pTypeRec);
                    szNSName = pMiniMd->getNamespaceOfTypeDef(pTypeRec);
                }
                else
                {   // If it is a typeref...
                    _ASSERTE(TypeFromToken(tkIface) == mdtTypeRef);
                    pRef = pMiniMd->getTypeRef(RidFromToken(tkIface));
                    szTypeName = pMiniMd->getNameOfTypeRef(pRef);
                    szNSName = pMiniMd->getNamespaceOfTypeRef(pRef);
                }
                // Put namespace + name together.
                iLen = ns::GetFullLength(szNSName, szTypeName);
                IfFailGo(rIface.ReSizeNoThrow(iLen+1));
                ns::MakePath(rIface.Ptr(), iLen+1, szNSName, szTypeName);


                PostError(hrIn, (LPWSTR) rName.Ptr(), (LPWSTR)rIface.Ptr(), token);
                break;
            }
            case META_E_CLASS_LAYOUT_INCONSISTENT:
            case META_E_METHOD_COUNTS:
            case META_E_FIELD_COUNTS:
            case META_E_EVENT_COUNTS:
            case META_E_PROPERTY_COUNTS:
            {
                // get the type name.
                _ASSERTE(TypeFromToken(token) == mdtTypeDef);
                pTypeRec = pMiniMd->getTypeDef(RidFromToken(token));
                szTypeName = pMiniMd->getNameOfTypeDef(pTypeRec);
                szNSName = pMiniMd->getNamespaceOfTypeDef(pTypeRec);
                // Put namespace + name together.
                iLen = ns::GetFullLength(szNSName, szTypeName);
                IfFailGo(rName.ReSizeNoThrow(iLen+1));
                ns::MakePath(rName.Ptr(), iLen+1, szNSName, szTypeName);

                PostError(hrIn, (LPWSTR) rName.Ptr(), token);
                break;
            }
            case META_E_GENERICPARAM_INCONSISTENT:
            {
                // If token is type, get type name; if method, get method name.
                LPWSTR      wzName;
                LPCUTF8     szMethodName;
                MethodRec   *pMethodRec;

                if ((TypeFromToken(token) == mdtMethodDef))
                {
                    // Get the method name.
                    pMethodRec = pMiniMd->getMethod(RidFromToken(token));
                    szMethodName = pMiniMd->getNameOfMethod(pMethodRec);
                    MAKE_WIDEPTR_FROMUTF8_NOTHROW(wzMethodName, szMethodName);
                    IfNullGo(wzMethodName);
                    wzName = wzMethodName;
                }
                else
                {
                    // Get the type name.
                    _ASSERTE(TypeFromToken(token) == mdtTypeDef);
                    pTypeRec = pMiniMd->getTypeDef(RidFromToken(token));
                    szTypeName = pMiniMd->getNameOfTypeDef(pTypeRec);
                    szNSName = pMiniMd->getNamespaceOfTypeDef(pTypeRec);
                    // Put namespace + name together.
                    iLen = ns::GetFullLength(szNSName, szTypeName);
                    IfFailGo(rName.ReSizeNoThrow(iLen+1));
                    ns::MakePath(rName.Ptr(), iLen+1, szNSName, szTypeName);
                    wzName = (LPWSTR)rName.Ptr();
                }

                PostError(hrIn, wzName, token);
                break;
            }
            case META_E_TYPEDEF_MISSING:
            {
                TypeRefRec          *pRef;          // TypeRef record when II is a typeref.

                // Get the record.
                _ASSERTE(TypeFromToken(token) == mdtTypeRef);
                pRef = pMiniMd->getTypeRef(RidFromToken(token));
                szTypeName = pMiniMd->getNameOfTypeRef(pRef);
                szNSName = pMiniMd->getNamespaceOfTypeRef(pRef);
                
                // Put namespace + name together.
                iLen = ns::GetFullLength(szNSName, szTypeName);
                IfFailGo(rName.ReSizeNoThrow(iLen+1));
                ns::MakePath(rName.Ptr(), iLen+1, szNSName, szTypeName);


                PostError(hrIn, (LPWSTR) rName.Ptr(), token);
                break;
            }
            default:
            {
                PostError(hrIn, token);
                break;
            }
        }
        hr = pIErr->OnError(hrIn, token);
    }
    else
        hr = S_FALSE;
ErrExit:
    if (pIErr)
        pIErr->Release();
    return (hr);
} // HRESULT NEWMERGER::OnError()


