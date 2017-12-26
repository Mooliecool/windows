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
////////////////////////////////////////////////////////////////////////////////
//
//   File:          COMSecurityConfig.h
//
//   Purpose:       Native implementation for security config access and manipulation
//
//   Date created : August 30, 2000
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _COMSecurityConfig_H_
#define _COMSecurityConfig_H_

class SecurityConfig
{
friend struct CacheHeader;

private:
    enum RegistryExtensionsAccessStatus {
        Unknown         = 0,
        NoExtensions    = 1,
        AccessFailure   = 2,
        AccessSuccess   = 3
    };

    struct RegistryExtensionsInfo {
        RegistryExtensionsAccessStatus  eStatus;
        FILETIME                        ftLastWriteTime;
    };

    static RegistryExtensionsInfo s_registryExtensionsInfo;

public:
    // Duplicated in System.Security.Util.Config.cs
    enum ConfigId
    {
        None                    = 0,
        MachinePolicyLevel      = 1,
        UserPolicyLevel         = 2,
        EnterprisePolicyLevel   = 3,
    };

    // Duplicated in System.Security.Util.Config.cs
    enum QuickCacheEntryType
    {
        FullTrustZoneMyComputer = 0x1000000,
        FullTrustZoneIntranet   = 0x2000000,
        FullTrustZoneInternet   = 0x4000000,
        FullTrustZoneTrusted    = 0x8000000,
        FullTrustZoneUntrusted  = 0x10000000,
        FullTrustAll            = 0x20000000,
    };

    // Duplicated in System.Security.Util.Config.cs
    enum ConfigRetval
    {
        NoFile = 0,
        ConfigFile = 1,
        CacheFile = 2
    };

    static void ReadRegistryExtensionsInfo ();

    static ConfigRetval InitData( INT32 id, const WCHAR* configFileName, const WCHAR* cacheFileName );
    static ConfigRetval InitData( void* configData, BOOL addToList );

    static BOOL SaveCacheData( INT32 id );

    static FCDECL1(void, EcallResetCacheData, INT32 id);
    static void ResetCacheData( INT32 id );

    static FCDECL4(FC_BOOL_RET, EcallSaveDataByte, StringObject* pathUNSAFE, U1Array* dataUNSAFE, DWORD dwOffset, DWORD dwLength);
    static BOOL __stdcall SaveData(__in_z WCHAR* wszConfigPath, LPBYTE pbData, DWORD cbData);

    static FCDECL1(FC_BOOL_RET, EcallRecoverData, INT32 id);
    static BOOL RecoverData( INT32 id );

    static BOOL GetQuickCacheEntry( INT32 id, QuickCacheEntryType type );

    static FCDECL2(void, EcallSetQuickCache, INT32 id, QuickCacheEntryType type);
    static void SetQuickCache( INT32 id, QuickCacheEntryType type );

    static FCDECL4(FC_BOOL_RET, GetCacheEntry, INT32 id, DWORD numEvidence, CHARArray* evidenceUNSAFE, U1ARRAYREF* policy);

    static FCDECL4(void, AddCacheEntry, INT32 id, DWORD numEvidence, CHARArray* evidenceUNSAFE, U1Array* policyUNSAFE);

    static FCDECL0(Object*, EcallGetMachineDirectory);
    static FCDECL0(Object*, EcallGetUserDirectory);

    static HRESULT GetMachineDirectory (__out_ecount(bufferCount) __out_z WCHAR* buffer, size_t bufferCount);
    static BOOL GetUserDirectory(__out_ecount(bufferCount) __out_z WCHAR* buffer, size_t bufferCount);
    static BOOL GetVIUserDirectory(__out_ecount(bufferCount) __out_z WCHAR* buffer, size_t bufferCount);

    static FCDECL1(FC_BOOL_RET, EcallWriteToEventLog, StringObject* messageUNSAFE);
    static BOOL WriteToEventLog( __in_z WCHAR* buffer );

#ifdef _DEBUG
    static FCDECL2(HRESULT, DebugOut, StringObject* fileUNSAFE, StringObject* messageUNSAFE);
#endif

    static void Init( void );
    static void Cleanup( void );
    static void Delete( void );

    static void* GetData( INT32 id );

    static ArrayListStatic  entries_;
    static CrstStatic       dataLock_;

    static WCHAR* wcscatDWORD( __out_ecount(cchdst) __out_z WCHAR* dst, size_t cchdst, DWORD num );
};

#endif
