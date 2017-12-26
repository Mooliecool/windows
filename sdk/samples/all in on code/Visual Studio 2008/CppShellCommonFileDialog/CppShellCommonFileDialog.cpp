/************************************ Module Header ******************************\
Module Name:  CppShellCommonFileDialog.cpp
Project:      CppShellCommonFileDialog
Copyright (c) Microsoft Corporation.

The code sample demos the use of shell common file dialogs.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\**********************************************************************************/

#pragma region Includes and Manifest Dependencies
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include "Resource.h"

#include <new>
#include <shlobj.h>
#include <shlwapi.h>

// Enable Visual Style
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma endregion


HINSTANCE g_hInst;        // Current instance


//
//   FUNCTION: ReportError(LPWSTR, DWORD)
//
//   PURPOSE: Display an error dialog for the failure of a certain function.
//
//   PARAMETERS:
//   * pszFunction - the name of the function that failed.
//   * hr - the HRESULT value.
//
void ReportError(LPCWSTR pszFunction, HRESULT hr)
{
    wchar_t szMessage[200];
    if (SUCCEEDED(StringCchPrintf(szMessage, ARRAYSIZE(szMessage), 
        L"%s failed w/hr 0x%08lx", pszFunction, hr)))
    {
        MessageBox(NULL, szMessage, L"Error", MB_ICONERROR);
    }
}


#pragma region Basic Open File Dialogs

const COMDLG_FILTERSPEC c_rgFileTypes[] =
{
    { L"Word Documents (*.docx)",   L"*.docx" },
    { L"Text Files (*.txt)",        L"*.txt" },
    { L"All Files (*.*)",           L"*.*" }
};

//
//   FUNCTION: OnOpenAFile(HWND)
//
//   PURPOSE: Use the common file open dialog to select a file.
//
void OnOpenAFile(HWND hWnd)
{
    HRESULT hr = S_OK;

    // Create a new common open file dialog.
    IFileDialog *pfd = NULL;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, 
        IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        // Control the default folder of the file dialog. Here we set it as the 
        // Music library known folder. 
        IShellItem *psiMusic = NULL;
        hr = SHCreateItemInKnownFolder(FOLDERID_Music, 0, NULL, 
            IID_PPV_ARGS(&psiMusic));
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetFolder(psiMusic);
            psiMusic->Release();
        }

        // Set the title of the dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetTitle(L"Select a File");
        }

        // Specify file types for the file dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetFileTypes(ARRAYSIZE(c_rgFileTypes), c_rgFileTypes);
            if (SUCCEEDED(hr))
            {
                // Set the selected file type index to Word Document.
                hr = pfd->SetFileTypeIndex(1);
            }
        }

        // Set the default extension to be added to file names as ".docx"
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetDefaultExtension(L"docx");
        }

        // Show the open file dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->Show(hWnd);
            if (SUCCEEDED(hr))
            {
                // Get the result of the open file dialog.
                IShellItem *psiResult = NULL;
                hr = pfd->GetResult(&psiResult);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszPath = NULL;
                    hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
                    if (SUCCEEDED(hr))
                    {
                        MessageBox(hWnd, pszPath, L"The selected file is", MB_OK);
                        CoTaskMemFree(pszPath);
                    }
                    psiResult->Release();
                }
            }
            else
            {
                if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
                {
                    // User cancelled the dialog...
                }
            }
        }

        pfd->Release();
    }

    // Report the error. 
    if (FAILED(hr))
    {
        // If it's not that the user cancelled the dialog, report the error in a 
        // message box.
        if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            ReportError(L"OnOpenAFile", hr);
        }
    }
}


