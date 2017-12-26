'***************************** Module Header ******************************\
'Module Name:  CWin32WindowWrapper.vb
'Project:      VBOneNoteRibbonAddIn
'Copyright (c) Microsoft Corporation.
'
'wrapper Win32 HWND handles
'
'This source is subject to the Microsoft Public License.
'See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'All other rights reserved.
'
'THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Public Class CWin32WindowWrapper
    Implements IWin32Window
    ' piont handle field
    Private _windowHandle As IntPtr

    ''' <summary>
    '''     CWin32WindowWrapper construct method  
    ''' </summary>
    ''' <param name="windowHandle">window handle</param>
    Public Sub New(ByVal windowHandle As IntPtr)
        Me._windowHandle = windowHandle
    End Sub

    ' Summary:
    '     Gets the handle to the window represented by the implementer.
    '
    ' Returns:
    '     A handle to the window represented by the implementer.
    Public ReadOnly Property Handle() As IntPtr Implements IWin32Window.Handle
        Get
            Return Me._windowHandle
        End Get
    End Property
End Class
