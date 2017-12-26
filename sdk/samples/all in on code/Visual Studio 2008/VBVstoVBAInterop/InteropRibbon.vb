'************************************* Module Header **************************************'
' Module Name:  InteropRibbon.vb
' Project:      VBVstoVBAInterop
' Copyright (c) Microsoft Corporation.
' 
' The VBVstoVBAInterop project demonstrates how to interop with VBA project object model in 
' VSTO projects. Including how to programmatically add Macros (or VBA UDF in Excel) into an
' Office document; how to call Macros / VBA UDFs from VSTO code; and how to call VSTO code
' from VBA code. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'******************************************************************************************'

#Region "Import directives"
Imports Microsoft.Office.Tools.Ribbon
Imports System.Windows.Forms
Imports System.Diagnostics

#End Region


Public Class InteropRibbon

    ' The demo UI.
    Private interopForm As InteropForm = Nothing

    Private Sub InteropRibbon_Load(ByVal sender As System.Object, ByVal e As RibbonUIEventArgs) Handles MyBase.Load

    End Sub

    Private Sub btnShowFormVB_Click(ByVal sender As System.Object, ByVal e As Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs) Handles btnShowFormVB.Click
        If interopForm Is Nothing Then
            interopForm = New InteropForm
            interopForm.Show(NativeWindow.FromHandle(Process.GetCurrentProcess().MainWindowHandle))
        Else
            interopForm.Activate()
        End If
    End Sub
End Class
