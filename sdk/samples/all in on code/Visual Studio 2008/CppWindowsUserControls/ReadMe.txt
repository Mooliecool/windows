=============================================================================
       WIN32 APPLICATION : CppWindowsUserControls Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

CppWindowsUserControls contains simple examples of how to create user 
controls defined in user32.dll. The controls include Buttons, Combo-boxes, 
Edits, List-boxes, RichEdit(in msftedit.dll or riched20.dll or riched32.dll), 
Scrollbars, and Statics.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. According to the CppWindowsDialog example, build up the dialogs for 
use in this example:

IDD_MAINDIALOG - The main dialog, having the buttons to show the sub-dialogs 
that demonstrate the use of every types of user-controls.

IDD_BUTTONDIALOG - Demonstrate the various types of button.

IDD_COMBOBOXDIALOG - Demonstrate the various types of comboxbox.

IDD_EDITDIALOG - Demonstrate the various types of edit control.

IDD_RICHEDITDIALOG - Demonstrate the richedit control.

IDD_LISTBOXDIALOG - Demonstrate the listbox control.

IDD_SCROLLBARDIALOG - Demonstrate the scrollbar control.

IDD_STATICDIALOG - Demonstrate the various types of static control.

Step2. In each dialog's WM_INITDIALOG event handler, create controls using 
the CreateWindowEx API. For example, 

    RECT rc = { 20, 20, 150, 30 };

    HWND hBtn = CreateWindowEx(0, L"BUTTON", L"Default Push Button", 
        BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE, 
        rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, reinterpret_cast<HMENU>(IDC_DEFPUSHBUTTON), g_hInst, 0);
    if (hBtn)
    {
        SendMessage(hBtn, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    }

The various control types are created by varying the style bits.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: About Window Classes 
http://msdn.microsoft.com/en-us/library/ms633574.aspx

Creating Windows and User Controls
http://winapi.foosyerdoos.org.uk/info/user_cntrls.php

MSDN: CreateWindowEx 
http://msdn.microsoft.com/en-us/library/ms632680.aspx

MSDN: Control Library
http://msdn.microsoft.com/en-us/library/bb773169.aspx


/////////////////////////////////////////////////////////////////////////////