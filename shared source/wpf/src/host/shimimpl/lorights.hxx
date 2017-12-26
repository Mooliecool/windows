//------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//     Implements the minimal set of interfaces required for
//     the version-independent hosting shim.
//
// History:
//      2005/05/09 - [....]
//          Created
//      2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once

BOOL ShouldProcessBeRestricted(void);
BOOL LaunchRestrictedProcess(__in LPCWSTR lpwszCmdLine, __in_ecount(dwDisabledPrivilegeCount) PLUID_AND_ATTRIBUTES pDisabledPrivileges, DWORD dwDisabledPrivilegeCount);
BOOL IsCurrentProcessRestricted(__in_ecount(dwDisabledPrivilegesCount) PLUID_AND_ATTRIBUTES ppDisabledPrivileges, DWORD dwDisabledPrivilegesCount);

const WELL_KNOWN_SID_TYPE g_disableSIDS[] = { WinBuiltinAdministratorsSid, 
                                              WinBuiltinPowerUsersSid };

const LPCWSTR g_ppwszPrivileges[18] = { SE_ASSIGNPRIMARYTOKEN_NAME,
                                        SE_BACKUP_NAME,
                                        SE_CREATE_GLOBAL_NAME,
                                        SE_CREATE_PERMANENT_NAME,
                                        SE_CREATE_TOKEN_NAME,
                                        SE_ENABLE_DELEGATION_NAME,
                                        SE_LOAD_DRIVER_NAME,
                                        SE_MACHINE_ACCOUNT_NAME,
                                        SE_REMOTE_SHUTDOWN_NAME,
                                        SE_RESTORE_NAME,
                                        SE_SECURITY_NAME,
                                        SE_SYSTEMTIME_NAME,
                                        SE_TAKE_OWNERSHIP_NAME,
                                        SE_TCB_NAME,
                                        SE_IMPERSONATE_NAME,
                                        SE_AUDIT_NAME,
                                        SE_DEBUG_NAME,
                                        SE_CREATE_PAGEFILE_NAME
                                        };

BOOL IsCurrentPresentationHostRestricted(__in PSID_AND_ATTRIBUTES pDisabledSids,
                                         __in DWORD dwDisabledSidCount,
                                         __in PLUID_AND_ATTRIBUTES pDisabledPrivileges,
                                         __in DWORD dwDisabledPrivilegeCount);

DWORD CreateRestrictedProcess(__in PSID_AND_ATTRIBUTES pDisabledSids,
                              __in DWORD dwDisabledSidCount,
                              __in PLUID_AND_ATTRIBUTES pDisabledPrivileges,
                              __in DWORD dwDisabledPrivilegeCount,
                              __in LPCWSTR lpwszCmdLine);

// Call FreeSIDArray() when done with the returned data.
DWORD GetDisabledSids(__out PSID_AND_ATTRIBUTES *ppDisabledSids, 
                      __out DWORD *pdwDisabledSidCount);


DWORD GetDisabledPrivileges(__out PLUID_AND_ATTRIBUTES *ppDisabledPrivileges,
                            __out DWORD *pdwDisabledPrivlegeCount);

DWORD GetSid(__in WELL_KNOWN_SID_TYPE sidType,
             __out PSID *ppSid,
             __inout DWORD *pdwSidSize);

DWORD GetUserSid(__in HANDLE hProcToken,
                 __out_bcount(SECURITY_MAX_SID_SIZE) PSID pSid);

BOOL CheckForDisabledSids(__in PSID_AND_ATTRIBUTES pSids,
                          __in DWORD dwSidCount,
                          __in PSID_AND_ATTRIBUTES pDisabledSids,
                          __in DWORD dwDisabledSidCount);

BOOL CheckForRestrictedSids(__in PSID_AND_ATTRIBUTES pSids,
                            __in DWORD dwSidCount,
                            __in PSID_AND_ATTRIBUTES pRestrictedSids,
                            __in DWORD dwRestrictedSidCount, 
                            __in PSID pUserSid);

BOOL CheckForPrivileges(__in PLUID_AND_ATTRIBUTES pPrivileges,
                        __in DWORD dwPrivilegeCount,
                        __in PLUID_AND_ATTRIBUTES pDisabledPrivileges,
                        __in DWORD dwDisabledPrivilegeCount);

DWORD AddSidsToToken(__in HANDLE hRestrictedToken,
                     __in PSID *ppSids,
                     __in DWORD dwSids,
                     __in DWORD dwAccess);

BOOL  SetSidsOnAcl(__in PSID *ppSids,
                   __in DWORD dwSids,
                   __in PACL pAclSource,
                   __out PACL *pAclDestination,
                   __out DWORD *pcbDacl,
                   __in DWORD AccessMask,
                   __in BYTE AceFlags);

#if DEBUG

void GetAndPrintTokenInfo(__in FILE *fLog, __in HANDLE hToken);
void GetAndPrintTokenDefaultDacl(__in FILE *fLog, __in HANDLE hToken);

void PrintTokenInfo(__in FILE *fLog, __in PTOKEN_GROUPS_AND_PRIVILEGES pTokenGroups);
void PrintDACL(__in FILE *fLog, __in PACL pAcl);
void PrintSid(__in FILE *fLog, __in PSID pSid, __in LPCWSTR lpwszData, __in DWORD dwIndex);
void PrintPrivileges(__in FILE *fLog, __in PLUID_AND_ATTRIBUTES pTokenPrivileges, __in DWORD dwPrivilegeCount);

#endif


DWORD CreateRestrictedProcess(__in PSID_AND_ATTRIBUTES pDisabledSids,
                              __in DWORD dwDisabledSidCount,
                              __in PLUID_AND_ATTRIBUTES pDisabledPrivileges,
                              __in DWORD dwDisabledPrivilegeCount,
                              __in LPCWSTR lpwszCmdLine);

BOOL IsPresentationHostHighIntegrity();
