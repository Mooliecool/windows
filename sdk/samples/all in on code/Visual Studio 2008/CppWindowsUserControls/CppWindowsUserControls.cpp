/********************************* Module Header *********************************\
Module Name:  CppWindowsUserControls.cpp
Project:      CppWindowsUserControls
Copyright (c) Microsoft Corporation.

CppWindowsUserControls contains simple examples of how to create user controls 
defined in user32.dll. The controls include Buttons, Combo-boxes, Edits, Listboxes, 
RichEdit(in msftedit.dll or riched20.dll or riched32.dll), Scrollbars, and Statics.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*********************************************************************************/

#pragma region Includes and Manifest Dependencies
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

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


HINSTANCE g_hInst;  // The handle to the instance of the current module
HFONT g_hFont;      // Default font in Windows


//
//   FUNCTION: OnClose(HWND)
//
//   PURPOSE: Process the WM_CLOSE message
//
void OnClose(HWND hWnd)
{
    EndDialog(hWnd, 0);
}


#pragma region Button

#define IDC_DEFPUSHBUTTON		990
#define IDC_PUSHBUTTON			991
#define IDC_AUTOCHECKBOX		992
#define IDC_AUTORADIOBUTTON		993
#define IDC_GROUPBOX			994
#define IDC_ICONBUTTON			995
#define IDC_BITMAPBUTTON		996

