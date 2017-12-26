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
#ifndef _DASM_SZ_H_
#define _DASM_SZ_H_

unsigned SizeOfValueType(mdToken tk, IMDInternalImport* pImport);

unsigned SizeOfField(mdToken tk, IMDInternalImport* pImport);

unsigned SizeOfField(PCCOR_SIGNATURE *ppSig, ULONG cSig, IMDInternalImport* pImport);

#endif
