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

#include "fusionp.h"
#include "ndpversion.h"
#include "helpers.h"
#include "policy.h"
#include "parse.h"
#include "naming.h"

#define NETCF_PUBLIC_KEY_TOKEN_1        L"1c9e259686f921e0"
#define NETCF_PUBLIC_KEY_TOKEN_2        L"5fd57c543a9c0247"
#define NETCF_PUBLIC_KEY_TOKEN_3        L"969db8053d3322ac"
#define SQL_PUBLIC_KEY_TOKEN            L"89845dcd8080cc91"
#define SQL_MOBILE_PUBLIC_KEY_TOKEN     L"3be235df1c8d2ad3"

#define VER_VS_ASSEMBLYVERSION_STR_L    L"8.0.0.0"
#define VER_SQL_ASSEMBLYVERSION_STR_L   L"9.0.242.0"

typedef struct tagRetargetConfig
{
    LPCWSTR pwzName;
//  culture by default is NULL/Neutral
//    LPCWSTR pwzCulture;   
    LPCWSTR pwzPKT;
    LPCWSTR pwzVersion;
    LPCWSTR pwzNewName;
    LPCWSTR pwzNewPKT;
    LPCWSTR pwzNewVersion;
}RetargetConfig;

typedef struct tagFrameworkConfig
{
    LPCWSTR pwzName;
//  culture by default is NULL/Neutral
//    LPCWSTR pwzCulture;
    LPCWSTR pwzPKT;
    LPCWSTR pwzNewVersion;
}FrameworkConfig;