//
//   FUNCTION: OnInitButtonDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message
//
BOOL OnInitButtonDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
    // The various button types are created by varying the style bits

    RECT rc = { 20, 20, 150, 30 };

    HWND hBtn = CreateWindowEx(0, L"BUTTON", L"Default Push Button", 
        BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_DEFPUSHBUTTON), g_hInst, 0);
    if (hBtn)
    {
        SendMessage(hBtn, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    rc.top += 40;

    hBtn = CreateWindowEx(0, L"BUTTON", L"Push Button",
        BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_PUSHBUTTON), g_hInst, 0);
    if (hBtn)
    {
        SendMessage(hBtn, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    rc.top += 40;

    hBtn = CreateWindowEx(0, L"BUTTON", L"Check Box",
        BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE,
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_AUTOCHECKBOX), g_hInst, 0);
    if (hBtn)
    {
        SendMessage(hBtn, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    rc.top += 30;

    hBtn = CreateWindowEx(0, L"BUTTON", L"Radio Button",
        BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE,
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_AUTORADIOBUTTON), g_hInst, 0);
    if (hBtn)
    {
        SendMessage(hBtn, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    rc.top += 40;
    rc.bottom = 50;

    hBtn = CreateWindowEx(0, L"BUTTON", L"Group Box",
        BS_GROUPBOX | WS_CHILD | WS_VISIBLE,
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_GROUPBOX), g_hInst, 0);
    if (hBtn)
    {
        SendMessage(hBtn, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    rc.top += 60;
    rc.right = 70;
    rc.bottom = 40;

    hBtn = CreateWindowEx(0, L"BUTTON", L"Icon Button - No Text", 
        BS_ICON | WS_CHILD | WS_VISIBLE,
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_ICONBUTTON), g_hInst, 0);
    if (hBtn)
    {
        HANDLE hIcon = LoadImage(0, IDI_EXCLAMATION, IMAGE_ICON, 0, 0, LR_SHARED);
        if (hIcon)
        {
            SendMessage(hBtn, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(hIcon));
        }
    }

    rc.left += 80;

    hBtn = CreateWindowEx(0, L"BUTTON", L"Bitmap Button - No Text", 
        BS_BITMAP | WS_CHILD | WS_VISIBLE,
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_BITMAPBUTTON), g_hInst, 0);
    if (hBtn)
    {
        HANDLE hBmp = LoadImage(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 
            0, 0, LR_SHARED);
        if (hBmp)
        {
            SendMessage(hBtn, BM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(hBmp));
        }
    }

    return TRUE;
}

//
//  FUNCTION: ButtonDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the Button control dialog.
//
//
INT_PTR CALLBACK ButtonDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitButtonDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitButtonDialog);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

#pragma endregion


#pragma region ComboBox

#define IDC_SIMPLECOMBO			1990
#define IDC_DROPDOWNCOMBO		1991
#define IDC_DROPDOWNLISTCOMBO	1992

//
//   FUNCTION: OnInitComboBoxDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message
//
BOOL OnInitComboBoxDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
    // The various combobox types are created by varying the style bits

    RECT rc = { 20, 20, 150, 90 };

    HWND hCombo = CreateWindowEx(0, L"COMBOBOX", L"Simple Combobox", 
        CBS_SIMPLE | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_SIMPLECOMBO), g_hInst, 0);
    if (hCombo)
    {
        SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Simple Combobox");
        SendMessage(hCombo, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    rc.top += 90;

    hCombo = CreateWindowEx(0, L"COMBOBOX", L"Drop Down Combobox", 
        CBS_DROPDOWN | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_DROPDOWNCOMBO), g_hInst, 0);
    if (hCombo)
    {
        SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Drop Down Combobox");
        SendMessage(hCombo, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE); 
    }

    rc.top += 40;

    hCombo = CreateWindowEx(0, L"COMBOBOX", L"Drop Down List Combobox", 
        CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_DROPDOWNLISTCOMBO), g_hInst, 0);
    if (hCombo)
    {
        SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Drop Down List Combobox");
        SendMessage(hCombo, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    return TRUE;
}

//
//  FUNCTION: ComboBoxDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the ComboBox control dialog.
//
//
INT_PTR CALLBACK ComboBoxDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitComboBoxDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitComboBoxDialog);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

#pragma endregion


#pragma region Edit

#define IDC_SINGLELINEEDIT		2990
#define IDC_MULTILINEEDIT		2991

//
//   FUNCTION: OnInitEditDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message
//
BOOL OnInitEditDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
    // The various edit types are created by varying the style bits

    RECT rc = { 20, 20, 150, 30 };

    HWND hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"Single Line", 
        WS_CHILD | WS_VISIBLE, rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_SINGLELINEEDIT), g_hInst, 0);
    if (hEdit)
    {
        SendMessage(hEdit, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    rc.top += 40;
    rc.bottom += 30;

    hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"Multi\r\nLine", 
        ES_MULTILINE | WS_VSCROLL | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_MULTILINEEDIT), g_hInst, 0);
    if (hEdit)
    {
        SendMessage(hEdit, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    return TRUE;
}

//
//  FUNCTION: EditDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the Edit control dialog.
//
//
INT_PTR CALLBACK EditDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitEditDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitEditDialog);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

#pragma endregion


#pragma region RichEdit

#include <richedit.h>		// To use richedit control

#pragma region Helpers

