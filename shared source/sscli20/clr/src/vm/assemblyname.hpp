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
/*============================================================
**
** Header:  AssemblyName.hpp
**
** Purpose: Implements AssemblyName (loader domain) architecture
**
** Date:  August 10, 1999
**
===========================================================*/
#ifndef _AssemblyName_H
#define _AssemblyName_H

class AssemblyNameNative
{
public:
    static void AssemblyNameInit(ASSEMBLYNAMEREF *pAsmName,
                                 LPCSTR szName,
                                 const void *pbPublicKeyOrToken,
                                 DWORD cbPublicKeyOrToken,
                                 USHORT usMajorVersion,
                                 USHORT usMinorVersion,
                                 USHORT usBuildNumber,
                                 USHORT usRevisionNumber,
                                 LPCSTR szLocale,
                                 LPCWSTR wszCodeBase,
                                 DWORD dwFlags,
                                 DWORD dwHashAlgId);
    
    static FCDECL1(Object*, GetFileInformation, StringObject* filenameUNSAFE);
    static FCDECL1(Object*, ToString, Object* refThisUNSAFE);
    static FCDECL1(Object*, GetPublicKeyToken, Object* refThisUNSAFE);
    static FCDECL1(Object*, EscapeCodeBase, StringObject* filenameUNSAFE);
    static FCDECL4(HRESULT, Init, Object* refThisUNSAFE, OBJECTREF *pAssemblyRef, CLR_BOOL fForIntrospection, CLR_BOOL fRaiseResolveEvent);
    static FCDECL2(FC_BOOL_RET, ReferenceMatchesDefinition, AssemblyNameBaseObject* refUNSAFE, AssemblyNameBaseObject* defUNSAFE);
};

#endif  // _AssemblyName_H

