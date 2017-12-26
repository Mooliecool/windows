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
// Helper.cpp
//
// Implementation for the meta data emit code.
//
//*****************************************************************************
#include "stdafx.h"
#include "regmeta.h"
#include "importhelper.h"
#include <sighelper.h>
#include "mdlog.h"

//*****************************************************************************
// translating signature from one scope to another scope
//*****************************************************************************
STDMETHODIMP RegMeta::TranslateSigWithScope(    // S_OK or error.
    IMetaDataAssemblyImport *pAssemImport, // [IN] importing assembly interface
    const void  *pbHashValue,           // [IN] Hash Blob for Assembly.
    ULONG       cbHashValue,            // [IN] Count of bytes.
    IMetaDataImport *pImport,           // [IN] importing interface
    PCCOR_SIGNATURE pbSigBlob,          // [IN] signature in the importing scope
    ULONG       cbSigBlob,              // [IN] count of bytes of signature
    IMetaDataAssemblyEmit   *pAssemEmit,// [IN] emit assembly interface
    IMetaDataEmit *pEmit,               // [IN] emit interface
    PCOR_SIGNATURE pvTranslatedSig,     // [OUT] buffer to hold translated signature
    ULONG       cbTranslatedSigMax,
    ULONG       *pcbTranslatedSig)      // [OUT] count of bytes in the translated signature
{
    HRESULT     hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    RegMeta     *pRegMetaAssemEmit = static_cast<RegMeta*>(pAssemEmit);
    RegMeta     *pRegMetaAssemImport = static_cast<RegMeta*>(pAssemImport);
    RegMeta     *pRegMetaEmit = NULL;
    RegMeta     *pRegMetaImport = NULL;
    
    IMetaModelCommon *pCommonAssemImport;
    IMetaModelCommon *pCommonImport;
    CQuickBytes qkSigEmit;
    ULONG       cbEmit;

    pRegMetaEmit = static_cast<RegMeta*>(pEmit);
    pRegMetaImport = static_cast<RegMeta*>(pImport);

    {
        // This function can cause new TypeRef being introduced.
        LOCKWRITE();

        _ASSERTE(pvTranslatedSig && pcbTranslatedSig);

        pCommonAssemImport = pRegMetaAssemImport ?
            static_cast<IMetaModelCommon*>(&(pRegMetaAssemImport->m_pStgdb->m_MiniMd)) : 0;
        pCommonImport = static_cast<IMetaModelCommon*>(&(pRegMetaImport->m_pStgdb->m_MiniMd));

        IfFailGo( ImportHelper::MergeUpdateTokenInSig(  // S_OK or error.
                pRegMetaAssemEmit ? &(pRegMetaAssemEmit->m_pStgdb->m_MiniMd) : 0, // The assembly emit scope.
                &(pRegMetaEmit->m_pStgdb->m_MiniMd),    // The emit scope.
                pCommonAssemImport,                     // Assembly where the signature is from.
                pbHashValue,                            // Hash value for the import assembly.
                cbHashValue,                            // Size in bytes.
                pCommonImport,                          // The scope where signature is from.
                pbSigBlob,                              // signature from the imported scope
                NULL,                                   // Internal OID mapping structure.
                &qkSigEmit,                             // [OUT] translated signature
                0,                                      // start from first byte of the signature
                0,                                      // don't care how many bytes consumed
                &cbEmit));                              // [OUT] total number of bytes write to pqkSigEmit
        memcpy(pvTranslatedSig, qkSigEmit.Ptr(), cbEmit > cbTranslatedSigMax ? cbTranslatedSigMax :cbEmit );
        *pcbTranslatedSig = cbEmit;
        if (cbEmit > cbTranslatedSigMax)
            hr = CLDB_S_TRUNCATION;
    }

ErrExit:


    END_ENTRYPOINT_NOTHROW;

    return hr;
} // STDMETHODIMP RegMeta::TranslateSigWithScope()


//*****************************************************************************
// Helper : export typelib from this module
//*****************************************************************************
STDMETHODIMP RegMeta::ExportTypeLibFromModule(    // Result.
    LPCWSTR     szModule,                   // [IN] Module name.
    LPCWSTR     szTlb,                      // [IN] Typelib name.
    BOOL        bRegister)                  // [IN] Set to TRUE to have the typelib be registered.
{
    _ASSERTE(false);
    return E_NOTIMPL;
} // HRESULT RegMeta::ExportTypeLibFromModule()




//*****************************************************************************
// Helper : Set ResolutionScope of a TypeRef
//*****************************************************************************
HRESULT RegMeta::SetResolutionScopeHelper(  // Return hresult.
    mdTypeRef   tr,                     // [IN] TypeRef record to update
    mdToken     rs)                     // [IN] new ResolutionScope
{
    HRESULT     hr = NOERROR;
    TypeRefRec  *pTypeRef;

    LOCKWRITE();

    pTypeRef = m_pStgdb->m_MiniMd.getTypeRef(RidFromToken(tr));
    IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_TypeRef, TypeRefRec::COL_ResolutionScope, pTypeRef, rs));
