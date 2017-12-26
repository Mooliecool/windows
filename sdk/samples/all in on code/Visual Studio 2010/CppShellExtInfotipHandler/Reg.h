/****************************** Module Header ******************************\
Module Name:  Reg.h
Project:      CppShellExtInfotipHandler
Copyright (c) Microsoft Corporation.

The file declares reusable helper functions to register and unregister 
in-process COM components and shell infotip handlers in the registry.

RegisterInprocServer - register the in-process component in the registry.
UnregisterInprocServer - unregister the in-process component in the registry.
RegisterShellExtInfotipHandler - register the infotip handler.
UnregisterShellExtInfotipHandler - unregister the infotip handler.

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
//   FUNCTION: RegisterInprocServer
//
//   PURPOSE: Register the in-process component in the registry.
//
//   PARAMETERS:
//   * pszModule - Path of the module that contains the component
//   * clsid - Class ID of the component
//   * pszFriendlyName - Friendly name
//   * pszThreadModel - Threading model
//
//   NOTE: The function creates the HKCR\CLSID\{<CLSID>} key in the registry.
// 
//   HKCR
//   {
//      NoRemove CLSID
//      {
//          ForceRemove {<CLSID>} = s '<Friendly Name>'
//          {
//              InprocServer32 = s '%MODULE%'
//              {
//                  val ThreadingModel = s '<Thread Model>'
//              }
//          }
//      }
//   }
//
HRESULT RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, 
    PCWSTR pszFriendlyName, PCWSTR pszThreadModel);


//
//   FUNCTION: UnregisterInprocServer
//
//   PURPOSE: Unegister the in-process component in the registry.
//
//   PARAMETERS:
//   * clsid - Class ID of the component
//
//   NOTE: The function deletes the HKCR\CLSID\{<CLSID>} key in the registry.
//
HRESULT UnregisterInprocServer(const CLSID& clsid);


//
//   FUNCTION: RegisterShellExtInfotipHandler
//
//   PURPOSE: Register the infotip handler.
//
//   PARAMETERS:
//   * pszFileType - The file type that the context menu handler is 
//     associated with. For example, '*' means all file types; '.txt' means 
//     all .txt files. The parameter must not be NULL.
//   * clsid - Class ID of the component
//
//   NOTE: The function creates the following key in the registry.
//
//   HKCR
//   {
//      NoRemove <File Type>
//      {
//          NoRemove shellex
//          {
//              {00021500-0000-0000-C000-000000000046} = s '{<CLSID>}'
//          }
//      }
//   }
//
HRESULT RegisterShellExtInfotipHandler(PCWSTR pszFileType, const CLSID& clsid);


//
//   FUNCTION: UnregisterShellExtInfotipHandler
//
//   PURPOSE: Unregister the infotip handler.
//
//   PARAMETERS:
//   * pszFileType - The file type that the context menu handler is 
//     associated with. For example, '*' means all file types; '.txt' means 
//     all .txt files. The parameter must not be NULL.
//
//   NOTE: The function removes the registry key
//   HKCR\<File Type>\shellex\{00021500-0000-0000-C000-000000000046}.
//
HRESULT UnregisterShellExtInfotipHandler(PCWSTR pszFileType);