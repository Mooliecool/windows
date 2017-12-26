/****************************** Module Header ******************************\
Module Name:  FileInfotipExt.h
Project:      CppShellExtInfotipHandler
Copyright (c) Microsoft Corporation.

The code sample demonstrates creating a Shell infotip handler with C++. An 
infotip handler is a shell extension handler that provides pop-up text when 
the user hovers the mouse pointer over the object. It is the most flexible 
way to customize infotips. The alternative way is to specify either a fixed 
string or a list of certain file properties to be displayed (See the Infotip 
Customization section in http://msdn.microsoft.com/en-us/library/cc144105.aspx)

The example infotip handler customizes the infotips of .cpp file objects. 
When you hover your mouse pointer over a .cpp file object in the Windows 
Explorer, you will see an infotip with the text:

    File: <File path, e.g. D:\Test.cpp>
    Lines: <Line number, e.g. 123 or N/A>
    - Infotip displayed by CppShellExtInfotipHandler

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include <windows.h>
#include <shlobj.h>     // For IPersistFile and IQueryInfo


class FileInfotipExt : public IPersistFile, public IQueryInfo
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IPersistFile
    IFACEMETHODIMP GetClassID(CLSID *pClassID);
    IFACEMETHODIMP IsDirty(void);
    IFACEMETHODIMP Load(LPCOLESTR pszFileName, DWORD dwMode);
    IFACEMETHODIMP Save(LPCOLESTR pszFileName, BOOL fRemember);
    IFACEMETHODIMP SaveCompleted(LPCOLESTR pszFileName);
	IFACEMETHODIMP GetCurFile(LPOLESTR *ppszFileName);

    // IQueryInfo
    IFACEMETHODIMP GetInfoTip(DWORD dwFlags, LPWSTR *ppwszTip);
    IFACEMETHODIMP GetInfoFlags(DWORD *pdwFlags);
	
    FileInfotipExt();

protected:
    ~FileInfotipExt();

private:
    // Reference count of component.
    long m_cRef;

    // The name of the selected file.
    wchar_t m_szSelectedFile[MAX_PATH];
};