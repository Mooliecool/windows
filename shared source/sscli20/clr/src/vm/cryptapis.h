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

#error FEATURE_PAL not allowed for this file

// ===========================================================================
// File: CryptApis.h
// 
// CryptoAPI entry points used for StrongName implementation. This file is
// included multiple times with different definitions of the DEFINE_IMPORT
// macro in order handle dynamically finding these entry points.
// ===========================================================================

#ifndef DEFINE_IMPORT
#error Must define DEFINE_IMPORT macro before including this file
#endif

// DEFINE_IMPORT parameters are:
//  1)  Function name (remember to add A to functions that take strings, don't
//      use W versions since they're unsupported on Win9X).
//  2)  Paranthesised argument types (return type is always assumed to be
//      BOOLEAN).
//  3)  TRUE if function is required, FALSE if it is optional (calls will not
//      fail because the function can't be found).

DEFINE_IMPORT(CryptAcquireContextA,     (HCRYPTPROV*, LPCSTR, LPCSTR, DWORD, DWORD),                TRUE)
DEFINE_IMPORT(CryptReleaseContext,      (HCRYPTPROV, DWORD),                                        TRUE)
DEFINE_IMPORT(CryptCreateHash,          (HCRYPTPROV, ALG_ID, HCRYPTKEY, DWORD, HCRYPTHASH*),        TRUE)
DEFINE_IMPORT(CryptDestroyHash,         (HCRYPTHASH),                                               TRUE)
DEFINE_IMPORT(CryptHashData,            (HCRYPTHASH, CONST BYTE*, DWORD, DWORD),                    TRUE)
DEFINE_IMPORT(CryptGetHashParam,        (HCRYPTHASH, DWORD, BYTE*, DWORD*, DWORD),                  TRUE)
DEFINE_IMPORT(CryptImportKey,           (HCRYPTPROV, CONST BYTE*, DWORD, HCRYPTKEY, DWORD, HCRYPTKEY*), TRUE)
DEFINE_IMPORT(CryptExportKey,           (HCRYPTKEY, HCRYPTKEY, DWORD, DWORD, BYTE*, DWORD*),        TRUE)
DEFINE_IMPORT(CryptGenKey,              (HCRYPTPROV, ALG_ID, DWORD, HCRYPTKEY*),                    TRUE)
DEFINE_IMPORT(CryptGetKeyParam,         (HCRYPTKEY, DWORD, BYTE*, DWORD*, DWORD),                   TRUE)
DEFINE_IMPORT(CryptDestroyKey,          (HCRYPTKEY),                                                TRUE)
DEFINE_IMPORT(CryptVerifySignatureA,    (HCRYPTHASH, CONST BYTE*, DWORD, HCRYPTKEY, LPCSTR, DWORD), TRUE)
DEFINE_IMPORT(CryptSignHashA,           (HCRYPTHASH, DWORD, LPCSTR, DWORD, BYTE*, DWORD*),          TRUE)

DEFINE_IMPORT(CryptGetProvParam,        (HCRYPTPROV, DWORD, BYTE*, DWORD*, DWORD),                  TRUE)
DEFINE_IMPORT(CryptGetUserKey,          (HCRYPTPROV, DWORD, HCRYPTKEY*),                            TRUE)
DEFINE_IMPORT(CryptEnumProvidersA,      (DWORD, DWORD*, DWORD, DWORD*, LPSTR, DWORD*),              FALSE)

#undef DEFINE_IMPORT
