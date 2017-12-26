/****************************** Module Header ******************************\
Module Name:  Reg.h
Project:      CppExeCOMServer
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include <windows.h>


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
                           PCWSTR pszVerIndProgID);


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
                              PCWSTR pszVerIndProgID);


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
HRESULT RegisterTypeLib(PCWSTR pszTypeLib);


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
HRESULT UnregisterTypeLib(PCWSTR pszTypeLib);