//
//   FUNCTION: OnOpenAFolder(HWND)
//
//   PURPOSE:  Use the common file open dialog to select a folder.
//
void OnOpenAFolder(HWND hWnd)
{
    HRESULT hr = S_OK; 

    // Create a new common open file dialog.
    IFileOpenDialog *pfd = NULL;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, 
        IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        // Set the dialog as a folder picker.
        DWORD dwOptions;
        hr = pfd->GetOptions(&dwOptions);
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
        }

        // Set the title of the dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetTitle(L"Select a Folder");
        }

        // Show the open file dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->Show(hWnd);
            if (SUCCEEDED(hr))
            {
                // Get the selection from the user.
                IShellItem *psiResult = NULL;
                hr = pfd->GetResult(&psiResult);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszPath = NULL;
                    hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
                    if (SUCCEEDED(hr))
                    {
                        MessageBox(hWnd, pszPath, L"The selected folder is", MB_OK);
                        CoTaskMemFree(pszPath);
                    }
                    psiResult->Release();
                }
            }
            else
            {
                if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
                {
                    // User cancelled the dialog...
                }
            }
        }

        pfd->Release();
    }

    // Report the error.
    if (FAILED(hr))
    {
        // If it's not that the user cancelled the dialog, report the error in a 
        // message box.
        if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            ReportError(L"OnOpenAFolder", hr);
        }
    }
}


//
//   FUNCTION: OnOpenFiles(HWND)
//
//   PURPOSE:  Use the common file open dialog to select multiple files.
//
void OnOpenFiles(HWND hWnd)
{
    HRESULT hr = S_OK;

    // Create a new common open file dialog.
    IFileOpenDialog *pfd = NULL;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, 
        IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        // Allow multi-selection in the common file dialog.
        DWORD dwOptions;
        hr = pfd->GetOptions(&dwOptions);
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetOptions(dwOptions | FOS_ALLOWMULTISELECT);
        }

        // Set the title of the dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetTitle(L"Select Files");
        }

        // Show the open file dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->Show(hWnd);
            if (SUCCEEDED(hr))
            {
                // Obtain the results of the user interaction.
                IShellItemArray *psiaResults = NULL;
                hr = pfd->GetResults(&psiaResults);
                if (SUCCEEDED(hr))
                {
                    // Get the number of files being selected.
                    DWORD dwFolderCount;
                    hr = psiaResults->GetCount(&dwFolderCount);
                    if (SUCCEEDED(hr))
                    {
                        // Allocate a zero buffer for concatting all file paths.
                        DWORD cch = dwFolderCount * MAX_PATH;
                        PWSTR pszPaths = new wchar_t[cch];
                        ZeroMemory(pszPaths, cch * sizeof(*pszPaths));

                        // Iterate through all selected files.
                        for (DWORD i = 0; i < dwFolderCount; i++)
                        {
                            IShellItem *psi = NULL;
                            if (SUCCEEDED(psiaResults->GetItemAt(i, &psi)))
                            {
                                // Retrieve the file path.
                                PWSTR pszPath = NULL;
                                if (SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, 
                                    &pszPath)))
                                {
                                    StringCchCat(pszPaths, cch, pszPath);
                                    StringCchCat(pszPaths, cch, L"\r\n");
                                    CoTaskMemFree(pszPath);
                                }
                                psi->Release();
                            }
                        }

                        // Display the result.
                        MessageBox(hWnd, pszPaths, L"The selected files are", MB_OK);

                        delete[] pszPaths;
                    }                    
                }

                psiaResults->Release();
            }
            else
            {
                if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
                {
                    // User cancelled the dialog...
                }
            }
        }

        pfd->Release();
    }

    // Report the error.
    if (FAILED(hr))
    {
        // If it's not that the user cancelled the dialog, report the error in a 
        // message box.
        if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            ReportError(L"OnOpenFiles", hr);
        }
    }
}

#pragma endregion


#pragma region Customized Open File Dialogs

// Controls
// It is OK for CONTROL_RADIOBUTTON2 to have the same ID as CONTROL_RADIOBUTTONLIST, 
// because it is a child control under CONTROL_RADIOBUTTONLIST.
#define CONTROL_GROUP           2000
#define CONTROL_RADIOBUTTONLIST 2
#define CONTROL_RADIOBUTTON1    1
#define CONTROL_RADIOBUTTON2    2


