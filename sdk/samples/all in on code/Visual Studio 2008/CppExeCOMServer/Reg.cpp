/****************************** Module Header ******************************\
Module Name:  Reg.cpp
Project:      CppExeCOMServer
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "Reg.h"
#include <strsafe.h>


//
//   FUNCTION: SetHKCRRegistryKeyAndValue
//
//   PURPOSE: The function creates a HKCR registry key and sets the specified 
//   registry value.
//
//   PARAMETERS:
//   * pszSubKey - specifies the registry key under HKCR. If the key does not 
//     exist, the function will create the registry key.
//   * pszValueName - specifies the registry value to be set. If pszValueName 
//     is NULL, the function will set the default value.
//   * pszData - specifies the string data of the registry value.
// 
HRESULT SetHKCRRegistryKeyAndValue(PCWSTR pszSubKey,
                                   PCWSTR pszValueName, 
                                   PCWSTR pszData)
{
    HRESULT hr;
    HKEY hKey = NULL;

    // Creates the specified registry key. If the key already exists, the 
    // function opens it. 
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, pszSubKey, 0, 
        NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));

    // Set the value of the key.
    if (SUCCEEDED(hr))
    {
        DWORD cbData = (pszData == NULL) ? 0 : (lstrlen(pszData) * sizeof(*pszData));
        hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, pszValueName, 0, REG_SZ, 
            (LPBYTE)pszData, cbData));
        
        RegCloseKey(hKey);
    }

    return hr;
}


//
//   FUNCTION: RegisterLocalServer
//
//   PURPOSE: This helper function registers the out-of-process component in 
//   the registry.
//
//   PARAMETERS:
//   * pszModule - Path of the module that contains the component
//   * clsid - Class ID of the component
//   * pszFriendlyName - Friendly name
//   * pszThreadModel - Threading model
//   * libid - Type library ID
//   * pszProgID - ProgID of the component
//   * pszVerIndProgID - Version independent ProgID
//
//   NOTE: The function creates the HKCR\CLSID\{<CLSID>} key and the 
//   HKCR\<ProgID> key in the registry.
// 
//   HKCR
//   {
//      <ProgID> = s '<Friendly Name>'
//      {
//          CLSID = s '{<CLSID>}'
//      }
//      <VersionIndependentProgID> = s '<Friendly Name>'
//      {
//          CLSID = s '{<CLSID>}'
//          CurVer = s '<ProgID>'
//      }
//      NoRemove CLSID
//      {
//          ForceRemove {<CLSID>} = s '<Friendly Name>'
//          {
//              ProgID = s '<ProgID>'
//              VersionIndependentProgID = s '<VersionIndependentProgID>'
//              LocalServer32 = s '%MODULE%'
//              TypeLib = s '{<LIBID>}'
//          }
//      }
//   }
//
HRESULT RegisterLocalServer(PCWSTR pszModule, 
                           const CLSID& clsid, 
                           PCWSTR pszFriendlyName,
                           const IID& libid,
                           PCWSTR pszProgID, 
                           PCWSTR pszVerIndProgID)
{
    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szLIBID[MAX_PATH];
    StringFromGUID2(libid, szLIBID, ARRAYSIZE(szLIBID));

    wchar_t szSubkey[MAX_PATH];

    // Create the HKCR\CLSID\{<CLSID>} key.
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (SUCCEEDED(hr))
    {
        hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszFriendlyName);

        // Create the HKCR\CLSID\{<CLSID>}\ProgID key with the default value 
        // '<ProgID>'
        if (SUCCEEDED(hr) && pszProgID != NULL)
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey),
                L"CLSID\\%s\\ProgID", szCLSID);
            if (SUCCEEDED(hr))
            {
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszProgID);
            }
        }

        // Create the HKCR\CLSID\{<CLSID>}\VersionIndependentProgID key with 
        // the default value '<VersionIndependentProgID>'.
        if (SUCCEEDED(hr) && pszVerIndProgID != NULL)
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), 
                L"CLSID\\%s\\VersionIndependentProgID", szCLSID);
            if (SUCCEEDED(hr))
            {
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszVerIndProgID);
            }
        }

        // Create the HKCR\CLSID\{<CLSID>}\TypeLib key with the default value 
        // '{<LIBID>}'
        if (SUCCEEDED(hr))
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), 
                L"CLSID\\%s\\TypeLib", szCLSID);
            if (SUCCEEDED(hr))
            {
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, szLIBID);
            }
        }

        // Create the HKCR\CLSID\{<CLSID>}\LocalServer32 key.
        if (SUCCEEDED(hr))
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), 
                L"CLSID\\%s\\LocalServer32", szCLSID);
            if (SUCCEEDED(hr))
            {
                // Set the default value of the LocalServer32 key to the path 
                // of the COM module.
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszModule);
            }
        }
    }

    // Create the HKCR\<ProgId> key.
    if (SUCCEEDED(hr) && pszProgID != NULL)
    {
        hr = SetHKCRRegistryKeyAndValue(pszProgID, NULL, pszFriendlyName);

        // Create the HKCR\<ProgId>\CLSID key with the default value 
        // '{<CLSID>}'.
        if (SUCCEEDED(hr))
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"%s\\CLSID", 
                pszProgID);
            if (SUCCEEDED(hr))
            {
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, szCLSID);
            }
        }
    }

    // Create the HKCR\<VersionIndependentProgID> key.
    if (SUCCEEDED(hr) && pszVerIndProgID != NULL)
    {
        hr = SetHKCRRegistryKeyAndValue(pszVerIndProgID, NULL, pszFriendlyName);

        // Create the HKCR\<VersionIndependentProgID>\CLSID key with the 
        // default value '{<CLSID>}'.
        if (SUCCEEDED(hr))
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"%s\\CLSID", 
                pszVerIndProgID);
            if (SUCCEEDED(hr))
            {
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, szCLSID);
            }
        }

        // Create the HKCR\<VersionIndependentProgID>\CurVer key with the 
        // default value '<ProgID>'.
        if (SUCCEEDED(hr) && pszProgID != NULL)
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"%s\\CurVer", 
                pszVerIndProgID);
            if (SUCCEEDED(hr))
            {
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszProgID);
            }
        }
    }

    return hr;
}


//
//   FUNCTION: RegisterTypeLib
//
//   PURPOSE: This helper function registers the type library.
//
//   PARAMETERS:
//   * pszTypeLib - The type library file.
//
//   NOTE: The function creates the HKCR\TypeLib\{<LIBID>} key and the 
//   HKCR\Interface\{<IID>} key in the registry.
// 
//   HKCR
//   {
//      NoRemove TypeLib
//      {
...
//      }
//      NoRemove Interface
//      {
//          ForceRemove {<IID>} = s '<Interface Name>'
//          {
//              ProxyStubClsid = s '<ProgID>'
//              ProxyStubClsid32 = s '<VersionIndependentProgID>'
//              TypeLib = s '{<LIBID>}'
//              {
//                  val Version = s '<TypeLib Version>'
//              }
//          }
//      }
//   }
//
HRESULT RegisterTypeLib(PCWSTR pszTypeLib)
{
    HRESULT hr;
    ITypeLib *pTLB = NULL;

    hr = LoadTypeLibEx(pszTypeLib, REGKIND_REGISTER, &pTLB);
    if (SUCCEEDED(hr))
    {
        pTLB->Release();
    }

    return hr;
}


//
//   FUNCTION: UnregisterLocalServer(void)
//
//   PURPOSE: Unegister the out-of-process component in the registry.
//
//   PARAMETERS:
//   * clsid - Class ID of the component
//   * pszProgID - ProgID of the component
//   * pszVerIndProgID - Version independent ProgID
//
//   NOTE: The function deletes the HKCR\CLSID\{<CLSID>} key and the 
//   HKCR\<ProgID> key in the registry.
//
HRESULT UnregisterLocalServer(const CLSID& clsid, 
                              PCWSTR pszProgID, 
                              PCWSTR pszVerIndProgID)
{
    HRESULT hr = S_OK;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // Delete the HKCR\CLSID\{<CLSID>} key.
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (SUCCEEDED(hr))
    {
        hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, szSubkey));
    }

    // Delete the HKCR\<ProgID> key.
    if (SUCCEEDED(hr) && pszProgID != NULL)
    {
        hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, pszProgID));
    }

    // Delete the HKCR\<VersionIndependentProgID> key.
    if (SUCCEEDED(hr) && pszVerIndProgID != NULL)
    {
        hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, pszVerIndProgID));
    }

    return hr;
}


//
//   FUNCTION: UnregisterTypeLib
//
//   PURPOSE: This helper function unregisters the type library.
//
//   PARAMETERS:
//   * pszTypeLib - The type library file.
//
//   NOTE: The function deletes the HKCR\TypeLib\{<LIBID>} key and the 
//   HKCR\Interface\{<IID>} key in the registry.
//
HRESULT UnregisterTypeLib(PCWSTR pszTypeLib)
{
    HRESULT hr;

    ITypeLib *pTLB = NULL;
    hr = LoadTypeLibEx(pszTypeLib, REGKIND_NONE, &pTLB);
    if (SUCCEEDED(hr))
    {
        TLIBATTR *pAttr = NULL;
		hr = pTLB->GetLibAttr(&pAttr);
        if (SUCCEEDED(hr))
        {
            hr = UnRegisterTypeLib(pAttr->guid, pAttr->wMajorVerNum, 
                pAttr->wMinorVerNum, pAttr->lcid, pAttr->syskind);

            pTLB->ReleaseTLibAttr(pAttr);
        }

        pTLB->Release();
    }

    return hr;
}