ErrExit:
    return hr;
}   // SetResolutionScopeHelper


//*****************************************************************************
// Helper : Set offset of a ManifestResource
//*****************************************************************************
HRESULT RegMeta::SetManifestResourceOffsetHelper(  // Return hresult.
    mdManifestResource mr,              // [IN] The manifest token
    ULONG       ulOffset)               // [IN] new offset
{
    HRESULT     hr = NOERROR;
    ManifestResourceRec  *pRec;

    LOCKWRITE();

    pRec = m_pStgdb->m_MiniMd.getManifestResource(RidFromToken(mr));
    pRec->SetOffset(ulOffset);

ErrExit:
    return hr;
}   // SetManifestResourceOffsetHelper

//*****************************************************************************
// Helper : get metadata information
//*****************************************************************************
STDMETHODIMP RegMeta::GetMetadata(                // Result.
    ULONG       ulSelect,                   // [IN] Selector.
    void        **ppData)                   // [OUT] Put pointer to data here.
{
    switch (ulSelect)
    {
    case 0:
        *ppData = &m_pStgdb->m_MiniMd;
        break;
    case 1:
        *ppData = (void*)g_CodedTokens;
        break;
    case 2:
        *ppData = (void*)g_Tables;
        break;
    default:
        *ppData = 0;
        break;
    }

    return S_OK;
} // STDMETHODIMP RegMeta::GetMetadata()




//*******************************************************************************
//
// IMetaDataEmitHelper.
// This following APIs are used by reflection emit.
//
//
//*******************************************************************************

//*******************************************************************************
// helper to define method semantics
//*******************************************************************************
HRESULT RegMeta::DefineMethodSemanticsHelper(
    mdToken     tkAssociation,          // [IN] property or event token
    DWORD       dwFlags,                // [IN] semantics
    mdMethodDef md)                     // [IN] method to associated with
{
    HRESULT     hr;
    LOCKWRITE();
    hr = _DefineMethodSemantics((USHORT) dwFlags, md, tkAssociation, false);

ErrExit:

    return hr;
}   // DefineMethodSemantics



//*******************************************************************************
// helper to set field layout
//*******************************************************************************
HRESULT RegMeta::SetFieldLayoutHelper(  // Return hresult.
    mdFieldDef  fd,                     // [IN] field to associate the layout info
    ULONG       ulOffset)               // [IN] the offset for the field
{
    HRESULT     hr;
    FieldLayoutRec *pFieldLayoutRec;
    RID         iFieldLayoutRec;

    LOCKWRITE();

    if (ulOffset == ULONG_MAX)
    {
        // invalid argument
        IfFailGo( E_INVALIDARG );
    }

    // create a field layout record
    IfNullGo(pFieldLayoutRec = m_pStgdb->m_MiniMd.AddFieldLayoutRecord(&iFieldLayoutRec));

    // Set the Field entry.
    IfFailGo(m_pStgdb->m_MiniMd.PutToken(
        TBL_FieldLayout,
        FieldLayoutRec::COL_Field,
        pFieldLayoutRec,
        fd));
    pFieldLayoutRec->SetOffSet(ulOffset);
    IfFailGo( m_pStgdb->m_MiniMd.AddFieldLayoutToHash(iFieldLayoutRec) );

ErrExit:

    return hr;
}   // SetFieldLayout



//*******************************************************************************
// helper to define event
//*******************************************************************************
STDMETHODIMP RegMeta::DefineEventHelper(    // Return hresult.
    mdTypeDef   td,                     // [IN] the class/interface on which the event is being defined
    LPCWSTR     szEvent,                // [IN] Name of the event
    DWORD       dwEventFlags,           // [IN] CorEventAttr
    mdToken     tkEventType,            // [IN] a reference (mdTypeRef or mdTypeRef) to the Event class
    mdEvent     *pmdEvent)              // [OUT] output event token
{
    HRESULT     hr = S_OK;
    LOG((LOGMD, "MD RegMeta::DefineEventHelper(0x%08x, %S, 0x%08x, 0x%08x, 0x%08x)\n",
        td, szEvent, dwEventFlags, tkEventType, pmdEvent));

    LOCKWRITE();

    m_pStgdb->m_MiniMd.PreUpdate();

    hr = _DefineEvent(td, szEvent, dwEventFlags, tkEventType, pmdEvent);

ErrExit:
    
    return hr;
}   // DefineEvent