//
//   CLASS: CFileDialogEventHandler
//
//   PURPOSE: 
//   File Dialog Event Handler that reesponds to Events in Added Controls. The 
//   events handler provided by the calling process can implement 
//   IFileDialogControlEvents in addition to IFileDialogEvents. 
//   IFileDialogControlEvents enables the calling process to react to these events: 
//     1) PushButton clicked. 
//     2) CheckButton state changed. 
//     3) Item selected from a menu, ComboBox, or RadioButton list. 
//     4) Control activating. This is sent when a menu is about to display a 
//        drop-down list, in case the calling process wants to change the items in 
//        the list.
//
class CFileDialogEventHandler : 
    public IFileDialogEvents, 
    public IFileDialogControlEvents
{
public:

    // 
    // IUnknown methods
    // 

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        static const QITAB qit[] = 
        {
            QITABENT(CFileDialogEventHandler, IFileDialogEvents),
            QITABENT(CFileDialogEventHandler, IFileDialogControlEvents),
            { 0 }
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&m_cRef);
        if (!cRef)
        {
            delete this;
        }
        return cRef;
    }

    // 
    // IFileDialogEvents methods
    // 

    IFACEMETHODIMP OnFileOk(IFileDialog*)
    { return S_OK; }
    IFACEMETHODIMP OnFolderChange(IFileDialog*)
    { return S_OK; }
    IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*)
    { return S_OK; }
    IFACEMETHODIMP OnHelp(IFileDialog*)
    { return S_OK; }
    IFACEMETHODIMP OnSelectionChange(IFileDialog*)
    { return S_OK; }
    IFACEMETHODIMP OnTypeChange(IFileDialog*)
    { return S_OK; }
    IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*)
    { return S_OK; }
    IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*)
    { return S_OK; }

    // 
    // IFileDialogControlEvents methods
    // 

    IFACEMETHODIMP OnItemSelected(IFileDialogCustomize*pfdc, DWORD dwIDCtl, DWORD dwIDItem)
    {
        IFileDialog *pfd = NULL;
        HRESULT hr = pfdc->QueryInterface(&pfd);
        if (SUCCEEDED(hr))
        {
            if (dwIDCtl == CONTROL_RADIOBUTTONLIST)
            {
                switch (dwIDItem)
                {
                case CONTROL_RADIOBUTTON1:
                    hr = pfd->SetTitle(L"Windows Vista");
                    break;

                case CONTROL_RADIOBUTTON2:
                    hr = pfd->SetTitle(L"Windows 7");
                    break;
                }
            }
            pfd->Release();
        }
        return hr;
    }

    IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize*, DWORD)
    { return S_OK; }
    IFACEMETHODIMP OnControlActivating(IFileDialogCustomize*, DWORD)
    { return S_OK; }
    IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize*, DWORD, BOOL)
    { return S_OK; }

    CFileDialogEventHandler() : m_cRef(1) { }

protected:

    ~CFileDialogEventHandler() { }
    long m_cRef;
};


//
//   FUNCTION: CFileDialogEventHandler_CreateInstance(REFIID, void**)
//
//   PURPOSE:  CFileDialogEventHandler instance creation helper function.
//
HRESULT CFileDialogEventHandler_CreateInstance(REFIID riid, void **ppv)
{
    *ppv = NULL;
    CFileDialogEventHandler* pFileDialogEventHandler = 
        new(std::nothrow)CFileDialogEventHandler();
    HRESULT hr = pFileDialogEventHandler ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = pFileDialogEventHandler->QueryInterface(riid, ppv);
        pFileDialogEventHandler->Release();
    }
    return hr;
}


