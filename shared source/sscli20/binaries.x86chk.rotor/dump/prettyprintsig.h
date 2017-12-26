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
// This code supports formatting a method and it's signature in a friendly
// and consistent format.
//
//*****************************************************************************
#ifndef __PrettyPrintSig_h__
#define __PrettyPrintSig_h__

#include <cor.h>

class CQuickBytes;

//
// The return value is either NULL or a null-terminated ASCII string
//

LPCWSTR PrettyPrintSig(
    PCCOR_SIGNATURE sigPtr,             // Method/field sig to convert
    unsigned    sigLen,                 // length of sig
    LPCWSTR     name,                   // the name of the method for this sig. Can be L""
    CQuickBytes *scratch,               // scratch buffer to use
    IMetaDataImport *pIMDI);            // Import api to use.

struct IMDInternalImport;

//
// On success, the null-terminated ASCII string is in "out.Ptr()"
//

HRESULT PrettyPrintSigInternal(         // S_OK or error.
    PCCOR_SIGNATURE sigPtr,             // sig to convert,     
    unsigned    sigLen,                 // length of sig
    LPCSTR  name,                       // can be "", the name of the method for this sig
    CQuickBytes *out,                   // where to put the pretty printed string   
    IMDInternalImport *pIMDI);          // Import api to use.

//
// On success, the null-terminated ASCII string is in "out.Ptr()"
//

HRESULT PrettyPrintTypeInternalA( // S_OK or error.
    PCCOR_SIGNATURE sigPtr,             // type sig to convert
    size_t          sigLen,             // length of signature
    CQuickBytes     *out,               // buffer where the null-terminated ASCII output is written
    IMDInternalImport *pIMDI);          // Import api to use.


#endif // __PrettyPrintSig_h__
