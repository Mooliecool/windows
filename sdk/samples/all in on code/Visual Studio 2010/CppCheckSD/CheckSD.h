/****************************** Module Header ******************************\
Module Name:  CheckSD.h
Project:      CppCheckSD
Copyright (c) Microsoft Corporation 1997 - 2011.

This module contains the declarations for the functions used in the check_sd
sample.  The declarations are grouped based on the modules they are located 
in.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


#ifndef _CHECKSD_
#define _CHECKSD_

#define STRICT

//
// only works on Windows XP and above
//
#define _WIN32_WINNT 0x0501

#include <windows.h>

//
// access for network shares.  these are based on the values set by
// explorer
//
#define READ        0x000001BF
#define CHANGE      0x000000A9
#define WRITE       0x00000040

//
// helper definitions, Helper.cpp
//
#define UNKNOWNSIDS 4

//
// helper functions, Helper.cpp
//
void DisplayError(DWORD dwError, LPTSTR pszAPI);
BOOL Privilege(LPWSTR pszPrivilege, BOOL bEnable);
void LookupAccountOtherSid(PSID psidCheck, LPWSTR pszName, LPDWORD pcbName, LPWSTR pszDomain, LPDWORD pcbDomain, PSID_NAME_USE psnu);
BOOL Is64(void);

//
// functions for obtaining information from a security descriptor, SD.cpp
//
DWORD DumpAclInfo(PACL pacl, BOOL bDacl);
DWORD DumpControl(PSECURITY_DESCRIPTOR psd);
void  DumpDacl(PSECURITY_DESCRIPTOR psd, WCHAR c, BOOL bDacl, BOOL bDaclPresent);
void  DumpOwnerGroup(PSID psid, BOOL bOwner);
void  DumpSD(PSECURITY_DESCRIPTOR psd, WCHAR c, BOOL bVerbose, BOOL bAudit, PSID pOwner, PSID pGroup, PACL pDacl, PACL pSacl);
void  DumpSDInfo(PSECURITY_DESCRIPTOR psd);
void  PrintAccessRights(ACCESS_ALLOWED_ACE *pace, WCHAR c, BOOL bDacl);

//
// functions for obtaining a security descriptor for a variety of securable objects,
// SecObjects.cpp
//
void DumpObjectWithHandle(LPWSTR pszObject, WCHAR c, BOOL bVerbose, BOOL bAudit);
void DumpObjectWithName(LPWSTR pszObject, WCHAR c, BOOL bVerbose, BOOL bAudit);

#endif // _CHECKSD_