//
//   FUNCTION: OnAddCustomControls(HWND)
// 
//   PURPOSE:  
//   The function demonstrates how to add custom controls in the Common File Dialog.
//
//
void OnAddCustomControls(HWND hWnd)
{
    HRESULT hr = S_OK;

    // Create a new common open file dialog
    IFileDialog *pfd = NULL;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, 
        IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        // Create an event handling object, and hook it up to the dialog.
        IFileDialogEvents *pfde = NULL;
        hr = CFileDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
        if (SUCCEEDED(hr))
        {
            // Hook up the event handler.
            DWORD dwCookie = 0;
            hr = pfd->Advise(pfde, &dwCookie);
            if (SUCCEEDED(hr))
            {
                // Set up the customization.
                IFileDialogCustomize *pfdc = NULL;
                hr = pfd->QueryInterface(IID_PPV_ARGS(&pfdc));
                if (SUCCEEDED(hr))
                {
                    // Create a visual group.
                    hr = pfdc->StartVisualGroup(CONTROL_GROUP, L"Change Title to ");
                    if (SUCCEEDED(hr))
                    {
                        // Add a radio-button list.
                        hr = pfdc->AddRadioButtonList(CONTROL_RADIOBUTTONLIST);
                        if (SUCCEEDED(hr))
                        {
                            // Set the state of the added radio-button list.
                            hr = pfdc->SetControlState(CONTROL_RADIOBUTTONLIST, 
                                CDCS_VISIBLE | CDCS_ENABLED);
                        }

                        // Add individual buttons to the radio-button list.
                        if (SUCCEEDED(hr))
                        {
                            hr = pfdc->AddControlItem(CONTROL_RADIOBUTTONLIST, 
                                CONTROL_RADIOBUTTON1, L"Windows Vista");
                        }
                        if (SUCCEEDED(hr))
                        {
                            hr = pfdc->AddControlItem(CONTROL_RADIOBUTTONLIST, 
                                CONTROL_RADIOBUTTON2, L"Windows 7");
                        }

                        // Set the default selection to option 1.
                        if (SUCCEEDED(hr))
                        {
                            hr = pfdc->SetSelectedControlItem(
                                CONTROL_RADIOBUTTONLIST, CONTROL_RADIOBUTTON1);
                        }

                        // End the visual group
                        pfdc->EndVisualGroup();
                    }
                    pfdc->Release();
                }

                // Show the open file dialog.
                if (SUCCEEDED(hr))
                {
                    hr = pfd->Show(hWnd);
                    if (SUCCEEDED(hr))
                    {                        
                        // You can add your own code here to handle the results...
                    }
                }

                // Unhook the event handler
                pfd->Unadvise(dwCookie);
            }
            pfde->Release();
        }
        pfd->Release();
    }

    // Report the error. 
    if (FAILED(hr))
    {
        // If it's not that the user cancelled the dialog, report the error in a 
        // message box.
        if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            ReportError(L"OnAddCustomControls", hr);
        }
    }
}


//
//   FUNCTION: OnAddCommonPlaces(HWND)
//
//   PURPOSE:  
//   The Common Places area in the Common File Dialog is extensible. This code 
//   snippet demonstrates how to extend the Common Places area.
//
//
void OnAddCommonPlaces(HWND hWnd)
{
    HRESULT hr = S_OK;

    // Create a new common open file dialog.
    IFileDialog *pfd = NULL;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, 
        IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        // Get the shell item of the PublicMusic known folder.
        IShellItem *psiPublicMusic = NULL;
        hr = SHCreateItemInKnownFolder(FOLDERID_PublicMusic, 0, NULL, 
            IID_PPV_ARGS(&psiPublicMusic));
        if (SUCCEEDED(hr))
        {
            // Add the place to the bottom of default list in Common File Dialog.
            hr = pfd->AddPlace(psiPublicMusic, FDAP_BOTTOM);
            psiPublicMusic->Release();
        }

        // Show the open file dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->Show(hWnd);
            if (SUCCEEDED(hr))
            {
                // You can add your own code here to handle the results...
            }
        }

        pfd->Release();
    }

    // Report the error. 
    if (FAILED(hr))
    {
        // If it's not that the user cancelled the dialog, report the error in a 
        // message box.
        if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            ReportError(L"OnAddCommonPlaces", hr);
        }
    }
}

#pragma endregion


#pragma region Basic Save File Dialogs

const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
    { L"Word Documents (*.docx)",   L"*.docx" },
    { L"Text Files (*.txt)",        L"*.txt" }
};

