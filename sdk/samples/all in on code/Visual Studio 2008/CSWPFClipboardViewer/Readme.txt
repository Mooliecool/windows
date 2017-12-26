========================================================================
               WPF Clipboard Viewer Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This Sample demonstrates how to monitor Windows clipboard changes in a WPF
application.

/////////////////////////////////////////////////////////////////////////////

In order to be notified by the system when the clipboard content changes, an
application must use the SetClipboardViewer function (user32.dll) to add its
window into the chain of clipboard viewers.

Clipboard viewer windows receive a WM_DRAWCLIPBOARD message whenever the 
content of the clipboard changes. And the WM_CHANGECBCHAIN message is sent 
to the first window in the clipboard viewer chain when a window is being 
removed from the chain.

In a WPF application, we use HwndSource class to register a Win32 window 
message handler to process the messages.

This sample also shows a workaround for a known issue in WPF 
Clipboard.GetImage() method.

/////////////////////////////////////////////////////////////////////////////
References:

Clipboard
http://msdn.microsoft.com/en-us/library/ms648709(VS.85).aspx

Clipboard Class (System.Windows)
http://msdn.microsoft.com/en-us/library/system.windows.clipboard.aspx

/////////////////////////////////////////////////////////////////////////////