'****************************** Module Header ******************************'
' Module Name:  Win32.vb
' Project:      VBWPFClipboardViewer
' Copyright (c) Microsoft Corporation.
' 
' The VBWPFClipboardViewer project provides the sample on how to monitor
' Windows clipboard changes in a WPF application.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
'***************************************************************************'


''' <summary>
''' This module holds the Win32 function declarations and constants needed by
''' this sample application.
''' </summary>
Friend Module Win32

    ''' <summary>
    ''' The WM_DRAWCLIPBOARD message notifies a clipboard viewer window that 
    ''' the content of the clipboard has changed. 
    ''' </summary>
    Friend Const WM_DRAWCLIPBOARD As Integer = &H308

    ''' <summary>
    ''' A clipboard viewer window receives the WM_CHANGECBCHAIN message when 
    ''' another window is removing itself from the clipboard viewer chain.
    ''' </summary>
    Friend Const WM_CHANGECBCHAIN As Integer = &H30D

    Friend Declare Function SetClipboardViewer Lib "user32.dll" (ByVal hWndNewViewer As IntPtr) As IntPtr

    Friend Declare Function ChangeClipboardChain Lib "user32.dll" (ByVal hWndRemove As IntPtr, ByVal hWndNewNext As IntPtr) As Boolean

    Friend Declare Auto Function SendMessage Lib "user32.dll" (ByVal hWnd As IntPtr, ByVal Msg As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr) As IntPtr

End Module
