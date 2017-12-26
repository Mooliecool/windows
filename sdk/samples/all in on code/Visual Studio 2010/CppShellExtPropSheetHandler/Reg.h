/****************************** Module Header ******************************\
Module Name:  Reg.h
Project:      CppShellExtPropSheetHandler
Copyright (c) Microsoft Corporation.

The file declares reusable helper functions to register and unregister 
in-process COM components and shell property sheet handlers in the registry.

RegisterInprocServer - register the in-process component in the registry.
UnregisterInprocServer - unregister the in-process component in the registry.
RegisterShellExtPropSheetHandler - register the property sheet handler.
UnregisterShellExtPropSheetHandler - unregister the property sheet handler.

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
//   FUNCTION: RegisterShellExtPropSheetHandler
//
//   PURPOSE: Register the property sheet handler.
//
//   PARAMETERS:
//   * pszFileType - The file type that the property sheet handler is 
//     associated with. For example, '*' means all file types; '.txt' means 
//     all .txt files. The parameter must not be NULL.
//   * clsid - Class ID of the component
//   * pszFriendlyName - Friendly name
//
//   NOTE: The function creates the following key in the registry.
//
//   HKCR
//   {
//      NoRemove <File Type>
//      {
//          NoRemove shellex
//          {
//              NoRemove PropertySheetHandlers
//              {
//                  {<CLSID>} = s '<Friendly Name>'
//              }
//          }
//      }
//   }
//
HRESULT RegisterShellExtPropSheetHandler(
    PCWSTR pszFileType, const CLSID& clsid, PCWSTR pszFriendlyName);


//
//   FUNCTION: UnregisterShellExtPropSheetHandler
//
//   PURPOSE: Unregister the property sheet handler.
//
//   PARAMETERS:
//   * pszFileType - The file type that the property sheet handler is 
//     associated with. For example, '*' means all file types; '.txt' means 
//     all .txt files. The parameter must not be NULL.
//   * clsid - Class ID of the component
//
//   NOTE: The function removes the {<CLSID>} key under 
//   HKCR\<File Type>\shellex\PropertySheetHandlers in the registry.
//
HRESULT UnregisterShellExtPropSheetHandler(
    PCWSTR pszFileType, const CLSID& clsid);