const RetargetConfig g_arRetargetPolicy[] = 
{
// ECMA v1.0 redirect    
    {L"System", ECMA_PUBLICKEY_STR_L, L"1.0.0.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Xml", ECMA_PUBLICKEY_STR_L, L"1.0.0.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
// Compat framework redirect
    {L"System", NETCF_PUBLIC_KEY_TOKEN_1, L"1.0.5000.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5000.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Drawing", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5000.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Web.Services", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5000.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Windows.Forms", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5000.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Xml", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5000.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Drawing", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Web.Services", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Windows.Forms", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Xml", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.VisualBasic", NETCF_PUBLIC_KEY_TOKEN_3, L"7.0.5000.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"System", NETCF_PUBLIC_KEY_TOKEN_1, L"1.0.5500.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5500.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Drawing", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5500.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Web.Services", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5500.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Windows.Forms", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5500.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Xml", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5500.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Drawing", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Web.Services", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Windows.Forms", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Xml", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.VisualBasic", NETCF_PUBLIC_KEY_TOKEN_3, L"7.0.5500.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.WindowsCE.Forms", NETCF_PUBLIC_KEY_TOKEN_1, L"1.0.5000.0", NULL, NETCF_PUBLIC_KEY_TOKEN_3, VER_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.WindowsCE.Forms", NETCF_PUBLIC_KEY_TOKEN_1, L"1.0.5500.0", NULL, NETCF_PUBLIC_KEY_TOKEN_3, VER_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.WindowsCE.Forms", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5000.0", NULL, NETCF_PUBLIC_KEY_TOKEN_3, VER_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.WindowsCE.Forms", NETCF_PUBLIC_KEY_TOKEN_2, L"1.0.5500.0", NULL, NETCF_PUBLIC_KEY_TOKEN_3, VER_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.WindowsCE.Forms", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", NULL, NETCF_PUBLIC_KEY_TOKEN_3, VER_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.WindowsCE.Forms", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", NULL, NETCF_PUBLIC_KEY_TOKEN_3, VER_ASSEMBLYVERSION_STR_L},

// Compat framework name redirect
    {L"System.Data.SqlClient", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", L"System.Data", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data.SqlClient", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", L"System.Data", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data.Common", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", L"System.Data", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data.Common", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", L"System.Data", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Windows.Forms.DataGrid", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5000.0", L"System.Windows.Forms", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Windows.Forms.DataGrid", NETCF_PUBLIC_KEY_TOKEN_3, L"1.0.5500.0", L"System.Windows.Forms", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},

// v2.0 Compact framework redirect
    {L"System", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Xml", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Drawing", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Web.Services", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Windows.Forms", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", NULL, ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Messaging", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data.SqlClient", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", L"System.Data", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data.Common", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", L"System.Data", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Windows.Forms.DataGrid", NETCF_PUBLIC_KEY_TOKEN_3, L"2.0.0.0-2.0.10.0", L"System.Windows.Forms", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.VisualBasic", NETCF_PUBLIC_KEY_TOKEN_3, L"8.0.0.0-8.0.10.0", NULL, MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    
// SQL CE redirect
    {L"System.Data.SqlClient", SQL_MOBILE_PUBLIC_KEY_TOKEN, L"3.0.3600.0", L"System.Data", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data.SqlServerCe", SQL_MOBILE_PUBLIC_KEY_TOKEN, L"3.0.3600.0", NULL, SQL_PUBLIC_KEY_TOKEN, VER_SQL_ASSEMBLYVERSION_STR_L},

};

const FrameworkConfig g_arFxPolicy[] = 
{
    {L"Adodb", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Accessibility", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"cscompmgd", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"CustomMarshalers", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"IEExecRemote", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"IEHost", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"IIEHost", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"ISymWrapper", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.JScript", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.Stdformat", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.VisualBasic", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.VisualBasic.Compatibility", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.VisualBasic.Compatibility.Data", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.VisualBasic.Vsa", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.VisualC", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.Vsa", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft.Vsa.Vb.CodeDOMProcessor", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Microsoft_VsaVb", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"mscorcfg", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"mscorlib", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"Msdatasrc", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"Stdole", MICROSOFT_PUBLICKEY_STR_L, VER_VS_ASSEMBLYVERSION_STR_L},
    {L"System", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Configuration", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Configuration.Install", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data.OracleClient", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Data.SqlXml", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Deployment", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Design", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.DirectoryServices", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.DirectoryServices.Protocols", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Drawing", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Drawing.Design", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.EnterpriseServices", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Management", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Messaging", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Runtime.Remoting", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Runtime.Serialization.Formatters.Soap", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Security", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.ServiceProcess", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Transactions", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Web", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Web.Mobile", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Web.RegularExpressions", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Web.Services", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Windows.Forms", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"System.Xml", ECMA_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"vjscor", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"VJSharpCodeProvider", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"vjsJBC", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"vjslib", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"vjslibcw", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"Vjssupuilib", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"vjsvwaux", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"vjswfc", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"VJSWfcBrowserStubLib", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"vjswfccw", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
    {L"vjswfchtml", MICROSOFT_PUBLICKEY_STR_L, VER_ASSEMBLYVERSION_STR_L},
};

#define FX_TABLE_SIZE                       89

class FrameworkAssemblyTable {
public:
    static HRESULT Create(FrameworkAssemblyTable **ppFrameworkAssemblyTable);
    virtual ~FrameworkAssemblyTable();

    // don't ever free the memory in ppwzFrameworkVersion
    HRESULT IsFrameworkAssembly(LPCWSTR pwzAsmName, LPCWSTR pwzVersion, LPCWSTR pwzCulture, LPCWSTR pwzPublicKeyToken, BOOL *pbIsFrameworkAssembly, LPCWSTR *ppwzFrameworkVersion);
    
    HRESULT GetFrameworkPolicyVersion(LPCWSTR wzAssemblyName, 
                                      LPCWSTR wzVersion,
                                      LPCWSTR wzCulture, 
                                      LPCWSTR wzPublicKeyToken, 
                                      __out_ecount(*pdwSizeVersion) LPWSTR  pwzVersionOut,
                                      __inout LPDWORD pdwSizeVersion);
private:
    HRESULT Init();
    FrameworkAssemblyTable() {};

private:
    List<const FrameworkConfig*>    _hashFXTable[FX_TABLE_SIZE];                                    
};

//
// FrameworkAssemblyTable
//

HRESULT FrameworkAssemblyTable::Create(FrameworkAssemblyTable **ppFrameworkAssemblyTable)
{
    HRESULT                              hr = S_OK;
    FrameworkAssemblyTable               *pFX = NULL;

    _ASSERTE(ppFrameworkAssemblyTable);

    pFX = NEW(FrameworkAssemblyTable);
    if (!pFX) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pFX->Init();
    if (FAILED(hr)) {
        SAFEDELETE(pFX);
        goto Exit;
    }

    *ppFrameworkAssemblyTable = pFX;

Exit:
    return hr;
}    

FrameworkAssemblyTable::~FrameworkAssemblyTable()
{
    for (int i = 0; i < FX_TABLE_SIZE; i++) {
        _hashFXTable[i].RemoveAll();
    }
}

HRESULT FrameworkAssemblyTable::Init()
{
    HRESULT            hr = S_OK;
    DWORD              dwHash;
    unsigned int       i;
    
    for (i = 0; i < sizeof(g_arFxPolicy)/sizeof(g_arFxPolicy[0]); i++) {
        dwHash = HashString(g_arFxPolicy[i].pwzName, 0, FX_TABLE_SIZE, FALSE);
        if (!_hashFXTable[dwHash].AddTail(&(g_arFxPolicy[i]))) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

Exit:
    return hr;
}

// don't ever free the memory in ppwzFrameworkVersion
HRESULT FrameworkAssemblyTable::IsFrameworkAssembly(LPCWSTR pwzAsmName, 
                                                    LPCWSTR pwzVersion,
                                                    LPCWSTR pwzCulture, 
                                                    LPCWSTR pwzPublicKeyToken, 
                                                    BOOL *pbIsFrameworkAssembly,
                                                    LPCWSTR *ppwzFrameworkVersion)
{
    HRESULT               hr = S_OK;
    DWORD                 dwHash;
    const FrameworkConfig *pConfig = NULL;
    LISTNODE              pos;

    _ASSERTE(pwzAsmName);

    // input culture is not neutral
    if (pwzCulture && pwzCulture[0] && FusionCompareStringI(pwzCulture, CFG_CULTURE_NEUTRAL)) {
        *pbIsFrameworkAssembly = FALSE;
        goto Exit;
    }

    // or public key token is not one of the two pre-defined. 
    if (!pwzPublicKeyToken) {
        *pbIsFrameworkAssembly = FALSE;
        goto Exit;
    }

    dwHash = HashString(pwzAsmName, 0, FX_TABLE_SIZE, FALSE);

    pos = _hashFXTable[dwHash].GetHeadPosition();
    while (pos) {
        pConfig = _hashFXTable[dwHash].GetNext(pos);
        _ASSERTE(pConfig);

        // compare name/pkt
        if ( !FusionCompareStringI(pwzAsmName, pConfig->pwzName)
          && !FusionCompareStringI(pwzPublicKeyToken, pConfig->pwzPKT)) {

            // make sure input version is no higher than fx version.
            ULONGLONG ullVer = 0;
            ULONGLONG ullVerFx = 0;
            hr = GetVersionFromString(pwzVersion, &ullVer);
            if (FAILED(hr)) {
                goto Exit;
            }

            hr = GetVersionFromString(pConfig->pwzNewVersion, &ullVerFx);
            if (FAILED(hr)) {
                goto Exit;
            }

            // mask off build/revision number
            ullVer = ullVer >> 32;
            ullVerFx = ullVerFx >> 32;

            if (ullVer <= ullVerFx) {
                *pbIsFrameworkAssembly = TRUE;
                if (ppwzFrameworkVersion) {
                    *ppwzFrameworkVersion = pConfig->pwzNewVersion;
                }
                goto Exit;
            }
        }
    }

    *pbIsFrameworkAssembly = FALSE;

Exit:
    return hr;
}

HRESULT FrameworkAssemblyTable::GetFrameworkPolicyVersion(LPCWSTR wzAssemblyName, 
                                                          LPCWSTR wzVersion,
                                                          LPCWSTR wzCulture, 
                                                          LPCWSTR wzPublicKeyToken, 

                                      __out_ecount(*pdwSizeVer) LPWSTR  pwzVersionOut,
                                      __inout LPDWORD pdwSizeVer)
{
    HRESULT               hr = S_OK;
    LPCWSTR               pVerMatched = NULL;
    DWORD                 dwSize = 0;
    DWORD                 dwHash;
    const FrameworkConfig *pConfig = NULL;
    LISTNODE              pos;
    ULONGLONG             ullVer = 0;
    ULONGLONG             ullVerFx= 0;


    _ASSERTE(wzAssemblyName);
    _ASSERTE(wzVersion);
    _ASSERTE(wzPublicKeyToken);
    _ASSERTE(pwzVersionOut);
    _ASSERTE(pdwSizeVer);
        
    // input culture is not neutral
    if (wzCulture && wzCulture[0] && FusionCompareStringI(wzCulture, CFG_CULTURE_NEUTRAL)) {
        hr = S_FALSE;
        pVerMatched = wzVersion;
    }
    else {
        dwHash = HashString(wzAssemblyName, 0, FX_TABLE_SIZE, FALSE);
        pos = _hashFXTable[dwHash].GetHeadPosition();
        while (pos) {
            pConfig = _hashFXTable[dwHash].GetNext(pos);
            _ASSERTE(pConfig);

            // only compare name/pkt. No compare on version.
            if ( !FusionCompareStringI(wzAssemblyName, pConfig->pwzName)
              && !FusionCompareStringI(wzPublicKeyToken, pConfig->pwzPKT)) {
                pVerMatched = pConfig->pwzNewVersion;
                hr = S_OK;
                break;
            }
        }

        if (!pVerMatched) {
            hr = S_FALSE;
            pVerMatched = wzVersion;
        }
        else {
            // The same version
            if (!FusionCompareStringI(wzVersion, pVerMatched)) {
                hr = S_FALSE;
            }
            else {
                // make sure we don't do downward redirect.
                hr = GetVersionFromString(wzVersion, &ullVer);
                if (FAILED(hr)) {
                    goto Exit;
                }

                hr = GetVersionFromString(pVerMatched, &ullVerFx);
                if (FAILED(hr)) {
                    goto Exit;
                }
                
                // mask off build/revision number
                ullVer = ullVer >> 32;
                ullVerFx = ullVerFx >> 32;

                if (ullVerFx < ullVer) {
                    pVerMatched = wzVersion;
                    hr = S_FALSE;
                }
                else {
                    hr = S_OK;
                }
            }
        }
    }

    dwSize = lstrlenW(pVerMatched) + 1;
    
    if (*pdwSizeVer < dwSize) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else {
        hr = StringCchCopy(pwzVersionOut, *pdwSizeVer, pVerMatched);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (FusionCompareStringI(pwzVersionOut, wzVersion)) {
            // versions are different.
            hr = S_OK;
        }
        else {
            hr = S_FALSE;
        }
    }

    *pdwSizeVer = dwSize;

Exit:
    return hr;
}

FrameworkAssemblyTable *g_pFxTable = NULL;

HRESULT InitializeFrameworkAssemblyTable()
{
    HRESULT hr = S_OK;
    FrameworkAssemblyTable *pTable = NULL;
    
    if (g_pFxTable) {
        return S_OK;
    }

    hr = FrameworkAssemblyTable::Create(&pTable);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (InterlockedCompareExchangePointer((void **)&g_pFxTable, pTable, NULL)) {
        SAFEDELETE(pTable);
    }

Exit:
    return hr;
}

HRESULT GetRetargetPolicyVersion(IAssemblyName *pName,
                                 IAssemblyName **ppNameRetarget)
{
    HRESULT                              hr = S_OK;
    LPWSTR                               pwzAsmName = NULL;
    LPWSTR                               pwzAsmVersion = NULL;
    LPWSTR                               pwzPublicKeyToken = NULL;
    LPWSTR                               pwzCulture = NULL;
    DWORD                                dwSizeName;
    DWORD                                dwSizeVer;
    DWORD                                dwSizePKT;
    DWORD                                dwSizeCulture;
    WCHAR                                wzNameRtg[MAX_PATH];
    WCHAR                                wzVerRtg[MAX_VERSION_DISPLAY_SIZE+1];
    WCHAR                                wzPktRtg[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    BOOL                                 bRetarget = FALSE;
    WORD                                 wVers[4];
    BYTE                                 abProp[PUBLIC_KEY_TOKEN_LEN];
    WCHAR                                wzName[MAX_PATH];
    WCHAR                                wzCulture[MAX_CULTURE_SIZE];
    WCHAR                                wzPublicKeyToken[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    WCHAR                                wzVersion[MAX_VERSION_DISPLAY_SIZE+1];
    IAssemblyName                       *pNameClone = NULL;

    _ASSERTE(pName && ppNameRetarget);
    
    dwSizeName = ARRAYSIZE(wzName);
    dwSizeCulture = ARRAYSIZE(wzCulture);
    dwSizePKT = ARRAYSIZE(wzPublicKeyToken);
    dwSizeVer = ARRAYSIZE(wzVersion);

    wzVersion[0] = L'\0';
    wzName[0] = L'\0';
    wzCulture[0] = L'\0';
    wzPublicKeyToken[0] = L'\0';
    
    hr = PrepQueryMatchData(pName, wzName, &dwSizeName,
                            wzVersion, &dwSizeVer,
                            wzPublicKeyToken, &dwSizePKT,
                            wzCulture, &dwSizeCulture,
                            NULL);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    dwSizeName = ARRAYSIZE(wzNameRtg);
    dwSizeVer = ARRAYSIZE(wzVerRtg);
    dwSizePKT = ARRAYSIZE(wzPktRtg);
    wzNameRtg[0] = L'\0';
    wzVerRtg[0] = L'\0';
    wzPktRtg[0] = L'\0';

    hr = GetRetargetPolicyVersion(wzName, wzVersion, wzCulture, wzPublicKeyToken,
                                  wzNameRtg, &dwSizeName, wzVerRtg, &dwSizeVer,
                                  wzPktRtg, &dwSizePKT);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (hr == S_OK) {
        bRetarget = TRUE;
    }

    hr = pName->Clone(&pNameClone);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!bRetarget) {
        *ppNameRetarget = pNameClone;
        (*ppNameRetarget)->AddRef();
        hr = S_FALSE;
        goto Exit;
    }

    hr = pNameClone->SetProperty(ASM_NAME_NAME, wzNameRtg, (lstrlenW(wzNameRtg) + 1) * sizeof(WCHAR));
    if (FAILED(hr)) {
        goto Exit;
    }
    
    memset(wVers, 0, sizeof(wVers));
    hr = VersionFromString(wzVerRtg, &wVers[0], &wVers[1], &wVers[2], &wVers[3]);
    if (FAILED(hr)) {
        goto Exit;
    }

    for (DWORD i = 0; i < 4; i++) {
        hr = pNameClone->SetProperty(ASM_NAME_MAJOR_VERSION + i, (LPBYTE)(&wVers[i]), sizeof(WORD));
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    
    dwSizePKT = PUBLIC_KEY_TOKEN_LEN;
    CParseUtils::UnicodeHexToBin(wzPktRtg, dwSizePKT * sizeof(WCHAR), abProp);

    hr = pNameClone->SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, abProp, dwSizePKT);
    if (FAILED(hr)) {
        goto Exit;
    }

    *ppNameRetarget = pNameClone;
    (*ppNameRetarget)->AddRef();
    
Exit:
    SAFEDELETEARRAY(pwzAsmName);
    SAFEDELETEARRAY(pwzAsmVersion);
    SAFEDELETEARRAY(pwzPublicKeyToken);
    SAFEDELETEARRAY(pwzCulture);

    SAFERELEASE(pNameClone);

    return hr;
}

HRESULT GetRetargetPolicyVersion(LPCWSTR wzAssemblyName, 
                                 LPCWSTR wzVersion, 
                                 LPCWSTR wzCulture, 
                                 LPCWSTR wzPublicKeyToken, 
                                 __out_ecount(*pdwSizeName) LPWSTR  pwzAssemblyNameOut,
                                 __inout LPDWORD pdwSizeName,
                                 __out_ecount(*pdwSizeVer) LPWSTR  pwzVersionOut,
                                 __inout LPDWORD pdwSizeVer,
                                 __out_ecount(*pdwSizePKT) LPWSTR  pwzPublicKeyTokenOut, 
                                 __inout LPDWORD pdwSizePKT
                                 )
{
    HRESULT hr = S_OK;
    BOOL    bMatched = FALSE;
    LPCWSTR pwzNameMatched = NULL;
    LPCWSTR pwzVerMatched = NULL;
    LPCWSTR pwzPKTMatched = NULL;
    DWORD   dwSize = 0;

    _ASSERTE(wzAssemblyName);
    _ASSERTE(wzVersion);
    _ASSERTE(wzPublicKeyToken);
    _ASSERTE(pwzVersionOut);
    _ASSERTE(pdwSizeVer);
    _ASSERTE(pwzAssemblyNameOut);
    _ASSERTE(pdwSizeName);
    _ASSERTE(pwzPublicKeyTokenOut);
    _ASSERTE(pdwSizePKT);

    pwzNameMatched = wzAssemblyName;
    pwzVerMatched = wzVersion;
    pwzPKTMatched = wzPublicKeyToken;

    // process only if input culture is neutral
    if (!wzCulture || !wzCulture[0] || !FusionCompareStringI(wzCulture, CFG_CULTURE_NEUTRAL)) {
        for (DWORD i = 0; i < ARRAYSIZE(g_arRetargetPolicy) && !bMatched; i++) {
            if ( !FusionCompareStringI(wzAssemblyName, g_arRetargetPolicy[i].pwzName)
              && !FusionCompareStringI(wzPublicKeyToken, g_arRetargetPolicy[i].pwzPKT)
              && IsMatchingVersion(g_arRetargetPolicy[i].pwzVersion, wzVersion) == S_OK) {
                bMatched = TRUE;
                pwzNameMatched = g_arRetargetPolicy[i].pwzNewName ? g_arRetargetPolicy[i].pwzNewName : wzAssemblyName;
                pwzPKTMatched = g_arRetargetPolicy[i].pwzNewPKT;
                pwzVerMatched = g_arRetargetPolicy[i].pwzNewVersion;
                break;
            }  
        }
    }

    dwSize = lstrlenW(pwzVerMatched) + 1;
    if (*pdwSizeVer < dwSize) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else {
        hr = StringCchCopy(pwzVersionOut, *pdwSizeVer, pwzVerMatched);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    *pdwSizeVer = dwSize;

    dwSize = lstrlenW(pwzNameMatched) + 1;
    if (*pdwSizeName < dwSize) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else {
        hr = StringCchCopy(pwzAssemblyNameOut, *pdwSizeName, pwzNameMatched);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    *pdwSizeName = dwSize;

    dwSize = lstrlenW(pwzPKTMatched) + 1;
    if (*pdwSizePKT < dwSize) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else {
        hr = StringCchCopy(pwzPublicKeyTokenOut, *pdwSizePKT, pwzPKTMatched);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    *pdwSizePKT = dwSize;

Exit:
    if (SUCCEEDED(hr) && !bMatched) {
        hr = S_FALSE;
    }

    return hr;
}

// don't ever free the memory in ppwzFrameworkVersion
HRESULT IsFrameworkAssembly(LPCWSTR pwzAsmName, 
                            LPCWSTR pwzVersion,
                            LPCWSTR pwzCulture, 
                            LPCWSTR pwzPublicKeyToken, 
                            BOOL *pbIsFrameworkAssembly,
                            LPCWSTR *ppwzFrameworkVersion)
{
    HRESULT hr = S_OK;

    hr = InitializeFrameworkAssemblyTable();
    if (SUCCEEDED(hr)) {
        hr = g_pFxTable->IsFrameworkAssembly(pwzAsmName, pwzVersion, pwzCulture, pwzPublicKeyToken, pbIsFrameworkAssembly, ppwzFrameworkVersion);
    }

    return hr;
}


HRESULT IsRetargetableAssembly(IAssemblyName *pName, BOOL *pbIsRetargetable)
{
    HRESULT                              hr = S_OK;
    WCHAR                                wzAsmName[MAX_PATH];
    DWORD                                dwSizeName;
    WCHAR                                wzAsmVersion[MAX_VERSION_DISPLAY_SIZE+1];
    DWORD                                dwSizeVer;
    WCHAR                                wzPublicKeyToken[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    DWORD                                dwSizePKT;
    WCHAR                                wzCulture[MAX_CULTURE_SIZE];
    DWORD                                dwSizeCulture;
    WCHAR                                wzVersion[MAX_VERSION_DISPLAY_SIZE + 1];
    DWORD                                dwSize;
    WORD                                 wVerMajor, wVerMinor, wVerBuild, wVerRev;

    _ASSERTE(pName && pbIsRetargetable);

    *pbIsRetargetable = FALSE;
    
    dwSizeName = ARRAYSIZE(wzAsmName);
    dwSizeVer = ARRAYSIZE(wzAsmVersion);
    dwSizePKT = ARRAYSIZE(wzPublicKeyToken);
    dwSizeCulture = ARRAYSIZE(wzCulture);

    wzAsmName[0] = L'\0';
    wzAsmVersion[0] = L'\0';
    wzPublicKeyToken[0] = L'\0';
    wzCulture[0] = L'\0';
    wzVersion[0] = L'\0';

    hr = PrepQueryMatchData(pName, wzAsmName, &dwSizeName, 
                            wzAsmVersion, &dwSizeVer, 
                            wzPublicKeyToken, &dwSizePKT, 
                            wzCulture, &dwSizeCulture, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (wzCulture && wzCulture[0] && FusionCompareStringI(wzCulture, CFG_CULTURE_NEUTRAL)) {
        goto Exit;
    }

    dwSize = sizeof(WORD);
    hr = pName->GetProperty(ASM_NAME_MAJOR_VERSION, &wVerMajor, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    dwSize = sizeof(WORD);
    hr = pName->GetProperty(ASM_NAME_MINOR_VERSION, &wVerMinor, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    dwSize = sizeof(WORD);
    hr = pName->GetProperty(ASM_NAME_BUILD_NUMBER, &wVerBuild, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    dwSize = sizeof(WORD);
    hr = pName->GetProperty(ASM_NAME_REVISION_NUMBER, &wVerRev, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchPrintf(wzVersion, MAX_VERSION_DISPLAY_SIZE, L"%d.%d.%d.%d", wVerMajor, wVerMinor, wVerBuild, wVerRev);
    if (FAILED(hr)) {
        goto Exit;
    }

    
    for (DWORD i = 0; i < ARRAYSIZE(g_arRetargetPolicy); i++) {
        if ( !FusionCompareStringI(wzAsmName, g_arRetargetPolicy[i].pwzName)
          && !FusionCompareStringI(wzPublicKeyToken, g_arRetargetPolicy[i].pwzPKT)
          && IsMatchingVersion(g_arRetargetPolicy[i].pwzVersion, wzVersion) == S_OK) {

            *pbIsRetargetable = TRUE;
            break;
        }  
    }

Exit:
    return hr;
}

// don't ever free the memory in ppwzFrameworkVersion
HRESULT IsFrameworkAssembly(IAssemblyName *pName, BOOL *pbIsFrameworkAssembly, LPCWSTR *ppwzFrameworkVersion)
{
    HRESULT hr = S_OK;
    WCHAR   wzName[MAX_PATH];
    WCHAR   wzVersion[MAX_VERSION_DISPLAY_SIZE+1];
    DWORD   dwSizeVer;
    DWORD   dwSizeName;
    WCHAR   wzCulture[MAX_CULTURE_SIZE];
    DWORD   dwSizeCulture;
    WCHAR   wzPublicKeyToken[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    DWORD   dwSizePKT;

    dwSizeName = ARRAYSIZE(wzName);
    dwSizeVer = ARRAYSIZE(wzVersion);
    dwSizeCulture = ARRAYSIZE(wzCulture);
    dwSizePKT = ARRAYSIZE(wzPublicKeyToken);
    wzName[0] = L'\0';
    wzVersion[0] = L'\0';
    wzCulture[0] = L'\0';
    wzPublicKeyToken[0] = L'\0';

    hr = PrepQueryMatchData(pName, wzName, &dwSizeName, wzVersion, &dwSizeVer, wzPublicKeyToken, &dwSizePKT, NULL, NULL, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = IsFrameworkAssembly(wzName, wzVersion, wzCulture, wzPublicKeyToken, pbIsFrameworkAssembly, ppwzFrameworkVersion);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    return hr;
}

STDAPI IsFrameworkAssembly(
        LPCWSTR pwzAssemblyReference, 
        LPBOOL pbIsFrameworkAssembly, 
        __out_ecount_opt(*pdwSize) LPWSTR pwzFrameworkAssemblyIdentity, 
        __inout_opt LPDWORD pdwSize)
{
    HRESULT hr = S_OK;
    IAssemblyName *pName = NULL;
    DWORD   dwSize;
    LPCWSTR pwzFrameworkVersion = NULL;
    DWORD dwDispFlags =  ASM_DISPLAYF_CULTURE
            | ASM_DISPLAYF_PUBLIC_KEY_TOKEN
            | ASM_DISPLAYF_VERSION;
    WORD    wVers[4];


    if (!pwzAssemblyReference || !pbIsFrameworkAssembly) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = CreateAssemblyNameObject(&pName, pwzAssemblyReference, CANOF_PARSE_DISPLAY_NAME, NULL);
    if (FAILED(hr)) {
        if (hr == FUSION_E_INVALID_NAME) {
            hr = E_INVALIDARG;
        }
        goto Exit;
    }

    dwSize = 0;
    pName->GetProperty(ASM_NAME_ARCHITECTURE, NULL, &dwSize);

    if (dwSize) {
        // input has processor architecture. Do not allow it. 
        hr = E_INVALIDARG;
        goto Exit;
    }

    dwSize = 0;
    pName->GetProperty(ASM_NAME_CULTURE, NULL, &dwSize);
    if (!dwSize) {
        // input has no culture
        hr = E_INVALIDARG;
        goto Exit;
    }

    dwSize = 0;
    pName->GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, NULL, &dwSize);
    if (!dwSize) {
        // input has no public key token
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = IsFrameworkAssembly(pName, pbIsFrameworkAssembly, &pwzFrameworkVersion);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (*pbIsFrameworkAssembly && pdwSize) {
        // want identity output as well. 
        hr = VersionFromString(pwzFrameworkVersion, &wVers[0], &wVers[1], &wVers[2], &wVers[3]);
        if (FAILED(hr)) {
            goto Exit;
        }

        for (DWORD i = 0; i < 4; i++) {
            hr = pName->SetProperty(ASM_NAME_MAJOR_VERSION + i, (LPBYTE)(&wVers[i]), sizeof(WORD));
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        hr = pName->GetDisplayName(pwzFrameworkAssemblyIdentity, pdwSize, dwDispFlags);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    return hr;
}

HRESULT GetFrameworkPolicyVersion(LPCWSTR wzAssemblyName, 
                                  LPCWSTR wzVersion,
                                  LPCWSTR wzCulture, 
                                  LPCWSTR wzPublicKeyToken, 
                                  __out_ecount(*pdwSizeVer) LPWSTR  pwzVersionOut,
                                  __inout LPDWORD pdwSizeVer)
{
    HRESULT hr = S_OK;

    hr = InitializeFrameworkAssemblyTable();
    if (SUCCEEDED(hr)) {
        hr = g_pFxTable->GetFrameworkPolicyVersion(wzAssemblyName, 
                                                wzVersion,
                                                wzCulture,
                                                wzPublicKeyToken,
                                                pwzVersionOut,
                                                pdwSizeVer);
    }

    return hr;
}

#define CAI_NAME_MATCHES            2000
#define CAI_CULTURE_MATCHES          200
#define CAI_CULTURE_PARTIAL          100
#define CAI_PKT_MATCHES               20
#define CAI_PKT_PARTIAL               10
#define CAI_VERSION_UNIFIES            3
#define CAI_VERSION_MATCHES            2
#define CAI_VERSION_PARTIAL            1

HRESULT ComputeCompareState(IAssemblyName *pName1, BOOL fUnified1,
                            IAssemblyName *pName2, BOOL fUnified2,
                            DWORD dwCmpFlags1,
                            DWORD *pdwComparison)
{
    HRESULT                                     hr = S_OK;

    *pdwComparison = 0;

    // Examine name
    
    hr = pName1->IsEqual(pName2, ASM_CMPF_NAME);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (hr == S_OK) {
        *pdwComparison += CAI_NAME_MATCHES;
    }

    // Examine culture

    if (dwCmpFlags1 & ASM_CMPF_CULTURE) {
        hr = pName1->IsEqual(pName2, ASM_CMPF_CULTURE);
        if (FAILED(hr)) {
            goto Exit;
        }
        
        if (hr == S_OK) {
            *pdwComparison += CAI_CULTURE_MATCHES;
        }
    }
    else {
        *pdwComparison += CAI_CULTURE_PARTIAL;
    }

    // Examine public key token
    
    if (dwCmpFlags1 & ASM_CMPF_PUBLIC_KEY_TOKEN) {
        hr = pName1->IsEqual(pName2, ASM_CMPF_PUBLIC_KEY_TOKEN);
        if (FAILED(hr)) {
            goto Exit;
        }
        
        if (hr == S_OK) {
            *pdwComparison += CAI_PKT_MATCHES;
        }
    }
    else {
        *pdwComparison += CAI_PKT_PARTIAL;
    }

    // Examine version

    if (!(dwCmpFlags1 & (ASM_CMPF_VERSION))) {
        *pdwComparison += CAI_VERSION_PARTIAL;
    }
    else {
        ULONGLONG                ullVersion1;
        ULONGLONG                ullVersion2;

        hr = CAssemblyName::GetVersion(pName1, TRUE, &ullVersion1);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CAssemblyName::GetVersion(pName2, TRUE, &ullVersion2);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (ullVersion1 == ullVersion2) {
            *pdwComparison += CAI_VERSION_MATCHES;
        }
        else if ((fUnified1 && ullVersion1 > ullVersion2) ||
                 (fUnified2 && ullVersion1 < ullVersion2)) {
            *pdwComparison += CAI_VERSION_UNIFIES;
        }
    }

    hr = S_OK;

Exit:
    return hr;
}                            

#define ASSEMBLY_IDENTITY_CACHE_SIZE 197
HRESULT CompareAssemblyIdentity(LPCWSTR pwzAssemblyIdentity1,
                                BOOL    fUnified1,
                                LPCWSTR pwzAssemblyIdentity2,
                                BOOL    fUnified2,
                                BOOL   *pfEquivalent,
                                AssemblyComparisonResult *pResult)
{
    HRESULT                            hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    IAssemblyName                     *pName1 = NULL;
    IAssemblyName                     *pName2 = NULL;
    IAssemblyName                     *pNameRetarget = NULL;
    BOOL                               fStronglyNamed1;
    BOOL                               fStronglyNamed2;
    BOOL                               fPartial1;
    BOOL                               fIsFXAssembly;
    BOOL                               fIsRetargetable1 = FALSE;
    BOOL                               fIsRetargetable2 = FALSE;
    DWORD                              cbSize;
    DWORD                              dwCmpFlags1;
    DWORD                              dwComparison;
    static CAssemblyIdentityCache *pCache = NULL;

    if (!pwzAssemblyIdentity1 || !pwzAssemblyIdentity2 || !pfEquivalent || !pResult) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *pfEquivalent = FALSE;
    *pResult = ACR_NonEquivalent;

    if (!pCache) {
        CAssemblyIdentityCache *pCacheTmp = NULL;
        hr = CAssemblyIdentityCache::Create(&pCacheTmp, ASSEMBLY_IDENTITY_CACHE_SIZE);
        if (FAILED(hr))
            goto Exit;
        if (InterlockedCompareExchangePointer((void **)&pCache, pCacheTmp, NULL)) {
            SAFEDELETE(pCacheTmp);
        }
    }

    hr = pCache->GetNameObject(&pName1, pwzAssemblyIdentity1, CANOF_PARSE_DISPLAY_NAME, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }

    fPartial1 = CAssemblyName::IsPartial(pName1, &dwCmpFlags1);
    if (fPartial1 && fUnified1) {
        // Partially-qualified names with unification is disallowed.
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = pCache->GetNameObject(&pName2, pwzAssemblyIdentity2, CANOF_PARSE_DISPLAY_NAME, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    if (CAssemblyName::IsPartial(pName2, NULL)) {
        // Partially-qualified names disallowed.
        hr = E_INVALIDARG;
        goto Exit;
    }

    // mscorlib is a special case
    hr = pName1->IsEqual(pName2, ASM_CMPF_NAME);
    if (FAILED(hr)) {
        goto Exit;
    }
    if (hr == S_OK) {
        // If both simple names are the same and they're both equal
        // to mscorlib, then the runtime considers them identical
        WCHAR                            wzName[MAX_PATH];
        DWORD                            dwSizeName;

        dwSizeName = ARRAYSIZE(wzName);
        hr = pName1->GetName(&dwSizeName, wzName);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (dwSizeName == 9 && !FusionCompareStringI(wzName, L"mscorlib")) {
            // As far as the runtime is concerned, this is the one and only mscorlib
            *pfEquivalent = TRUE;
            *pResult = ACR_EquivalentFullMatch;
            hr = S_OK;
            goto Exit;
        }
    }

    fStronglyNamed1 = CAssemblyName::IsStronglyNamed(pName1);
    fStronglyNamed2 = CAssemblyName::IsStronglyNamed(pName2);

    // Handle retargeting cases
    
    cbSize = sizeof(BOOL);
    hr = pName1->GetProperty(ASM_NAME_RETARGET, &fIsRetargetable1, &cbSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!cbSize) {
        fIsRetargetable1 = FALSE;
    }

    cbSize = sizeof(BOOL);
    hr = pName2->GetProperty(ASM_NAME_RETARGET, &fIsRetargetable2, &cbSize);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    if (!cbSize) {
        fIsRetargetable2 = FALSE;
    }

    if (!fIsRetargetable1 && fIsRetargetable2) {
        // Ref is not retarggettable, but def is retargetable.
        // Non-equivalent.
        goto Exit;
    }
    else if (fIsRetargetable1 && !fIsRetargetable2) {
        if (fPartial1) {
            *pResult = ACR_Unknown;
            goto Exit;
        }

        // Ref needs to be retargeted before comparison
        
        hr = GetRetargetPolicyVersion(pName1, &pNameRetarget);
        if (FAILED(hr)) {
            goto Exit;
        }
        else if (hr == S_FALSE) {
            // Should be in retarget table
            
            *pResult = ACR_Unknown;
            hr = S_OK;
            goto Exit;
        }

        SAFERELEASE(pName1);
        pName1 = pNameRetarget;
        pName1->AddRef();
    }
    
    // At this point we are in one of the following states:
    //
    //   1) Both ref/def are not retargetable
    //   2) Both ref/def are retargetable
    //   3) Ref is retargetable (and has been retargeted)
    //
    // We can do a straight compare of ref/def at this point using the
    // regular rules

    if (fIsRetargetable1 && fIsRetargetable2) {
        hr = IsRetargetableAssembly(pName2, &fIsFXAssembly);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        hr = IsFrameworkAssembly(pName2, &fIsFXAssembly);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (!fStronglyNamed2) {
        if (fStronglyNamed1) {
            // Definitely doesn't match.
            
            *pfEquivalent = FALSE;
            *pResult = ACR_NonEquivalent;
        }
        else if (!fPartial1) {
            // Both simply-named, fully-specified
            
            hr = pName1->IsEqual(pName2, ASM_CMPF_DEFAULT);
            if (FAILED(hr)) {
                goto Exit;
            }
    
            if (hr == S_FALSE) {
                // Not equivalent. Reset HRESULT.
                hr = S_OK;
                goto Exit;
            }
    
            *pfEquivalent = TRUE;
            *pResult = ACR_EquivalentWeakNamed;
        }
        else {
            // Partial name. Name is set, and PKT==null.

            // Culture is the only remaining attribute that
            // is relevant. 

            hr = pName1->IsEqual(pName2, ASM_CMPF_NAME);
            if (FAILED(hr)) {
                goto Exit;
            }

            if (hr == S_OK) {
                if (dwCmpFlags1 & ASM_CMPF_CULTURE) {
                    // Culture set. Compare it.

                    hr = pName1->IsEqual(pName2, ASM_CMPF_CULTURE);
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    if (hr == S_OK) {
                        *pfEquivalent = TRUE;
                        *pResult = ACR_EquivalentWeakNamed;
                    }
                }
                else {
                    // Culture not set. Version is irrelevant.

                    *pfEquivalent = TRUE;
                    *pResult = ACR_EquivalentPartialWeakNamed;
                }

            }

            hr = S_OK;
        }

        goto Exit;
    }

    hr = ComputeCompareState(pName1, fUnified1, pName2, fUnified2,
                             dwCmpFlags1, &dwComparison);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Order of comparison mask: Name, culture, pkt, version
    
    if (fIsFXAssembly) {
        switch (dwComparison) {
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_MATCHES + CAI_VERSION_MATCHES: // 2222
                *pfEquivalent = TRUE;
                *pResult = ACR_EquivalentFullMatch;
                break;
            
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_MATCHES: // 2220
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_MATCHES + CAI_VERSION_PARTIAL: // 2221
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_MATCHES + CAI_VERSION_UNIFIES: // 2223
                *pfEquivalent = TRUE;
                *pResult = ACR_EquivalentFXUnified;
                break;

            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_MATCHES + CAI_VERSION_PARTIAL:  // 2121
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_MATCHES + CAI_VERSION_UNIFIES:  // 2123
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_MATCHES: //2120
                *pfEquivalent = TRUE;
                *pResult = ACR_EquivalentPartialFXUnified;
                break;

            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_PARTIAL: // 2210
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_PARTIAL + CAI_VERSION_PARTIAL: // 2211
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_PARTIAL + CAI_VERSION_MATCHES: // 2212
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_PARTIAL + CAI_VERSION_UNIFIES: // 2213
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_PARTIAL: // 2110
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_PARTIAL + CAI_VERSION_PARTIAL: // 2111
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_PARTIAL + CAI_VERSION_MATCHES: // 2112
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_PARTIAL + CAI_VERSION_UNIFIES: // 2113
                *pResult = ACR_Unknown;
                break;
                
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_MATCHES + CAI_VERSION_MATCHES: // 2122
                *pfEquivalent = TRUE;
                *pResult = ACR_EquivalentPartialMatch;
                break;
        }
        
    }
    else {
        switch (dwComparison) {
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_MATCHES + CAI_VERSION_MATCHES: // 2222
                *pfEquivalent = TRUE;
                *pResult = ACR_EquivalentFullMatch;
                break;

            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_MATCHES + CAI_VERSION_UNIFIES: // 2223
                *pfEquivalent = TRUE;
                *pResult = ACR_EquivalentUnified;
                break;

            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_MATCHES + CAI_VERSION_PARTIAL: // 2221
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_PARTIAL + CAI_VERSION_MATCHES: // 2212
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_MATCHES + CAI_VERSION_MATCHES: // 2122
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_MATCHES + CAI_VERSION_PARTIAL: // 2121
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_PARTIAL + CAI_VERSION_PARTIAL: // 2111
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_PARTIAL + CAI_VERSION_MATCHES: // 2112
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_PARTIAL + CAI_VERSION_PARTIAL: // 2211
                *pfEquivalent = TRUE;
                *pResult = ACR_EquivalentPartialMatch;
                break;

            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_MATCHES: // 2220
                *pResult = ACR_NonEquivalentVersion;
                break;
            
            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_PARTIAL + CAI_VERSION_UNIFIES: // 2213
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_MATCHES + CAI_VERSION_UNIFIES: // 2123
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_PARTIAL + CAI_VERSION_UNIFIES: // 2113
                *pfEquivalent = TRUE;
                *pResult = ACR_EquivalentPartialUnified;
                break;

            case CAI_NAME_MATCHES + CAI_CULTURE_MATCHES + CAI_PKT_PARTIAL: // 2210
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_MATCHES: // 2120
            case CAI_NAME_MATCHES + CAI_CULTURE_PARTIAL + CAI_PKT_PARTIAL: // 2110
                *pResult = ACR_NonEquivalentPartialVersion;
                break;
        }
    }


Exit:
    SAFERELEASE(pName1);
    SAFERELEASE(pName2);
    SAFERELEASE(pNameRetarget);
    END_ENTRYPOINT_NOTHROW;

    return hr;
}