//
//   FUNCTION: IsWinXPSp1Min()
//
//   PURPOSE: Return TRUE if operating sytem is Windows XP SP1 or later. 
//   Windows XP SP1 is the minimum system required to use a richedit v4.1 but only 
//   when UNICODE is defined.
//
BOOL IsWinXPSp1Min()
{
    OSVERSIONINFO osvi = { sizeof(osvi) };
    if (!GetVersionEx(&osvi))
    {
        return FALSE;
    }

    // Determine if system is Windows XP minimum
    if (osvi.dwMajorVersion >= 5 && osvi.dwMinorVersion >= 1)
    {
        // Now check if system is specifically WinXP and, if so, what service pack 
        // version is installed.
        if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
        {
            // The following test assumes that the szCSDVersion member of the 
            // OSVERSIONINFO struct's format will always be a string like 
            // "Service Pack x", where 'x' is a number >= 1. This is fine for SP1 
            // and SP2 but future service packs may have a different string 
            // descriptor.
            TCHAR* pszCSDVersion = L"Service Pack 1";
            if (osvi.szCSDVersion < pszCSDVersion)
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}

//
//   FUNCTION: GetRichEditClassName()
//
//   PURPOSE: Load the proper version of RichEdit and return the class name.
//
PCWSTR GetRichEditClassName()
{
    HINSTANCE hLib;

    // Try to load latest version of rich edit control. Since v4.1 is available 
    // only as an UNICODE control on a minimum of Windows XP with service pack 1 
    // installed, use preprocessor conditional to ensure that an attempt to load 
    // Msftedit.dll is only made if UNICODE is defined.

#if defined UNICODE
    if (IsWinXPSp1Min())
    {
        // Try to get richedit v4.1, explicitly use wide character string as this 
        // is UNICODE only.
        hLib = LoadLibrary(L"msftedit.dll");
        if (hLib)
        {
            return MSFTEDIT_CLASS;
        }
    }
#endif

    // Cannot get latest version (v4.1) so try to get earlier one

    // Rich Edit Version 2.0/3.0
    hLib = LoadLibrary(L"riched20.dll");
    if (hLib)
    {
        // Version 2.0/3.0 is good
        return RICHEDIT_CLASS;
    }

    // Rich Edit Version 1.0
    hLib = LoadLibrary(L"riched32.dll");
    if (hLib)
    {
        // Version 1.0 is good
        return L"RichEdit";
    }

    // Cannot get any versions of RichEdit control (error)
    return L"";
}

#pragma endregion


#define IDC_RICHEDIT		3990

//
//   FUNCTION: OnInitRichEditDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message
//
BOOL OnInitRichEditDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
    RECT rc = { 20, 20, 160, 250 };

    HWND hRichEdit = CreateWindowEx(WS_EX_CLIENTEDGE, GetRichEditClassName(), 
        L"RichEdit Control", 
        ES_MULTILINE | ES_AUTOHSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
        WS_VSCROLL | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_RICHEDIT), g_hInst, 0);
    if (hRichEdit)
    {
        SendMessage(hRichEdit, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    return TRUE;
}

//
//  FUNCTION: RichEditDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the RichEdit control dialog.
//
//
INT_PTR CALLBACK RichEditDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitRichEditDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitRichEditDialog);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

#pragma endregion


#pragma region ListBox

#define IDC_LISTBOX		4990

//
//   FUNCTION: OnInitListBoxDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message
//
BOOL OnInitListBoxDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
    RECT rc = { 20, 20, 160, 250 };

    HWND hList = CreateWindowEx(WS_EX_CLIENTEDGE, L"LISTBOX", L"", 
        WS_CHILD | WS_VISIBLE,
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_LISTBOX), g_hInst, 0);
    if (hList)
    {
        SendMessage(hList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"ListBox"));
        SendMessage(hList, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    return TRUE;
}

//
//  FUNCTION: ListBoxDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the ListBox control dialog.
//
//
INT_PTR CALLBACK ListBoxDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitListBoxDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitListBoxDialog);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

#pragma endregion


#pragma region ScrollBar

#define IDC_HORZ_SCROLLBAR		5990
#define IDC_VERT_SCROLLBAR		5991

