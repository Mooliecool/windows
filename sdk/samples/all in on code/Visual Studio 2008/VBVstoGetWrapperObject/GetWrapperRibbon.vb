'/************************************* Module Header **************************************\
'* Module Name:	GetWrapperRibbon.vb
'* Project:		VBVstoGetWrapperObject
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBVstoGetWrapperObject project demonstrates how to get a VSTO wrapper
'* object from an existing Office COM object.
'*
'* This feature requires Visual Studio Tools for Office 3.0 SP1 (included in 
'* Visual Studio 2008 SP1) for both design-time and runtime support.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 10/30/2009 3:00 PM Tim Li Created
'\******************************************************************************************/


#Region "Import directives"
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
Imports System.Diagnostics
Imports Microsoft.Office.Tools.Ribbon
#End Region

Public Class GetWrapperRibbon
    Private getWrapperForm As GetWrapperForm = Nothing

    Private Sub GetWrapperRibbon_Load(ByVal sender As System.Object, ByVal e As RibbonUIEventArgs) Handles MyBase.Load

    End Sub

    Private Sub btnShowGetWrapperVB_Click(ByVal sender As Object, ByVal e As RibbonControlEventArgs) Handles btnShowGetWrapperVB.Click
        If getWrapperForm Is Nothing OrElse getWrapperForm.IsDisposed Then
            getWrapperForm = New GetWrapperForm()
            getWrapperForm.Show(NativeWindow.FromHandle(Process.GetCurrentProcess().MainWindowHandle))
        Else
            getWrapperForm.Activate()
        End If
    End Sub
End Class
