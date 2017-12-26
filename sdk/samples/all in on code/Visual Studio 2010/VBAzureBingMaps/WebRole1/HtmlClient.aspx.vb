'***************************** Module Header ******************************\
'* Module Name:	HtmlClient.aspx.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* The code behind for the aspx page hosting the HTML client.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Partial Public Class HtmlClient
    Inherits System.Web.UI.Page
    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)
        ' Query session data to see if the user has been authenticated.
        ' And display sign in link or welcome message based on the information.
        If Session("User") IsNot Nothing Then
            Me.LoginLink.Visible = False
            Me.UserNameLabel.Visible = True
            Me.UserNameLabel.Text = "Welcome, " & DirectCast(Session("User"), String) & "."
        Else
            Me.LoginLink.Visible = True
            Me.UserNameLabel.Visible = False
        End If
    End Sub
End Class