//
//   FUNCTION: OnInitScrollBarDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message
//
BOOL OnInitScrollBarDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
    // The various scrollbar types are created by simply varying the style bits

    RECT rc = { 20, 20, 160, 20 };

    HWND hScroll = CreateWindowEx(0, L"SCROLLBAR", NULL, 
        SBS_HORZ | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_HORZ_SCROLLBAR), g_hInst, 0);
    if (hScroll)
    {
        SendMessage(hScroll, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    SetRect(&rc, 160, 50, 20, 200);

    hScroll = CreateWindowEx(0, L"SCROLLBAR", NULL, 
        SBS_VERT | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_VERT_SCROLLBAR), g_hInst, 0);
    if (hScroll)
    {
        SendMessage(hScroll, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    return TRUE;
}

//
//  FUNCTION: ScrollBarDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the ScrollBar control dialog.
//
//
INT_PTR CALLBACK ScrollBarDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitScrollBarDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitScrollBarDialog);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

#pragma endregion


#pragma region Static

#define IDC_WHITE		6990
#define IDC_TEXT		6991
#define IDC_IMAGE		6992

//
//   FUNCTION: OnInitStaticDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message
//
BOOL OnInitStaticDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
    // The various static types are created by simply varying the style bits

    RECT rc = { 20, 20, 150, 20 };

    HWND hStatic = CreateWindowEx(0, L"STATIC", L"", 
        SS_WHITERECT | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom,
        hWnd, reinterpret_cast<HMENU>(IDC_WHITE), g_hInst, 0);
    if (hStatic)
    {
        SendMessage(hStatic, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    rc.top += 40;

    hStatic = CreateWindowEx(0, L"STATIC", L"Text Static Control", 
        SS_SIMPLE | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom,
        hWnd, reinterpret_cast<HMENU>(IDC_TEXT), g_hInst, 0);
    if (hStatic)
    {
        SendMessage(hStatic, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    rc.top += 30;

    hStatic = CreateWindowEx(0, L"STATIC", L"", 
        SS_ICON | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom,
        hWnd, reinterpret_cast<HMENU>(IDC_IMAGE), g_hInst, 0);
    if (hStatic)
    {
        HANDLE hImage = LoadImage(0, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
        if (hImage)
        {
            SendMessage(hStatic, STM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(hImage));
        }
        SendMessage(hStatic, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    return TRUE;
}

//
//  FUNCTION: StaticDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the Static control dialog.
//
//
INT_PTR CALLBACK StaticDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitStaticDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitStaticDialog);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

#pragma endregion


#pragma region Main Window

// 
//   FUNCTION: OnInitDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message. 
//
BOOL OnInitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{
    // Initialize the default font for Windows.
    g_hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"MS Shell Dlg");

    return (g_hFont != NULL);
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
    case IDC_BUTTON_BUTTON:
        {
            HWND hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_BUTTONDIALOG), 
                hWnd, ButtonDlgProc);
            if (hDlg)
            {
                ShowWindow(hDlg, SW_SHOW);
            }
        }
        break;

    case IDC_BUTTON_COMBOBOX:
        {
            HWND hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_COMBOBOXDIALOG), 
                hWnd, ComboBoxDlgProc);
            if (hDlg)
            {
                ShowWindow(hDlg, SW_SHOW);
            }
        }
        break;

    case IDC_BUTTON_EDIT:
        {
            HWND hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_EDITDIALOG), 
                hWnd, EditDlgProc);
            if (hDlg)
            {
                ShowWindow(hDlg, SW_SHOW);
            }
        }
        break;

    case IDC_BUTTON_RICHEDIT:
        {
            HWND hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_RICHEDITDIALOG), 
                hWnd, RichEditDlgProc);
            if (hDlg)
            {
                ShowWindow(hDlg, SW_SHOW);
            }
        }
        break;

    case IDC_BUTTON_LISTBOX:
        {
            HWND hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_LISTBOXDIALOG), 
                hWnd, ListBoxDlgProc);
            if (hDlg)
            {
                ShowWindow(hDlg, SW_SHOW);
            }
        }
        break;

    case IDC_BUTTON_SCROLLBAR:
        {
            HWND hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_SCROLLBARDIALOG), 
                hWnd, ScrollBarDlgProc);
            if (hDlg)
            {
                ShowWindow(hDlg, SW_SHOW);
            }
        }
        break;

    case IDC_BUTTON_STATIC:
        {
            HWND hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_STATICDIALOG), 
                hWnd, StaticDlgProc);
            if (hDlg)
            {
                ShowWindow(hDlg, SW_SHOW);
            }
        }
        break;

    case IDOK:
    case IDCANCEL:
        EndDialog(hWnd, 0);
        break;
    }
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
    g_hInst = hInstance;
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
}