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
// MDInternalDispenser.h
//
// Contains utility code for MD directory
//
//*****************************************************************************
#ifndef __MDInternalDispenser__h__
#define __MDInternalDispenser__h__



#include "mdinternalro.h"


enum MDFileFormat
{
    MDFormat_ReadOnly = 0,
    MDFormat_ReadWrite = 1,
    MDFormat_ICR = 2,
    MDFormat_Invalid = 3
};


HRESULT CheckFileFormat(LPVOID pData, ULONG cbData, MDFileFormat *pFormat);
STDAPI GetMDInternalInterface(
    LPVOID      pData,                  // [IN] Buffer with the metadata.
    ULONG       cbData,                 // [IN] Size of the data in the buffer.
    DWORD       flags,                  // [IN] MDInternal_OpenForRead or MDInternal_OpenForENC
    REFIID      riid,                   // [in] The interface desired.
    void        **ppIUnk);              // [out] Return interface on success.

HRESULT GetAssemblyMDInternalImportHelper(HCORMODULE hModule,
                                          DWORD      dwFileLength,
                                          REFIID     riid,
                                          MDInternalImportFlags flags,
                                          IUnknown   **ppIUnk);

#endif // __MDInternalDispenser__h__
