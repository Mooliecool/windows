'*************************** Module Header ******************************'
' Module Name:  OpenImageMenuExt.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' The class OpenImageMenuExt is used to add/remove the menu in registry when this 
' assembly is registered/unregistered.
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

Imports System.IO
Imports System.Reflection
Imports System.Runtime.InteropServices
Imports Microsoft.Win32

Public Class OpenImageMenuExt
    Private Const IEMenuExtRegistryKey As String = _
        "Software\Microsoft\Internet Explorer\MenuExt"

    Public Shared Sub RegisterMenuExt()

        ' If the key exists, CreateSubKey will open it.
        Dim ieMenuExtKey As RegistryKey = _
        Registry.CurrentUser.CreateSubKey( _
                IEMenuExtRegistryKey & "\Open image in new Tab")


        ' Get the path of Resource\OpenImage.htm.
        Dim fileIofo As New FileInfo(System.Reflection.Assembly.GetExecutingAssembly().Location)
        Dim path As String = fileIofo.Directory.FullName & "\Resource\OpenImage.htm"

        ' Set the default value of the key to the path.
        ieMenuExtKey.SetValue(String.Empty, path)

        ' Set the value of Name.
        ieMenuExtKey.SetValue("Name", "Open_Image")

        ' Set the value of Contexts to indicate which contexts your entry should
        ' appear in the standard context menu by using a bit mask consisting of
        ' the logical OR of the following values:
        ' Default 0x1 
        ' Images 0x2 
        ' Controls 0x4 
        ' Tables 0x8 
        ' Text selection 0x10 
        ' Anchor 0x20 
        ieMenuExtKey.SetValue("Contexts", &H2)

        ieMenuExtKey.Close()
    End Sub

    Public Shared Sub UnRegisterMenuExt()
        Dim ieMenuExtskey As RegistryKey = _
            Registry.CurrentUser.OpenSubKey(IEMenuExtRegistryKey, True)

        If ieMenuExtskey IsNot Nothing Then
            ieMenuExtskey.DeleteSubKey("Open image in new Tab", False)
        End If

        ieMenuExtskey.Close()
    End Sub
End Class
