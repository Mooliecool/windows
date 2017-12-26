'*************************** Module Header ******************************'
' Module Name:  IOleWindow.vb
' Project:	    VBIEExplorerBar
' Copyright (c) Microsoft Corporation.
' 
' The IOleWindow interface provides methods that allow an application to 
' obtain the handle to the various windows that participate in in-place 
' activation, and also to enter and exit context-sensitive help mode. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Runtime.InteropServices

Namespace NativeMethods
    <ComImport()>
    <Guid("00000114-0000-0000-C000-000000000046")>
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    Public Interface IOleWindow
        Sub GetWindow(<System.Runtime.InteropServices.Out()> ByRef phwnd As IntPtr)

        Sub ContextSensitiveHelp(<[In]()> ByVal fEnterMode As Boolean)

    End Interface
End Namespace
