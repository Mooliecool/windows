/****************************** Module Header ******************************\
Module Name:  Reg.cpp
Project:      CppDllCOMServer
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


#pragma region Registry Helper Functions

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

    if (SUCCEEDED(hr))
    {
        if (pszData != NULL)
        {
            // Set the specified value of the key.
            DWORD cbData = lstrlen(pszData) * sizeof(*pszData);
            hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, pszValueName, 0, 
                REG_SZ, reinterpret_cast<const BYTE *>(pszData), cbData));
        }
        
        RegCloseKey(hKey);
    }

    return hr;
}

#pragma endregion


//
//   FUNCTION: RegisterInprocServer
//
//   PURPOSE: Register the in-process component in the registry.
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
//              InprocServer32 = s '%MODULE%'
//              {
//                  val ThreadingModel = s '<Thread Model>'
//              }
//              TypeLib = s '{<LIBID>}'
//          }
//      }
//   }
//
HRESULT RegisterInprocServer(PCWSTR pszModule, 
                             const CLSID& clsid, 
                             PCWSTR pszFriendlyName,
                             PCWSTR pszThreadModel,
                             const IID& libid,
                             PCWSTR pszProgID, 
                             PCWSTR pszVerIndProgID)
{
    HRESULT hr = E_INVALIDARG;
    if (pszModule == NULL || pszThreadModel == NULL)
    {
        return hr;
    }

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

        // Create the HKCR\CLSID\{<CLSID>}\InprocServer32 key.
        if (SUCCEEDED(hr))
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), 
                L"CLSID\\%s\\InprocServer32", szCLSID);
            if (SUCCEEDED(hr))
            {
                // Set the default value of the InprocServer32 key to the 
                // path of the COM module.
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszModule);
                if (SUCCEEDED(hr))
                {
                    // Set the threading model of the component.
                    hr = SetHKCRRegistryKeyAndValue(szSubkey, 
                        L"ThreadingModel", pszThreadModel);
                }
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
//   PURPOSE: Register the type library.
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
//          ForceRemove {<LIBID>}
//          {
//              1.0
//              {
//                  0
//                  {
//                      win32 = s '%MODULE%'
//                  }
//                  FLAGS = 0
//                  HELPDIR = s '%MODULE DIR%'
//              }
//          }
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
//   FUNCTION: UnregisterInprocServer(void)
//
//   PURPOSE: Unegister the in-process component in the registry.
//
//   PARAMETERS:
//   * clsid - Class ID of the component
//   * pszProgID - ProgID of the component
//   * pszVerIndProgID - Version independent ProgID
//
//   NOTE: The function deletes the HKCR\CLSID\{<CLSID>} key and the 
//   HKCR\<ProgID> key in the registry.
//
HRESULT UnregisterInprocServer(const CLSID& clsid, 
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
//   PURPOSE: Unregister the type library.
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