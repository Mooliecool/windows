'***************************** Module Header ******************************\
'* Module Name:	SilverlightClient.aspx.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* The code behind for the aspx page hosting the Silverlight client.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Partial Public Class SilverlightClient
    Inherits System.Web.UI.Page
    ' Those properties will be passed to Silverlight as initial parameters.
    Public Property IsAuthenticated As Boolean
    Public Property WelcomeMessage As String

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)
        ' Query session data to see if the user has been authenticated.
        If Session("User") IsNot Nothing Then
            Me.IsAuthenticated = True
            Me.WelcomeMessage = "Welcome: " & DirectCast(Session("User"), String) & "."
        Else
            Me.IsAuthenticated = False
            Me.WelcomeMessage = Nothing
        End If
    End Sub
End Class