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
/*****************************************************************************
 **                                                                         **
 ** Xmlreader.h - general header for the shim parser                        **
 **                                                                         **
 *****************************************************************************/


#ifndef _XMLREADER_H_
#define _XMLREADER_H_

#include <corerror.h>

#define STARTUP_FOUND EMAKEHR(0xffff)           // This does not leak out of the shim so we can set it to anything

// This will cap a version length at 25 characters. This allows a version
// string to look like
// vXXXXX.XXXXX.XXXXX.XXXXX

#define MAX_VERSION_LENGTH 25

HRESULT 
XMLGetVersion(LPCWSTR filename, 
              __out LPWSTR* pVersion, 
              __out_opt LPWSTR* pImageVersion, 
              __out_opt LPWSTR* pBuildFlavor, 
              BOOL* bSafeMode,
              BOOL *bRequiredTagSafeMode);

HRESULT 
XMLGetVersionFromStream(IStream* pCfgStream, 
                        DWORD dwReserved, 
                        __out LPWSTR* pVersion, 
                        __out_opt LPWSTR* pImageVersion, 
                        __out_opt LPWSTR* pBuildFlavor, 
                        BOOL *bSafeMode, 
                        BOOL *bRequiredTagSafeMode);

HRESULT 
XMLGetVersionWithSupported(PCWSTR filename, 
                           __out LPWSTR* pVersion, 
                           __out_opt LPWSTR* pImageVersion, 
                           __out_opt LPWSTR* pBuildFlavor, 
                           BOOL *bSafeMode,
                           BOOL *bRequiredTagSafeMode,
                           __out_opt LPWSTR** pwszSupportedVersions, 
                           DWORD* nSupportedVersions);

HRESULT 
XMLGetVersionWithSupportedFromStream(IStream* pCfgStream, 
                                     DWORD dwReserved, 
                                     __out LPWSTR* pVersion, 
                                     __out_opt LPWSTR* pImageVersion, 
                                     __out_opt LPWSTR* pBuildFlavor, 
                                     BOOL *bSafeMode,
                                     BOOL *bRequiredTagSafeMode,
                                     __out_opt LPWSTR** pwszSupportedVersions, 
                                     DWORD* nSupportedVersions);

int 
XMLStringCompare(const WCHAR *pStr1, 
                    DWORD cchStr1, 
                    const WCHAR *pStr2, 
                    DWORD cchStr2);
#endif