//*******************************************************************************
// helper to add a declarative security blob to a class or method
//*******************************************************************************
STDMETHODIMP RegMeta::AddDeclarativeSecurityHelper(
    mdToken     tk,                     // [IN] Parent token (typedef/methoddef)
    DWORD       dwAction,               // [IN] Security action (CorDeclSecurity)
    void const  *pValue,                // [IN] Permission set blob
    DWORD       cbValue,                // [IN] Byte count of permission set blob
    mdPermission*pmdPermission)         // [OUT] Output permission token
{
    HRESULT         hr = S_OK;
    DeclSecurityRec *pDeclSec = NULL;
    RID             iDeclSec;
    short           sAction = static_cast<short>(dwAction);
    mdPermission    tkPerm;

    LOG((LOGMD, "MD RegMeta::AddDeclarativeSecurityHelper(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)\n",
        tk, dwAction, pValue, cbValue, pmdPermission));

    LOCKWRITE();
    m_pStgdb->m_MiniMd.PreUpdate();

    _ASSERTE(TypeFromToken(tk) == mdtTypeDef || TypeFromToken(tk) == mdtMethodDef || TypeFromToken(tk) == mdtAssembly);

    // Check for valid Action.
    if (sAction == 0 || sAction > dclMaximumValue)
        IfFailGo(E_INVALIDARG);

    if (CheckDups(MDDupPermission))
    {
        hr = ImportHelper::FindPermission(&(m_pStgdb->m_MiniMd), tk, sAction, &tkPerm);

        if (SUCCEEDED(hr))
        {
            // Set output parameter.
            if (pmdPermission)
                *pmdPermission = tkPerm;
            if (IsENCOn())
                pDeclSec = m_pStgdb->m_MiniMd.getDeclSecurity(RidFromToken(tkPerm));
            else
            {
                hr = META_S_DUPLICATE;
                goto ErrExit;
            }
        }
        else if (hr != CLDB_E_RECORD_NOTFOUND)
            IfFailGo(hr);
    }

    // Create a new record.
    if (!pDeclSec)
    {
        IfNullGo(pDeclSec = m_pStgdb->m_MiniMd.AddDeclSecurityRecord(&iDeclSec));
        tkPerm = TokenFromRid(iDeclSec, mdtPermission);

        // Set output parameter.
        if (pmdPermission)
            *pmdPermission = tkPerm;

        // Save parent and action information.
        IfFailGo(m_pStgdb->m_MiniMd.PutToken(TBL_DeclSecurity, DeclSecurityRec::COL_Parent, pDeclSec, tk));
        pDeclSec->SetAction(sAction);

        // Turn on the internal security flag on the parent.
        if (TypeFromToken(tk) == mdtTypeDef)
            IfFailGo(_TurnInternalFlagsOn(tk, tdHasSecurity));
        else if (TypeFromToken(tk) == mdtMethodDef)
            IfFailGo(_TurnInternalFlagsOn(tk, mdHasSecurity));
        IfFailGo(UpdateENCLog(tk));
    }

    // Write the blob into the record.
    IfFailGo(m_pStgdb->m_MiniMd.PutBlob(TBL_DeclSecurity, DeclSecurityRec::COL_PermissionSet,
                                        pDeclSec, pValue, cbValue));

    IfFailGo(UpdateENCLog(tkPerm));

ErrExit:

    return hr;
}


//*******************************************************************************
// helper to set type's extends column
//*******************************************************************************
HRESULT RegMeta::SetTypeParent(         // Return hresult.
    mdTypeDef   td,                     // [IN] Type definition
    mdToken     tkExtends)              // [IN] parent type
{
    HRESULT     hr;
    TypeDefRec  *pRec;

    LOCKWRITE();

    IfNullGo( pRec = m_pStgdb->m_MiniMd.getTypeDef(RidFromToken(td)) );
    IfFailGo( m_pStgdb->m_MiniMd.PutToken(TBL_TypeDef, TypeDefRec::COL_Extends, pRec, tkExtends) );

ErrExit:
    return hr;
}   // SetTypeParent


//*******************************************************************************
// helper to set type's extends column
//*******************************************************************************
HRESULT RegMeta::AddInterfaceImpl(      // Return hresult.
    mdTypeDef   td,                     // [IN] Type definition
    mdToken     tkInterface)            // [IN] interface type
{
    HRESULT             hr;
    InterfaceImplRec    *pRec;
    RID                 ii;

    LOCKWRITE();
    hr = ImportHelper::FindInterfaceImpl(&(m_pStgdb->m_MiniMd), td, tkInterface, (mdInterfaceImpl *)&ii);
    if (hr == S_OK)
        goto ErrExit;
    IfNullGo( pRec = m_pStgdb->m_MiniMd.AddInterfaceImplRecord((RID *)&ii) );
    IfFailGo( m_pStgdb->m_MiniMd.PutToken( TBL_InterfaceImpl, InterfaceImplRec::COL_Class, pRec, td) );
    IfFailGo( m_pStgdb->m_MiniMd.PutToken( TBL_InterfaceImpl, InterfaceImplRec::COL_Interface, pRec, tkInterface) );

ErrExit:
    return hr;
}   // AddInterfaceImpl

//*******************************************************************************
// helper to change MVID
//*******************************************************************************
HRESULT RegMeta::ChangeMvid(            // S_OK or error.
    REFGUID newMvid)                    // GUID to use as the MVID
{
    return GetMiniMd()->ChangeMvid(newMvid);
}

