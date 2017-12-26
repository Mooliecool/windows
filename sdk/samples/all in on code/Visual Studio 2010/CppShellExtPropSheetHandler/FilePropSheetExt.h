/****************************** Module Header ******************************\
Module Name:  FilePropSheetExt.h
Project:      CppShellExtPropSheetHandler
Copyright (c) Microsoft Corporation.

The code sample demonstrates creating a Shell property sheet handler with C++. 

A property sheet extension is a COM object implemented as an in-proc server. 
The property sheet extension must implement the IShellExtInit and 
IShellPropSheetExt interfaces. A property sheet extension is instantiated 
when the user displays the property sheet for an object of a class for which 
the property sheet extension has been registered in the display specifier of 
the class. It enables you to add or replace pages. You can register and 
implement a property sheet handler for a file class, a mounted drive, a 
control panel application, and starting from Windows 7, you can install a 
property sheet handler to devices in Devices and Printers dialog.

The example property sheet handler adds a property sheet page with the title 
"CppShellExtPropSheetHandler" to the Properties dialog of the .cpp file class 
when one .cpp file is selected in the Windows Explorer. The property sheet 
page displays the name of the selected file. It also has a button "Simulate 
Property Changing" to simulate the change of properties that activates the 
"Apply" button of the property sheet. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include <windows.h>
#include <shlobj.h>     // For IShellExtInit and IShellPropSheetExt


class FilePropSheetExt : public IShellExtInit, public IShellPropSheetExt
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

    // IShellPropSheetExt
    IFACEMETHODIMP AddPages(LPFNADDPROPSHEETPAGE pfnAddPage, LPARAM lParam);
    IFACEMETHODIMP ReplacePage(UINT uPageID, LPFNADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam);

    FilePropSheetExt();
    PCWSTR GetSelectedFile();

protected:
    ~FilePropSheetExt();

private:
    // Reference count of component.
    long m_cRef;

    // The name of the selected file.
    wchar_t m_szSelectedFile[MAX_PATH];

    // Property sheet page title.
    PCWSTR pszPageTitle;
};