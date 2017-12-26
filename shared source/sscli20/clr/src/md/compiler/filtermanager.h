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
// FilterManager.h
//
// Contains utility code for MD directory
//
//*****************************************************************************
#ifndef __FilterManager__h__
#define __FilterManager__h__




//*********************************************************************
// FilterManager Class 
//*********************************************************************
class FilterManager
{
public:
        FilterManager(CMiniMdRW *pMiniMd) {m_pMiniMd = pMiniMd; hasModuleBeenMarked = false; hasAssemblyBeenMarked = false;}
        ~FilterManager() {};

        HRESULT Mark(mdToken tk);

    // Unmark helper                                                                                    
    HRESULT UnmarkTypeDef(mdTypeDef td);
    HRESULT MarkNewUserString(mdString str);


private:
        HRESULT MarkCustomAttribute(mdCustomAttribute cv);
        HRESULT MarkDeclSecurity(mdPermission pe);
        HRESULT MarkStandAloneSig(mdSignature sig);
        HRESULT MarkTypeSpec(mdTypeSpec ts);
        HRESULT MarkTypeRef(mdTypeRef tr);
        HRESULT MarkMemberRef(mdMemberRef mr);
        HRESULT MarkModuleRef(mdModuleRef mr);
        HRESULT MarkAssemblyRef(mdAssemblyRef ar);
        HRESULT MarkModule(mdModule mo);
    HRESULT MarkAssembly(mdAssembly as);
        HRESULT MarkInterfaceImpls(mdTypeDef    td);
    HRESULT MarkUserString(mdString str);

    HRESULT MarkMethodSpec(mdMethodSpec ms);

        HRESULT MarkCustomAttributesWithParentToken(mdToken tkParent);
        HRESULT MarkDeclSecuritiesWithParentToken(mdToken tkParent);
        HRESULT MarkMemberRefsWithParentToken(mdToken tk);

        HRESULT MarkParam(mdParamDef pd);
        HRESULT MarkMethod(mdMethodDef md);
        HRESULT MarkField(mdFieldDef fd);
        HRESULT MarkEvent(mdEvent ev);
        HRESULT MarkProperty(mdProperty pr);

        HRESULT MarkParamsWithParentToken(mdMethodDef md);
        HRESULT MarkMethodsWithParentToken(mdTypeDef td);
    HRESULT MarkMethodImplsWithParentToken(mdTypeDef td);
        HRESULT MarkFieldsWithParentToken(mdTypeDef td);
        HRESULT MarkEventsWithParentToken(mdTypeDef td);
        HRESULT MarkPropertiesWithParentToken(mdTypeDef td);

    HRESULT MarkGenericParamWithParentToken(mdToken tk);


        HRESULT MarkTypeDef(mdTypeDef td);


        HRESULT MarkMethodDebugInfo(mdMethodDef md);

        // walk the signature and mark all of the embedded TypeDef or TypeRef
        HRESULT MarkSignature(PCCOR_SIGNATURE pbSig, ULONG cbSig, ULONG *pcbUsed);
        HRESULT MarkFieldSignature(PCCOR_SIGNATURE pbSig, ULONG cbSig, ULONG *pcbUsed);


private:
        CMiniMdRW       *m_pMiniMd;
    bool        hasModuleBeenMarked;
    bool        hasAssemblyBeenMarked;
};

#endif // __FilterManager__h__