//
//   FUNCTION: OnSaveAFile(HWND)
//
//   PURPOSE: Use the common file save dialog to save a file.
//
void OnSaveAFile(HWND hWnd)
{
    HRESULT hr = S_OK;

    // Create a new common save file dialog.
    IFileDialog *pfd = NULL;
    hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, 
        IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        // (Optional) Set the title of the dialog.
        hr = pfd->SetTitle(L"Save a File");

        // (Optional) Specify file types for the file dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
            if (SUCCEEDED(hr))
            {
                // Set the selected file type index to Word Document.
                hr = pfd->SetFileTypeIndex(1);
            }
        }

        // (Optional) Set the default extension to be added as ".docx".
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetDefaultExtension(L"docx");
        }

        // (Optional) Display a warning if the user specifies a file name that 
        // aleady exists. This is a default value for the Save dialog.
        if (SUCCEEDED(hr))
        {
            DWORD dwOptions;
            hr = pfd->GetOptions(&dwOptions);
            if (SUCCEEDED(hr))
            {
                hr = pfd->SetOptions(dwOptions | FOS_OVERWRITEPROMPT);
            }
        }

        // Show the save file dialog.
        if (SUCCEEDED(hr))
        {
            hr = pfd->Show(hWnd);
            if (SUCCEEDED(hr))
            {
                // Get the result of the save file dialog.
                IShellItem *psiResult = NULL;
                hr = pfd->GetResult(&psiResult);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszPath = NULL;
                    hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
                    if (SUCCEEDED(hr))
                    {
                        // Open and save to the file.
                        HANDLE hFile = CreateFile(pszPath, 
                            GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, 
                            FILE_ATTRIBUTE_NORMAL, NULL);
                        if (hFile != INVALID_HANDLE_VALUE)
                        {
                            // Write to the file stream.
                            // ...

                            CloseHandle(hFile);

                            MessageBox(hWnd, pszPath, L"The saved file is", MB_OK);
                        }

                        CoTaskMemFree(pszPath);
                    }
                    psiResult->Release();
                }
            }
        }

        pfd->Release();
    }

    // Report the error. 
    if (FAILED(hr))
    {
        // If it's not that the user cancelled the dialog, report the error in a 
        // message box.
        if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            ReportError(L"OnSaveAFile", hr);
        }
    }
}

#pragma endregion


#pragma region Customized Save File Dialogs



#pragma endregion


#pragma region Main Window

// 
//   FUNCTION: OnInitDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message. 
//
BOOL OnInitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{
    return TRUE;
}


//
//   FUNCTION: OnCommand(HWND, int, HWND, UINT)
//
//   PURPOSE: Process the WM_COMMAND message
//
void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDC_BUTTON_OPENAFILE:
        OnOpenAFile(hWnd);
        break;

    case IDC_BUTTON_OPENAFOLDER:
        OnOpenAFolder(hWnd);
        break;

    case IDC_BUTTON_OPENFILES:
        OnOpenFiles(hWnd);
        break;

    case IDC_BUTTON_ADDCUSTOMCONTROLS:
        OnAddCustomControls(hWnd);
        break;

    case IDC_BUTTON_ADDCOMMONPLACES:
        OnAddCommonPlaces(hWnd);
        break;

    case IDC_BUTTON_SAVEAFILE:
        OnSaveAFile(hWnd);
        break;

    case IDOK:
    case IDCANCEL:
        EndDialog(hWnd, 0);
        break;
    }
}


//
//   FUNCTION: OnClose(HWND)
//
//   PURPOSE: Process the WM_CLOSE message
//
void OnClose(HWND hWnd)
{
    EndDialog(hWnd, 0);
}


//
//  FUNCTION: DialogProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main dialog.
//
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitDialog);

        // Handle the WM_COMMAND message in OnCommand
        HANDLE_MSG (hWnd, WM_COMMAND, OnCommand);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;
    }
    return 0;
}

#pragma endregion


//
//  FUNCTION: wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
//
//  PURPOSE:  The entry point of the application.
//
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow)
{
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
}