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
#include "stdafx.h"
#include "wsinfo.h"
#include "holder.h"

WSInfo::WSInfo(IMetaDataImport *pImport)
    : m_unknown(0),
	m_pImport(pImport)
{
    CONTRACTL
    {
        THROWS;     // calls new and IfFailThrow
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    
    HCORENUM hEnum = NULL;
    DWORD count;

    IfFailThrow(pImport->EnumTypeDefs(&hEnum, NULL, 0, NULL));
    IfFailThrow(pImport->CountEnum(hEnum, &m_cTypes));
    m_cTypes += 1;

    //

    m_cMethods = 0;
    m_cFields = 0;
    while (TRUE)
    {
        mdTypeDef td;
        IfFailThrow(pImport->EnumTypeDefs(&hEnum, &td, 1, &count));

        if (count == 0)
            break;

        HCORENUM hMethodEnum = NULL;
        IfFailThrow(pImport->EnumMethods(&hMethodEnum, td, NULL, 0, NULL));
        IfFailThrow(pImport->CountEnum(hMethodEnum, &count));
        pImport->CloseEnum(hMethodEnum);

        m_cMethods += count;

        HCORENUM hFieldEnum = NULL;
        IfFailThrow(pImport->EnumFields(&hFieldEnum, td, NULL, 0, NULL));
        IfFailThrow(pImport->CountEnum(hFieldEnum, &count));
        pImport->CloseEnum(hFieldEnum);

        m_cFields += count;
    }

    pImport->CloseEnum(hEnum);

    HCORENUM hMethodEnum = NULL;
    IfFailThrow(pImport->EnumMethods(&hMethodEnum, mdTypeDefNil, NULL, 0, NULL));
    IfFailThrow(pImport->CountEnum(hMethodEnum, &count));
    pImport->CloseEnum(hMethodEnum);

    m_cMethods += count;

    HCORENUM hFieldEnum = NULL;
    IfFailThrow(pImport->EnumFields(&hFieldEnum, mdTypeDefNil, NULL, 0, NULL));
    IfFailThrow(pImport->CountEnum(hFieldEnum, &count));
    pImport->CloseEnum(hFieldEnum);

    m_cFields += count;

    //
    // Now allocate arrays
    //

    m_pTypeSizes = new ULONG [ m_cTypes+1 ];
    NewArrayHolder<ULONG> typeSizesHolder(m_pTypeSizes);
    ZeroMemory(m_pTypeSizes, sizeof(ULONG) * (m_cTypes+1));

    m_pMethodSizes = new ULONG [ m_cMethods+1 ];
    NewArrayHolder<ULONG> methodSizesHolder(m_pMethodSizes);
    ZeroMemory(m_pMethodSizes, sizeof(ULONG) * (m_cMethods+1));

    m_pFieldSizes = new ULONG [ m_cFields+1 ];
    ZeroMemory(m_pFieldSizes, sizeof(ULONG) * (m_cFields+1));

    typeSizesHolder.SuppressRelease();
    methodSizesHolder.SuppressRelease();
}

WSInfo::~WSInfo()
{
    CONTRACTL
    {
        THROWS;     // calls delete
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    if (m_pImport != NULL)
        m_pImport->Release();

    if (m_pTypeSizes != NULL)
        delete [] m_pTypeSizes;

    if (m_pMethodSizes != NULL)
        delete [] m_pMethodSizes;

    if (m_pFieldSizes != NULL)
        delete [] m_pFieldSizes;
}

void WSInfo::AdjustTypeSize(mdTypeDef token, LONG size)
{
    LEAF_CONTRACT;
    
    if (IsNilToken(token))
    {
        m_pTypeSizes[0] += size;
    }
    else
    {
        _ASSERTE(m_pImport->IsValidToken(token));
        m_pTypeSizes[RidFromToken(token)] += size;
    }
}

void WSInfo::AdjustMethodSize(mdMethodDef token, LONG size)
{
    LEAF_CONTRACT;

    if (IsNilToken(token))
    {
        m_pMethodSizes[0] += size;
    }
    else
    {
        _ASSERTE(m_pImport->IsValidToken(token));
        m_pMethodSizes[RidFromToken(token)] += size;
    }

}

void WSInfo::AdjustFieldSize(mdFieldDef token, LONG size)
{
    LEAF_CONTRACT;    

    if (IsNilToken(token))
    {
        m_pFieldSizes[0] += size;
    }
    else
    {
        _ASSERTE(m_pImport->IsValidToken(token));
        m_pFieldSizes[RidFromToken(token)] += size;
    }
}

void WSInfo::AdjustTokenSize(mdToken token, LONG size)
{
    LEAF_CONTRACT;    
    switch (TypeFromToken(token))
    {
    case mdtTypeDef:
        AdjustTypeSize(token, size);
        return;

    case mdtFieldDef:
        AdjustFieldSize(token, size);
        return;

    case mdtMethodDef:
        AdjustMethodSize(token, size);
        return;
        
    default:
        m_unknown += size;
        break;
    }
}

ULONG WSInfo::GetTotalAttributedSize()
{
    LEAF_CONTRACT;
    ULONG size = 0;

    ULONG *p, *pEnd;

    p = m_pTypeSizes;
    pEnd = p + m_cTypes;
    while (p < pEnd)
        size += *p++;

    p = m_pMethodSizes;
    pEnd = p + m_cMethods;
    while (p < pEnd)
        size += *p++;

    p = m_pFieldSizes;
    pEnd = p + m_cFields;
    while (p < pEnd)
        size += *p++;

    return size;
}

ULONG WSInfo::GetTotalUnknownSize()
{
    ULONG size = m_unknown;

    size += m_pTypeSizes[0];
    size += m_pFieldSizes[0];
    size += m_pMethodSizes[0];

    return size;
}


