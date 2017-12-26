'****************************** Module Header ******************************\
' Module Name: Default.aspx.vb
' Project:     VBASPNETPreventMultipleWindows
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to detect and prevent multiple windows or 
' tab usage in Web Applications.When user want to open this link 
' in a new window or tab, erb application will reject these requests.This 
' approach will solve many security problems like sharing sessions,
' protect dupicated login,data concurrency,etc.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'*****************************************************************************/


Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub

End Class