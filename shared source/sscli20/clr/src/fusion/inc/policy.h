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
#ifndef __POLICY_H_INCLUDED__
#define __POLICY_H_INCLUDED__

#include "nodefact.h"
#include "helpers.h"

#define POLICY_ASSEMBLY_PREFIX                   L"policy."

class CDebugLog;
class CApplicationContext;

// Get the various property out of IAssemblyName object
HRESULT PrepQueryMatchData(IAssemblyName *pName, 
                           __out_ecount(*pdwSizeName) LPWSTR pwzAsmName,
                           __inout LPDWORD pdwSizeName,
                           __out_ecount(*pdwSizeVer) LPWSTR pwzAsmVersion, 
                           __inout LPDWORD pdwSizeVer,
                           __out_ecount(*pdwSizePKT) LPWSTR pwzPublicKeyToken, 
                           __inout LPDWORD pdwSizePKT,
                           __out_ecount_opt(pdwSizeCulture) LPWSTR pwzCulture,
                           __inout_opt LPDWORD pdwSizeCulture,
                           __out_opt PEKIND   *pe
                           );

// Given input assembly info, 
// get the path of publish policy for that assembly
HRESULT GetPublisherPolicyFilePath(LPCWSTR pwzAsmName, 
                                   LPCWSTR pwzPublicKeyToken,
                                   LPCWSTR pwzCulture, 
                                   PEKIND peIn, 
                                   WORD wVerMajor,
                                   WORD wVerMinor, 
                                   __out_ecount_opt(*pdwSize) LPWSTR pwzPublisherCfg,
                                   __inout LPDWORD pdwSize,
                                   __out PEKIND *peOut);

HRESULT GetRetargetPolicyVersion(LPCWSTR wzAssemblyNameIn, 
                                 LPCWSTR wzVersionIn, 
                                 LPCWSTR wzCulture, 
                                 LPCWSTR wzPublicKeyTokenIn, 
                                 __out_ecount(*pdwSizeName) LPWSTR  pwzAssemblyNameOut,
                                 __inout LPDWORD pdwSizeName,
                                 __out_ecount(*pdwSizeVersion) LPWSTR  pwzVersionOut,
                                 __inout LPDWORD pdwSizeVersion,
                                 __out_ecount(*pdwSizePKT) LPWSTR  pwzPublicKeyTokenOut, 
                                 __inout LPDWORD pdwSizePKT
                                 );

HRESULT GetFrameworkPolicyVersion(LPCWSTR wzAssemblyName, 
                                 LPCWSTR wzVersion,
                                 LPCWSTR wzCulture, 
                                 LPCWSTR wzPublicKeyToken, 
                                 __out_ecount(*pdwSizeVersion) LPWSTR  pwzVersionOut,
                                 __inout LPDWORD pdwSizeVersion);

HRESULT IsFrameworkAssembly(LPCWSTR pwzAsmName, 
                            LPCWSTR pwzVersion,
                            LPCWSTR pwzCulture, 
                            LPCWSTR pwzPublicKeyToken, 
                            BOOL *pbIsFrameworkAssembly,
                            LPCWSTR *ppwzFrameworkVersion = NULL);

HRESULT IsFrameworkAssembly(IAssemblyName *pName, 
                            BOOL *pbIsFrameworkAssembly,
                            LPCWSTR *ppwzFrameworkVersion = NULL);

HRESULT ParseXML(CNodeFactory **ppNodeFactory, 
                 LPCWSTR wzFileName, 
                 CDebugLog *pdbglog,
                 BOOL bProcessLinkedConfigurations,
                 CNodeFactory::ParseCtl parseCtl = CNodeFactory::parseAll);

HRESULT ParseXML(CNodeFactory *pNodeFactory,
                 LPCWSTR wzFileName,
                 CDebugLog *pdbglog);

HRESULT ParseXML(CNodeFactory **ppNodeFactory, 
                 LPVOID lpMemory, 
                 ULONG cbSize, 
                 CDebugLog *pdbglog);

HRESULT ApplyPolicy(
        /* in */    IAssemblyName *pNameSource,     // name before policy
        /* in */    IApplicationContext *pAppCtx,   // contains various setting
        /* in */    CDebugLog *pdbglog,             // fuslog
        /* out */   IAssemblyName **ppNamePolicy,   // name after policy
        /* out */   __deref_opt_out_opt LPWSTR *ppwzPolicyCodebase,     // policy codebase hint
        /* out */   AsmBindHistoryInfo **ppHistInfo,  // history logging
        /* out */   DWORD *pdwPolicyApplied);       // binding result info

// Parse the version string.
HRESULT GetVersionFromString(LPCWSTR wzStr, ULONGLONG *pullVer);

// Check if the given version is within the range of versions
// specified in config file.
// return 
//  S_OK        matched
//  S_FALSE     not matched
//  OTHER       failure
HRESULT IsMatchingVersion(LPCWSTR wzVerCfg, LPCWSTR wzVerSource);

#endif 

