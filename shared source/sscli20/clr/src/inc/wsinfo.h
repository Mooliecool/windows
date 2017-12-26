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
#ifndef WSINFO_H_
#define WSINFO_H_

#include <ex.h>
#include <cor.h>

class WSInfo
{
 public:

    ULONG   m_unknown;
    ULONG   m_cTypes;
    ULONG   *m_pTypeSizes;
    ULONG   m_cMethods;
    ULONG   *m_pMethodSizes;
    ULONG   m_cFields;
    ULONG   *m_pFieldSizes;

    IMetaDataImport *m_pImport;

    WSInfo(IMetaDataImport *pImport);
    ~WSInfo();

    void AdjustTypeSize(mdTypeDef token, LONG size);
    void AdjustMethodSize(mdMethodDef token, LONG size);
    void AdjustFieldSize(mdFieldDef token, LONG size);
    void AdjustTokenSize(mdToken token, LONG size);

    ULONG GetTotalUnknownSize();
    ULONG GetTotalAttributedSize();
};


#endif // WSINFO_H_
