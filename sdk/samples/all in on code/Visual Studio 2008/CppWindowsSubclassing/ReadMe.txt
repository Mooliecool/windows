=============================================================================
       WIN32 APPLICATION : CppWindowsSubclassing Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

If a control or a window does almost everything you want, but you need a few 
more features, you can change or add features to the original control by 
subclassing it. A subclass can have all the features of an existing class as 
well as any additional features you want to give it.

Two subclassing rules apply to subclassing in Win32.

1. Subclassing is allowed only within a process. An application cannot 
subclass a window or class that belongs to another process.

2. The subclassing process may not use the original window procedure address 
directly.

There are two approaches to window subclassing.

1. Subclassing Controls Prior to ComCtl32.dll version 6

The first is usable by most windows operating systems (Windows 2000, XP and 
later). You can put a control in a subclass and store user data within a 
control. You do this when you use versions of ComCtl32.dll prior to version 6 
which ships with Microsoft Windows XP. There are some disadvantages in 
creating subclasses with earlier versions of ComCtl32.dll.

	a) The window procedure can only be replaced once.
	b) It is difficult to remove a subclass after it is created.
	c) Associating private data with a window is inefficient.
	d) To call the next procedure in a subclass chain, you cannot cast the 
	old window procedure and call it, you must call it using CallWindowProc.

To make a new control it is best to start with one of the Windows common 
controls and extend it to fit a particular need. To extend a control, create 
a control and replace its existing window procedure with a new one. The new 
procedure intercepts the control's messages and either acts on them or passes 
them to the original procedure for default processing. Use the SetWindowLong 
or SetWindowLongPtr function to replace the WNDPROC of the control.

2. Subclassing Controls Using ComCtl32.dll version 6

The second is only usable with a minimum operating system of Windows XP since 
it relies on ComCtl32.dll version 6. ComCtl32.dll version 6 supplied with 
Windows XP contains four functions that make creating subclasses easier and 
eliminate the disadvantages previously discussed. The new functions 
encapsulate the management involved with multiple sets of reference data, 
therefore the developer can focus on programming features and not on managing 
subclasses. The subclassing functions are: 

	SetWindowSubclass
	GetWindowSubclass
	RemoveWindowSubclass
	DefSubclassProc


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. According to the CppWindowsDialog example, build up the main dialog  
for use in this example.

IDD_MAINDIALOG - The main dialog, having a button with the caption "Right-
click the button", which serves as the target of subclassing. 

Step2. To demonstrate subclassing Controls Prior to ComCtl32.dll version 6, 
add the buttons IDC_SUBCLASS_BN and IDC_UNSUBCLASS_BN, and the new button 
procedure with the prototype:

	LRESULT CALLBACK NewBtnProc(HWND hButton, UINT message, WPARAM wParam, 
		LPARAM lParam)

Inside NewBtnProc, customize the control's behaviors for the proper messages, 
get the old button procedure and call CallWindowProc to invoke the old 
behavior of the control.

	1) Subclass (OnSubclass)
	
	Get the handle of the control to be subclassed -> Subclass the button 
	control (SetWindowLongPtr, GWLP_WNDPROC) -> Store the original, default 
	window procedure of the button as the button control's user data (
	SetWindowLongPtr, GWLP_USERDATA)
	
	2) Unsubclass (OnUnsubclass)
	
	Get the handle of the control that was subclassed -> Retrieve the 
	previously stored original button window procedure (GetWindowLongPtr, 
	GWLP_USERDATA) -> Replace the current handler with the old one (
	SetWindowLongPtr, GWLP_WNDPROC)

Step3. To demonstrate subclassing Controls Using ComCtl32.dll version 6, add 
the buttons IDC_SAFESUBCLASS_BN and IDC_SAFEUNSUBCLASS_BN, and the new button 
procedure with the prototype:

	LRESULT CALLBACK NewSafeBtnProc(HWND hButton, UINT message, WPARAM wParam, 
		LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)

Every time a message is received by the new window procedure, a subclass ID 
and reference data are included. Inside NewSafeBtnProc, customize the 
control's behaviors for the proper messages, and call DefSubclassProc to 
invoke the default behavior of the control. Additionally, You must remove 
your window subclass before the window being subclassed is destroyed. This is 
typically done either by removing the subclass once your temporary need has 
passed, or if you are installing a permanent subclass, by inserting a call to 
RemoveWindowSubclass inside the subclass procedure itself:

	case WM_NCDESTROY:
		RemoveWindowSubclass(hButton, NewSafeBtnProc, uIdSubclass);
		return DefSubclassProc(hButton, message, wParam, lParam);

The application also needs to link to comctl32.lib (Project Properties / 
Linker / Input / Additional Dependencies), and includes the comctl32.h header 
file.

	1) Subclass (OnSafeSubclass)
	
	Get the handle of the control to be subclassed -> Subclass the button 
	control (SetWindowSubclass)
	
	2) Unsubclass (OnSafeUnsubclass)
	
	Get the handle of the control that was subclassed -> Unsubclass the 
	control (RemoveWindowSubclass)


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Subclassing Controls
http://msdn.microsoft.com/en-us/library/bb773183.aspx

Subclassing and Superclassing
http://winapi.foosyerdoos.org.uk/info/sub_superclass.php

When you subclass a window, it's the original window procedure of the window 
you subclass you have to call when you want to call the original window 
procedure
http://blogs.msdn.com/oldnewthing/archive/2009/05/07/9592397.aspx

Safer subclassing
http://blogs.msdn.com/oldnewthing/archive/2003/11/11/55653.aspx

MSDN: Safe Subclassing in Win32
http://msdn.microsoft.com/en-us/library/ms997565.aspx

Cross Process Subclassing
http://www.codeproject.com/KB/DLL/subhook.aspx


/////////////////////////////////////////////////////////////////////////////
