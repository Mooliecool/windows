'*************************** Module Header ******************************'
' Module Name:  IObjectWithSite.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' Provides simple objects with a lightweight siting mechanism (lighter than 
' IOleObject). A BHO must implement this interface.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System
Imports System.Runtime.InteropServices

Namespace NativeMethods
    <ComImport()> _
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    <Guid("FC4801A3-2BA9-11CF-A229-00AA003D7352")> _
    Public Interface IObjectWithSite
        Sub SetSite(<[In](), MarshalAs(UnmanagedType.IUnknown)> ByVal pUnkSite As Object)

        Sub GetSite( _
                   ByRef riid As Guid, _
                   <System.Runtime.InteropServices.Out(), _
                   MarshalAs(UnmanagedType.IUnknown)> ByRef ppvSite As Object)
    End Interface
End Namespace

